# Chapter 31: Coroutines (Stackless State Machines)

# C++20 COROUTINES

## 1. Asynchronous Programming

Coroutines are functions that can suspend execution to be resumed later. They enable writing async code that looks synchronous.

### 1.1 Keywords
*   `co_await`: Suspend execution until an awaited operation completes.
*   `co_yield`: Suspend execution and return a value (generator).
*   `co_return`: Complete execution and return a final value.

A function containing any of these is a coroutine.

## 2. Generators (The Python Style)

(Note: `std::generator` arrived in C++23, but the machinery exists in C++20).

```cpp
#include <coroutine>
#include <iostream>

struct Generator {
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    handle_type h;

    Generator(handle_type h) : h(h) {}
    ~Generator() { if (h) h.destroy(); }

    struct promise_type {
        int current_value;
        Generator get_return_object() { return Generator{handle_type::from_promise(*this)}; }
        std::suspend_always initial_suspend() { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(int value) {
            current_value = value; return {};
        }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    bool next() { h.resume(); return !h.done(); }
    int value() const { return h.promise().current_value; }
};

Generator sequence(int start, int end) {
    for (int i = start; i < end; ++i) {
        co_yield i; // Suspend here
    }
}

int main() {
    auto gen = sequence(0, 5);
    while (gen.next()) {
        std::cout << gen.value() << " ";
    }
}
```

## 3. Tasks (`co_await`)

Used for async I/O. (Requires a library like `cppcoro` or custom implementation in C++20).

```cpp
Task<int> fetch_data() {
    auto conn = co_await connect("db.local");
    auto result = co_await conn.query("SELECT * FROM users");
    co_return result.size();
}
```

## 4. Under the Hood

The compiler generates a state machine for the coroutine, allocating the "coroutine frame" (local variables, promise object, instruction pointer) on the heap (usually).

