# Chapter 37: Coroutines II — Awaitables, Tasks, coroutine_handle, and Symmetric Transfer

> *Chapter 36 built the generator: the `co_yield` side of coroutines, driven synchronously by a consumer pulling values. This chapter completes the picture with the `co_await` side: the awaitable protocol that `co_await` actually invokes, how to build a `Task<T>` that represents an asynchronous result, how coroutines resume one another without growing the stack via symmetric transfer, and the lifetime and ownership rules that keep `coroutine_handle` from becoming a use-after-free machine.*

`co_await` is not magic — it is a three-method protocol (`await_ready`, `await_suspend`, `await_resume`) that the compiler calls on whatever you await. Understanding that protocol is what lets you write `Task` types, integrate with event loops, chain coroutines, and avoid the stack-overflow and dangling-handle traps that make naive coroutine code dangerous. This chapter assumes the promise-type machinery of Chapter 36 and builds the asynchronous half on top of it.

---

## Table of Contents

- [37.1 The Awaitable Protocol](#371-the-awaitable-protocol)
- [37.2 await_suspend Return Types and What They Mean](#372-await_suspend-return-types-and-what-they-mean)
- [37.3 awaitable vs awaiter and operator co_await](#373-awaitable-vs-awaiter-and-operator-co_await)
- [37.4 Building a Task<T>](#374-building-a-taskt)
- [37.5 Symmetric Transfer and the Stack-Overflow Trap](#375-symmetric-transfer-and-the-stack-overflow-trap)
- [37.6 coroutine_handle: Ownership, Lifetime, and Type Erasure](#376-coroutine_handle-ownership-lifetime-and-type-erasure)
- [37.7 Integrating with an Event Loop](#377-integrating-with-an-event-loop)
- [37.8 What C++20 Lacks: generator, task, and Executors](#378-what-c20-lacks-generator-task-and-executors)
- [37.9 Professional Insights](#379-professional-insights)

---

## 37.1 The Awaitable Protocol

When the compiler sees `co_await expr`, it obtains an **awaiter** from `expr` and calls three methods on it, in order:

| Method | Returns | Role |
|--------|---------|------|
| `await_ready()` | `bool` | "Is the result already available?" If `true`, skip suspension entirely. |
| `await_suspend(handle)` | `void`/`bool`/`coroutine_handle` | Called *after* the coroutine has suspended; schedule the resumption. |
| `await_resume()` | `T` | Called on resume; its return value is the result of the whole `co_await` expression. |

```cpp
// Listing 37.1: the two trivial awaiters, written out, reveal the protocol
#include <coroutine>

struct always_suspend {                // == std::suspend_always
    bool await_ready()  const noexcept { return false; }  // never ready -> suspend
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};

struct never_suspend {                 // == std::suspend_never
    bool await_ready()  const noexcept { return true; }   // always ready -> no suspend
    void await_suspend(std::coroutine_handle<>) const noexcept {}
    void await_resume() const noexcept {}
};
```

The pattern is: `await_ready` is the fast-path optimization (skip the suspend machinery when the value is already there); `await_suspend` is where you stash the handle somewhere so something — an event loop, another thread, an I/O completion — can resume it later; `await_resume` produces the value `co_await` evaluates to (or rethrows a stored exception).

---

## 37.2 await_suspend Return Types and What They Mean

`await_suspend`'s return type selects one of three resumption behaviors. This is the most important and least-understood part of the protocol.

```cpp
// Listing 37.2: the three await_suspend signatures
#include <coroutine>

// (a) void  -> suspend and return control to the caller/resumer.
void await_suspend(std::coroutine_handle<> h);

// (b) bool  -> false means "actually, do not suspend; resume me immediately."
//              true means "stay suspended" (like the void form).
bool await_suspend(std::coroutine_handle<> h);

// (c) coroutine_handle -> SYMMETRIC TRANSFER: immediately resume the RETURNED
//     coroutine, with no nested call frame (a tail call). Return
//     std::noop_coroutine() to mean "suspend and return to the loop."
std::coroutine_handle<> await_suspend(std::coroutine_handle<> h);
```

The `bool` form is an optimization for "I checked and the result is ready after all — don't pay for suspension." The `coroutine_handle` form (c) is **symmetric transfer**, the key to chaining coroutines without overflowing the stack — covered in Section 37.5. Returning `std::noop_coroutine()` is the idiomatic "nothing to resume; go back to the scheduler."

---

## 37.3 awaitable vs awaiter and operator co_await

Two distinct terms, often conflated:

- An **awaitable** is anything you can write `co_await` on.
- An **awaiter** is the object with `await_ready`/`await_suspend`/`await_resume`.

The compiler turns an awaitable into an awaiter in this order: (1) if the promise has `await_transform`, call `promise.await_transform(expr)`; (2) if the result has `operator co_await`, call it; (3) otherwise the result must already be an awaiter.

```cpp
// Listing 37.3: operator co_await turns a value type into an awaiter
#include <coroutine>
#include <chrono>

struct sleep_for {                          // an awaitable value
    std::chrono::milliseconds dur;
};

struct sleep_awaiter {                      // its awaiter
    std::chrono::milliseconds dur;
    bool await_ready() const noexcept { return dur.count() <= 0; }
    void await_suspend(std::coroutine_handle<> h) const {
        // hand 'h' + 'dur' to a timer service that will resume(h) when it fires
    }
    void await_resume() const noexcept {}
};

sleep_awaiter operator co_await(sleep_for s) { return {s.dur}; }
// now: co_await sleep_for{100ms};
```

`await_transform` on the promise is the hook libraries use to (a) make *every* `co_await` in a coroutine go through a scheduler, or (b) **disable** `co_await` in a generator by declaring it deleted — a clean way to make `co_yield`-only coroutines reject `co_await`.

---

## 37.4 Building a Task<T>

A `Task<T>` represents a coroutine that runs asynchronously and eventually produces a `T`. It is lazy (starts suspended), stores its result or exception in the promise, and resumes whoever awaited it when it finishes (via symmetric transfer to a stored *continuation*).

```cpp
// Listing 37.4: a lazy Task<T> with a continuation, in pure C++20
#include <coroutine>
#include <exception>
#include <utility>
#include <variant>

template<typename T>
struct Task {
    struct promise_type {
        std::variant<std::monostate, T, std::exception_ptr> result;
        std::coroutine_handle<> continuation;        // who awaits this task

        Task get_return_object() {
            return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }   // lazy

        // On completion, symmetrically transfer back to the awaiting coroutine.
        struct final_awaiter {
            bool await_ready() const noexcept { return false; }
            std::coroutine_handle<> await_suspend(
                std::coroutine_handle<promise_type> h) noexcept {
                auto cont = h.promise().continuation;
                return cont ? cont : std::noop_coroutine();
            }
            void await_resume() const noexcept {}
        };
        final_awaiter final_suspend() noexcept { return {}; }

        void return_value(T v) { result.template emplace<1>(std::move(v)); }
        void unhandled_exception() {
            result.template emplace<2>(std::current_exception());
        }
    };

    std::coroutine_handle<promise_type> h;
    explicit Task(std::coroutine_handle<promise_type> handle) : h(handle) {}
    Task(Task&& o) noexcept : h(std::exchange(o.h, {})) {}
    ~Task() { if (h) h.destroy(); }

    // Make Task itself awaitable: awaiting it starts it and records the continuation.
    bool await_ready() const noexcept { return false; }
    std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
        h.promise().continuation = awaiting;   // remember who to resume
        return h;                              // symmetric transfer: start the task
    }
    T await_resume() {
        auto& r = h.promise().result;
        if (r.index() == 2) std::rethrow_exception(std::get<2>(r));
        return std::move(std::get<1>(r));
    }
};

Task<int> add(int a, int b) { co_return a + b; }
Task<int> compute() {
    int x = co_await add(2, 3);      // suspends compute, runs add, resumes with 5
    co_return x * 10;                // -> 50
}
```

The flow: `co_await add(2,3)` suspends `compute`, records `compute`'s handle as `add`'s continuation, and symmetric-transfers into `add`. When `add` hits `final_suspend`, its `final_awaiter` transfers control straight back to `compute` — no growing call stack, and `await_resume` extracts the `int` (or rethrows).

---

## 37.5 Symmetric Transfer and the Stack-Overflow Trap

Naive chaining resumes the next coroutine *from inside* `await_suspend`:

```cpp
// Listing 37.5: the WRONG way -- nested resume() grows the stack unboundedly
void await_suspend(std::coroutine_handle<> awaiting) {
    h.promise().continuation = awaiting;
    h.resume();          // BUG: resumes the task in a nested frame.
}                        // For a loop of awaits, each resume nests -> stack overflow.
```

Every `resume()` called from within `await_suspend` adds a stack frame. A coroutine that awaits another in a loop (or deep recursion of tasks) accumulates frames and eventually **overflows the stack**. **Symmetric transfer** fixes this: by *returning* the handle from `await_suspend` (form (c) in Section 37.2), the compiler performs a guaranteed **tail call** — the current frame is torn down *before* the next coroutine resumes, so stack depth stays constant no matter how long the chain.

```cpp
// Listing 37.6: the RIGHT way -- return the handle, compiler tail-calls it
std::coroutine_handle<> await_suspend(std::coroutine_handle<> awaiting) noexcept {
    h.promise().continuation = awaiting;
    return h;            // tail call: no net stack growth across the whole chain
}
```

This is why the `Task` in Listing 37.4 returns handles from both its own `await_suspend` and the promise's `final_awaiter`. **Symmetric transfer is mandatory for any task type that chains**; it is the single most important correctness property of an async coroutine library, and `std::noop_coroutine()` is the sentinel that terminates the chain by returning to the scheduler.

---

## 37.6 coroutine_handle: Ownership, Lifetime, and Type Erasure

`std::coroutine_handle` is a **non-owning** pointer to a coroutine frame. It does not manage lifetime; you must call `.destroy()` exactly once, and never resume a handle whose frame is already destroyed or already `done()`.

```cpp
// Listing 37.7: handle operations and the type-erased form
#include <coroutine>

template<typename P>
void inspect(std::coroutine_handle<P> h) {
    if (!h) return;                 // null check (default-constructed / moved-from)
    P& promise = h.promise();       // typed access to the promise
    bool finished = h.done();       // reached final_suspend?
    if (!finished) h.resume();      // == h()

    // Type-erase to coroutine_handle<void> for storage in containers/schedulers:
    std::coroutine_handle<> erased = h;
    // erased.promise() is NOT available -- the promise type was erased.
    // from_promise / address / from_address round-trip the raw pointer:
    void* raw = erased.address();
    auto  back = std::coroutine_handle<>::from_address(raw);
    (void)back;
}
```

Three rules prevent the common crashes: (1) **exactly one owner** calls `.destroy()` — typically the RAII wrapper (`Task`/`Generator`) in its destructor, which is why those types are move-only and null out the moved-from handle with `std::exchange`; (2) **never resume a `done()` coroutine** — it is undefined behavior; (3) **`coroutine_handle<>` (type-erased)** loses `.promise()` access but is what you store in heterogeneous scheduler queues, round-tripping through `.address()`/`from_address()`.

---

## 37.7 Integrating with an Event Loop

The whole point of `co_await` is non-blocking I/O: `await_suspend` hands the handle to an external service and returns; the service resumes the handle when the operation completes, on whatever thread it chooses.

```cpp
// Listing 37.8: an awaiter that parks the coroutine on an external completion
#include <coroutine>
#include <functional>

struct AsyncRead {
    int fd;
    char* buf;
    size_t len;

    bool await_ready() const noexcept { return false; }   // always async

    void await_suspend(std::coroutine_handle<> h) {
        // Register with the reactor: when fd is readable, perform the read and
        // then resume the coroutine. The lambda owns no frame; it only holds 'h'.
        reactor().on_readable(fd, [h, this]() mutable {
            ::read(fd, buf, len);
            h.resume();          // resume on the reactor's thread
        });
        // return void: control goes back to whoever was running the loop.
    }

    ssize_t await_resume() const noexcept { return len; }  // bytes read
};
// usage:  ssize_t n = co_await AsyncRead{fd, buffer, 4096};
```

Two correctness notes for production reactors: the resuming thread is the reactor's thread, so anything after the `co_await` runs there (mind data races and thread-affinity); and the coroutine frame must outlive the pending operation — if the `Task` owning the frame is destroyed while the read is in flight, the `h.resume()` is a use-after-free. Real frameworks tie frame lifetime to operation completion precisely to avoid this.

---

## 37.8 What C++20 Lacks: generator, task, and Executors

Everything in this chapter is hand-written for a reason: **C++20 standardizes no coroutine types**, only the language machinery.

| You want | Status | C++20 workaround |
|----------|--------|------------------|
| `std::generator<T>` | C++23 | hand-roll (Chapter 36, Listing 36.4) |
| a standard `task<T>` | not even C++23 | hand-roll (Listing 37.4) or use a library |
| executors / `std::execution` (senders/receivers) | C++26 | use Asio, libunifex, stdexec |
| `std::lazy`, `when_all`, `sync_wait` | library-only | cppcoro, libcoro, Boost.Cobalt |

For real asynchronous systems on C++20, the pragmatic path is a vetted library (Asio's coroutine support, cppcoro, libunifex) rather than re-deriving symmetric transfer and cancellation from scratch — the `Task` above is correct but minimal, lacking cancellation, allocator customization, and `when_all`-style composition.

---

## 37.9 Professional Insights

**Symmetric transfer is non-negotiable for chaining task types.** Any `Task`/`Lazy` that awaits other tasks must return a `coroutine_handle` from `await_suspend` (and from the promise's `final_awaiter`), never call `resume()` nested inside it. Get this wrong and the code works in unit tests and stack-overflows in production under deep await chains. This is the defining difference between a toy coroutine type and a usable one.

**Treat `coroutine_handle` like a raw pointer, because it is one.** It does not own the frame, does not null itself on destroy, and resuming a `done()` or destroyed handle is undefined behavior. Wrap it in exactly one move-only RAII type that calls `.destroy()` once, null the moved-from handle with `std::exchange`, and never let two owners exist. The majority of coroutine crashes are handle-lifetime bugs, not logic bugs.

**Anchor frame lifetime to operation completion in async code.** When `await_suspend` hands a handle to a reactor or thread pool, the frame must stay alive until that external party resumes it. Destroying the owning `Task` mid-flight turns the eventual `resume()` into a use-after-free. Design ownership so the in-flight operation keeps the frame alive (e.g., the operation holds the owning object, or the scheduler does).

**Use `await_transform` to enforce coroutine discipline.** Declaring `await_transform` deleted in a generator's promise makes `co_await` a compile error there, cleanly separating pull-based generators from async tasks. Conversely, a non-deleted `await_transform` routes every `co_await` through your scheduler — the standard hook for imposing thread-affinity or cancellation on all awaits in a coroutine.

**Reach for a library before hand-rolling async coroutines.** The generator in Chapter 36 is reasonable to own; a full task system with cancellation, `when_all`, `sync_wait`, and allocator control is not. C++20 ships no executor model (that is C++26's `std::execution`), so use Asio, libunifex, or cppcoro for real async work and reserve hand-written awaiters for narrow, well-understood integration points like Listing 37.8.
