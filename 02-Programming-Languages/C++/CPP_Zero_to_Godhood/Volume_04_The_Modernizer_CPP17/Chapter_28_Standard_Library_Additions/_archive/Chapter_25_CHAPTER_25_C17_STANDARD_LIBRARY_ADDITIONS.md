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

C++20 is the most significant update to the language since C++11. It introduces the **Four Great Pillars** that fundamentally change how we architect C++ software.

### The Four Great Pillars (Head First Style)

| Pillar | Analogy | Why we need it |
| :--- | :--- | :--- |
| **Concepts** | **The Bouncer at the Club** | Before C++20, templates were "all are welcome." If you brought the wrong type, the compiler would wait until you were inside the club to scream at you. Concepts are like a bouncer at the door who checks your ID (type) before you even enter. |
| **Modules** | **Sealed Folders vs. Messy Desks** | `#include` is like dumping a giant pile of messy blueprints on your desk every time you want to build a small part. Modules are like sealed folders; you just grab exactly what you need without making a mess of your current workspace. |
| **Coroutines** | **The Expert Chef** | A normal function is like a chef who *must* finish a whole recipe before doing anything else. A Coroutine is a chef who can pause a recipe to wait for the oven to heat up, work on another dish, and then come back exactly where they left off. |
| **Ranges** | **The LEGO Pipe Factory** | Instead of manually moving items from one box to another using iterators, Ranges let you snap together "pipes" (filters, transforms) to create a high-speed data assembly line. |

---
