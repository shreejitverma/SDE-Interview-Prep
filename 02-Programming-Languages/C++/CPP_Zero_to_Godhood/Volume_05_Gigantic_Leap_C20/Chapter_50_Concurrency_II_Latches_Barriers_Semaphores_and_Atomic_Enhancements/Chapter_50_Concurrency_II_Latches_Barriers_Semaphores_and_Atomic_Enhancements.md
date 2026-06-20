# Chapter 50: Concurrency II — Latches, Barriers, Semaphores, and Atomic Enhancements

> *C++20 rounds out the concurrency toolkit with the synchronization primitives that previously had to be built by hand from mutexes and condition variables: `std::latch` (a one-shot countdown gate), `std::barrier` (a reusable rendezvous with a completion phase), and `std::counting_semaphore` / `std::binary_semaphore` (classic counting permits). It also upgrades atomics with `std::atomic_ref` (atomic operations on non-atomic objects), `std::atomic<std::shared_ptr<T>>`, and `wait`/`notify` on atomics. This chapter covers each primitive and when to reach for it.*

Before C++20, coordinating "wait until N threads have finished initializing" or "let exactly K threads into this region" meant assembling a mutex, a condition variable, and a counter yourself — code that is easy to get subtly wrong (lost wakeups, spurious-wakeup loops, off-by-one counts). C++20 standardizes these patterns as dedicated, efficiently-implemented primitives, most of them lock-free and backed by the OS futex mechanism. It also fills three long-standing gaps in `<atomic>`: applying atomic operations to objects you do not own atomically, atomic smart pointers, and the ability for a thread to block efficiently on an atomic's value.

---

## Table of Contents

- [50.1 std::latch: A One-Shot Countdown Gate](#501-stdlatch-a-one-shot-countdown-gate)
- [50.2 std::barrier: A Reusable Rendezvous](#502-stdbarrier-a-reusable-rendezvous)
- [50.3 Semaphores: counting_semaphore and binary_semaphore](#503-semaphores-counting_semaphore-and-binary_semaphore)
- [50.4 std::atomic_ref: Atomic Ops on Non-Atomic Objects](#504-stdatomic_ref-atomic-ops-on-non-atomic-objects)
- [50.5 atomic wait and notify](#505-atomic-wait-and-notify)
- [50.6 std::atomic<std::shared_ptr<T>>](#506-stdatomicstdshared_ptrt)
- [50.7 Choosing the Right Primitive](#507-choosing-the-right-primitive)
- [50.8 Professional Insights](#508-professional-insights)

---

## 50.1 std::latch: A One-Shot Countdown Gate

`std::latch` (header `<latch>`) is a **single-use** counter that threads decrement (`count_down`) and wait on (`wait`); once it reaches zero it stays open forever. It is the simplest "wait for N things to finish" primitive.

```cpp
// Listing 50.1: a latch coordinating a startup fan-in
#include <latch>
#include <thread>
#include <vector>

void run_with_latch() {
    constexpr int N = 4;
    std::latch ready{N};            // counter initialized to N

    std::vector<std::jthread> workers;
    for (int i = 0; i < N; ++i)
        workers.emplace_back([&ready]{
            // ... perform one-time initialization ...
            ready.count_down();     // signal "I'm done" (decrement by 1)
        });

    ready.wait();                   // blocks until the counter hits zero
    // All N workers have finished initializing — safe to proceed.
}
```

A latch cannot be reset or reused — once it hits zero it is permanently open, and further `count_down` calls are erroneous. `count_down(n)` can decrement by more than one, `try_wait()` checks without blocking, and `arrive_and_wait()` combines a decrement with a wait. Use a latch for one-time events: "wait until all worker threads have started," "wait until all results are in," "release all threads at once after setup."

---

## 50.2 std::barrier: A Reusable Rendezvous

`std::barrier` (header `<barrier>`) is like a latch that **resets automatically** for the next round, and it runs an optional **completion function** when each phase's count reaches zero. It is the tool for iterative parallel algorithms where all threads must finish phase *k* before any begins phase *k+1*.

```cpp
// Listing 50.2: a barrier synchronizing iterative phases
#include <barrier>
#include <thread>
#include <vector>
#include <iostream>

void run_with_barrier() {
    constexpr int N = 4;

    // The completion function runs ONCE per phase, on the last arriving thread,
    // before any thread is released into the next phase.
    auto on_phase_done = []() noexcept {
        std::cout << "phase complete\n";
    };
    std::barrier sync{N, on_phase_done};

    std::vector<std::jthread> workers;
    for (int i = 0; i < N; ++i)
        workers.emplace_back([&sync](std::stop_token st){
            while (!st.stop_requested()) {
                // ... compute this phase ...
                sync.arrive_and_wait();   // wait for all N, then proceed together
                // ... barrier auto-resets; next iteration is the next phase ...
            }
        });
}
```

Each thread calls `arrive_and_wait()`; when all `N` have arrived, the completion function runs exactly once (on the last-arriving thread) and then **all** threads are released and the barrier resets to `N` for the next phase. A thread can also `arrive_and_drop()` to bow out of all future phases (decrementing the expected count). The completion function is where you do the serial work between phases — merging partial results, swapping double buffers — guaranteed free of data races because no thread is computing during it.

---

## 50.3 Semaphores: counting_semaphore and binary_semaphore

`std::counting_semaphore<Max>` (header `<semaphore>`) maintains a count of available **permits**: `acquire()` takes one (blocking if none are free), `release()` returns one. `std::binary_semaphore` is the alias `counting_semaphore<1>`, usable as a lightweight signal or lock.

```cpp
// Listing 50.3: limiting concurrency with a counting semaphore
#include <semaphore>
#include <thread>
#include <vector>

// Allow at most 3 threads into the critical resource at once.
std::counting_semaphore<3> pool{3};   // 3 permits available

void use_resource() {
    pool.acquire();                   // take a permit (block if all 3 are out)
    // ... use the rate-limited resource ...
    pool.release();                   // return the permit
}

// binary_semaphore as a one-shot signal between two threads:
std::binary_semaphore signal{0};      // start with 0 permits (unsignaled)

void producer() { /* produce */ signal.release(); }   // signal "ready"
void consumer() { signal.acquire(); /* now safe to consume */ }
```

The template parameter is the *maximum* count (a compile-time hint enabling more efficient implementations); the constructor argument is the *initial* count. `try_acquire()`, `try_acquire_for()`, and `try_acquire_until()` provide non-blocking and timed variants. Unlike a mutex, a semaphore has no ownership — any thread may `release()` a permit it did not `acquire()`, which is exactly what makes `binary_semaphore` work as a cross-thread signal (one thread waits, another signals).

---

## 50.4 std::atomic_ref: Atomic Ops on Non-Atomic Objects

`std::atomic_ref<T>` (header `<atomic>`) applies atomic operations to an **existing non-atomic object** for the lifetime of the `atomic_ref`. This lets you store data in plain arrays or structs and apply atomicity selectively, rather than declaring everything `std::atomic`.

```cpp
// Listing 50.4: atomic operations on plain array elements
#include <atomic>
#include <vector>
#include <thread>

void parallel_histogram(std::vector<int>& buckets,
                        const std::vector<int>& samples) {
    // buckets holds plain ints — NOT std::atomic<int>.
    // Many threads increment shared buckets concurrently, atomically:
    for (int s : samples) {
        std::atomic_ref<int> bucket{buckets[s]};   // atomic view of one element
        bucket.fetch_add(1, std::memory_order_relaxed);
    }
}
```

`atomic_ref` solves a real layout problem: you cannot put `std::atomic<T>` in a type that must remain trivially copyable, memcpy-able, or laid out for a C API — and you often want to read/write a large buffer non-atomically in one phase and atomically in another. While any `atomic_ref` to an object exists, **all** access to that object must go through `atomic_ref`s (mixing atomic and non-atomic access to the same object concurrently is a data race). The referenced object must also outlive the `atomic_ref`, and for full atomicity it should satisfy the type's required alignment (queryable via `atomic_ref<T>::required_alignment`).

---

## 50.5 atomic wait and notify

C++20 adds `wait`, `notify_one`, and `notify_all` to `std::atomic` (and `atomic_ref`, and the semaphores/latches internally). A thread can block efficiently until an atomic's value changes — replacing busy-wait spin loops with an OS-level wait (futex), without a separate mutex/condition-variable pair.

```cpp
// Listing 50.5: blocking on an atomic without a condition variable
#include <atomic>
#include <thread>

std::atomic<bool> ready{false};

void waiter() {
    ready.wait(false);          // block while value == false; wake when it changes
    // ... ready is now true, proceed ...
}

void setter() {
    ready.store(true);
    ready.notify_one();         // wake one thread blocked in ready.wait(...)
}
```

`atomic.wait(old)` blocks **as long as** the atomic still holds `old`, returning when the value differs and a notification arrives; `notify_one`/`notify_all` wake waiters. This is a lighter-weight alternative to a condition variable for the common "flag flips once" pattern — no mutex, no predicate loop (the wait already loops internally and handles spurious wakeups). It is the mechanism underpinning the efficient blocking in latches, barriers, and semaphores.

---

## 50.6 std::atomic<std::shared_ptr<T>>

C++20 provides a proper specialization `std::atomic<std::shared_ptr<T>>` (and `atomic<weak_ptr<T>>`), replacing the deprecated free-function `std::atomic_load(&sp)` overloads. It makes lock-free-style publication of shared, reference-counted data safe and ergonomic.

```cpp
// Listing 50.6: atomically swapping shared configuration
#include <atomic>
#include <memory>

struct Config { /* ... immutable snapshot ... */ };

std::atomic<std::shared_ptr<Config>> g_config{std::make_shared<Config>()};

// Readers: grab a consistent snapshot, atomically (and keep it alive).
std::shared_ptr<Config> current() {
    return g_config.load();          // atomic; the returned shared_ptr is safe to use
}

// Writer: publish a new config without locking readers out.
void update(std::shared_ptr<Config> next) {
    g_config.store(std::move(next));  // old config destroyed when last reader releases it
}
```

The specialization makes the control-block reference counting and the pointer swap atomic together, so readers always observe a consistent, fully-owned `shared_ptr` and the previous object's lifetime is correctly extended until the last reader is done. This is the canonical pattern for **read-mostly shared configuration** or any "publish a new immutable snapshot" scenario, giving much of the benefit of RCU with standard tools. Note that whether the implementation is truly lock-free is platform-dependent (check `is_lock_free()`); even when internally locked, it is correct and far simpler than hand-rolled double-checked locking.

---

## 50.7 Choosing the Right Primitive

The new primitives overlap; the decision rests on reusability, ownership, and how many threads pass.

| Primitive | One-shot? | Use when |
|-----------|-----------|----------|
| `latch` | yes | Wait once for N tasks to finish; release-all gate |
| `barrier` | no (auto-resets) | Iterative phases; N threads rendezvous each round |
| `counting_semaphore` | reusable | Limit concurrency to K; resource pool of permits |
| `binary_semaphore` | reusable | One-shot cross-thread signal; lightweight non-owning lock |
| `atomic::wait`/`notify` | reusable | Block on a single value change; no mutex needed |
| `mutex` + `condition_variable` | reusable | Complex predicates over shared mutable state |

The rule of thumb: reach for the **most specific** primitive that fits. A latch expresses "wait for N once" more clearly and efficiently than a hand-built counter; a barrier expresses phased computation; a semaphore expresses permit-counting. Fall back to mutex + condition variable only when the wait condition is a non-trivial predicate over shared mutable state that none of the dedicated primitives capture.

---

## 50.8 Professional Insights

**Replace hand-built counter/condition-variable coordination with `latch` and `barrier`.** "Wait until N threads finish" and "all threads rendezvous each phase" are exactly the patterns that hand-rolled mutex+condvar+counter code gets wrong — lost wakeups, off-by-one counts, missing spurious-wakeup loops. `std::latch` and `std::barrier` are correct by construction, typically lock-free, and express intent directly. The barrier's completion function is especially valuable: it gives you a guaranteed race-free serial point between parallel phases for merging results or flipping buffers.

**Use `counting_semaphore` to cap concurrency and `binary_semaphore` to signal.** A semaphore initialized to K is the cleanest way to limit how many threads touch a resource at once (connection pools, rate limiters, bounded in-flight work). Crucially, semaphores are *unowned* — any thread may release a permit it never acquired — which is precisely what makes `binary_semaphore{0}` the ideal one-shot producer→consumer signal, something a mutex (which requires the releaser to be the owner) cannot do.

**Reach for `std::atomic_ref` when atomicity must be selective or the type must stay plain.** When a buffer is filled non-atomically in one phase and updated concurrently in another, or when a type must remain trivially copyable for a C API or memcpy, you cannot make its members `std::atomic`. `atomic_ref` applies atomicity per-access without changing the object's type or layout. The discipline it demands is absolute: while any `atomic_ref` to an object is live, *every* concurrent access to that object must go through an `atomic_ref`, and you must honor the required alignment — mixing a plain write with an atomic access is a data race.

**Prefer `atomic::wait`/`notify` over spin loops and over condvars for single-value waits.** A `while (!flag.load()) ;` spin burns a core; a mutex+condition-variable for a single boolean flag is heavyweight boilerplate. `flag.wait(false)` blocks at the OS level (futex) and wakes on `notify`, handling spurious wakeups internally — the right tool when the wait condition is "this one atomic changed." Save the full condition-variable machinery for genuinely compound predicates over shared mutable state.

**Adopt `std::atomic<std::shared_ptr<T>>` for read-mostly published snapshots.** It is the modern, standard replacement for the deprecated `std::atomic_*(shared_ptr*)` free functions and the safe way to hot-swap immutable configuration or data snapshots: readers `load()` a fully-owned, consistent `shared_ptr` while a writer `store()`s a new one, with the old object's lifetime correctly extended until the last reader releases it. Verify `is_lock_free()` on your target if lock-freedom matters — but even when internally locked it is dramatically simpler and safer than hand-rolled double-checked locking, and it eliminates the torn-pointer and refcount-race bugs that plague manual approaches.
