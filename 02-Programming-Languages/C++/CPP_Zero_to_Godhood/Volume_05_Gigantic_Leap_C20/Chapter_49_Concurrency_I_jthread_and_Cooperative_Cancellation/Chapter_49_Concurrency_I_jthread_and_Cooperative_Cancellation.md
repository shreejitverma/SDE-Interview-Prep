# Chapter 49: Concurrency I — `jthread` and Cooperative Cancellation

> *`std::jthread` fixes the two most dangerous defaults of `std::thread`: it joins automatically in its destructor (no more `std::terminate` from a forgotten `join`), and it carries a built-in cooperative cancellation mechanism via `std::stop_token`. Together with `std::stop_source` and `std::stop_callback`, C++20 finally gives the standard library a structured, RAII-clean way to start, stop, and wait for threads. This chapter covers automatic joining, the stop-token cancellation protocol, and the callback mechanism.*

`std::thread` has a notorious trap: if a joinable thread is destroyed without `join()` or `detach()`, the program calls `std::terminate()`. Every `std::thread` therefore needs careful RAII wrapping or scrupulous discipline, and the language offered no standard way to *ask* a thread to stop — you rolled your own `std::atomic<bool> done` flag every time. `std::jthread` (the "j" is for *joining*) solves both: its destructor requests a stop and then joins, and the stop request flows through a standard `std::stop_token` that the thread function polls. This is the foundation of structured concurrency in C++20.

---

## Table of Contents

- [49.1 The std::thread Problem jthread Solves](#491-the-stdthread-problem-jthread-solves)
- [49.2 Automatic Joining: RAII for Threads](#492-automatic-joining-raii-for-threads)
- [49.3 The Cancellation Trio: stop_token, stop_source, stop_callback](#493-the-cancellation-trio-stop_token-stop_source-stop_callback)
- [49.4 Writing a Cancellable Thread Function](#494-writing-a-cancellable-thread-function)
- [49.5 stop_source: Requesting a Stop from Outside](#495-stop_source-requesting-a-stop-from-outside)
- [49.6 stop_callback: Reacting to Cancellation](#496-stop_callback-reacting-to-cancellation)
- [49.7 Interrupting a Blocking Wait](#497-interrupting-a-blocking-wait)
- [49.8 Professional Insights](#498-professional-insights)

---

## 49.1 The std::thread Problem jthread Solves

`std::jthread` (header `<thread>`) is a drop-in improvement over `std::thread` with two added behaviors: **automatic join** on destruction and **integrated cooperative cancellation**.

```cpp
// Listing 49.1: the std::thread footgun vs jthread
#include <thread>

void with_plain_thread() {
    std::thread t{[]{ /* work */ }};
    // If we return here WITHOUT t.join() or t.detach(),
    // ~thread() calls std::terminate() — the program crashes.
    t.join();   // mandatory, easy to forget
}

void with_jthread() {
    std::jthread t{[]{ /* work */ }};
    // ~jthread() automatically requests stop and joins. Nothing to remember.
}   // clean, exception-safe, no terminate()
```

The plain `std::thread` requires an explicit `join()` (or `detach()`) on every path out of the scope, including exceptional ones, or the program terminates. `std::jthread`'s destructor does the right thing automatically, making it exception-safe by construction and eliminating the most common threading bug in C++.

---

## 49.2 Automatic Joining: RAII for Threads

`jthread`'s destructor calls `request_stop()` then `join()`. This makes a `jthread` a proper RAII handle: the thread's lifetime is bounded by the object's scope, and stack unwinding during an exception cleanly stops and joins it.

```cpp
// Listing 49.2: scope-bounded thread lifetime
#include <thread>
#include <vector>

void process(const std::vector<int>& data) {
    std::jthread worker{[&]{
        for (int x : data) { /* ... */ }
    }};

    if (data.empty())
        return;            // ~jthread joins here — no leak, no terminate

    // ... more work on the main thread ...
}                          // ~jthread joins here on the normal path too
```

Because the destructor both requests stop and joins, an early `return` or a thrown exception cannot leave the worker running or crash the process. This is the same guarantee `std::lock_guard` gives for mutexes and `unique_ptr` gives for memory, finally extended to threads — RAII all the way down.

---

## 49.3 The Cancellation Trio: stop_token, stop_source, stop_callback

C++20 cooperative cancellation rests on three cooperating types in `<stop_token>`. They share a single atomic stop-state.

| Type | Role |
|------|------|
| `std::stop_source` | The *requester*. Calling `request_stop()` sets the shared stop-state. |
| `std::stop_token` | The *observer*. The thread polls `stop_requested()` to see if it should quit. |
| `std::stop_callback` | A *reaction*. A callable invoked automatically when a stop is requested. |

```cpp
// Listing 49.3: how the three types share one stop-state
#include <stop_token>

std::stop_source source;                    // owns the stop-state
std::stop_token  token = source.get_token(); // a view onto that state

bool before = token.stop_requested();        // false
source.request_stop();                       // sets the shared flag
bool after  = token.stop_requested();        // true — observed through the token
```

A `stop_source` and all `stop_token`s derived from it refer to the same shared, thread-safe stop-state. One side requests the stop; the other side observes it. `jthread` wires this up for you — it owns an internal `stop_source` and passes a `stop_token` to your thread function — but the types are usable standalone for any cancellation scenario.

---

## 49.4 Writing a Cancellable Thread Function

If your callable accepts a `std::stop_token` as its **first parameter**, `jthread` passes its internal token automatically. The function polls `stop_requested()` in its loop and exits cleanly when a stop is requested.

```cpp
// Listing 49.4: a cooperatively cancellable worker
#include <thread>
#include <stop_token>
#include <chrono>
#include <iostream>

void worker(std::stop_token st, int id) {     // stop_token MUST be the first param
    while (!st.stop_requested()) {            // poll the cancellation flag
        // ... do a unit of work ...
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "worker " << id << " stopping cleanly\n";
}

int main() {
    std::jthread t{worker, 42};   // jthread injects the stop_token as arg 0
    std::this_thread::sleep_for(std::chrono::seconds(1));
    // No explicit stop needed: ~jthread() calls request_stop() then join().
}
```

The contract is purely cooperative: there is **no forced thread termination** in C++ (and there never will be — killing a thread mid-operation cannot run destructors or release locks safely). The thread must voluntarily check `stop_requested()` at safe points. Make those checkpoints frequent enough that cancellation is responsive but not so frequent that polling dominates the work.

---

## 49.5 stop_source: Requesting a Stop from Outside

You can request a stop explicitly — from another thread, a signal handler's flagged state, or a controlling object — via the `jthread`'s `request_stop()` or a standalone `stop_source`.

```cpp
// Listing 49.5: explicit and external stop requests
#include <thread>
#include <stop_token>

std::jthread t{[](std::stop_token st){
    while (!st.stop_requested()) { /* work */ }
}};

// Option A: ask this specific jthread to stop.
t.request_stop();                 // returns immediately; thread stops at next poll

// Option B: share one stop_source across several threads (fan-out cancellation).
std::stop_source group;
std::jthread a{[](std::stop_token s){ while (!s.stop_requested()){} }, };
// Pass group.get_token() to many workers so ONE request_stop() stops them all:
auto tok = group.get_token();
std::jthread b{[tok]{ while (!tok.stop_requested()){} }};
group.request_stop();             // stops every worker holding a token from 'group'
```

`request_stop()` is non-blocking — it sets the flag and returns; the actual stopping happens when each worker next polls. Sharing one `stop_source` across many workers gives **fan-out cancellation**: a single `request_stop()` signals an entire pool. `request_stop()` is idempotent and thread-safe, and returns whether *this* call was the one that performed the transition.

---

## 49.6 stop_callback: Reacting to Cancellation

`std::stop_callback` registers a callable that runs **automatically** when a stop is requested on its token — useful for waking a blocked thread, closing a socket, or signalling a condition variable, without polling.

```cpp
// Listing 49.6: running code the instant a stop is requested
#include <stop_token>
#include <atomic>

void register_reaction(std::stop_token st, std::atomic_flag& wake) {
    std::stop_callback cb{st, [&wake]{
        wake.test_and_set();
        wake.notify_one();        // wake a thread blocked on this flag
    }};
    // The lambda fires either:
    //   - immediately, if a stop was ALREADY requested when cb was constructed, or
    //   - the moment request_stop() is later called.
    // ... cb stays registered for its lifetime; its destructor deregisters it ...
}
```

A `stop_callback` runs its callable on the thread that calls `request_stop()` (or on the constructing thread if a stop was already pending). Its destructor unregisters the callback, and if a stop is in progress the destructor blocks until the callback finishes — preventing the classic use-after-free where the callback outlives the data it captures. This is the push-based counterpart to polling `stop_requested()`.

---

## 49.7 Interrupting a Blocking Wait

The most valuable use of `stop_callback` is making a blocking wait cancellable. `std::condition_variable_any::wait` has a C++20 overload that takes a `stop_token` and returns when either the predicate is satisfied or a stop is requested.

```cpp
// Listing 49.7: a condition-variable wait that honors cancellation
#include <thread>
#include <stop_token>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class BlockingQueue {
    std::mutex m_;
    std::condition_variable_any cv_;
    std::queue<T> q_;
public:
    // Returns false if a stop was requested while waiting.
    bool pop(std::stop_token st, T& out) {
        std::unique_lock lock{m_};
        // This overload wakes on notify OR on st's stop request.
        if (!cv_.wait(lock, st, [&]{ return !q_.empty(); }))
            return false;                  // stop requested -> bail out
        out = std::move(q_.front());
        q_.pop();
        return true;
    }
};
```

The `wait(lock, stop_token, predicate)` overload internally registers a `stop_callback` that notifies the condition variable, so a `request_stop()` wakes the waiter immediately instead of leaving it blocked until the next `notify`. Note it requires `condition_variable_any` (not plain `condition_variable`), because the stop-token integration needs the more general locking interface. This is how you build worker threads that block efficiently yet still shut down promptly.

---

## 49.8 Professional Insights

**Default to `std::jthread` for every new thread; reserve `std::thread` for interop only.** The automatic join eliminates the single most common threading crash — a forgotten `join()` calling `std::terminate()` on an exceptional path — and the integrated stop-token removes the boilerplate `std::atomic<bool>` flag you would otherwise write by hand. There is essentially no reason to start a raw `std::thread` in new code; `jthread` is strictly safer and carries no overhead you would not have added yourself.

**Cancellation is cooperative — design responsive, frequent checkpoints.** There is no safe way to forcibly kill a thread (it would skip destructors and leak locks), so a `jthread` only stops when its function polls `stop_requested()` or waits on a stop-aware primitive. Place checkpoints at natural unit-of-work boundaries: frequent enough that `request_stop()` takes effect promptly, coarse enough that polling does not dominate. A long-running, unchecked loop inside a `jthread` will hang the destructor's join just as a `std::thread` would.

**Make blocking waits stop-aware with the `condition_variable_any` + `stop_token` overload.** A worker that blocks on a queue or condition must use `cv.wait(lock, stop_token, pred)` (requiring `condition_variable_any`), or it will sleep through a cancellation request and stall shutdown. This overload registers an internal `stop_callback` that wakes the waiter on `request_stop()`, giving you both efficient blocking and prompt cancellation — the combination that naive `atomic<bool>` polling cannot achieve without busy-waiting.

**Use one shared `stop_source` for fan-out cancellation of a thread pool.** Handing the same `stop_source`'s token to many workers lets a single `request_stop()` shut down the entire group atomically and race-free, which is far cleaner than signalling each thread individually. This is the building block of structured shutdown: a controller owns the `stop_source`, the pool observes tokens, and teardown is one call.

**Rely on `stop_callback`'s destructor semantics for safe cleanup.** Because a `stop_callback`'s destructor deregisters the callback and blocks until any in-progress invocation completes, you can safely capture local state in the callback as long as the `stop_callback` object outlives that state's scope. This prevents the use-after-free that ad-hoc cancellation callbacks invite, but it also means a callback that blocks can stall the thread calling `request_stop()` — keep callbacks short and non-blocking (signal, don't compute).
