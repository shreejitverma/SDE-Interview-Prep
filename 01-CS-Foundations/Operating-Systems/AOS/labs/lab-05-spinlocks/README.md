---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed: 2026-10-01
sources: []
course: cs6210
lessons: [L04b]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-05-spinlocks: Spinlock zoo: TAS, TTAS, backoff, ticket, Anderson, and MCS in C11

> [!info] Goal
> Make L04b concrete with real commands and measurements. 
> Demonstrate various spinlock implementations and how they scale with thread count on a multicore processor.

See [setup](../setup/README.md) for the VM.

## Prerequisites

- Understanding of atomic instructions (Test-and-Set, Fetch-and-Add, Compare-and-Swap).
- Familiarity with false sharing and cache line bouncing.
- Basic C11 threads and stdatomic.h.

## Run Commands

```sh
make test      # Compiles and runs correctness checks for all spinlocks
make run       # Runs the scaling benchmark across 1 to 8 threads
```

Inside the VM, use the provided wrapper:
```sh
labs/setup/run-in-vm.sh labs/lab-05-spinlocks test
labs/setup/run-in-vm.sh labs/lab-05-spinlocks capture
```

## What you should see

The output captures the execution time for 1 to 8 threads attempting to acquire and release the lock one million times each.
From `expected-output.txt`:
- Uncontended performance (1 thread): Simple TAS and TTAS locks are generally fastest because they have the lowest instruction overhead.
- Contended performance (8 threads): MCS and Anderson array locks significantly outperform the simpler locks. As thread count increases, TAS and TTAS experience heavy cache invalidation traffic (cache line bouncing), causing execution time to spike.
- Exponential backoff improves TTAS under contention but remains non-deterministic.
- Ticket locks provide fairness (FIFO) but still suffer from a single shared `now_serving` variable that invalidates the cache for all waiters upon release.
- Array locks and MCS locks spin on locally cached variables (or separate cache lines), achieving much better scaling.

## How it works

The lab implements six classic spinlocks in C11 using `stdatomic.h`:
1. **TAS (Test-And-Set):** Repeatedly executes an atomic exchange to acquire the lock, thrashing the cache.
2. **TTAS (Test-and-Test-and-Set):** Spins reading a normal variable and only attempts the atomic exchange when the lock appears free. Still suffers a thundering herd when the lock is released.
3. **Exponential Backoff:** An improvement over TTAS where threads wait for a randomized, exponentially increasing delay after a failed atomic exchange, reducing bus traffic.
4. **Ticket Lock:** Uses an atomic `fetch_add` to get a ticket and waits until `now_serving` matches the ticket. Provides FIFO fairness.
5. **Anderson Array Lock:** Each thread is given a slot in a cache-line-padded array and spins on its own flag, eliminating false sharing and reducing bus traffic.
6. **MCS Lock:** A queue-based lock where each thread allocates its own node on the stack and links it into a list via atomic swap. Threads spin on their own node's flag, providing excellent scaling and fairness.

For benchmarking, `clock_gettime(CLOCK_MONOTONIC)` measures the elapsed time for thread creation, execution, and joining.

## Experiments to try

1. **Vary the critical section length:** What happens to the relative performance of TTAS vs. Backoff if you add a short delay inside the critical section?
   > Prediction: Backoff will perform significantly better than TTAS for longer critical sections, as fewer retries will hit the bus while another thread is holding the lock.
2. **Change the Anderson lock padding:** Remove the `CACHE_LINE_SIZE` padding in `anderson_flag_t`. How does it affect performance?
   > Prediction: Performance will degrade due to false sharing. When one thread releases the lock, the cache line containing adjacent flags is invalidated, forcing waiting threads to fetch the cache line again from main memory.
3. **Trace cache misses:** Use `perf stat -e cache-misses ./spinlocks benchmark` outside the VM (on bare-metal Linux) to observe hardware events.
   > Prediction: TAS and TTAS will show orders of magnitude more cache misses under high thread counts compared to Anderson or MCS locks.
4. **Compare NUMA behavior:** If you have access to a multi-socket machine, pin threads to different sockets. How does MCS compare to Ticket?
   > Prediction: MCS will heavily outperform Ticket across NUMA nodes, as the Ticket lock causes cross-socket invalidations of the `now_serving` variable on every release.

## Questions

<details>
<summary>Why does TTAS perform worse than TAS on some architectures under high contention?</summary>

When the lock is released, all waiting TTAS threads exit their inner read-loop simultaneously and attempt the atomic exchange (the thundering herd problem). This burst of atomic operations can saturate the memory bus more severely than a continuous stream of TAS operations, depending on the cache coherence protocol's backoff and arbitration mechanisms.
</details>

<details>
<summary>Why do we use `cpu_relax()` in the spin loops?</summary>

`cpu_relax()` (which emits `pause` on x86 or `yield` on ARM) hints to the processor that the thread is in a spin-wait loop. It prevents the processor's pipeline from being flooded with speculative instructions that will eventually be discarded, saving power and allowing hyper-threads on the same core to utilize execution resources.
</details>

<details>
<summary>In the MCS lock, why do we need an atomic compare-and-exchange during release if `next` is NULL?</summary>

When a thread sees `next == NULL`, it means it is currently the last node in the queue. However, another thread might be in the middle of `mcs_acquire()` and is about to link its node. The atomic compare-and-exchange on `lock->tail` ensures that we only clear the tail if no new thread has arrived. If the CAS fails, we must wait until the new thread finishes linking its node before we can signal it.
</details>

<details>
<summary>What is a disadvantage of the Anderson Array Lock compared to MCS?</summary>

The Anderson lock requires pre-allocating an array of size `max_threads`, where each element must be padded to a full cache line (e.g., 64 bytes) to avoid false sharing. This consumes `max_threads * 64` bytes of memory and requires knowing the maximum number of threads in advance. MCS only requires a small node structure allocated locally (often on the stack) by each participating thread, which is more memory efficient and dynamic.
</details>
