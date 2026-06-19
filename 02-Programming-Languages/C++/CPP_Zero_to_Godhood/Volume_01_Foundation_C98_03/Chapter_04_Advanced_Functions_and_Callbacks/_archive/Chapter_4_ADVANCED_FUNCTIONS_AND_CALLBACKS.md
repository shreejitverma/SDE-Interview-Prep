# ADVANCED FUNCTIONS AND CALLBACKS


# ADVANCED FUNCTIONS & CALLBACKS



<!-- Merged content from Chapter_3_ADVANCED_FUNCTIONS.md -->

# ADVANCED FUNCTIONS


## 2.1 Variadic Functions

```cpp
#include <iostream>
#include <cstdarg>
using namespace std;

// Function with variable number of arguments
int sum(int count, ...) {
    va_list args;
    va_start(args, count);
    
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);
    }
    
    va_end(args);
    return total;
}

int main() {
    cout << sum(3, 10, 20, 30) << endl;     // 60
    cout << sum(5, 1, 2, 3, 4, 5) << endl;  // 15
    
    return 0;
}
```

## 2.2 Function Recursion

```cpp
#include <iostream>
using namespace std;

// Factorial using recursion
int factorial(int n) {
    if (n <= 1) {
        return 1;  // Base case
    }
    return n * factorial(n - 1);  // Recursive case
}

// Fibonacci using recursion (inefficient)
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Fibonacci with memoization (efficient)
int fib_memo(int n, int memo[]) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];
    
    memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
    return memo[n];
}

int main() {
    cout << factorial(5) << endl;  // 120
    cout << fibonacci(10) << endl; // 55
    
    int memo[11];
    for (int i = 0; i < 11; i++) memo[i] = -1;
    cout << fib_memo(10, memo) << endl;  // 55
    
    return 0;
}
```

## 2.3 Inline Functions

```cpp
#include <iostream>
using namespace std;

// Inline function - compiler may expand code
inline int square(int x) {
    return x * x;
}

// Inline with condition
inline double max_value(double a, double b) {
    return (a > b) ? a : b;
}

int main() {
    cout << square(5) << endl;      // 25
    cout << max_value(3.5, 2.1) << endl;  // 3.5
    
    return 0;
}
```

---
### Professional Notes: Functional Depth

#### 1. Recursion and Tail-Call Optimization (TCO)
Recursion is a technique where a function calls itself.
*   **Base Case**: The condition where recursion stops.
*   **Stack Overflow**: Deep recursion can exhaust the stack.
*   **Tail-Call Optimization**: If the recursive call is the *last* action in the function, some compilers can transform it into a loop, saving stack space.
```cpp
// Tail-recursive factorial
int factorial_tail(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}
```

#### 2. Callable Objects (Functors)
In C++, anything that can be invoked with `()` is a callable.
*   **Function Pointers**: `void (*ptr)(int)`.
*   **Functors**: Classes that overload `operator()`.
*   **Lambdas (C++11)**: Anonymous functions.
*   **`std::function` (C++11)**: A polymorphic wrapper for any callable.

#### 3. Argument Dependent Lookup (ADL) - Recap
Functions are found in the namespaces of their arguments. This is why you can call `std::cout << obj` without qualifying the operator if it's defined in the same namespace as `obj`.

---

## 2.4 Static Functions

```cpp
#include <iostream>
using namespace std;

// File scope - only visible in this file
static void internal_function() {
    cout << "Internal function" << endl;
}

// Static with counter
int get_call_count() {
    static int count = 0;  // Persists between calls
    return ++count;
}

int main() {
    cout << get_call_count() << endl;  // 1
    cout << get_call_count() << endl;  // 2
    cout << get_call_count() << endl;  // 3
    
    internal_function();  // OK in same file
    
    return 0;
}
```

---


<!-- Merged content from Chapter_4_FUNCTION_POINTERS__CALLBACKS.md -->

# FUNCTION POINTERS & CALLBACKS


## 3.1 Function Pointers

```cpp
#include <iostream>
using namespace std;

// Function pointer declaration: return_type (*name)(parameters)
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

int main() {
    // Declare function pointer
    int (*operation)(int, int);
    
    // Assign function to pointer
    operation = add;
    cout << operation(5, 3) << endl;  // 8
    
    operation = subtract;
    cout << operation(5, 3) << endl;  // 2
    
    operation = multiply;
    cout << operation(5, 3) << endl;  // 15
    
    return 0;
}
```

## 3.2 Function Pointer Arrays

```cpp
#include <iostream>
using namespace std;

int add(int a, int b) { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b) { return a / b; }

int main() {
    // Array of function pointers
    int (*operations[4])(int, int) = {
        add, subtract, multiply, divide
    };
    
    int a = 20, b = 4;
    
    for (int i = 0; i < 4; i++) {
        cout << "Result: " << operations[i](a, b) << endl;
    }
    // Output: 24, 16, 80, 5
    
    return 0;
}
```

## 3.3 Callbacks

```cpp
#include <iostream>
#include <vector>
using namespace std;

// Callback function type
typedef void (*Callback)(const string&);

class Button {
private:
    Callback on_click;
    
public:
    void set_click_handler(Callback callback) {
        on_click = callback;
    }
    
    void click() {
        if (on_click) {
            on_click("Button clicked!");
        }
    }
};

void handle_click(const string& message) {
    cout << message << endl;
}

int main() {
    Button button;
    button.set_click_handler(handle_click);
    button.click();  // Output: Button clicked!
    
    return 0;
}
```

---


---

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


---
