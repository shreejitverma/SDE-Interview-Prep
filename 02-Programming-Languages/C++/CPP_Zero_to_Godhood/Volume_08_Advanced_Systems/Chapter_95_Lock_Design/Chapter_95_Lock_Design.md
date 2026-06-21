# Chapter 95: Lock Design — Spinlocks, MCS Locks, Futexes, and Contention

"Use a mutex" is the right default and a non-answer at the systems level, because a *mutex* hides a design space of radically different cost and fairness characteristics. A spinlock burns CPU but never sleeps; a futex sleeps but pays a syscall; a ticket lock is fair but bounces cache lines; an MCS lock scales but needs a per-thread node. This chapter builds locks from the atomic up, exposes the cache-coherence cost that makes naive locks collapse under contention, and gives the cost model for choosing — including the recurring conclusion that the best lock is often *less sharing*.

## Chapter Roadmap

- 95.1 What a Lock Must Do, and the Cost of Contention
- 95.2 The Naive Spinlock and Why It Collapses
- 95.3 Test-and-Test-and-Set with Back-off
- 95.4 Ticket Locks: Fairness
- 95.5 MCS Locks: Scalable Queueing
- 95.6 Futexes: Sleeping Without a Syscall (Usually)
- 95.7 Adaptive and Reader-Writer Locks
- 95.8 Choosing a Lock — and Choosing Not To

---

## 95.1 What a Lock Must Do, and the Cost of Contention

A lock provides **mutual exclusion**: at most one thread holds it at a time. The design questions are what a waiter *does* (spin or sleep), whether waiters are served **fairly** (FIFO) or arbitrarily, and how the lock behaves as the number of contenders grows. The dominant cost under contention is not the lock logic but **cache coherence** (Chapter 87): the lock variable is a shared cache line, and every contender that writes it forces a coherence round-trip.

> **Why this matters / cost model.** An *uncontended* lock is cheap — one atomic RMW, the line already in the holder's cache. A *contended* lock is dominated by the cache line bouncing between cores: each failed acquisition attempt that *writes* the line invalidates every other waiter's copy, so N spinners can generate O(N) coherence traffic per release. This is why lock design is largely the art of *minimising writes to the shared line under contention* — spinning on a read-only copy, queueing so each waiter spins on its *own* line, or sleeping so waiters generate no traffic at all. Fairness and the spin-vs-sleep decision are secondary axes layered on top.

---

## 95.2 The Naive Spinlock and Why It Collapses

The simplest lock spins on an atomic flag with `exchange` until it wins:

```cpp
// Min standard: C++11. Portable. CORRECT but scales badly under contention.
#include <atomic>
class SpinLock {
    std::atomic<bool> locked_{false};
public:
    void lock() {
        while (locked_.exchange(true, std::memory_order_acquire)) {  // WRITES the line every spin
            // spin
        }
    }
    void unlock() { locked_.store(false, std::memory_order_release); }
};
```
*Listing 95.1 — A test-and-set spinlock. Correct, but every spinning thread writes the shared line each iteration.*

> **Why this matters.** This lock is correct and fine when *uncontended*, but it collapses under contention for a precise reason: `exchange` is a write, so every spinning thread, on every iteration, attempts to write `locked_` — invalidating every other core's copy and the holder's too. The line ping-pongs furiously, the *holder* itself is slowed trying to release (its store contends with the spinners' reads-for-ownership), and throughput drops as cores are added. It also burns 100% CPU spinning and is unfair (whoever's `exchange` happens to win, wins). This is the canonical "do not ship a naive spinlock" lesson.

---

## 95.3 Test-and-Test-and-Set with Back-off

The first fix: spin on a *read* (which can be satisfied from the local cached copy) and only attempt the *write* when the lock looks free. Add exponential back-off to reduce the thundering herd at release.

```cpp
// Min standard: C++11. Portable. Test-and-test-and-set with back-off.
#include <atomic>
#include <thread>
class TTASLock {
    std::atomic<bool> locked_{false};
public:
    void lock() {
        for (;;) {
            while (locked_.load(std::memory_order_relaxed)) {   // spin on a READ — local cache, no traffic
                cpu_relax();                                    // PAUSE / YIELD hint
            }
            if (!locked_.exchange(true, std::memory_order_acquire))  // only WRITE when it looks free
                return;
            // lost the race; back off and retry
        }
    }
    void unlock() { locked_.store(false, std::memory_order_release); }
    static void cpu_relax() {
    #if defined(__x86_64__)
        __builtin_ia32_pause();   // x86 PAUSE — non-portable; reduces spin power & pipeline waste
    #else
        std::this_thread::yield();
    #endif
    }
};
```
*Listing 95.2 — Test-and-test-and-set. Spinning on a read keeps the line Shared; the write happens only when contention drops. `PAUSE` is x86-specific.*

> **Why this matters / cost model.** The read-spin keeps the cache line in **Shared** state across all waiters — reads do not invalidate, so there is *no* coherence traffic while spinning. Traffic occurs only at release (one invalidation) and the brief write-attempt race. The `PAUSE`/`cpu_relax` hint tells the CPU this is a spin loop, reducing power and freeing pipeline resources (and on SMT, the sibling hyperthread). Back-off further thins the herd. This is dramatically better than Listing 95.1 — but it is still *unfair* (no ordering among waiters) and still bounces the line once per release among all waiters, so it does not truly *scale*: that needs queueing.

---

## 95.4 Ticket Locks: Fairness

A **ticket lock** enforces FIFO fairness like a deli counter: each arriving thread atomically takes the next ticket; the holder serves tickets in order by advancing a `serving` counter on release.

```cpp
// Min standard: C++11. Portable. FIFO-fair.
#include <atomic>
class TicketLock {
    std::atomic<unsigned> next_{0};     // next ticket to hand out
    std::atomic<unsigned> serving_{0};  // ticket currently being served
public:
    void lock() {
        unsigned my = next_.fetch_add(1, std::memory_order_relaxed);   // take a ticket
        while (serving_.load(std::memory_order_acquire) != my)          // wait my turn
            TTASLock::cpu_relax();
    }
    void unlock() { serving_.fetch_add(1, std::memory_order_release); } // serve next
};
```
*Listing 95.3 — A ticket lock: fair, simple, but all waiters spin on the same `serving_` line.*

> **Why this matters / cost model.** Fairness matters: an unfair lock can *starve* a thread indefinitely under contention, and starvation is a tail-latency disaster. The ticket lock guarantees FIFO service in two atomic instructions. Its weakness is that *all* waiters spin on the single `serving_` variable, so every release invalidates *every* waiter's cached copy (each must re-read to check if it is their turn) — O(N) coherence traffic per release, the same scaling wall as TTAS. It is fair but not scalable. To get both fairness *and* scalability, each waiter must spin on a *different* line — the MCS lock.

---

## 95.5 MCS Locks: Scalable Queueing

The **MCS lock** (Mellor-Crummey & Scott) builds an explicit queue of waiters, each spinning on *its own* node's flag. A waiter appends its node to the queue; the previous tail points its `next` at the newcomer; on release the holder flips the flag of *only* its successor's node.

```cpp
// Min standard: C++11. Portable. Each waiter spins on its OWN node -> O(1) traffic per handoff.
#include <atomic>
struct McsNode {
    std::atomic<McsNode*> next{nullptr};
    std::atomic<bool> locked{true};     // this waiter spins on ITS OWN flag
};
class McsLock {
    std::atomic<McsNode*> tail_{nullptr};
public:
    void lock(McsNode* self) {
        self->next.store(nullptr, std::memory_order_relaxed);
        self->locked.store(true, std::memory_order_relaxed);
        McsNode* pred = tail_.exchange(self, std::memory_order_acq_rel);  // enqueue
        if (pred) {                                                       // someone ahead of us
            pred->next.store(self, std::memory_order_release);
            while (self->locked.load(std::memory_order_acquire))          // spin on OUR line
                ; // cpu_relax
        }
    }
    void unlock(McsNode* self) {
        McsNode* succ = self->next.load(std::memory_order_acquire);
        if (!succ) {                                                      // no known successor yet
            McsNode* expected = self;
            if (tail_.compare_exchange_strong(expected, nullptr,
                    std::memory_order_release, std::memory_order_relaxed))
                return;                                                   // queue empty; done
            while (!(succ = self->next.load(std::memory_order_acquire)))  // wait for successor to link
                ; // cpu_relax
        }
        succ->locked.store(false, std::memory_order_release);            // wake ONLY the successor
    }
};
```
*Listing 95.4 — The MCS lock. Each waiter spins on its own node, so a release touches exactly one other line.*

> **Why this matters / cost model.** MCS is the gold standard for scalable, fair spinlocks. Because each waiter spins on a **private** node, a release writes exactly *one* other cache line (the successor's), giving **O(1) coherence traffic per handoff** regardless of contender count — it scales to dozens of cores where ticket and TTAS locks collapse. It is also FIFO-fair. The cost: each acquirer must supply a `McsNode` (typically on its stack), the protocol is more complex, and uncontended acquisition is slightly heavier than a simple spinlock. The Linux kernel's queued spinlocks and `std::atomic`-based scalable locks use MCS-family designs. This is the answer when a spinlock must scale.

---

## 95.6 Futexes: Sleeping Without a Syscall (Usually)

All the above *spin*, burning CPU while waiting — fine for sub-microsecond critical sections, wasteful for longer ones. A **futex** (fast userspace mutex, Linux) lets a lock take the fast path entirely in user space (one atomic, no syscall) when uncontended, and fall into the kernel to *sleep* only when it must actually block.

```cpp
// Min standard: C++11. Linux futex (non-portable). Conceptual fast/slow path.
// lock():   if (atomic CAS 0->1 succeeds) return;          // FAST PATH: no syscall
//           else futex_wait(&val, 1);                       // SLOW PATH: sleep in kernel
// unlock(): atomic store 0;
//           if (there were waiters) futex_wake(&val, 1);    // syscall only if needed
```
*Listing 95.5 — Futex fast/slow path. `std::mutex` on Linux is built on this. Linux-specific.*

> **Why this matters / cost model.** The futex insight is that the common case — uncontended lock/unlock — should never enter the kernel: it is a single atomic CAS in user space. Only on actual contention does a thread make the `futex_wait` syscall to sleep (a few hundred ns to microseconds plus a context switch) and the releaser make `futex_wake`. This is what `std::mutex`, `std::condition_variable`, and most modern locks are built on. The trade-off versus spinning: a futex *sleeps* (zero CPU while blocked, good for long or rare critical sections and when threads outnumber cores) but pays a syscall + context switch + scheduler latency on the blocking path — catastrophic for a sub-microsecond critical section, where a spinlock's busy-wait is far cheaper. Spin for short and rare-contention; sleep (futex) for long or heavily-oversubscribed.

---

## 95.7 Adaptive and Reader-Writer Locks

- **Adaptive mutexes** combine both: spin briefly (betting the holder releases soon) and fall back to a futex sleep if spinning exceeds a threshold. This captures the spinlock's cheapness for short holds and the futex's efficiency for long ones, and is what high-quality `std::mutex` implementations actually do.
- **Reader-writer locks** (`std::shared_mutex`) allow many concurrent readers *or* one writer, exploiting read-mostly workloads. But the shared reader count is itself a contended atomic — under heavy read traffic the rwlock's own counter becomes the bottleneck (every reader writes it), which is precisely the workload where RCU (Chapter 94) crushes it by making reads truly free.

> **Why this matters.** The adaptive mutex is why "just use `std::mutex`" is good advice for most code: a modern implementation already spins-then-sleeps optimally for the uncontended and lightly-contended cases. Reader-writer locks are tempting for read-heavy data but frequently disappoint — the reader-count cache line bounces among readers, so a `shared_mutex` can be *slower* than a plain mutex when reads are short and frequent. Reach for an rwlock only when read critical sections are *long* (so the concurrency win exceeds the counter contention), and consider RCU/seqlocks for read-mostly short sections instead.

---

## 95.8 Choosing a Lock — and Choosing Not To

| Lock | Waiter behaviour | Fairness | Scales? | Best for |
|---|---|---|---|---|
| Naive spinlock (TAS) | Spin (writes line) | No | No | Never ship; teaching only |
| TTAS + back-off | Spin (reads line) | No | Poorly | Short critical sections, low contention |
| Ticket lock | Spin (shared line) | FIFO | No | Short sections needing fairness |
| MCS lock | Spin (private line) | FIFO | **Yes** | Short sections, high core count |
| Futex / `std::mutex` | Sleep (fast path userspace) | Impl-defined | Yes | General; long or rare-contention sections |
| Adaptive mutex | Spin then sleep | Impl-defined | Yes | The sensible default |
| `shared_mutex` (rwlock) | Reader/writer | Impl-defined | Read-limited | Long read sections |

> **The discipline.** Lock choice follows the critical-section length and contention level: sub-microsecond and lightly contended → spin (TTAS/MCS); longer or oversubscribed → sleep (futex/`std::mutex`); the safe default is an adaptive `std::mutex`. But the deepest lesson of this chapter is the one Chapter 77 opened with: *the best lock is often no lock.* Before tuning a lock, ask whether the sharing can be removed — partition the data per thread (thread-per-core, Chapter 96), use an SPSC ring (Chapter 77), or make reads free with RCU (Chapter 94). A contended lock, however well-designed, serialises your program; eliminating the contention scales it. Next, the architecture that eliminates sharing at the source: threading discipline.
