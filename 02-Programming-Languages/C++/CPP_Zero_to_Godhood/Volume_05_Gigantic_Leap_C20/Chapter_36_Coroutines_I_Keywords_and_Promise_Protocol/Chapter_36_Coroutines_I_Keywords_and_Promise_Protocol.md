# Chapter 36: Coroutines I — co_await, co_yield, co_return, and the Promise Protocol

> *Coroutines are the third pillar and the most unusual: C++20 ships the low-level language machinery — three keywords and a customization protocol — but almost none of the high-level types you would actually use (`std::generator` and a `task` type are C++23 and beyond). This chapter explains what a coroutine is, the three keywords that turn a function into one, the compiler-generated state machine and coroutine frame, and the promise-type protocol you must implement to make a coroutine return something useful.*

A coroutine is a function that can **suspend** its execution, hand control back to its caller, and later be **resumed** from exactly where it left off, with all its locals intact. This makes asynchronous code read like synchronous code and makes generators read like simple loops. The price is that C++20 gives you the engine, not the car: to use coroutines you implement a **promise type** that tells the compiler how the coroutine behaves at each lifecycle point. Chapter 37 builds complete generator and task types on this foundation.

---

## Table of Contents

- [36.1 What a Coroutine Is](#361-what-a-coroutine-is)
- [36.2 The Three Keywords](#362-the-three-keywords)
- [36.3 The Coroutine Frame and the Generated State Machine](#363-the-coroutine-frame-and-the-generated-state-machine)
- [36.4 The Promise Type Protocol](#364-the-promise-type-protocol)
- [36.5 The Coroutine Lifecycle, Step by Step](#365-the-coroutine-lifecycle-step-by-step)
- [36.6 initial_suspend and final_suspend](#366-initial_suspend-and-final_suspend)
- [36.7 A Complete Minimal Generator](#367-a-complete-minimal-generator)
- [36.8 Exceptions and unhandled_exception](#368-exceptions-and-unhandled_exception)
- [36.9 Professional Insights](#369-professional-insights)

---

## 36.1 What a Coroutine Is

An ordinary function runs to completion (or throws) once called; its stack frame is destroyed on return. A **coroutine** can pause partway through, returning control to whoever resumed it, and be continued later. C++20 coroutines are **stackless**: they do not keep a separate call stack: instead the compiler transforms the function into a state machine whose state (locals, the resume point, the promise) lives in a single heap-allocated block called the **coroutine frame**.

The two dominant uses:

- **Generators** — a function that `co_yield`s a sequence of values one at a time, computing each only when the consumer asks. The Python `yield` model.
- **Tasks / async** — a function that `co_await`s asynchronous operations (I/O, timers) and suspends without blocking a thread, resuming when the operation completes.

Both look like straight-line code, which is the entire point: the suspension/resumption bookkeeping that you would otherwise hand-write as a state machine is generated for you.

---

## 36.2 The Three Keywords

A function becomes a coroutine if its body uses **any** of these three keywords. There is no separate "coroutine" declaration — the keywords are the trigger.

| Keyword | Meaning |
|---------|---------|
| `co_await expr` | suspend until `expr` (an *awaitable*) is ready; possibly yield control to the caller |
| `co_yield expr` | suspend and produce a value to the consumer (sugar for `co_await promise.yield_value(expr)`) |
| `co_return expr` | finish the coroutine, delivering a final value (or `co_return;` for none) |

```cpp
// Listing 36.1: each keyword makes the enclosing function a coroutine
#include <coroutine>

Generator<int> counter();        // uses co_yield  -> coroutine
Task<int>      fetch();          // uses co_await/co_return -> coroutine
Task<void>     fire_and_forget();// uses co_return; -> coroutine
```

A crucial constraint: a coroutine **cannot** use a plain `return` statement, cannot be `constexpr`/`consteval`, cannot be `main`, and cannot be a variadic function. The return type is not deduced from the keywords — it is a type *you* provide, and it must wire up to a promise type (Section 36.4).

---

## 36.3 The Coroutine Frame and the Generated State Machine

When you call a coroutine, the compiler:

1. **Allocates the coroutine frame** (normally on the heap via `operator new`; the allocation may be elided if the compiler can prove the frame does not outlive the caller — the *Halo* optimization).
2. **Copies the parameters** into the frame (by value — a common dangling pitfall with reference parameters, see Section 36.9).
3. **Constructs the promise object** inside the frame.
4. Calls `promise.get_return_object()` to produce the **return value handed to the caller**.
5. Evaluates `co_await promise.initial_suspend()` to decide whether to start running or suspend immediately.

The frame holds: the promise, the copied parameters, all local variables that live across a suspension point, and an integer "resume index" encoding where to continue. A **`std::coroutine_handle<Promise>`** is the non-owning handle used to resume (`.resume()` / `operator()`), query completion (`.done()`), reach the promise (`.promise()`), and destroy the frame (`.destroy()`).

```cpp
// Listing 36.2: coroutine_handle is how non-coroutine code drives a coroutine
#include <coroutine>

template<typename Promise>
void drive(std::coroutine_handle<Promise> h) {
    while (!h.done())   // has the coroutine reached final_suspend?
        h.resume();     // continue from the last suspension point
    h.destroy();        // free the frame (only if the handle owns it)
}
```

---

## 36.4 The Promise Type Protocol

The compiler discovers the promise type via `std::coroutine_traits<ReturnType, Args...>::promise_type` — by default the nested `ReturnType::promise_type`. Your return type therefore must contain (or designate) a **promise type** implementing a fixed set of member functions the compiler calls at well-defined points. This is the customization seam: the promise *is* the coroutine's behavior.

The required (and optional) members:

| Promise member | When the compiler calls it | Purpose |
|----------------|----------------------------|---------|
| `get_return_object()` | once, at start | produce the object returned to the caller |
| `initial_suspend()` | once, before the body | return an awaitable: suspend or run immediately |
| `final_suspend() noexcept` | once, after the body finishes | return an awaitable: keep frame alive or end |
| `unhandled_exception()` | if the body throws | handle the escaping exception |
| `return_value(v)` | on `co_return v` | store the result (mutually exclusive with `return_void`) |
| `return_void()` | on `co_return;` or fall-off | for value-less coroutines |
| `yield_value(v)` | on `co_yield v` | store/transport the yielded value, return an awaitable |

A promise must provide exactly one of `return_value`/`return_void`. `final_suspend` **must be `noexcept`**.

---

## 36.5 The Coroutine Lifecycle, Step by Step

Putting the protocol in temporal order clarifies why each member exists:

1. **Call** → frame allocated, parameters copied, promise constructed.
2. `auto ret = promise.get_return_object();` → the handle/value the caller receives.
3. `co_await promise.initial_suspend();` → if it suspends (`suspend_always`), control returns to the caller now; if not (`suspend_never`), the body runs immediately.
4. **Body executes** until it hits a `co_await`/`co_yield` (suspend), or `co_return`/falls off the end (complete).
5. On a value: `promise.return_value(x)` / `promise.return_void()`.
6. On an exception escaping the body: `promise.unhandled_exception()`.
7. `co_await promise.final_suspend();` → typically `suspend_always`, so the frame stays alive for the caller to read results before calling `.destroy()`.

The consumer drives steps 4–7 by repeatedly calling `handle.resume()` and reading the promise between resumes.

---

## 36.6 initial_suspend and final_suspend

These two awaitables shape the coroutine's start and end behavior. C++20 provides two trivial awaitables for them: **`std::suspend_always`** (suspends) and **`std::suspend_never`** (does not).

```cpp
// Listing 36.3: the two standard trivial awaitables drive start/end policy
#include <coroutine>

// Lazy start (generator style): suspend before running, so the first value is
// produced only on the first resume().
std::suspend_always initial_suspend() noexcept { return {}; }

// Eager start (task style): begin executing immediately on call.
std::suspend_never  initial_suspend() noexcept { return {}; }

// final_suspend MUST be noexcept. suspend_always keeps the frame alive so the
// caller can retrieve the final value/exception before destroying it.
std::suspend_always final_suspend() noexcept { return {}; }
```

The choice at `initial_suspend` defines **lazy vs eager**: generators want `suspend_always` (compute on demand); tasks often want `suspend_never` or a custom awaitable that schedules the work. Returning `suspend_always` from `final_suspend` is the safe default for value-returning coroutines because it prevents the frame self-destructing before the caller reads the result.

---

## 36.7 A Complete Minimal Generator

Here is a full, compilable C++20 generator implemented from the raw machinery — the canonical illustration of the protocol. (Recall `std::generator` is C++23; this is what you write in C++20.)

```cpp
// Listing 36.4: a complete hand-written generator coroutine in C++20
#include <coroutine>
#include <iostream>
#include <utility>

template<typename T>
struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    struct promise_type {
        T current_value;

        Generator get_return_object() {
            return Generator{ handle_type::from_promise(*this) };
        }
        std::suspend_always initial_suspend() noexcept { return {}; }  // lazy
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) {                     // on co_yield
            current_value = std::move(value);
            return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    handle_type h;
    explicit Generator(handle_type handle) : h(handle) {}
    Generator(const Generator&) = delete;                  // owns the frame: move-only
    Generator(Generator&& o) noexcept : h(std::exchange(o.h, {})) {}
    ~Generator() { if (h) h.destroy(); }

    bool next() { h.resume(); return !h.done(); }          // advance one value
    const T& value() const { return h.promise().current_value; }
};

Generator<int> sequence(int start, int end) {
    for (int i = start; i < end; ++i)
        co_yield i;                 // suspend here, surfacing 'i' to the consumer
}

int main() {
    auto gen = sequence(0, 5);
    while (gen.next())
        std::cout << gen.value() << ' ';   // 0 1 2 3 4
}
```

Trace the flow: calling `sequence` allocates the frame and (because `initial_suspend` is `suspend_always`) returns immediately without running the loop. The first `next()` resumes into the loop, runs to `co_yield 0`, stores `0` in the promise, and suspends. Each subsequent `next()` resumes after the `co_yield`, advances the loop, and yields again, until the loop ends and the coroutine reaches `final_suspend` (so `h.done()` becomes true).

---

## 36.8 Exceptions and unhandled_exception

If an exception propagates out of the coroutine body, the compiler calls `promise.unhandled_exception()` (from within an implicit `catch`). What you do there defines the coroutine's error policy:

```cpp
// Listing 36.5: capturing the exception to rethrow it on the consumer side
#include <coroutine>
#include <exception>

struct promise_type_with_errors {
    std::exception_ptr error;
    void unhandled_exception() noexcept {
        error = std::current_exception();   // capture, deliver to consumer later
    }
    // ... consumer checks 'error' after the coroutine completes and rethrows:
    //     if (h.promise().error) std::rethrow_exception(h.promise().error);
};
```

The minimal generator above used `std::terminate()` for brevity, but production generator/task types capture the exception with `std::current_exception()` and rethrow it from the consumer-facing accessor, so the failure surfaces where the caller can handle it. Because `final_suspend` is `noexcept`, the frame is still intact when the consumer inspects the captured exception.

---

## 36.9 Professional Insights

**Understand that C++20 ships the engine, not the car.** There is no standard `generator`, `task`, `lazy`, or executor in C++20 — only the keywords, `coroutine_handle`, the trivial awaitables, and the promise protocol. Real projects either hand-roll the small set of types they need (as in Listing 36.4) or pull in a library (cppcoro, libcoro, Boost.Cobalt, Asio). Treat "use coroutines in C++20" as "implement or import the coroutine types," not "they're ready to go."

**Never capture a reference parameter across a suspension.** Coroutine parameters are copied into the frame, but a *reference* parameter copies the reference, not the referent. If the referent is a temporary, it is gone after the first suspension and the coroutine reads a dangling reference on resume. Pass by value into coroutines, or guarantee the referent outlives the coroutine. This is the single most common coroutine bug.

**Make `final_suspend` `noexcept` and usually `suspend_always`.** The standard requires `noexcept`; `suspend_always` keeps the frame alive so the consumer can read the result or the captured exception before destroying it. Returning `suspend_never` from `final_suspend` self-destructs the frame at completion and is correct only for fire-and-forget coroutines that own their own cleanup.

**Budget for the frame allocation in hot paths.** Each coroutine call may heap-allocate its frame. The *Halo* optimization (HALO: heap allocation elision) can remove it when the frame provably does not escape, but you cannot rely on it across translation units or through type-erased handles. For latency-critical coroutines, supply a custom `operator new`/`operator delete` on the promise (e.g., a pool or arena allocator) and profile to confirm the elision or the pool is actually engaged.

**Capture exceptions in the promise; do not let them call `std::terminate` in production.** `unhandled_exception` is your one chance to convert an escaping exception into a value the consumer can inspect. Store `std::current_exception()` and rethrow it from the consumer-facing accessor so coroutine failures behave like ordinary function failures rather than hard aborts.
