# CHAPTER 37: C23 LIBRARY UTILITIES


# C++23 LIBRARY UTILITIES

### 1. Hardware Sympathy
*   **`std::unreachable`**: Tells the optimizer that a specific branch of code can never be reached. If it is reached, it is Undefined Behavior. This allows the compiler to strip out safety checks.
    ```cpp
    enum class State { A, B };
    void f(State s) {
        if (s == State::A) do_a();
        else if (s == State::B) do_b();
        else std::unreachable(); // Compiler optimizes knowing this is impossible
    }
    ```
*   **`std::byteswap`**: Highly optimized byte reversal (endianness swap), mapping directly to compiler intrinsics like `bswap`.

### 2. Core Utilities
*   **`std::to_underlying`**: A safe, clean way to extract the numeric value of an `enum class`.
    ```cpp
    enum class Flags : uint8_t { Read = 1, Write = 2 };
    auto val = std::to_underlying(Flags::Write); // uint8_t 2
    ```
*   **`std::move_only_function`**: A lightweight version of `std::function` that can hold non-copyable callables (like lambdas capturing `std::unique_ptr`). It has significantly less overhead.
*   **`std::string::contains`**: Finally, a readable way to check for substrings without comparing against `std::string::npos`.
    ```cpp
    if (str.contains("error")) { /* ... */ }
    ```
