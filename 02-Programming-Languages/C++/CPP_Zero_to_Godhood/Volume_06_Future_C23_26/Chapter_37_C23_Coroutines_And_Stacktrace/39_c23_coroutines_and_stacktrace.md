# Chapter 39: Coroutines & Stacktrace

# C++23 COROUTINES & DIAGNOSTICS

### 1. `std::generator`

C++20 provided the core language support for coroutines, but no standard library types. C++23 introduces `std::generator`, a ready-to-use return type for synchronous coroutine generators that works seamlessly with Ranges.
```cpp
#include <generator>

std::generator<int> fibonacci() {
    int a = 0, b = 1;
    while (true) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

// Seamless integration with views
auto first_10 = fibonacci() | std::views::take(10);
```

### 2. `std::stacktrace`

Native support for capturing and printing call stacks, revolutionizing C++ debugging and error logging.
```cpp
#include <stacktrace>
#include <print>

void crash_handler() {
    std::println("Crash! Stacktrace:
{}", std::stacktrace::current());
}
```

