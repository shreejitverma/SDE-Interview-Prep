# Numeric, Mathematical, and Cryptographic Randomness


Python provides a robust suite of modules for numerical computing, ranging from standard floating-point math to arbitrary-precision decimals and cryptographically secure random number generation.

### 35.1 `decimal`: Control over Precision

The standard `float` in Python is a 64-bit IEEE 754 double, which suffers from precision issues (e.g., `0.1 + 0.2 != 0.3`). The `decimal` module provides a `Decimal` type for correctly-rounded decimal floating-point arithmetic.

#### 1. The `decNumber` C Library
In CPython, the `decimal` module is implemented as `_decimal`, which is a wrapper around the **decNumber** library. This allows for extremely fast decimal arithmetic that follows the General Decimal Arithmetic Specification.

#### 2. Contexts and Precision
You can control the global or local precision using `getcontext()`:
```python
from decimal import Decimal, getcontext
getcontext().prec = 50  # 50 digits of precision
print(Decimal(1) / Decimal(7))
```
*   **Performance Note**: While `_decimal` is fast, it is still significantly slower than hardware-native `float`. Use it for financial applications or cases where exact decimal representation is mandatory.

### 35.2 `fractions`: Exact Rational Numbers

The `fractions` module provides support for rational number arithmetic.
*   **Internals**: A `Fraction` object stores two integers: a numerator and a denominator. It automatically reduces the fraction to its lowest terms using the Greatest Common Divisor (GCD).
*   **Exactness**: Unlike `float` or `decimal`, `Fraction` can represent `1/3` exactly without any rounding error.

### 35.3 `math` and `cmath`: The C Standard Library Wrappers

*   **`math`**: Provides access to the mathematical functions defined by the C standard for real numbers (`sin`, `cos`, `log`, `sqrt`).
*   **`cmath`**: Provides the same functions for complex numbers.
*   **Optimization**: These functions are thin wrappers around the host C library. They are highly optimized and release the GIL for heavy calculations (though most are too fast for the release overhead to be worth it).

### 35.4 `random`: Pseudorandom Number Generation

The `random` module is a **Pseudorandom Number Generator (PRNG)**. It is deterministic if you know the seed.

#### 1. The Mersenne Twister (MT19937)
Historically, Python used the Mersenne Twister as its primary PRNG.
*   **Period**: $2^{19937} - 1$.
*   **State**: It maintains a large state (624 integers).
*   **Weakness**: It is not cryptographically secure; observing a sufficient number of outputs allows an attacker to predict future values.

#### 2. PCG64 (Python 3.13+)
Modern Python versions have introduced more modern PRNGs like PCG64, which offer better statistical properties and smaller state.

### 35.5 `secrets`: Cryptographic Security

For security-sensitive applications (passwords, tokens), you must use the `secrets` module.
*   **Internals**: `secrets` uses the OS's cryptographically secure source of randomness (`/dev/urandom` on Unix, `CryptGenRandom` on Windows).
*   **Why?**: Unlike `random`, the output of `secrets` is not predictable even if an attacker sees millions of previous values.

---


---

