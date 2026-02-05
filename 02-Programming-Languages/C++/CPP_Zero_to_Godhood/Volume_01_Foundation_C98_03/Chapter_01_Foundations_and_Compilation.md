# FOUNDATIONS & COMPILATION MODEL

# ABSOLUTE BASICS (C++98)


## Getting Started

### What is C++?

C++ is a statically-typed, compiled programming language that combines low-level memory manipulation with high-level abstractions. It's the language of choice for performance-critical applications.

### Your First Program (C++98)

```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

**Breakdown:**
- `#include <iostream>` - Include input/output library
- `std::cout` - Standard output stream (print to console)
- `std::endl` - End line and flush buffer
- `main()` - Entry point of program
- `return 0` - Exit code (0 = success)

**Compile and run:**
```bash
g++ -o hello hello.cpp
./hello
```

---

## Basic Types & Variables

### Fundamental Types (C++98)

```cpp
#include <iostream>
#include <limits>

int main() {
    // Integer types
    int x = 42;                  // 32-bit integer
    short y = 10;                // 16-bit integer
    long z = 1000000;            // 32 or 64-bit integer
    long long w = 9999999999;    // 64-bit integer
    
    // Floating-point types
    float f = 3.14f;             // 32-bit (4 bytes)
    double d = 3.14159265;       // 64-bit (8 bytes)
    long double ld = 3.14159265359L;  // 80+ bits
    
    // Character and boolean types
    char c = 'A';                // Single byte
    bool b = true;               // true or false
    
    // Print sizes
    std::cout << "int: " << sizeof(int) << " bytes\n";
    std::cout << "double: " << sizeof(double) << " bytes\n";
    
    // Min/max values
    std::cout << "int max: " << std::numeric_limits<int>::max() << "\n";
    std::cout << "int min: " << std::numeric_limits<int>::min() << "\n";
    
    return 0;
}
```

### Variable Declaration & Initialization

```cpp
#include <iostream>

int main() {
    // C-style initialization
    int x = 5;
    float f = 3.14f;
    
    // Multiple variables
    int a, b, c;
    
    // Uninitialized (dangerous - contains garbage)
    int uninitialized;
    std::cout << uninitialized << "\n";  // Undefined behavior!
    
    // Constants
    const int MAX_SIZE = 100;
    // MAX_SIZE = 200;  // Error: can't modify const
    
    return 0;
}
```

### Scope & Lifetime (C++98)

```cpp
#include <iostream>

int global = 100;  // Global scope - lives entire program

void function() {
    int local = 5;      // Local scope - lives only in function
    {
        int nested = 10;  // Block scope - lives only in block
        std::cout << nested << "\n";
    }
    // std::cout << nested << "\n";  // Error: nested out of scope
}

int main() {
    {
        int x = 5;
    }
    // std::cout << x << "\n";  // Error: x out of scope
    
    return 0;
}
```

### Deep Dive: The Memory Model of Variables

Understanding *where* your variables live is the first step to Godhood.

1.  **The Stack (Automatic Storage)**:
    *   **What**: Local variables (`int x`).
    *   **Speed**: Extremely fast (just moving a pointer).
    *   **Lifetime**: Scope-based (die at `}`).
    *   **Limit**: Small (typically 1MB-8MB). Recursion depth is limited by this.

2.  **The Heap (Dynamic Storage)**:
    *   **What**: `new int`, `malloc`.
    *   **Speed**: Slower (allocation requires finding free block).
    *   **Lifetime**: Manual (until `delete` / `free`).
    *   **Limit**: RAM size (Gigabytes).

3.  **Static/Global (Static Storage)**:
    *   **What**: Global variables, `static` locals.
    *   **Speed**: Fast access, but initialization order is tricky.
    *   **Lifetime**: Program start to program end.

4.  **Registers**:
    *   **What**: CPU internal storage.
    *   **Speed**: Instant (0 cycles).
    *   **Note**: Variables are often optimized into registers, never touching RAM!

---

## Operators & Control Flow

### Arithmetic Operators (C++98)

```cpp
#include <iostream>

int main() {
    int a = 10, b = 3;
    
    std::cout << a + b << "\n";   // 13 (addition)
    std::cout << a - b << "\n";   // 7 (subtraction)
    std::cout << a * b << "\n";   // 30 (multiplication)
    std::cout << a / b << "\n";   // 3 (integer division)
    std::cout << a % b << "\n";   // 1 (modulo)
    
    // Compound assignment
    int x = 5;
    x += 3;   // x = 8
    x -= 2;   // x = 6
    x *= 2;   // x = 12
    x /= 3;   // x = 4
    
    // Increment/Decrement
    int y = 5;
    y++;      // Post-increment: 6
    ++y;      // Pre-increment: 7
    y--;      // Post-decrement: 6
    --y;      // Pre-decrement: 5
    
    return 0;
}
```

### Deep Dive: Bitwise Mastery (Low-Level Optimization)

Bitwise operators manipulate individual bits. Essential for embedded systems, graphics, and cryptography.

#### The Operators
*   `&` (AND): Both bits must be 1.
*   `|` (OR): At least one bit must be 1.
*   `^` (XOR): Bits must be different.
*   `~` (NOT): Flip all bits.
*   `<<` (Left Shift): Multiply by 2^N.
*   `>>` (Right Shift): Divide by 2^N.

#### God-Tier Tricks
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

### Comparison & Logical Operators (C++98)

```cpp
#include <iostream>

int main() {
    int a = 10, b = 5;
    
    // Comparison (return true/false)
    std::cout << (a > b) << "\n";   // 1 (true)
    std::cout << (a < b) << "\n";   // 0 (false)
    std::cout << (a == b) << "\n";  // 0 (false)
    std::cout << (a != b) << "\n";  // 1 (true)
    std::cout << (a >= b) << "\n";  // 1 (true)
    std::cout << (a <= b) << "\n";  // 0 (false)
    
    // Logical operators
    bool x = true, y = false;
    std::cout << (x && y) << "\n";  // 0 (AND)
    std::cout << (x || y) << "\n";  // 1 (OR)
    std::cout << (!x) << "\n";      // 0 (NOT)
    
    return 0;
}
```

### If-Else Statements (C++98)

```cpp
#include <iostream>

int main() {
    int score = 85;
    
    // Basic if-else
    if (score >= 90) {
        std::cout << "Grade: A\n";
    } else if (score >= 80) {
        std::cout << "Grade: B\n";
    } else if (score >= 70) {
        std::cout << "Grade: C\n";
    } else {
        std::cout << "Grade: F\n";
    }
    
    // Ternary operator
    std::string grade = (score >= 80) ? "Pass" : "Fail";
    std::cout << grade << "\n";
    
    return 0;
}
```

### Loops (C++98)

```cpp
#include <iostream>

int main() {
    // While loop
    int i = 0;
    while (i < 5) {
        std::cout << i << " ";
        i++;
    }
    std::cout << "\n";
    
    // Do-while loop (executes at least once)
    int j = 0;
    do {
        std::cout << j << " ";
        j++;
    } while (j < 5);
    std::cout << "\n";
    
    // For loop
    for (int k = 0; k < 5; k++) {
        std::cout << k << " ";
    }
    std::cout << "\n";
    
    // Break and continue
    for (int m = 0; m < 10; m++) {
        if (m == 3) continue;  // Skip 3
        if (m == 7) break;     // Exit at 7
        std::cout << m << " ";
    }
    std::cout << "\n";
    
    return 0;
}
```

### Switch Statement (C++98)

```cpp
#include <iostream>

int main() {
    int day = 3;
    
    switch (day) {
        case 1:
            std::cout << "Monday\n";
            break;
        case 2:
            std::cout << "Tuesday\n";
            break;
        case 3:
            std::cout << "Wednesday\n";
            break;
        default:
            std::cout << "Unknown day\n";
    }
    
    return 0;
}
```

---

## Functions

### Function Declaration & Definition (C++98)

```cpp
#include <iostream>

// Function declaration (prototype)
int add(int a, int b);
void print_hello();

// Function definition
int add(int a, int b) {
    return a + b;
}

void print_hello() {
    std::cout << "Hello!\n";
}

int main() {
    print_hello();
    std::cout << add(5, 3) << "\n";  // 8
    return 0;
}
```

### Parameters & Return Values (C++98)

```cpp
#include <iostream>

// Pass by value (copy)
void increment_value(int x) {
    x++;
    std::cout << "Inside: " << x << "\n";
}

// Pass by reference (same variable)
void increment_ref(int& x) {
    x++;
    std::cout << "Inside: " << x << "\n";
}

// Pass by const reference (can't modify)
void print_value(const int& x) {
    std::cout << x << "\n";
}

// Returning by value
int get_value() {
    return 42;
}

// Returning by reference (dangerous!)
int& get_global() {
    static int x = 100;
    return x;
}

int main() {
    int a = 5;
    
    increment_value(a);   // Copy passed
    std::cout << a << "\n";  // Still 5
    
    increment_ref(a);      // Reference passed
    std::cout << a << "\n";  // Now 6
    
    print_value(a);        // Can't modify a
    
    return 0;
}
```

### Default Parameters (C++98)

```cpp
#include <iostream>

void greet(const std::string& name = "World") {
    std::cout << "Hello, " << name << "!\n";
}

int main() {
    greet();                // Uses default: "World"
    greet("Alice");         // Uses provided: "Alice"
    return 0;
}
```

### Function Overloading (C++98)

```cpp
#include <iostream>

// Same function name, different parameters
int add(int a, int b) {
    return a + b;
}

double add(double a, double b) {
    return a + b;
}

void print(int x) {
    std::cout << "Integer: " << x << "\n";
}

void print(double x) {
    std::cout << "Double: " << x << "\n";
}

void print(const std::string& s) {
    std::cout << "String: " << s << "\n";
}

int main() {
    std::cout << add(5, 3) << "\n";      // 8 (int version)
    std::cout << add(2.5, 3.7) << "\n";  // 6.2 (double version)
    
    print(42);           // Integer version
    print(3.14);         // Double version
    print("Hello");      // String version
    
    return 0;
}
```

---

## Arrays & Pointers

### Arrays (C++98)

```cpp
#include <iostream>

int main() {
    // Array declaration and initialization
    int arr[5] = {1, 2, 3, 4, 5};
    
    // Access elements (0-indexed)
    std::cout << arr[0] << "\n";  // 1
    std::cout << arr[4] << "\n";  // 5
    
    // Array size (local arrays only)
    int size = 5;
    for (int i = 0; i < size; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << "\n";
    
    // 2D array
    int matrix[3][3] = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 9}
    };
    
    std::cout << matrix[1][2] << "\n";  // 6
    
    return 0;
}
```

### Pointers (C++98)

```cpp
#include <iostream>

int main() {
    int x = 42;
    
    // Create a pointer
    int* ptr = &x;  // & = address-of operator
    
    // Dereference pointer
    std::cout << *ptr << "\n";   // 42 (dereference with *)
    
    // Modify through pointer
    *ptr = 100;
    std::cout << x << "\n";      // 100
    
    // Pointer arithmetic
    int arr[5] = {10, 20, 30, 40, 50};
    int* p = arr;           // Array decays to pointer
    
    std::cout << p[0] << "\n";     // 10
    std::cout << *(p+1) << "\n";   // 20
    std::cout << p[2] << "\n";     // 30
    
    // Null pointer
    int* null_ptr = nullptr;  // or NULL in C++98
    
    // Pointer to pointer
    int** pp = &ptr;
    std::cout << **pp << "\n";  // 100
    
    return 0;
}
```

### Dynamic Memory (C++98)

```cpp
#include <iostream>

int main() {
    // Allocate single value on heap
    int* ptr = new int;
    *ptr = 42;
    std::cout << *ptr << "\n";
    delete ptr;          // Must deallocate
    ptr = nullptr;       // Good practice
    
    // Allocate array on heap
    int* arr = new int[10];
    for (int i = 0; i < 10; i++) {
        arr[i] = i * i;
    }
    delete[] arr;        // Note the [] for arrays
    
    // Forgetting to delete = memory leak
    int* leaked = new int(100);
    // delete leaked;  // Forgot this!
    
    return 0;
}
```


<!-- Merged content from Chapter_17_THE_C_COMPILATION__EXECUTION_MODEL.md -->

# THE C++ COMPILATION & EXECUTION MODEL


To truly understand C++, you must understand how your code transforms from text to a running process. This section demystifies the "black box" of the compiler.

### 1.5.1 The Build Pipeline: From Source to Binary

The "compilation" process actually consists of four distinct stages:

1.  **Preprocessing**: Text substitution.
    *   Removes comments.
    *   Expands macros (`#define`).
    *   Includes headers (`#include`) recursively.
    *   Handles conditionals (`#ifdef`).
    *   *Output*: A single "Translation Unit" (pure C++ source code).

2.  **Compilation**: Syntax to Assembly.
    *   **Lexical Analysis**: Tokenizes source (e.g., `int`, `main`, `{`).
    *   **Parsing**: Builds Abstract Syntax Tree (AST).
    *   **Semantic Analysis**: Type checking, overload resolution.
    *   **Optimization**: Dead code elimination, loop unrolling, inlining (O1, O2, O3).
    *   **Code Generation**: Generates assembly code for the target architecture (x86_64, ARM64).
    *   *Output*: Assembly file (`.s` or `.asm`).

3.  **Assembly**: Assembly to Machine Code.
    *   Translates mnemonics (`MOV`, `ADD`) to opcodes (`0x89`, `0x01`).
    *   *Output*: Object file (`.o` or `.obj`). Contains machine code but with *unresolved symbols*.

4.  **Linking**: Combining Object Files.
    *   Combines multiple `.o` files and static libraries (`.a`/`.lib`).
    *   Resolves symbols: Matches function calls in one TU to definitions in another.
    *   Relocates addresses: Adjusts internal pointers.
    *   *Output*: Executable (`.exe` or ELF/Mach-O) or Shared Library (`.so`/`.dll`).

### 1.5.2 Translation Units (TU) & Linkage

A **Translation Unit** is the input to the compiler after preprocessing. It is the fundamental unit of compilation.

#### Linkage Types
Linkage determines if a name (variable/function) is visible to other TUs.

1.  **No Linkage**:
    *   Visible only within the current scope (block).
    *   Example: Local variables.

2.  **Internal Linkage**:
    *   Visible only within the current Translation Unit.
    *   Hidden from the linker.
    *   Examples: `static` global variables, `static` free functions, `const` globals (by default).
    *   *Use Case*: Private helper functions.

3.  **External Linkage**:
    *   Visible to the linker and other TUs.
    *   Examples: Global variables, non-static functions, `extern` variables.
    *   *Danger*: Requires strict ODR compliance.

```cpp
// TU1.cpp
static int internal_var = 10; // Internal Linkage
int global_var = 20;          // External Linkage

// TU2.cpp
extern int global_var;        // Declares existence of external symbol
// extern int internal_var;   // ERROR: Linker error (symbol not found)
```

### 1.5.3 The One Definition Rule (ODR)

The ODR is the most important rule in C++ linking.

1.  **ODR for Translation Units**: A function/variable can have only **one definition** in a single TU. (Declarations can be repeated).
2.  **ODR for Programs**: A non-inline function/variable can have only **one definition** in the entire program.
3.  **ODR for Classes/Templates**: Can be defined in multiple TUs (via headers), but definitions must be **token-for-token identical**.

**Violation Example:**
```cpp
// header.h
int x = 10; // DEFINITION (allocates memory)

// a.cpp
#include "header.h"

// b.cpp
#include "header.h"

// Linker Error: multiple definition of `x`
// Fix: Use 'extern int x;' in header, definition in one .cpp
// Fix (C++17): Use 'inline int x = 10;'
```

### 1.5.4 Process Memory Layout

When your C++ program runs, the OS allocates virtual memory segments:

1.  **Text Segment (.text)**:
    *   Contains executable machine instructions.
    *   Read-Only (prevents self-modifying code exploits).
    *   Shared between multiple instances of the app.

2.  **Data Segment (.data)**:
    *   Initialized global and static variables.
    *   `int g = 10;` lives here.

3.  **BSS Segment (.bss)**:
    *   Uninitialized global/static variables.
    *   `int g;` lives here.
    *   Automatically zero-initialized by OS before `main()`.

4.  **Heap**:
    *   Dynamic memory (`new`, `malloc`).
    *   Grows upwards (typically).
    *   Managed manually or by smart pointers.

5.  **Stack**:
    *   Local variables, function parameters, return addresses.
    *   Grows downwards (typically).
    *   LIFO (Last-In, First-Out).
    *   *Stack Overflow*: Exceeding stack size (e.g., deep recursion).

### 1.5.5 Program Startup: Before main()

`main()` is NOT the first thing to run.

1.  **OS Loader**: Loads EXE into memory.
2.  **Entry Point (`_start`)**: Defined by C Runtime (CRT).
3.  **Global Initialization**:
    *   Constructors of global/static objects run **before** `main`.
    *   *Static Initialization Order Fiasco*: Order between TUs is undefined.
4.  **`main()`**: Your code runs.
5.  **Global Destruction**:
    *   Destructors of global/static objects run **after** `main` returns.
    *   `atexit` handlers run.

### 1.5.6 Deep Dive into Data Representation

To understand bugs like integer overflow and floating-point inaccuracy, you must know how data is stored bits-by-bits.

#### Integer Representation (Two's Complement)
Most modern systems use **Two's Complement** for signed integers.
*   **Positive Numbers**: Standard binary. `0000 0101` (5).
*   **Negative Numbers**: Invert all bits and add 1.
    *   `5` = `0000 0101`
    *   Invert: `1111 1010`
    *   Add 1: `1111 1011` (-5)
*   **Advantage**: Addition/Subtraction works identically for signed/unsigned.
*   **Range**: `-2^(N-1)` to `2^(N-1) - 1`. (One extra negative number).

#### Floating Point (IEEE 754)
`float` (32-bit) and `double` (64-bit) follow IEEE 754.
*   **Components**:
    1.  **Sign Bit**: 0 (+) or 1 (-).
    2.  **Exponent**: Biased integer (allows very large/small numbers).
    3.  **Mantissa (Significand)**: The precision bits (normalized to 1.xxxxx).
*   **Implication**:
    *   `0.1 + 0.2 != 0.3` because 0.1 cannot be represented exactly in binary (infinite repeating fraction).
    *   **NaN (Not a Number)**: Result of 0/0 or sqrt(-1). `NaN != NaN` is always true.
    *   **Infinity**: Result of 1.0/0.0.

```cpp
#include <iostream>
#include <cmath>
#include <limits>

int main() {
    float a = 0.1f;
    float b = 0.2f;
    if (a + b == 0.3f) {
        std::cout << "Math works!\n";
    } else {
        std::cout << "Math is broken: " << (a+b) << "\n"; // Prints 0.30000001
    }
    
    // Correct comparison
    if (std::abs((a + b) - 0.3f) < 1e-5) {
        std::cout << "Close enough!\n";
    }
}
```

---
