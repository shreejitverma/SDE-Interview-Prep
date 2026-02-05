# C++23 COROUTINES & STACKTRACE

## 1. `std::generator`

Standardized generator coroutine. Supports `co_yield` and recursive usage.

### 1.1 Basic Generator

```cpp
#include <generator>
#include <ranges>

std::generator<int> seq(int start) {
    while (true) {
        co_yield start++;
    }
}

int main() {
    for (int i : seq(0) | std::views::take(5)) {
        std::println("{}", i);
    }
}
```

### 1.2 Recursive Generator

`std::generator` supports `co_yield ranges::elements_of(...)` to yield values from a sub-generator or range efficiently.

## 2. `std::stacktrace`

Obtain the call stack at runtime. Useful for logging and debugging.

```cpp
#include <stacktrace>
#include <print>

void boom() {
    auto trace = std::stacktrace::current();
    std::println("{}", std::to_string(trace));
}
```

**Note:** Requires compiler/linker flags (e.g., `-lstdc++_libbacktrace` on GCC).
