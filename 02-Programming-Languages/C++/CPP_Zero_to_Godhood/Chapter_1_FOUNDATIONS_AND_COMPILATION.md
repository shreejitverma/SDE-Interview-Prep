# FOUNDATIONS AND COMPILATION


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

---
### Professional Notes: Basics & I/O

#### 1. The Compilation Pipeline (Deep Dive)
Compilation in C++ is a multi-stage process that transforms human-readable source code into machine-executable binaries.
1.  **Preprocessing**: The preprocessor (`cpp`) handles directives starting with `#`. It includes headers (`#include`) and expands macros (`#define`).
2.  **Compilation**: The compiler (`g++`, `clang++`) translates the preprocessed source into assembly code.
3.  **Assembly**: The assembler (`as`) converts assembly into machine-readable object files (`.o` or `.obj`).
4.  **Linking**: The linker (`ld`) combines multiple object files and libraries into a single executable, resolving symbols and addresses.

#### 2. Standard Streams and Buffered I/O
C++ provides a robust I/O system based on streams.
*   **`std::cout`**: Buffered output stream (standard output).
*   **`std::cin`**: Buffered input stream (standard input).
*   **`std::cerr`**: Unbuffered output stream (standard error).
*   **`std::clog`**: Buffered output stream (logging standard error).

**Godhood Tip**: Use `std::endl` only when you need to flush the buffer (e.g., in real-time logging). For performance, use `'\n'` to avoid unnecessary flushes.

#### 3. Stream State and Error Handling
Always check the state of a stream after an operation:
```cpp
int x;
if (!(std::cin >> x)) {
    std::cin.clear(); // Clear the error flags
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard bad input
    std::cerr << "Invalid input received!" << std::endl;
}
```
Stream states include `good()`, `bad()`, `fail()`, and `eof()`.

---

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

---
### Professional Notes: Literals & Keywords

#### 1. Integer and Floating Point Literals
*   **Decimal**: `42`
*   **Octal**: `052` (Starts with 0)
*   **Hexadecimal**: `0x2A`
*   **Binary (C++14)**: `0b101010`
*   **Suffixes**: `u` (unsigned), `l` (long), `ll` (long long), `f` (float).
*   **Digit Separators (C++14)**: Use single quotes to improve readability: `1'000'000` or `0b1111'0000`.

#### 2. String Literals and Encodings
*   **Raw Strings (C++11)**: `R"(string with \ and " without escaping)"`
*   **Encodings**: `u8""` (UTF-8), `u""` (char16_t), `U""` (char32_t), `L""` (wchar_t).

#### 3. Core Keywords and Type Qualifiers
*   **`const`**: Declares a variable as read-only.
*   **`volatile`**: Tells the compiler that the value can change outside the program's control (e.g., hardware registers). Prevents aggressive optimization.
*   **`mutable`**: Allows a member of a `const` object to be modified (Chapter 5).
*   **`explicit`**: Prevents the compiler from using a constructor for implicit conversions.
*   **`inline`**: Suggests to the compiler to replace function calls with the actual code to save overhead.

---

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
### Professional Notes: Operators & Control Flow

#### 1. Operator Precedence and Associativity
C++ has a strict hierarchy for operator evaluation. When multiple operators appear in an expression, precedence determines the grouping.
*   **High Precedence**: `()`, `[]`, `->`, `::`, `++` (postfix).
*   **Medium Precedence**: `*`, `/`, `%` followed by `+`, `-`.
*   **Low Precedence**: Bitwise shifts `<<`, `>>`, then comparisons `<`, `>`, then logical `&&`, `||`, and finally assignment `=`.

**Godhood Warning**: Never write ambiguous code like `a = b++ + ++b;`. This is **Undefined Behavior (UB)** because it attempts to modify `b` twice between sequence points.

#### 2. Advanced Loop Patterns
*   **Range-based for (C++11)**: Iterate directly over containers: `for (const auto& x : vec)`.
*   **Empty Loop Body**: A semicolon or empty braces can be used for loops that perform all work in the header: `while (*dest++ = *src++);`.
*   **The `for` Loop as a `while` Loop**: `for (; condition ;) {}` is identical to `while (condition) {}`.

#### 3. Flow Control Quirks
*   **`switch` Fallthrough**: Without a `break`, execution continues to the next case. In C++17, use `[[fallthrough]];` to signal intent and silence warnings.
*   **`goto` Statement**: While generally discouraged, `goto` is acceptable for breaking out of deeply nested loops or for error-cleanup blocks in low-level code.
*   **The Comma Operator**: `a, b` evaluates `a`, discards the result, then returns `b`. Useful in for-loop headers: `for (int i=0, j=10; i<j; ++i, --j)`.

---

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


# Professional Notes: Chapter 1: Getting started with C++

Version
C++98 ISO/IEC 14882:1998 1998-09-01
Standard
Release Date
C++03 ISO/IEC 14882:2003 2003-10-16
C++11 ISO/IEC 14882:2011 2011-09-01
C++14 ISO/IEC 14882:2014 2014-12-15
C++17 TBD
C++20 TBD
2017-01-01
2020-01-01
Section 1.1: Hello World
This program prints Hello World! to the standard output stream:
#include <iostream>
int main()
{
    std::cout << "Hello World!" << std::endl;
}
See it live on Coliru.
Analysis
Let's examine each part of this code in detail:
#include <iostream> is a preprocessor directive that includes the content of the standard C++ header le
iostream.
iostream is a standard library header le that contains denitions of the standard input and output
streams. These denitions are included in the std namespace, explained below.
The standard input/output (I/O) streams provide ways for programs to get input from and output to an
external system -- usually the terminal.
int main() { ... } denes a new function named main. By convention, the main function is called upon
execution of the program. There must be only one main function in a C++ program, and it must always return
a number of the int type.
Here, the int is what is called the function's return type. The value returned by the main function is an exit
code.
By convention, a program exit code of 0 or EXIT_SUCCESS is interpreted as success by a system that executes
the program. Any other return code is associated with an error.
If no return statement is present, the main function (and thus, the program itself) returns 0 by default. In this
example, we don't need to explicitly write return 0;.
All other functions, except those that return the void type, must explicitly return a value according to their
return type, or else must not return at all.
std::cout << "Hello World!" << std::endl; prints "Hello World!" to the standard output stream:
std is a namespace, and :: is the scope resolution operator that allows look-ups for objects by name
within a namespace.
There are many namespaces. Here, we use :: to show we want to use cout from the std namespace.
For more information refer to Scope Resolution Operator - Microsoft Documentation.
std::cout is the standard output stream object, dened in iostream, and it prints to the standard
output (stdout).
<< is, in this context, the stream insertion operator, so called because it inserts an object into the
stream object.
The standard library denes the << operator to perform data insertion for certain data types into
output streams. stream << content inserts content into the stream and returns the same, but
updated stream. This allows stream insertions to be chained: std::cout << "Foo" << " Bar"; prints
"FooBar" to the console.
"Hello World!" is a character string literal, or a "text literal." The stream insertion operator for
character string literals is dened in le iostream.
std::endl is a special I/O stream manipulator object, also dened in le iostream. Inserting a
manipulator into a stream changes the state of the stream.
The stream manipulator std::endl does two things: rst it inserts the end-of-line character and then it
ushes the stream buer to force the text to show up on the console. This ensures that the data
inserted into the stream actually appear on your console. (Stream data is usually stored in a buer and
then "ushed" in batches unless you force a ush immediately.)
An alternate method that avoids the ush is:
std::cout << "Hello World!\n";
where \n is the character escape sequence for the newline character.
The semicolon (;) noties the compiler that a statement has ended. All C++ statements and class
denitions require an ending/terminating semicolon.
Section 1.2: Comments
A comment is a way to put arbitrary text inside source code without having the C++ compiler interpret it with any
functional meaning. Comments are used to give insight into the design or method of a program.
There are two types of comments in C++:
Single-Line Comments
The double forward-slash sequence // will mark all text until a newline as a comment:
int main()
{
   // This is a single-line comment.
   int a;  // this also is a single-line comment
   int i;  // this is another single-line comment
}
C-Style/Block Comments
The sequence /* is used to declare the start of the comment block and the sequence */ is used to declare the end
of comment. All text between the start and end sequences is interpreted as a comment, even if the text is
otherwise valid C++ syntax. These are sometimes called "C-style" comments, as this comment syntax is inherited
from C++'s predecessor language, C:
int main()
{
   /*
    *  This is a block comment.
    */
   int a;
}
In any block comment, you can write anything you want. When the compiler encounters the symbol */, it
terminates the block comment:
int main()
{
   /* A block comment with the symbol /*
      Note that the compiler is not affected by the second /*
      however, once the end-block-comment symbol is reached,
      the comment ends.
   */
   int a;
}
The above example is valid C++ (and C) code. However, having additional /* inside a block comment might result in
a warning on some compilers.
Block comments can also start and end within a single line. For example:
void SomeFunction(/* argument 1 */ int a, /* argument 2 */ int b);
Importance of Comments
As with all programming languages, comments provide several benets:
Explicit documentation of code to make it easier to read/maintain
Explanation of the purpose and functionality of code
Details on the history or reasoning behind the code
Placement of copyright/licenses, project notes, special thanks, contributor credits, etc. directly in the source
code.
However, comments also have their downsides:
They must be maintained to reect any changes in the code
Excessive comments tend to make the code less readable
The need for comments can be reduced by writing clear, self-documenting code. A simple example is the use of
explanatory names for variables, functions, and types. Factoring out logically related tasks into discrete functions
goes hand-in-hand with this.
Comment markers used to disable code
During development, comments can also be used to quickly disable portions of code without deleting it. This is
often useful for testing or debugging purposes, but is not good style for anything other than temporary edits. This
is often referred to as commenting out.
Similarly, keeping old versions of a piece of code in a comment for reference purposes is frowned upon, as it
clutters les while oering little value compared to exploring the code's history via a versioning system.
Section 1.3: The standard C++ compilation process
Executable C++ program code is usually produced by a compiler.
A compiler is a program that translates code from a programming language into another form which is (more)
directly executable for a computer. Using a compiler to translate code is called compilation.
C++ inherits the form of its compilation process from its "parent" language, C. Below is a list showing the four major
steps of compilation in C++:
1.
The C++ preprocessor copies the contents of any included header les into the source code le, generates
macro code, and replaces symbolic constants dened using #dene with their values.
2.
The expanded source code le produced by the C++ preprocessor is compiled into assembly language
appropriate for the platform.
3.
4.
The assembler code generated by the compiler is assembled into appropriate object code for the platform.
The object code le generated by the assembler is linked together with the object code les for any library
functions used to produce an executable le.
Note: some compiled code is linked together, but not to create a nal program. Usually, this "linked" code
can also be packaged into a format that can be used by other programs. This "bundle of packaged, usable
code" is what C++ programmers refer to as a library.
Many C++ compilers may also merge or un-merge certain parts of the compilation process for ease or for additional
analysis. Many C++ programmers will use dierent tools, but all of the tools will generally follow this generalized
process when they are involved in the production of a program.
The link below extends this discussion and provides a nice graphic to help. [1]:
http://faculty.cs.niu.edu/~mcmahon/CS241/Notes/compile.html
Section 1.4: Function
A function is a unit of code that represents a sequence of statements.
Functions can accept arguments or values and return a single value (or not). To use a function, a function call is
used on argument values and the use of the function call itself is replaced with its return value.
Every function has a type signature -- the types of its arguments and the type of its return type.
Functions are inspired by the concepts of the procedure and the mathematical function.
Note: C++ functions are essentially procedures and do not follow the exact denition or rules of
mathematical functions.
Functions are often meant to perform a specic task. and can be called from other parts of a program. A function
must be declared and dened before it is called elsewhere in a program.
Note: popular function denitions may be hidden in other included les (often for convenience and reuse
across many les). This is a common use of header les.
Function Declaration
A function declaration is declares the existence of a function with its name and type signature to the compiler.
The syntax is as the following:
int add2(int i); // The function is of the type (int) -> (int)
In the example above, the int add2(int i) function declares the following to the compiler:
The return type is int.
The name of the function is add2.
The number of arguments to the function is 1:
The rst argument is of the type int.
The rst argument will be referred to in the function's contents by the name i.
The argument name is optional; the declaration for the function could also be the following:
int add2(int); // Omitting the function arguments' name is also permitted.
Per the one-denition rule, a function with a certain type signature can only be declared or dened once in an
entire C++ code base visible to the C++ compiler. In other words, functions with a specic type signature cannot be
re-dened -- they must only be dened once. Thus, the following is not valid C++:
int add2(int i);  // The compiler will note that add2 is a function (int) -> int
int add2(int j);  // As add2 already has a definition of (int) -> int, the compiler
                  // will regard this as an error.
If a function returns nothing, its return type is written as void. If it takes no parameters, the parameter list should
be empty.
void do_something(); // The function takes no parameters, and does not return anything.
                     // Note that it can still affect variables it has access to.
Function Call
A function can be called after it has been declared. For example, the following program calls add2 with the value of
2 within the function of main:
#include <iostream>
int add2(int i);    // Declaration of add2
// Note: add2 is still missing a DEFINITION.
// Even though it doesn't appear directly in code,
// add2's definition may be LINKED in from another object file.
int main()
{
    std::cout << add2(2) << "\n";  // add2(2) will be evaluated at this point,
                                   // and the result is printed.
    return 0;  
}
Here, add2(2) is the syntax for a function call.
Function Denition
A function denition* is similar to a declaration, except it also contains the code that is executed when the function
is called within its body.
An example of a function denition for add2 might be:
int add2(int i)       // Data that is passed into (int i) will be referred to by the name i
{                     // while in the function's curly brackets or "scope."
    int j = i + 2;    // Definition of a variable j as the value of i+2.
    return j;         // Returning or, in essence, substitution of j for a function call to
                      // add2.
}
Function Overloading
You can create multiple functions with the same name but dierent parameters.
int add2(int i)           // Code contained in this definition will be evaluated
{                         // when add2() is called with one parameter.
    int j = i + 2;
    return j;
}
int add2(int i, int j)    // However, when add2() is called with two parameters, the
{                         // code from the initial declaration will be overloaded,
    int k = i + j + 2 ;   // and the code in this declaration will be evaluated
    return k;             // instead.
}
Both functions are called by the same name add2, but the actual function that is called depends directly on the
amount and type of the parameters in the call. In most cases, the C++ compiler can compute which function to call.
In some cases, the type must be explicitly stated.
Default Parameters
Default values for function parameters can only be specied in function declarations.
int multiply(int a, int b = 7); // b has default value of 7.
int multiply(int a, int b)
{
    return a * b;               // If multiply() is called with one parameter, the
}                               // value will be multiplied by the default, 7.
In this example, multiply() can be called with one or two parameters. If only one parameter is given, b will have
default value of 7. Default arguments must be placed in the latter arguments of the function. For example:
int multiply(int a = 10, int b = 20); // This is legal
int multiply(int a = 10, int b);      // This is illegal since int a is in the former
Special Function Calls - Operators
There exist special function calls in C++ which have dierent syntax than name_of_function(value1, value2,
value3). The most common example is that of operators.
Certain special character sequences that will be reduced to function calls by the compiler, such as !, +, -, *, %, and
<< and many more. These special characters are normally associated with non-programming usage or are used for
aesthetics (e.g. the + character is commonly recognized as the addition symbol both within C++ programming as
well as in elementary math).
C++ handles these character sequences with a special syntax; but, in essence, each occurrence of an operator is
reduced to a function call. For example, the following C++ expression:
3+3
is equivalent to the following function call:
operator+(3, 3)
All operator function names start with operator.
While in C++'s immediate predecessor, C, operator function names cannot be assigned dierent meanings by
providing additional denitions with dierent type signatures, in C++, this is valid. "Hiding" additional function
denitions under one unique function name is referred to as operator overloading in C++, and is a relatively
common, but not universal, convention in C++.
Section 1.5: Visibility of function prototypes and declarations
In C++, code must be declared or dened before usage. For example, the following produces a compile time error:
int main()
{
  foo(2); // error: foo is called, but has not yet been declared
}
void foo(int x) // this later definition is not known in main
{
}
There are two ways to resolve this: putting either the denition or declaration of foo() before its usage in main().
Here is one example:
void foo(int x) {}  //Declare the foo function and body first
int main()
{
  foo(2); // OK: foo is completely defined beforehand, so it can be called here.
}
However it is also possible to "forward-declare" the function by putting only a "prototype" declaration before its
usage and then dening the function body later:
void foo(int);  // Prototype declaration of foo, seen by main
                // Must specify return type, name, and argument list types
int main()
{
  foo(2); // OK: foo is known, called even though its body is not yet defined
}
void foo(int x) //Must match the prototype
{
    // Define body of foo here
}
The prototype must specify the return type (void), the name of the function (foo), and the argument list variable
types (int), but the names of the arguments are NOT required.
One common way to integrate this into the organization of source les is to make a header le containing all of the
prototype declarations:
// foo.h
void foo(int); // prototype declaration
and then provide the full denition elsewhere:
// foo.cpp --> foo.o
#include "foo.h" // foo's prototype declaration is "hidden" in here
void foo(int x) { } // foo's body definition
and then, once compiled, link the corresponding object le foo.o into the compiled object le where it is used in
the linking phase, main.o:
// main.cpp --> main.o
#include "foo.h" // foo's prototype declaration is "hidden" in here
int main() { foo(2); } // foo is valid to call because its prototype declaration was beforehand.
// the prototype and body definitions of foo are linked through the object files
An unresolved external symbol error occurs when the function prototype and call exist, but the function body is
not dened. These can be trickier to resolve as the compiler won't report the error until the nal linking stage, and
it doesn't know which line to jump to in the code to show the error.
Section 1.6: Preprocessor
The preprocessor is an important part of the compiler.
It edits the source code, cutting some bits out, changing others, and adding other things.
In source les, we can include preprocessor directives. These directives tells the preprocessor to perform specic
actions. A directive starts with a # on a new line. Example:
#define ZERO 0
The rst preprocessor directive you will meet is probably the
#include <something>
directive. What it does is takes all of something and inserts it in your le where the directive was. The hello world
program starts with the line
#include <iostream>
This line adds the functions and objects that let you use the standard input and output.
The C language, which also uses the preprocessor, does not have as many header les as the C++ language, but in
C++ you can use all the C header les.
The next important directive is probably the
#define something something_else
directive. This tells the preprocessor that as it goes along the le, it should replace every occurrence of something
with something_else. It can also make things similar to functions, but that probably counts as advanced C++.
The something_else is not needed, but if you dene something as nothing, then outside preprocessor directives, all
occurrences of something will vanish.
This actually is useful, because of the #if,#else and #ifdef directives. The format for these would be the following:
#if something==true
//code
#else
//more code
#endif
#ifdef thing_that_you_want_to_know_if_is_defined
//code
#endif
These directives insert the code that is in the true bit, and deletes the false bits. this can be used to have bits of
code that are only included on certain operating systems, without having to rewrite the whole code.

# Professional Notes: Chapter 2: Literals

Traditionally, a literal is an expression denoting a constant whose type and value are evident from its spelling. For
example, 42 is a literal, while x is not since one must see its declaration to know its type and read previous lines of
code to know its value.
However, C++11 also added user-dened literals, which are not literals in the traditional sense but can be used as a
shorthand for function calls.
Section 2.1: this
Within a member function of a class, the keyword this is a pointer to the instance of the class on which the
function was called. this cannot be used in a static member function.
struct S {
    int x;
    S& operator=(const S& other) {
        x = other.x;
        // return a reference to the object being assigned to
        return *this;
    }
};
The type of this depends on the cv-qualication of the member function: if X::f is const, then the type of this
within f is const X*, so this cannot be used to modify non-static data members from within a const member
function. Likewise, this inherits volatile qualication from the function it appears in.
Version  C++11
this can also be used in a brace-or-equal-initializer for a non-static data member.
struct S;
struct T {
    T(const S* s);
    // ...
};
struct S {
    // ...
    T t{this};
};
this is an rvalue, so it cannot be assigned to.
Section 2.2: Integer literal
An integer literal is a primary expression of the form
decimal-literal
It is a non-zero decimal digit (1, 2, 3, 4, 5, 6, 7, 8, 9), followed by zero or more decimal digits (0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
int d = 42;
octal-literal
It is the digit zero (0) followed by zero or more octal digits (0, 1, 2, 3, 4, 5, 6, 7)
int o = 052
hex-literal
It is the character sequence 0x or the character sequence 0X followed by one or more hexadecimal digits (0, 1, 2, 3,
4, 5, 6, 7, 8, 9, a, A, b, B, c, C, d, D, e, E, f, F)
int x = 0x2a; int X = 0X2A;
binary-literal (since C++14)
It is the character sequence 0b or the character sequence 0B followed by one or more binary digits (0, 1)
int b = 0b101010; // C++14
Integer-sux, if provided, may contain one or both of the following (if both are provided, they may appear in any
order:
unsigned-sux (the character u or the character U)
unsigned int u_1 = 42u;
long-sux (the character l or the character L) or the long-long-sux (the character sequence ll or the
character sequence LL) (since C++11)
The following variables are also initialized to the same value:
unsigned long long l1 = 18446744073709550592ull; // C++11
unsigned long long l2 = 18'446'744'073'709'550'592llu; // C++14
unsigned long long l3 = 1844'6744'0737'0955'0592uLL; // C++14
unsigned long long l4 = 184467'440737'0'95505'92LLU; // C++14
Notes
Letters in the integer literals are case-insensitive: 0xDeAdBaBeU and 0XdeadBABEu represent the same number
(one exception is the long-long-sux, which is either ll or LL, never lL or Ll)
There are no negative integer literals. Expressions such as -1 apply the unary minus operator to the value
represented by the literal, which may involve implicit type conversions.
In C prior to C99 (but not in C++), unsuxed decimal values that do not t in long int are allowed to have the type
unsigned long int.
When used in a controlling expression of #if or #elif, all signed integer constants act as if they have type
std::intmax_t and all unsigned integer constants act as if they have type std::uintmax_t.
Section 2.3: true
A keyword denoting one of the two possible values of type bool.
bool ok = true;
if (!f()) {
    ok = false;
    goto end;
}
Section 2.4: false
A keyword denoting one of the two possible values of type bool.
bool ok = true;
if (!f()) {
    ok = false;
    goto end;
}
Section 2.5: nullptr
Version  C++11
A keyword denoting a null pointer constant. It can be converted to any pointer or pointer-to-member type, yielding
a null pointer of the resulting type.
Widget* p = new Widget();
delete p;
p = nullptr; // set the pointer to null after deletion
Note that nullptr is not itself a pointer. The type of nullptr is a fundamental type known as std::nullptr_t.
void f(int* p);
template <class T>
void g(T* p);
void h(std::nullptr_t p);
int main() {
    f(nullptr); // ok
    g(nullptr); // error
    h(nullptr); // ok
}

# Professional Notes: Chapter 4: Floating Point Arithmetic

Section 4.1: Floating Point Numbers are Weird
The rst mistake that nearly every single programmer makes is presuming that this code will work as intended:
float total = 0;
for(float a = 0; a != 2; a += 0.01f) {
    total += a;
}
The novice programmer assumes that this will sum up every single number in the range 0, 0.01, 0.02, 0.03,
..., 1.97, 1.98, 1.99, to yield the result 199the mathematically correct answer.
Two things happen that make this untrue:
1.
2.
The program as written never concludes. a never becomes equal to 2, and the loop never terminates.
If we rewrite the loop logic to check a < 2 instead, the loop terminates, but the total ends up being
something dierent from 199. On IEEE754-compliant machines, it will often sum up to about 201 instead.
The reason that this happens is that Floating Point Numbers represent Approximations of their assigned
values.
The classical example is the following computation:
double a = 0.1;
double b = 0.2;
double c = 0.3;
if(a + b == c)
    //This never prints on IEEE754-compliant machines
    std::cout << "This Computer is Magic!" << std::endl;
else
    std::cout << "This Computer is pretty normal, all things considered." << std::endl;
Though what we the programmer see is three numbers written in base10, what the compiler (and the underlying
hardware) see are binary numbers. Because 0.1, 0.2, and 0.3 require perfect division by 10which is quite easy in
a base-10 system, but impossible in a base-2 systemthese numbers have to be stored in imprecise formats,
similar to how the number 1/3 has to be stored in the imprecise form 0.333333333333333... in base-10.
//64-bit floats have 53 digits of precision, including the whole-number-part.
double a =     0011111110111001100110011001100110011001100110011001100110011010; //imperfect
representation of 0.1
double b =     0011111111001001100110011001100110011001100110011001100110011010; //imperfect
representation of 0.2
double c =     0011111111010011001100110011001100110011001100110011001100110011; //imperfect
representation of 0.3
double a + b = 0011111111010011001100110011001100110011001100110011001100110100; //Note that this
is not quite equal to the "canonical" 0.3!
---
### Professional Notes: Build Systems & Automation

#### 1. The Build Process (Architectural View)
A build system automates the invocation of the compiler, assembler, and linker.
*   **Makefile**: Uses a dependency graph to determine which files need recompilation. Only rebuilds changed files.
*   **CMake**: A meta-build system that generates native build files (Makefiles, Ninja, VS Solutions).

#### 2. Linker Symbols and Name Mangling
Use tools like `nm` or `objdump` to inspect object files. Demangle names with `c++filt`.

---


---
