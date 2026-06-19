# Chapter 34: Standard Library Additions

# C++20 STANDARD LIBRARY EXPANSION

### 1. The Powerhouses

*   **std::format**: Type-safe, compile-time-checked Python-style string formatting.
```cpp
    std::string s = std::format("Hello {} {}", "C++", 20);
```

*   **std::span**: Lightweight non-owning view over contiguous memory (arrays, vectors).
```cpp
    void f(std::span<int> s) { for(auto x : s) { /* ... */ } }
```
*   **std::jthread**: A joining thread that automatically `join()`s on destruction and supports cooperative cancellation.
```cpp
    std::jthread t([](std::stop_token s){ while(!s.stop_requested()){} });
```
*   **std::stop_token / stop_source**: Mechanism for cooperative thread cancellation.

### 2. Synchronization Primitives

*   **std::latch**: Single-use countdown synchronization.
*   **std::barrier**: Reusable synchronization point for multiple threads.
*   **std::semaphore**: `std::counting_semaphore` and `std::binary_semaphore`.
*   **std::atomic_ref**: Temporarily apply atomic operations to non-atomic objects.
*   **Atomic smart pointers**: `std::atomic<std::shared_ptr<T>>` is now fully supported.

### 3. Modern Utilities

*   **std::source_location**: Capture call-site info (file, line, function) without macros.
*   **std::bit_cast**: Reinterpret bit representations safely (and in `constexpr`).
```cpp
    float f = std::bit_cast<float>(0x3F800000u);
```

*   **std::endian**: Compile-time byte order check.
*   **std::is_constant_evaluated()**: Detect if code is running at compile vs run time.
*   **std::ssize**: Signed version of `size()`.
*   **std::numbers**: Math constants like `pi`, `e`, `sqrt2` in `<numbers>`.

### 4. Container & String Upgrades

*   ** associative::contains**: `map` and `set` now have a `.contains(key)` member.
*   **string::starts_with / ends_with**: For `std::string` and `std::string_view`.
*   **std::erase / std::erase_if**: Free function versions of the erase-remove idiom.
```cpp
    std::erase_if(v, [](int x){ return x < 0; });
```
*   **std::midpoint / std::lerp**: Numerically correct math.
*   **std::make_shared for arrays**: `auto p = std::make_shared<int[]>(10);`

# VOLUME 05: GODHOOD SUMMARY

### C++20 LANDMARK FEATURES REFERENCE


| # | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 1 | **Concepts** | Formal constraints on template arguments with readable errors | `template<integral T>` |
| 2 | **Modules** | Binary semantic inclusion replacing textual headers | `import std.core;` |
| 3 | **Coroutines** | Stackless state machines for async programming | `co_await`, `co_yield`, `co_return` |
| 4 | **Ranges** | Composable, lazy-evaluated container operations | `v | views::filter(even) | views::transform(sq);` |
| 5 | **Spaceship Operator**| Three-way comparison (`<=>`) for auto-generating operators | `auto operator<=>(const T&) = default;` |
| 6 | **std::span** | Non-owning view over contiguous memory (array/vector) | `void f(std::span<int> s);` |
| 7 | **std::format** | Type-safe, high-performance string formatting | `std::format("Hello {}!", name);` |
| 8 | **std::jthread** | Joining thread with cooperative interruption | `std::jthread t([](std::stop_token s){});` |
| 9 | **consteval / constinit** | Immediate functions (compile-time only) and static init | `consteval int sq(int n);` |
| 10 | **Designated Init** | C-style aggregate initialization for readability | `Point p = {.x = 10, .y = 20};` |


C++20 was the **Gigantic Leap**. It is as significant as C++11 was a decade prior, introducing four "Great Pillars" that redefine how we write C++.
1. **Concepts**: Type-safe templates with readable errors.
2. **Modules**: The end of the "Header/Source" and `#include` era.
3. **Coroutines**: Native support for asynchronous programming and generators.
4. **Ranges**: Composable, functional-style container operations.

**The Golden Rule of C++20**: Constraints over SFINAE, Modules over Headers, and Ranges over Iterators. You have leaped into a new era of C++ architecture.
