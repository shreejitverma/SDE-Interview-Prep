# CHAPTER 25: C17 STANDARD LIBRARY ADDITIONS


# C++17 STANDARD LIBRARY ADDITIONS

## 1. `std::byte`

A distinct type for byte-oriented memory access. Unlike `char`, it is not an arithmetic type (prevents accidental math).

```cpp
#include <cstddef>

std::byte b = std::byte{0xAB};
// b += 1; // Error
int i = std::to_integer<int>(b);
```

## 2. Algorithms

*   `std::sample`: Selects n random elements from a range.
*   `std::clamp`: Clamps a value between lo and hi.
*   `std::gcd`, `std::lcm`: Math utilities.

```cpp
int x = std::clamp(10, 0, 5); // 5
int g = std::gcd(12, 18);     // 6
```

## 3. Mathematical Special Functions

Support for Laguerre polynomials, Bessel functions, elliptic integrals, etc., in `<cmath>`.

## 4. Elementary String Conversions (`<charconv>`)

Low-level, allocation-free, locale-independent string conversions. Extremely fast.

```cpp
#include <charconv>

char buffer[10];
int value = 42;

// To chars
std::to_chars(buffer, buffer + 10, value);

// From chars
std::from_chars(buffer, buffer + 10, value);
# VOLUME 04: GODHOOD SUMMARY

### C++17 LANDMARK FEATURES REFERENCE
| # | Feature | Explanation | Code Example |
| :--- | :--- | :--- | :--- |
| 1 | **Structured Bindings** | Unpack tuples, pairs, and structs into named variables | `auto [x, y] = my_pair;` |
| 2 | **if constexpr** | Compile-time conditional branching in templates | `if constexpr (is_int_v<T>)` |
| 3 | **Init-statements** | `if` and `switch` can now initialize local variables | `if (auto it = m.find(k); it != m.end())` |
| 4 | **Fold Expressions** | Simplify variadic template unpacking with operators | `(... + args)` |
| 5 | **CTAD** | Class Template Argument Deduction from constructors | `std::vector v = {1, 2, 3};` |
| 6 | **Inline Variables** | Define static data members in headers without ODR issues | `static inline int val = 5;` |
| 7 | **std::string_view** | Non-owning, zero-allocation string reference | `void f(std::string_view sv);` |
| 8 | **std::optional** | Type-safe representation of a maybe-present value | `std::optional<int> res;` |
| 9 | **std::variant** | Type-safe union (discriminated union) | `std::variant<int, float> v;` |
| 10 | **std::any** | Type-safe container for any single value | `std::any a = 42;` |
| 11 | **std::filesystem** | Standard library for file and directory manipulation | `fs::exists("path");` |
| 12 | **Parallel Algos** | Standard algorithms with execution policies | `std::sort(std::execution::par, v.begin(), v.end());` |
| 13 | **std::scoped_lock** | Deadlock-avoiding multi-mutex RAII lock | `std::scoped_lock lk(m1, m2);` |
| 14 | **Guaranteed Copy Elision** | Compiler MUST omit copies in specific return scenarios | `T f() { return T(); }` |
| 15 | **std::byte** | Distinct type for raw memory bits | `std::byte b{0xFF};` |


C++17 was the release of **Simplification and Vocabulary**. It focused on making the language cleaner and providing standard types for common patterns.
1. **Structured Bindings**: Unpacking tuples and structs became trivial.
2. **if constexpr**: Compile-time branching simplified template metaprogramming.
3. **Vocabulary Types**: `std::optional`, `std::variant`, and `std::any` replaced unsafe C-style patterns.
4. **Filesystem**: Finally, a standard way to talk to the OS about files.

**The Golden Rule of C++17**: Use `std::optional` instead of null pointers, and `string_view` for efficient string passing. You have simplified the vocabulary of your code.

# VOLUME 05 GIGANTIC LEAP C20
