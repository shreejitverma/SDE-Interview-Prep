# Chapter 78: Advanced Concurrency Patterns

Primitives — atomics, locks, lock-free queues — are necessary but not sufficient; at scale you need *architectures* that arrange work so that contention is rare, allocation is absent from the hot path, and the design itself prevents data races. This chapter surveys the patterns senior engineers actually compose systems from: thread pools, the actor model, the Disruptor, coroutine pipelines, and C++26 structured concurrency (`std::execution`). Each is examined for what it costs, what hazard it eliminates by construction, and when its overhead is not worth paying.

## Chapter Roadmap

- 78.1 From Primitives to Architectures
- 78.2 Thread Pools and Work Queues
- 78.3 The Actor Model: Concurrency by Message Passing
- 78.4 The Disruptor: Ring-Buffer Inter-Thread Messaging
- 78.5 Coroutines for Async Pipelines
- 78.6 Structured Concurrency and `std::execution`
- 78.7 False Sharing and Padding
- 78.8 Choosing a Pattern

---

## 78.1 From Primitives to Architectures

A correct mutex does not make a scalable system. The dominant performance problems at scale are *architectural*: too many threads contending one lock, allocation on the hot path triggering page faults, cache lines ping-ponging between cores, and threads migrating across NUMA nodes. The patterns in this chapter address those at the design level.

> **Why this matters.** The cheapest synchronisation is the synchronisation you avoid. Every pattern here is, at bottom, a strategy for *reducing the surface area of sharing*: a thread pool funnels work through one queue you can tune; an actor owns its state so no lock is needed; the Disruptor pre-allocates and uses single-writer slots; coroutines and senders express dependencies so the scheduler — not a blocking call — manages waiting. The unifying theme of mastery-level concurrency is structure, not cleverness with `memory_order`.

---

## 78.2 Thread Pools and Work Queues

Spawning a thread is expensive: a `clone`/`pthread_create` syscall, a stack allocation (often 8 MB of virtual address space committed lazily), and scheduler bookkeeping — easily tens of microseconds. A **thread pool** amortises this by creating worker threads once and feeding them tasks through a queue.

```cpp
// Min standard: C++17. Portable. Illustrative fixed-size pool.
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;
    std::mutex m_;
    std::condition_variable cv_;
    bool stop_ = false;
public:
    explicit ThreadPool(unsigned n) {
        for (unsigned i = 0; i < n; ++i)
            workers_.emplace_back([this] { worker_loop(); });
    }
    void submit(std::function<void()> task) {
        { std::lock_guard lk(m_); tasks_.push(std::move(task)); }
        cv_.notify_one();
    }
    ~ThreadPool() {
        { std::lock_guard lk(m_); stop_ = true; }
        cv_.notify_all();
        for (auto& t : workers_) t.join();
    }
private:
    void worker_loop() {
        for (;;) {
            std::function<void()> task;
            { std::unique_lock lk(m_);
              cv_.wait(lk, [this] { return stop_ || !tasks_.empty(); });
              if (stop_ && tasks_.empty()) return;
              task = std::move(tasks_.front()); tasks_.pop(); }
            task();
        }
    }
};
```
*Listing 78.1 — A minimal thread pool with a single mutex-protected queue.*

> **Why this matters / cost model.** The single global queue in Listing 78.1 is the obvious bottleneck: every submit and every steal contends `m_`. Production pools (Intel TBB, folly, `libdispatch`) use **per-worker deques with work stealing**: each worker pushes/pops its own deque LIFO (cache-hot, lock-free) and only steals from another worker's deque (FIFO, contended) when idle. This converts the common case to zero contention. Sizing matters too: more threads than cores causes scheduler thrash and cache pollution; the right size is usually `hardware_concurrency()` for CPU-bound work, higher only when tasks block on I/O.

---

## 78.3 The Actor Model: Concurrency by Message Passing

The **actor model** eliminates data races by eliminating shared state. Each actor owns its private state, has a mailbox (a queue), and processes one message at a time on a single logical thread of control. Actors communicate *only* by sending immutable messages.

```cpp
// Min standard: C++17. Conceptual sketch.
class Actor {
    SpscRing<Message> mailbox_;   // or MPSC for multiple senders
public:
    void send(Message m) { mailbox_.push(std::move(m)); }  // never touches actor state
    void run() {                                            // one scheduler thread drives this
        Message m;
        while (mailbox_.pop(m)) handle(m);                  // serial: no lock needed
    }
private:
    void handle(const Message&);   // mutates private state safely — single-threaded by construction
};
```
*Listing 78.2 — Actor skeleton: state is private, mutation is serial.*

> **Why this matters.** The actor model trades shared-memory speed for a *correctness guarantee by construction*: because only the actor's own thread mutates its state, there is no data race to reason about and no lock to forget. The cost is message-passing overhead (queue operations, possible copies) and the loss of direct shared-memory throughput. It shines for stateful, logically-independent entities (connections, sessions, order books per symbol) and maps naturally onto thread-per-core (Chapter 96). Frameworks: CAF (C++ Actor Framework), Erlang/Akka conceptually. The hazard is mailbox unboundedness — a fast sender can overwhelm a slow actor; production actors apply backpressure.

---

## 78.4 The Disruptor: Ring-Buffer Inter-Thread Messaging

The **Disruptor** (LMAX) is a pre-allocated ring buffer with published sequence numbers, designed for the lowest possible latency in handing events between stages. It generalises the SPSC ring (Chapter 77) to multiple consumers arranged in a dependency graph, using sequence counters and memory barriers instead of locks.

Key design choices and *why* each exists:

- **Pre-allocated ring of fixed entries.** Zero allocation on the hot path — no `malloc`, no GC pause, no page fault mid-trade.
- **Published sequence numbers.** A producer claims slot `n`, writes it, then publishes by advancing a sequence; consumers spin on the sequence with an acquire load. This is release/acquire publication, not a lock.
- **Consumers form a dependency graph.** A "journaller" and a "business-logic" consumer can both read each slot; a downstream consumer waits on the slowest upstream sequence. No event is copied between stages — they all read the same slot.
- **Cache-line padding** on every sequence counter to prevent false sharing.

> **Why this matters / cost model.** The Disruptor's insight is that a queue's real cost is not the enqueue/dequeue logic but the *allocation, the contended head/tail, and the cache traffic*. By pre-allocating and giving each writer/reader its own padded sequence, it reduces per-event cost to a handful of cache-hot operations and sustains tens of millions of events/sec with sub-microsecond latency. It is the canonical HFT inter-thread mechanism. The trade-off: it is a *fixed-capacity, busy-spinning* design that burns a core polling — appropriate only when latency dominates and a core can be dedicated (Chapter 96).

---

## 78.5 Coroutines for Async Pipelines

C++20 **coroutines** let asynchronous code be written in straight-line form: a function suspends at `co_await`, returning control to a scheduler, and resumes when its awaited result is ready — without a thread blocking in the meantime.

```cpp
// Min standard: C++20. Requires a Task<>/awaitable framework (illustrative).
Task<int> async_algo() {
    int a = co_await fetch_a();     // suspends here; the thread is free to do other work
    int b = co_await fetch_b();
    co_return a + b;
}
```
*Listing 78.3 — Async logic that reads like synchronous code.*

> **Why this matters / cost model.** The alternative to coroutines is callback hell (manual continuation-passing) or thread-per-request (one OS thread blocked per in-flight operation, which does not scale past a few thousand). A coroutine frame is a small heap allocation (often elidable by HALO — heap allocation elision optimisation — when the compiler can prove the frame's lifetime) holding only the live locals across suspension points. Ten thousand in-flight requests cost ten thousand small frames, not ten thousand 8 MB stacks. The hazards are subtle: a coroutine that captures a reference which dangles across a suspension point is a use-after-free, and the lifetime of the awaited object must outlive the suspension. Coroutines move the bookkeeping from the programmer to the compiler — but the lifetime reasoning is now *across* suspension points, which is harder.

---

## 78.6 Structured Concurrency and `std::execution`

C++26's **`std::execution`** (senders/receivers, P2300) provides a standard, composable model for asynchronous work: a *sender* describes work that will produce a value, *algorithms* (`then`, `when_all`, `let_value`) compose senders into a graph, and a *scheduler* decides where each piece runs. **Structured concurrency** is the discipline that an asynchronous operation's lifetime is bounded by a lexical scope — no work outlives the scope that launched it, so cancellation and error propagation are well-defined.

```cpp
// Min standard: C++26 (std::execution). Illustrative; APIs still stabilising.
// auto work = schedule(pool)
//           | then([] { return load(); })
//           | then([](Data d) { return process(d); });
// auto [result] = sync_wait(std::move(work)).value();
```
*Listing 78.4 — A sender pipeline; the scheduler, not a blocking call, manages waiting.*

> **Why this matters.** Unstructured async (raw threads, detached futures, fire-and-forget callbacks) leaks work, races on shutdown, and propagates errors poorly — a `std::async` whose future is discarded may block in its destructor or run detached. Structured concurrency makes the *shape* of concurrent work match the *shape* of the code: errors and cancellation flow back to the launching scope like exceptions flow up a call stack. `std::execution` standardises this so libraries from different vendors compose. Until it stabilises across toolchains, treat it as forward-looking; the *principle* (bound async lifetimes to scopes) applies today with libraries like libunifex and stdexec.

---

## 78.7 False Sharing and Padding

**False sharing** occurs when two threads write to *different* variables that happen to occupy the *same cache line* (typically 64 bytes). The cache coherence protocol treats the line as the unit of ownership, so each write invalidates the other core's copy, forcing a coherence round-trip even though the threads share no logical data.

```cpp
// Min standard: C++17. Portable.
#include <atomic>
#include <new>   // std::hardware_destructive_interference_size (C++17)

struct alignas(64) PaddedCounter {        // each counter on its own cache line
    std::atomic<long> value{0};
};
PaddedCounter counters[NUM_THREADS];      // no false sharing between threads
```
*Listing 78.5 — Padding each per-thread counter to its own cache line.*

`std::hardware_destructive_interference_size` (C++17) names the alignment that avoids false sharing portably, though many codebases hard-code 64 (and some CPUs prefetch pairs of lines, suggesting 128).

> **Why this matters / cost model.** False sharing turns an embarrassingly-parallel design into a serialised one: N threads each incrementing their "own" counter, but all counters on one line, run *slower than one thread* because every increment triggers an invalidation and re-fetch (dozens to hundreds of cycles). It is invisible in the source — the variables are distinct — and only a profiler (`perf c2c` on Linux) or a cache-aware reading of the layout reveals it. This is developed fully in Chapter 87; the rule here is: any per-thread mutable datum that is written hot must be padded to its own line.

---

## 78.8 Choosing a Pattern

| Pattern | Eliminates by construction | Cost | Best for |
|---|---|---|---|
| Thread pool (work-stealing) | Thread-spawn overhead | Queue contention (mitigated by per-worker deques) | General CPU-bound task parallelism |
| Actor model | Data races (private state) | Message-passing overhead | Stateful independent entities; thread-per-core |
| Disruptor | Allocation + contended queue | A busy-spinning dedicated core | Lowest-latency event pipelines (HFT) |
| Coroutines | Thread-per-request blocking | Frame allocation; lifetime subtlety | High-concurrency I/O |
| `std::execution` | Leaked/unstructured async | Framework complexity; immaturity | Composable async graphs with cancellation |

> **The discipline.** Reach for the pattern that removes the hazard *by design* rather than guarding against it with locks. For independent stateful work, prefer actors/thread-per-core; for fan-out compute, a work-stealing pool; for the latency-critical hand-off, a Disruptor/SPSC ring; for high-concurrency I/O, coroutines under a structured scheduler. The next chapters — allocators, threading discipline, and the OS interface — give these patterns the allocation-free, core-pinned, syscall-light substrate they need to actually hit their latency targets.
