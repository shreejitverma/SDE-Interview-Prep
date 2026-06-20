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
```
