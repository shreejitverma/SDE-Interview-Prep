# Chapter 77: Lock-Free Programming

Lock-free programming removes the mutex from the critical path so that the *system as a whole* always makes progress, even when a thread is preempted mid-operation. The problem it solves is tail latency and convoying: a thread that holds a lock and is then descheduled stalls every other thread waiting on it, producing latency spikes that are fatal in trading, audio, and kernel paths. This chapter builds lock-free reasoning from the compare-and-swap primitive up through real stacks, queues, and ring buffers — and is candid about the steep correctness cost, deferring the deepest treatment of CAS/ABA to Chapter 93 and of safe reclamation to Chapter 94.

## Chapter Roadmap

- 77.1 Progress Guarantees: Lock-Free, Wait-Free, Obstruction-Free
- 77.2 Why Lock-Free: The Cost Model vs Mutexes
- 77.3 Compare-And-Swap: The Primitive
- 77.4 The ABA Problem
- 77.5 The Lock-Free Stack (Treiber)
- 77.6 The Lock-Free Queue (Michael–Scott)
- 77.7 The SPSC Ring Buffer
- 77.8 The Reclamation Problem
- 77.9 When *Not* to Go Lock-Free

---

## 77.1 Progress Guarantees: Lock-Free, Wait-Free, Obstruction-Free

Lock-free programming is best understood through a coffee-shop analogy. Imagine a busy shop with many customers (threads) and one shared counter (the data):

- **Mutex (the locked door).** To interact with the barista you lock the front door; no one else may even enter until you are done. Safe, but if your order takes ten minutes, a queue forms outside. Worse, if you fall asleep at the counter (preemption), *everyone* is blocked indefinitely.
- **Lock-free (the ticket system).** Everyone is inside at once. The counter has a "current ticket" number; you attempt to swap your ticket for coffee in one instant motion. If someone beat you to it, your ticket is stale — you go to the back and retry. No one is ever blocked by a sleeping customer.

Formally, the progress hierarchy is:

| Guarantee | Definition | Implication |
|---|---|---|
| **Obstruction-free** | A thread makes progress if it runs in isolation | Weakest; livelock possible under contention |
| **Lock-free** | At least one thread always makes progress system-wide | No deadlock; individual threads may starve |
| **Wait-free** | *Every* thread completes in a bounded number of steps | Strongest; bounded latency per thread; hardest to build |

> **Why this matters.** "Lock-free" does not mean "fast" and does not mean "every thread is guaranteed to finish." It means the system cannot deadlock and cannot be stalled by a single preempted thread. Wait-free is what hard-real-time and the tightest HFT paths actually want, because only it bounds *per-thread* latency — but wait-free structures are dramatically harder to design and often slower in the common case. Most production "lock-free" code is lock-free, not wait-free.

---

## 77.2 Why Lock-Free: The Cost Model vs Mutexes

A mutex is not slow in the uncontended case — an uncontended `std::mutex` lock/unlock is a handful of atomic operations. The problem is what happens under contention and preemption:

- **Convoying.** If the lock holder is preempted by the scheduler, every waiter blocks until it is rescheduled — a latency spike of milliseconds, not nanoseconds.
- **Priority inversion.** A low-priority thread holding a lock blocks a high-priority thread.
- **Syscall on contention.** A contended `std::mutex` typically parks the waiter via a futex syscall (hundreds of ns to microseconds) and incurs a context switch.

> **Why this matters / cost model.** Lock-free code trades these worst-case stalls for *more work in the common case*: a CAS-retry loop may spin several times under contention, and every operation pays the cost of atomic RMW (a `lock`-prefixed instruction on x86, ~20–40 cycles, that also acts as a full barrier). The win is bounded *tail* latency and immunity to preemption, not better average throughput — under low contention a mutex is often faster and always simpler. Choose lock-free when the tail matters and contention is real; otherwise a good lock (or no sharing at all) wins.

---

## 77.3 Compare-And-Swap: The Primitive

The heart of lock-free programming is **compare-and-swap (CAS)** — an atomic "honest exchange." Think of showing the barista a photo of the counter as it looked a moment ago (the *expected* value): "if the counter still looks exactly like this photo, place this coffee on it (the *new* value)." The barista checks; if it matches, the swap happens atomically; if not — someone moved a cup — the transaction is denied and you are handed a fresh photo (the current value).

In C++ this is `compare_exchange`:

```cpp
// Min standard: C++11. Portable.
#include <atomic>
std::atomic<int> value{0};

void try_update(int new_val) {
    int expected = value.load(std::memory_order_relaxed);
    // Loop until we successfully swap; expected is refreshed on failure.
    while (!value.compare_exchange_weak(expected, new_val,
                                        std::memory_order_acq_rel,
                                        std::memory_order_relaxed)) {
        // expected now holds the current value — recompute new_val if needed.
    }
}
```
*Listing 77.1 — A CAS retry loop. On failure, `expected` is updated to the current value automatically.*

**`compare_exchange_weak` vs `strong`:** the weak form may fail *spuriously* (return false even when `expected` matched) because it maps to LL/SC hardware (ARM's `ldxr/stxr`) that can fail for unrelated reasons (an interrupt, a cache event). Use `weak` inside a loop — the spurious failure just retries, and `weak` avoids an extra inner loop the `strong` form emits on LL/SC machines. Use `strong` when you are *not* already in a loop (e.g. a one-shot CAS).

> **Why this matters.** CAS is the universal primitive: anything can be made lock-free by reading the current state, computing a new state, and CAS-ing the change in, retrying on failure. Chapter 93 develops CAS, the `weak`/`strong` distinction, and double-width CAS in full. The crucial cost-model fact is that every CAS is a full-barrier atomic RMW; a tight CAS loop that fails repeatedly under high contention can be *slower* than a lock, because every retry re-pays the barrier and re-reads contended cache lines.

---

## 77.4 The ABA Problem

The most notorious lock-free trap is **ABA**. The water-cooler analogy: you see a full bottle (value A) and leave to fetch a cup. While you are gone, a colleague drinks it empty (B) and another refills it (A again). You return, see "full" (A), conclude nothing changed, and drink — but the water is now different. CAS sees only the *value*, not the *history*: a pointer that was freed and a new node allocated at the same address passes a CAS check that should have failed.

```text
Thread 1: reads head -> node A
Thread 2: pops A, pops B, frees A, pushes a new node that malloc returns at A's old address
Thread 1: CAS(head, A, A->next) SUCCEEDS — but A->next is now garbage
```

> **Why this matters.** ABA is not theoretical; it is the default failure mode of a naive lock-free stack/queue with manual memory management. The CAS succeeds because the pointer bit-pattern matches, but the object it pointed to is gone. Solutions:
>
> - **Tagged / versioned pointers.** Pack a monotonically-increasing counter alongside the pointer in a double-width atomic (`{ptr, tag}` in a 16-byte `atomic`, requiring `cmpxchg16b` on x86). The tag changes on every modification, so A-with-tag-7 ≠ A-with-tag-9.
> - **Hazard pointers.** Each reader publishes the pointer it is using; memory is not reclaimed while any hazard pointer references it (Chapter 94).
> - **RCU (read-copy-update).** Defer reclamation until all readers active at the time of removal have finished (Chapter 94).
>
> The reclamation problem (§77.8) and ABA are two faces of the same issue: *when is it safe to free a node that another thread might still be looking at?*

---

## 77.5 The Lock-Free Stack (Treiber)

The **Treiber stack** is the canonical first lock-free structure: push and pop are each a single CAS on the head pointer.

```cpp
// Min standard: C++11. Illustrative — leaks on pop to avoid the reclamation problem (see 77.8).
#include <atomic>
template <typename T>
class TreiberStack {
    struct Node { T value; Node* next; };
    std::atomic<Node*> head_{nullptr};
public:
    void push(T v) {
        Node* n = new Node{std::move(v), head_.load(std::memory_order_relaxed)};
        // Publish n; release so the consumer sees the fully-constructed node.
        while (!head_.compare_exchange_weak(n->next, n,
                   std::memory_order_release, std::memory_order_relaxed)) {}
    }
    bool pop(T& out) {
        Node* old = head_.load(std::memory_order_acquire);
        while (old && !head_.compare_exchange_weak(old, old->next,
                          std::memory_order_acquire, std::memory_order_relaxed)) {}
        if (!old) return false;
        out = std::move(old->value);
        // delete old;  <-- UNSAFE without reclamation: another thread may hold `old`. See 77.8.
        return true;
    }
};
```
*Listing 77.2 — Treiber stack. Push is correct and safe; pop is correct but cannot free nodes without a reclamation scheme.*

> **Why this matters.** The stack shows both the elegance and the trap of lock-free: push is genuinely simple and correct. Pop is where ABA and reclamation bite — `old->next` is dereferenced after another thread may have popped and freed `old`. The release on push pairs with the acquire on pop to ensure the node's `value` is fully constructed before any consumer reads it (the memory-model edge from Chapter 76). This structure is correct only with hazard pointers, RCU, or tagged pointers added.

---

## 77.6 The Lock-Free Queue (Michael–Scott)

A FIFO queue is markedly harder than a stack because it has *two* hot pointers — head and tail — that must stay consistent. The **Michael–Scott queue** solves this with a dummy sentinel node and a two-step tail advance: an enqueuer first CAS-links the new node, then CAS-advances the tail; any thread that observes a lagging tail *helps* by advancing it before proceeding.

```cpp
// Min standard: C++11. Sketch — full version requires hazard pointers/RCU for node reclamation.
template <typename T>
class MSQueue {
    struct Node { T value; std::atomic<Node*> next{nullptr}; };
    std::atomic<Node*> head_, tail_;
public:
    MSQueue() { Node* d = new Node{}; head_.store(d); tail_.store(d); }  // dummy sentinel
    void enqueue(T v) {
        Node* n = new Node{std::move(v), {}};
        for (;;) {
            Node* tail = tail_.load(std::memory_order_acquire);
            Node* next = tail->next.load(std::memory_order_acquire);
            if (tail == tail_.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    if (tail->next.compare_exchange_weak(next, n,
                            std::memory_order_release, std::memory_order_relaxed)) {
                        tail_.compare_exchange_strong(tail, n,           // swing tail
                            std::memory_order_release, std::memory_order_relaxed);
                        return;
                    }
                } else {
                    tail_.compare_exchange_strong(tail, next,           // help advance lagging tail
                        std::memory_order_release, std::memory_order_relaxed);
                }
            }
        }
    }
    // dequeue() symmetric: read head, read head->next, CAS head forward, return next->value.
};
```
*Listing 77.3 — Michael–Scott queue enqueue. The "help advance" step is what makes it lock-free under interleaving.*

> **Why this matters.** The MS-queue introduces the **helping** principle central to lock-free design: a thread that finds the structure in an intermediate state completes another thread's operation rather than blocking on it — this is exactly what guarantees system-wide progress when a thread is preempted between its two CAS steps. The sentinel node decouples head and tail so an empty queue and a one-element queue are not special cases. Reclamation is again the hard part: a dequeued node may still be referenced by a concurrent enqueuer.

---

## 77.7 The SPSC Ring Buffer

When there is exactly *one* producer and *one* consumer, the whole problem collapses: no CAS is needed at all, only release/acquire on two indices. This single-producer/single-consumer (**SPSC**) ring buffer is the highest-throughput, lowest-latency queue in existence and is the backbone of the Disruptor pattern and most HFT message paths.

```cpp
// Min standard: C++11. Portable. Single producer, single consumer only.
#include <atomic>
#include <vector>
#include <optional>
template <typename T>
class SpscRing {
    std::vector<T> buf_;
    const size_t mask_;                       // capacity is a power of two
    alignas(64) std::atomic<size_t> head_{0}; // consumer index — own cache line
    alignas(64) std::atomic<size_t> tail_{0}; // producer index — own cache line
public:
    explicit SpscRing(size_t cap) : buf_(cap), mask_(cap - 1) {}
    bool push(T v) {                                   // producer thread only
        size_t t = tail_.load(std::memory_order_relaxed);
        if (((t + 1) & mask_) == (head_.load(std::memory_order_acquire) & mask_))
            return false;                              // full
        buf_[t & mask_] = std::move(v);
        tail_.store(t + 1, std::memory_order_release); // publish the slot
        return true;
    }
    bool pop(T& out) {                                 // consumer thread only
        size_t h = head_.load(std::memory_order_relaxed);
        if (h == tail_.load(std::memory_order_acquire)) return false; // empty
        out = std::move(buf_[h & mask_]);
        head_.store(h + 1, std::memory_order_release);
        return true;
    }
};
```
*Listing 77.4 — SPSC ring buffer. No CAS; ABA cannot occur; the only synchronisation is release/acquire on the indices.*

> **Why this matters / cost model.** SPSC is the lesson that the cheapest concurrency is the concurrency you design *out*. Because only one thread writes each index, there is no contention on them and no CAS — just a release store and an acquire load. The two indices live on **separate cache lines** (`alignas(64)`) to avoid false sharing (Chapter 87): if `head_` and `tail_` shared a line, the producer's store would invalidate the consumer's cached copy on every push. The pre-allocated buffer means zero allocation on the hot path. This structure is wait-free for both parties and routinely sustains hundreds of millions of messages per second.

---

## 77.8 The Reclamation Problem

Every pointer-based lock-free structure faces the same question: **when is it safe to `delete` a removed node?** The node was unlinked, but another thread may have loaded a pointer to it just before the unlink and is about to dereference it. Freeing it creates a use-after-free; never freeing it leaks.

The three production answers, all developed in Chapter 94:

- **Hazard pointers** — each thread publishes the pointers it is currently dereferencing; a reclaiming thread defers `delete` until no hazard pointer references the node.
- **RCU (read-copy-update)** — readers mark critical sections cheaply; reclamation waits for a *grace period* in which every pre-existing reader has departed.
- **Epoch-based reclamation** — a coarser, faster cousin of RCU using global epoch counters.

> **Why this matters.** Reclamation is the single hardest part of lock-free programming and the reason most engineers should *use* a vetted library (folly, boost.lockfree, libcds) rather than write their own. The data-structure logic in §§77.5–77.6 is the easy 20%; safe, performant reclamation is the other 80%. A lock-free structure without a correct reclamation strategy is not lock-free — it is broken.

---

## 77.9 When *Not* to Go Lock-Free

| Situation | Better choice |
|---|---|
| Low or no contention | A plain mutex — simpler, often faster |
| Single producer / single consumer | An SPSC ring (§77.7) — no CAS at all |
| Complex multi-word invariants | A lock; lock-free multi-CAS is research-grade |
| Team cannot maintain it | A lock; lock-free bugs are non-deterministic and rare-to-reproduce |
| You haven't measured a lock as the bottleneck | Measure first — most "we need lock-free" intuitions are wrong |

> **The discipline.** Lock-free is open-heart surgery on a running patient: spectacular when the tail latency truly demands it, catastrophic when adopted on intuition. Prefer, in order: no sharing (thread-per-core, Chapter 96) → SPSC rings → a good lock (Chapter 95) → a vetted lock-free library → hand-rolled lock-free. Only the last, and only after measurement, justifies the techniques in this chapter.
