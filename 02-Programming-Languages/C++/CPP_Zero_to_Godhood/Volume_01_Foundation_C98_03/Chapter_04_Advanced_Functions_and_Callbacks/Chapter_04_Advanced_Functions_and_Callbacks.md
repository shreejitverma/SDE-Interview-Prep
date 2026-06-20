# Chapter 04: Advanced Functions & Callbacks

> *The engines of your application — and the low-level data manipulation that powers them.*

A function is the smallest unit of reusable logic in C++, but the language wraps that simple idea in a surprising amount of machinery: parameter-passing semantics that decide whether you copy a megabyte or a pointer, overload resolution that picks one of many same-named functions, the call stack that makes recursion possible and stack overflow inevitable, and function pointers that let you treat code itself as data. This chapter takes you from the anatomy of a single function through the callback patterns that underpin every event-driven system, and closes with the bit-level and floating-point fundamentals that separate engineers who *use* the hardware from those who merely tolerate it.

---

## Table of Contents

- [4.1 Anatomy of a Function](#41-anatomy-of-a-function)
- [4.2 Passing Arguments: Value, Reference, and `const` Reference](#42-passing-arguments-value-reference-and-const-reference)
- [4.3 Default Parameters](#43-default-parameters)
- [4.4 Function Overloading](#44-function-overloading)
- [4.5 The `inline` Keyword](#45-the-inline-keyword)
- [4.6 Recursion](#46-recursion)
- [4.7 The Call Stack and Stack Overflow](#47-the-call-stack-and-stack-overflow)
- [4.8 Variadic Functions](#48-variadic-functions)
- [4.9 Static Functions and Internal Linkage](#49-static-functions-and-internal-linkage)
- [4.10 Function Pointers](#410-function-pointers)
- [4.11 Arrays of Function Pointers](#411-arrays-of-function-pointers)
- [4.12 Callbacks](#412-callbacks)
- [4.13 Professional Insights: Functional Depth](#413-professional-insights-functional-depth)
- [4.14 Floating-Point Arithmetic](#414-floating-point-arithmetic)
- [4.15 Bitwise Operations and Low-Level Optimization](#415-bitwise-operations-and-low-level-optimization)
- [4.16 Bit Fields](#416-bit-fields)
- [4.17 Professional Insights: Bit and Float Tricks](#417-professional-insights-bit-and-float-tricks)

---

## 4.1 Anatomy of a Function

Consider writing a game where every time the player takes damage you must calculate armor reduction, deduct health, play a sound effect, and update the screen. Inlining those twenty lines at every call site produces a bloated, unmaintainable program. **Functions** solve this: a function is a named block of code that performs a specific task, written once and called as often as needed. This is the core of the **DRY Principle — Don't Repeat Yourself**.

To define a function you specify four things:

1. **Return type** — what kind of data the function gives back when it finishes. If it returns nothing, the type is `void`.
2. **Name** — the identifier for the action, e.g. `calculateDamage`.
3. **Parameters** — the data the function needs to do its job, e.g. `int damage_amount`.
4. **Body** — the code to execute, wrapped in `{ }`.

```cpp
// Listing 4.1: The four parts of a function definition
// return_type  name (parameters)
int add(int a, int b) {
    int result = a + b; // The body
    return result;      // The return statement
}
```

Once defined, you **call** the function from `main()` or from any other function:

```cpp
// Listing 4.2: Calling a function
int main() {
    int sum = add(5, 10); // sum becomes 15
    return 0;
}
```

---

## 4.2 Passing Arguments: Value, Reference, and `const` Reference

How an argument travels into a function is one of the most consequential decisions in C++. There are three mechanisms.

### 4.2.1 Pass by Value (The Copy)

By default, C++ copies the value into the function. The function operates on a private duplicate.

```cpp
// Listing 4.3: Pass by value modifies only a copy
void tryToChange(int x) {
    x = 99; // Only changes the local copy
}

int main() {
    int score = 10;
    tryToChange(score);
    // score is STILL 10! The function only modified a photocopy.
    return 0;
}
```

**Pros:** Safe — the function cannot corrupt your original data. **Cons:** Slow for large objects — passing a 3D model copies millions of bytes just to hand it over.

### 4.2.2 Pass by Reference (`&`) (The Original)

Adding an ampersand `&` to the parameter type passes a **reference**. You instruct the function: *do not make a copy; operate on the exact memory where the original variable lives.*

```cpp
// Listing 4.4: Pass by reference modifies the original
void actuallyChange(int& x) {
    x = 99; // Modifies the original variable directly!
}

int main() {
    int score = 10;
    actuallyChange(score);
    // score is now 99!
    return 0;
}
```

**Pros:** No copying, and the function can modify the original. **Cons:** Dangerous if modification was not intended.

### 4.2.3 Pass by `const` Reference (The Holy Grail)

To combine the speed of a reference with a guarantee against accidental modification, make the reference `const`.

```cpp
// Listing 4.5: const reference — fast and safe
#include <iostream>
#include <string>

// Fast because it's a reference. Safe because it's const.
void printScore(const std::string& player_name) {
    std::cout << player_name;
    // player_name = "Hacker"; // ERROR! The compiler will stop this.
}
```

> **Godhood Tip — When to use which:**
> - **Fundamental types** (`int`, `double`, `bool`): pass by **value**. They are so small that copying is faster than dereferencing a hidden pointer.
> - **Large objects** (`std::string`, `std::vector`, classes): pass by **`const` reference**.
> - **When you must modify the caller's variable**: pass by **reference**.

---

## 4.3 Default Parameters

You can supply default values for parameters. If the caller omits them, the compiler fills them in.

```cpp
// Listing 4.6: Default parameter values
#include <iostream>
#include <string>

void greet(std::string name = "Traveler", int level = 1) {
    std::cout << "Hello, " << name << " (Lvl " << level << ")\n";
}

int main() {
    greet("Aloy", 50); // Prints: Hello, Aloy (Lvl 50)
    greet("Link");     // Prints: Hello, Link (Lvl 1)
    greet();           // Prints: Hello, Traveler (Lvl 1)
    return 0;
}
```

**Rule:** default parameters must always appear at the *end* of the parameter list. Once a parameter has a default, every parameter to its right must also have one.

---

## 4.4 Function Overloading

In C, printing an `int` and a `double` required two differently named functions, `print_int()` and `print_double()`. C++ supports **function overloading**: multiple functions may share the *exact same name* as long as their parameter lists differ. The compiler selects the correct overload from the argument types at the call site.

```cpp
// Listing 4.7: Two overloads of print
#include <iostream>

void print(int x) {
    std::cout << "Printing an integer: " << x << "\n";
}

void print(double x) {
    std::cout << "Printing a double: " << x << "\n";
}

int main() {
    print(42);    // Calls the int version
    print(3.14);  // Calls the double version
    return 0;
}
```

Note that the **return type alone cannot distinguish overloads** — overload resolution considers only the parameter list.

---

## 4.5 The `inline` Keyword

Every function call carries a small penalty: the CPU saves its current state, jumps to the function's address, executes, and jumps back. For trivially small functions, this overhead can exceed the work itself. The `inline` keyword *suggests* that the compiler copy the function body directly into each call site, eliminating the jump.

```cpp
// Listing 4.8: inline suggestion
inline int square(int x) {
    return x * x;
}

int main() {
    int y = square(5);
    // The compiler may rewrite the above as: int y = 5 * 5;
    return 0;
}
```

`inline` also accepts conditional expressions and floating-point work:

```cpp
// Listing 4.9: inline with a conditional expression
#include <iostream>
using namespace std;

inline int square(int x) {
    return x * x;
}

inline double max_value(double a, double b) {
    return (a > b) ? a : b;
}

int main() {
    cout << square(5) << endl;            // 25
    cout << max_value(3.5, 2.1) << endl;  // 3.5
    return 0;
}
```

`inline` is only a hint. Modern compilers (GCC, Clang) routinely ignore the keyword and make their own inlining decisions from complex heuristics. The keyword's *guaranteed* effect in C++98/03 is on linkage: an `inline` function may be defined in a header and included in multiple translation units without violating the One Definition Rule.

---

## 4.6 Recursion

A **recursive function** calls itself. Recursion is the natural expression of problems with self-similar structure — tree traversal, sorting, and divide-and-conquer algorithms. Every recursive function *must* have a **base case**: a condition that stops the recursion. Without one, it calls itself forever.

```cpp
// Listing 4.10: Factorial via recursion
int factorial(int n) {
    if (n <= 1) {
        return 1;  // Base case: stop calling yourself
    }
    return n * factorial(n - 1);  // Recursive case
}
```

The classic Fibonacci sequence is elegant recursively but exponentially slow, because it recomputes the same subproblems repeatedly. **Memoization** — caching already-computed results — collapses the exponential blowup to linear time.

```cpp
// Listing 4.11: Naive vs. memoized recursion
#include <iostream>
using namespace std;

// Fibonacci using recursion (inefficient: O(2^n))
int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// Fibonacci with memoization (efficient: O(n))
int fib_memo(int n, int memo[]) {
    if (n <= 1) return n;
    if (memo[n] != -1) return memo[n];

    memo[n] = fib_memo(n - 1, memo) + fib_memo(n - 2, memo);
    return memo[n];
}

int main() {
    cout << factorial(5) << endl;   // 120
    cout << fibonacci(10) << endl;  // 55

    int memo[11];
    for (int i = 0; i < 11; i++) memo[i] = -1;
    cout << fib_memo(10, memo) << endl;  // 55

    return 0;
}
```

---

## 4.7 The Call Stack and Stack Overflow

To understand recursion — and to understand why programs crash — you must understand the **call stack**.

Think of the stack as a physical stack of cafeteria trays. When your program starts, the OS places a tray for `main()` on the table. Inside `main()` you call `add()`; a new tray for `add()` goes on top. If `add()` calls `multiply()`, a `multiply()` tray goes on top of that.

**The rule of the stack:** the CPU can only see and work on the tray at the very top. When `multiply()` finishes, its tray is popped off and destroyed, revealing the `add()` tray beneath it; the CPU resumes `add()`. Each tray (a **stack frame**) holds that invocation's local variables and bookkeeping.

> **Stack Overflow.** What happens when a recursive function forgets its base case? It calls itself, adding a tray. It calls itself again, another tray. After tens of thousands of calls the stack of trays hits the ceiling. The operating system detects the exhaustion and instantly kills the program before it consumes all available memory. This is a **stack overflow** — the most famous crash in computer science.

---

## 4.8 Variadic Functions

A **variadic function** accepts a variable number of arguments. The C-style mechanism uses the `<cstdarg>` macros `va_list`, `va_start`, `va_arg`, and `va_end`. The first fixed parameter typically encodes how many variadic arguments follow.

```cpp
// Listing 4.12: A variadic sum using <cstdarg>
#include <iostream>
#include <cstdarg>
using namespace std;

// Function with a variable number of arguments
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

This mechanism is type-unsafe: the compiler cannot verify that the arguments match the types `va_arg` extracts. C++11's variadic templates supersede it for type-safe variadic code, but `<cstdarg>` remains the only option in C++98/03.

---

## 4.9 Static Functions and Internal Linkage

The `static` keyword at file scope gives a function **internal linkage** — it is visible only within its own translation unit, preventing name collisions across files. Applied to a *local variable*, `static` gives that variable **static storage duration**: it is initialized once and persists across calls.

```cpp
// Listing 4.13: static for file scope and persistent state
#include <iostream>
using namespace std;

// File scope - only visible in this file
static void internal_function() {
    cout << "Internal function" << endl;
}

// Static local persists between calls
int get_call_count() {
    static int count = 0;  // Initialized once; survives across calls
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

## 4.10 Function Pointers

A **function pointer** stores the address of a function, letting you treat code as data — selecting and invoking behavior at runtime. The declaration syntax names the return type, a parenthesized `(*name)`, and the parameter list.

```cpp
// Listing 4.14: Declaring and reassigning a function pointer
#include <iostream>
using namespace std;

// Function pointer declaration: return_type (*name)(parameters)
int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }

int main() {
    // Declare function pointer
    int (*operation)(int, int);

    // Assign a function to the pointer, then call through it
    operation = add;
    cout << operation(5, 3) << endl;  // 8

    operation = subtract;
    cout << operation(5, 3) << endl;  // 2

    operation = multiply;
    cout << operation(5, 3) << endl;  // 15

    return 0;
}
```

---

## 4.11 Arrays of Function Pointers

Because a function pointer is an ordinary value, you can store many of them in an array and dispatch by index — a compact alternative to a `switch` for tabular dispatch (the foundation of jump tables and bytecode interpreters).

```cpp
// Listing 4.15: An array of function pointers as a dispatch table
#include <iostream>
using namespace std;

int add(int a, int b)      { return a + b; }
int subtract(int a, int b) { return a - b; }
int multiply(int a, int b) { return a * b; }
int divide(int a, int b)   { return a / b; }

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

---

## 4.12 Callbacks

A **callback** is a function passed into another component so that the component can "call back" into your code when an event occurs. This inverts control: the library decides *when*, your code decides *what*. Callbacks are the backbone of event-driven systems — GUI buttons, network handlers, and signal dispatch.

```cpp
// Listing 4.16: A callback-driven Button
#include <iostream>
#include <string>
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

## 4.13 Professional Insights: Functional Depth

### 4.13.1 Recursion and Tail-Call Optimization (TCO)

When the recursive call is the *last* action a function performs, it is a **tail call**. Some compilers can transform tail recursion into an iterative loop, reusing a single stack frame and eliminating the stack-overflow risk of deep recursion.

```cpp
// Listing 4.17: Tail-recursive factorial
int factorial_tail(int n, int acc = 1) {
    if (n <= 1) return acc;
    return factorial_tail(n - 1, n * acc);
}
```

TCO is permitted but not mandated by the C++ standard; treat it as an optimization you can encourage, not one you can rely on for correctness.

### 4.13.2 Callable Objects (Functors)

In C++, anything invocable with `()` is a **callable**:

- **Function pointers** — `void (*ptr)(int)`.
- **Functors** — classes that overload `operator()`, carrying state between calls.
- **Lambdas** *(C++11)* — anonymous inline functions.
- **`std::function`** *(C++11)* — a polymorphic wrapper that can hold any callable with a given signature.

### 4.13.3 Argument-Dependent Lookup (ADL)

Functions are also searched for in the namespaces of their arguments. This is why `std::cout << obj` resolves the overloaded `operator<<` defined in the same namespace as `obj` without explicit qualification.

---

## 4.14 Floating-Point Arithmetic

Understanding how data is stored at the bit level and how floating-point numbers approximate real values is essential for high-performance C++ engineering. **Floating-point numbers store approximations**, because most base-10 decimals have no exact finite representation in binary.

### 4.14.1 The Imprecision Pitfall

Assuming floating-point equality behaves like mathematical equality is a classic mistake:

```cpp
// Listing 4.18: Floating-point equality is treacherous
double a = 0.1;
double b = 0.2;
double c = 0.3;
if (a + b == c) {
    std::cout << "Exact match!" << std::endl;
} else {
    std::cout << "Imprecise!" << std::endl; // Usually prints this
}
```

Under the **IEEE 754** standard used by most C++ compilers, `0.1` and `0.2` cannot be represented exactly in binary, so their sum carries a tiny rounding error and differs from the stored value of `0.3`.

### 4.14.2 Comparison Strategy

Never compare floating-point values with `==`. Compare against a small tolerance, an **epsilon**:

```cpp
// Listing 4.19: Tolerance-based floating-point comparison
#include <cmath>
#include <limits>

bool nearly_equal(double a, double b) {
    return std::abs(a - b) < std::numeric_limits<double>::epsilon();
}
```

For values far from 1.0, a fixed epsilon is too strict or too loose; production code typically scales the tolerance by the magnitude of the operands (relative comparison).

### 4.14.3 Special Values: NaN and Infinity

- **`std::numeric_limits<double>::quiet_NaN()`** — Not a Number, produced by undefined operations such as `0.0/0.0`. A NaN compares unequal to everything, including itself.
- **`std::numeric_limits<double>::infinity()`** — positive infinity, produced by operations such as `1.0/0.0`.

---

## 4.15 Bitwise Operations and Low-Level Optimization

Bitwise operators manipulate individual bits and are essential for embedded systems, graphics, networking, and cryptography.

| Operator | Name | Effect |
| :------- | :--- | :----- |
| `&` | AND | Result bit is 1 only if both bits are 1 |
| `\|` | OR | Result bit is 1 if at least one bit is 1 |
| `^` | XOR | Result bit is 1 if the bits differ |
| `~` | NOT | Flips every bit |
| `<<` | Left shift | Multiply by 2^N |
| `>>` | Right shift | Divide by 2^N |

### 4.15.1 Essential Bit Tricks

1. **Check odd/even:** `(x & 1) == 0` is even — faster than `% 2`.
2. **Multiply by 2:** `x << 1`.
3. **Divide by 2:** `x >> 1`.
4. **Clear the lowest set bit:** `x & (x - 1)` — the basis of Kernighan's set-bit-counting algorithm.
5. **Check power of two:** `(x > 0) && ((x & (x - 1)) == 0)`.
6. **Toggle bit N:** `x ^= (1 << N)`.
7. **Set bit N:** `x |= (1 << N)`.
8. **Clear bit N:** `x &= ~(1 << N)`.

```cpp
// Listing 4.20: Branch-free power-of-two test
bool isPowerOf2(int x) {
    return x && !(x & (x - 1));
}
```

---

## 4.16 Bit Fields

**Bit fields** let you specify exactly how many bits each member of a struct or class occupies — crucial for matching hardware register layouts or wire protocols and for packing flags densely.

```cpp
// Listing 4.21: A bit-field struct mirroring a hardware register
struct HardwareRegister {
    unsigned int enable : 1;  // 1 bit
    unsigned int mode   : 3;  // 3 bits
    unsigned int value  : 4;  // 4 bits
};
```

**Professional note:** the exact memory layout of bit fields — bit ordering, straddling of storage units, padding — is implementation-defined and depends on the platform's endianness and alignment rules. Never assume a portable on-the-wire layout from a bit-field struct.

---

## 4.17 Professional Insights: Bit and Float Tricks

### 4.17.1 Bit Manipulation Hacks

- **Swap without a temporary:** `a ^= b; b ^= a; a ^= b;`. *Warning:* on modern CPUs this is usually **slower** than a temporary variable because it introduces a data-dependency chain that stalls the pipeline.
- **Branch-free absolute value (32-bit signed):** `(x + (x >> 31)) ^ (x >> 31)`. This exploits arithmetic right-shift producing all-ones or all-zeros from the sign bit.

### 4.17.2 Floating-Point Models

- **`fast-math`** — a compiler flag (e.g. `-ffast-math` in GCC) that permits the compiler to ignore some IEEE 754 guarantees for speed, potentially altering precision and breaking NaN/Inf semantics. Enable it only when you understand the numerical consequences.
- **`long double`** — on many platforms this provides 80-bit or 128-bit precision for high-precision scientific computation, at the cost of performance and portability.
