# CHAPTER 37: C23 LIBRARY UTILITIES


# C++23 LIBRARY UTILITIES

## 1. `std::unreachable`

Marks code as unreachable. If executed, Undefined Behavior (allows optimization).

```cpp
#include <utility>

enum Color { Red, Green };

int get_value(Color c) {
    switch (c) {
        case Red: return 1;
        case Green: return 2;
    }
    std::unreachable();
}
```

## 2. `std::to_underlying`

Safely cast an `enum class` to its underlying integer type.

```cpp
enum class Flags : unsigned char { A = 1 };
auto val = std::to_underlying(Flags::A); // unsigned char
```

## 3. `std::byteswap`

Endianness reversal.

```cpp
#include <bit>
uint32_t x = 0x12345678;
uint32_t y = std::byteswap(x); // 0x78563412
```

## 4. `std::stdatomic.h`

C compatibility header for atomics.

## 5. `std::move_only_function`

A replacement for `std::function` that works with move-only callables (like lambdas capturing `unique_ptr`).

```cpp
#include <functional>

std::move_only_function<void()> f;
auto ptr = std::make_unique<int>(42);

f = [p = std::move(ptr)]() { /*...*/ }; // OK (std::function would fail)
---

## 6. Professional Notes: C++23 Deep Dives

### 6.1 Deducing `this`: The End of CRTP?
Explicit object parameters (deducing `this`) allow member functions to be written as templates, where the first parameter is the object instance itself.

*   **Recursive Lambdas**: Previously, a lambda couldn't easily call itself without `std::function` or a helper. Now:
    ```cpp
    auto fib = [](this auto self, int n) -> int {
        return n <= 1 ? n : self(n - 1) + self(n - 2);
    };
    ```
*   **Forwarding Overloads**: Instead of writing `const&` and `&&` overloads for a getter, you can write one:
    ```cpp
    template<typename Self>
    auto&& get_data(this Self&& self) {
        return std::forward<Self>(self).data;
    }
    ```

### 6.2 `std::expected`: Functional Error Handling
`std::expected<T, E>` is the definitive replacement for `std::optional` in cases where the *reason* for failure matters.

*   **Monadic Chaining**: C++23 adds `and_then`, `or_else`, and `transform` to both `expected` and `optional`.
    ```cpp
    auto result = fetch_user(id)
        .and_then(get_permissions)
        .and_then(validate_access)
        .or_else(handle_error);
    ```
    This eliminates the "Pyramid of Doom" (nested `if` checks).

### 6.3 `std::print`: Zero-Allocation I/O
While `std::format` returns a `std::string` (potentially allocating), `std::print` writes directly to the underlying file pointer (e.g., `stdout`).

*   **Internal Mechanics**: It uses the same formatting engine as `format` but bypasses the string creation step, making it as fast as `printf` but with the type-safety and Unicode support of modern C++.

# VOLUME 06: GODHOOD SUMMARY

### C++23 LANDMARK FEATURES REFERENCE
| # | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 1 | **Deducing this** | Explicit object parameters for simpler CRTP and recursive lambdas | `void f(this auto&& self);` |
| 2 | **std::expected** | Functional error handling representing value or error | `std::expected<int, Error> parse();` |
| 3 | **std::print / println** | Native type-safe replacement for `printf` | `std::println("Value: {}", x);` |
| 4 | **std::mdspan** | Multidimensional non-owning view (Matrices) | `std::mdspan m(data.data(), 3, 3);` |
| 5 | **std::flat_map** | Cache-friendly, vector-backed associative container | `std::flat_map<int, string> m;` |
| 6 | **if consteval** | Cleaner branch for compile-time vs run-time paths | `if consteval { /* compile-time */ }` |
| 7 | **std::generator** | Coroutine-based generator for Range-compatible sequences | `std::generator<int> fib();` |
| 8 | **std::stacktrace** | Native support for capturing and printing call stacks | `std::stacktrace::current();` |
| 9 | **std::unreachable** | Hint for optimizer that a branch is impossible | `if (cond) { ... } else { std::unreachable(); }` |


C++23 is the **Latest Evolution**, providing the "missing pieces" of C++20 and making the language even more ergonomic.
1. **Deducing `this`**: Simplified CRTP and reduced member function bloat.
2. **expected**: A standard way to handle errors with values.
3. **print/println**: Finally, a modern, type-safe replacement for `printf`.
4. **Multidimensional operator[]**: Paving the way for high-performance linear algebra.

**The Golden Rule of C++23**: Use `std::print` for I/O and `std::expected` for error-prone logic. You are now at the cutting edge of production C++.

# VOLUME 07 THE NEXT FRONTIER C26
