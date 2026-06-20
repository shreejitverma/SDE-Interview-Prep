# Chapter 16: Concurrency

> *Before C++11 the language had no idea threads existed. C++11 standardized a memory model, threads, mutexes, condition variables, futures, and atomics — turning portable, well-defined concurrency into a first-class part of the language.*

This is the chapter that mattered most for systems work. C++11 defined, for the first time, **what a program means when multiple threads touch the same memory** — the *memory model* — and built a portable concurrency library on top of it: `std::thread`, the `std::mutex` family, `std::condition_variable`, `std::call_once`, the futures framework (`std::async`/`std::future`/`std::promise`/`std::packaged_task`), and `std::atomic`. We cover each with the correctness traps and performance characteristics that decide their use in low-latency and kernel-adjacent code, and close with OpenMP for data-parallel loops.

---

## Table of Contents

- [16.1 The C++11 Threading Model](#161-the-c11-threading-model)
- [16.2 Mutexes and Locks](#162-mutexes-and-locks)
- [16.3 Condition Variables](#163-condition-variables)
- [16.4 `std::call_once` and `std::once_flag`](#164-stdcall_once-and-stdonce_flag)
- [16.5 Futures, Promises, and `std::async`](#165-futures-promises-and-stdasync)
- [16.6 Atomics and the C++11 Memory Model](#166-atomics-and-the-c11-memory-model)
- [16.7 `thread_local` Storage](#167-thread_local-storage)
- [16.8 Data Parallelism with OpenMP](#168-data-parallelism-with-openmp)
- [16.9 Professional Insights](#169-professional-insights)

---

## 16.1 The C++11 Threading Model

A `std::thread` is a separate flow of execution. You construct it with a callable and its arguments; the thread starts running immediately. Before the `std::thread` object is destroyed you **must** call either `join()` (block until it finishes) or `detach()` (let it run independently) — otherwise the destructor calls `std::terminate()`.

```cpp
// Listing 16.1: launching a thread with any callable
#include <thread>
#include <iostream>

void free_fn(int a)             { std::cout << a << '\n'; }
struct Functor { void operator()(int a) const { std::cout << a << '\n'; } };

int main() {
    std::thread t1(free_fn, 10);                       // free function
    std::thread t2(Functor{}, 20);                     // functor
    std::thread t3([](int a){ std::cout << a; }, 30);  // lambda
    Functor f;                                         // member fn:
    // std::thread t4(&Class::method, &obj, args...);
    t1.join(); t2.join(); t3.join();
}
```

**Arguments are copied/moved into the thread**, never passed by reference, even when the target takes a reference. To pass a genuine reference you must wrap it with `std::ref`/`std::cref` (Chapter 13):

```cpp
// Listing 16.2: passing by reference requires std::ref
void fill(int& out) { out = 10; }

int a = 1;
std::thread t(fill, std::ref(a));   // without ref(), 'a' would be copied
t.join();                           // a == 10
```

**Joinability and exception safety.** If code between thread creation and `join()` throws, the `join()` is skipped and the program terminates. The fix is RAII — a guard that joins in its destructor (the standard later codified this as C++20's `std::jthread`):

```cpp
// Listing 16.3: an RAII thread joiner (exception-safe)
class thread_joiner {
    std::thread t_;
public:
    explicit thread_joiner(std::thread t) : t_(std::move(t)) {}
    ~thread_joiner() { if (t_.joinable()) t_.join(); }
};
```

**Current-thread operations** live in `std::this_thread`: `get_id()`, `sleep_for(duration)`, `sleep_until(time_point)`, and `yield()` (hint the scheduler to run other threads).

---

## 16.2 Mutexes and Locks

A **mutex** (mutual exclusion) serializes access to shared data. C++11 provides a family:

| Type | Purpose |
| :--- | :--- |
| `std::mutex` | Basic, non-recursive lock |
| `std::recursive_mutex` | Same thread may lock repeatedly |
| `std::timed_mutex` | Adds `try_lock_for` / `try_lock_until` |
| `std::recursive_timed_mutex` | Recursive + timed |

You almost never lock a mutex directly; you use an RAII **lock wrapper** so the mutex is released on every exit path, including exceptions.

```cpp
// Listing 16.4: lock_guard — lock for the full scope
std::mutex m;
void worker() {
    std::lock_guard<std::mutex> guard(m);  // locks here
    // critical section
}                                          // unlocks automatically
```

`std::lock_guard` is the lightweight, scope-bound choice. **`std::unique_lock`** is heavier but flexible: it can defer locking, unlock and re-lock manually, transfer ownership, and is required by `std::condition_variable`.

```cpp
// Listing 16.5: unique_lock with manual control
std::unique_lock<std::mutex> guard(m, std::defer_lock); // not locked yet
guard.lock();
// critical section
guard.unlock();                                          // release early
```

**Lock strategies** (the optional second constructor argument):

- `std::defer_lock` — construct without locking; lock later.
- `std::try_to_lock` — attempt to lock without blocking; test `owns_lock()`.
- `std::adopt_lock` — assume the calling thread already owns the mutex.

**Locking multiple mutexes** risks deadlock if two threads acquire them in different orders. `std::lock` solves this with a deadlock-avoidance algorithm:

```cpp
// Listing 16.6: deadlock-free multi-lock with std::lock + adopt_lock
std::mutex m1, m2;
std::lock(m1, m2);                                  // atomic, order-independent
std::lock_guard<std::mutex> g1(m1, std::adopt_lock);
std::lock_guard<std::mutex> g2(m2, std::adopt_lock);
// both held; released in reverse order at scope exit
```

> **C++14/17 forward references:** C++14 added `std::shared_timed_mutex` and `std::shared_lock` (reader-writer locking — many readers or one writer). C++17 added `std::shared_mutex` and `std::scoped_lock` (a variadic, deadlock-avoiding RAII lock for any number of mutexes — prefer it over the `std::lock` + `adopt_lock` dance).

---

## 16.3 Condition Variables

A **condition variable** lets a thread sleep until another thread signals that some shared state changed — the standard building block for producer-consumer queues. It is always used with a `std::unique_lock<std::mutex>` so the wait can atomically release the mutex while sleeping and re-acquire it on wakeup.

```cpp
// Listing 16.7: the predicate form guards against spurious wakeups
std::mutex mtx;
std::condition_variable cv;
bool ready = false;

// Waiter:
{
    std::unique_lock<std::mutex> lk(mtx);
    cv.wait(lk, []{ return ready; });   // sleeps until ready == true
    // proceed: mutex is held here
}

// Notifier:
{
    std::lock_guard<std::mutex> lk(mtx);
    ready = true;
}
cv.notify_one();                        // wake one waiter (notify_all wakes all)
```

**Always use the predicate overload** of `wait`. A condition variable may return from `wait` *spuriously* (without any notification); the predicate is re-checked on every wakeup, so a spurious return simply goes back to sleep. The bare `cv.wait(lk)` form, without a predicate, is a bug magnet.

```cpp
// Listing 16.8: producer-consumer sketch
std::queue<int> q;
bool stopped = false;
// consumer:
std::unique_lock<std::mutex> lk(mtx);
cv.wait(lk, [&]{ return stopped || !q.empty(); });
while (!q.empty()) { int v = q.front(); q.pop(); /* use v */ }
```

The notifier should hold the mutex while *modifying* the shared state, then may notify with the lock held or released. Notifying without first updating the state (and without the waiter holding the lock to observe it) is the classic lost-wakeup race.

---

## 16.4 `std::call_once` and `std::once_flag`

`std::call_once` guarantees a callable runs **exactly once** across all threads, even if many call it concurrently — the correct, race-free way to do lazy one-time initialization. It pairs with a `std::once_flag` that records whether the work has happened.

```cpp
// Listing 16.9: thread-safe lazy initialization
#include <mutex>

std::once_flag init_flag;
Resource* g_resource = nullptr;

void init() { g_resource = new Resource(/* expensive setup */); }

Resource& get() {
    std::call_once(init_flag, init);  // init() runs once, ever; others block until done
    return *g_resource;
}
```

If the callable throws, the flag is **not** marked done and the next caller retries — `call_once` provides this exactly-once-on-success guarantee for you. This is cleaner and safer than the double-checked-locking pattern that programmers wrote (often incorrectly) before C++11.

> **Note:** a function-local `static` with a non-trivial initializer also gives thread-safe one-time init in C++11 (the so-called "magic statics"), and is preferable when the initialized object can simply *be* the static. Use `call_once` when initialization and storage are separated, or must be triggered explicitly.

---

## 16.5 Futures, Promises, and `std::async`

The futures framework moves a *result* (or an exception) from a producer to a consumer across threads, replacing manual mutex+condition-variable plumbing for the common "compute a value elsewhere, fetch it later" case.

- **`std::future<T>`** — the consumer handle; `get()` blocks until the value is ready (and rethrows any stored exception).
- **`std::promise<T>`** — the producer handle; `set_value()` / `set_exception()` fulfills the matching future.
- **`std::packaged_task<R(Args...)>`** — wraps a callable so its return value lands in an associated future; the unit a thread pool queues.
- **`std::async`** — the high-level launcher: runs a callable and returns a future for its result.

```cpp
// Listing 16.10: std::async for fire-and-fetch
#include <future>
unsigned square(unsigned i) { return i * i; }

auto f = std::async(std::launch::async, square, 8); // forces a new thread
// ... do other work ...
unsigned r = f.get();                               // blocks until ready -> 64
```

**Two notorious `std::async` pitfalls:**

1. **The launch policy.** `std::async(square, 8)` (no policy) lets the implementation choose between `launch::async` (new thread) and `launch::deferred` (lazy, runs synchronously inside `get()`). In practice implementations often pick `deferred`, so *nothing runs in parallel*. Pass `std::launch::async` explicitly when you need a real thread.

2. **The blocking destructor.** The future returned by `std::async` has a special property: its **destructor blocks** until the task completes. So `std::async(std::launch::async, f);` with the return value discarded is effectively synchronous — the temporary future destructs at the end of the full expression and waits. Always bind the future to a named variable that lives as long as you want the work to overlap.

```cpp
// Listing 16.11: promise/future for explicit hand-off
std::promise<int> p;
std::future<int> f = p.get_future();
std::thread producer([&p]{ p.set_value(42); });
int v = f.get();          // 42
producer.join();
```

The framework also transports exceptions: an exception escaping the task is captured and rethrown from `future::get()`, so error handling crosses the thread boundary cleanly (`std::exception_ptr` is the lower-level vehicle).

---

## 16.6 Atomics and the C++11 Memory Model

The deepest part of C++11 concurrency is the **memory model**: the formal contract describing when one thread's writes become visible to another. Before C++11 this was implementation-defined folklore; C++11 made it a guarantee, and `std::atomic` is how you program against it.

**`std::atomic<T>`** provides indivisible operations on a `TriviallyCopyable` `T` (integers, pointers, small PODs). Two threads accessing the same atomic — one writing, one reading — is **well-defined**, not a data race. `std::atomic` is neither copyable nor movable.

```cpp
// Listing 16.12: atomic counter — no mutex required
#include <atomic>
std::atomic<int> counter{0};
counter.fetch_add(1);          // atomic read-modify-write
++counter;                     // same thing
int now = counter.load();      // atomic read
counter.store(100);            // atomic write
```

Beyond atomicity, every atomic operation carries a **memory ordering** that constrains how *surrounding non-atomic* memory operations may be reordered by the compiler and CPU. This is the mechanism that makes lock-free code correct.

| `std::memory_order` | Guarantee |
| :--- | :--- |
| `memory_order_seq_cst` | **Default.** Single global total order; the simplest to reason about, the strongest, the slowest |
| `memory_order_acquire` | On a load: no later reads/writes move before it (pairs with release) |
| `memory_order_release` | On a store: no earlier reads/writes move after it (publishes prior writes) |
| `memory_order_acq_rel` | Both, for read-modify-write operations |
| `memory_order_relaxed` | Atomicity only — **no** ordering constraints on other memory |

```cpp
// Listing 16.13: acquire/release publishes data without a lock
std::atomic<bool> ready{false};
int payload = 0;

// producer:
payload = 42;                                  // ordinary write
ready.store(true, std::memory_order_release);  // publish: payload happens-before

// consumer:
while (!ready.load(std::memory_order_acquire)) {}  // acquire: see the publish
// payload is now guaranteed to read 42
```

```cpp
// Listing 16.14: relaxed is enough for a pure counter
std::atomic<long> hits{0};
hits.fetch_add(1, std::memory_order_relaxed);  // count only; no ordering needed
```

**Why this matters for systems work.** The default `seq_cst` is correct everywhere and is the right starting point. But sequential consistency forces memory fences that cost real cycles on multi-socket hardware. In a profiled hot path — a lock-free queue, a statistics counter, a sequence-lock — dropping to `acquire`/`release` or `relaxed` removes fences the algorithm does not need. This is also where the **ABA problem** and the subtlety of `compare_exchange_weak`/`compare_exchange_strong` live; relaxing ordering is powerful and unforgiving, and must be justified by both a correctness argument and a measurement.

`std::atomic_flag` is the one type guaranteed lock-free on every platform; for other `T`, query `is_lock_free()` — a "lock-free" `std::atomic<T>` that secretly uses a mutex defeats the purpose in a real-time context.

---

## 16.7 `thread_local` Storage

The **`thread_local`** storage-duration specifier gives each thread its own independent instance of a variable. A function-local `thread_local` is implicitly `static` and initialized the first time control reaches it on each thread; a namespace/class-scope one is initialized at thread startup. Each thread's copy is destroyed when that thread exits.

```cpp
// Listing 16.15: per-thread state without locking
void record() {
    thread_local int calls = 0;     // one counter per thread
    ++calls;                        // no synchronization needed
}
```

`thread_local` is the idiomatic way to give each thread a private random-number engine (Chapter 15), a scratch buffer, or a per-thread allocator arena — eliminating contention by eliminating sharing. A class member may be `thread_local` only if it is also `static` (one copy per thread, not per object).

---

## 16.8 Data Parallelism with OpenMP

The standard library gives you *task* concurrency; **OpenMP** is a complementary, compiler-directive API for *data* parallelism — splitting loop iterations across a team of threads with minimal code change. It is ubiquitous in HPC and useful wherever a loop's iterations are independent.

```cpp
// Listing 16.16: a parallel region and a parallel loop
#include <omp.h>
#pragma omp parallel
{
    int id = omp_get_thread_num();   // each thread runs this block
}

#pragma omp parallel for
for (int i = 0; i < 1000; ++i)
    results[i] = compute(i);          // iterations distributed across threads
```

**Data-sharing clauses** declare how variables behave across threads: `shared` (one instance, visible to all), `private` (a per-thread copy), and `reduction` (per-thread partials combined into one result):

```cpp
// Listing 16.17: reduction combines per-thread partial sums safely
double total = 0;
#pragma omp parallel for reduction(+:total)
for (int i = 0; i < 100; ++i)
    total += data[i];
```

**Scheduling** controls how iterations are handed out: `static` (fixed chunks, lowest overhead), `dynamic` (handed out at runtime, best for uneven workloads), `guided` (large chunks shrinking over time to cut tail latency).

> **Performance traps.** *False sharing* — two threads writing different variables that land on the same cache line — silently destroys scaling as the line ping-pongs between cores; fix it by padding or spacing per-thread data to cache-line boundaries. *Thread affinity* (`OMP_PROC_BIND=true`) pins threads to cores, preventing migration that thrashes caches.

---

## 16.9 Professional Insights

**Always make `join`/`detach` exception-safe.** A bare `std::thread` whose `join()` is skipped by an exception terminates the process. Wrap threads in an RAII joiner (or use C++20's `std::jthread`). Never leave joinability to a single happy-path `join()` call.

**Prefer the highest-level tool that fits.** `std::async`/futures for "compute a value elsewhere"; a condition-variable queue for streaming producer-consumer; mutexes for shared mutable state; atomics only when you have measured that locking is the bottleneck. Reaching for `relaxed` atomics first is premature optimization that produces subtly broken code.

**Lock ordering is a global invariant.** Deadlocks come from inconsistent acquisition order. Either always acquire mutexes in one documented order, or use `std::scoped_lock` (C++17) / `std::lock` so the library enforces deadlock avoidance. Keep critical sections short — hold locks for data manipulation, not for I/O or computation.

**`seq_cst` first, relax with evidence.** Start every lock-free design at the default sequential consistency. Weaken to acquire/release or relaxed only in a profiled hot path, with a written happens-before argument and verification under a tool like ThreadSanitizer. The fences `seq_cst` inserts are exactly what make multi-socket atomics expensive — and exactly what makes naive weakening wrong.

**Minimize sharing before you optimize synchronization.** The fastest lock is the one you never take. `thread_local` state, per-thread queues drained by one owner, and partitioned data eliminate contention at the design level — far more effective than tuning memory orders on a contended cache line.
