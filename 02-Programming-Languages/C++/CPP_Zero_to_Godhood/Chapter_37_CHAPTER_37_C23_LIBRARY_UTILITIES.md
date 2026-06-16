# CHAPTER 37: C23 LIBRARY UTILITIES


# C++23 LIBRARY UTILITIES

### 1. Hardware & Memory
*   **std::unreachable()**: Marks code that should never be reached. Gives the compiler optimization permission and causes UB if reached.
    ```cpp
    default: std::unreachable();
    ```
*   **std::byteswap**: Reverses the byte order of an integral value; useful for endianness conversion.
    ```cpp
    uint32_t be = std::byteswap(0x01020304u);
    ```
*   **std::out_ptr / std::inout_ptr**: Helpers for passing smart pointers to legacy C APIs that expect `T**` output parameters.
    ```cpp
    legacy_init(std::out_ptr(my_unique_ptr));
    ```
*   **std::spanstream**: A string stream that operates on a fixed `std::span<char>` buffer rather than allocating heap memory (faster than `stringstream`).
    ```cpp
    std::spanstream ss{buf}; ss << 42;
    ```

### 2. Functional Utilities
*   **std::move_only_function**: Like `std::function` but only move-constructible; supports move-only callables (lambdas capturing `unique_ptr`) and avoids unnecessary copies.
*   **std::to_underlying**: Converts an enum to its underlying integer type without a `static_cast`.
*   **string::contains**: `std::string` and `std::string_view` gain `.contains()` to check for substring presence.

### 3. Math & Constexpr Upgrades
*   **Fixed-width floating-point types**: `<stdfloat>` introduces `std::float16_t`, `std::float32_t`, `std::float64_t`, and `std::bfloat16_t` (if supported by platform).
*   **constexpr upgrades**: `std::optional`, `std::variant`, `std::unique_ptr`, and many `<cmath>` functions (e.g., `abs`, `ceil`) are now fully `constexpr`.
