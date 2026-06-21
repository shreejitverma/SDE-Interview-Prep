# Chapter 96: Threading Discipline — Affinity, Core Isolation, and Thread-Per-Core

The operating system scheduler is optimised for *fairness and throughput across many processes* — which is exactly the wrong objective for a latency-critical thread that wants a core to itself, its caches warm, and no involuntary interruptions. This chapter is about taking control away from the scheduler: pinning threads to cores, isolating those cores from the kernel and other work, deciding when to busy-spin instead of block, and adopting the thread-per-core architecture that eliminates shared-state contention by construction. These are the disciplines that turn the techniques of the preceding chapters into deterministic, low-tail-latency systems.

## Chapter Roadmap

- 96.1 Why the Scheduler Is Your Adversary
- 96.2 Thread Affinity and Pinning
- 96.3 Core Isolation
- 96.4 Busy-Spin vs Blocking
- 96.5 SMT, NUMA, and IRQ Placement
- 96.6 The Thread-Per-Core Architecture
- 96.7 The Discipline and Its Costs

---

## 96.1 Why the Scheduler Is Your Adversary

The OS scheduler time-slices many threads onto the available cores, migrating threads between cores to balance load and preempting them when their slice expires or a higher-priority thread wakes. For a server running hundreds of processes, this is exactly right. For a single latency-critical thread it is a source of jitter:

- **Migration** moves a thread to a different core, leaving its hot data in the *old* core's L1/L2 — every subsequent access is a cache miss until the working set is re-pulled (Chapter 87).
- **Preemption** deschedules the thread for the OS to run something else — a multi-microsecond-to-millisecond gap in the middle of your critical path.
- **Involuntary context switches** flush the pipeline, pollute caches and the TLB, and cost ~1–5 μs of direct overhead plus the indirect cache-refill cost.

> **Why this matters.** Every one of these is a *tail-latency event*: the thread runs fine 99.9% of the time, then a migration or preemption injects a spike that blows the latency budget. The whole of threading discipline is *removing the scheduler's discretion* over your hot thread — pinning it so it never migrates, isolating its core so nothing else runs there, and (where appropriate) busy-spinning so it never voluntarily yields. You are trading the scheduler's average-case efficiency for worst-case determinism, which for these systems is the correct trade.

---

## 96.2 Thread Affinity and Pinning

**CPU affinity** restricts a thread to run on a specified set of cores. **Pinning** sets that set to a single core, so the thread never migrates.

```cpp
// Min standard: C++11 + POSIX/Linux (non-portable). Pin the calling thread to one core.
#include <pthread.h>
#include <sched.h>
void pin_to_core(int core) {
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(core, &set);
    pthread_setaffinity_np(pthread_self(), sizeof(set), &set);  // Linux/glibc
}
// Or externally: taskset -c 2 ./app   ;   or numactl --physcpubind=2 ./app
```
*Listing 96.1 — Pinning a thread to a core. `pthread_setaffinity_np` and `cpu_set_t` are Linux-specific.*

> **Why this matters / cost model.** Pinning eliminates migration, so the thread's working set stays resident in *one* core's L1/L2/TLB — the cache-warmth assumption that makes everything else fast. It also lets you place a thread on a core local to the NUMA node holding its memory (Chapter 88) and away from cores handling interrupts (§96.5). The cost is loss of automatic load balancing: if you pin two busy threads to the same core, they fight; if you pin to a core that also handles OS work, you inherit its jitter. Pinning is necessary but not sufficient — the core must also be *isolated* so nothing else lands on it.

---

## 96.3 Core Isolation

Pinning keeps *your* thread on a core; **core isolation** keeps *everything else off* it. On Linux, `isolcpus` (boot parameter) removes cores from the general scheduler's balancing, `nohz_full` stops the periodic scheduler timer tick on them, and `rcu_nocbs` offloads RCU callback processing elsewhere — so an isolated core runs essentially nothing but your pinned thread.

```bash
# Linux kernel boot parameters (non-portable, system-wide). Reserve cores 2-3:
# isolcpus=2,3 nohz_full=2,3 rcu_nocbs=2,3
# Then pin the hot thread to core 2 and leave OS/other work on cores 0-1.
```
*Listing 96.2 — Isolating cores at boot so a pinned thread owns the core. Linux-specific, requires reboot.*

> **Why this matters / cost model.** A merely *pinned* thread still suffers the scheduler timer tick (the periodic interrupt that can preempt it ~100–1000 times/sec), kernel housekeeping, and any other thread the scheduler places there. Isolation removes all of it: `nohz_full` eliminates the tick on a core running a single thread, and `isolcpus` stops the load balancer from scheduling anything there. The result is a core that runs your thread *uninterrupted*, approaching bare-metal determinism. The cost is stark: those cores are *gone* from the general pool (you have fewer cores for everything else), and the configuration is system-level (boot params, often coordinated with `irqaffinity` and `cpuset`). This is the configuration HFT and real-time systems run — a few isolated cores for hot threads, the rest for the OS and cold work.

---

## 96.4 Busy-Spin vs Blocking

A thread waiting for work can either **block** (sleep until signalled, via a futex/condition variable) or **busy-spin** (loop, polling for work without yielding the core).

| Strategy | CPU while waiting | Wake latency | Best for |
|---|---|---|---|
| Block (futex/condvar) | ~0% (sleeps) | ~1–10 μs (syscall + wakeup + reschedule) | Most code; oversubscribed cores |
| Busy-spin | 100% (one core) | ~tens of ns (no syscall) | Latency-critical, dedicated isolated core |

```cpp
// Min standard: C++11. Busy-spin consumer on a dedicated, isolated core.
while (running) {
    if (queue.pop(msg)) {        // SPSC ring (Chapter 77): no lock, no syscall
        process(msg);            // react in tens of ns of the producer publishing
    } else {
        cpu_relax();             // PAUSE hint; do NOT yield/sleep on a dedicated core
    }
}
```
*Listing 96.3 — A busy-spin loop on an isolated core: lowest wake latency at the cost of a fully-consumed core.*

> **Why this matters / cost model.** Blocking is the correct default — it frees the core for other work and saves power. But waking a blocked thread costs a syscall, a scheduler decision, and a context switch (~microseconds), which is *fatal* when the goal is to react within nanoseconds of a market tick or packet arrival. Busy-spinning trades an entire core (100% utilisation, heat, power) for the lowest possible wake latency: the thread is already running, so it reacts in the time it takes to observe the new value (an acquire load on a cache line the producer just wrote). The trade is only sane on a *dedicated, isolated* core — busy-spinning on a shared core starves everything else. This is why §96.3's isolation and §96.4's busy-spin go together: the spin loop is what makes the dedicated core worth dedicating.

---

## 96.5 SMT, NUMA, and IRQ Placement

Three more placement concerns complete the picture:

- **SMT / hyperthreading.** Two logical threads share one physical core's execution units, L1, and L2. A latency-critical thread sharing a physical core with a busy sibling competes for those resources unpredictably — so latency-sensitive shops often *disable SMT* or leave the sibling idle. The OS numbers SMT siblings in a non-obvious way; verify the topology (`lscpu`, `/sys`).
- **NUMA locality.** Pin the thread to a core on the same NUMA node as its memory, and first-touch its memory from that thread (Chapter 88), so accesses stay local.
- **IRQ affinity.** Device interrupts (network cards, timers) preempt whatever runs on their assigned core. Steer IRQs *away* from isolated hot cores (`/proc/irq/*/smp_affinity`) so a packet arriving for an unrelated flow does not interrupt your hot thread.

> **Why this matters.** These are the leaks that survive pinning and isolation. An un-steered NIC interrupt firing on your hot core injects a multi-microsecond stall exactly when you least want it; a hyperthread sibling running a GC thread steals your L1; a NUMA-remote allocation doubles your miss latency. The complete recipe is: isolate the core, pin the thread, disable/idle the SMT sibling, place memory NUMA-local, and steer interrupts elsewhere. Each step closes one source of jitter; together they approach the determinism of a dedicated machine.

---

## 96.6 The Thread-Per-Core Architecture

The architectural culmination is **thread-per-core**: instead of a pool of threads sharing data structures behind locks, run *one* thread per core, each owning a *shard* of the data exclusively, communicating with peers only through explicit message-passing (SPSC rings). No shared mutable state means no locks, no atomics on the data path, and no cache-line contention.

```cpp
// Min standard: C++11. Thread-per-core skeleton.
// For each isolated core c:
//   - pin a worker thread to c
//   - give it an exclusive shard of state (e.g. orders for symbols hashed to c)
//   - inbound work is routed to the owning core's SPSC ring
//   - the worker busy-spins on its ring, processes its shard with NO locks
// Cross-shard work is an explicit message to the owning core, never a shared write.
```
*Listing 96.4 — Thread-per-core: exclusive ownership replaces locking.*

> **Why this matters / cost model.** Thread-per-core is the synthesis of the entire concurrency block: it makes the data race *impossible by construction* (like the actor model, Chapter 78), eliminates lock contention and false sharing (each core writes only its own lines, Chapter 87), keeps caches warm (no migration), and uses the cheapest possible communication (SPSC rings, Chapter 77). It is the architecture behind Seastar, ScyllaDB, Redis's threading model, and most HFT engines. The cost: it requires the problem to *shard* cleanly (work must partition by some key), cross-shard operations become explicit messages with their own latency, and load imbalance between shards cannot be smoothed by a shared queue. When the workload shards well, thread-per-core scales linearly with cores in a way locked shared-state designs never can.

---

## 96.7 The Discipline and Its Costs

| Technique | Removes | Cost |
|---|---|---|
| Pinning | Migration, cold-cache restarts | Manual load placement |
| Core isolation | Scheduler tick, foreign threads | Cores removed from general pool |
| Busy-spin | Wake-up syscall latency | A fully-consumed core, power/heat |
| SMT disable/idle | Sibling resource contention | Half the logical cores |
| NUMA-local + IRQ steering | Remote-memory and interrupt jitter | System-level configuration |
| Thread-per-core | Locks, false sharing, races | Requires shardable workload |

> **The discipline.** Threading discipline is the deliberate seizure of control from the OS: you decide where each hot thread runs, guarantee it owns its core, keep its caches and memory local, and let it busy-spin so it never sleeps through an event. Applied fully, it converts the scheduler from an unpredictable adversary into a non-participant on your critical cores. The costs are real — dedicated cores, disabled SMT, system-level tuning, a shardable design — and they are *only* justified when the tail latency genuinely demands them; on an ordinary service these techniques waste resources for no benefit. But for the systems this volume targets, thread-per-core on isolated, pinned, busy-spinning cores is the architecture that makes nanosecond-scale determinism achievable. The volume now descends to the memory and OS boundary that these threads must also master.
