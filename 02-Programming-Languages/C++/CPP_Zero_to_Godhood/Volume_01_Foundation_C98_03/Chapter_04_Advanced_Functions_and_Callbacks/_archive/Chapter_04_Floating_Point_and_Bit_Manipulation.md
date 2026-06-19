# Chapter 4: Floating Point & Bit Manipulation

Understanding how data is stored at the bit level and how floating-point numbers approximate real values is essential for high-performance C++ engineering.

## 4.1 Floating Point Arithmetic

Floating point numbers represent approximations of their assigned values. This is due to the binary representation of base-10 decimals.

### 1. The Imprecision Pitfall
A common mistake is assuming floating point equality will work as expected:
```cpp
double a = 0.1;
double b = 0.2;
double c = 0.3;
if (a + b == c) {
    std::cout << "Exact match!" << std::endl;
} else {
    std::cout << "Imprecise!" << std::endl; // Usually prints this
}
```
In IEEE 754 standard (used by most C++ compilers), 0.1 and 0.2 cannot be represented exactly in binary, leading to a small rounding error when added.

### 2. Comparison Strategy
Never use `==` with floats. Use an "epsilon" (a small tolerance):
```cpp
#include <cmath>
#include <limits>

bool nearly_equal(double a, double b) {
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}
```

### 3. Special Values: NaN and Infinity
*   **`std::numeric_limits<double>::quiet_NaN()`**: Not a Number (e.g., 0/0).
*   **`std::numeric_limits<double>::infinity()`**: Infinity (e.g., 1/0).

---

## 4.2 Bitwise Mastery (Low-Level Optimization)

Bitwise operators manipulate individual bits. Essential for embedded systems, graphics, and cryptography.

### The Operators
*   `&` (AND): Both bits must be 1.
*   `|` (OR): At least one bit must be 1.
*   `^` (XOR): Bits must be different.
*   `~` (NOT): Flip all bits.
*   `<<` (Left Shift): Multiply by 2^N.
*   `>>` (Right Shift): Divide by 2^N.

### God-Tier Tricks
1.  **Check Odd/Even**: `(x & 1) == 0` (Even). Faster than `% 2`.
2.  **Multiply by 2**: `x << 1`.
3.  **Divide by 2**: `x >> 1`.
4.  **Clear Last Set Bit**: `x & (x - 1)`. Used to count set bits (Kernighan's Algorithm).
5.  **Check Power of 2**: `(x > 0) && ((x & (x - 1)) == 0)`.
6.  **Toggle Bit N**: `x ^= (1 << N)`.
7.  **Set Bit N**: `x |= (1 << N)`.
8.  **Clear Bit N**: `x &= ~(1 << N)`.

```cpp
// Fast Power of 2 check
bool isPowerOf2(int x) {
    return x && !(x & (x - 1));
}
```

---

## 4.3 Bit Fields

Bit fields allow you to specify the number of bits each member of a struct or class should occupy. This is crucial for matching hardware protocols or saving space.

```cpp
struct HardwareRegister {
    unsigned int enable : 1;  // 1 bit
    unsigned int mode   : 3;  // 3 bits
    unsigned int value  : 4;  // 4 bits
};
```
**Professional Note**: The exact layout of bit fields is implementation-defined and depends on the platform's endianness and alignment rules.

---
### Professional Notes: Bits & Floats

#### 1. Bit Manipulation Hacks
*   **Swapping without temp**: `a ^= b; b ^= a; a ^= b;` (Warning: Slower than a temp variable on modern CPUs due to pipeline stalls).
*   **Absolute value without branching**: `(x + (x >> 31)) ^ (x >> 31)` for 32-bit signed integers.

#### 2. Floating Point Models
*   **`fast-math`**: A compiler flag (e.g., `-ffast-math` in GCC) that allows the compiler to ignore some IEEE 754 rules for speed, potentially breaking precision.
*   **`long double`**: On many systems, this provides 80-bit or 128-bit precision for high-precision scientific calculations.

---
