# The Complete C++ Programmer's Guide: From Zero to Godhood (C++98 to C++26)

**Author:** Shreejit Verma

## Preface

### About the Author
Shreejit Verma is a dedicated software engineer and quantitative developer with a passion for high-performance systems. With deep expertise in C++, distributed systems, and algorithmic trading, this book distills years of "in the trenches" experience into a single, comprehensive volume. The goal is simple: to provide the resource I wish I had when I started—a path that doesn't just teach syntax, but teaches **architecture, performance, and hardware sympathy**.

### Book Purpose & Scope
This book is not just a language reference; it is a **career accelerator**. It aims to bridge the gap between academic C++ (often stuck in 1998) and the high-frequency trading/low-latency engineering standards of 2026.

**We cover:**
*   **Legacy to Modern**: From `void*` and `malloc` to `std::unique_ptr` and `std::pmr`.
*   **Standards Evolution**: A strict chronological progression from C++98 through C++23, with a preview of C++26.
*   **Systems Programming**: Memory models, atomics, networking, and compiler internals.
*   **Optimization**: SIMD, cache locality, branch prediction, and lock-free data structures.

### Target Audience
1.  **The Student**: Start at **Chapter 1**. Do not skip the "Deep Dive" sections; they lay the groundwork for understanding *why* things crash.
2.  **The Professional**: Use **Chapters 4-8** to update your stack to Modern C++.
3.  **The Specialist**: Jump to **Part 17 (Low Latency)** or **Part 24 (Architecture)** for domain-specific mastery.

### Structure of the Book
The book is divided into **33 Chapters** (formerly "Parts") organized into **5 Phases**:

*   **Phase I: The Foundation (Chapters 1-3.5)**
    *   Core syntax, OOP mechanics, and the "Classic" STL.
    *   *Goal*: Write correct, compiling C++98 code.
*   **Phase II: The Modern Renaissance (Chapters 4-6)**
    *   C++11, C++14, and C++17. Move semantics, Lambdas, Smart Pointers.
    *   *Goal*: Write safe, expressive, and efficient code.
*   **Phase III: The Conceptual Revolution (Chapters 7-8)**
    *   C++20/23. Concepts, Ranges, Coroutines, Modules.
    *   *Goal*: Write generic, composable, and modular libraries.
*   **Phase IV: Systems & Architecture (Chapters 9-16)**
    *   Metaprogramming, Memory Models, Distributed Systems.
    *   *Goal*: Design scalable systems that span threads and machines.
*   **Phase V: Godhood (Chapters 17-33)**
    *   Hardware Sympathy, SIMD, Custom Allocators, Compiler Construction.
    *   *Goal*: Squeeze every nanosecond out of the CPU.

### Conventions & Style Guide
To ensure clarity, this book follows strict conventions:

*   **Code Standards**: All code examples use `Allman` or `K&R` brace styles and `snake_case` for variables, consistent with the C++ Standard Library.
*   **Terminology**:
    *   **UB**: Undefined Behavior (The compiler can do anything).
    *   **Ill-formed**: The code will not compile.
    *   **ODR**: One Definition Rule.
*   **Measurable Outcomes**: Each major section concludes with a "Skill Check" or "Implementation Task" to verify mastery.
*   **Professional Notes**: Selected chapters include a "Professional Notes & Tricks" section extracted from expert references, providing practical tips, edge cases, and industry-standard patterns.

---

## Table of Contents

### Volume I: Foundations
*   Chapter 1: [ABSOLUTE BASICS (C++98)](#chapter-1-absolutebasicsc98)
*   Chapter 2: [THE C++ COMPILATION & EXECUTION MODEL](#chapter-2-theccompilationexecutionmodel)
*   Chapter 3: [OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS](#chapter-3-objectorientedprogrammingfundamentals)
*   Chapter 4: [DEEP OBJECT MODEL & VIRTUALIZATION](#chapter-4-deepobjectmodelvirtualization)
*   Chapter 5: [C++98/03 STANDARD LIBRARY](#chapter-5-c9803standardlibrary)
*   Chapter 6: [STL INTERNALS DEEP DIVE](#chapter-6-stlinternalsdeepdive)
### Volume II: The Modern Renaissance
*   Chapter 7: [C++11 REVOLUTION](#chapter-7-c11revolution)
*   Chapter 8: [ADVANCED MOVE SEMANTICS & VALUE CATEGORIES](#chapter-8-advancedmovesemanticsvaluecategories)
*   Chapter 9: [C++14 ENHANCEMENTS](#chapter-9-c14enhancements)
*   Chapter 10: [C++17 MODERN FEATURES](#chapter-10-c17modernfeatures)
### Volume III: Modern Mastery
*   Chapter 11: [C++20 REVOLUTIONARY FEATURES](#chapter-11-c20revolutionaryfeatures)
*   Chapter 12: [C++23 LATEST FEATURES](#chapter-12-c23latestfeatures)
*   Chapter 13: [THE FUTURE - C++26 PREVIEW](#chapter-13-thefuturec26preview)
### Volume IV: Systems & Architecture
*   Chapter 14: [ADVANCED TOPICS](#chapter-14-advancedtopics)
*   Chapter 15: [PRODUCTION & PROFESSIONAL](#chapter-15-productionprofessional)
*   Chapter 16: [SYSTEM DESIGN CASE STUDIES (C++ EDITION)](#chapter-16-systemdesigncasestudiescedition)
*   Chapter 17: [CONCURRENCY DESIGN PATTERNS](#chapter-17-concurrencydesignpatterns)
*   Chapter 18: [THE C++ BUILD ECOSYSTEM MASTERY](#chapter-18-thecbuildecosystemmastery)
### Volume V: High Performance & Low Latency
*   Chapter 19: [LOW-LATENCY C++ OPTIMIZATION](#chapter-19-lowlatencycoptimization)
*   Chapter 20: [LOW-LATENCY SYSTEM ARCHITECTURE](#chapter-20-lowlatencysystemarchitecture)
*   Chapter 21: [EXTREME LOW LATENCY & HARDWARE MASTERY](#chapter-21-extremelowlatencyhardwaremastery)
*   Chapter 22: [ADVANCED SIMD (AVX2 & AVX-512)](#chapter-22-advancedsimdavx2avx512)
*   Chapter 23: [CUSTOM MEMORY ALLOCATORS](#chapter-23-custommemoryallocators)
### Volume VI: Deep Internals
*   Chapter 24: [C++ UNDER THE HOOD](#chapter-24-cunderthehood)
*   Chapter 25: [MASTERING THE MEMORY MODEL](#chapter-25-masteringthememorymodel)
*   Chapter 26: [WRITING A C++ COMPILER (BASICS)](#chapter-26-writingaccompilerbasics)
*   Chapter 27: [WRITING A GARBAGE COLLECTOR](#chapter-27-writingagarbagecollector)
*   Chapter 28: [THE STANDARD LIBRARY FROM SCRATCH](#chapter-28-thestandardlibraryfromscratch)
### Volume VII: Specialized Domains
*   Chapter 29: [DISTRIBUTED C++](#chapter-29-distributedc)
*   Chapter 30: [NETWORKING FROM SCRATCH](#chapter-30-networkingfromscratch)
*   Chapter 31: [C++ IN THE CLOUD](#chapter-31-cinthecloud)
*   Chapter 32: [CROSS-PLATFORM DEVELOPMENT](#chapter-32-crossplatformdevelopment)
*   Chapter 33: [GUI DEVELOPMENT WITH C++](#chapter-33-guidevelopmentwithc)
*   Chapter 34: [SCIENTIFIC COMPUTING & GPU](#chapter-34-scientificcomputinggpu)
*   Chapter 35: [INTEROPERABILITY](#chapter-35-interoperability)
*   Chapter 36: [SECURITY ENGINEERING](#chapter-36-securityengineering)
*   Chapter 37: [SPECIALIZED DOMAINS](#chapter-37-specializeddomains)
### Volume VIII: Expert Mastery
*   Chapter 38: [ABA PROBLEM & MEMORY RECLAMATION](#chapter-38-abaproblemmemoryreclamation)
*   Chapter 39: [TEMPLATE METAPROGRAMMING PATTERNS](#chapter-39-templatemetaprogrammingpatterns)
*   Chapter 40: [HIGH-PERFORMANCE DATA STRUCTURES](#chapter-40-highperformancedatastructures)
*   Chapter 41: [REAL-TIME AUDIO & SIGNAL PROCESSING](#chapter-41-realtimeaudiosignalprocessing)
*   Chapter 42: [ROBOTICS & ROS2 DEVELOPMENT](#chapter-42-roboticsros2development)
*   Chapter 43: [MACHINE LEARNING INFRASTRUCTURE](#chapter-43-machinelearninginfrastructure)
*   Chapter 44: [DATABASE INTERNALS (LSM TREES)](#chapter-44-databaseinternalslsmtrees)
### Volume IX: Final Reference
*   Chapter 45: [THE ULTIMATE ALGORITHM REFERENCE](#chapter-45-theultimatealgorithmreference)
*   Chapter 46: [CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK](#chapter-46-capstoneprojecthighperformanceorderbook)

---

# Volume I: Foundations

## <a name="chapter-1-absolutebasicsc98"></a>CHAPTER 1: ABSOLUTE BASICS (C++98)

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

# SECTION 1: ADVANCED POINTERS & MEMORY

## 1.1 Pointer to Const vs Const Pointer

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 5, y = 10;
    
    // Pointer to const - can't modify data
    const int* ptr1 = &x;
    // *ptr1 = 10;  // ERROR
    ptr1 = &y;      // OK - can change pointer
    
    // Const pointer - can't modify pointer
    int* const ptr2 = &x;
    *ptr2 = 10;     // OK - can change data
    // ptr2 = &y;   // ERROR
    
    // Const pointer to const - can't modify either
    const int* const ptr3 = &x;
    // *ptr3 = 10;  // ERROR
    // ptr3 = &y;   // ERROR
    
    return 0;
}
```

## 1.2 Void Pointers

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 42;
    double y = 3.14;
    
    // Void pointer can point to any type
    void* ptr = &x;
    cout << *(int*)ptr << endl;  // 42
    
    ptr = &y;
    cout << *(double*)ptr << endl;  // 3.14
    
    // Generic function using void*
    void print_value(void* ptr, char type) {
        if (type == 'i') {
            cout << *(int*)ptr << endl;
        } else if (type == 'd') {
            cout << *(double*)ptr << endl;
        }
    }
    
    print_value(&x, 'i');  // 42
    print_value(&y, 'd');  // 3.14
    
    return 0;
}
```

## 1.3 Null Pointer Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int* ptr = NULL;  // Set to null
    
    // Always check before dereferencing
    if (ptr != NULL) {
        cout << *ptr << endl;
    } else {
        cout << "Pointer is NULL" << endl;
    }
    
    // Safer approach
    ptr = new int(42);
    if (ptr) {
        cout << *ptr << endl;
        delete ptr;
        ptr = NULL;
    }
    
    return 0;
}
```

## 1.4 Memory Layout & Alignment

```cpp
#include <iostream>
using namespace std;

int main() {
    struct Data {
        char a;     // 1 byte
        int b;      // 4 bytes
        double c;   // 8 bytes
    };
    
    cout << "Size of Data: " << sizeof(Data) << endl;
    // Likely 16 or 24 (due to alignment padding)
    
    cout << "Size of char: " << sizeof(char) << endl;      // 1
    cout << "Size of int: " << sizeof(int) << endl;        // 4
    cout << "Size of double: " << sizeof(double) << endl;  // 8
    
    Data data;
    cout << "Address of a: " << (void*)&data.a << endl;
    cout << "Address of b: " << (void*)&data.b << endl;
    cout << "Address of c: " << (void*)&data.c << endl;
    
    return 0;
}
```

---

# SECTION 2: ADVANCED FUNCTIONS

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

# SECTION 3: FUNCTION POINTERS & CALLBACKS

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

# SECTION 4: ADVANCED ARRAYS

## 4.1 Dynamic Arrays

```cpp
#include <iostream>
using namespace std;

int main() {
    // 1D dynamic array
    int size = 5;
    int* arr = new int[size];
    
    for (int i = 0; i < size; i++) {
        arr[i] = i * 10;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    delete[] arr;
    arr = NULL;
    
    // 2D dynamic array
    int rows = 3, cols = 4;
    int** matrix = new int*[rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    
    // Fill matrix
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            matrix[i][j] = i * cols + j;
        }
    }
    
    // Delete matrix
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
    
    return 0;
}
```

## 4.2 Variable Length Arrays (Non-standard)

```cpp
#include <iostream>
using namespace std;

int main() {
    int size;
    cout << "Enter size: ";
    cin >> size;
    
    // VLA - not standard but supported by many compilers
    int arr[size];  // GCC extension
    
    for (int i = 0; i < size; i++) {
        arr[i] = i;
    }
    
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 4.3 Array Bounds & Safety

```cpp
#include <iostream>
using namespace std;

int main() {
    int arr[5] = {10, 20, 30, 40, 50};
    
    // No bounds checking in C++
    cout << arr[0] << endl;   // 10 (OK)
    cout << arr[10] << endl;  // Undefined behavior!
    
    // Manual bounds checking
    int index = 5;
    if (index >= 0 && index < 5) {
        cout << arr[index] << endl;
    } else {
        cout << "Index out of bounds" << endl;
    }
    
    return 0;
}
```

---

# SECTION 5: ADVANCED STRINGS

## 5.1 String Manipulation

```cpp
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

int main() {
    string s = "Hello World";
    
    // Length and capacity
    cout << "Length: " << s.length() << endl;
    cout << "Capacity: " << s.capacity() << endl;
    
    // Access characters
    cout << "First char: " << s[0] << endl;
    cout << "Last char: " << s[s.length() - 1] << endl;
    
    // Finding substrings
    size_t pos = s.find("World");
    if (pos != string::npos) {
        cout << "Found at position: " << pos << endl;
    }
    
    // Replace
    s.replace(6, 5, "C++");
    cout << s << endl;  // Hello C++
    
    // Insert
    s.insert(5, " there");
    cout << s << endl;  // Hello there C++
    
    // Erase
    s.erase(5, 6);
    cout << s << endl;  // Hello C++
    
    // Substring
    cout << s.substr(0, 5) << endl;  // Hello
    
    // Reverse
    reverse(s.begin(), s.end());
    cout << s << endl;  // ++C olleH
    
    return 0;
}
```

## 5.2 String Conversion

```cpp
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;

int main() {
    // String to number (C style)
    string s1 = "42";
    int num = atoi(s1.c_str());
    cout << num << endl;
    
    string s2 = "3.14";
    double dbl = atof(s2.c_str());
    cout << dbl << endl;
    
    // Number to string (using stringstream)
    stringstream ss;
    ss << 42 << " " << 3.14 << " " << true;
    string result = ss.str();
    cout << result << endl;  // 42 3.14 1
    
    // Reverse conversion
    stringstream ss2("100 200 300");
    int a, b, c;
    ss2 >> a >> b >> c;
    cout << a << " " << b << " " << c << endl;  // 100 200 300
    
    return 0;
}
```

## 5.3 String Tokenization

```cpp
#include <iostream>
#include <string>
#include <cstring>
using namespace std;

int main() {
    string line = "apple,banana,orange,grape";
    
    // Using stringstream and getline
    stringstream ss(line);
    string token;
    
    while (getline(ss, token, ',')) {
        cout << token << endl;
    }
    // Output: apple, banana, orange, grape
    
    // Using strtok (C style)
    char str[] = "hello world how are you";
    char* ptr = strtok(str, " ");
    
    while (ptr != NULL) {
        cout << ptr << endl;
        ptr = strtok(NULL, " ");
    }
    // Output: hello, world, how, are, you
    
    return 0;
}
```

---

# SECTION 6: BITWISE OPERATIONS

## 6.1 Bitwise Operators

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned char a = 5;   // 0101
    unsigned char b = 3;   // 0011
    
    // AND
    cout << (a & b) << endl;  // 0001 = 1
    
    // OR
    cout << (a | b) << endl;  // 0111 = 7
    
    // XOR
    cout << (a ^ b) << endl;  // 0110 = 6
    
    // NOT (bitwise complement)
    cout << (~a) << endl;     // 1010 = 250 (for unsigned char)
    
    // Left shift
    cout << (a << 1) << endl; // 1010 = 10
    
    // Right shift
    cout << (b >> 1) << endl; // 0001 = 1
    
    return 0;
}
```

## 6.2 Bit Manipulation Techniques

```cpp
#include <iostream>
using namespace std;

int main() {
    unsigned int num = 5;  // 0101
    
    // Check if bit is set
    int bit_pos = 2;
    bool is_set = (num >> bit_pos) & 1;
    cout << "Bit " << bit_pos << " is: " << is_set << endl;
    
    // Set a bit
    num |= (1 << 1);  // Set bit 1
    cout << "After setting bit 1: " << num << endl;  // 7 (0111)
    
    // Clear a bit
    num &= ~(1 << 1);  // Clear bit 1
    cout << "After clearing bit 1: " << num << endl;  // 5 (0101)
    
    // Toggle a bit
    num ^= (1 << 0);  // Toggle bit 0
    cout << "After toggling bit 0: " << num << endl;  // 4 (0100)
    
    // Count set bits
    unsigned int count = 0;
    unsigned int temp = num;
    while (temp) {
        count += temp & 1;
        temp >>= 1;
    }
    cout << "Number of set bits: " << count << endl;
    
    return 0;
}
```

---

# SECTION 7: PREPROCESSOR DIRECTIVES

## 7.1 #define and #include

```cpp
// Define constants
#define PI 3.14159
#define MAX_SIZE 100
#define SQUARE(x) ((x) * (x))

// Conditional compilation
#define DEBUG

#ifdef DEBUG
    #define LOG(msg) cout << msg << endl
#else
    #define LOG(msg)  // Do nothing in release
#endif

#include <iostream>
using namespace std;

int main() {
    cout << "PI = " << PI << endl;
    
    int arr[MAX_SIZE];
    cout << "Array size: " << sizeof(arr) << endl;
    
    cout << "Square of 5: " << SQUARE(5) << endl;
    
    LOG("Debug message");
    
    return 0;
}
```

## 7.2 Conditional Compilation

```cpp
#include <iostream>
using namespace std;

// Platform-specific code
#ifdef _WIN32
    #define OS "Windows"
#elif __APPLE__
    #define OS "macOS"
#elif __linux__
    #define OS "Linux"
#else
    #define OS "Unknown"
#endif

int main() {
    cout << "Running on: " << OS << endl;
    
#if defined(DEBUG)
    cout << "Debug mode" << endl;
#else
    cout << "Release mode" << endl;
#endif
    
    return 0;
}
```

## 7.3 Pragma Directives

```cpp
#include <iostream>
using namespace std;

// Disable specific warnings
#pragma warning(disable: 4996)  // MSVC

// Pack structure
#pragma pack(1)
struct PackedData {
    char a;
    int b;
    double c;
};
#pragma pack()

int main() {
    cout << "Size of PackedData: " << sizeof(PackedData) << endl;
    // Without pragma pack: 24 (aligned)
    // With pragma pack: 13 (packed)
    
    return 0;
}
```

---

# SECTION 8: TYPE CASTING

## 8.1 C-Style Casting

```cpp
#include <iostream>
#include <cmath>
using namespace std;

int main() {
    double d = 3.14;
    
    // C-style cast (avoid in modern C++)
    int i = (int)d;  // 3
    cout << i << endl;
    
    int x = 65;
    char c = (char)x;  // 'A'
    cout << c << endl;
    
    float f = (float)d;
    cout << f << endl;
    
    return 0;
}
```

## 8.2 Implicit Conversions

```cpp
#include <iostream>
using namespace std;

int main() {
    // Implicit conversions
    int x = 5;
    double d = x;  // int to double (automatic)
    cout << d << endl;  // 5.0
    
    double d2 = 3.9;
    int y = d2;  // double to int (loses precision)
    cout << y << endl;  // 3
    
    // Char arithmetic
    char c = 'A';
    int code = c;  // char to int (gets ASCII)
    cout << code << endl;  // 65
    
    return 0;
}
```

---

# SECTION 9: ADVANCED CONTROL FLOW

## 9.1 Ternary Operator

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10, y = 5;
    
    // condition ? true_value : false_value
    int max = (x > y) ? x : y;
    cout << "Max: " << max << endl;  // 10
    
    // Nested ternary (use with caution)
    int age = 20;
    string status = (age < 18) ? "Minor" : (age < 65) ? "Adult" : "Senior";
    cout << status << endl;
    
    // String ternary
    cout << (x % 2 == 0 ? "Even" : "Odd") << endl;
    
    return 0;
}
```

## 9.2 goto Statement (Avoid)

```cpp
#include <iostream>
using namespace std;

int main() {
    // goto is generally discouraged
    int x = 0;
    
loop:
    cout << x << " ";
    x++;
    
    if (x < 5) {
        goto loop;
    }
    cout << endl;
    
    // Better alternative: use loops
    for (int i = 0; i < 5; i++) {
        cout << i << " ";
    }
    cout << endl;
    
    return 0;
}
```

## 9.3 Label & Goto for Error Handling

```cpp
#include <iostream>
#include <cstdlib>
using namespace std;

int main() {
    FILE* file = NULL;
    char* buffer = NULL;
    
    // Using goto for cleanup (rare acceptable use)
    file = fopen("test.txt", "r");
    if (!file) {
        cout << "Failed to open file" << endl;
        goto cleanup;
    }
    
    buffer = new char[100];
    if (!buffer) {
        cout << "Memory allocation failed" << endl;
        goto cleanup;
    }
    
    // Do work...
    
cleanup:
    if (buffer) delete[] buffer;
    if (file) fclose(file);
    
    return 0;
}
```

---

# SECTION 10: ENUMERATION & UNIONS

## 10.1 Enumerations

```cpp
#include <iostream>
using namespace std;

// Enum definition
enum Color { RED, GREEN, BLUE };

enum Direction {
    NORTH = 0,
    EAST = 1,
    SOUTH = 2,
    WEST = 3
};

int main() {
    Color c = RED;
    cout << c << endl;  // 0
    
    Color colors[3] = {RED, GREEN, BLUE};
    
    // Switching on enum
    switch (c) {
        case RED:
            cout << "Red" << endl;
            break;
        case GREEN:
            cout << "Green" << endl;
            break;
        case BLUE:
            cout << "Blue" << endl;
            break;
    }
    
    // Iterate through enum values
    for (int dir = NORTH; dir <= WEST; dir++) {
        cout << "Direction: " << dir << endl;
    }
    
    return 0;
}
```

## 10.2 Unions

```cpp
#include <iostream>
using namespace std;

// Union - all members share same memory
union Data {
    int i;
    float f;
    char c;
};

int main() {
    Data data;
    
    cout << "Size of Data: " << sizeof(data) << endl;  // 4 (size of largest member)
    
    data.i = 10;
    cout << "data.i: " << data.i << endl;     // 10
    cout << "data.f: " << data.f << endl;     // Garbage (overwrites data.i)
    
    data.f = 3.14;
    cout << "data.i: " << data.i << endl;     // Garbage (overwrites by data.f)
    cout << "data.f: " << data.f << endl;     // 3.14
    
    // Union useful for memory-constrained systems
    union Variant {
        int int_val;
        double double_val;
        char char_val;
    };
    
    cout << "Size of Variant: " << sizeof(Variant) << endl;
    
    return 0;
}
```

---

# SECTION 11: CONST & VOLATILE

## 11.1 Const Correctness

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10;
    
    // const variable
    const int constant = 5;
    // constant = 10;  // ERROR
    
    // pointer to const
    const int* ptr1 = &x;
    // *ptr1 = 20;  // ERROR
    ptr1 = &constant;  // OK
    
    // const pointer
    int* const ptr2 = &x;
    *ptr2 = 20;  // OK
    // ptr2 = &constant;  // ERROR
    
    // const reference
    const int& ref = x;
    // ref = 20;  // ERROR
    
    cout << x << endl;
    cout << *ptr1 << endl;
    cout << *ptr2 << endl;
    cout << ref << endl;
    
    return 0;
}
```

## 11.2 Volatile Keyword

```cpp
#include <iostream>
using namespace std;

int main() {
    // volatile - tells compiler value may change unexpectedly
    volatile int sensor_reading = 0;  // From hardware
    
    // Compiler won't optimize away reads
    while (sensor_reading < 100) {
        // Check actual value each time, not cached
    }
    
    // Common use: hardware registers
    volatile int* hardware_register = (volatile int*)0x1000;
    
    // Each access reads from actual location
    int val1 = *hardware_register;
    int val2 = *hardware_register;
    
    // Without volatile, compiler might optimize one read
    
    return 0;
}
```

---

# SECTION 12: INLINE FUNCTIONS & MACROS

## 12.1 Macro Functions vs Inline Functions

```cpp
#include <iostream>
using namespace std;

// Macro function - preprocessor substitution
#define ADD_MACRO(a, b) ((a) + (b))

// Inline function - type-safe
inline int add_inline(int a, int b) {
    return a + b;
}

int main() {
    cout << ADD_MACRO(5, 3) << endl;         // 8
    cout << add_inline(5, 3) << endl;       // 8
    
    // Macro danger: side effects
    int x = 5, y = 3;
    cout << ADD_MACRO(x++, y++) << endl;    // Evaluates as: ((x++) + (y++))
    cout << "x = " << x << ", y = " << y << endl;  // x = 6, y = 4
    
    // Inline function is safer
    x = 5, y = 3;
    cout << add_inline(x++, y++) << endl;   // 8
    cout << "x = " << x << ", y = " << y << endl;  // x = 6, y = 4 (correct)
    
    return 0;
}
```

---

# SECTION 13: NAMESPACES

## 13.1 Namespace Basics

```cpp
#include <iostream>
using namespace std;

// Define namespace
namespace Math {
    double PI = 3.14159;
    
    double circle_area(double radius) {
        return PI * radius * radius;
    }
}

namespace Graphics {
    double PI = 3.14;  // Different PI
    
    void draw_circle(double radius) {
        cout << "Drawing circle with radius: " << radius << endl;
    }
}

int main() {
    // Access with namespace::name
    cout << Math::PI << endl;
    cout << Graphics::PI << endl;
    
    cout << Math::circle_area(5) << endl;
    Graphics::draw_circle(5);
    
    return 0;
}
```

## 13.2 Namespace Aliases

```cpp
#include <iostream>
using namespace std;

namespace Very {
    namespace Long {
        namespace Namespace {
            void function() {
                cout << "Long namespace function" << endl;
            }
        }
    }
}

int main() {
    // Use alias to shorten
    namespace VLN = Very::Long::Namespace;
    
    VLN::function();
    
    // or use using
    using namespace Very::Long::Namespace;
    function();
    
    return 0;
}
```

---

# SECTION 14: FILE I/O ADVANCED

## 14.1 Binary File I/O

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write binary data
    ofstream outfile("data.bin", ios::binary);
    
    int numbers[] = {10, 20, 30, 40, 50};
    outfile.write((char*)numbers, sizeof(numbers));
    outfile.close();
    
    // Read binary data
    ifstream infile("data.bin", ios::binary);
    
    int buffer[5];
    infile.read((char*)buffer, sizeof(buffer));
    
    for (int i = 0; i < 5; i++) {
        cout << buffer[i] << " ";
    }
    cout << endl;
    
    infile.close();
    
    return 0;
}
```

## 14.2 Stream Positioning

```cpp
#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Write to file
    ofstream outfile("test.txt");
    outfile << "0123456789";
    outfile.close();
    
    // Read with positioning
    ifstream infile("test.txt");
    
    // Tell position
    cout << "Current position: " << infile.tellg() << endl;
    
    // Seek to position
    infile.seekg(5);
    char c;
    infile.get(c);
    cout << "Character at position 5: " << c << endl;  // '5'
    
    // Seek from end
    infile.seekg(-3, ios::end);
    infile.get(c);
    cout << "Third from end: " << c << endl;  // '7'
    
    infile.close();
    
    return 0;
}
```

---

# SECTION 15: ERROR HANDLING & DEBUGGING

## 15.1 Assert Macro

```cpp
#include <iostream>
#include <cassert>
using namespace std;

int divide(int a, int b) {
    assert(b != 0);  // b must not be zero
    return a / b;
}

int main() {
    cout << divide(10, 2) << endl;  // 5
    
    // cout << divide(10, 0) << endl;  // Assertion fails!
    
    return 0;
}
```

## 15.2 Debug Output

```cpp
#include <iostream>
#include <cstdio>
using namespace std;

#ifdef DEBUG
    #define DPRINTF(fmt, ...) printf(fmt, __VA_ARGS__)
#else
    #define DPRINTF(fmt, ...) (void)0
#endif

int main() {
    int x = 42;
    
    DPRINTF("Debug: x = %d\n", x);
    
    cout << "Regular output" << endl;
    
    
    return 0;
}
```

---



---
### Professional Notes: Foundation & Basics
#### 1: Getting started with C: 
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
#include <iostream> is a preprocessor directive that includes the content of the standard C++ header ﬁle
iostream.
iostream is a standard library header ﬁle that contains deﬁnitions of the standard input and output
streams. These deﬁnitions are included in the std namespace, explained below.
The standard input/output (I/O) streams provide ways for programs to get input from and output to an
external system -- usually the terminal.
int main() { ... } deﬁnes a new function named main. By convention, the main function is called upon
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
std::cout is the standard output stream object, deﬁned in iostream, and it prints to the standard
output (stdout).
<< is, in this context, the stream insertion operator, so called because it inserts an object into the
stream object.
The standard library deﬁnes the << operator to perform data insertion for certain data types into
output streams. stream << content inserts content into the stream and returns the same, but
updated stream. This allows stream insertions to be chained: std::cout << "Foo" << " Bar"; prints
"FooBar" to the console.
"Hello World!" is a character string literal, or a "text literal." The stream insertion operator for
character string literals is deﬁned in ﬁle iostream.
std::endl is a special I/O stream manipulator object, also deﬁned in ﬁle iostream. Inserting a
manipulator into a stream changes the state of the stream.
The stream manipulator std::endl does two things: ﬁrst it inserts the end-of-line character and then it
ﬂushes the stream buﬀer to force the text to show up on the console. This ensures that the data
inserted into the stream actually appear on your console. (Stream data is usually stored in a buﬀer and
then "ﬂushed" in batches unless you force a ﬂush immediately.)
An alternate method that avoids the ﬂush is:
std::cout << "Hello World!\n";
where \n is the character escape sequence for the newline character.
The semicolon (;) notiﬁes the compiler that a statement has ended. All C++ statements and class
deﬁnitions require an ending/terminating semicolon.
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
As with all programming languages, comments provide several beneﬁts:
Explicit documentation of code to make it easier to read/maintain
Explanation of the purpose and functionality of code
Details on the history or reasoning behind the code
Placement of copyright/licenses, project notes, special thanks, contributor credits, etc. directly in the source
code.
However, comments also have their downsides:
They must be maintained to reﬂect any changes in the code
Excessive comments tend to make the code less readable
The need for comments can be reduced by writing clear, self-documenting code. A simple example is the use of
explanatory names for variables, functions, and types. Factoring out logically related tasks into discrete functions
goes hand-in-hand with this.
Comment markers used to disable code
During development, comments can also be used to quickly disable portions of code without deleting it. This is
often useful for testing or debugging purposes, but is not good style for anything other than temporary edits. This
is often referred to as “commenting out”.
Similarly, keeping old versions of a piece of code in a comment for reference purposes is frowned upon, as it
clutters ﬁles while oﬀering little value compared to exploring the code's history via a versioning system.
Section 1.3: The standard C++ compilation process
Executable C++ program code is usually produced by a compiler.
A compiler is a program that translates code from a programming language into another form which is (more)
directly executable for a computer. Using a compiler to translate code is called compilation.
C++ inherits the form of its compilation process from its "parent" language, C. Below is a list showing the four major
steps of compilation in C++:
1.
The C++ preprocessor copies the contents of any included header ﬁles into the source code ﬁle, generates
macro code, and replaces symbolic constants deﬁned using #deﬁne with their values.
2.
The expanded source code ﬁle produced by the C++ preprocessor is compiled into assembly language
appropriate for the platform.
3.
4.
The assembler code generated by the compiler is assembled into appropriate object code for the platform.
The object code ﬁle generated by the assembler is linked together with the object code ﬁles for any library
functions used to produce an executable ﬁle.
Note: some compiled code is linked together, but not to create a ﬁnal program. Usually, this "linked" code
can also be packaged into a format that can be used by other programs. This "bundle of packaged, usable
code" is what C++ programmers refer to as a library.
Many C++ compilers may also merge or un-merge certain parts of the compilation process for ease or for additional
analysis. Many C++ programmers will use diﬀerent tools, but all of the tools will generally follow this generalized
process when they are involved in the production of a program.
The link below extends this discussion and provides a nice graphic to help. [1]:
http://faculty.cs.niu.edu/~mcmahon/CS241/Notes/compile.html
Section 1.4: Function
A function is a unit of code that represents a sequence of statements.
Functions can accept arguments or values and return a single value (or not). To use a function, a function call is
used on argument values and the use of the function call itself is replaced with its return value.
Every function has a type signature -- the types of its arguments and the type of its return type.
Functions are inspired by the concepts of the procedure and the mathematical function.
Note: C++ functions are essentially procedures and do not follow the exact deﬁnition or rules of
mathematical functions.
Functions are often meant to perform a speciﬁc task. and can be called from other parts of a program. A function
must be declared and deﬁned before it is called elsewhere in a program.
Note: popular function deﬁnitions may be hidden in other included ﬁles (often for convenience and reuse
across many ﬁles). This is a common use of header ﬁles.
Function Declaration
A function declaration is declares the existence of a function with its name and type signature to the compiler.
The syntax is as the following:
int add2(int i); // The function is of the type (int) -> (int)
In the example above, the int add2(int i) function declares the following to the compiler:
The return type is int.
The name of the function is add2.
The number of arguments to the function is 1:
The ﬁrst argument is of the type int.
The ﬁrst argument will be referred to in the function's contents by the name i.
The argument name is optional; the declaration for the function could also be the following:
int add2(int); // Omitting the function arguments' name is also permitted.
Per the one-deﬁnition rule, a function with a certain type signature can only be declared or deﬁned once in an
entire C++ code base visible to the C++ compiler. In other words, functions with a speciﬁc type signature cannot be
re-deﬁned -- they must only be deﬁned once. Thus, the following is not valid C++:
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
Function Deﬁnition
A function deﬁnition* is similar to a declaration, except it also contains the code that is executed when the function
is called within its body.
An example of a function deﬁnition for add2 might be:
int add2(int i)       // Data that is passed into (int i) will be referred to by the name i
{                     // while in the function's curly brackets or "scope."
    int j = i + 2;    // Definition of a variable j as the value of i+2.
    return j;         // Returning or, in essence, substitution of j for a function call to
                      // add2.
}
Function Overloading
You can create multiple functions with the same name but diﬀerent parameters.
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
Default values for function parameters can only be speciﬁed in function declarations.
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
There exist special function calls in C++ which have diﬀerent syntax than name_of_function(value1, value2,
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
While in C++'s immediate predecessor, C, operator function names cannot be assigned diﬀerent meanings by
providing additional deﬁnitions with diﬀerent type signatures, in C++, this is valid. "Hiding" additional function
deﬁnitions under one unique function name is referred to as operator overloading in C++, and is a relatively
common, but not universal, convention in C++.
Section 1.5: Visibility of function prototypes and declarations
In C++, code must be declared or deﬁned before usage. For example, the following produces a compile time error:
int main()
{
  foo(2); // error: foo is called, but has not yet been declared
}
void foo(int x) // this later definition is not known in main
{
}
There are two ways to resolve this: putting either the deﬁnition or declaration of foo() before its usage in main().
Here is one example:
void foo(int x) {}  //Declare the foo function and body first
int main()
{
  foo(2); // OK: foo is completely defined beforehand, so it can be called here.
}
However it is also possible to "forward-declare" the function by putting only a "prototype" declaration before its
usage and then deﬁning the function body later:
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
One common way to integrate this into the organization of source ﬁles is to make a header ﬁle containing all of the
prototype declarations:
// foo.h
void foo(int); // prototype declaration
and then provide the full deﬁnition elsewhere:
// foo.cpp --> foo.o
#include "foo.h" // foo's prototype declaration is "hidden" in here
void foo(int x) { } // foo's body definition
and then, once compiled, link the corresponding object ﬁle foo.o into the compiled object ﬁle where it is used in
the linking phase, main.o:
// main.cpp --> main.o
#include "foo.h" // foo's prototype declaration is "hidden" in here
int main() { foo(2); } // foo is valid to call because its prototype declaration was beforehand.
// the prototype and body definitions of foo are linked through the object files
An “unresolved external symbol” error occurs when the function prototype and call exist, but the function body is
not deﬁned. These can be trickier to resolve as the compiler won't report the error until the ﬁnal linking stage, and
it doesn't know which line to jump to in the code to show the error.
Section 1.6: Preprocessor
The preprocessor is an important part of the compiler.
It edits the source code, cutting some bits out, changing others, and adding other things.
In source ﬁles, we can include preprocessor directives. These directives tells the preprocessor to perform speciﬁc
actions. A directive starts with a # on a new line. Example:
#define ZERO 0
The ﬁrst preprocessor directive you will meet is probably the
#include <something>
directive. What it does is takes all of something and inserts it in your ﬁle where the directive was. The hello world
program starts with the line
#include <iostream>
This line adds the functions and objects that let you use the standard input and output.
The C language, which also uses the preprocessor, does not have as many header ﬁles as the C++ language, but in
C++ you can use all the C header ﬁles.
The next important directive is probably the
#define something something_else
directive. This tells the preprocessor that as it goes along the ﬁle, it should replace every occurrence of something
with something_else. It can also make things similar to functions, but that probably counts as advanced C++.
The something_else is not needed, but if you deﬁne something as nothing, then outside preprocessor directives, all
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
#### 2: Literals
Traditionally, a literal is an expression denoting a constant whose type and value are evident from its spelling. For
example, 42 is a literal, while x is not since one must see its declaration to know its type and read previous lines of
code to know its value.
However, C++11 also added user-deﬁned literals, which are not literals in the traditional sense but can be used as a
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
The type of this depends on the cv-qualiﬁcation of the member function: if X::f is const, then the type of this
within f is const X*, so this cannot be used to modify non-static data members from within a const member
function. Likewise, this inherits volatile qualiﬁcation from the function it appears in.
Version ≥ C++11
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
Integer-suﬃx, if provided, may contain one or both of the following (if both are provided, they may appear in any
order:
unsigned-suﬃx (the character u or the character U)
unsigned int u_1 = 42u;
long-suﬃx (the character l or the character L) or the long-long-suﬃx (the character sequence ll or the
character sequence LL) (since C++11)
The following variables are also initialized to the same value:
unsigned long long l1 = 18446744073709550592ull; // C++11
unsigned long long l2 = 18'446'744'073'709'550'592llu; // C++14
unsigned long long l3 = 1844'6744'0737'0955'0592uLL; // C++14
unsigned long long l4 = 184467'440737'0'95505'92LLU; // C++14
Notes
Letters in the integer literals are case-insensitive: 0xDeAdBaBeU and 0XdeadBABEu represent the same number
(one exception is the long-long-suﬃx, which is either ll or LL, never lL or Ll)
There are no negative integer literals. Expressions such as -1 apply the unary minus operator to the value
represented by the literal, which may involve implicit type conversions.
In C prior to C99 (but not in C++), unsuﬃxed decimal values that do not ﬁt in long int are allowed to have the type
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
Version ≥ C++11
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
#### 11: Loops
A loop statement executes a group of statements repeatedly until a condition is met. There are 3 types of primitive
loops in C++: for, while, and do...while.
Section 11.1: Range-Based For
Version ≥ C++11
for loops can be used to iterate over the elements of a iterator-based range, without using a numeric index or
directly accessing the iterators:
vector<float> v = {0.4f, 12.5f, 16.234f};
for(auto val: v)
{
    std::cout << val << " ";
}
std::cout << std::endl;
This will iterate over every element in v, with val getting the value of the current element. The following statement:
for (for-range-declaration : for-range-initializer ) statement
is equivalent to:
{
    auto&& __range = for-range-initializer;
    auto __begin = begin-expr, __end = end-expr;
    for (; __begin != __end; ++__begin) {
        for-range-declaration = *__begin;
        statement
    }
}
Version ≥ C++17
{
    auto&& __range = for-range-initializer;
    auto __begin = begin-expr;
    auto __end = end-expr; // end is allowed to be a different type than begin in C++17
    for (; __begin != __end; ++__begin) {
        for-range-declaration = *__begin;
        statement
    }
}
This change was introduced for the planned support of Ranges TS in C++20.
In this case, our loop is equivalent to:
{
    auto&& __range = v;
    auto __begin = v.begin(), __end = v.end();
    for (; __begin != __end; ++__begin) {
        auto val = *__begin;
        std::cout << val << " ";
    }
}
Note that auto val declares a value type, which will be a copy of a value stored in the range (we are copy-initializing
it from the iterator as we go). If the values stored in the range are expensive to copy, you may want to use const
auto &val. You are also not required to use auto; you can use an appropriate typename, so long as it is implicitly
convertible from the range's value type.
If you need access to the iterator, range-based for cannot help you (not without some eﬀort, at least).
If you wish to reference it, you may do so:
vector<float> v = {0.4f, 12.5f, 16.234f};
for(float &val: v)
{
    std::cout << val << " ";
}
You could iterate on const reference if you have const container:
const vector<float> v = {0.4f, 12.5f, 16.234f};
for(const float &val: v)
{
    std::cout << val << " ";
}
One would use forwarding references when the sequence iterator returns a proxy object and you need to operate
on that object in a non-const way. Note: it will most likely confuse readers of your code.
vector<bool> v(10);
for(auto&& val: v)
{
    val = true;
}
The "range" type provided to range-based for can be one of the following:
Language arrays:
float arr[] = {0.4f, 12.5f, 16.234f};
for(auto val: arr)
{
    std::cout << val << " ";
}
Note that allocating a dynamic array does not count:
float *arr = new float[3]{0.4f, 12.5f, 16.234f};
for(auto val: arr) //Compile error.
{
    std::cout << val << " ";
}
Any type which has member functions begin() and end(), which return iterators to the elements of the type.
The standard library containers qualify, but user-deﬁned types can be used as well:
struct Rng
{
    float arr[3];
    // pointers are iterators
    const float* begin() const {return &arr[0];}
    const float* end() const   {return &arr[3];}
    float* begin() {return &arr[0];}
    float* end()   {return &arr[3];}
};
int main()
{
    Rng rng = {{0.4f, 12.5f, 16.234f}};
    for(auto val: rng)
    {
        std::cout << val << " ";
    }
}
Any type which has non-member begin(type) and end(type) functions which can found via argument
dependent lookup, based on type. This is useful for creating a range type without having to modify class type
itself:
namespace Mine
{
    struct Rng {float arr[3];};
    // pointers are iterators
    const float* begin(const Rng &rng) {return &rng.arr[0];}
    const float* end(const Rng &rng) {return &rng.arr[3];}
    float* begin(Rng &rng) {return &rng.arr[0];}
    float* end(Rng &rng) {return &rng.arr[3];}
}
int main()
{
    Mine::Rng rng = {{0.4f, 12.5f, 16.234f}};
    for(auto val: rng)
    {
        std::cout << val << " ";
    }
}
Section 11.2: For loop
A for loop executes statements in the loop body, while the loop condition is true. Before the loop initialization
statement is executed exactly once. After each cycle, the iteration execution part is executed.
A for loop is deﬁned as follows:
for (/*initialization statement*/; /*condition*/; /*iteration execution*/)
{
    // body of the loop
}
Explanation of the placeholder statements:
initialization statement: This statement gets executed only once, at the beginning of the for loop. You
can enter a declaration of multiple variables of one type, such as int i = 0, a = 2, b = 3. These variables
are only valid in the scope of the loop. Variables deﬁned before the loop with the same name are hidden
during execution of the loop.
condition: This statement gets evaluated ahead of each loop body execution, and aborts the loop if it
evaluates to false.
iteration execution: This statement gets executed after the loop body, ahead of the next condition
evaluation, unless the for loop is aborted in the body (by break, goto, return or an exception being thrown).
You can enter multiple statements in the iteration execution part, such as a++, b+=10, c=b+a.
The rough equivalent of a for loop, rewritten as a while loop is:
/*initialization*/
while (/*condition*/)
{
    // body of the loop; using 'continue' will skip to increment part below
    /*iteration execution*/
}
The most common case for using a for loop is to execute statements a speciﬁc number of times. For example,
consider the following:
for(int i = 0; i < 10; i++) {
    std::cout << i << std::endl;
}
A valid loop is also:
for(int a = 0, b = 10, c = 20; (a+b+c < 100); c--, b++, a+=c) {
    std::cout << a << " " << b << " " << c << std::endl;
}
An example of hiding declared variables before a loop is:
int i = 99; //i = 99
for(int i = 0; i < 10; i++) { //we declare a new variable i
    //some operations, the value of i ranges from 0 to 9 during loop execution
}
//after the loop is executed, we can access i with value of 99
But if you want to use the already declared variable and not hide it, then omit the declaration part:
int i = 99; //i = 99
for(i = 0; i < 10; i++) { //we are using already declared variable i
    //some operations, the value of i ranges from 0 to 9 during loop execution
}
//after the loop is executed, we can access i with value of 10
Notes:
The initialization and increment statements can perform operations unrelated to the condition statement, or
nothing at all - if you wish to do so. But for readability reasons, it is best practice to only perform operations
directly relevant to the loop.
A variable declared in the initialization statement is visible only inside the scope of the for loop and is
released upon termination of the loop.
Don't forget that the variable which was declared in the initialization statement can be modiﬁed during
the loop, as well as the variable checked in the condition.
Example of a loop which counts from 0 to 10:
for (int counter = 0; counter <= 10; ++counter)
{
    std::cout << counter << '\n';
}
// counter is not accessible here (had value 11 at the end)
Explanation of the code fragments:
int counter = 0 initializes the variable counter to 0. (This variable can only be used inside of the for loop.)
counter <= 10 is a Boolean condition that checks whether counter is less than or equal to 10. If it is true,
the loop executes. If it is false, the loop ends.
++counter is an increment operation that increments the value of counter by 1 ahead of the next condition
check.
By leaving all statements empty, you can create an inﬁnite loop:
// infinite loop
for (;;)
    std::cout << "Never ending!\n";
The while loop equivalent of the above is:
// infinite loop
while (true)
    std::cout << "Never ending!\n";
However, an inﬁnite loop can still be left by using the statements break, goto, or return or by throwing an
exception.
The next common example of iterating over all elements from an STL collection (e.g., a vector) without using the
<algorithm> header is:
std::vector<std::string> names = {"Albert Einstein", "Stephen Hawking", "Michael Ellis"};
for(std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it) {
    std::cout << *it << std::endl;
}
Section 11.3: While loop
A while loop executes statements repeatedly until the given condition evaluates to false. This control statement is
used when it is not known, in advance, how many times a block of code is to be executed.
For example, to print all the numbers from 0 up to 9, the following code can be used:
int i = 0;
while (i < 10)
{
    std::cout << i << " ";
    ++i; // Increment counter
}
std::cout << std::endl; // End of line; "0 1 2 3 4 5 6 7 8 9" is printed to the console
Version ≥ C++17
Note that since C++17, the ﬁrst 2 statements can be combined
while (int i = 0; i < 10)
//... The rest is the same
To create an inﬁnite loop, the following construct can be used:
while (true)
{
    // Do something forever (however, you can exit the loop by calling 'break'
}
There is another variant of while loops, namely the do...while construct. See the do-while loop example for more
information.
Section 11.4: Do-while loop
A do-while loop is very similar to a while loop, except that the condition is checked at the end of each cycle, not at
the start. The loop is therefore guaranteed to execute at least once.
The following code will print 0, as the condition will evaluate to false at the end of the ﬁrst iteration:
int i =0;
do
{
    std::cout << i;
    ++i; // Increment counter
}
while (i < 0);
std::cout << std::endl; // End of line; 0 is printed to the console
Note: Do not forget the semicolon at the end of while(condition);, which is needed in the do-while construct.
In contrast to the do-while loop, the following will not print anything, because the condition evaluates to false at
the beginning of the ﬁrst iteration:
int i =0;
while (i < 0)
{
    std::cout << i;
    ++i; // Increment counter
}    
std::cout << std::endl; // End of line; nothing is printed to the console
Note: A while loop can be exited without the condition becoming false by using a break, goto, or return statement.
int i = 0;
do
{
    std::cout << i;
    ++i; // Increment counter
    if (i > 5)
    {
        break;
    }
}
while (true);
std::cout << std::endl; // End of line; 0 1 2 3 4 5 is printed to the console
A trivial do-while loop is also occasionally used to write macros that require their own scope (in which case the
trailing semicolon is omitted from the macro deﬁnition and required to be provided by the user):
#define BAD_MACRO(x) f1(x); f2(x); f3(x);
// Only the call to f1 is protected by the condition here
if (cond) BAD_MACRO(var);
#define GOOD_MACRO(x) do { f1(x); f2(x); f3(x); } while(0)
// All calls are protected here
if (cond) GOOD_MACRO(var);
Section 11.5: Loop Control statements : Break and Continue
Loop control statements are used to change the ﬂow of execution from its normal sequence. When execution
leaves a scope, all automatic objects that were created in that scope are destroyed. The break and continue are
loop control statements.
The break statement terminates a loop without any further consideration.
for (int i = 0; i < 10; i++)
{
    if (i == 4)
        break; // this will immediately exit our loop
    std::cout << i << '\n';
}
The above code will print out:
The continue statement does not immediately exit the loop, but rather skips the rest of the loop body and goes to
the top of the loop (including checking the condition).
for (int i = 0; i < 6; i++)
{
    if (i % 2 == 0) // evaluates to true if i is even
        continue; // this will immediately go back to the start of the loop
    /* the next line will only be reached if the above "continue" statement
       does not execute  */
    std::cout << i << " is an odd number\n";
}
The above code will print out:
1 is an odd number
3 is an odd number
5 is an odd number
Because such control ﬂow changes are sometimes diﬃcult for humans to easily understand, break and continue
are used sparingly. More straightforward implementation are usually easier to read and understand. For example,
the ﬁrst for loop with the break above might be rewritten as:
for (int i = 0; i < 4; i++)
{
    std::cout << i << '\n';
}
The second example with continue might be rewritten as:
for (int i = 0; i < 6; i++)
{
    if (i % 2 != 0) {
        std::cout << i << " is an odd number\n";
    }
}
Section 11.6: Declaration of variables in conditions
In the condition of the for and while loops, it's also permitted to declare an object. This object will be considered to
be in scope until the end of the loop, and will persist through each iteration of the loop:
for (int i = 0; i < 5; ++i) {
    do_something(i);
}
// i is no longer in scope.
for (auto& a : some_container) {
    a.do_something();
}
// a is no longer in scope.
while(std::shared_ptr<Object> p = get_object()) {
   p->do_something();
}
// p is no longer in scope.
However, it is not permitted to do the same with a do...while loop; instead, declare the variable before the loop,
and (optionally) enclose both the variable and the loop within a local scope if you want the variable to go out of
scope after the loop ends:
//This doesn't compile
do {
    s = do_something();
} while (short s > 0);
// Good
short s;
do {
    s = do_something();
} while (s > 0);
This is because the statement portion of a do...while loop (the loop's body) is evaluated before the expression
portion (the while) is reached, and thus, any declaration in the expression will not be visible during the ﬁrst iteration
of the loop.
Section 11.7: Range-for over a sub-range
Using range-base loops, you can loop over a sub-part of a given container or other range by generating a proxy
object that qualiﬁes for range-based for loops.
template<class Iterator, class Sentinel=Iterator>
struct range_t {
  Iterator b;
  Sentinel e;
  Iterator begin() const { return b; }
  Sentinel end() const { return e; }
  bool empty() const { return begin()==end(); }
  range_t without_front( std::size_t count=1 ) const {
    if (std::is_same< std::random_access_iterator_tag, typename
std::iterator_traits<Iterator>::iterator_category >{} ) {
      count = (std::min)(std::size_t(std::distance(b,e)), count);
    }
    return {std::next(b, count), e};
  }
  range_t without_back( std::size_t count=1 ) const {
    if (std::is_same< std::random_access_iterator_tag, typename
std::iterator_traits<Iterator>::iterator_category >{} ) {
      count = (std::min)(std::size_t(std::distance(b,e)), count);
    }
    return {b, std::prev(e, count)};
  }
};
template<class Iterator, class Sentinel>
range_t<Iterator, Sentinel> range( Iterator b, Sentinal e ) {
  return {b,e};
}
template<class Iterable>
auto range( Iterable& r ) {
  using std::begin; using std::end;
  return range(begin(r),end(r));
}
template<class C>
auto except_first( C& c ) {
  auto r = range(c);
  if (r.empty()) return r;
  return r.without_front();
}
now we can do:
std::vector<int> v = {1,2,3,4};
for (auto i : except_first(v))
  std::cout << i << '\n';
and print out
Be aware that intermediate objects generated in the for(:range_expression) part of the for loop will have
expired by the time the for loop starts.
#### 15: Flow Control
Section 15.1: case
Introduces a case label of a switch statement. The operand must be a constant expression and match the switch
condition in type. When the switch statement is executed, it will jump to the case label with operand equal to the
condition, if any.
char c = getchar();
bool confirmed;
switch (c) {
  case 'y':
    confirmed = true;
    break;
  case 'n':
    confirmed = false;
    break;
  default:
    std::cout << "invalid response!\n";
    abort();
}
Section 15.2: switch
According to the C++ standard,
The switch statement causes control to be transferred to one of several statements depending on the
value of a condition.
The keyword switch is followed by a parenthesized condition and a block, which may contain case labels and an
optional default label. When the switch statement is executed, control will be transferred either to a case label
with a value matching that of the condition, if any, or to the default label, if any.
The condition must be an expression or a declaration, which has either integer or enumeration type, or a class type
with a conversion function to integer or enumeration type.
char c = getchar();
bool confirmed;
switch (c) {
  case 'y':
    confirmed = true;
    break;
  case 'n':
    confirmed = false;
    break;
  default:
    std::cout << "invalid response!\n";
    abort();
}
Section 15.3: catch
The catch keyword introduces an exception handler, that is, a block into which control will be transferred when an
exception of compatible type is thrown. The catch keyword is followed by a parenthesized exception declaration,
which is similar in form to a function parameter declaration: the parameter name may be omitted, and the ellipsis
... is allowed, which matches any type. The exception handler will only handle the exception if its declaration is
compatible with the type of the exception. For more details, see catching exceptions.
try {
    std::vector<int> v(N);
    // do something
} catch (const std::bad_alloc&) {
    std::cout << "failed to allocate memory for vector!" << std::endl;
} catch (const std::runtime_error& e) {
    std::cout << "runtime error: " << e.what() << std::endl;
} catch (...) {
    std::cout << "unexpected exception!" << std::endl;
    throw;
}
Section 15.4: throw
1.
When throw occurs in an expression with an operand, its eﬀect is to throw an exception, which is a copy of
the operand.
void print_asterisks(int count) {
    if (count < 0) {
        throw std::invalid_argument("count cannot be negative!");
    }
    while (count--) { putchar('*'); }
}
2.
When throw occurs in an expression without an operand, its eﬀect is to rethrow the current exception. If
there is no current exception, std::terminate is called.
try {
    // something risky
} catch (const std::bad_alloc&) {
    std::cerr << "out of memory" << std::endl;
} catch (...) {
    std::cerr << "unexpected exception" << std::endl;
    // hope the caller knows how to handle this exception
    throw;
}
3.
When throw occurs in a function declarator, it introduces a dynamic exception speciﬁcation, which lists the
types of exceptions that the function is allowed to propagate.
// this function might propagate a std::runtime_error,
// but not, say, a std::logic_error
void risky() throw(std::runtime_error);
// this function can't propagate any exceptions
void safe() throw();
Dynamic exception speciﬁcations are deprecated as of C++11.
Note that the ﬁrst two uses of throw listed above constitute expressions rather than statements. (The type of a
throw expression is void.) This makes it possible to nest them within expressions, like so:
unsigned int predecessor(unsigned int x) {
    return (x > 0) ? (x - 1) : (throw std::invalid_argument("0 has no predecessor"));
}
Section 15.5: default
In a switch statement, introduces a label that will be jumped to if the condition's value is not equal to any of the
case labels' values.
char c = getchar();
bool confirmed;
switch (c) {
  case 'y':
    confirmed = true;
    break;
  case 'n':
    confirmed = false;
    break;
  default:
    std::cout << "invalid response!\n";
    abort();
}
Version ≥ C++11
Deﬁnes a default constructor, copy constructor, move constructor, destructor, copy assignment operator, or move
assignment operator to have its default behaviour.
class Base {
    // ...
    // we want to be able to delete derived classes through Base*,
    // but have the usual behaviour for Base's destructor.
    virtual ~Base() = default;
};
Section 15.6: try
The keyword try is followed by a block, or by a constructor initializer list and then a block (see here). The try block is
followed by one or more catch blocks. If an exception propagates out of the try block, each of the corresponding
catch blocks after the try block has the opportunity to handle the exception, if the types match.
std::vector<int> v(N);     // if an exception is thrown here,
                           // it will not be caught by the following catch block
try {
    std::vector<int> v(N); // if an exception is thrown here,
                           // it will be caught by the following catch block
    // do something with v
} catch (const std::bad_alloc&) {
    // handle bad_alloc exceptions from the try block
}    
Section 15.7: if
Introduces an if statement. The keyword if must be followed by a parenthesized condition, which can be either an
expression or a declaration. If the condition is truthy, the substatement after the condition will be executed.
int x;
std::cout << "Please enter a positive number." << std::endl;
std::cin >> x;
if (x <= 0) {
    std::cout << "You didn't enter a positive number!" << std::endl;
    abort();
}
Section 15.8: else
The ﬁrst substatement of an if statement may be followed by the keyword else. The substatement after the else
keyword will be executed when the condition is falsey (that is, when the ﬁrst substatement is not executed).
int x;
std::cin >> x;
if (x%2 == 0) {
    std::cout << "The number is even\n";
} else {
    std::cout << "The number is odd\n";
}
Section 15.9: Conditional Structures: if, if..else
if and else:
it used to check whether the given expression returns true or false and acts as such:
if (condition) statement
the condition can be any valid C++ expression that returns something that be checked against truth/falsehood for
example:
if (true) { /* code here */ }  // evaluate that true is true and execute the code in the brackets
if (false) { /* code here */ } // always skip the code since false is always false
the condition can be anything, a function, a variable, or a comparison for example
if(istrue()) { } // evaluate the function, if it returns true, the if will execute the code
if(isTrue(var)) { } //evaluate the return of the function after passing the argument var
if(a == b) { } // this will evaluate the return of the experssion (a==b) which will be true if
equal and false if unequal
if(a) { } //if a is a boolean type, it will evaluate for its value, if it's an integer, any non
zero value will be true,
if we want to check for a multiple expressions we can do it in two ways :
using binary operators :
if (a && b) { } // will be true only if both a and b are true (binary operators are outside the
scope here
if (a || b ) { } //true if a or b is true
using if/ifelse/else:
for a simple switch either if or else
if (a== "test") {
    //will execute if a is a string "test"
} else {
    // only if the first failed, will execute
}
for multiple choices :
if (a=='a') {
// if a is a char valued 'a'  
} else if (a=='b') {
// if a is a char valued 'b'
} else if (a=='c') {
// if a is a char valued 'c'
} else {
//if a is none of the above
}
however it must be noted that you should use 'switch' instead if your code checks for the same variable's value
Section 15.10: goto
Jumps to a labelled statement, which must be located in the current function.
bool f(int arg) {
    bool result = false;
    hWidget widget = get_widget(arg);
    if (!g()) {
        // we can't continue, but must do cleanup still
        goto end;
    }
    // ...
    result = true;
  end:
    release_widget(widget);
    return result;
}
Section 15.11: Jump statements : break, continue, goto, exit
The break instruction:
Using break we can leave a loop even if the condition for its end is not fulﬁlled. It can be used to end an inﬁnite
loop, or to force it to end before its natural end
The syntax is
break;
Example: we often use break in switch cases,ie once a case i switch is satisﬁed then the code block of that
condition is executed .
switch(conditon){
case 1: block1;
case 2: block2;
case 3: block3;
default: blockdefault;
}
in this case if case 1 is satisﬁed then block 1 is executed , what we really want is only the block1 to be processed but
instead once the block1 is processed remaining blocks,block2,block3 and blockdefault are also processed even
though only case 1 was satiﬁed.To avoid this we use break at the end of each block like :
switch(condition){
case 1: block1;
        break;
case 2: block2;
        break;
case 3: block3;
        break;
default: blockdefault;
        break;
}
so only one block is processed and the control moves out of the switch loop.
break can also be used in other conditional and non conditional loops like if,while,for etc;
example:
if(condition1){
   if(condition2){
    break;
    }
}
The continue instruction:
The continue instruction causes the program to skip the rest of the loop in the present iteration as if the end of the
statement block would have been reached, causing it to jump to the following iteration.
The syntax is
continue;
Example consider the following :
for(int i=0;i<10;i++){
if(i%2==0)
continue;
cout<<"\n @"<<i;
}
which produces the output:
 @1
 @3
 @5
 @7
 @9
i this code whenever the condition i%2==0 is satisﬁed continue is processed,this causes the compiler to skip all the
remaining code( printing @ and i) and increment/decrement statement of the loop gets executed.
## <a name="chapter-2-theccompilationexecutionmodel"></a>CHAPTER 2: THE C++ COMPILATION & EXECUTION MODEL

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

## <a name="chapter-3-objectorientedprogrammingfundamentals"></a>CHAPTER 3: OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS

## Classes & Objects

### Basic Class (C++98)

```cpp
#include <iostream>
#include <string>

class Person {
public:      // Publicly accessible
    std::string name;
    int age;
    
    void introduce() {
        std::cout << "I am " << name << ", age " << age << "\n";
    }
    
private:     // Private to class
    std::string secret;
    
    void hidden_method() {
        // Can't be called from outside
    }
    
protected:   // Protected (for inheritance)
    std::string protected_data;
};

int main() {
    Person p;
    p.name = "Alice";
    p.age = 30;
    p.introduce();
    
    // p.secret = "xyz";  // Error: private
    // p.hidden_method();  // Error: private
    
    return 0;
}
```

### Class Member Variables & Methods (C++98)

```cpp
#include <iostream>

class BankAccount {
private:
    double balance;
    std::string owner;
public:
    // Constructor
    BankAccount(const std::string& name, double initial) 
        : owner(name), balance(initial) {}
    
    // Methods
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
        }
    }
    
    double get_balance() const {  // const = doesn't modify
        return balance;
    }
    
    std::string get_owner() const {
        return owner;
    }
};

int main() {
    BankAccount account("Alice", 1000);
    account.deposit(500);
    account.withdraw(200);
    
    std::cout << account.get_owner() << ": $" 
              << account.get_balance() << "\n";
    
    return 0;
}
```
## Classes & Objects - Complete Mastery

### What is a Class?

A **class** is a blueprint for creating objects. It defines:
- **Attributes** (member variables) - what the object has
- **Methods** (member functions) - what the object does

### What is an Object?

An **object** is an instance of a class - a concrete entity with specific values.

## The Four Pillars of OOP

Object-Oriented Programming is built on four fundamental concepts that distinguish it from procedural programming:

### 1. **Encapsulation** - Data Hiding
### 2. **Inheritance** - Code Reuse
### 3. **Polymorphism** - Flexible Behavior
### 4. **Abstraction** - Simplified Interface

```cpp
#include <iostream>
#include <string>
using namespace std;

// Class definition
class Car {
public:  // Public members accessible from outside
    // Member variables
    string brand;
    string color;
    int year;
    int speed;
    
    // Member functions
    void accelerate() {
        speed += 10;
        cout << brand << " accelerates to " << speed << " mph\n";
    }
    
    void brake() {
        if (speed >= 10) {
            speed -= 10;
        } else {
            speed = 0;
        }
        cout << brand << " brakes to " << speed << " mph\n";
    }
    
    void displayInfo() {
        cout << year << " " << color << " " << brand 
             << " at " << speed << " mph\n";
    }
};

int main() {
    // Creating objects (instances)
    Car car1;
    car1.brand = "Toyota";
    car1.color = "Red";
    car1.year = 2020;
    car1.speed = 0;
    
    Car car2;
    car2.brand = "BMW";
    car2.color = "Blue";
    car2.year = 2022;
    car2.speed = 0;
    
    // Using objects
    car1.displayInfo();
    car1.accelerate();
    car1.accelerate();
    car1.brake();
    
    car2.displayInfo();
    car2.accelerate();
    car2.accelerate();
    car2.accelerate();
    
    return 0;
}
```

**Output:**
```
2020 Red Toyota at 0 mph
Toyota accelerates to 10 mph
Toyota accelerates to 20 mph
Toyota brakes to 10 mph
2022 Blue BMW at 0 mph
BMW accelerates to 10 mph
BMW accelerates to 20 mph
BMW accelerates to 30 mph
```

---

## Encapsulation

**Encapsulation** is hiding internal details and exposing only what's necessary.

### Access Levels (Access Modifiers)

```cpp
#include <iostream>
#include <string>
using namespace std;

class BankAccount {
private:  // Only accessible within this class
    double balance;
    string accountNumber;
    
    // Private helper function
    bool validateAmount(double amount) {
        return amount > 0;
    }
    
public:  // Accessible from anywhere
    // Constructor
    BankAccount(string accNum, double initialBalance)
        : accountNumber(accNum), balance(initialBalance) {
        cout << "Account created: " << accountNumber << "\n";
    }
    
    // Public methods to access private data
    void deposit(double amount) {
        if (validateAmount(amount)) {
            balance += amount;
            cout << "Deposited: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Invalid deposit amount\n";
        }
    }
    
    void withdraw(double amount) {
        if (validateAmount(amount) && amount <= balance) {
            balance -= amount;
            cout << "Withdrew: $" << amount 
                 << ". New balance: $" << balance << "\n";
        } else {
            cout << "Cannot withdraw that amount\n";
        }
    }
    
    double getBalance() const {  // const = doesn't modify
        return balance;
    }
    
    string getAccountNumber() const {
        return accountNumber;
    }

protected:  // Accessible in this class and derived classes
    void logTransaction(string type) {
        cout << "Transaction (" << type << ") logged\n";
    }
};

int main() {
    BankAccount account("ACC123456", 1000);
    
    account.deposit(500);
    account.withdraw(200);
    account.withdraw(2000);  // Won't work - insufficient funds
    
    cout << "Final balance: $" << account.getBalance() << "\n";
    
    // These would cause compile errors:
    // account.balance = 10000;        // Error: private
    // account.validateAmount(100);    // Error: private
    // account.accountNumber = "123";  // Error: private
    
    return 0;
}
```

**Output:**
```
Account created: ACC123456
Deposited: $500. New balance: $1500
Withdrew: $200. New balance: $1300
Cannot withdraw that amount
Final balance: $1300
```

**Why Encapsulation?**
- **Data Protection**: Prevents invalid states
- **Control**: You control how data is modified
- **Flexibility**: Can change internal implementation without affecting users
- **Security**: Sensitive data is hidden
- **Maintainability**: Easy to modify implementation later

---

## Constructors & Destructors - Complete Guide

### Types of Constructors

```cpp
#include <iostream>
#include <string>
using namespace std;

class Student {
private:
    string name;
    int rollNumber;
    double gpa;
    
public:
    // 1. Default Constructor (no parameters)
    Student() : name("Unknown"), rollNumber(0), gpa(0.0) {
        cout << "Default constructor called\n";
    }
    
    // 2. Parameterized Constructor
    Student(string n, int roll, double g) 
        : name(n), rollNumber(roll), gpa(g) {
        cout << "Parameterized constructor called\n";
    }
    
    // 3. Copy Constructor (copies another object)
    Student(const Student& other) 
        : name(other.name), rollNumber(other.rollNumber), gpa(other.gpa) {
        cout << "Copy constructor called\n";
    }
    
    // 4. Move Constructor (C++11) - transfers ownership
    Student(Student&& other) noexcept
        : name(move(other.name)), rollNumber(other.rollNumber), gpa(other.gpa) {
        other.rollNumber = 0;
        other.gpa = 0.0;
        cout << "Move constructor called\n";
    }
    
    // Destructor - called when object is destroyed
    ~Student() {
        cout << "Destructor called for " << name << "\n";
    }
    
    // Getter methods
    void display() const {
        cout << "Name: " << name << ", Roll: " << rollNumber 
             << ", GPA: " << gpa << "\n";
    }
};

int main() {
    cout << "--- Creating objects ---\n";
    
    // Default constructor
    Student s1;
    s1.display();
    cout << "\n";
    
    // Parameterized constructor
    Student s2("Alice", 101, 3.8);
    s2.display();
    cout << "\n";
    
    // Copy constructor (explicit)
    Student s3 = s2;  // Calls copy constructor
    s3.display();
    cout << "\n";
    
    // Move constructor
    Student s4 = move(s2);  // Calls move constructor
    s4.display();
    cout << "\n";
    
    cout << "--- Objects going out of scope ---\n";
    
    return 0;  // Destructors called here for all objects
}
```

**Output:**
```
--- Creating objects ---
Default constructor called
Name: Unknown, Roll: 0, GPA: 0

Parameterized constructor called
Name: Alice, Roll: 101, GPA: 3.8

Copy constructor called
Name: Alice, Roll: 101, GPA: 3.8

Move constructor called
Name: Alice, Roll: 101, GPA: 3.8

--- Objects going out of scope ---
Destructor called for Alice
Destructor called for Alice
Destructor called for Unknown
Destructor called for Unknown
```

### Initialization Lists (Member Initializer List)

```cpp
#include <iostream>
using namespace std;

class Rectangle {
private:
    int width;
    int height;

public:
    // Using initialization list
    // This is ALWAYS better than assignment in constructor body
    Rectangle(int w, int h) : width(w), height(h) {
        cout << "Rectangle created\n";
    }
    
    // Alternative (NOT recommended) - less efficient
    // Rectangle(int w, int h) {
    //     width = w;    // Assignment, not initialization
    //     height = h;
    // }
    
    int getArea() const {
        return width * height;
    }
};

int main() {
    Rectangle rect(5, 10);
    cout << "Area: " << rect.getArea() << "\n";
    
    return 0;
}
```

**Why initialization lists?**
- **Efficiency**: Initializes variables once (not create then assign)
- **Const members**: Can only be initialized with initializer list
- **Reference members**: Must use initializer list
- **Base class initialization**: Only way to initialize base class

---

## Inheritance - All Types

**Inheritance** allows a class to inherit properties and methods from another class.

### Single Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

// Base class (Parent class)
class Animal {
protected:  // Accessible in derived classes
    string name;
    int age;

public:
    Animal(string n, int a) : name(n), age(a) {
        cout << "Animal constructor called\n";
    }
    
    virtual void eat() {
        cout << name << " is eating\n";
    }
    
    virtual void sleep() {
        cout << name << " is sleeping\n";
    }
    
    virtual void makeSound() {
        cout << name << " makes a sound\n";
    }
    
    virtual ~Animal() {
        cout << "Animal destructor called\n";
    }
};

// Derived class (Child class)
class Dog : public Animal {
private:
    string breed;

public:
    Dog(string n, int a, string b) 
        : Animal(n, a), breed(b) {
        cout << "Dog constructor called\n";
    }
    
    // Override methods from base class
    void makeSound() override {
        cout << name << " barks: Woof! Woof!\n";
    }
    
    void fetch() {
        cout << name << " is fetching the ball\n";
    }
    
    ~Dog() {
        cout << "Dog destructor called\n";
    }
};

class Cat : public Animal {
private:
    bool isIndoor;

public:
    Cat(string n, int a, bool indoor) 
        : Animal(n, a), isIndoor(indoor) {
        cout << "Cat constructor called\n";
    }
    
    void makeSound() override {
        cout << name << " meows: Meow! Meow!\n";
    }
    
    void scratch() {
        cout << name << " is scratching the furniture\n";
    }
    
    ~Cat() {
        cout << "Cat destructor called\n";
    }
};

int main() {
    cout << "=== Creating Dog ===\n";
    Dog dog("Rex", 3, "Golden Retriever");
    dog.eat();
    dog.sleep();
    dog.makeSound();
    dog.fetch();
    
    cout << "\n=== Creating Cat ===\n";
    Cat cat("Whiskers", 2, true);
    cat.eat();
    cat.makeSound();
    cat.scratch();
    
    cout << "\n=== Using Polymorphism ===\n";
    Animal* animals[2] = {&dog, &cat};
    for (int i = 0; i < 2; i++) {
        animals[i]->makeSound();
    }
    
    cout << "\n=== Destructors ===\n";
    return 0;
}
```

**Output:**
```
=== Creating Dog ===
Animal constructor called
Dog constructor called
Rex is eating
Rex is sleeping
Rex barks: Woof! Woof!
Rex is fetching the ball

=== Creating Cat ===
Animal constructor called
Cat constructor called
Whiskers is eating
Whiskers meows: Meow! Meow!
Whiskers is scratching the furniture

=== Using Polymorphism ===
Rex barks: Woof! Woof!
Whiskers meows: Meow! Meow!

=== Destructors ===
Cat destructor called
Animal destructor called
Dog destructor called
Animal destructor called
```

### Multiple Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

class Flyer {
public:
    virtual void fly() {
        cout << "Flying...\n";
    }
    virtual ~Flyer() {}
};

class Swimmer {
public:
    virtual void swim() {
        cout << "Swimming...\n";
    }
    virtual ~Swimmer() {}
};

// Duck inherits from both Flyer and Swimmer
class Duck : public Flyer, public Swimmer {
private:
    string name;

public:
    Duck(string n) : name(n) {}
    
    void fly() override {
        cout << name << " is flying\n";
    }
    
    void swim() override {
        cout << name << " is swimming\n";
    }
    
    void quack() {
        cout << name << " quacks: Quack! Quack!\n";
    }
};

int main() {
    Duck duck("Donald");
    duck.fly();
    duck.swim();
    duck.quack();
    
    return 0;
}
```

### Multilevel Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

// Level 1: Base class
class Vehicle {
protected:
    string brand;
    
public:
    Vehicle(string b) : brand(b) {}
    virtual void start() {
        cout << brand << " is starting\n";
    }
    virtual ~Vehicle() {}
};

// Level 2: Derived from Vehicle
class Car : public Vehicle {
protected:
    int numDoors;
    
public:
    Car(string b, int doors) : Vehicle(b), numDoors(doors) {}
    void start() override {
        cout << brand << " car with " << numDoors 
             << " doors is starting\n";
    }
    virtual ~Car() {}
};

// Level 3: Derived from Car
class ElectricCar : public Car {
private:
    int batteryPercentage;
    
public:
    ElectricCar(string b, int doors, int battery)
        : Car(b, doors), batteryPercentage(battery) {}
    
    void start() override {
        cout << "Electric " << brand << " (Battery: " 
             << batteryPercentage << "%) starting\n";
    }
};

int main() {
    ElectricCar tesla("Tesla", 4, 95);
    tesla.start();
    
    return 0;
}
```

### Hierarchical Inheritance

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
protected:
    string name;
    double salary;
    
public:
    Employee(string n, double s) : name(n), salary(s) {}
    virtual void work() = 0;
    virtual ~Employee() {}
};

class Manager : public Employee {
public:
    Manager(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is managing the team\n";
    }
};

class Developer : public Employee {
private:
    string language;
    
public:
    Developer(string n, double s, string lang)
        : Employee(n, s), language(lang) {}
    void work() override {
        cout << name << " is coding in " << language << "\n";
    }
};

class Designer : public Employee {
public:
    Designer(string n, double s) : Employee(n, s) {}
    void work() override {
        cout << name << " is designing UI/UX\n";
    }
};

int main() {
    Manager manager("Alice", 80000);
    Developer dev("Bob", 70000, "C++");
    Designer designer("Carol", 65000);
    
    manager.work();
    dev.work();
    designer.work();
    
    return 0;
}
```

---

## Polymorphism

**Polymorphism** means "many forms" - the ability of objects to take multiple forms.

### Compile-Time Polymorphism (Static Polymorphism)

#### 1. Function Overloading

```cpp
#include <iostream>
#include <string>
using namespace std;

class Calculator {
public:
    // Overload for integers
    int add(int a, int b) {
        cout << "Adding integers\n";
        return a + b;
    }
    
    // Overload for doubles
    double add(double a, double b) {
        cout << "Adding doubles\n";
        return a + b;
    }
    
    // Overload for strings (concatenation)
    string add(string a, string b) {
        cout << "Concatenating strings\n";
        return a + b;
    }
    
    // Overload with different number of parameters
    int add(int a, int b, int c) {
        cout << "Adding three integers\n";
        return a + b + c;
    }
};

int main() {
    Calculator calc;
    
    cout << "Result: " << calc.add(5, 3) << "\n";
    cout << "Result: " << calc.add(2.5, 3.7) << "\n";
    cout << "Result: " << calc.add("Hello", " World") << "\n";
    cout << "Result: " << calc.add(5, 3, 2) << "\n";
    
    return 0;
}
```

#### 2. Operator Overloading

```cpp
#include <iostream>
using namespace std;

class Complex {
private:
    double real, imag;
    
public:
    Complex(double r = 0, double i = 0) : real(r), imag(i) {}
    
    // Overload + operator
    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }
    
    // Overload - operator
    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }
    
    // Overload * operator
    Complex operator*(const Complex& other) const {
        double r = real * other.real - imag * other.imag;
        double i = real * other.imag + imag * other.real;
        return Complex(r, i);
    }
    
    // Overload == operator
    bool operator==(const Complex& other) const {
        return real == other.real && imag == other.imag;
    }
    
    // Overload << operator for output
    friend ostream& operator<<(ostream& os, const Complex& c) {
        os << c.real << " + " << c.imag << "i";
        return os;
    }
    
    // Overload = operator (assignment)
    Complex& operator=(const Complex& other) {
        if (this != &other) {
            real = other.real;
            imag = other.imag;
        }
        return *this;
    }
};

int main() {
    Complex c1(3, 4);
    Complex c2(2, 5);
    
    Complex c3 = c1 + c2;
    cout << c1 << " + " << c2 << " = " << c3 << "\n";
    
    Complex c4 = c1 * c2;
    cout << c1 << " * " << c2 << " = " << c4 << "\n";
    
    if (c1 == c2) {
        cout << "Complex numbers are equal\n";
    } else {
        cout << "Complex numbers are not equal\n";
    }
    
    return 0;
}
```

#### 3. Template Specialization

```cpp
#include <iostream>
#include <string>
using namespace std;

// Generic template
template <typename T>
class Printer {
public:
    void print(T value) {
        cout << "Generic: " << value << "\n";
    }
};

// Template specialization for bool
template <>
class Printer<bool> {
public:
    void print(bool value) {
        cout << "Boolean: " << (value ? "true" : "false") << "\n";
    }
};

// Template specialization for string
template <>
class Printer<string> {
public:
    void print(string value) {
        cout << "String: \"" << value << "\"\n";
    }
};

int main() {
    Printer<int> intPrinter;
    intPrinter.print(42);
    
    Printer<double> doublePrinter;
    doublePrinter.print(3.14);
    
    Printer<bool> boolPrinter;
    boolPrinter.print(true);
    
    Printer<string> stringPrinter;
    stringPrinter.print("Hello, World!");
    
    return 0;
}
```

### Run-Time Polymorphism (Dynamic Polymorphism)

#### Virtual Functions & Override

```cpp
#include <iostream>
#include <memory>
#include <vector>
using namespace std;

class Shape {
public:
    virtual void draw() = 0;           // Pure virtual
    virtual double area() = 0;
    virtual string getName() = 0;
    virtual ~Shape() {}                // Virtual destructor (important!)
};

class Circle : public Shape {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        cout << "Drawing Circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
    
    string getName() override {
        return "Circle";
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Drawing Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
    
    string getName() override {
        return "Rectangle";
    }
};

class Triangle : public Shape {
private:
    double base, height;
    
public:
    Triangle(double b, double h) : base(b), height(h) {}
    
    void draw() override {
        cout << "Drawing Triangle\n";
    }
    
    double area() override {
        return 0.5 * base * height;
    }
    
    string getName() override {
        return "Triangle";
    }
};

int main() {
    // Using smart pointers (C++11)
    vector<unique_ptr<Shape>> shapes;
    shapes.push_back(make_unique<Circle>(5));
    shapes.push_back(make_unique<Rectangle>(4, 6));
    shapes.push_back(make_unique<Triangle>(3, 4));
    
    cout << "=== Drawing all shapes ===\n";
    for (auto& shape : shapes) {
        shape->draw();
        cout << "Area of " << shape->getName() << ": " 
             << shape->area() << "\n\n";
    }
    
    return 0;  // Smart pointers automatically deleted
}
```

**Output:**
```
=== Drawing all shapes ===
Drawing Circle
Area of Circle: 78.5385

Drawing Rectangle
Area of Rectangle: 24

Drawing Triangle
Area of Triangle: 6
```

#### Abstract Classes & Interfaces

```cpp
#include <iostream>
#include <string>
using namespace std;

// Abstract class (interface-like)
class DatabaseConnection {
public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual bool isConnected() = 0;
    virtual ~DatabaseConnection() {}
};

class MySQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    MySQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to MySQL...\n";
        connected = true;
        cout << "Connected to MySQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from MySQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

class PostgreSQLConnection : public DatabaseConnection {
private:
    bool connected;
    
public:
    PostgreSQLConnection() : connected(false) {}
    
    void connect() override {
        cout << "Connecting to PostgreSQL...\n";
        connected = true;
        cout << "Connected to PostgreSQL\n";
    }
    
    void disconnect() override {
        cout << "Disconnecting from PostgreSQL...\n";
        connected = false;
    }
    
    bool isConnected() override {
        return connected;
    }
};

int main() {
    // Cannot create DatabaseConnection directly
    // DatabaseConnection db;  // Error: abstract class
    
    // Create through derived classes
    MySQLConnection mysql;
    PostgreSQLConnection postgres;
    
    // Use polymorphically
    DatabaseConnection* db1 = &mysql;
    DatabaseConnection* db2 = &postgres;
    
    db1->connect();
    cout << "MySQL connected: " << (db1->isConnected() ? "Yes" : "No") << "\n\n";
    
    db2->connect();
    cout << "PostgreSQL connected: " << (db2->isConnected() ? "Yes" : "No") << "\n\n";
    
    db1->disconnect();
    db2->disconnect();
    
    return 0;
}
```

---

## Abstraction

**Abstraction** is showing only essential features and hiding unnecessary details.

```cpp
#include <iostream>
#include <string>
#include <cmath>
using namespace std;

// Abstract class - hides implementation details
class Shape {
public:
    virtual void draw() = 0;
    virtual double area() = 0;
    virtual double perimeter() = 0;
    virtual ~Shape() {}
};

// Concrete implementation
class Circle : public Shape {
private:
    double radius;
    const double PI = 3.14159;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        cout << "Displaying Circle\n";
    }
    
    double area() override {
        return PI * radius * radius;
    }
    
    double perimeter() override {
        return 2 * PI * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
    
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        cout << "Displaying Rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
    
    double perimeter() override {
        return 2 * (width + height);
    }
};

// Client code doesn't know HOW things are calculated
void printShapeInfo(Shape* shape) {
    shape->draw();
    cout << "Area: " << shape->area() << "\n";
    cout << "Perimeter: " << shape->perimeter() << "\n";
}

int main() {
    Circle circle(5);
    Rectangle rectangle(4, 6);
    
    cout << "=== Circle ===\n";
    printShapeInfo(&circle);
    
    cout << "\n=== Rectangle ===\n";
    printShapeInfo(&rectangle);
    
    return 0;
}
```

---

## Advanced Class Features

### Nested Classes

```cpp
#include <iostream>
#include <string>
using namespace std;

class Car {
public:
    // Nested class
    class Engine {
    private:
        double horsepower;
        
    public:
        Engine(double hp) : horsepower(hp) {}
        
        void startEngine() {
            cout << "Engine with " << horsepower 
                 << " HP is starting\n";
        }
    };
    
private:
    string brand;
    Engine engine;
    
public:
    Car(string b, double hp) : brand(b), engine(hp) {}
    
    void display() {
        cout << brand << " car\n";
        engine.startEngine();
    }
};

int main() {
    Car car("Ferrari", 800);
    car.display();
    
    // Can also create nested class independently
    Car::Engine standaloneEngine(500);
    standaloneEngine.startEngine();
    
    return 0;
}
```

### Inner Classes with Access Control

```cpp
#include <iostream>
using namespace std;

class Outer {
private:
    int outerPrivate = 10;
    
public:
    class Inner {
    public:
        void accessOuterPrivate(Outer& outer) {
            // Inner class can access Outer's private members
            cout << "Accessing outer private: " 
                 << outer.outerPrivate << "\n";
        }
    };
    
    Inner createInner() {
        return Inner();
    }
};

int main() {
    Outer outer;
    Outer::Inner inner = outer.createInner();
    inner.accessOuterPrivate(outer);
    
    return 0;
}
```

---

## Access Modifiers & Friend Classes

### Public, Private, Protected

```cpp
#include <iostream>
using namespace std;

class Base {
public:
    int publicData = 1;
    void publicMethod() {
        cout << "Public method\n";
    }
    
protected:
    int protectedData = 2;
    void protectedMethod() {
        cout << "Protected method\n";
    }
    
private:
    int privateData = 3;
    void privateMethod() {
        cout << "Private method\n";
    }
};

class Derived : public Base {
public:
    void testAccess() {
        cout << "Public data: " << publicData << "\n";
        cout << "Protected data: " << protectedData << "\n";
        // cout << "Private data: " << privateData << "\n";  // Error!
    }
};

int main() {
    Base b;
    cout << "Public: " << b.publicData << "\n";
    // cout << "Protected: " << b.protectedData << "\n";  // Error!
    // cout << "Private: " << b.privateData << "\n";      // Error!
    
    Derived d;
    d.testAccess();
    
    return 0;
}
```

### Friend Functions and Classes

```cpp
#include <iostream>
using namespace std;

class MyClass {
private:
    int secretValue;
    
public:
    MyClass(int value) : secretValue(value) {}
    
    // Friend function - can access private members
    friend void revealSecret(MyClass& obj);
    
    // Friend class - can access private members
    friend class FriendClass;
};

// Friend function
void revealSecret(MyClass& obj) {
    cout << "Secret value: " << obj.secretValue << "\n";
}

// Friend class
class FriendClass {
public:
    void accessPrivate(MyClass& obj) {
        cout << "Friend class accessing: " << obj.secretValue << "\n";
    }
};

int main() {
    MyClass obj(42);
    revealSecret(obj);
    
    FriendClass friend;
    friend.accessPrivate(obj);
    
    return 0;
}
```

---

## Static Members

### Static Variables & Methods

```cpp
#include <iostream>
#include <string>
using namespace std;

class Employee {
private:
    string name;
    int id;
    static int nextId;      // Shared by all instances
    static int totalCount;  // Count of employees
    
public:
    Employee(string n) : name(n), id(nextId++) {
        totalCount++;
    }
    
    ~Employee() {
        totalCount--;
    }
    
    // Static method - can only access static members
    static void displayStats() {
        cout << "Total employees: " << totalCount << "\n";
        cout << "Next ID will be: " << nextId << "\n";
    }
    
    void display() {
        cout << "ID: " << id << ", Name: " << name << "\n";
    }
};

// Initialize static members
int Employee::nextId = 1;
int Employee::totalCount = 0;

int main() {
    cout << "Creating employees...\n";
    Employee emp1("Alice");
    emp1.display();
    
    Employee emp2("Bob");
    emp2.display();
    
    Employee emp3("Carol");
    emp3.display();
    
    // Call static method
    Employee::displayStats();
    
    {
        Employee emp4("David");
        emp4.display();
        Employee::displayStats();
    }
    
    cout << "\nAfter scope ends:\n";
    Employee::displayStats();
    
    return 0;
}
```

**Output:**
```
Creating employees...
ID: 1, Name: Alice
ID: 2, Name: Bob
ID: 3, Name: Carol
Total employees: 3
Next ID will be: 4
ID: 4, Name: David
Total employees: 4
Next ID will be: 5

After scope ends:
Total employees: 3
Next ID will be: 5
```

---

## Const Correctness in OOP

```cpp
#include <iostream>
#include <string>
using namespace std;

class Person {
private:
    mutable int accessCount;  // Can be modified even in const functions
    string name;
    int age;
    
public:
    Person(string n, int a) : name(n), age(a), accessCount(0) {}
    
    // Const method - cannot modify member variables
    string getName() const {
        accessCount++;  // OK because accessCount is mutable
        return name;
    }
    
    // Const method returning const reference
    const string& getNameRef() const {
        accessCount++;
        return name;
    }
    
    // Non-const method - can modify members
    void setAge(int newAge) {
        age = newAge;  // OK in non-const method
    }
    
    // Const method
    int getAge() const {
        return age;
    }
    
    int getAccessCount() const {
        return accessCount;
    }
    
    void display() const {
        cout << "Name: " << name << ", Age: " << age << "\n";
    }
};

int main() {
    Person p("Alice", 30);
    
    // Can call both const and non-const methods
    cout << p.getName() << "\n";
    cout << p.getAge() << "\n";
    p.setAge(31);
    
    // Const object
    const Person cp("Bob", 25);
    
    // Can only call const methods
    cout << cp.getName() << "\n";
    cout << cp.getAge() << "\n";
    // cp.setAge(26);  // Error: cannot call non-const method on const object
    
    cp.display();
    cout << "Access count: " << cp.getAccessCount() << "\n";
    
    return 0;
}
```

---

## Operator Overloading

### Overloadable Operators

```cpp
#include <iostream>
using namespace std;

class Vector {
private:
    int x, y;
    
public:
    Vector(int x = 0, int y = 0) : x(x), y(y) {}
    
    // Arithmetic operators
    Vector operator+(const Vector& v) const {
        return Vector(x + v.x, y + v.y);
    }
    
    Vector operator-(const Vector& v) const {
        return Vector(x - v.x, y - v.y);
    }
    
    Vector operator*(int scalar) const {
        return Vector(x * scalar, y * scalar);
    }
    
    // Comparison operators
    bool operator==(const Vector& v) const {
        return x == v.x && y == v.y;
    }
    
    bool operator!=(const Vector& v) const {
        return !(*this == v);
    }
    
    // Assignment operator
    Vector& operator=(const Vector& v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }
    
    // Unary operators
    Vector operator-() const {
        return Vector(-x, -y);
    }
    
    // Increment/Decrement
    Vector& operator++() {  // Pre-increment
        x++; y++;
        return *this;
    }
    
    Vector operator++(int) {  // Post-increment
        Vector temp = *this;
        x++; y++;
        return temp;
    }
    
    // Subscript operator
    int operator[](int index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        throw out_of_range("Invalid index");
    }
    
    // Stream operators (must be friend or free functions)
    friend ostream& operator<<(ostream& os, const Vector& v) {
        os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
    
    friend istream& operator>>(istream& is, Vector& v) {
        is >> v.x >> v.y;
        return is;
    }
};

int main() {
    Vector v1(3, 4);
    Vector v2(1, 2);
    
    cout << "v1 = " << v1 << "\n";
    cout << "v2 = " << v2 << "\n";
    
    Vector v3 = v1 + v2;
    cout << "v1 + v2 = " << v3 << "\n";
    
    Vector v4 = v1 - v2;
    cout << "v1 - v2 = " << v4 << "\n";
    
    Vector v5 = v1 * 2;
    cout << "v1 * 2 = " << v5 << "\n";
    
    cout << "v1 == v2: " << (v1 == v2 ? "true" : "false") << "\n";
    
    cout << "-v1 = " << (-v1) << "\n";
    
    cout << "v1[0] = " << v1[0] << ", v1[1] = " << v1[1] << "\n";
    
    Vector v6 = v1;
    cout << "After v6 = v1: v6 = " << v6 << "\n";
    
    return 0;
}
```

---

## SOLID Principles

### S - Single Responsibility Principle

```cpp
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// BAD: Class doing multiple things
// class User {
//     void save() { }      // Saving logic
//     void sendEmail() { } // Sending email
//     void validate() { }  // Validation
// };

// GOOD: Each class has one responsibility
class User {
private:
    string name, email;
    
public:
    User(string n, string e) : name(n), email(e) {}
    string getName() { return name; }
    string getEmail() { return email; }
};

class UserValidator {
public:
    bool isValid(const User& user) {
        return !user.getName().empty() && 
               !user.getEmail().empty();
    }
};

class UserRepository {
public:
    void save(const User& user) {
        // Save to database or file
        cout << "Saving user: " << user.getName() << "\n";
    }
};

class EmailService {
public:
    void sendWelcomeEmail(const User& user) {
        cout << "Sending welcome email to: " << user.getEmail() << "\n";
    }
};

int main() {
    User user("Alice", "alice@example.com");
    UserValidator validator;
    UserRepository repo;
    EmailService emailService;
    
    if (validator.isValid(user)) {
        repo.save(user);
        emailService.sendWelcomeEmail(user);
    }
    
    return 0;
}
```

### O - Open/Closed Principle

```cpp
#include <iostream>
#include <vector>
using namespace std;

// GOOD: Open for extension, closed for modification
class Shape {
public:
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    double area() override {
        return 3.14 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    double area() override {
        return width * height;
    }
};

// New shape can be added without modifying existing code
class Triangle : public Shape {
private:
    double base, height;
public:
    Triangle(double b, double h) : base(b), height(h) {}
    double area() override {
        return 0.5 * base * height;
    }
};

class AreaCalculator {
public:
    double totalArea(vector<Shape*> shapes) {
        double total = 0;
        for (Shape* shape : shapes) {
            total += shape->area();
        }
        return total;
    }
};

int main() {
    Circle c(5);
    Rectangle r(4, 6);
    Triangle t(3, 4);
    
    vector<Shape*> shapes = {&c, &r, &t};
    AreaCalculator calc;
    
    cout << "Total area: " << calc.totalArea(shapes) << "\n";
    
    return 0;
}
```

### L - Liskov Substitution Principle

```cpp
#include <iostream>
using namespace std;

class Bird {
public:
    virtual void fly() = 0;
    virtual ~Bird() {}
};

// GOOD: Penguin shouldn't be Bird if it can't fly
// Instead, create separate hierarchy
class FlyingBird : public Bird {
public:
    void fly() override {
        cout << "Flying...\n";
    }
};

class Sparrow : public FlyingBird {
public:
    void fly() override {
        cout << "Sparrow flying\n";
    }
};

// Penguin is a Bird, but not a FlyingBird
class NonFlyingBird {
public:
    virtual void move() = 0;
    virtual ~NonFlyingBird() {}
};

class Penguin : public NonFlyingBird {
public:
    void move() override {
        cout << "Penguin swimming/waddling\n";
    }
};

int main() {
    Sparrow sparrow;
    sparrow.fly();
    
    Penguin penguin;
    penguin.move();
    
    return 0;
}
```

### I - Interface Segregation Principle

```cpp
#include <iostream>
using namespace std;

// BAD: Worker interface too bloated
// class Worker {
//     virtual void work() = 0;
//     virtual void eat() = 0;
//     virtual void sleep() = 0;
// };

// GOOD: Segregated interfaces
class Workable {
public:
    virtual void work() = 0;
    virtual ~Workable() {}
};

class Eatable {
public:
    virtual void eat() = 0;
    virtual ~Eatable() {}
};

class Sleepable {
public:
    virtual void sleep() = 0;
    virtual ~Sleepable() {}
};

class Human : public Workable, public Eatable, public Sleepable {
public:
    void work() override {
        cout << "Human working\n";
    }
    void eat() override {
        cout << "Human eating\n";
    }
    void sleep() override {
        cout << "Human sleeping\n";
    }
};

class Robot : public Workable {
public:
    void work() override {
        cout << "Robot working\n";
    }
};

int main() {
    Human human;
    human.work();
    human.eat();
    human.sleep();
    
    Robot robot;
    robot.work();
    // robot.eat();  // Robot doesn't have eat, which is correct!
    
    return 0;
}
```

### D - Dependency Inversion Principle

```cpp
#include <iostream>
#include <memory>
using namespace std;

// Abstraction
class DataStore {
public:
    virtual void save(const string& data) = 0;
    virtual ~DataStore() {}
};

// Concrete implementations
class DatabaseStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to database: " << data << "\n";
    }
};

class FileStore : public DataStore {
public:
    void save(const string& data) override {
        cout << "Saving to file: " << data << "\n";
    }
};

// High-level module depends on abstraction, not concrete class
class UserService {
private:
    shared_ptr<DataStore> dataStore;
    
public:
    UserService(shared_ptr<DataStore> store) : dataStore(store) {}
    
    void registerUser(const string& username) {
        cout << "Registering user: " << username << "\n";
        dataStore->save(username);
    }
};

int main() {
    // Can easily switch implementations
    auto dbStore = make_shared<DatabaseStore>();
    UserService service1(dbStore);
    service1.registerUser("Alice");
    
    cout << "\n";
    
    auto fileStore = make_shared<FileStore>();
    UserService service2(fileStore);
    service2.registerUser("Bob");
    
    return 0;
}
```
---

## Constructors & Destructors

### Constructors (C++98)

```cpp
#include <iostream>
#include <string>

class Car {
private:
    std::string brand;
    int year;
public:
    // Default constructor (no parameters)
    Car() : brand("Unknown"), year(2000) {
        std::cout << "Default constructor called\n";
    }
    
    // Parameterized constructor
    Car(const std::string& b, int y) : brand(b), year(y) {
        std::cout << "Constructor called\n";
    }
    
    // Copy constructor
    Car(const Car& other) : brand(other.brand), year(other.year) {
        std::cout << "Copy constructor called\n";
    }
    
    void show() const {
        std::cout << brand << " (" << year << ")\n";
    }
};

int main() {
    Car c1;                        // Calls default
    Car c2("Toyota", 2020);        // Calls parameterized
    Car c3 = c2;                   // Calls copy
    
    c1.show();
    c2.show();
    c3.show();
    
    return 0;
}
```

### Destructors (C++98)

```cpp
#include <iostream>
#include <fstream>

class File {
private:
    std::ofstream file;
public:
    File(const std::string& filename) {
        file.open(filename);
        if (file) {
            std::cout << "File opened\n";
        }
    }
    
    // Destructor (called when object destroyed)
    ~File() {
        if (file.is_open()) {
            file.close();
            std::cout << "File closed\n";
        }
    }
    
    void write(const std::string& data) {
        file << data << "\n";
    }
};

int main() {
    {
        File f("output.txt");
        f.write("Hello, World!");
    }  // Destructor called here, file closed
    
    return 0;
}
```

### 2.2 Advanced Constructor Features (C++11)

#### Delegating Constructors
One constructor can call another constructor of the same class to reduce code duplication.

```cpp
class Data {
    int x, y;
    std::string s;
public:
    // Target constructor
    Data(int x, int y, std::string s) : x(x), y(y), s(s) {}
    
    // Delegating constructor
    Data() : Data(0, 0, "default") {} 
    
    // Another delegating constructor
    Data(int x) : Data(x, 0, "default") {}
};
```

#### Inheriting Constructors
Using `using` to expose base class constructors.

```cpp
class Base {
public:
    Base(int x) { std::cout << "Base(int)\n"; }
};

class Derived : public Base {
public:
    using Base::Base; // Inherits Base(int)
    // Implicitly generates Derived(int x) : Base(x) {}
};
```

### 2.3 Construction & Destruction Order
The order is strict and deterministic (Stack logic: LIFO).

**Construction Order:**
1.  Base Classes (in order of inheritance)
2.  Member Objects (in order of declaration in class)
3.  Constructor Body

**Destruction Order:**
1.  Destructor Body
2.  Member Objects (reverse order of declaration)
3.  Base Classes (reverse order of inheritance)

```cpp
class Base { public: Base() { cout << "Base "; } ~Base() { cout << "~Base "; } };
class Member { public: Member() { cout << "Member "; } ~Member() { cout << "~Member "; } };

class Derived : public Base {
    Member m;
public:
    Derived() { cout << "Derived "; }
    ~Derived() { cout << "~Derived "; }
};

int main() {
    Derived d; // Output: Base Member Derived
    // End of scope: ~Derived ~Member ~Base
}
```

### 2.4 The Rule of Three, Five, and Zero

This is the cornerstone of resource management.

1.  **Rule of Three (C++98)**: If you implement one of: Destructor, Copy Constructor, Copy Assignment Operator; you likely need to implement all three.
2.  **Rule of Five (C++11)**: For Move semantics, add Move Constructor and Move Assignment Operator.
3.  **Rule of Zero**: If your class uses RAII types (`std::string`, `std::vector`, `std::unique_ptr`), do **NOT** declare any of the special member functions. Let the compiler generate them.

```cpp
// Rule of Zero Example (Best Practice)
class User {
    std::string name; // Manages its own memory
    std::vector<int> scores; // Manages its own memory
    // No destructor needed!
};
```

---

## Inheritance

### Basic Inheritance (C++98)

```cpp
#include <iostream>
#include <string>

// Base class
class Animal {
protected:
    std::string name;
public:
    Animal(const std::string& n) : name(n) {}
    
    virtual void speak() {  // virtual allows override
        std::cout << name << " makes a sound\n";
    }
    
    virtual ~Animal() {}
};

// Derived class
class Dog : public Animal {
public:
    Dog(const std::string& n) : Animal(n) {}
    
    void speak() override {  // override keyword (C++11)
        std::cout << name << " barks\n";
    }
};

class Cat : public Animal {
public:
    Cat(const std::string& n) : Animal(n) {}
    
    void speak() override {
        std::cout << name << " meows\n";
    }
};

int main() {
    Dog dog("Rex");
    Cat cat("Whiskers");
    
    dog.speak();
    cat.speak();
    
    // Polymorphism
    Animal* animals[2] = {&dog, &cat};
    for (int i = 0; i < 2; i++) {
        animals[i]->speak();
    }
    
    return 0;
}
```

### Multiple Inheritance (C++98)

```cpp
#include <iostream>

class A {
public:
    void func_a() { std::cout << "A::func\n"; }
};

class B {
public:
    void func_b() { std::cout << "B::func\n"; }
};

class C : public A, public B {
public:
    void func_c() { std::cout << "C::func\n"; }
};

int main() {
    C c;
    c.func_a();
    c.func_b();
    c.func_c();
    
    return 0;
}
```

---

## Virtual Functions & Polymorphism

### Virtual Functions (C++98)

```cpp
#include <iostream>
#include <vector>
#include <memory>

class Shape {
public:
    virtual void draw() = 0;  // Pure virtual (abstract)
    virtual double area() = 0;
    virtual ~Shape() {}
};

class Circle : public Shape {
private:
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() override {
        std::cout << "Drawing circle\n";
    }
    
    double area() override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
private:
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() override {
        std::cout << "Drawing rectangle\n";
    }
    
    double area() override {
        return width * height;
    }
};

int main() {
    std::vector<Shape*> shapes;
    shapes.push_back(new Circle(5));
    shapes.push_back(new Rectangle(4, 6));
    
    for (Shape* s : shapes) {
        s->draw();
        std::cout << "Area: " << s->area() << "\n";
    }
    
    // Cleanup
    for (Shape* s : shapes) {
        delete s;
    }
    
    return 0;
}
```

### 2.6 Advanced Polymorphism

#### 1. Virtual Destructors (CRITICAL)
If you delete a derived object through a base pointer, the base destructor must be virtual. Otherwise, the derived destructor is **never called**, causing memory leaks.

```cpp
class Base {
public:
    // virtual ~Base() {} // Correct
    ~Base() {} // Dangerous!
};

class Derived : public Base {
    int* ptr;
public:
    Derived() { ptr = new int[100]; }
    ~Derived() { delete[] ptr; }
};

Base* b = new Derived();
delete b; // If ~Base is not virtual, ~Derived is NOT called! Leak!
```

#### 2. Covariant Return Types
An override can return a pointer/reference to a *derived* class, not just the base.

```cpp
class Shape {
public:
    virtual Shape* clone() = 0;
};

class Circle : public Shape {
public:
    // Returns Circle* instead of Shape* - Valid!
    Circle* clone() override { return new Circle(*this); }
};
```

#### 3. RTTI & dynamic_cast
Run-Time Type Information allows safe downcasting. It uses the `vptr` to check the actual type.

```cpp
Shape* s = new Circle(5);

// Safe cast: returns nullptr if s is not a Circle
if (Circle* c = dynamic_cast<Circle*>(s)) {
    c->special_circle_method();
} else {
    std::cout << "Not a circle\n";
}
```

#### 4. Static Polymorphism (CRTP)
Curiously Recurring Template Pattern. Faster than virtual functions (compile-time resolution).

```cpp
template<typename Derived>
class Shape {
public:
    void draw() {
        // Compile-time dispatch
        static_cast<Derived*>(this)->draw_impl();
    }
};

class Circle : public Shape<Circle> {
public:
    void draw_impl() { cout << "Circle\n"; }
};
```

---
## <a name="chapter-4-deepobjectmodelvirtualization"></a>CHAPTER 4: DEEP OBJECT MODEL & VIRTUALIZATION

Understanding the "C++ Object Model" distinguishes a user from a master. This section explains what the compiler generates for your classes.

### 2.5.1 The Cost of Polymorphism (vptr & vtable)

Every class with *at least one* virtual function has a hidden overhead.

1.  **vptr (Virtual Pointer)**: A hidden member added to the *layout* of the object.
2.  **vtable (Virtual Table)**: A static table of function pointers for that class.

```cpp
class Base {
    int data;
    virtual void func() {}
};
// sizeof(Base) = sizeof(int) + sizeof(void*) + padding
// On 64-bit: 4 bytes (int) + 4 bytes (padding) + 8 bytes (ptr) = 16 bytes
```

### 2.5.2 Multiple Inheritance & Thunks

When inheriting from multiple classes, pointer arithmetic gets tricky.

```cpp
class A { int a; virtual void f() {} };
class B { int b; virtual void g() {} };
class C : public A, public B { int c; };

C obj;
A* pa = &obj; // Points to start of obj
B* pb = &obj; // Points to obj + sizeof(A) !!
```

*   **Thunk**: A small piece of assembly code generated by the compiler to adjust the `this` pointer when calling a virtual function from a base class pointer that isn't at offset 0.

### 2.5.3 Virtual Inheritance (The Diamond Problem)

```cpp
class Top { int t; };
class Left : virtual public Top { int l; };
class Right : virtual public Top { int r; };
class Bottom : public Left, public Right { int b; };
```

To solve the Diamond Problem (Top appearing twice), `virtual` inheritance ensures `Top` is shared.
*   **Cost**: `Left` and `Right` now contain a **vbptr** (Virtual Base Pointer) pointing to the shared `Top` instance. Accessing members of `Top` becomes slower (indirection).

### 2.5.4 Alignment & Padding Rules

CPU reads are efficient at specific addresses (multiples of 4 or 8). Compilers insert "padding bytes".

**Rule**: A member of size $N$ must sit at an offset divisible by $N$.

```cpp
struct Mixed {
    char a;     // 1 byte
                // 3 bytes PADDING
    int b;      // 4 bytes
    short c;    // 2 bytes
                // 6 bytes PADDING (to align structure size to 8)
};
// sizeof(Mixed) = 16 (on 64-bit)
```

**Optimization**: Sort members by size (Largest to Smallest) to minimize padding.

---


---
### Professional Notes: I/O Mastery

#### Chapter 14: Stream manipulators

Manipulators are special helper functions that help controlling input and output streams using operator >> or
operator <<.
They all can be included by #include <iomanip>.
Section 14.1: Stream manipulators
std::boolalpha and std::noboolalpha - switch between textual and numeric representation of booleans.
std::cout << std::boolalpha << 1;
// Output: true
std::cout << std::noboolalpha << false;
// Output: 0
bool boolValue;
std::cin >> std::boolalpha >> boolValue;
std::cout << "Value \"" << std::boolalpha << boolValue
          << "\" was parsed as " << std::noboolalpha << boolValue;
// Input: true
// Output: Value "true" was parsed as 0
std::showbase and std::noshowbase - control whether preﬁx indicating numeric base is used.
std::dec (decimal), std::hex (hexadecimal) and std::oct (octal) - are used for changing base for integers.
#include <sstream>
std::cout << std::dec << 29 << ' - '
          << std::hex << 29 << ' - '
          << std::showbase << std::oct << 29 << ' - '
          << std::noshowbase << 29  '\n';
int number;
std::istringstream("3B") >> std::hex >> number;
std::cout << std::dec << 10;
// Output: 22 - 1D - 35 - 035
// 59
Default values are std::ios_base::noshowbase and std::ios_base::dec.
If you want to see more about std::istringstream check out the <sstream> header.
std::uppercase and std::nouppercase - control whether uppercase characters are used in ﬂoating-point and
hexadecimal integer output. Have no eﬀect on input streams.
std::cout << std::hex << std::showbase
              << "0x2a with nouppercase: " << std::nouppercase << 0x2a << '\n'
              << "1e-10 with uppercase: " << std::uppercase << 1e-10 << '\n'
}
// Output: 0x2a with nouppercase: 0x2a
// 1e-10 with uppercase: 1E-10
Default is std::nouppercase.
std::setw(n) - changes the width of the next input/output ﬁeld to exactly n.
The width property n is resetting to 0 when some functions are called (full list is here).
std::cout << "no setw:" << 51 << '\n'
          << "setw(7): " << std::setw(7) << 51 << '\n'
          << "setw(7), more output: " << 13
          << std::setw(7) << std::setfill('*') << 67 << ' ' << 94 << '\n';
char* input = "Hello, world!";
char arr[10];
std::cin >> std::setw(6) >> arr;
std::cout << "Input from \"Hello, world!\" with setw(6) gave \"" << arr << "\"\n";
// Output: 51
// setw(7):      51
// setw(7), more output: 13*****67 94
// Input: Hello, world!
// Output: Input from "Hello, world!" with setw(6) gave "Hello"
Default is std::setw(0).
std::left, std::right and std::internal - modify the default position of the ﬁll characters by setting
std::ios_base::adjustfield to std::ios_base::left, std::ios_base::right and std::ios_base::internal
correspondingly. std::left and std::right apply to any output, std::internal - for integer, ﬂoating-point and
monetary output. Have no eﬀect on input streams.
#include <locale>
std::cout.imbue(std::locale("en_US.utf8"));
std::cout << std::left << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, false) << '\n';
// Output:
// flt: -9.87**********
// hex: 41*************
//   $: $3.67**********
// usd: USD *3.67******
// usd: $3.67          
std::cout << std::internal << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, true) << '\n';
// Output:
// flt: -**********9.87
// hex: *************41
//   $: $3.67**********
// usd: USD *******3.67
// usd: USD        3.67
std::cout << std::right << std::showbase << std::setfill('*')
          << "flt: " << std::setw(15) << -9.87  << '\n'
          << "hex: " << std::setw(15) << 41 << '\n'
          << "  $: " << std::setw(15) << std::put_money(367, false) << '\n'
          << "usd: " << std::setw(15) << std::put_money(367, true) << '\n'
          << "usd: " << std::setw(15)
          << std::setfill(' ') << std::put_money(367, true) << '\n';
// Output:
// flt: **********-9.87
// hex: *************41
//   $: **********$3.67
// usd: ******USD *3.67
// usd:       USD  3.67
Default is std::left.
std::fixed, std::scientific, std::hexfloat [C++11] and std::defaultfloat [C++11] - change formatting for
ﬂoating-point input/output.
std::fixed sets the std::ios_base::floatfield to std::ios_base::fixed,
std::scientific - to std::ios_base::scientific,
std::hexfloat - to std::ios_base::fixed | std::ios_base::scientific and
std::defaultfloat - to std::ios_base::fmtflags(0).
fmtflags
#include <sstream>
std::cout << '\n'
          << "The number 0.07 in fixed:      " << std::fixed << 0.01 << '\n'
          << "The number 0.07 in scientific: " << std::scientific << 0.01 << '\n'
          << "The number 0.07 in hexfloat:   " << std::hexfloat << 0.01 << '\n'
          << "The number 0.07 in default:    " << std::defaultfloat << 0.01 << '\n';
double f;
std::istringstream is("0x1P-1022");
double f = std::strtod(is.str().c_str(), NULL);
std::cout << "Parsing 0x1P-1022 as hex gives " << f << '\n';
// Output:
// The number 0.01 in fixed:      0.070000
// The number 0.01 in scientific: 7.000000e-02
// The number 0.01 in hexfloat:   0x1.1eb851eb851ecp-4
// The number 0.01 in default:    0.07
// Parsing 0x1P-1022 as hex gives 2.22507e-308
Default is std::ios_base::fmtflags(0).
There is a bug on some compilers which causes
double f;
std::istringstream("0x1P-1022") >> std::hexfloat >> f;
std::cout << "Parsing 0x1P-1022 as hex gives " << f << '\n';
// Output: Parsing 0x1P-1022 as hex gives 0
std::showpoint and std::noshowpoint - control whether decimal point is always included in ﬂoating-point
representation. Have no eﬀect on input streams.
std::cout << "7.0 with showpoint: " << std::showpoint << 7.0 << '\n'
          << "7.0 with noshowpoint: " << std::noshowpoint << 7.0 << '\n';
// Output: 1.0 with showpoint: 7.00000
// 1.0 with noshowpoint: 7
Default is std::showpoint.
std::showpos and std::noshowpos - control displaying of the + sign in non-negative output. Have no eﬀect on input
streams.
std::cout << "With showpos: " << std::showpos
          << 0 << ' ' << -2.718 << ' ' << 17 << '\n'
          << "Without showpos: " << std::noshowpos
          << 0 << ' ' << -2.718 << ' ' << 17 << '\n';
// Output: With showpos: +0 -2.718 +17
// Without showpos: 0 -2.718 17
Default if std::noshowpos.
std::unitbuf, std::nounitbuf - control ﬂushing output stream after every operation. Have no eﬀect on input
stream. std::unitbuf causes ﬂushing.
std::setbase(base) - sets the numeric base of the stream.
std::setbase(8) equals to setting std::ios_base::basefield to std::ios_base::oct,
std::setbase(16) - to std::ios_base::hex,
std::setbase(10) - to std::ios_base::dec.
If base is other then 8, 10 or 16 then std::ios_base::basefield is setting to std::ios_base::fmtflags(0). It
means decimal output and preﬁx-dependent input.
As default std::ios_base::basefield is std::ios_base::dec then by default std::setbase(10).
std::setprecision(n) - changes ﬂoating-point precision.
#include <cmath>
#include <limits>
typedef std::numeric_limits<long double> ld;
const long double pi = std::acos(-1.L);
std::cout << '\n'
          << "default precision (6):   pi: " << pi << '\n'
          << "                       10pi: " << 10 * pi << '\n'
          << "std::setprecision(4):  10pi: " << std::setprecision(4) << 10 * pi << '\n'
          << "                    10000pi: " << 10000 * pi << '\n'
          << "std::fixed:         10000pi: " << std::fixed << 10000 * pi << std::defaultfloat <<
'\n'
          << "std::setprecision(10):   pi: " << std::setprecision(10) << pi << '\n'
          << "max-1 radix precicion:   pi: " << std::setprecision(ld::digits - 1) << pi << '\n'
          << "max+1 radix precision:   pi: " << std::setprecision(ld::digits + 1) << pi << '\n'
          << "significant digits prec: pi: " << std::setprecision(ld::digits10) << pi << '\n';
// Output:
// default precision (6):   pi: 3.14159
//                        10pi: 31.4159
// std::setprecision(4):  10pi: 31.42
//                     10000pi: 3.142e+04
// std::fixed:         10000pi: 31415.9265
// std::setprecision(10):   pi: 3.141592654
// max-1 radix precicion:   pi: 3.14159265358979323851280895940618620443274267017841339111328125
// max+1 radix precision:   pi: 3.14159265358979323851280895940618620443274267017841339111328125
// significant digits prec: pi: 3.14159265358979324
Default is std::setprecision(6).
std::setiosflags(mask) and std::resetiosflags(mask) - set and clear ﬂags speciﬁed in mask of
std::ios_base::fmtflags type.
#include <sstream>
std::istringstream in("10 010 10 010 10 010");
int num1, num2;
in >> std::oct >> num1 >> num2;
std::cout << "Parsing \"10 010\" with std::oct gives:   " << num1 << ' ' << num2 << '\n';
// Output: Parsing "10 010" with std::oct gives:   8 8
in >> std::dec >> num1 >> num2;
std::cout << "Parsing \"10 010\" with std::dec gives:   " << num1 << ' ' << num2 << '\n';
// Output: Parsing "10 010" with std::oct gives:   10 10
in >> std::resetiosflags(std::ios_base::basefield) >> num1 >> num2;
std::cout << "Parsing \"10 010\" with autodetect gives: " << num1 << ' ' << num2 << '\n';
// Parsing "10 010" with autodetect gives: 10 8
std::cout << std::setiosflags(std::ios_base::hex |
                              std::ios_base::uppercase |
                              std::ios_base::showbase) << 42 << '\n';
// Output: OX2A
std::skipws and std::noskipws - control skipping of leading whitespace by the formatted input functions. Have no
eﬀect on output streams.
#include <sstream>
char c1, c2, c3;
std::istringstream("a b c") >> c1 >> c2 >> c3;
std::cout << "Default  behavior:  c1 = " << c1 << "  c2 = " << c2 << "  c3 = " << c3 << '\n';
std::istringstream("a b c") >> std::noskipws >> c1 >> c2 >> c3;
std::cout << "noskipws behavior:  c1 = " << c1 << "  c2 = " << c2 << "  c3 = " << c3 << '\n';
// Output: Default  behavior:  c1 = a  c2 = b  c3 = c
// noskipws behavior:  c1 = a  c2 =    c3 = b
Default is std::ios_base::skipws.
std::quoted(s[, delim[, escape]]) [C++14] - inserts or extracts quoted strings with embedded spaces.
s - the string to insert or extract.
delim - the character to use as the delimiter, " by default.
escape - the character to use as the escape character, \ by default.
#include <sstream>
std::stringstream ss;
std::string in = "String with spaces, and embedded \"quotes\" too";
std::string out;
ss << std::quoted(in);
std::cout << "read in     [" << in << "]\n"
          << "stored as   [" << ss.str() << "]\n";
ss >> std::quoted(out);
std::cout << "written out [" << out << "]\n";
// Output:
// read in     [String with spaces, and embedded "quotes" too]
// stored as   ["String with spaces, and embedded \"quotes\" too"]
// written out [String with spaces, and embedded "quotes" too]
For more information see the link above.
Section 14.2: Output stream manipulators
std::ends - inserts a null character '\0' to output stream. More formally this manipulator's declaration looks like
template <class charT, class traits>
std::basic_ostream<charT, traits>& ends(std::basic_ostream<charT, traits>& os);
and this manipulator places character by calling os.put(charT()) when used in an expression
os << std::ends;
std::endl and std::flush both ﬂush output stream out by calling out.flush(). It causes immediately producing
output. But std::endl inserts end of line '\n' symbol before ﬂushing.
std::cout << "First line." << std::endl << "Second line. " << std::flush
          << "Still second line.";
// Output: First line.
// Second line. Still second line.
std::setfill(c) - changes the ﬁll character to c. Often used with std::setw.
std::cout << "\nDefault fill: " << std::setw(10) << 79 << '\n'
          << "setfill('#'): " << std::setfill('#')
          << std::setw(10) << 42 << '\n';
// Output:
// Default fill:         79
// setfill('#'): ########79
std::put_money(mon[, intl]) [C++11]. In an expression out << std::put_money(mon, intl), converts the
monetary value mon (of long double or std::basic_string type) to its character representation as speciﬁed by the
std::money_put facet of the locale currently imbued in out. Use international currency strings if intl is true, use
currency symbols otherwise.
long double money = 123.45;
// or std::string money = "123.45";
std::cout.imbue(std::locale("en_US.utf8"));
std::cout << std::showbase << "en_US: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: en_US: $1.23 or USD  1.23
std::cout.imbue(std::locale("ru_RU.utf8"));
std::cout << "ru_RU: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: ru_RU: 1.23 руб or 1.23 RUB
std::cout.imbue(std::locale("ja_JP.utf8"));
std::cout << "ja_JP: " << std::put_money(money)
          << " or " << std::put_money(money, true) << '\n';
// Output: ja_JP: ￥123 or JPY  123
std::put_time(tmb, fmt) [C++11] - formats and outputs a date/time value to std::tm according to the speciﬁed
format fmt.
tmb - pointer to the calendar time structure const std::tm* as obtained from localtime() or gmtime().
fmt - pointer to a null-terminated string const CharT* specifying the format of conversion.
#include <ctime>
std::time_t t = std::time(nullptr);
std::tm tm = *std::localtime(&t);
std::cout.imbue(std::locale("ru_RU.utf8"));
std::cout << "\nru_RU: " << std::put_time(&tm, "%c %Z") << '\n';
// Possible output:
// ru_RU: Вт 04 июл 2017 15:08:35 UTC
For more information see the link above.
Section 14.3: Input stream manipulators
std::ws - consumes leading whitespaces in input stream. It diﬀerent from std::skipws.
#include <sstream>
std::string str;
std::istringstream("  \v\n\r\t    Wow!There   is no whitespaces!") >> std::ws >> str;
std::cout << str;
// Output: Wow!There   is no whitespaces!
std::get_money(mon[, intl]) [C++11]. In an expression in >> std::get_money(mon, intl) parses the character
input as a monetary value, as speciﬁed by the std::money_get facet of the locale currently imbued in in, and stores
the value in mon (of long double or std::basic_string type). Manipulator expects required international currency
strings if intl is true, expects optional currency symbols otherwise.
#include <sstream>
#include <locale>
std::istringstream in("$1,234.56 2.22 USD  3.33");
long double v1, v2;
std::string v3;
in.imbue(std::locale("en_US.UTF-8"));
in >> std::get_money(v1) >> std::get_money(v2) >> std::get_money(v3, true);
if (in) {
    std::cout << std::quoted(in.str()) << " parsed as: "
              << v1 << ", " << v2 << ", " << v3 << '\n';
}
// Output:
// "$1,234.56 2.22 USD  3.33" parsed as: 123456, 222, 333
std::get_time(tmb, fmt) [C++11] - parses a date/time value stored in tmb of speciﬁed format fmt.
tmb - valid pointer to the const std::tm* object where the result will be stored.
fmt - pointer to a null-terminated string const CharT* specifying the conversion format.
#include <sstream>
#include <locale>
std::tm t = {};
std::istringstream ss("2011-Februar-18 23:12:34");
ss.imbue(std::locale("de_DE.utf-8"));
ss >> std::get_time(&t, "%Y-%b-%d %H:%M:%S");
if (ss.fail()) {
    std::cout << "Parse failed\n";
}
else {
    std::cout << std::put_time(&t, "%c") << '\n';
}
// Possible output:
// Sun Feb 18 23:12:34 2011
For more information see the link above.

## <a name="chapter-5-c9803standardlibrary"></a>CHAPTER 5: C++98/03 STANDARD LIBRARY

## Standard Template Library

---

## Introduction to STL

The Standard Template Library (STL) is a collection of template classes and functions that provide:
- **Containers** - Data structures to hold objects
- **Iterators** - Objects to traverse containers
- **Algorithms** - Functions to manipulate data
- **Function Objects** - Objects that act like functions

### Key Advantages
- Generic programming (templates)
- High performance (optimized)
- Code reuse
- Type-safe
- Well-tested and standardized

---

## STL Components Overview

```
┌───────────────────────────────────────────┐
│          STL (Standard Template Library)  │
├───────────────────────────────────────────┤
│                                           │
│  ┌──────────────┐  ┌────────────────┐     │
│  │ CONTAINERS   │  │  ITERATORS     │     │
│  ├──────────────┤  ├────────────────┤     │
│  │ • Sequence   │  │ • Input        │     │
│  │ • Associative│  │ • Output       │     │
│  │ • Adapters   │  │ • Forward      │     │
│  └──────────────┘  │ • Bidirectional|     |
│                    │ • Random Access│     │
│  ┌──────────────┐  └──────────────--┘     │
│  │ ALGORITHMS   │  ┌──────────────┐       │
│  ├──────────────┤  │FUNCTION OBJ. │       │
│  │ • Searching  │  ├──────────────┤       │
│  │ • Sorting    │  │ • Predicates │       │
│  │ • Modifying  │  │ • Comparators│       │
│  │ • Numeric    │  │ • Functors   │       │
│  └──────────────┘  └──────────────┘       │
└───────────────────────────────────────────┘
```

---

# SECTION 1: CONTAINERS - COMPLETE REFERENCE

## Container Characteristics

| Container | Type | Insert | Delete | Search | Random Access | Memory |
|-----------|------|--------|--------|--------|---------------|--------|
| vector | Sequence | O(n) | O(n) | O(n) | O(1) | Contiguous |
| list | Sequence | O(1) | O(1) | O(n) | O(n) | Scattered |
| deque | Sequence | O(n) | O(n) | O(n) | O(1) | Blocks |
| map | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| set | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| multimap | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| multiset | Associative | O(log n) | O(log n) | O(log n) | - | Tree |
| unordered_map | Hash | O(1) avg | O(1) avg | O(1) avg | - | Hash |
| unordered_set | Hash | O(1) avg | O(1) avg | O(1) avg | - | Hash |
| priority_queue | Adapter | O(log n) | O(log n) | O(n) | - | Heap |
| queue | Adapter | O(1) | O(1) | O(n) | - | - |
| stack | Adapter | O(1) | O(1) | O(n) | - | - |

---

## 1.1 VECTOR - Dynamic Array

### What is Vector?
A dynamic array that grows automatically. Use this most of the time.

### Declaration & Initialization

```cpp
#include <vector>
using namespace std;

// Empty vector
vector<int> v1;

// Vector with initial size
vector<int> v2(10);           // 10 elements, initialized to 0

// Vector with initial values
vector<int> v3(5, 10);        // 5 elements, all set to 10

// Copy constructor
vector<int> v4(v3);           // Copy of v3

// From array (C++11)
int arr[] = {1, 2, 3, 4, 5};
vector<int> v5(arr, arr + 5); // From array

// Initializer list (C++11)
vector<int> v6 = {1, 2, 3, 4, 5};

// Deduction (C++17)
vector v7 = {1, 2, 3};        // Type deduced as vector<int>
```

### Accessing Elements

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// 1. Using operator[]
cout << v[0] << "\n";         // 10 - No bounds checking

// 2. Using at()
cout << v.at(0) << "\n";      // 10 - With bounds checking, throws exception

// 3. Front and back
cout << v.front() << "\n";    // 10 (first element)
cout << v.back() << "\n";     // 50 (last element)

// 4. Direct pointer access (C++11)
int* ptr = v.data();          // Pointer to underlying array
cout << ptr[0] << "\n";       // 10
```

### Modifying Elements

```cpp
vector<int> v = {10, 20, 30};

// Assignment
v[0] = 100;
v.at(1) = 200;

// Adding elements
v.push_back(40);              // Add to end: {10, 200, 30, 40}
v.insert(v.begin() + 1, 15);  // Insert 15 at index 1: {10, 15, 200, 30, 40}

// Removing elements
v.pop_back();                 // Remove last: {10, 15, 200, 30}
v.erase(v.begin() + 1);       // Remove at index 1: {10, 200, 30}
v.erase(v.begin(), v.begin() + 2);  // Remove first 2: {30}

// Clear all
v.clear();                    // Empty vector
```

### Size & Capacity

```cpp
vector<int> v = {1, 2, 3};

cout << v.size() << "\n";      // 3 - Number of elements
cout << v.capacity() << "\n";  // 3+ - Allocated space
cout << v.empty() << "\n";     // false

// Reserve space (optimization)
v.reserve(100);                // Allocate for 100 elements
cout << v.capacity() << "\n";  // 100

// Resize
v.resize(5, 0);                // Resize to 5, new elements = 0
v.resize(2);                   // Shrink to 2 elements

// Shrink to fit (C++11)
v.shrink_to_fit();             // Release unused capacity
```

### Iterating Through Vector

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// 1. Traditional for loop
for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
}

// 2. Iterator loop
for (vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 3. Auto with iterator (C++11)
for (auto it = v.begin(); it != v.end(); ++it) {
    cout << *it << " ";
}

// 4. Range-based for (C++11)
for (int val : v) {
    cout << val << " ";
}

// 5. Reverse iteration
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    cout << *it << " ";
}
```

### Comparison & Assignment

```cpp
vector<int> v1 = {1, 2, 3};
vector<int> v2 = {1, 2, 3};
vector<int> v3 = {1, 2, 4};

cout << (v1 == v2) << "\n";    // 1 (true)
cout << (v1 != v3) << "\n";    // 1 (true)
cout << (v1 < v3) << "\n";     // 1 (true) - lexicographic

// Assignment
v1 = v3;                       // Copy v3 to v1
v1.swap(v2);                   // Swap v1 and v2

// Swap single elements
swap(v1[0], v1[1]);
```

### Complete Vector Example

```cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
    vector<int> nums;
    
    // Adding elements
    for (int i = 1; i <= 5; i++) {
        nums.push_back(i * 10);
    }
    
    // Display
    cout << "Vector contents: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << "\n";
    
    // Modify
    nums[2] = 999;
    
    // Insert
    nums.insert(nums.begin() + 2, 777);
    
    // Remove
    nums.erase(nums.begin() + 1);
    
    // Display after modifications
    cout << "After modifications: ";
    for (int num : nums) {
        cout << num << " ";
    }
    cout << "\nSize: " << nums.size() << "\n";
    
    return 0;
}
```

---

## 1.2 DEQUE - Double Ended Queue

### What is Deque?
Fast insertion/deletion at both ends. Like vector but with efficient operations on both sides.

```cpp
#include <deque>
using namespace std;

// Declaration
deque<int> dq;

// Adding elements
dq.push_back(10);          // Add to end
dq.push_front(5);          // Add to front: {5, 10}

// Removing elements
dq.pop_back();             // Remove from end: {5}
dq.pop_front();            // Remove from front: {}

// Accessing
dq.push_back(20);
dq.push_back(30);
cout << dq.front() << "\n"; // 20
cout << dq.back() << "\n";  // 30
cout << dq[0] << "\n";      // 20 - supports random access

// Iteration
for (int val : dq) {
    cout << val << " ";
}

// Size operations
cout << dq.size() << "\n";
cout << dq.empty() << "\n";

// Clearing
dq.clear();
```

### Deque vs Vector
- **Deque**: Better for push_front/pop_front operations
- **Vector**: Better for push_back/pop_back and random access

---

## 1.3 LIST - Doubly Linked List

### What is List?
Efficient insertion/deletion anywhere. No random access.

```cpp
#include <list>
using namespace std;

// Declaration
list<int> lst;

// Adding elements
lst.push_back(10);         // Add to end
lst.push_front(5);         // Add to front: {5, 10}

// Insert at position
auto it = lst.begin();
++it;
lst.insert(it, 7);         // Insert 7 at position 1: {5, 7, 10}

// Removing elements
lst.pop_back();            // Remove from end
lst.pop_front();           // Remove from front
lst.erase(it);             // Remove at iterator
lst.remove(5);             // Remove all elements with value 5
lst.clear();               // Clear all

// Accessing
cout << lst.front() << "\n"; // First element
cout << lst.back() << "\n";  // Last element
// NO random access: lst[0] - NOT AVAILABLE

// Iteration
for (int val : lst) {
    cout << val << " ";
}

// Reverse iteration
for (auto it = lst.rbegin(); it != lst.rend(); ++it) {
    cout << *it << " ";
}

// Size
cout << lst.size() << "\n";

// Useful operations
lst.reverse();             // Reverse the list
lst.sort();                // Sort the list
lst.unique();              // Remove consecutive duplicates
lst.sort(greater<int>()); // Sort in descending order
```

### List-Specific Operations

```cpp
list<int> lst1 = {1, 2, 3};
list<int> lst2 = {4, 5, 6};

// Splice - move elements from one list to another
lst1.splice(lst1.end(), lst2);  // Append lst2 to lst1
// lst1: {1, 2, 3, 4, 5, 6}
// lst2: {} (empty)

// Merge - combine two sorted lists
list<int> a = {1, 3, 5};
list<int> b = {2, 4, 6};
a.merge(b);                      // a: {1, 2, 3, 4, 5, 6}, b: {}

// Remove if
list<int> nums = {1, 2, 3, 4, 5};
nums.remove_if([](int x) { return x % 2 == 0; }); // Remove even numbers
// nums: {1, 3, 5}
```

---

## 1.4 MAP - Sorted Key-Value Pairs

### What is Map?
Stores key-value pairs, sorted by key. Logarithmic operations.

```cpp
#include <map>
using namespace std;

// Declaration
map<string, int> ages;

// Insertion
ages["Alice"] = 30;
ages["Bob"] = 25;
ages["Carol"] = 28;
ages.insert({{"David", 32}});
ages.insert({"Eve", 27});

// Accessing
cout << ages["Alice"] << "\n";  // 30
cout << ages.at("Bob") << "\n"; // 25 - with bounds checking

// Check existence
if (ages.find("Alice") != ages.end()) {
    cout << "Alice found\n";
}

// Safe access with count
if (ages.count("Bob")) {
    cout << "Bob exists\n";
}

// Size
cout << ages.size() << "\n";

// Iteration
for (auto& pair : ages) {
    cout << pair.first << ": " << pair.second << "\n";
}

// With auto (C++11)
for (const auto& [name, age] : ages) {  // Structured binding (C++17)
    cout << name << ": " << age << "\n";
}

// Reverse iteration
for (auto it = ages.rbegin(); it != ages.rend(); ++it) {
    cout << it->first << ": " << it->second << "\n";
}
```

### Map Operations

```cpp
map<int, string> dict;

// Insert
dict[1] = "one";
dict[2] = "two";
dict[3] = "three";

// Erase
dict.erase(2);                 // Erase by key
dict.erase(dict.begin());      // Erase by iterator

// Find
auto it = dict.find(1);
if (it != dict.end()) {
    cout << it->second << "\n";
}

// Lower and upper bound
map<int, string> scores = {{1, "A"}, {2, "B"}, {3, "C"}};

// find first >= key
auto it1 = scores.lower_bound(2);  // Points to {2, "B"}

// Find first > key
auto it2 = scores.upper_bound(2);  // Points to {3, "C"}

// Range [lower_bound, upper_bound)
auto range = scores.equal_range(2);  // Pair of iterators
for (auto it = range.first; it != range.second; ++it) {
    cout << it->second << "\n";
}

// Clear
dict.clear();
```

### Map with Custom Comparator

```cpp
// Descending order
map<int, string, greater<int>> descending;
descending[3] = "three";
descending[1] = "one";
descending[2] = "two";
// Iteration order: 3, 2, 1

// Custom comparator
struct Compare {
    bool operator()(const string& a, const string& b) const {
        return a.length() < b.length();  // Sort by string length
    }
};

map<string, int, Compare> byLength;
byLength["a"] = 1;
byLength["abc"] = 3;
byLength["ab"] = 2;
```

---

## 1.5 SET - Sorted Unique Elements

### What is Set?
Stores unique, sorted elements. Like map but key only (no value).

```cpp
#include <set>
using namespace std;

// Declaration
set<int> nums;

// Insertion
nums.insert(30);
nums.insert(10);
nums.insert(20);
nums.insert(10);  // Duplicate - ignored
// {10, 20, 30}

// Accessing
auto it = nums.find(20);
if (it != nums.end()) {
    cout << "Found: " << *it << "\n";
}

// Count
cout << nums.count(20) << "\n";  // 1 or 0

// Iteration
for (int val : nums) {
    cout << val << " ";
}

// Erase
nums.erase(20);                // Erase by value
nums.erase(nums.begin());      // Erase by iterator
nums.erase(nums.lower_bound(15), nums.upper_bound(25));  // Range erase

// Size and empty
cout << nums.size() << "\n";
cout << nums.empty() << "\n";

// Clear
nums.clear();
```

### Set with Strings

```cpp
set<string> words;

words.insert("zebra");
words.insert("apple");
words.insert("mango");
words.insert("apple");  // Duplicate ignored

// Prints in alphabetical order
for (const string& word : words) {
    cout << word << "\n";
}
// Output: apple, mango, zebra
```

### Multiset - Allows Duplicates

```cpp
#include <set>

multiset<int> nums;

nums.insert(10);
nums.insert(20);
nums.insert(10);
nums.insert(20);
// {10, 10, 20, 20}

cout << nums.count(10) << "\n";  // 2

// All operations similar to set
for (int val : nums) {
    cout << val << " ";
}
```

---

## 1.6 MULTIMAP - Key-Value Pairs with Duplicate Keys

```cpp
#include <map>

multimap<string, int> students;

// Multiple values for same key
students.insert({"Math", 95});
students.insert({"Math", 87});
students.insert({"English", 92});
students.insert({"English", 88});

// Find all with key "Math"
auto range = students.equal_range("Math");
for (auto it = range.first; it != range.second; ++it) {
    cout << it->first << ": " << it->second << "\n";
}
// Output: Math: 95, Math: 87

// Count how many with key "Math"
cout << students.count("Math") << "\n";  // 2

// Iterate all
for (const auto& [subject, score] : students) {
    cout << subject << ": " << score << "\n";
}
```

---

## 1.7 UNORDERED_MAP - Hash-Based Key-Value Pairs

### What is Unordered Map?
Like map but no sorting, O(1) average operations.

```cpp
#include <unordered_map>

// Declaration
unordered_map<string, int> ages;

// Insertion - same as map
ages["Alice"] = 30;
ages["Bob"] = 25;

// Accessing - same as map
cout << ages["Alice"] << "\n";

// Find
if (ages.find("Bob") != ages.end()) {
    cout << "Found Bob\n";
}

// Erase
ages.erase("Alice");

// Iteration - ORDER IS ARBITRARY
for (const auto& [name, age] : ages) {
    cout << name << ": " << age << "\n";
}

// Size
cout << ages.size() << "\n";

// Bucket information
cout << ages.bucket_count() << "\n";      // Number of buckets
cout << ages.load_factor() << "\n";       // Load factor
cout << ages.max_load_factor() << "\n";   // Max load factor

// Rehash
ages.rehash(100);                         // Rehash with hint 100
ages.reserve(50);                         // Reserve space for 50 elements

// Clear
ages.clear();
```

### Unordered Set

```cpp
#include <unordered_set>

unordered_set<int> nums = {30, 10, 20};

// Similar to set but unordered
if (nums.count(10)) {
    cout << "Found 10\n";
}

for (int val : nums) {
    cout << val << " ";  // Order is arbitrary
}
```

---

## 1.8 QUEUE - FIFO (First In, First Out)

### What is Queue?
Adapter container. Elements added at back, removed from front.

```cpp
#include <queue>

queue<int> q;

// Adding (enqueue)
q.push(10);
q.push(20);
q.push(30);

// Size and check empty
cout << q.size() << "\n";
cout << q.empty() << "\n";

// Access front and back
cout << q.front() << "\n";  // 10 (first element)
cout << q.back() << "\n";   // 30 (last element)

// Removing (dequeue)
q.pop();  // Removes 10

// Typical queue pattern
while (!q.empty()) {
    cout << q.front() << " ";
    q.pop();
}
```

### Queue Example - Task Processing

```cpp
queue<string> taskQueue;

taskQueue.push("Task 1");
taskQueue.push("Task 2");
taskQueue.push("Task 3");

while (!taskQueue.empty()) {
    cout << "Processing: " << taskQueue.front() << "\n";
    taskQueue.pop();
}
// Output: Task 1, Task 2, Task 3
```

---

## 1.9 STACK - LIFO (Last In, First Out)

### What is Stack?
Adapter container. Elements added and removed from top.

```cpp
#include <stack>

stack<int> st;

// Adding (push)
st.push(10);
st.push(20);
st.push(30);

// Size and check empty
cout << st.size() << "\n";
cout << st.empty() << "\n";

// Access top
cout << st.top() << "\n";  // 30 (last added)

// Removing (pop)
st.pop();  // Removes 30

// Typical stack pattern
while (!st.empty()) {
    cout << st.top() << " ";
    st.pop();
}
// Output: 20 10
```

### Stack Example - Balanced Parentheses

```cpp
#include <stack>
#include <string>

bool isBalanced(string expr) {
    stack<char> st;
    
    for (char c : expr) {
        if (c == '(' || c == '[' || c == '{') {
            st.push(c);
        } else if (c == ')' || c == ']' || c == '}') {
            if (st.empty()) return false;
            
            char top = st.top();
            if ((c == ')' && top == '(') ||
                (c == ']' && top == '[') ||
                (c == '}' && top == '{')) {
                st.pop();
            } else {
                return false;
            }
        }
    }
    
    return st.empty();
}
```

---

## 1.10 PRIORITY_QUEUE - Heap-Based Container

### What is Priority Queue?
Elements removed in order of priority (max/min).

```cpp
#include <queue>

// Max heap (largest element has highest priority)
priority_queue<int> pq;

pq.push(10);
pq.push(30);
pq.push(20);

while (!pq.empty()) {
    cout << pq.top() << " ";  // Largest first
    pq.pop();
}
// Output: 30 20 10

// Min heap (smallest element has highest priority)
priority_queue<int, vector<int>, greater<int>> minPQ;

minPQ.push(10);
minPQ.push(30);
minPQ.push(20);

while (!minPQ.empty()) {
    cout << minPQ.top() << " ";  // Smallest first
    minPQ.pop();
}
// Output: 10 20 30
```

### Priority Queue with Custom Comparator

```cpp
struct Task {
    string name;
    int priority;
    
    // For priority_queue to work
    bool operator<(const Task& other) const {
        return priority < other.priority;  // Max heap on priority
    }
};

priority_queue<Task> tasks;

tasks.push({"Task A", 5});
tasks.push({"Task B", 10});
tasks.push({"Task C", 3});

while (!tasks.empty()) {
    cout << tasks.top().name << " (P: " << tasks.top().priority << ")\n";
    tasks.pop();
}
// Output: Task B (P: 10), Task A (P: 5), Task C (P: 3)
```

---

# SECTION 2: ITERATORS - DEEP DIVE

## Iterator Categories

```
                    ┌─────────────────────┐
                    │  Iterator           │
                    ├─────────────────────┤
                    │ • Single pass       │
                    │ • Basic operations  │
                    └──────────┬──────────┘
                               │
        ┌──────────────────────┼──────────────────────┐
        │                      │                      │
    ┌───▼────┐          ┌─────▼──────┐        ┌──────▼───┐
    │ Input  │          │   Output   │        │ Forward  │
    ├────────┤          ├────────────┤        ├──────────┤
    │ Read   │          │ Write      │        │ Read+Write
    │ ++, *  │          │ ++, =      │        │ All ops  │
    └────────┘          └────────────┘        └──────┬───┘
                                                     │
                        ┌────────────────────────────┘
                        │
                    ┌───▼─────────────┐
                    │  Bidirectional  │
                    ├─────────────────┤
                    │ ++, --, *, =    │
                    └────────┬────────┘
                             │
                    ┌────────▼────────┐
                    │ Random Access   │
                    ├─────────────────┤
                    │ All ops + []    │
                    └─────────────────┘
```

### Iterator Types

```cpp
#include <vector>
#include <list>
#include <map>

vector<int> vec;         // Random access
list<int> lst;           // Bidirectional
map<int, int> mp;        // Bidirectional
deque<int> dq;           // Random access
set<int> st;             // Bidirectional

// Declaring iterators
vector<int>::iterator it1;                 // Random access
list<int>::iterator it2;                   // Bidirectional
map<int, int>::iterator it3;               // Bidirectional
const vector<int>::iterator it4;           // Const iterator

// Auto (C++11)
auto it = vec.begin();                     // Type deduced
```

### Iterator Operations

```cpp
vector<int> v = {10, 20, 30, 40, 50};

auto it = v.begin();

// Navigation
++it;                    // Move forward
--it;                    // Move backward (if bidirectional)
it++;                    // Post-increment
it--;                    // Post-decrement

// Dereference
cout << *it << "\n";     // Get value

// Arithmetic (random access only)
it = it + 3;             // Move 3 positions forward
it = it - 2;             // Move 2 positions backward
it += 5;
it -= 3;

// Comparison
if (it == v.end()) {}    // Check equality
if (it != v.end()) {}    // Check inequality
if (it < v.end()) {}     // Less than (random access)

// Distance
int dist = distance(v.begin(), it);

// Advance
advance(it, 5);          // Move 5 positions forward
```

### Reverse Iterators

```cpp
vector<int> v = {10, 20, 30, 40, 50};

// Reverse iteration
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    cout << *it << " ";  // 50 40 30 20 10
}

// Reverse iteration with auto
for (auto val : v) {
    cout << val << " ";  // 10 20 30 40 50
}

// Reverse iteration (explicit)
for (int i = v.size() - 1; i >= 0; --i) {
    cout << v[i] << " ";  // 50 40 30 20 10
}
```

### Const Iterators

```cpp
const vector<int> v = {10, 20, 30};

// Const iterator
auto it = v.begin();     // const_iterator
cout << *it << "\n";     // OK - read
// *it = 100;            // Error - can't modify

// Explicit const iterator
const_iterator cit = v.cbegin();

// Reverse const iterator
auto rit = v.crbegin();

// Using const_iterator with non-const vector
vector<int> v2 = {1, 2, 3};
const_iterator it2 = v2.cbegin();
```

const_iterator it2 = v2.cbegin();
```

### Advanced Iterator Concepts

#### 1. Iterator Traits (std::iterator_traits)
Algorithms use `iterator_traits` to know what an iterator can do.

```cpp
#include <iterator>

template<typename Iter>
void my_advance(Iter& it, int n) {
    using category = typename std::iterator_traits<Iter>::iterator_category;
    
    if constexpr (std::is_base_of_v<std::random_access_iterator_tag, category>) {
        it += n; // O(1)
    } else {
        while (n--) ++it; // O(N)
    }
}
```

#### 2. Writing a Custom Iterator
To make a class compatible with STL algorithms (like `std::find`), you need a conformant iterator.

```cpp
class Integers {
    struct Iterator {
        using iterator_category = std::forward_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = int;
        using pointer           = int*;
        using reference         = int&;

        int value;
        Iterator(int v) : value(v) {}

        reference operator*() { return value; }
        pointer operator->() { return &value; }
        
        Iterator& operator++() { value++; return *this; }
        Iterator operator++(int) { Iterator tmp = *this; ++(*this); return tmp; }
        
        friend bool operator== (const Iterator& a, const Iterator& b) { return a.value == b.value; };
        friend bool operator!= (const Iterator& a, const Iterator& b) { return a.value != b.value; };
    };

public:
    Iterator begin() { return Iterator(0); }
    Iterator end()   { return Iterator(10); } // Range [0, 10)
};
```

#### 3. Stream Iterators
Treat IO streams as containers.

```cpp
#include <iterator>
#include <algorithm>

// Read ints from cin until EOF or invalid input
std::istream_iterator<int> input_it(std::cin);
std::istream_iterator<int> eos;

// Write ints to cout with ", " delimiter
std::ostream_iterator<int> output_it(std::cout, ", ");

std::copy(input_it, eos, output_it);
```

#### 4. Insert Iterators
Special output iterators that grow the container.

*   `std::back_inserter(c)`: Calls `c.push_back(val)`. (Vector, List, Deque)
*   `std::front_inserter(c)`: Calls `c.push_front(val)`. (List, Deque)
*   `std::inserter(c, it)`: Calls `c.insert(it, val)`. (Map, Set, List, Vector)

```cpp
std::vector<int> v;
std::fill_n(std::back_inserter(v), 5, 42); // v becomes {42, 42, 42, 42, 42}
```

---

# SECTION 3: ALGORITHMS - COMPLETE REFERENCE
# C++ STL Advanced - Extended Reference & Algorithms Library

## COMPREHENSIVE STL ALGORITHMS REFERENCE

### All Algorithms by Category (60+ Algorithms)

---

## NON-MODIFYING SEQUENCE ALGORITHMS

### 1. find Family
```cpp
#include <algorithm>

auto it = find(first, last, value);              // Find element
auto it = find_if(first, last, predicate);       // Find matching condition
auto it = find_if_not(first, last, predicate);   // Find non-matching (C++11)
```

### 2. count Family
```cpp
int n = count(first, last, value);               // Count occurrences
int n = count_if(first, last, predicate);        // Count matching
```

### 3. mismatch
```cpp
auto [it1, it2] = mismatch(first1, last1, first2);  // Find first difference
auto [it1, it2] = mismatch(first1, last1, first2, comp); // With comparator
```

### 4. equal
```cpp
bool eq = equal(first1, last1, first2);          // Compare ranges
bool eq = equal(first1, last1, first2, comp);    // With comparator
```

### 5. search & adjacent_find
```cpp
auto it = search(first, last, s_first, s_last);  // Find subsequence
auto it = search_n(first, last, count, value);   // Find N equal elements
auto it = adjacent_find(first, last);            // Find adjacent equal elements
auto it = adjacent_find(first, last, comp);      // With comparator
```

### 6. Logical Operations
```cpp
bool b = all_of(first, last, predicate);         // All match
bool b = any_of(first, last, predicate);         // Any matches
bool b = none_of(first, last, predicate);        // None match
```

### 7. Min/Max
```cpp
auto it = min_element(first, last);              // Find minimum
auto it = max_element(first, last);              // Find maximum
auto [minIt, maxIt] = minmax_element(first, last);  // Both (C++11)

auto it = min_element(first, last, comp);        // With comparator
auto it = max_element(first, last, comp);
auto [minIt, maxIt] = minmax_element(first, last, comp);
```

---

## MODIFYING SEQUENCE ALGORITHMS

### 1. copy Family
```cpp
copy(first, last, d_first);                      // Copy range
copy_n(first, count, d_first);                   // Copy N elements
copy_if(first, last, d_first, predicate);        // Conditional copy
copy_backward(first, last, d_last);              // Copy backwards
```

### 2. move (C++11)
```cpp
move(first, last, d_first);                      // Move range
move_backward(first, last, d_last);              // Move backwards
```

### 3. transform
```cpp
transform(first, last, d_first, op);             // Apply function
transform(first1, last1, first2, d_first, op);   // Apply to two ranges
```

### 4. fill & generate
```cpp
fill(first, last, value);                        // Fill with value
fill_n(first, count, value);                     // Fill N elements
generate(first, last, gen);                      // Generate values
generate_n(first, count, gen);                   // Generate N values
```

### 5. replace
```cpp
replace(first, last, old_value, new_value);      // Replace values
replace_if(first, last, predicate, new_value);   // Conditional replace
replace_copy(first, last, d_first, old, new);    // Copy with replace
replace_copy_if(first, last, d_first, pred, new);// Conditional copy-replace
```

### 6. swap & reverse
```cpp
swap(a, b);                                       // Swap two values
iter_swap(it1, it2);                             // Swap via iterators
reverse(first, last);                            // Reverse range
reverse_copy(first, last, d_first);              // Copy reversed
```

### 7. rotate
```cpp
rotate(first, middle, last);                     // Rotate range
rotate_copy(first, middle, last, d_first);       // Copy rotated
```

### 8. unique
```cpp
auto it = unique(first, last);                   // Remove consecutive duplicates
auto it = unique(first, last, comp);             // With comparator
auto it = unique_copy(first, last, d_first);     // Copy unique
auto it = unique_copy(first, last, d_first, comp);
```

### 9. shuffle & random
```cpp
shuffle(first, last, rng);                       // Random shuffle
random_shuffle(first, last);                     // Legacy shuffle
random_shuffle(first, last, randFunc);           // With random function
```

### 10. remove
```cpp
auto it = remove(first, last, value);            // Remove all matching
auto it = remove_if(first, last, predicate);     // Conditional remove
auto it = remove_copy(first, last, d_first, val);// Copy without matching
auto it = remove_copy_if(first, last, d_first, pred);
```

---

## SORTING & PARTITIONING ALGORITHMS

### Sorting
```cpp
sort(first, last);                               // Sort (introsort)
sort(first, last, comp);                         // With comparator
stable_sort(first, last);                        // Stable sort
stable_sort(first, last, comp);

partial_sort(first, middle, last);               // Sort first part
partial_sort(first, middle, last, comp);
partial_sort_copy(first, last, d_first, d_last);// Copy partially sorted

nth_element(first, nth, last);                   // Sort around nth
nth_element(first, nth, last, comp);
```

### Partitioning
```cpp
auto it = partition(first, last, predicate);     // Partition
auto it = stable_partition(first, last, pred);   // Stable partition
auto it = partition_copy(first, last, d_true, d_false, pred);  // Copy partitions

bool b = is_partitioned(first, last, predicate); // Check if partitioned
auto it = partition_point(first, last, predicate); // Find partition point
```

### Binary Search (requires sorted range)
```cpp
auto it = lower_bound(first, last, value);       // First >= value
auto it = upper_bound(first, last, value);       // First > value
auto [lo, hi] = equal_range(first, last, value);  // Range of value
bool b = binary_search(first, last, value);      // Check existence

auto it = lower_bound(first, last, value, comp);
auto it = upper_bound(first, last, value, comp);
auto [lo, hi] = equal_range(first, last, value, comp);
bool b = binary_search(first, last, value, comp);
```

---

## NUMERIC ALGORITHMS

```cpp
#include <numeric>

// Accumulation
int sum = accumulate(first, last, init);          // Sum
auto prod = accumulate(first, last, init, op);    // Custom operation

// Inner product (dot product)
int dot = inner_product(first1, last1, first2, init);
auto result = inner_product(first1, last1, first2, init, op1, op2);

// Partial sums
partial_sum(first, last, d_first);                // Cumulative sum
partial_sum(first, last, d_first, op);            // Custom operation

// Adjacent differences
adjacent_difference(first, last, d_first);        // Differences
adjacent_difference(first, last, d_first, op);    // Custom operation
```

---

## SET OPERATIONS (require sorted ranges)

```cpp
// Union - all unique elements
auto it = set_union(first1, last1, first2, last2, d_first);
auto it = set_union(first1, last1, first2, last2, d_first, comp);

// Intersection - common elements
auto it = set_intersection(first1, last1, first2, last2, d_first);
auto it = set_intersection(first1, last1, first2, last2, d_first, comp);

// Difference - in first but not in second
auto it = set_difference(first1, last1, first2, last2, d_first);
auto it = set_difference(first1, last1, first2, last2, d_first, comp);

// Symmetric difference - in one but not both
auto it = set_symmetric_difference(first1, last1, first2, last2, d_first);
auto it = set_symmetric_difference(first1, last1, first2, last2, d_first, comp);

// Check relationship
bool b = includes(first1, last1, first2, last2);   // first1 includes first2
bool b = includes(first1, last1, first2, last2, comp);
```

---

## HEAP OPERATIONS

```cpp
#include <algorithm>

make_heap(first, last);                          // Create heap
make_heap(first, last, comp);                    // With comparator

push_heap(first, last);                          // Add element to heap
pop_heap(first, last);                           // Extract max from heap
sort_heap(first, last);                          // Sort heap

bool b = is_heap(first, last);                   // Check if valid heap
bool b = is_heap(first, last, comp);
auto it = is_heap_until(first, last);            // Find where heap property breaks
```

---

## PERMUTATION ALGORITHMS

```cpp
bool b = next_permutation(first, last);          // Next lexicographic permutation
bool b = next_permutation(first, last, comp);
bool b = prev_permutation(first, last);          // Previous permutation
bool b = prev_permutation(first, last, comp);

bool b = is_permutation(first1, last1, first2);  // Check if permutation
bool b = is_permutation(first1, last1, first2, comp);
```

---

## COMPLETE STL ALGORITHMS QUICK REFERENCE

| Algorithm | Purpose | Returns |
|-----------|---------|---------|
| find | Find element | Iterator |
| find_if | Find matching | Iterator |
| count | Count occurrences | Count |
| equal | Compare ranges | Bool |
| search | Find subsequence | Iterator |
| sort | Sort range | Void |
| binary_search | Check existence (sorted) | Bool |
| lower_bound | First >= value (sorted) | Iterator |
| partition | Divide by condition | Iterator |
| copy | Copy range | Iterator |
| transform | Apply function | Iterator |
| remove | Remove matching | Iterator |
| unique | Remove duplicates | Iterator |
| reverse | Reverse range | Void |
| rotate | Rotate range | Void |
| min_element | Find minimum | Iterator |
| max_element | Find maximum | Iterator |
| accumulate | Sum/aggregate | Value |
| inner_product | Dot product | Value |
| set_union | Union of sets | Iterator |
| set_intersection | Intersection | Iterator |

---

## CONTAINERS DETAILED OPERATIONS

### Vector Operations
```cpp
v.push_back(val);               // Add to end
v.pop_back();                   // Remove from end
v.insert(pos, val);             // Insert at position
v.erase(pos);                   // Erase at position
v.clear();                      // Remove all
v.resize(n);                    // Change size
v.reserve(n);                   // Pre-allocate
v.shrink_to_fit();              // Release excess memory
v.swap(other);                  // Swap two vectors

// Access
v[i];                           // O(1) random access
v.at(i);                        // O(1) with bounds check
v.front();                      // First element
v.back();                       // Last element
v.data();                       // Raw pointer (C++11)

// Iteration
begin(v), end(v);               // Iterators
rbegin(v), rend(v);             // Reverse iterators
cbegin(v), cend(v);             // Const iterators (C++11)

// Properties
v.size();                       // Number of elements
v.capacity();                   // Allocated space
v.empty();                      // Check if empty
v.max_size();                   // Maximum possible size
```

### Map Operations
```cpp
m[key] = value;                 // Insert/update
m.insert({key, value});         // Insert
m.erase(key);                   // Erase by key
m.clear();                      // Remove all
m.swap(other);                  // Swap two maps

// Access
m[key];                         // Access (creates if missing)
m.at(key);                      // Access (throws if missing)
m.find(key);                    // Find key
m.count(key);                   // Check existence

// Range operations
m.lower_bound(key);             // First >= key
m.upper_bound(key);             // First > key
m.equal_range(key);             // All equal keys

// Iteration
m.begin(), m.end();             // Forward
m.rbegin(), m.rend();           // Reverse

// Properties
m.size();                       // Number of elements
m.empty();                      // Check if empty
```

### Set Operations
```cpp
s.insert(val);                  // Insert element
s.erase(val);                   // Erase by value
s.clear();                      // Remove all
s.swap(other);                  // Swap

// Access
s.find(val);                    // Find element
s.count(val);                   // Check existence (1 or 0)
s.lower_bound(val);             // First >= value
s.upper_bound(val);             // First > value
s.equal_range(val);             // All equal values

// Iteration
s.begin(), s.end();             // Forward
s.rbegin(), s.rend();           // Reverse

// Properties
s.size();
s.empty();
```

---

## ALGORITHM COMPLEXITY CHEAT SHEET

```
find, find_if, find_if_not:        O(n)
count, count_if:                   O(n)
search, search_n:                  O(n*m)
all_of, any_of, none_of:           O(n)

sort:                              O(n log n) avg
stable_sort:                       O(n log n)
partial_sort:                      O(n log k) k=distance(first,last)
nth_element:                       O(n) avg
make_heap:                         O(n)
push_heap:                         O(log n)
pop_heap:                          O(log n)

copy:                              O(n)
transform:                         O(n)
fill:                              O(n)
reverse:                           O(n)
rotate:                            O(n)
unique:                            O(n)
remove:                            O(n)
partition:                         O(n)
binary_search:                     O(log n)
lower_bound:                       O(log n)
upper_bound:                       O(log n)

accumulate:                        O(n)
inner_product:                     O(n)
partial_sum:                       O(n)

set_union:                         O(n+m)
set_intersection:                  O(n+m)
set_difference:                    O(n+m)
```

---

## CONTAINER COMPLEXITY COMPARISON

```
                  Insert  Delete  Search  Random Access  Memory
vector            O(n)    O(n)    O(n)    O(1)           Contiguous
deque             O(n)    O(n)    O(n)    O(1)           Chunks
list              O(1)    O(1)    O(n)    O(n)           Scattered
map               O(log n) O(log n) O(log n) -           Tree
set               O(log n) O(log n) O(log n) -           Tree
multimap          O(log n) O(log n) O(log n) -           Tree
multiset          O(log n) O(log n) O(log n) -           Tree
unordered_map     O(1)    O(1)    O(1)    -              Hash
unordered_set     O(1)    O(1)    O(1)    -              Hash
queue             O(1)    O(1)    O(n)    -              - (adapter)
stack             O(1)    O(1)    O(n)    -              - (adapter)
priority_queue    O(log n) O(log n) O(n)    -              Heap
```

---

## ITERATOR COMPARISON TABLE

```
Container        Iterator Type          Bidirectional  Random Access
vector           random access           Yes            Yes
deque            random access           Yes            Yes
list             bidirectional           Yes            No
map              bidirectional           Yes            No
set              bidirectional           Yes            No
multimap         bidirectional           Yes            No
multiset         bidirectional           Yes            No
unordered_map    forward                 No             No
unordered_set    forward                 No             No
```

---

## PRACTICAL STL PATTERNS

### Pattern 1: Find & Remove
```cpp
vector<int> v = {1, 2, 3, 2, 4, 2};
auto it = find(v.begin(), v.end(), 2);
if (it != v.end()) {
    v.erase(it);  // Remove first occurrence
}
// v: {1, 3, 2, 4, 2}

// Remove all
v.erase(remove(v.begin(), v.end(), 2), v.end());
// v: {1, 3, 4}
```

### Pattern 2: Filter & Copy
```cpp
vector<int> v = {1, 2, 3, 4, 5};
vector<int> even;

copy_if(v.begin(), v.end(), back_inserter(even),
    [](int x) { return x % 2 == 0; });
// even: {2, 4}
```

### Pattern 3: Transform & Collect
```cpp
vector<int> v = {1, 2, 3};
vector<int> squared;

transform(v.begin(), v.end(), back_inserter(squared),
    [](int x) { return x * x; });
// squared: {1, 4, 9}
```

### Pattern 4: Partition & Process
```cpp
vector<int> v = {1, 2, 3, 4, 5, 6};

auto it = partition(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });

// Process even numbers
for (auto i = v.begin(); i != it; ++i) {
    cout << *i << " ";  // 2 4 6
}
```

### Pattern 5: Sort & Deduplicate
```cpp
vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

sort(v.begin(), v.end());
auto it = unique(v.begin(), v.end());
v.erase(it, v.end());
// v: {1, 2, 3, 4, 5, 6, 9}
```

### Pattern 6: Group & Count
```cpp
map<string, int> frequency;

vector<string> words = {"apple", "banana", "apple", "cherry", "banana"};
for (const string& word : words) {
    frequency[word]++;
}

for (const auto& [word, count] : frequency) {
    cout << word << ": " << count << "\n";
}
// Output: apple: 2, banana: 2, cherry: 1
```

### Pattern 7: Custom Sorting
```cpp
struct Person {
    string name;
    int age;
};

vector<Person> people = {
    {"Alice", 30}, {"Bob", 25}, {"Carol", 30}
};

// Sort by age, then by name
sort(people.begin(), people.end(),
    [](const Person& a, const Person& b) {
        if (a.age != b.age) return a.age < b.age;
        return a.name < b.name;
    });
```

### Pattern 8: Merge Ranges
```cpp
vector<int> v1 = {1, 3, 5};
vector<int> v2 = {2, 4, 6};
vector<int> merged;

merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
    back_inserter(merged));
// merged: {1, 2, 3, 4, 5, 6}
```

---

## STL WITH LAMBDAS (C++11 and later)

```cpp
#include <algorithm>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Simple lambda
auto isEven = [](int x) { return x % 2 == 0; };

// With capture by value
int threshold = 5;
auto gt = [threshold](int x) { return x > threshold; };

// With capture by reference
int sum = 0;
for_each(v.begin(), v.end(),
    [&sum](int x) { sum += x; });

// Mutable lambda
auto counter = [count = 0]() mutable { return ++count; };

// Generic lambda (C++14)
auto print = [](auto x) { cout << x << " "; };
for_each(v.begin(), v.end(), print);

// Find even numbers
auto it = find_if(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });

// Count odd numbers
int oddCount = count_if(v.begin(), v.end(),
    [](int x) { return x % 2 != 0; });

// Transform to squares
vector<int> squared;
transform(v.begin(), v.end(), back_inserter(squared),
    [](int x) { return x * x; });

// Filter and collect
vector<int> filtered;
copy_if(v.begin(), v.end(), back_inserter(filtered),
    [](int x) { return x % 2 == 0; });
```

---

# SECTION 4: STRINGS - MASTER GUIDE

## 4.1 String Basics

```cpp
#include <string>
using namespace std;

// Declaration
string s1;                     // Empty string
string s2 = "Hello";           // Initialize with C-string
string s3("World");            // Constructor
string s4(5, 'a');             // 5 'a' characters: "aaaaa"
string s5 = s2;                // Copy
string s6(s2, 1, 3);           // Substring of s2 from pos 1, length 3: "ell"
string s7(s2.begin(), s2.end()); // From iterators

// C-string
const char* cstr = s2.c_str(); // Convert to C-string
const char* data = s2.data();  // Get data pointer
```

### Size and Capacity

```cpp
string s = "Hello";

cout << s.length() << "\n";   // 5
cout << s.size() << "\n";     // 5 (same as length)
cout << s.capacity() << "\n"; // >= 5

cout << s.empty() << "\n";    // 0 (false)
cout << s.max_size() << "\n"; // Maximum possible size

// Resize
s.resize(10, '*');            // Resize to 10, fill with '*'
// s: "Hello*****"

// Reserve
s.reserve(100);               // Reserve space for 100 characters

// Clear
s.clear();                    // Empty the string
```

---

## 4.2 Accessing Characters

```cpp
string s = "Hello";

// Using operator[]
cout << s[0] << "\n";        // 'H' - No bounds checking
cout << s.at(0) << "\n";     // 'H' - With bounds checking

// Front and back
cout << s.front() << "\n";   // 'H'
cout << s.back() << "\n";    // 'o'

// Modifying characters
s[0] = 'J';                  // "Jello"
s.at(1) = 'A';               // "JAllo"
s.front() = 'B';             // "BAllo"
s.back() = 'z';              // "BAllz"
```

---

## 4.3 String Concatenation

```cpp
string s1 = "Hello";
string s2 = "World";

// Using + operator
string s3 = s1 + " " + s2;   // "Hello World"

// Using += operator
s1 += " ";
s1 += s2;                    // s1: "Hello World"

// Using append()
s1.append(" C++");           // "Hello World C++"
s1.append(3, '!');           // "Hello World C+++!!"

// Using push_back()
s1.push_back('*');           // "Hello World C+++!!*"

// Using insert()
s1.insert(5, "_");           // Insert "_" at position 5
// s1: "Hello_ World C+++!!*"
```

---

## 4.4 String Searching

```cpp
string s = "Hello World Hello";

// find() - find substring
size_t pos = s.find("World");
if (pos != string::npos) {
    cout << "Found at position: " << pos << "\n";  // 6
}

// find() with starting position
pos = s.find("Hello", 2);    // Find "Hello" starting from position 2
// Returns 12 (second "Hello")

// rfind() - find from right
pos = s.rfind("Hello");      // Position of last "Hello"
// Returns 12

// find_first_of() - find first occurrence of any character in string
pos = s.find_first_of("aeiou");
// Returns position of first vowel: 1 ('e')

// find_last_of() - find last occurrence of any character
pos = s.find_last_of("aeiou");
// Returns position of last vowel: 14 ('o')

// find_first_not_of() - find first character NOT in string
pos = s.find_first_not_of("He");
// Returns 2 ('l')

// find_last_not_of() - find last character NOT in string
pos = s.find_last_not_of("o");
// Returns 15

// Check if string starts with (C++20)
// bool starts = s.starts_with("Hello");
// bool ends = s.ends_with("ello");
```

---

## 4.5 String Manipulation

```cpp
string s = "Hello World";

// Substring
string sub = s.substr(0, 5);  // "Hello"
string sub2 = s.substr(6);    // "World"

// Replace
s.replace(0, 5, "Hi");        // Replace first 5 chars with "Hi"
// s: "Hi World"

// Erase
s.erase(2, 1);                // Erase 1 char starting at position 2
// s: "HiWorld"

// remove() + erase() idiom (for removing specific characters)
string s2 = "H-e-l-l-o";
s2.erase(remove(s2.begin(), s2.end(), '-'), s2.end());
// s2: "Hello"

// Compare
string a = "apple";
string b = "apple";
cout << (a == b) << "\n";     // 1 (true)
cout << a.compare(b) << "\n"; // 0 (equal)

// Case conversion (manual)
for (char& c : s) {
    c = toupper(c);  // Convert to uppercase
}
// s: "HI WORLD"

// Transform with algorithm
transform(s.begin(), s.end(), s.begin(), ::toupper);
```

---

## 4.6 String Iteration

```cpp
string s = "Hello";

// Iterator
for (auto it = s.begin(); it != s.end(); ++it) {
    cout << *it << " ";
}

// Range-based for (C++11)
for (char c : s) {
    cout << c << " ";
}

// Index-based
for (int i = 0; i < s.length(); i++) {
    cout << s[i] << " ";
}

// Reverse iteration
for (auto it = s.rbegin(); it != s.rend(); ++it) {
    cout << *it << " ";
}
```

---

## 4.7 String Conversion

```cpp
#include <string>

// String to numbers
string num = "123";
int intVal = stoi(num);           // 123
long longVal = stol(num);         // 123L
float floatVal = stof("3.14");    // 3.14
double doubleVal = stod("3.14159"); // 3.14159

// Number to string
int x = 42;
string s1 = to_string(x);         // "42"
string s2 = to_string(3.14);      // "3.140000"
string s3 = to_string(true);      // "1"

// With radix (base)
string hex = to_string(255);      // "255" (decimal)
// For hex, use stringstream or manual conversion
```

---

## 4.8 String Comparison

```cpp
string s1 = "apple";
string s2 = "apple";
string s3 = "banana";

// Equality
cout << (s1 == s2) << "\n";      // 1 (true)
cout << (s1 != s3) << "\n";      // 1 (true)

// Ordering (lexicographic)
cout << (s1 < s3) << "\n";       // 1 (true) - "apple" < "banana"
cout << (s1 > s3) << "\n";       // 0 (false)

// compare() method
cout << s1.compare(s2) << "\n";  // 0 (equal)
cout << s1.compare(s3) << "\n";  // -1 (s1 < s3)
cout << s3.compare(s1) << "\n";  // 1 (s3 > s1)

// Compare substring
cout << s1.compare(0, 3, "app") << "\n";  // 0 (equal)

// Case-insensitive comparison (manual)
bool caseInsensitive = true;
for (int i = 0; i < s1.length() && i < s3.length(); i++) {
    if (tolower(s1[i]) != tolower(s3[i])) {
        caseInsensitive = false;
        break;
    }
}
```

---

## 4.9 String from Stringstream

```cpp
#include <sstream>

// Building string
ostringstream oss;
oss << "Value: " << 42 << ", Name: " << "Alice";
string result = oss.str();  // "Value: 42, Name: Alice"

// Parsing string
istringstream iss("10 20 30");
int a, b, c;
iss >> a >> b >> c;  // a=10, b=20, c=30

// Convert various types
int num = 42;
double pi = 3.14159;
string name = "Alice";

ostringstream convert;
convert << num << " " << pi << " " << name;
string combined = convert.str();

// Parse line with specific delimiter
istringstream lineStream("apple,banana,mango");
string fruit;
while (getline(lineStream, fruit, ',')) {
    cout << fruit << "\n";
}
```

---

# SECTION 5: FILE I/O - COMPLETE COVERAGE

## 5.1 File I/O Basics

```cpp
#include <fstream>
#include <iostream>
using namespace std;

// Output file stream (write)
ofstream outFile;
outFile.open("output.txt");

if (outFile.is_open()) {
    outFile << "Hello, File!\n";
    outFile << "This is a test.\n";
    outFile.close();
} else {
    cout << "Error opening file\n";
}

// Input file stream (read)
ifstream inFile;
inFile.open("output.txt");

if (inFile.is_open()) {
    string line;
    while (getline(inFile, line)) {
        cout << line << "\n";
    }
    inFile.close();
} else {
    cout << "Error opening file\n";
}
```

## 5.2 File Operations

### Open Modes

```cpp
#include <fstream>

// Write (truncate if exists)
ofstream file1("data.txt");  // Default
ofstream file2("data.txt", ios::out);  // Explicit

// Append
ofstream file3("data.txt", ios::app);

// Read
ifstream file4("data.txt");
ifstream file5("data.txt", ios::in);

// Read and Write
fstream file6("data.txt", ios::in | ios::out);

// Binary mode
ofstream binFile("data.bin", ios::binary);
ifstream binRead("data.bin", ios::binary);

// Truncate (discards existing content)
ofstream file7("data.txt", ios::trunc);

// Seek position
fstream file8("data.txt", ios::in | ios::out);
file8.seekg(10);  // Seek to position 10 for reading
file8.seekp(10);  // Seek to position 10 for writing
```

---

## 5.3 Writing to Files

```cpp
ofstream outFile("output.txt");

if (outFile) {
    // Write strings
    outFile << "Hello, World!\n";
    outFile << "Line 2\n";
    
    // Write numbers
    outFile << 42 << " " << 3.14 << "\n";
    
    // Write characters
    outFile << 'A' << 'B' << 'C' << "\n";
    
    // Write using put() for single character
    outFile.put('X');
    
    // Write raw data
    string data = "Raw data";
    outFile.write(data.c_str(), data.length());
    
    outFile.close();
}
```

---

## 5.4 Reading from Files

### Line by Line

```cpp
#include <fstream>
#include <string>

ifstream inFile("input.txt");

if (inFile) {
    string line;
    while (getline(inFile, line)) {
        cout << line << "\n";
    }
    inFile.close();
}
```

### Word by Word

```cpp
ifstream inFile("input.txt");

if (inFile) {
    string word;
    while (inFile >> word) {
        cout << word << "\n";
    }
    inFile.close();
}
```

### Character by Character

```cpp
ifstream inFile("input.txt");

if (inFile) {
    char ch;
    while (inFile.get(ch)) {
        cout << ch;
    }
    inFile.close();
}
```

### Specific Format

```cpp
ifstream inFile("data.txt");

if (inFile) {
    int id;
    string name;
    double salary;
    
    while (inFile >> id >> name >> salary) {
        cout << "ID: " << id << ", Name: " << name 
             << ", Salary: " << salary << "\n";
    }
    inFile.close();
}
```

---

## 5.5 Binary File I/O

```cpp
#include <fstream>

// Writing binary
struct Person {
    int age;
    double height;
    char initial;
};

ofstream binOut("people.bin", ios::binary);
Person p = {30, 5.9, 'A'};
binOut.write(reinterpret_cast<char*>(&p), sizeof(Person));
binOut.close();

// Reading binary
ifstream binIn("people.bin", ios::binary);
Person p2;
binIn.read(reinterpret_cast<char*>(&p2), sizeof(Person));
cout << "Age: " << p2.age << ", Height: " << p2.height << "\n";
binIn.close();

// Reading multiple binary objects
vector<Person> people;
Person p3;
while (binIn.read(reinterpret_cast<char*>(&p3), sizeof(Person))) {
    people.push_back(p3);
}
```

---

## 5.6 File Position

```cpp
fstream file("data.txt", ios::in | ios::out);

// Get position
streampos pos = file.tellg();  // Get read position
pos = file.tellp();            // Get write position

// Set position
file.seekg(0, ios::beg);       // Seek to beginning
file.seekg(0, ios::end);       // Seek to end
file.seekg(-10, ios::end);     // Seek 10 bytes from end
file.seekp(5);                 // Seek write position to 5

// File size
file.seekg(0, ios::end);
int fileSize = file.tellg();
cout << "File size: " << fileSize << " bytes\n";

file.close();
```

---

## 5.7 Error Handling

```cpp
ifstream file("input.txt");

// Check if file opened
if (!file) {
    cout << "Failed to open file\n";
    return;
}

// Check read state
if (file.fail()) {
    cout << "Read operation failed\n";
}

if (file.bad()) {
    cout << "Severe error\n";
}

if (file.eof()) {
    cout << "End of file reached\n";
}

// Clear error flags
file.clear();

// Check if good
if (file.good()) {
    cout << "File is in good state\n";
}

file.close();
```

---

## 5.8 Complete File I/O Example

```cpp
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

struct Student {
    int id;
    string name;
    double gpa;
};

// Write students to file
void writeStudents(const string& filename, const vector<Student>& students) {
    ofstream file(filename);
    
    for (const auto& student : students) {
        file << student.id << " " << student.name << " " << student.gpa << "\n";
    }
    
    file.close();
}

// Read students from file
vector<Student> readStudents(const string& filename) {
    vector<Student> students;
    ifstream file(filename);
    
    int id;
    string name;
    double gpa;
    
    while (file >> id >> name >> gpa) {
        students.push_back({id, name, gpa});
    }
    
    file.close();
    return students;
}

// Main
int main() {
    vector<Student> students = {
        {101, "Alice", 3.8},
        {102, "Bob", 3.5},
        {103, "Carol", 3.9}
    };
    
    // Write
    writeStudents("students.txt", students);
    
    // Read
    auto readData = readStudents("students.txt");
    
    for (const auto& s : readData) {
        cout << s.id << " " << s.name << " " << s.gpa << "\n";
    }
    
    return 0;
}
```

---

# SECTION 6: FUNCTION OBJECTS & COMPARATORS

## 6.1 Function Objects (Functors)

```cpp
// Function object for greater than comparison
struct GreaterThan {
    int value;
    
    GreaterThan(int v) : value(v) {}
    
    bool operator()(int x) const {
        return x > value;
    }
};

vector<int> v = {10, 20, 30, 40, 50};

// Use with algorithm
auto it = find_if(v.begin(), v.end(), GreaterThan(25));
if (it != v.end()) {
    cout << "Found: " << *it << "\n";  // 30
}

// Count elements greater than 25
int count = count_if(v.begin(), v.end(), GreaterThan(25));
cout << "Count: " << count << "\n";  // 3
```

## 6.2 Standard Comparators

```cpp
#include <functional>

// less - ascending order
sort(v.begin(), v.end(), less<int>());

// greater - descending order
sort(v.begin(), v.end(), greater<int>());

// equal_to, not_equal_to
count_if(v.begin(), v.end(), bind(equal_to<int>(), placeholders::_1, 20));

// Map with custom comparator
map<string, int, less<string>> ascending;      // A-Z
map<string, int, greater<string>> descending;  // Z-A
```

---

# SECTION 7: STL BEST PRACTICES

## 7.1 Container Selection

```
Use VECTOR when:
  - Need random access
  - Need cache locality
  - Mostly append operations

Use LIST when:
  - Need frequent insertion/deletion in middle
  - Don't need random access

Use DEQUE when:
  - Need efficient push_front/pop_front
  - Need random access

Use MAP/SET when:
  - Need sorted, unique elements
  - Need O(log n) lookup

Use UNORDERED_MAP/SET when:
  - Need O(1) average lookup
  - Don't care about order

Use QUEUE when:
  - Need FIFO behavior

Use STACK when:
  - Need LIFO behavior

Use PRIORITY_QUEUE when:
  - Need elements processed by priority
```

## 7.2 Algorithm Selection

```cpp
// For small data: simple loop
for (int i = 0; i < v.size(); i++) {
    // Process v[i]
}

// For searching: find_if
auto it = find_if(v.begin(), v.end(), predicate);

// For filtering: copy_if
copy_if(v.begin(), v.end(), back_inserter(result), predicate);

// For transforming: transform
transform(v.begin(), v.end(), result.begin(), function);

// For aggregating: accumulate
int sum = accumulate(v.begin(), v.end(), 0);

// For sorting: sort
sort(v.begin(), v.end());
```

## 7.3 Memory Management

```cpp
// Reserve space when size is known
vector<int> v;
v.reserve(1000);  // Avoid reallocations

// Clear and shrink
v.clear();
v.shrink_to_fit();  // Release memory

// Use move semantics
vector<int> getVector() {
    vector<int> v;
    // ... fill v
    return v;  // Move, not copy (C++11)
}
```

## 7.4 Performance Tips

```cpp
// Prefer iterators over indices in generic code
for (auto it = v.begin(); it != v.end(); ++it) {
    // Optimized for all container types
}

// Use const references to avoid copying
void process(const vector<int>& v);

// Pre-allocate space
map<string, int> m;
m.reserve(1000);

// Use stable algorithms when order matters
stable_sort(v.begin(), v.end());

// Avoid repeated function calls in loops
int size = v.size();
for (int i = 0; i < size; i++) {
    // Use cached size
}
```

---

### Containers Intro (C++98)

```cpp
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>

int main() {
    // Vector (dynamic array)
    std::vector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    
    for (int i = 0; i < v.size(); i++) {
        std::cout << v[i] << " ";
    }
    std::cout << "\n";
    
    // List (linked list)
    std::list<int> l;
    l.push_back(10);
    l.push_front(5);
    
    for (std::list<int>::iterator it = l.begin(); it != l.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
    
    return 0;
}
```

---



---
### Professional Notes: Standard Library & I/O
#### 8: Arrays
Arrays are elements of the same type placed in adjoining memory locations. The elements can be individually
referenced by a unique identiﬁer with an added index.
This allows you to declare multiple variable values of a speciﬁc type and access them individually without needing
to declare a variable for each value.
Section 8.1: Array initialization
An array is just a block of sequential memory locations for a speciﬁc type of variable. Arrays are allocated the same
way as normal variables, but with square brackets appended to its name [] that contain the number of elements
that ﬁt into the array memory.
The following example of an array uses the typ int, the variable name arrayOfInts, and the number of elements
[5] that the array has space for:
int arrayOfInts[5];
An array can be declared and initialized at the same time like this
int arrayOfInts[5] = {10, 20, 30, 40, 50};
When initializing an array by listing all of its members, it is not necessary to include the number of elements inside
the square brackets. It will be automatically calculated by the compiler. In the following example, it's 5:
int arrayOfInts[] = {10, 20, 30, 40, 50};
It is also possible to initialize only the ﬁrst elements while allocating more space. In this case, deﬁning the length in
brackets is mandatory. The following will allocate an array of length 5 with partial initialization, the compiler
initializes all remaining elements with the standard value of the element type, in this case zero.
int arrayOfInts[5] = {10,20}; // means 10, 20, 0, 0, 0
Arrays of other basic data types may be initialized in the same way.
char arrayOfChars[5]; // declare the array and allocate the memory, don't initialize
char arrayOfChars[5] = { 'a', 'b', 'c', 'd', 'e' } ; //declare and initialize
double arrayOfDoubles[5] = {1.14159, 2.14159, 3.14159, 4.14159, 5.14159};
string arrayOfStrings[5] = { "C++", "is", "super", "duper", "great!"};
It is also important to take note that when accessing array elements, the array's element index(or position) starts
from 0.
int array[5] = { 10/*Element no.0*/, 20/*Element no.1*/, 30, 40, 50/*Element no.4*/};
std::cout << array[4]; //outputs 50
std::cout << array[0]; //outputs 10
Section 8.2: A ﬁxed size raw array matrix (that is, a 2D raw
array)
// A fixed size raw array matrix (that is, a 2D raw array).
#include <iostream>
#include <iomanip>
using namespace std;
auto main() -> int
{
    int const   n_rows  = 3;
    int const   n_cols  = 7;
    int const   m[n_rows][n_cols] =             // A raw array matrix.
    {
        {  1,  2,  3,  4,  5,  6,  7 },
        {  8,  9, 10, 11, 12, 13, 14 },
        { 15, 16, 17, 18, 19, 20, 21 }
    };
    for( int y = 0; y < n_rows; ++y )
    {
        for( int x = 0; x < n_cols; ++x )
        {
            cout << setw( 4 ) << m[y][x];       // Note: do NOT use m[y,x]!
        }
        cout << '\n';
    }
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
C++ doesn't support special syntax for indexing a multi-dimensional array. Instead such an array is viewed as an
array of arrays (possibly of arrays, and so on), and the ordinary single index notation [i] is used for each level. In
the example above m[y] refers to row y of m, where y is a zero-based index. Then this row can be indexed in turn,
e.g. m[y][x], which refers to the xth item – or column – of row y.
I.e. the last index varies fastest, and in the declaration the range of this index, which here is the number of columns
per row, is the last and “innermost” size speciﬁed.
Since C++ doesn't provide built-in support for dynamic size arrays, other than dynamic allocation, a dynamic size
matrix is often implemented as a class. Then the raw array matrix indexing notation m[y][x] has some cost, either
by exposing the implementation (so that e.g. a view of a transposed matrix becomes practically impossible) or by
adding some overhead and slight inconvenience when it's done by returning a proxy object from operator[]. And
so the indexing notation for such an abstraction can and will usually be diﬀerent, both in look-and-feel and in the
order of indices, e.g. m(x,y) or m.at(x,y) or m.item(x,y).
Section 8.3: Dynamically sized raw array
// Example of raw dynamic size array. It's generally better to use std::vector.
#include <algorithm>            // std::sort
#include <iostream>
using namespace std;
auto int_from( istream& in ) -> int { int x; in >> x; return x; }
auto main()
    -> int
{
    cout << "Sorting n integers provided by you.\\n";
    cout << "n? ";
    int const   n   = int_from( cin );
    int*        a   = new int[n];       // ← Allocation of array of n items.
    for( int i = 1; i <= n; ++i )
    {
        cout << "The #" << i << " number, please: ";
        a[i-1] = int_from( cin );
    }
    sort( a, a + n );
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\\n';
    delete[] a;
}
A program that declares an array T a[n]; where n is determined a run-time, can compile with certain compilers
that support C99 variadic length arrays (VLAs) as a language extension. But VLAs are not supported by standard C++.
This example shows how to manually allocate a dynamic size array via a new[]-expression,
int*        a   = new int[n];       // ← Allocation of array of n items.
… then use it, and ﬁnally deallocate it via a delete[]-expression:
delete[] a;
The array allocated here has indeterminate values, but it can be zero-initialized by just adding an empty
parenthesis (), like this: new int[n](). More generally, for arbitrary item type, this performs a value-initialization.
As part of a function down in a call hierarchy this code would not be exception safe, since an exception before the
delete[] expression (and after the new[]) would cause a memory leak. One way to address that issue is to
automate the cleanup via e.g. a std::unique_ptr smart pointer. But a generally better way to address it is to just
use a std::vector: that's what std::vector is there for.
Section 8.4: Array size: type safe at compile time
#include      // size_t, ptrdiff_t
//----------------------------------- Machinery:
using Size = ptrdiff_t;
template< class Item, size_t n >
constexpr auto n_items( Item (&)[n] ) noexcept
-> Size
{ return n; }
//----------------------------------- Usage:
#include
using namespace std;
auto main()
-> int
{
int const   a[]     = {3, 1, 4, 1, 5, 9, 2, 6, 5, 4};
Size const  n       = n_items( a );
int         b[n]    = {};       // An array of the same size as a.
(void) b;
cout <}
The C idiom for array size, sizeof(a)/sizeof(a[0]), will accept a pointer as argument and will then generally yield
an incorrect result.
For C++11
using C++11 you can do:
std::extent<decltype(MyArray)>::value;
Example:
char MyArray[] = { 'X','o','c','e' };
const auto n = std::extent<decltype(MyArray)>::value;
std::cout << n << "\n"; // Prints 4
Up till C++17 (forthcoming as of this writing) C++ had no built-in core language or standard library utility to obtain
the size of an array, but this can be implemented by passing the array by reference to a function template, as shown
above. Fine but important point: the template size parameter is a size_t, somewhat inconsistent with the signed
Size function result type, in order to accommodate the g++ compiler which sometimes insists on size_t for
template matching.
With C++17 and later one may instead use std::size, which is specialized for arrays.
Section 8.5: Expanding dynamic size array by using
std::vector
// Example of std::vector as an expanding dynamic size array.
#include <algorithm>            // std::sort
#include <iostream>
#include <vector>               // std::vector
using namespace std;
int int_from( std::istream& in ) { int x = 0; in >> x; return x; }
int main()
{
    cout << "Sorting integers provided by you.\n";
    cout << "You can indicate EOF via F6 in Windows or Ctrl+D in Unix-land.\n";
    vector<int> a;      // ← Zero size by default.
    while( cin )
    {
        cout << "One number, please, or indicate EOF: ";
        int const x = int_from( cin );
        if( !cin.fail() ) { a.push_back( x ); }  // Expands as necessary.
    }
    sort( a.begin(), a.end() );
    int const n = a.size();
    for( int i = 0; i < n; ++i ) { cout << a[i] << ' '; }
    cout << '\n';
}
std::vector is a standard library class template that provides the notion of a variable size array. It takes care of all
the memory management, and the buﬀer is contiguous so a pointer to the buﬀer (e.g. &v[0] or v.data()) can be
passed to API functions requiring a raw array. A vector can even be expanded at run time, via e.g. the push_back
member function that appends an item.
The complexity of the sequence of n push_back operations, including the copying or moving involved in the vector
expansions, is amortized O(n). “Amortized”: on average.
Internally this is usually achieved by the vector doubling its buﬀer size, its capacity, when a larger buﬀer is needed.
E.g. for a buﬀer starting out as size 1, and being repeatedly doubled as needed for n=17 push_back calls, this
involves 1 + 2 + 4 + 8 + 16 = 31 copy operations, which is less than 2×n = 34. And more generally the sum of this
sequence can't exceed 2×n.
Compared to the dynamic size raw array example, this vector-based code does not require the user to supply (and
know) the number of items up front. Instead the vector is just expanded as necessary, for each new item value
speciﬁed by the user.
Section 8.6: A dynamic size matrix using std::vector for
storage
Unfortunately as of C++14 there's no dynamic size matrix class in the C++ standard library. Matrix classes that
support dynamic size are however available from a number of 3rd party libraries, including the Boost Matrix library
(a sub-library within the Boost library).
If you don't want a dependency on Boost or some other library, then one poor man's dynamic size matrix in C++ is
just like
vector<vector<int>> m( 3, vector<int>( 7 ) );
… where vector is std::vector. The matrix is here created by copying a row vector n times where n is the number
of rows, here 3. It has the advantage of providing the same m[y][x] indexing notation as for a ﬁxed size raw array
matrix, but it's a bit ineﬃcient because it involves a dynamic allocation for each row, and it's a bit unsafe because
it's possible to inadvertently resize a row.
A more safe and eﬃcient approach is to use a single vector as storage for the matrix, and map the client code's (x, y)
to a corresponding index in that vector:
// A dynamic size matrix using std::vector for storage.
//--------------------------------------------- Machinery:
#include         // std::copy
#include          // assert
#include  // std::initializer_list
#include            // std::vector
#include          // ptrdiff_t
namespace my {
using Size = ptrdiff_t;
using std::initializer_list;
using std::vector;
template< class Item >
class Matrix
{
private:
vector    items_;
Size            n_cols_;
auto index_for( Size const x, Size const y ) const
-> Size
{ return y*n_cols_ + x; }
public:
auto n_rows() const -> Size { return items_.size()/n_cols_; }
auto n_cols() const -> Size { return n_cols_; }
auto item( Size const x, Size const y )
-> Item&
{ return items_[index_for(x, y)]; }
auto item( Size const x, Size const y ) const
-> Item const&
{ return items_[index_for(x, y)]; }
Matrix(): n_cols_( 0 ) {}
Matrix( Size const n_cols, Size const n_rows )
: items_( n_cols*n_rows )
, n_cols_( n_cols )
{}
Matrix( initializer_list< initializer_list > const& values )
: items_()
, n_cols_( values.size() == 0? 0 : values.begin()->size() )
{
for( auto const& row : values )
{
assert( Size( row.size() ) == n_cols_ );
items_.insert( items_.end(), row.begin(), row.end() );
}
}
};
}  // namespace my
//--------------------------------------------- Usage:
using my::Matrix;
auto some_matrix()
-> Matrix
{
return
{
{  1,  2,  3,  4,  5,  6,  7 },
{  8,  9, 10, 11, 12, 13, 14 },
{ 15, 16, 17, 18, 19, 20, 21 }
};
}
#include
#include
using namespace std;
auto main() -> int
{
Matrix const m = some_matrix();
assert( m.n_cols() == 7 );
assert( m.n_rows() == 3 );
for( int y = 0, y_end = m.n_rows(); y < y_end; ++y )
{
for( int x = 0, x_end = m.n_cols(); x < x_end; ++x )
{
cout <← Note: not `m[y][x]`!
}
cout <}
}
Output:
1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21
The above code is not industrial grade: it's designed to show the basic principles, and serve the needs of students
learning C++.
For example, one may deﬁne operator() overloads to simplify the indexing notation.
#### 9: Iterators
Section 9.1: Overview
Iterators are Positions
Iterators are a means of navigating and operating on a sequence of elements and are a generalized extension of
pointers. Conceptually it is important to remember that iterators are positions, not elements. For example, take the
following sequence:
A B C
The sequence contains three elements and four positions
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
Elements are things within a sequence. Positions are places where meaningful operations can happen to the
sequence. For example, one inserts into a position, before or after element A, not into an element. Even deletion of
an element (erase(A)) is done by ﬁrst ﬁnding its position, then deleting it.
From Iterators to Values
To convert from a position to a value, an iterator is dereferenced:
auto my_iterator = my_vector.begin(); // position
auto my_value = *my_iterator; // value
One can think of an iterator as dereferencing to the value it refers to in the sequence. This is especially useful in
understanding why you should never dereference the end() iterator in a sequence:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           +-- An iterator here has no value. Do not dereference it!
+-------------- An iterator here dereferences to the value A.
In all the sequences and containers found in the C++ standard library, begin() will return an iterator to the ﬁrst
position, and end() will return an iterator to one past the last position (not the last position!). Consequently, the
names of these iterators in algorithms are oftentimes labelled first and last:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           |
+- first    +- last
It is also possible to obtain an iterator to any sequence, because even an empty sequence contains at least one
position:
+---+
|   |
+---+
In an empty sequence, begin() and end() will be the same position, and neither can be dereferenced:
+---+
|   |
+---+
  ↑
  |
  +- empty_sequence.begin()
  |
  +- empty_sequence.end()
The alternative visualization of iterators is that they mark the positions between elements:
+---+---+---+
| A | B | C |
+---+---+---+
↑   ^   ^   ↑
|           |
+- first    +- last
and dereferencing an iterator returns a reference to the element coming after the iterator. Some situations where
this view is particularly useful are:
insert operations will insert elements into the position indicated by the iterator,
erase operations will return an iterator corresponding to the same position as the one passed in,
an iterator and its corresponding reverse iterator are located in the same .position between elements
Invalid Iterators
An iterator becomes invalidated if (say, in the course of an operation) its position is no longer a part of a sequence.
An invalidated iterator cannot be dereferenced until it has been reassigned to a valid position. For example:
std::vector<int>::iterator first;
{
    std::vector<int> foo;
    first = foo.begin(); // first is now valid
} // foo falls out of scope and is destroyed
// At this point first is now invalid
The many algorithms and sequence member functions in the C++ standard library have rules governing when
iterators are invalidated. Each algorithm is diﬀerent in the way they treat (and invalidate) iterators.
Navigating with Iterators
As we know, iterators are for navigating sequences. In order to do that an iterator must migrate its position
throughout the sequence. Iterators can advance forward in the sequence and some can advance backwards:
auto first = my_vector.begin();
++first;                                             // advance the iterator 1 position
std::advance(first, 1);                              // advance the iterator 1 position
first = std::next(first);                            // returns iterator to the next element
std::advance(first, -1);                             // advance the iterator 1 position backwards
first = std::next(first, 20);                        // returns iterator to the element 20 position
forward
first = std::prev(first, 5);                         // returns iterator to the element 5 position
backward
auto dist = std::distance(my_vector.begin(), first); // returns distance between two iterators.
Note, second argument of std::distance should be reachable from the ﬁrst one(or, in other words first should be
less or equal than second).
Even though you can perform arithmetic operators with iterators, not all operations are deﬁned for all types of
iterators. a = b + 3; would work for Random Access Iterators, but wouldn't work for Forward or Bidirectional
Iterators, which still can be advanced by 3 position with something like b = a; ++b; ++b; ++b;. So it is
recommended to use special functions in case you are not sure what is iterator type (for example, in a template
function accepting iterator).
Iterator Concepts
The C++ standard describes several diﬀerent iterator concepts. These are grouped according to how they behave in
the sequences they refer to. If you know the concept an iterator models (behaves like), you can be assured of the
behavior of that iterator regardless of the sequence to which it belongs. They are often described in order from the
most to least restrictive (because the next iterator concept is a step better than its predecessor):
Input Iterators : Can be dereferenced only once per position. Can only advance, and only one position at a
time.
Forward Iterators : An input iterator that can be dereferenced any number of times.
Bidirectional Iterators : A forward iterator that can also advance backwards one position at a time.
Random Access Iterators : A bidirectional iterator that can advance forwards or backwards any number of
positions at a time.
Contiguous Iterators (since C++17) : A random access iterator that guaranties that underlying data is
contiguous in memory.
Algorithms can vary depending on the concept modeled by the iterators they are given. For example, although
random_shuffle can be implemented for forward iterators, a more eﬃcient variant that requires random access
iterators could be provided.
Iterator traits
Iterator traits provide uniform interface to the properties of iterators. They allow you to retrieve value, diﬀerence,
pointer, reference types and also category of iterator:
template<class Iter>
Iter find(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type val)  {
    while (first != last) {
        if (*first == val)
            return first;
        ++first;
    }
    return last;
}
Category of iterator can be used to specialize algorithms:
template<class BidirIt>
void test(BidirIt a, std::bidirectional_iterator_tag)  {
    std::cout << "Bidirectional iterator is used" << std::endl;
}
template<class ForwIt>
void test(ForwIt a, std::forward_iterator_tag)  {
    std::cout << "Forward iterator is used" << std::endl;
}
template<class Iter>
void test(Iter a)  {
    test(a, typename std::iterator_traits<Iter>::iterator_category());
}
Categories of iterators are basically iterators concepts, except Contiguous Iterators don't have their own tag, since it
was found to break code.
Section 9.2: Vector Iterator
begin returns an iterator to the ﬁrst element in the sequence container.
end returns an iterator to the ﬁrst element past the end.
If the vector object is const, both begin and end return a const_iterator. If you want a const_iterator to be
returned even if your vector is not const, you can use cbegin and cend.
Example:
#include <vector>
#include <iostream>
int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };  //intialize vector using an initializer_list
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    return 0;
}
Output:
1 2 3 4 5
Section 9.3: Map Iterator
An iterator to the ﬁrst element in the container.
If a map object is const-qualiﬁed, the function returns a const_iterator. Otherwise, it returns an iterator.
// Create a map and insert some values
std::map<char,int> mymap;
mymap['b'] = 100;
mymap['a'] = 200;
mymap['c'] = 300;
// Iterate over all tuples
for (std::map<char,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
Output:
a => 200
b => 100
c => 300
Section 9.4: Reverse Iterators
If we want to iterate backwards through a list or vector we can use a reverse_iterator. A reverse iterator is made
from a bidirectional, or random access iterator which it keeps as a member which can be accessed through base().
To iterate backwards use rbegin() and rend() as the iterators for the end of the collection, and the start of the
collection respectively.
For instance, to iterate backwards use:
std::vector<int> v{1, 2, 3, 4, 5};
for (std::vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it)
{
    cout << *it;
} // prints 54321
A reverse iterator can be converted to a forward iterator via the base() member function. The relationship is that
the reverse iterator references one element past the base() iterator:
std::vector<int>::reverse_iterator r = v.rbegin();
std::vector<int>::iterator i = r.base();
assert(&*r == &*(i-1)); // always true if r, (i-1) are dereferenceable
                        // and are not proxy iterators
 +---+---+---+---+---+---+---+
 |   | 1 | 2 | 3 | 4 | 5 |   |
 +---+---+---+---+---+---+---+
   ↑   ↑               ↑   ↑
   |   |               |   |
rend() |         rbegin()  end()
       |                   rbegin().base()
     begin()
     rend().base()
In the visualization where iterators mark positions between elements, the relationship is simpler:
  +---+---+---+---+---+
| 1 | 2 | 3 | 4 | 5 |
+---+---+---+---+---+
↑                   ↑
|                   |
|                 end()
|                 rbegin()
begin()             rbegin().base()
rend()
rend().base()
Section 9.5: Stream Iterators
Stream iterators are useful when we need to read a sequence or print formatted data from a container:
// Data stream. Any number of various whitespace characters will be OK.
std::istringstream istr("1\t 2     3 4");
std::vector<int> v;
// Constructing stream iterators and copying data from stream into vector.
std::copy(
    // Iterator which will read stream data as integers.
    std::istream_iterator<int>(istr),
    // Default constructor produces end-of-stream iterator.
    std::istream_iterator<int>(),
    std::back_inserter(v));
// Print vector contents.
std::copy(v.begin(), v.end(),
    //Will print values to standard output as integers delimeted by " -- ".
    std::ostream_iterator<int>(std::cout, " -- "));
The example program will print 1 -- 2 -- 3 -- 4 -- to standard output.
Section 9.6: C Iterators (Pointers)
// This creates an array with 5 values.
const int array[] = { 1, 2, 3, 4, 5 };
#ifdef BEFORE_CPP11
// You can use `sizeof` to determine how many elements are in an array.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
// Then you can iterate over the array by incrementing a pointer until
// it reaches past the end of our array.
for (const int* i = first; i < afterLast; ++i) {
    std::cout << *i << std::endl;
}
#else
// With C++11, you can let the STL compute the start and end iterators:
for (auto i = std::begin(array); i != std::end(array); ++i) {
    std::cout << *i << std::endl;
}
#endif
This code would output the numbers 1 through 5, one on each line like this:
Breaking It Down
const int array[] = { 1, 2, 3, 4, 5 };
This line creates a new integer array with 5 values. C arrays are just pointers to memory where each value is stored
together in a contiguous block.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
These lines create two pointers. The ﬁrst pointer is given the value of the array pointer, which is the address of the
ﬁrst element in the array. The sizeof operator when used on a C array returns the size of the array in bytes.
Divided by the size of an element this gives the number of elements in the array. We can use this to ﬁnd the
address of the block after the array.
for (const int* i = first; i < afterLast; ++i) {
Here we create a pointer which we will use as an iterator. It is initialized with the address of the ﬁrst element we
want to iterate over, and we'll continue to iterate as long as i is less than afterLast, which means as long as i is
pointing to an address within array.
    std::cout << *i << std::endl;
Finally, within the loop we can access the value our iterator i is pointing to by dereferencing it. Here the
dereference operator * returns the value at the address in i.
Section 9.7: Write your own generator-backed iterator
A common pattern in other languages is having a function that produces a "stream" of objects, and being able to
use loop-code to loop over it.
We can model this in C++ as
template<class T>
struct generator_iterator {
  using difference_type=std::ptrdiff_t;
  using value_type=T;
  using pointer=T*;
  using reference=T;
  using iterator_category=std::input_iterator_tag;
  std::optional<T> state;
  std::function< std::optional<T>() > operation;
  // we store the current element in "state" if we have one:
  T operator*() const {
    return *state;
  }
  // to advance, we invoke our operation.  If it returns a nullopt
  // we have reached the end:
  generator_iterator& operator++() {
    state = operation();
    return *this;        
  }
  generator_iterator operator++(int) {
    auto r = *this;
    ++(*this);
    return r;
  }
  // generator iterators are only equal if they are both in the "end" state:
  friend bool operator==( generator_iterator const& lhs, generator_iterator const& rhs ) {
    if (!lhs.state && !rhs.state) return true;
    return false;
  }
  friend bool operator!=( generator_iterator const& lhs, generator_iterator const& rhs ) {
    return !(lhs==rhs);
  }
  // We implicitly construct from a std::function with the right signature:
  generator_iterator( std::function< std::optional<T>() > f ):operation(std::move(f))
  {
    if (operation)
      state = operation();
  }
  // default all special member functions:
  generator_iterator( generator_iterator && ) =default;
  generator_iterator( generator_iterator const& ) =default;
  generator_iterator& operator=( generator_iterator && ) =default;
  generator_iterator& operator=( generator_iterator const& ) =default;
  generator_iterator() =default;
};
live example.
We store the generated element early so we can more easily detect if we are already at the end.
As the function of an end generator iterator is never used, we can create a range of generator iterators by only
copying the std::function once. A default constructed generator iterator compares equal to itself, and to all other
end-generator-iterators.
#### 12: File I O
C++ ﬁle I/O is done via streams. The key abstractions are:
std::istream for reading text.
std::ostream for writing text.
std::streambuf for reading or writing characters.
Formatted input uses operator>>.
Formatted output uses operator<<.
Streams use std::locale, e.g., for details of the formatting and for translation between external encodings and the
internal encoding.
More on streams: <iostream> Library
Section 12.1: Writing to a ﬁle
There are several ways to write to a ﬁle. The easiest way is to use an output ﬁle stream (ofstream) together with the
stream insertion operator (<<):
std::ofstream os("foo.txt");
if(os.is_open()){
    os << "Hello World!";
}
Instead of <<, you can also use the output ﬁle stream's member function write():
std::ofstream os("foo.txt");
if(os.is_open()){
    char data[] = "Foo";
    // Writes 3 characters from data -> "Foo".
    os.write(data, 3);
}
After writing to a stream, you should always check if error state ﬂag badbit has been set, as it indicates whether the
operation failed or not. This can be done by calling the output ﬁle stream's member function bad():
os << "Hello Badbit!"; // This operation might fail for any reason.
if (os.bad())
    // Failed to write!
Section 12.2: Opening a ﬁle
Opening a ﬁle is done in the same way for all 3 ﬁle streams (ifstream, ofstream, and fstream).
You can open the ﬁle directly in the constructor:
std::ifstream ifs("foo.txt");  // ifstream: Opens file "foo.txt" for reading only.
std::ofstream ofs("foo.txt");  // ofstream: Opens file "foo.txt" for writing only.
std::fstream iofs("foo.txt");  // fstream:  Opens file "foo.txt" for reading and writing.
Alternatively, you can use the ﬁle stream's member function open():
std::ifstream ifs;
ifs.open("bar.txt");           // ifstream: Opens file "bar.txt" for reading only.
std::ofstream ofs;
ofs.open("bar.txt");           // ofstream: Opens file "bar.txt" for writing only.
std::fstream iofs;
iofs.open("bar.txt");          // fstream:  Opens file "bar.txt" for reading and writing.
You should always check if a ﬁle has been opened successfully (even when writing). Failures can include: the ﬁle
doesn't exist, ﬁle hasn't the right access rights, ﬁle is already in use, disk errors occurred, drive disconnected ...
Checking can be done as follows:
// Try to read the file 'foo.txt'.
std::ifstream ifs("fooo.txt");  // Note the typo; the file can't be opened.
// Check if the file has been opened successfully.
if (!ifs.is_open()) {
    // The file hasn't been opened; take appropriate actions here.
    throw CustomException(ifs, "File could not be opened");
}
When ﬁle path contains backslashes (for example, on Windows system) you should properly escape them:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs("c:\\\\folder\\\\foo.txt"); // using escaped backslashes
Version ≥ C++11
or use raw literal:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs(R"(c:\\folder\\foo.txt)"); // using raw literal
or use forward slashes instead:
// Open the file 'c:\\folder\\foo.txt' on Windows.
std::ifstream ifs("c:/folder/foo.txt");
Version ≥ C++11
If you want to open ﬁle with non-ASCII characters in path on Windows currently you can use non-standard wide
character path argument:
// Open the file 'пример\\foo.txt' on Windows.
std::ifstream ifs(LR"(пример\\foo.txt)"); // using wide characters with raw literal
Section 12.3: Reading from a ﬁle
There are several ways to read data from a ﬁle.
If you know how the data is formatted, you can use the stream extraction operator (>>). Let's assume you have a ﬁle
named foo.txt which contains the following data:
John Doe 25 4 6 1987
Jane Doe 15 5 24 1976
Then you can use the following code to read that data from the ﬁle:
// Define variables.
std::ifstream is("foo.txt");
std::string firstname, lastname;
int age, bmonth, bday, byear;
// Extract firstname, lastname, age, bday month, bday day, and bday year in that order.
// Note: '>>' returns false if it reached EOF (end of file) or if the input data doesn't
// correspond to the type of the input variable (for example, the string "foo" can't be
// extracted into an 'int' variable).
while (is >> firstname >> lastname >> age >> bmonth >> bday >> byear)
    // Process the data that has been read.
The stream extraction operator >> extracts every character and stops if it ﬁnds a character that can't be stored or if
it is a special character:
For string types, the operator stops at a whitespace () or at a newline (\n).
For numbers, the operator stops at a non-number character.
This means that the following version of the ﬁle foo.txt will also be successfully read by the previous code:
John
Doe 25
4 6 1987
Jane
Doe
15 5
The stream extraction operator >> always returns the stream given to it. Therefore, multiple operators can be
chained together in order to read data consecutively. However, a stream can also be used as a Boolean expression
(as shown in the while loop in the previous code). This is because the stream classes have a conversion operator
for the type bool. This bool() operator will return true as long as the stream has no errors. If a stream goes into an
error state (for example, because no more data can be extracted), then the bool() operator will return false.
Therefore, the while loop in the previous code will be exited after the input ﬁle has been read to its end.
If you wish to read an entire ﬁle as a string, you may use the following code:
// Opens 'foo.txt'.
std::ifstream is("foo.txt");
std::string whole_file;
// Sets position to the end of the file.
is.seekg(0, std::ios::end);
// Reserves memory for the file.
whole_file.reserve(is.tellg());
// Sets position to the start of the file.
is.seekg(0, std::ios::beg);
// Sets contents of 'whole_file' to all characters in the file.
whole_file.assign(std::istreambuf_iterator<char>(is),
  std::istreambuf_iterator<char>());
This code reserves space for the string in order to cut down on unneeded memory allocations.
If you want to read a ﬁle line by line, you can use the function getline():
std::ifstream is("foo.txt");  
// The function getline returns false if there are no more lines.
for (std::string str; std::getline(is, str);) {
    // Process the line that has been read.
}
If you want to read a ﬁxed number of characters, you can use the stream's member function read():
std::ifstream is("foo.txt");
char str[4];
// Read 4 characters from the file.
is.read(str, 4);
After executing a read command, you should always check if the error state ﬂag failbit has been set, as it
indicates whether the operation failed or not. This can be done by calling the ﬁle stream's member function fail():
is.read(str, 4); // This operation might fail for any reason.
if (is.fail())
    // Failed to read!
Section 12.4: Opening modes
When creating a ﬁle stream, you can specify an opening mode. An opening mode is basically a setting to control
how the stream opens the ﬁle.
(All modes can be found in the std::ios namespace.)
An opening mode can be provided as second parameter to the constructor of a ﬁle stream or to its open() member
function:
std::ofstream os("foo.txt", std::ios::out | std::ios::trunc);
std::ifstream is;
is.open("foo.txt", std::ios::in | std::ios::binary);
It is to be noted that you have to set ios::in or ios::out if you want to set other ﬂags as they are not implicitly set
by the iostream members although they have a correct default value.
If you don't specify an opening mode, then the following default modes are used:
ifstream - in
ofstream - out
fstream - in and out
The ﬁle opening modes that you may specify by design are:
Mode Meaning
app
append Output
For
Description
Appends data at the end of the ﬁle.
binary binary
Input/Output Input and output is done in binary.
in
out
input
Input
Opens the ﬁle for reading.
output Output
Opens the ﬁle for writing.
trunc truncate Input/Output Removes contents of the ﬁle when opening.
ate
at end
Input
Goes to the end of the ﬁle when opening.
Note: Setting the binary mode lets the data be read/written exactly as-is; not setting it enables the translation of
the newline '\n' character to/from a platform speciﬁc end of line sequence.
For example on Windows the end of line sequence is CRLF ("\r\n").
Write: "\n" => "\r\n"
Read: "\r\n" => "\n"
Section 12.5: Reading an ASCII ﬁle into a std::string
std::ifstream f("file.txt");
if (f)
{
  std::stringstream buffer;
  buffer << f.rdbuf();
  f.close();
  // The content of "file.txt" is available in the string `buffer.str()`
}
The rdbuf() method returns a pointer to a streambuf that can be pushed into buffer via the
stringstream::operator<< member function.
Another possibility (popularized in Eﬀective STL by Scott Meyers) is:
std::ifstream f("file.txt");
if (f)
{
  std::string str((std::istreambuf_iterator<char>(f)),
                  std::istreambuf_iterator<char>());
  // Operations on `str`...
}
This is nice because requires little code (and allows reading a ﬁle directly into any STL container, not only strings)
but can be slow for big ﬁles.
NOTE: the extra parentheses around the ﬁrst argument to the string constructor are essential to prevent the most
vexing parse problem.
Last but not least:
std::ifstream f("file.txt");
if (f)
{
  f.seekg(0, std::ios::end);
  const auto size = f.tellg();
  std::string str(size, ' ');
  f.seekg(0);
  f.read(&str[0], size);
  f.close();
  // Operations on `str`...
}
which is probably the fastest option (among the three proposed).
Section 12.6: Writing ﬁles with non-standard locale settings
If you need to write a ﬁle using diﬀerent locale settings to the default, you can use std::locale and
std::basic_ios::imbue() to do that for a speciﬁc ﬁle stream:
Guidance for use:
You should always apply a local to a stream before opening the ﬁle.
Once the stream has been imbued you should not change the locale.
Reasons for Restrictions: Imbuing a ﬁle stream with a locale has undeﬁned behavior if the current locale is not
state independent or not pointing at the beginning of the ﬁle.
UTF-8 streams (and others) are not state independent. Also a ﬁle stream with a UTF-8 locale may try and read the
BOM marker from the ﬁle when it is opened; so just opening the ﬁle may read characters from the ﬁle and it will
not be at the beginning.
#include <iostream>
#include <fstream>
#include <locale>
int main()
{
  std::cout << "User-preferred locale setting is "
            << std::locale("").name().c_str() << std::endl;
  // Write a floating-point value using the user's preferred locale.
  std::ofstream ofs1;
  ofs1.imbue(std::locale(""));
  ofs1.open("file1.txt");
  ofs1 << 78123.456 << std::endl;
  // Use a specific locale (names are system-dependent)
  std::ofstream ofs2;
  ofs2.imbue(std::locale("en_US.UTF-8"));
  ofs2.open("file2.txt");
  ofs2 << 78123.456 << std::endl;
  // Switch to the classic "C" locale
  std::ofstream ofs3;
  ofs3.imbue(std::locale::classic());
  ofs3.open("file3.txt");
  ofs3 << 78123.456 << std::endl;
}
Explicitly switching to the classic "C" locale is useful if your program uses a diﬀerent default locale and you want to
ensure a ﬁxed standard for reading and writing ﬁles. With a "C" preferred locale, the example writes
78,123.456
78,123.456
78123.456
If, for example, the preferred locale is German and hence uses a diﬀerent number format, the example writes
78 123,456
78,123.456
78123.456
(note the decimal comma in the ﬁrst line).
Section 12.7: Checking end of ﬁle inside a loop condition, bad
practice?
eof returns true only after reading the end of ﬁle. It does NOT indicate that the next read will be the end of
stream.
while (!f.eof())
{
  // Everything is OK
  f >> buffer;
  // What if *only* now the eof / fail bit is set?
  /* Use `buffer` */
}
You could correctly write:
while (!f.eof())
{  
  f >> buffer >> std::ws;
  if (f.fail())
    break;
  /* Use `buffer` */
}
but
while (f >> buffer)
{
  /* Use `buffer` */
}
is simpler and less error prone.
Further references:
std::ws: discards leading whitespace from an input stream
std::basic_ios::fail: returns true if an error has occurred on the associated stream
Section 12.8: Flushing a stream
File streams are buﬀered by default, as are many other types of streams. This means that writes to the stream may
not cause the underlying ﬁle to change immediately. In oder to force all buﬀered writes to take place immediately,
you can ﬂush the stream. You can do this either directly by invoking the flush() method or through the std::flush
stream manipulator:
std::ofstream os("foo.txt");
os << "Hello World!" << std::flush;
char data[3] = "Foo";
os.write(data, 3);
os.flush();
There is a stream manipulator std::endl that combines writing a newline with ﬂushing the stream:
// Both following lines do the same thing
os << "Hello World!\n" << std::flush;
os << "Hello world!" << std::endl;
Buﬀering can improve the performance of writing to a stream. Therefore, applications that do a lot of writing
should avoid ﬂushing unnecessarily. Contrary, if I/O is done infrequently, applications should consider ﬂushing
frequently in order to avoid data getting stuck in the stream object.
Section 12.9: Reading a ﬁle into a container
In the example below we use std::string and operator>> to read items from the ﬁle.
    std::ifstream file("file3.txt");
    std::vector<std::string>  v;
    std::string s;
    while(file >> s) // keep reading until we run out
    {
        v.push_back(s);
    }
In the above example we are simply iterating through the ﬁle reading one "item" at a time using operator>>. This
same aﬀect can be achieved using the std::istream_iterator which is an input iterator that reads one "item" at a
time from the stream. Also most containers can be constructed using two iterators so we can simplify the above
code to:
    std::ifstream file("file3.txt");
    std::vector<std::string>  v(std::istream_iterator<std::string>{file},
                                std::istream_iterator<std::string>{});
We can extend this to read any object types we like by simply specifying the object we want to read as the template
parameter to the std::istream_iterator. Thus we can simply extend the above to read lines (rather than words)
like this:
// Unfortunately there is  no built in type that reads line using >>
// So here we build a simple helper class to do it. That will convert
// back to a string when used in string context.
struct Line
{
    // Store data here
    std::string data;
    // Convert object to string
    operator std::string const&() const {return data;}
    // Read a line from a stream.
    friend std::istream& operator>>(std::istream& stream, Line& line)
    {
        return std::getline(stream, line.data);
    }
};
    std::ifstream file("file3.txt");
    // Read the lines of a file into a container.
    std::vector<std::string>  v(std::istream_iterator<Line>{file},
                                std::istream_iterator<Line>{});
Section 12.10: Copying a ﬁle
std::ifstream  src("source_filename", std::ios::binary);
std::ofstream  dst("dest_filename",   std::ios::binary);
dst << src.rdbuf();
Version ≥ C++17
With C++17 the standard way to copy a ﬁle is including the <filesystem> header and using copy_file:
std::fileystem::copy_file("source_filename", "dest_filename");
The ﬁlesystem library was originally developed as boost.filesystem and ﬁnally merged to ISO C++ as of C++17.
Section 12.11: Closing a ﬁle
Explicitly closing a ﬁle is rarely necessary in C++, as a ﬁle stream will automatically close its associated ﬁle in its
destructor. However, you should try to limit the lifetime of a ﬁle stream object, so that it does not keep the ﬁle
handle open longer than necessary. For example, this can be done by putting all ﬁle operations into an own scope
({}):
std::string const prepared_data = prepare_data();
{
    // Open a file for writing.
    std::ofstream output("foo.txt");
    // Write data.
    output << prepared_data;
}  // The ofstream will go out of scope here.
   // Its destructor will take care of closing the file properly.
Calling close() explicitly is only necessary if you want to reuse the same fstream object later, but don't want to
keep the ﬁle open in between:
// Open the file "foo.txt" for the first time.
std::ofstream output("foo.txt");
// Get some data to write from somewhere.
std::string const prepared_data = prepare_data();
// Write data to the file "foo.txt".
output << prepared_data;
// Close the file "foo.txt".
output.close();
// Preparing data might take a long time. Therefore, we don't open the output file stream
// before we actually can write some data to it.
std::string const more_prepared_data = prepare_complex_data();
// Open the file "foo.txt" for the second time once we are ready for writing.
output.open("foo.txt");
// Write the data to the file "foo.txt".
output << more_prepared_data;
// Close the file "foo.txt" once again.
output.close();
Section 12.12: Reading a `struct` from a formatted text ﬁle
Version ≥ C++11
struct info_type
{
    std::string name;
    int age;
    float height;
    // we define an overload of operator>> as a friend function which
    // gives in privileged access to private data members
    friend std::istream& operator>>(std::istream& is, info_type& info)
    {
        // skip whitespace
        is >> std::ws;
        std::getline(is, info.name);
        is >> info.age;
        is >> info.height;
        return is;
    }
};
void func4()
{
    auto file = std::ifstream("file4.txt");
    std::vector<info_type> v;
    for(info_type info; file >> info;) // keep reading until we run out
    {
        // we only get here if the read succeeded
        v.push_back(info);
    }
    for(auto const& info: v)
    {
        std::cout << "  name: " << info.name << '\n';
        std::cout << "   age: " << info.age << " years" << '\n';
        std::cout << "height: " << info.height << "lbs" << '\n';
        std::cout << '\n';
    }
}
ﬁle4.txt
Wogger Wabbit
6.2
Bilbo Baggins
81.3
Mary Poppins
154.8
Output:
name: Wogger Wabbit
 age: 2 years
height: 6.2lbs
name: Bilbo Baggins
 age: 111 years
height: 81.3lbs
name: Mary Poppins
 age: 29 years
height: 154.8lbs
#### 13: C:  Streams
Section 13.1: String streams
std::ostringstream is a class whose objects look like an output stream (that is, you can write to them via
operator<<), but actually store the writing results, and provide them in the form of a stream.
Consider the following short code:
#include <sstream>
#include <string>                                                                                  
using namespace std;
int main()
{
    ostringstream ss;
    ss << "the answer to everything is " << 42;
    const string result = ss.str();
}  
The line
ostringstream ss;
creates such an object. This object is ﬁrst manipulated like a regular stream:
ss << "the answer to everything is " << 42;
Following that, though, the resulting stream can be obtained like this:
const string result = ss.str();
(the string result will be equal to "the answer to everything is 42").
This is mainly useful when we have a class for which stream serialization has been deﬁned, and for which we want a
string form. For example, suppose we have some class
class foo
{  
    // All sort of stuff here.
};  
ostream &operator<<(ostream &os, const foo &f);
To get the string representation of a foo object,
foo f;
we could use
ostringstream ss;
ss << f;
const string result = ss.str();        
Then result contains the string representation of the foo object.
Section 13.2: Printing collections with iostream
Basic printing
std::ostream_iterator allows to print contents of an STL container to any output stream without explicit loops.
The second argument of std::ostream_iterator constructor sets the delimiter. For example, the following code:
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " ! "));
will print
1 ! 2 ! 3 ! 4 !
Implicit type cast
std::ostream_iterator allows to cast container's content type implicitly. For example, let's tune std::cout to print
ﬂoating-point values with 3 digits after decimal point:
std::cout << std::setprecision(3);
std::fixed(std::cout);
and instantiate std::ostream_iterator with float, while the contained values remain int:
std::vector<int> v = {1,2,3,4};
std::copy(v.begin(), v.end(), std::ostream_iterator<float>(std::cout, " ! "));
so the code above yields
1.000 ! 2.000 ! 3.000 ! 4.000 !
despite std::vector holds ints.
Generation and transformation
std::generate, std::generate_n and std::transform functions provide a very powerful tool for on-the-ﬂy data
manipulation. For example, having a vector:
std::vector<int> v = {1,2,3,4,8,16};
we can easily print boolean value of "x is even" statement for each element:
std::boolalpha(std::cout); // print booleans alphabetically
std::transform(v.begin(), v.end(), std::ostream_iterator<bool>(std::cout, " "),
[](int val) {
    return (val % 2) == 0;
});
or print the squared element:
std::transform(v.begin(), v.end(), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});
Printing N space-delimited random numbers:
const int N = 10;
std::generate_n(std::ostream_iterator<int>(std::cout, " "), N, std::rand);
Arrays
As in the section about reading text ﬁles, almost all these considerations may be applied to native arrays. For
example, let's print squared values from a native array:
int v[] = {1,2,3,4,8,16};
std::transform(v, std::end(v), std::ostream_iterator<int>(std::cout, " "),
[](int val) {
    return val * val;
});


---
### Professional Notes: Iterators Deep Dive

#### Chapter 9: Iterators

Section 9.1: Overview
Iterators are Positions
Iterators are a means of navigating and operating on a sequence of elements and are a generalized extension of
pointers. Conceptually it is important to remember that iterators are positions, not elements. For example, take the
following sequence:
A B C
The sequence contains three elements and four positions
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
Elements are things within a sequence. Positions are places where meaningful operations can happen to the
sequence. For example, one inserts into a position, before or after element A, not into an element. Even deletion of
an element (erase(A)) is done by ﬁrst ﬁnding its position, then deleting it.
From Iterators to Values
To convert from a position to a value, an iterator is dereferenced:
auto my_iterator = my_vector.begin(); // position
auto my_value = *my_iterator; // value
One can think of an iterator as dereferencing to the value it refers to in the sequence. This is especially useful in
understanding why you should never dereference the end() iterator in a sequence:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           +-- An iterator here has no value. Do not dereference it!
+-------------- An iterator here dereferences to the value A.
In all the sequences and containers found in the C++ standard library, begin() will return an iterator to the ﬁrst
position, and end() will return an iterator to one past the last position (not the last position!). Consequently, the
names of these iterators in algorithms are oftentimes labelled first and last:
+---+---+---+---+
| A | B | C |   |
+---+---+---+---+
↑           ↑
|           |
+- first    +- last
It is also possible to obtain an iterator to any sequence, because even an empty sequence contains at least one
position:
+---+
|   |
+---+
In an empty sequence, begin() and end() will be the same position, and neither can be dereferenced:
+---+
|   |
+---+
  ↑
  |
  +- empty_sequence.begin()
  |
  +- empty_sequence.end()
The alternative visualization of iterators is that they mark the positions between elements:
+---+---+---+
| A | B | C |
+---+---+---+
↑   ^   ^   ↑
|           |
+- first    +- last
and dereferencing an iterator returns a reference to the element coming after the iterator. Some situations where
this view is particularly useful are:
insert operations will insert elements into the position indicated by the iterator,
erase operations will return an iterator corresponding to the same position as the one passed in,
an iterator and its corresponding reverse iterator are located in the same .position between elements
Invalid Iterators
An iterator becomes invalidated if (say, in the course of an operation) its position is no longer a part of a sequence.
An invalidated iterator cannot be dereferenced until it has been reassigned to a valid position. For example:
std::vector<int>::iterator first;
{
    std::vector<int> foo;
    first = foo.begin(); // first is now valid
} // foo falls out of scope and is destroyed
// At this point first is now invalid
The many algorithms and sequence member functions in the C++ standard library have rules governing when
iterators are invalidated. Each algorithm is diﬀerent in the way they treat (and invalidate) iterators.
Navigating with Iterators
As we know, iterators are for navigating sequences. In order to do that an iterator must migrate its position
throughout the sequence. Iterators can advance forward in the sequence and some can advance backwards:
auto first = my_vector.begin();
++first;                                             // advance the iterator 1 position
std::advance(first, 1);                              // advance the iterator 1 position
first = std::next(first);                            // returns iterator to the next element
std::advance(first, -1);                             // advance the iterator 1 position backwards
first = std::next(first, 20);                        // returns iterator to the element 20 position
forward
first = std::prev(first, 5);                         // returns iterator to the element 5 position
backward
auto dist = std::distance(my_vector.begin(), first); // returns distance between two iterators.
Note, second argument of std::distance should be reachable from the ﬁrst one(or, in other words first should be
less or equal than second).
Even though you can perform arithmetic operators with iterators, not all operations are deﬁned for all types of
iterators. a = b + 3; would work for Random Access Iterators, but wouldn't work for Forward or Bidirectional
Iterators, which still can be advanced by 3 position with something like b = a; ++b; ++b; ++b;. So it is
recommended to use special functions in case you are not sure what is iterator type (for example, in a template
function accepting iterator).
Iterator Concepts
The C++ standard describes several diﬀerent iterator concepts. These are grouped according to how they behave in
the sequences they refer to. If you know the concept an iterator models (behaves like), you can be assured of the
behavior of that iterator regardless of the sequence to which it belongs. They are often described in order from the
most to least restrictive (because the next iterator concept is a step better than its predecessor):
Input Iterators : Can be dereferenced only once per position. Can only advance, and only one position at a
time.
Forward Iterators : An input iterator that can be dereferenced any number of times.
Bidirectional Iterators : A forward iterator that can also advance backwards one position at a time.
Random Access Iterators : A bidirectional iterator that can advance forwards or backwards any number of
positions at a time.
Contiguous Iterators (since C++17) : A random access iterator that guaranties that underlying data is
contiguous in memory.
Algorithms can vary depending on the concept modeled by the iterators they are given. For example, although
random_shuffle can be implemented for forward iterators, a more eﬃcient variant that requires random access
iterators could be provided.
Iterator traits
Iterator traits provide uniform interface to the properties of iterators. They allow you to retrieve value, diﬀerence,
pointer, reference types and also category of iterator:
template<class Iter>
Iter find(Iter first, Iter last, typename std::iterator_traits<Iter>::value_type val)  {
    while (first != last) {
        if (*first == val)
            return first;
        ++first;
    }
    return last;
}
Category of iterator can be used to specialize algorithms:
template<class BidirIt>
void test(BidirIt a, std::bidirectional_iterator_tag)  {
    std::cout << "Bidirectional iterator is used" << std::endl;
}
template<class ForwIt>
void test(ForwIt a, std::forward_iterator_tag)  {
    std::cout << "Forward iterator is used" << std::endl;
}
template<class Iter>
void test(Iter a)  {
    test(a, typename std::iterator_traits<Iter>::iterator_category());
}
Categories of iterators are basically iterators concepts, except Contiguous Iterators don't have their own tag, since it
was found to break code.
Section 9.2: Vector Iterator
begin returns an iterator to the ﬁrst element in the sequence container.
end returns an iterator to the ﬁrst element past the end.
If the vector object is const, both begin and end return a const_iterator. If you want a const_iterator to be
returned even if your vector is not const, you can use cbegin and cend.
Example:
#include <vector>
#include <iostream>
int main() {
    std::vector<int> v = { 1, 2, 3, 4, 5 };  //intialize vector using an initializer_list
    for (std::vector<int>::iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << *it << " ";
    }
    return 0;
}
Output:
1 2 3 4 5
Section 9.3: Map Iterator
An iterator to the ﬁrst element in the container.
If a map object is const-qualiﬁed, the function returns a const_iterator. Otherwise, it returns an iterator.
// Create a map and insert some values
std::map<char,int> mymap;
mymap['b'] = 100;
mymap['a'] = 200;
mymap['c'] = 300;
// Iterate over all tuples
for (std::map<char,int>::iterator it = mymap.begin(); it != mymap.end(); ++it)
    std::cout << it->first << " => " << it->second << '\n';
Output:
a => 200
b => 100
c => 300
Section 9.4: Reverse Iterators
If we want to iterate backwards through a list or vector we can use a reverse_iterator. A reverse iterator is made
from a bidirectional, or random access iterator which it keeps as a member which can be accessed through base().
To iterate backwards use rbegin() and rend() as the iterators for the end of the collection, and the start of the
collection respectively.
For instance, to iterate backwards use:
std::vector<int> v{1, 2, 3, 4, 5};
for (std::vector<int>::reverse_iterator it = v.rbegin(); it != v.rend(); ++it)
{
    cout << *it;
} // prints 54321
A reverse iterator can be converted to a forward iterator via the base() member function. The relationship is that
the reverse iterator references one element past the base() iterator:
std::vector<int>::reverse_iterator r = v.rbegin();
std::vector<int>::iterator i = r.base();
assert(&*r == &*(i-1)); // always true if r, (i-1) are dereferenceable
                        // and are not proxy iterators
 +---+---+---+---+---+---+---+
 |   | 1 | 2 | 3 | 4 | 5 |   |
 +---+---+---+---+---+---+---+
   ↑   ↑               ↑   ↑
   |   |               |   |
rend() |         rbegin()  end()
       |                   rbegin().base()
     begin()
     rend().base()
In the visualization where iterators mark positions between elements, the relationship is simpler:
  +---+---+---+---+---+
| 1 | 2 | 3 | 4 | 5 |
+---+---+---+---+---+
↑                   ↑
|                   |
|                 end()
|                 rbegin()
begin()             rbegin().base()
rend()
rend().base()
Section 9.5: Stream Iterators
Stream iterators are useful when we need to read a sequence or print formatted data from a container:
// Data stream. Any number of various whitespace characters will be OK.
std::istringstream istr("1\t 2     3 4");
std::vector<int> v;
// Constructing stream iterators and copying data from stream into vector.
std::copy(
    // Iterator which will read stream data as integers.
    std::istream_iterator<int>(istr),
    // Default constructor produces end-of-stream iterator.
    std::istream_iterator<int>(),
    std::back_inserter(v));
// Print vector contents.
std::copy(v.begin(), v.end(),
    //Will print values to standard output as integers delimeted by " -- ".
    std::ostream_iterator<int>(std::cout, " -- "));
The example program will print 1 -- 2 -- 3 -- 4 -- to standard output.
Section 9.6: C Iterators (Pointers)
// This creates an array with 5 values.
const int array[] = { 1, 2, 3, 4, 5 };
#ifdef BEFORE_CPP11
// You can use `sizeof` to determine how many elements are in an array.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
// Then you can iterate over the array by incrementing a pointer until
// it reaches past the end of our array.
for (const int* i = first; i < afterLast; ++i) {
    std::cout << *i << std::endl;
}
#else
// With C++11, you can let the STL compute the start and end iterators:
for (auto i = std::begin(array); i != std::end(array); ++i) {
    std::cout << *i << std::endl;
}
#endif
This code would output the numbers 1 through 5, one on each line like this:
Breaking It Down
const int array[] = { 1, 2, 3, 4, 5 };
This line creates a new integer array with 5 values. C arrays are just pointers to memory where each value is stored
together in a contiguous block.
const int* first = array;
const int* afterLast = first + sizeof(array) / sizeof(array[0]);
These lines create two pointers. The ﬁrst pointer is given the value of the array pointer, which is the address of the
ﬁrst element in the array. The sizeof operator when used on a C array returns the size of the array in bytes.
Divided by the size of an element this gives the number of elements in the array. We can use this to ﬁnd the
address of the block after the array.
for (const int* i = first; i < afterLast; ++i) {
Here we create a pointer which we will use as an iterator. It is initialized with the address of the ﬁrst element we
want to iterate over, and we'll continue to iterate as long as i is less than afterLast, which means as long as i is
pointing to an address within array.
    std::cout << *i << std::endl;
Finally, within the loop we can access the value our iterator i is pointing to by dereferencing it. Here the
dereference operator * returns the value at the address in i.
Section 9.7: Write your own generator-backed iterator
A common pattern in other languages is having a function that produces a "stream" of objects, and being able to
use loop-code to loop over it.
We can model this in C++ as
template<class T>
struct generator_iterator {
  using difference_type=std::ptrdiff_t;
  using value_type=T;
  using pointer=T*;
  using reference=T;
  using iterator_category=std::input_iterator_tag;
  std::optional<T> state;
  std::function< std::optional<T>() > operation;
  // we store the current element in "state" if we have one:
  T operator*() const {
    return *state;
  }
  // to advance, we invoke our operation.  If it returns a nullopt
  // we have reached the end:
  generator_iterator& operator++() {
    state = operation();
    return *this;        
  }
  generator_iterator operator++(int) {
    auto r = *this;
    ++(*this);
    return r;
  }
  // generator iterators are only equal if they are both in the "end" state:
  friend bool operator==( generator_iterator const& lhs, generator_iterator const& rhs ) {
    if (!lhs.state && !rhs.state) return true;
    return false;
  }
  friend bool operator!=( generator_iterator const& lhs, generator_iterator const& rhs ) {
    return !(lhs==rhs);
  }
  // We implicitly construct from a std::function with the right signature:
  generator_iterator( std::function< std::optional<T>() > f ):operation(std::move(f))
  {
    if (operation)
      state = operation();
  }
  // default all special member functions:
  generator_iterator( generator_iterator && ) =default;
  generator_iterator( generator_iterator const& ) =default;
  generator_iterator& operator=( generator_iterator && ) =default;
  generator_iterator& operator=( generator_iterator const& ) =default;
  generator_iterator() =default;
};
live example.
We store the generated element early so we can more easily detect if we are already at the end.
As the function of an end generator iterator is never used, we can create a range of generator iterators by only
copying the std::function once. A default constructed generator iterator compares equal to itself, and to all other
end-generator-iterators.

## <a name="chapter-6-stlinternalsdeepdive"></a>CHAPTER 6: STL INTERNALS DEEP DIVE

To master the STL, you must understand what happens under the hood.

### 3.5.1 The Truth About std::vector
`std::vector` is a dynamic array. It guarantees contiguous memory.

*   **Layout**: Three pointers: `start`, `finish`, `end_of_storage`.
    *   `start`: Points to first element.
    *   `finish`: Points to one-past-the-last active element (size).
    *   `end_of_storage`: Points to end of allocated buffer (capacity).

*   **Growth Strategy**: Geometric growth.
    *   When `size() == capacity()`, a new buffer is allocated (usually 2x or 1.5x larger).
    *   **Elements are MOVED** (or copied) to the new buffer.
    *   Old buffer is deleted.
    *   *Cost*: Amortized O(1) push_back, but worst-case O(N).

*   **Iterator Invalidation**:
    *   **Reallocation**: Invalidates ALL iterators, pointers, and references.
    *   **Insertion/Erasure**: Invalidates iterators at and after the point of operation.

### 3.5.2 The std::deque Implementation
`std::deque` (Double-Ended Queue) is NOT a contiguous array.

*   **Layout**: A "Map" (dynamic array) of pointers to fixed-size "Chunks" (blocks).
    *   Iterators are smart pointers that know how to jump between chunks.
*   **Performance**:
    *   O(1) random access (double dereference).
    *   O(1) push/pop at BOTH ends (no full reallocation needed, just add a new chunk).
*   **Cache Locality**: Worse than vector, better than list.

### 3.5.3 Why std::list is (Almost) Always Wrong
`std::list` is a Doubly Linked List.

*   **Layout**: Nodes allocated individually on the heap.
    *   `struct Node { T val; Node* prev; Node* next; }`
*   **The Cache Problem**: Nodes are scattered in memory. Traversing a list causes constant **Cache Misses**.
*   **Benchmark**: Iterating a `vector` is orders of magnitude faster than a `list`, even for large types, due to prefetching.
*   **Use Case**: Only when you need **Reference Stability** (insertions never invalidate references to other elements).

### 3.5.4 Associative Containers (Map/Set)
`std::map`, `std::set`, `std::multimap`, `std::multiset`.

*   **Implementation**: Balanced Binary Search Tree (usually **Red-Black Tree**).
*   **Node Layout**: `struct Node { T val; Node* left; Node* right; Node* parent; Color color; }`
*   **Complexity**: O(log N) for insert, lookup, delete.
*   **Overhead**: 3 pointers + enum per element (heavy memory overhead).

### 3.5.5 Unordered Containers (Hash Maps)
`std::unordered_map`, `std::unordered_set`.

*   **Implementation**: Array of "Buckets" (Linked Lists).
    *   Hash function maps Key -> Bucket Index.
    *   Collisions handled by Chaining (linked list in bucket).
*   **Complexity**:
    *   Average: O(1).
    *   Worst Case: O(N) (if all keys hash to same bucket).
*   **Rehashing**: When `load_factor > max_load_factor`, bucket array grows, all elements rehashed.

### 3.5.6 Iterator Invalidation Cheat Sheet

| Container | Operation | Invalidates |
| :--- | :--- | :--- |
| **Vector** | Capacity Change | **ALL** |
| **Vector** | Insert/Erase | Current & After |
| **Deque** | Insert/Erase (ends) | Iterators only (Refs valid!) |
| **Deque** | Insert/Erase (middle) | **ALL** |
| **List** | Insert/Erase | Only deleted element |
| **Map/Set** | Insert/Erase | Only deleted element |
| **Unordered** | Rehash | **ALL** |
| **Unordered** | Insert (no rehash) | None |

---

# Volume II: The Modern Renaissance
## <a name="chapter-7-c11"></a>CHAPTER 7: C++11 CORE LANGUAGE FEATURES

The C++11 standard (originally known as C++0x) was a revolutionary update that transformed C++ into a modern language. It addressed verbosity, safety, and performance.

---

## 1. AUTO & TYPE DEDUCTION

### 1.1 The `auto` Keyword

In C++98, you had to explicitly declare types, which could be verbose, especially with iterators. C++11 introduced `auto` to ask the compiler to deduce the type from the initializer.

```cpp
// C++98
int x = 5;
std::vector<int> v;
std::vector<int>::iterator it = v.begin();

// C++11
auto x = 5;       // deduced as int
auto it = v.begin(); // deduced as std::vector<int>::iterator
```

**Key Rules:**
1.  **Must be initialized**: `auto x;` is invalid.
2.  **References**: `auto` strips references. Use `auto&` to keep them.
3.  **Const**: `auto` strips top-level const. Use `const auto` or `const auto&`.

```cpp
int a = 10;
int& ref = a;

auto x = ref;  // x is int (copy), not int&
auto& y = ref; // y is int& (reference)
```

### 1.2 `decltype`

`decltype` inspects the declared type of an entity or expression. Unlike `auto`, it does not strip references or const.

```cpp
int x = 5;
decltype(x) y = 10; // y is int

const int& z = x;
decltype(z) w = x; // w is const int&
```

---

## 2. RANGE-BASED FOR LOOPS

C++11 introduced a syntactic sugar for iterating over containers, arrays, and initializer lists.

```cpp
std::vector<int> vec = {1, 2, 3, 4, 5};

// C++98
for (std::vector<int>::iterator it = vec.begin(); it != vec.end(); ++it) {
    std::cout << *it << " ";
}

// C++11 Range-based for
for (int i : vec) {
    std::cout << i << " ";
}

// With auto (Common pattern)
for (auto i : vec) {
    std::cout << i << " ";
}

// By Reference (to modify elements)
for (auto& i : vec) {
    i *= 2;
}

// By Const Reference (read-only, avoids copy)
for (const auto& i : vec) {
    std::cout << i << " ";
}
```

---

## 3. UNIFORM INITIALIZATION

C++11 introduced a consistent syntax for initializing everything using curly braces `{}`.

### 3.1 Brace Initialization

```cpp
// C++98 inconsistency
int a = 5;
int arr[] = {1, 2};
std::vector<int> v; v.push_back(1); // Tedious

// C++11 Uniformity
int a{5};
int arr[]{1, 2};
std::vector<int> v{1, 2, 3}; // Initializer list!
std::string s{"Hello"};
```

### 3.2 Preventing Narrowing

Brace initialization forbids "narrowing conversions" where data loss might occur.

```cpp
int x = 3.14;  // C++98: Compiles (x becomes 3), implicit cast
// int y{3.14}; // C++11: Error! Narrowing conversion
```

### 3.3 `std::initializer_list`

Classes can now take a list of elements in their constructor.

```cpp
#include <initializer_list>

class MyClass {
public:
    MyClass(std::initializer_list<int> list) {
        for (auto elem : list) {
            // process elem
        }
    }
};

MyClass obj = {1, 2, 3, 4, 5};
```

---

## 4. NULLPTR

C++98 used `0` or `NULL` (macro for 0) for null pointers. This caused ambiguity with function overloading.

```cpp
void func(int x) { std::cout << "Integer"; }
void func(char* p) { std::cout << "Pointer"; }

// C++98
func(NULL); // Calls func(int)! Because NULL is 0.

// C++11
func(nullptr); // Calls func(char*).
```

`nullptr` is a keyword of type `std::nullptr_t`. It implicitly converts to any pointer type but *not* to integers (except bool `false`).

---

## 5. STRONGLY TYPED ENUMS

C++98 enums leaked their names into the surrounding scope and implicitly converted to integers. C++11 `enum class` fixes this.

```cpp
// C++98
enum Color { RED, GREEN, BLUE };
int r = RED; // Implicit conversion allowed

// C++11
enum class TrafficLight { RED, YELLOW, GREEN };

// TrafficLight t = RED; // Error: RED not in scope
TrafficLight t = TrafficLight::RED;
// int x = TrafficLight::RED; // Error: No implicit conversion

// Explicit underlying type
enum class Byte : unsigned char { A, B, C };
```

---

## 6. OTHER CORE FEATURES

-   **`constexpr`**: Allows functions and variables to be evaluated at compile-time (limited in C++11, expanded later).
-   **`static_assert`**: Compile-time assertion checking.
    ```cpp
    static_assert(sizeof(int) == 4, "Int must be 4 bytes");
    ```
-   **Delegating Constructors**: One constructor can call another.
-   **`default` and `delete` functions**:
    ```cpp
    class NonCopyable {
        NonCopyable(const NonCopyable&) = delete; // Ban copying
        NonCopyable() = default; // Explicitly request default ctor
    };
    ```
-   **`override` and `final`**: Virtual function controls (See Chapter 19).
## <a name="chapter-8-c11"></a>CHAPTER 8: C++11 SMART POINTERS & MEMORY MANAGEMENT

C++11 revolutionized memory management by introducing smart pointers, which strictly define ownership semantics and automate memory reclamation, effectively making `new` and `delete` unnecessary in user code.

---

## 1. THE PROBLEM WITH RAW POINTERS

In C++98, dynamic memory required manual management:
1.  **Memory Leaks**: Forgetting `delete`.
2.  **Dangling Pointers**: Accessing deleted memory.
3.  **Double Free**: Deleting the same memory twice.
4.  **Exception Safety**: If an exception throws before `delete`, memory leaks.

Smart pointers solve these by using **RAII (Resource Acquisition Is Initialization)**.

---

## 2. UNIQUE_PTR (Exclusive Ownership)

`std::unique_ptr` represents exclusive ownership. An object can have only one `unique_ptr` pointing to it. When the `unique_ptr` is destroyed, the object is deleted.

### 2.1 Basic Usage

```cpp
#include <memory>

void func() {
    std::unique_ptr<int> ptr(new int(10));
    // or better: auto ptr = std::make_unique<int>(10); (C++14)
    
    *ptr = 20;
    // No delete needed. Memory freed when ptr goes out of scope.
}
```

### 2.2 Move Only

You cannot copy a `unique_ptr`. You must **move** it. This ensures uniqueness.

```cpp
std::unique_ptr<int> p1(new int(5));
// std::unique_ptr<int> p2 = p1; // Error! Copy deleted.

std::unique_ptr<int> p2 = std::move(p1); // OK. p1 is now empty/null.
```

### 2.3 Custom Deleters

Useful for managing C-style resources (files, sockets).

```cpp
auto deleter = [](FILE* f) { fclose(f); };
std::unique_ptr<FILE, decltype(deleter)> file(fopen("test.txt", "r"), deleter);
```

---

## 3. SHARED_PTR (Shared Ownership)

`std::shared_ptr` allows multiple pointers to own the same resource. The resource is deleted only when the *last* `shared_ptr` is destroyed.

### 3.1 Reference Counting

It maintains a "control block" with a reference count.

```cpp
auto p1 = std::make_shared<int>(100); // Ref count = 1
{
    auto p2 = p1; // Copy allowed. Ref count = 2
} // p2 destroyed. Ref count = 1

// p1 destroyed. Ref count = 0. Memory freed.
```

### 3.2 Performance Cost

`shared_ptr` is heavier than `unique_ptr` (2x size usually, plus atomic ref-count increment/decrement overhead). Use only when ownership is truly shared.

---

## 4. WEAK_PTR (Non-Owning Reference)

`std::weak_ptr` observes a `shared_ptr` without keeping it alive. It breaks **circular references**.

### 4.1 Circular Reference Problem

If A has a `shared_ptr` to B, and B has a `shared_ptr` to A, the reference count never drops to zero.

### 4.2 Using weak_ptr

```cpp
struct B;
struct A {
    std::shared_ptr<B> b_ptr;
};
struct B {
    std::weak_ptr<A> a_ptr; // Use weak_ptr back to A
};
```

To use a `weak_ptr`, you must convert it to `shared_ptr` via `.lock()`.

```cpp
if (auto shared = weak.lock()) {
    // safe to use shared
} else {
    // object died
}
```

---

## 5. BEST PRACTICES

1.  **Prefer `unique_ptr`** by default. It has zero overhead.
2.  **Use `make_unique`** (C++14) and **`make_shared`**. They are cleaner and exception-safe. `make_shared` is also more efficient (allocates object and control block in one chunk).
3.  **Avoid `new` and `delete`**.
## <a name="chapter-9-c11"></a>CHAPTER 9: C++11 MOVE SEMANTICS

Move semantics is arguably the most significant performance feature in C++11. It allows resources to be "transferred" (moved) from temporary objects rather than copied.

---

## 1. LVALUES AND RVALUES

### 1.1 Lvalues
An **lvalue** (locator value) represents an object that occupies an identifiable location in memory (has an address).
- Example: `int x = 5;` (`x` is lvalue).
- You can take its address: `&x`.

### 1.2 Rvalues
An **rvalue** is everything else: temporary values, literals, or results of expressions.
- Example: `5`, `x + 2`, `funcReturningVal()`.
- You cannot take its address.

---

## 2. RVALUE REFERENCES

C++11 introduced the rvalue reference: `T&&`. It binds *only* to rvalues.

```cpp
int x = 10;
int& lref = x;      // Lvalue ref binds to lvalue
// int&& rref = x;  // Error: cannot bind rvalue ref to lvalue

int&& rref2 = 20;   // OK: 20 is rvalue
```

---

## 3. MOVE CONSTRUCTOR & ASSIGNMENT

This allows a class to steal resources from a temporary object instead of making a deep copy.

### 3.1 Deep Copy (The Old Way)

```cpp
class Vector {
    int* data;
    size_t size;
public:
    // Copy Constructor
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
    }
};
```

### 3.2 Move Constructor (The C++11 Way)

```cpp
    // Move Constructor
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        // Steal the pointer
        other.data = nullptr; // Null out source
        other.size = 0;
    }
```

If `other` is a temporary, the compiler selects the Move Constructor. This is O(1) instead of O(N).

---

## 4. STD::MOVE

`std::move(x)` does exactly one thing: it casts `x` to an rvalue reference (`T&&`). It essentially says, "I am done with this object, you can steal from it."

```cpp
Vector v1(100);
Vector v2 = std::move(v1); // Calls Move Constructor
// v1 is now empty (if implemented correctly)
```

---

## 5. PERFECT FORWARDING

Used in templates to preserve the value category (lvalue vs rvalue) of arguments.

### 5.1 Universal References (Forwarding References)

If `T` is a template parameter, `T&&` is a **universal reference**, not just an rvalue reference. It can bind to anything.

### 5.2 std::forward

```cpp
template<typename T>
void wrapper(T&& arg) {
    func(std::forward<T>(arg));
}
```

- If `wrapper` is called with lvalue, `arg` is lvalue, `forward` keeps it lvalue.
- If `wrapper` is called with rvalue, `arg` is lvalue (as a named variable), but `forward` casts it back to rvalue.

This enables `emplace_back` to work efficiently.


## <a name="chapter-10-c11"></a>CHAPTER 10: C++11 FUNCTIONAL PROGRAMMING

C++11 brought functional programming paradigms to the language, centered around Lambdas.

---

## 1. LAMBDA EXPRESSIONS

Lambdas are anonymous function objects.

### 1.1 Syntax

`[ captures ] ( params ) -> ret { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int sum = add(1, 2);
```

### 1.2 Captures

- `[]`: No capture.
- `[=]`: Capture everything by value (copy).
- `[&]`: Capture everything by reference.
- `[x]`: Capture x by value.
- `[&x]`: Capture x by reference.

```cpp
int factor = 10;
auto multiply = [factor](int n) { return n * factor; }; // factor captured by value
```

### 1.3 Mutable Lambdas

By default, value captures are `const`. Use `mutable` to modify them.

```cpp
int x = 0;
auto increment = [x]() mutable { return ++x; }; // x is internal state
```

---

## 2. STD::FUNCTION

`std::function` is a polymorphic wrapper for *any* callable (function pointer, lambda, functor, bind result).

```cpp
#include <functional>

void print(int i) { std::cout << i; }

std::function<void(int)> func;
func = print;
func = [](int i) { std::cout << i * 2; };
```

It has runtime overhead (virtual function call, possible allocation). Prefer templates or auto if possible.

---

## 3. STD::BIND

`std::bind` performs partial application of functions.

```cpp
#include <functional>
using namespace std::placeholders;

int sub(int a, int b) { return a - b; }

// Bind second argument to 5
auto sub5 = std::bind(sub, _1, 5); 
// sub5(10) calls sub(10, 5) -> 5
```

**Note:** Lambdas mostly replaced `std::bind` in modern C++ because they are clearer and faster (compiler optimization).
## <a name="chapter-11-c11"></a>CHAPTER 11: C++11 CONCURRENCY

Before C++11, multithreading was platform-specific (pthreads, Windows threads). C++11 added a standard memory model and threading library.

---

## 1. THREADS

`std::thread` represents a single thread of execution.

```cpp
#include <thread>
#include <iostream>

void task(int id) {
    std::cout << "Thread " << id << " running\n";
}

int main() {
    std::thread t1(task, 1);
    std::thread t2(task, 2);

    // Must join or detach before destructor
    t1.join(); // Wait for finish
    t2.join();
    return 0;
}
```

---

## 2. MUTEX AND LOCKS

Protect shared data with `std::mutex`.

```cpp
#include <mutex>

std::mutex mtx;
int count = 0;

void safe_increment() {
    // RAII Lock: locks on construction, unlocks on destruction
    std::lock_guard<std::mutex> lock(mtx);
    count++;
}
```

---

## 3. ATOMICS

`std::atomic<T>` provides lock-free thread safety for simple types.

```cpp
#include <atomic>

std::atomic<int> counter(0);

void fast_increment() {
    counter++; // Atomic increment (hardware supported)
}
```

This avoids the overhead of mutexes for simple counters and flags.

---

## 4. ASYNC AND FUTURE

`std::async` runs a function asynchronously and returns a `std::future` that holds the result.

```cpp
#include <future>

int calculate() { return 42; }

int main() {
    // Launch async task
    std::future<int> result = std::async(std::launch::async, calculate);
    
    // Do other work...    
    // Get result (blocks if not ready)
    std::cout << result.get(); 
}
```

---

## 5. CONDITION VARIABLES

Used for thread synchronization (waiting for an event).

```cpp
std::condition_variable cv;
std::mutex cv_m;
bool ready = false;

void worker() {
    std::unique_lock<std::mutex> lk(cv_m);
    cv.wait(lk, []{ return ready; }); // Wait until ready is true
    // process...
}

void signal() {
    {
        std::lock_guard<std::mutex> lk(cv_m);
        ready = true;
    }
    cv.notify_one();
}
```
## <a name="chapter-12-c11"></a>CHAPTER 12: C++11 STANDARD LIBRARY ADDITIONS

C++11 massively expanded the STL.

---

## 1. UNORDERED CONTAINERS (HASH MAPS)

C++98 `std::map` is a tree (O(log n)). C++11 added hash tables (O(1) average).

- `std::unordered_map`
- `std::unordered_set`
- `std::unordered_multimap`
- `std::unordered_multiset`

```cpp
#include <unordered_map>
std::unordered_map<std::string, int> scores;
scores["Alice"] = 100; // O(1)
```

They require a Hash function for the key type.

---

## 2. STD::ARRAY

`std::array` is a fixed-size wrapper around C-style arrays. It doesn't decay to a pointer automatically and knows its size.

```cpp
#include <array>
std::array<int, 5> arr = {1, 2, 3, 4, 5};
// arr.size() is 5
```

Prefer over C-arrays (`int arr[5]`).

---

## 3. STD::TUPLE

Generalization of `std::pair` for N elements.

```cpp
#include <tuple>
auto t = std::make_tuple(10, "Hello", 3.14);
int i = std::get<0>(t);
```

---

## 4. REGULAR EXPRESSIONS

`std::regex` provides regex matching and replacement.

```cpp
#include <regex>
std::regex pattern(R"(\d+)"); // Matches digits
bool match = std::regex_match("123", pattern);
```

---

## 5. CHRONO (TIME)

Type-safe time library.

```cpp
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
// ... work ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

---

## 6. RANDOM

Better random number generation than `rand()`.

```cpp
#include <random>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 6); // Dice roll
int roll = dis(gen);
```


## <a name="chapter-13-c11"></a>CHAPTER 13: C++11 METAPROGRAMMING

C++11 made Template Metaprogramming (TMP) usable by mere mortals.

---

## 1. VARIADIC TEMPLATES

Templates that accept an arbitrary number of arguments.

```cpp
template<typename T>
T sum(T t) { return t; }

template<typename T, typename... Args>
T sum(T t, Args... args) {
    return t + sum(args...);
}

// sum(1, 2, 3, 4) -> 10
```

---

## 2. TYPE TRAITS

The `<type_traits>` header allows compile-time inspection of types.

```cpp
#include <type_traits>

static_assert(std::is_integral<int>::value, "Int must be integral");
static_assert(std::is_pointer<int*>::value, "Must be pointer");
```

Used heavily with **SFINAE** (`std::enable_if`) to restrict templates.

---

## 3. CONSTEXPR (INTRODUCTION)

`constexpr` functions can be evaluated at compile-time.

```cpp
constexpr int square(int x) { return x * x; }

int array[square(5)]; // Valid! Size 25 at compile time.
```

In C++11, `constexpr` functions were very limited (single return statement). C++14 relaxed this.


## <a name="chapter-9-c14enhancements"></a>CHAPTER 9: C++14 ENHANCEMENTS

## C++14 Overview & Philosophy

C++14 (finalized in 2014) is a **refinement and maintenance release** of C++11.

### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement + useful features)
- **2017**: C++17 released (significant improvements)
- **2020**: C++20 released (revolutionary again)

### C++14 Philosophy
- **Smaller, focused** improvements rather than revolution
- **Fix** C++11 issues and limitations
- **Enhance** usability and convenience
- **Add** frequently-requested features
- **Simplify** compile-time computation

### Key Features
1. Generic lambdas with `auto` parameters
2. Return type deduction for all functions
3. Binary literals and digit separators
4. std::make_unique
5. Relaxed constexpr rules
6. Variable templates
7. Library improvements

### Why C++14 Matters
While smaller than C++11, C++14 makes C++11 more practical:
- ✅ Fixes usability issues
- ✅ Adds convenient features
- ✅ Improves compile-time computation
- ✅ Better template support
- ✅ More standard library features

---

# SECTION 1: GENERIC LAMBDAS

## 1.1 Auto Parameters in Lambdas

C++14 allows `auto` as lambda parameters, creating **generic lambdas**.

### Basic Generic Lambda

```cpp
#include <iostream>
#include <vector>
using namespace std;

// C++11: Type-specific lambda
auto add11 = [](int a, int b) { return a + b; };
cout << add11(5, 3) << "\n";           // 8
// cout << add11(2.5, 3.5) << "\n";   // ERROR - int only

// C++14: Generic lambda with auto
auto add14 = [](auto a, auto b) { return a + b; };
cout << add14(5, 3) << "\n";           // 8 (int)
cout << add14(2.5, 3.5) << "\n";       // 6 (double)
cout << add14(string("Hello"), string(" World")) << "\n";  // "Hello World"
```

### Generic Lambda Deduction

```cpp
// Each auto parameter is independently deduced
auto process = [](auto x, auto y) {
    // x and y can be different types
    cout << x << ", " << y << "\n";
};

process(5, 3.14);              // int, double
process("hello", 42);          // const char*, int
process(3.14, "world");        // double, const char*
```

### Generic Lambdas with std::vector

```cpp
vector<int> vi = {1, 2, 3};
vector<double> vd = {1.1, 2.2, 3.3};
vector<string> vs = {"a", "b", "c"};

// Single generic lambda works with all containers
auto print = [](auto val) {
    cout << val << " ";
};

for_each(vi.begin(), vi.end(), print);
cout << "\n";

for_each(vd.begin(), vd.end(), print);
cout << "\n";

for_each(vs.begin(), vs.end(), print);
cout << "\n";
```

### Generic Lambdas with Algorithms

```cpp
// Works with any type supporting operator*
auto square = [](auto x) { return x * x; };

vector<int> vi = {1, 2, 3};
vector<double> vd = {1.5, 2.5, 3.5};

transform(vi.begin(), vi.end(), vi.begin(), square);
// vi: {1, 4, 9}

transform(vd.begin(), vd.end(), vd.begin(), square);
// vd: {2.25, 6.25, 12.25}
```

### Generic Lambda Compile-Time Behavior

```cpp
// Type checking still happens at compile time
auto multiply = [](auto a, auto b) { return a * b; };

cout << multiply(5, 3) << "\n";        // 15 (int)
cout << multiply(2.5, 3.0) << "\n";    // 7.5 (double)

// This would compile-time error if * not defined:
// multiply("a", "b");                // ERROR - string doesn't support *
```

### When to Use Generic Lambdas

```cpp
// Good use case: Works with any comparable type
auto find_min = [](auto a, auto b) { return a < b ? a : b; };

int min_int = find_min(5, 3);          // 3
double min_double = find_min(2.5, 1.5);  // 1.5
string min_str = find_min("cat", "apple");  // "apple"

// Bad use case: Type-specific logic
auto process = [](auto x) {
    if (is_integral_v<decltype(x)>) {
        cout << "Integer\n";
    } else if (is_floating_point_v<decltype(x)>) {
        cout << "Float\n";
    }
    // Too complex - use template or function overloads instead
};
```

---

# SECTION 2: RETURN TYPE DEDUCTION FOR ALL FUNCTIONS

## 2.1 Return Type Deduction (C++14 Enhancement)

C++11 allowed return type deduction with `-> auto`, but C++14 simplifies it.

### Basic Return Type Deduction

```cpp
// C++11: Must use trailing return type
auto add_11(int a, int b) -> int { return a + b; }
auto divide_11(double a, double b) -> double { return a / b; }

// C++14: Can deduce from return statement
auto add_14(int a, int b) { return a + b; }      // Returns int
auto divide_14(double a, double b) { return a / b; }  // Returns double

auto get_string() { return string("hello"); }    // Returns string
auto get_vector() { return vector<int>{1, 2, 3}; }  // Returns vector<int>
```

### Multiple Return Statements

```cpp
// C++14: All returns must be consistent type
auto absolute(int x) {
    if (x >= 0) {
        return x;          // int
    } else {
        return -x;         // Must also be int
    }
}

// ERROR: Different return types
// auto mixed(int x) {
//     if (x > 0) {
//         return x;      // int
//     } else {
//         return 3.14;   // double - ERROR!
//     }
// }
```

### Return Type Deduction with Complex Types

```cpp
#include <vector>
using namespace std;

// Deduce vector
auto get_data() {
    return vector<int>{1, 2, 3, 4, 5};
}

// Deduce map
auto get_map() {
    return map<string, int>{{"a", 1}, {"b", 2}};
}

// Deduce function
auto get_comparator() {
    return [](int a, int b) { return a > b; };
}

// Works seamlessly
vector<int> v = get_data();
map<string, int> m = get_map();
auto cmp = get_comparator();
```

### Return Type Deduction in Templates

```cpp
template<typename T, typename U>
auto add(T a, U b) {
    return a + b;  // Type deduced from a + b
}

cout << add(5, 3) << "\n";              // int
cout << add(2.5, 3.0) << "\n";          // double
cout << add(5, 3.14) << "\n";           // double (int + double = double)

// Return type varies by input types
static_assert(is_same_v<decltype(add(5, 3)), int>);
static_assert(is_same_v<decltype(add(5.0, 3)), double>);
```

### Recursion with Return Type Deduction

```cpp
// C++14: Recursive functions can use auto return type
// (But compiler may need hints for some cases)

auto factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

cout << factorial(5) << "\n";  // 120

// More complex recursion
auto fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

cout << fibonacci(10) << "\n";  // 55
```

### Benefits of Return Type Deduction

```cpp
// Less redundant code
// Before: Must specify return type
int add_old(int a, int b) -> int { return a + b; }

// After: Auto-deduced
auto add_new(int a, int b) { return a + b; }

// Refactoring friendly - type changes automatically
auto get_value() { return 42; }        // int
// Later: auto get_value() { return 3.14; }  // Changes to double - easy!
```

---

# SECTION 3: AUTO FOR VARIABLES IN LAMBDAS

## 3.1 Init Capture with Auto (C++14)

Lambda capture allows variables to be initialized inside the capture list.

### Basic Init Capture

```cpp
#include <memory>
#include <iostream>
using namespace std;

// Create a unique_ptr (move-only type)
auto ptr = make_unique<int>(42);

// C++11: Can't capture unique_ptr (can't copy)
// auto lambda11 = [ptr]() { };  // ERROR - can't copy unique_ptr

// C++14: Init capture allows move
auto lambda = [ptr = move(ptr)]() {
    cout << *ptr << "\n";
};

lambda();  // 42
// ptr is now nullptr (moved into lambda)
```

### Init Capture with Values

```cpp
int original = 10;

// Capture with transformation
auto lambda = [copy = original * 2]() {
    return copy;  // 20
};

cout << lambda() << "\n";  // 20
cout << original << "\n";  // Still 10 (original unchanged)

// Useful for expensive copies
vector<int> large_vector = {1, 2, 3, 4, 5};
auto process = [copy = vector<int>(large_vector)]() {
    // Use copy (independent of large_vector)
};
```

### Init Capture with Move

```cpp
class Resource {
public:
    Resource() { cout << "Created\n"; }
    ~Resource() { cout << "Destroyed\n"; }
    void use() { cout << "Using\n"; }
};

auto res = make_unique<Resource>();

// Move resource into lambda
auto lambda = [res = move(res)]() {
    if (res) {
        res->use();
    }
};

lambda();  // Using, Destroyed
// res is now nullptr
```

### Init Capture with Complex Types

```cpp
#include <map>

map<string, int> data{{"a", 1}, {"b", 2}};

// Copy map into lambda
auto process = [data_copy = data]() {
    for (const auto& [k, v] : data_copy) {
        cout << k << ": " << v << "\n";
    }
};

// Modify copy without affecting original
auto modify = [data_copy = move(data)]() {
    data_copy["c"] = 3;
    // data is moved
};

modify();
```

### Init Capture Patterns

```cpp
// Pattern 1: Capture with computation
auto compute = [val = 2 + 3]() { return val; };  // val = 5

// Pattern 2: Capture with function call
auto get_timestamp = [time = chrono::high_resolution_clock::now()]() {
    return time;
};

// Pattern 3: Capture with complex initialization
auto setup = [config = []() {
    map<string, string> m;
    m["key"] = "value";
    return m;
}()]() {
    // config is initialized with map
};

// Pattern 4: Move-only types
auto factory = [ptr = make_unique<int>(42)]() {
    return *ptr;
};
```

---

# SECTION 4: BINARY LITERALS & DIGIT SEPARATORS

## 4.1 Binary Literals

C++14 introduces `0b` prefix for binary literals.

### Binary Literal Syntax

```cpp
#include <iostream>
using namespace std;

// Decimal (C++98)
int dec = 42;

// Hexadecimal (C++98)
int hex = 0x2A;

// Octal (C++98)
int oct = 052;

// Binary (C++14)
int bin = 0b101010;

cout << dec << "\n";  // 42
cout << hex << "\n";  // 42
cout << oct << "\n";  // 42
cout << bin << "\n";  // 42
```

### Binary Literals Use Cases

```cpp
// Bitwise operations are clearer with binary
unsigned char flags = 0b11010110;
unsigned char mask = 0b00001111;

unsigned char result = flags & mask;  // Much clearer than 0xD6 & 0x0F

// Single bit operations
unsigned int option1 = 0b00000001;
unsigned int option2 = 0b00000010;
unsigned int option3 = 0b00000100;

unsigned int enabled = option1 | option3;  // 0b00000101

// Permission bits
unsigned char read = 0b100;    // 4
unsigned char write = 0b010;   // 2
unsigned char execute = 0b001; // 1

unsigned char permissions = read | write;
```

## 4.2 Digit Separators

C++14 allows single quotes `'` as digit separators for readability.

### Digit Separator Examples

```cpp
// Large numbers are clearer with separators
long large = 1'000'000'000;      // One billion
double pi = 3.141'592'653;       // Pi

// Binary with separators (very clear)
unsigned char bits = 0b1111'0000;
unsigned short value = 0xDEAD'BEEF;

// All numeric literals support separators
int decimal = 123'456'789;
long long big = 9'223'372'036'854'775'807;  // Max int64

double d = 1'000.123'456;                   // Works with decimals
double e = 1.234'567e3;                     // Works with exponents
```

### Readability Improvement

```cpp
// Before (hard to count zeros)
unsigned int ip = 192168001001;  // What is this?

// After (clear structure)
unsigned int ip = 192'168'001'001;  // IP address: 192.168.1.1

// Before (hard to verify)
long big = 9223372036854775807;

// After (easy to verify)
long big = 9'223'372'036'854'775'807;  // Max int64

// Before (unclear magnitude)
double money = 1000000000;

// After (clear)
double money = 1'000'000'000;  // One billion
```

### Digit Separator Rules

```cpp
// Valid usage
int a = 1'000'000;
int b = 0xDEAD'BEEF;
int c = 0b1111'0000;

// NOT at start or end
// int bad1 = '123;          // ERROR
// int bad2 = 123';          // ERROR

// NOT adjacent to decimal point or exponent
// double bad3 = 1.'5;       // ERROR
// double bad4 = 1e'10;      // ERROR

// Multiple separators are OK
int d = 1'000'000'000;
int e = 0xFF'FF'FF'FF;
```

---

# SECTION 5: STD::MAKE_UNIQUE

## 5.1 std::make_unique (C++14)

`std::make_unique` creates `unique_ptr` safely and efficiently.

### Before C++14

```cpp
#include <memory>
using namespace std;

// C++11: Two-step process
unique_ptr<int> ptr1(new int(42));
unique_ptr<string> ptr2(new string("hello"));
unique_ptr<vector<int>> ptr3(new vector<int>{1, 2, 3});

// Problem: New and unique_ptr are separate
// If exception between new and unique_ptr, memory leaks
```

### With std::make_unique

```cpp
#include <memory>
using namespace std;

// C++14: One-step, exception-safe
auto ptr1 = make_unique<int>(42);
auto ptr2 = make_unique<string>("hello");
auto ptr3 = make_unique<vector<int>>(initializer_list<int>{1, 2, 3});

// Automatically determines type
// Exception-safe: if constructor throws, no memory leak
```

### make_unique with Classes

```cpp
class Person {
public:
    string name;
    int age;
    
    Person(string n, int a) : name(n), age(a) {
        cout << "Person created\n";
    }
    ~Person() {
        cout << "Person destroyed\n";
    }
};

// Create unique_ptr with constructor arguments
auto person = make_unique<Person>("Alice", 30);
cout << person->name << " is " << person->age << "\n";

// Automatic cleanup when going out of scope
```

### make_unique with Arrays (C++20)

```cpp
// C++14: Dynamic sized arrays need manual approach
unique_ptr<int[]> arr1(new int[10]);

// C++20: make_unique supports arrays
// auto arr2 = make_unique<int[]>(10);  // C++20 only

// For C++14, use the manual approach:
auto arr3 = make_unique<int[]>();  // C++20
```

### make_unique vs new

```cpp
// Old way (manual, error-prone)
function<unique_ptr<int>()> factory_old = []() {
    return unique_ptr<int>(new int(42));
};

// New way (cleaner, safer)
function<unique_ptr<int>()> factory_new = []() {
    return make_unique<int>(42);
};

// Exception safety benefit:
class Dangerous {
public:
    Dangerous(unique_ptr<Resource> r) : resource(move(r)) { }
private:
    unique_ptr<Resource> resource;
};

// Safe: If Dangerous constructor throws, r is still managed
auto danger = make_unique<Dangerous>(make_unique<Resource>());

// Unsafe: If Dangerous constructor throws, new Resource() leaks
// auto danger = unique_ptr<Dangerous>(
//     new Dangerous(unique_ptr<Resource>(new Resource())));
```

### make_unique Best Practices

```cpp
// Prefer make_unique over new + unique_ptr
// Exception-safe
auto ptr1 = make_unique<MyClass>(arg1, arg2);

// More concise
auto ptr2 = make_unique<MyClass>();

// Automatic type deduction
auto ptr3 = make_unique<string>("hello");

// Use in containers
vector<unique_ptr<Resource>> resources;
resources.push_back(make_unique<Resource>());
resources.push_back(make_unique<Resource>());
// Automatic cleanup when vector destroyed
```

---

# SECTION 6: RELAXED CONSTEXPR RESTRICTIONS

## 6.1 Enhanced constexpr Functions

C++14 relaxes constexpr restrictions, allowing more complex compile-time computation.

### C++11 constexpr Limitations

```cpp
// C++11: constexpr function must have exactly one statement
constexpr int square_11(int x) {
    return x * x;  // Only return statement allowed
}

// C++11: Can't use local variables or loops
// constexpr int factorial_11(int n) {
//     int result = 1;           // ERROR - variable not allowed
//     for (int i = 2; i <= n; i++) {  // ERROR - loops not allowed
//         result *= i;
//     }
//     return result;
// }
```

### C++14 constexpr Enhancements

```cpp
// C++14: Local variables allowed
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

cout << factorial(5) << "\n";  // Computed at compile-time if possible

// Compile-time constant
int arr[factorial(5)];  // Array of size 120

// C++14: More complex logic allowed
constexpr bool is_prime(int n) {
    if (n < 2) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

static_assert(is_prime(7));    // Compile-time check
static_assert(!is_prime(4));   // Compile-time check
```

### C++14 constexpr Features

```cpp
// Control flow statements
constexpr int abs_diff(int a, int b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}

// Multiple return points
constexpr int sign(int x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

// Loops
constexpr int sum_range(int start, int end) {
    int total = 0;
    for (int i = start; i < end; i++) {
        total += i;
    }
    return total;
}

cout << sum_range(1, 10) << "\n";  // 45

// Fibonacci with better performance
constexpr int fib(int n) {
    if (n <= 1) return n;
    int a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        int next = a + b;
        a = b;
        b = next;
    }
    return b;
}

cout << fib(20) << "\n";  // 6765
```

### constexpr Still Has Limitations

```cpp
// C++14: Still can't do
// - Dynamic memory allocation (new/delete)
// - Floating-point in some contexts (limited)
// - Most library functions

constexpr void* bad() {
    // return new int(42);  // ERROR
}

// But can call other constexpr functions
constexpr int helper() { return 42; }
constexpr int caller() {
    return helper() * 2;  // OK
}
```

### Practical constexpr Uses

```cpp
// Compile-time lookup table
constexpr int digit_to_value(char d) {
    if (d >= '0' && d <= '9') return d - '0';
    if (d >= 'a' && d <= 'f') return d - 'a' + 10;
    if (d >= 'A' && d <= 'F') return d - 'A' + 10;
    return -1;
}

// Compile-time string parsing
constexpr int hex_to_int(const char* str) {
    int result = 0;
    for (int i = 0; str[i]; i++) {
        int digit = digit_to_value(str[i]);
        if (digit < 0) break;
        result = result * 16 + digit;
    }
    return result;
}

constexpr int hex_value = hex_to_int("FF");  // 255, computed at compile-time

// Compile-time array generation
constexpr int powers[10] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000
};

// Compile-time validation
constexpr bool validate_date(int year, int month, int day) {
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    return true;
}

static_assert(validate_date(2024, 12, 25));
```

---

# SECTION 7: VARIABLE TEMPLATES

## 7.1 Template Variables (C++14)

Variables can be templates, not just functions and classes.

### Basic Variable Template

```cpp
#include <iostream>
using namespace std;

// Template variable
template<typename T>
constexpr T pi = T(3.141592653589793);

// Use with different types
cout << pi<double> << "\n";        // 3.14159 (double)
cout << pi<float> << "\n";         // 3.14159 (float)
cout << pi<int> << "\n";           // 3 (int)

// Can use in computations
double circle_area = pi<double> * 5 * 5;  // Area of circle with radius 5
float sphere_volume = (4.0/3.0) * pi<float> * 3 * 3 * 3;
```

### Variable Template with Type Traits

```cpp
#include <type_traits>

// Type trait as variable template
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;

template<typename T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;

// Usage (cleaner than ::value)
if (is_integral_v<int>) { }        // true
if (is_integral_v<double>) { }     // false
if (is_floating_point_v<double>) { }  // true
```

### Useful Variable Templates

```cpp
// Concept-like variable template
template<typename T>
constexpr bool is_arithmetic_type = 
    is_integral_v<T> || is_floating_point_v<T>;

static_assert(is_arithmetic_type<int>);
static_assert(is_arithmetic_type<double>);
// static_assert(is_arithmetic_type<string>);  // false

// Size information
template<typename T>
constexpr size_t sizeof_v = sizeof(T);

cout << sizeof_v<int> << "\n";      // 4
cout << sizeof_v<double> << "\n";   // 8

// Min/max values
template<typename T>
constexpr T max_value = numeric_limits<T>::max();

template<typename T>
constexpr T min_value = numeric_limits<T>::min();

cout << max_value<int> << "\n";
cout << max_value<unsigned char> << "\n";
```

### C++17 Standard Variable Templates

```cpp
// C++17 standard library additions
#include <type_traits>

// These are now variable templates in C++17
is_integral_v<int>;            // true
is_floating_point_v<double>;   // true
is_same_v<int, int>;           // true
remove_const_v<const int>;     // int
is_pointer_v<int*>;            // true

// Work like the old ::value but more concise
// is_integral<int>::value;      // Old way
is_integral_v<int>;            // New way (C++17)
```

---

# SECTION 8: AGGREGATE MEMBER INITIALIZATION

## 8.1 Extended Aggregate Initialization

C++14 extends what can be aggregate-initialized.

### Basic Aggregate Initialization (C++98)

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
};

// C++98: Brace initialization
Point p1 = {10, 20};
Point p2{30, 40};

cout << p1.x << ", " << p1.y << "\n";  // 10, 20
```

### With Base Classes (C++14)

```cpp
struct Base {
    int b;
};

struct Derived : Base {
    int d;
};

// C++14: Can initialize base class members
Derived obj{1, 2};  // b=1, d=2
cout << obj.b << ", " << obj.d << "\n";  // 1, 2
```

### Nested Aggregates

```cpp
struct Address {
    string street;
    string city;
};

struct Person {
    string name;
    int age;
    Address address;
};

// Nested initialization
Person p{"Alice", 30, {"123 Main St", "NYC"}};

cout << p.name << " lives at " << p.address.street << "\n";
```

### C++14 vs C++11 Differences

```cpp
struct Point {
    int x = 0;  // Default member initializer (C++11)
    int y = 0;
};

// C++11: Default initializer sets x=0, y=0
Point p1;           // x=0, y=0
Point p2{};         // x=0, y=0

// Explicit initialization
Point p3{10, 20};   // x=10, y=20

// Partial initialization with defaults
// Behavior similar between C++11 and C++14
```

---

# SECTION 9: MEMBER FUNCTION REF/CONST-REF QUALIFIERS

## 9.1 Lvalue vs Rvalue Member Functions

C++11 introduced, C++14 standardized: member functions can be qualified as `&` or `&&`.

### Member Function Overloading

```cpp
#include <iostream>
#include <string>
using namespace std;

class Text {
private:
    string data;

public:
    Text(string s) : data(s) {}

    // For lvalue (normal objects)
    string& get_data() & {
        cout << "Lvalue version\n";
        return data;
    }

    // For rvalue (temporaries)
    string get_data() && {
        cout << "Rvalue version\n";
        return move(data);
    }

    // Const lvalue
    const string& get_data() const& {
        cout << "Const lvalue version\n";
        return data;
    }
};

int main() {
    Text t("hello");
    
    // Calls lvalue version
    auto& result1 = t.get_data();      // "Lvalue version"
    
    // Calls const lvalue version
    const auto& result2 = t.get_data();  // "Const lvalue version"
    
    // Calls rvalue version
    auto result3 = Text("world").get_data();  // "Rvalue version"
    
    return 0;
}
```

### Practical Use Case

```cpp
class Vector {
private:
    int* data;
    int size;

public:
    Vector() : data(nullptr), size(0) {}
    Vector(int n) : data(new int[n]), size(n) {}

    // Cheap copy for lvalue - return reference
    int* get_data() & {
        return data;
    }

    // Cheap move for rvalue - return by value
    int* get_data() && {
        int* tmp = data;
        data = nullptr;
        return tmp;
    }

    ~Vector() { delete[] data; }
};

Vector createVector(int n) {
    return Vector(n);
}

int main() {
    Vector v(100);
    
    // Lvalue: efficient reference
    int* ptr1 = v.get_data();
    
    // Rvalue: efficient move
    int* ptr2 = createVector(100).get_data();
    
    return 0;
}
```

### Const/Volatile Combinations

```cpp
class Object {
public:
    // All combinations possible:
    void method() & { }           // Lvalue
    void method() const& { }      // Const lvalue
    void method() && { }          // Rvalue
    void method() const&& { }     // Const rvalue
    void method() volatile& { }   // Volatile lvalue
    // ... more combinations
};
```

---

# SECTION 10: STD::INTEGER_SEQUENCE

## 10.1 Compile-Time Integer Sequences

`std::integer_sequence` provides compile-time sequences of integers.

### Basic Usage

```cpp
#include <utility>
#include <iostream>
using namespace std;

// Create a sequence 0, 1, 2, 3, 4
using seq = integer_sequence<int, 0, 1, 2, 3, 4>;

// More practical: Generate sequence
using seq5 = make_integer_sequence<int, 5>;  // 0, 1, 2, 3, 4

// Use with function
template<typename T, T... Is>
void print_sequence(integer_sequence<T, Is...>) {
    ((cout << Is << " "), ...);  // C++17 fold expression
    cout << "\n";
}

print_sequence(make_integer_sequence<int, 10>());  // 0 1 2 3 4 5 6 7 8 9
```

### Unpacking Tuple

```cpp
#include <tuple>

// Convert tuple to function arguments
template<typename F, typename Tuple, size_t... Is>
auto apply_impl(F&& f, Tuple&& t, index_sequence<Is...>) {
    return forward<F>(f)(get<Is>(forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
auto apply(F&& f, Tuple&& t) {
    return apply_impl(
        forward<F>(f),
        forward<Tuple>(t),
        make_index_sequence<tuple_size_v<decay_t<Tuple>>>()
    );
}

// Usage
auto add = [](int a, int b, int c) { return a + b + c; };
auto result = apply(add, make_tuple(1, 2, 3));  // 6
```

### Array Initialization

```cpp
template<typename T, size_t N, size_t... Is>
void fill_array_impl(array<T, N>& arr, index_sequence<Is...>) {
    (..., (arr[Is] = Is * Is));  // Fill with squares
}

template<typename T, size_t N>
void fill_array(array<T, N>& arr) {
    fill_array_impl(arr, make_index_sequence<N>());
}

array<int, 5> arr;
fill_array(arr);
// arr: {0, 1, 4, 9, 16}
```

---

# SECTION 11: LIBRARY IMPROVEMENTS

## 11.1 STL Enhancements in C++14

### std::quoted for String I/O

```cpp
#include <iostream>
#include <iomanip>
#include <string>
using namespace std;

string text = "Hello \"World\"";

// Without quoted
cout << text << "\n";
// Output: Hello "World"

// With quoted (C++14)
cout << quoted(text) << "\n";
// Output: "Hello \"World\""

// Useful for CSV and JSON
cout << quoted("value with spaces") << "\n";
```

### std::less and Comparators

```cpp
// Transparent comparators (C++14)
set<int, less<>> s;  // Uses operator< for any comparable types

s.insert(5);
cout << s.count(5) << "\n";  // 1

// Can search with different type
cout << s.count(5.0) << "\n";  // Works with double too
```

### Algorithms Returning Pair

```cpp
#include <algorithm>

vector<int> v = {1, 2, 3, 4, 5};

// Functions returning pairs of iterators
auto [first, last] = equal_range(v.begin(), v.end(), 3);
// C++17: structured binding to unpack pair

// Alternative (C++14)
auto range = equal_range(v.begin(), v.end(), 3);
auto first_elem = range.first;
auto last_elem = range.second;
```

### std::exchange

```cpp
#include <utility>

int x = 5;
int old_value = exchange(x, 10);

cout << x << "\n";          // 10
cout << old_value << "\n";  // 5

// Useful for swapping
struct Object {
    Data data;
    Object& operator=(Object&& other) noexcept {
        data = exchange(other.data, Data());
        return *this;
    }
};
```

### std::get with Type

```cpp
#include <tuple>

tuple<int, double, string> t{42, 3.14, "hello"};

// Get by index (C++11)
auto a = get<0>(t);  // 42

// Get by type (C++14) - must be unique type
auto b = get<double>(t);  // 3.14
auto c = get<string>(t);  // "hello"

// ERROR if type appears twice
// tuple<int, int, string> t2;
// get<int>(t2);  // Ambiguous!
```

---

# SECTION 12: DEPRECATED FEATURES & REMOVALS

## 12.1 Features Deprecated in C++14

```cpp
// 1. std::auto_ptr (deprecated)
// Use unique_ptr instead
// auto_ptr<int> old_ptr(new int(42));  // Deprecated
auto new_ptr = make_unique<int>(42);     // Modern

// 2. std::binary_function, unary_function
// No longer needed with lambdas
// struct Plus : binary_function<int, int, int> {
//     int operator()(int a, int b) const { return a + b; }
// };

auto plus = [](int a, int b) { return a + b; };  // Modern

// 3. std::bind1st, bind2nd
// Use std::bind or lambdas
// auto partial = bind1st(plus(), 5);  // Deprecated

auto partial = [](int x) { return 5 + x; };  // Modern
```

### 12.5 Shared Locks (Reader-Writer Mutex)

C++14 introduces `shared_timed_mutex` allowing multiple readers but exclusive writers.

```cpp
#include <shared_mutex>
#include <mutex>
#include <map>

class ThreadSafeCache {
    std::map<int, int> data;
    mutable std::shared_timed_mutex mtx; // C++14 (use shared_mutex in C++17)

public:
    // Reader: Multiple threads can hold shared_lock
    int get(int key) const {
        std::shared_lock<std::shared_timed_mutex> lock(mtx);
        if (data.find(key) != data.end()) {
            return data.at(key);
        }
        return -1;
    }

    // Writer: Only one thread can hold unique_lock
    void put(int key, int value) {
        std::unique_lock<std::shared_timed_mutex> lock(mtx);
        data[key] = value;
    }
};
```

---

# SECTION 13: C++14 BEST PRACTICES

## What's Better with C++14

```cpp
// 1. Use generic lambdas for flexibility
auto process = [](auto x) { cout << x << "\n"; };
process(42);
process("hello");
process(3.14);

// 2. Use auto return types to avoid redundancy
auto add(int a, int b) { return a + b; }

// 3. Use make_unique for safety
auto ptr = make_unique<MyClass>(arg1, arg2);

// 4. Use binary literals for clarity
unsigned char mask = 0b11110000;

// 5. Use digit separators for readability
long big = 1'000'000'000'000;

// 6. Use init capture for move-only types
auto lambda = [ptr = move(ptr)]() { };

// 7. Use relaxed constexpr for compile-time computation
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return result;
}

// 8. Use variable templates for type information
template<typename T>
constexpr bool is_integral_v = is_integral<T>::value;
```

---

## <a name="chapter-10-c17modernfeatures"></a>CHAPTER 10: C++17 MODERN FEATURES

## C++17 Overview & Significance

C++17 (finalized in 2017) is a **major language update** rivaling C++11 in scope.

### Timeline & Context
- **2011**: C++11 released (revolutionary)
- **2014**: C++14 released (refinement)
- **2017**: C++17 released (major overhaul)
- **2020**: C++20 released (revolutionary again)

### C++17 Philosophy
- **Fix** fundamental issues with C++11/14
- **Add** major features requested by industry
- **Improve** performance and expressivity
- **Simplify** complex code patterns
- **Standardize** common idioms

### Key Themes
1. **Pattern Matching** - Structured bindings
2. **Null Safety** - optional, variant
3. **Performance** - string_view, if constexpr
4. **Expressivity** - Fold expressions
5. **Safety** - Filesystem library
6. **Parallelism** - Parallel algorithms
7. **Type Deduction** - CTAD
8. **Flexibility** - std::any

### Why C++17 Matters
C++17 addresses real pain points:
- ✅ Safer null handling (optional)
- ✅ Type-safe unions (variant)
- ✅ Zero-copy string operations (string_view)
- ✅ Compile-time branching (if constexpr)
- ✅ Pattern matching (structured bindings)
- ✅ Safe filesystem access
- ✅ Automatic template deduction
- ✅ Flexible type storage (any)

---

# SECTION 1: STRUCTURED BINDINGS

## 1.1 Introduction to Structured Bindings

Structured bindings allow unpacking objects into individual variables.

### Basic Structured Bindings

```cpp
#include <tuple>
#include <iostream>
using namespace std;

// Before C++17: Verbose
tuple<int, double, string> t = {42, 3.14, "hello"};
int a = get<0>(t);
double b = get<1>(t);
string c = get<2>(t);

// C++17: Clean and simple
auto [x, y, z] = t;
cout << x << ", " << y << ", " << z << "\n";  // 42, 3.14, hello
```

### Structured Bindings with Pairs

```cpp
#include <map>

map<string, int> ages{{"Alice", 30}, {"Bob", 25}};

// Before C++17: Awkward
for (const auto& pair : ages) {
    const string& name = pair.first;
    int age = pair.second;
    cout << name << " is " << age << "\n";
}

// C++17: Natural
for (const auto& [name, age] : ages) {
    cout << name << " is " << age << "\n";
}
```

### Structured Bindings with Arrays

```cpp
// Arrays
int arr[3] = {1, 2, 3};
auto [a, b, c] = arr;
cout << a << ", " << b << ", " << c << "\n";  // 1, 2, 3

// Fixed-size arrays
array<double, 4> coords = {1.0, 2.0, 3.0, 4.0};
auto [x, y, z, w] = coords;
```

### Structured Bindings with Structs

```cpp
struct Person {
    string name;
    int age;
    string city;
};

Person p{"Alice", 30, "NYC"};

// Before C++17
string name = p.name;
int age = p.age;
string city = p.city;

// C++17
auto [name, age, city] = p;
cout << name << " is " << age << " from " << city << "\n";
```

### Structured Bindings with Return Values

```cpp
pair<bool, int> divide(int a, int b) {
    if (b == 0) return {false, 0};
    return {true, a / b};
}

// Before C++17: Awkward
auto result = divide(10, 2);
if (result.first) {
    cout << "Result: " << result.second << "\n";
}

// C++17: Clear
auto [success, value] = divide(10, 2);
if (success) {
    cout << "Result: " << value << "\n";
}
```

### Structured Bindings with References

```cpp
int x = 5, y = 10;

// Modify through references
auto& [rx, ry] = tuple<int&, int&>(x, y);
rx = 20;
cout << x << "\n";  // 20 (modified)

// Const references
const auto& [cx, cy] = tuple<int, int>(5, 10);
// cx = 100;  // ERROR - const
```

### Practical Use Cases

```cpp
// Function returning multiple values
tuple<bool, string, int> parse_config(const string& path) {
    // Parse and return success, name, port
    return {true, "server", 8080};
}

auto [ok, name, port] = parse_config("/etc/config");
if (ok) {
    cout << "Server " << name << " on port " << port << "\n";
}

// Iterating over map with unpacking
map<string, vector<int>> data{
    {"a", {1, 2, 3}},
    {"b", {4, 5, 6}}
};

for (auto [key, values] : data) {
    cout << key << ": ";
    for (int v : values) cout << v << " ";
    cout << "\n";
}

// Database-like access
vector<tuple<int, string, double>> records{
    {1, "Alice", 95.5},
    {2, "Bob", 87.0},
    {3, "Carol", 92.5}
};

for (auto [id, name, score] : records) {
    cout << id << ": " << name << " scored " << score << "\n";
}
```

### Structured Bindings Rules

```cpp
// Auto deduction (copies)
tuple<int, int> t{1, 2};
auto [a, b] = t;           // a, b are copies

// References
auto& [x, y] = t;          // x, y reference t's elements
const auto& [cx, cy] = t;  // const references

// Move
auto&& [mx, my] = move(t); // rvalue references

// Partial binding (with operator[])
struct Container {
    int& operator[](size_t i);  // Must return reference
};

Container c;
auto [elem] = c;           // Gets copy via operator[]

// Multiple items
auto [a, b, c, d] = tuple{1, 2, 3, 4};
auto [x, y, z] = array{10, 20, 30};
```

## 1.2 Structured Bindings Under the Hood

The compiler generates a hidden variable.

**Code:**
```cpp
auto [x, y] = my_pair;
```

**Compiler Logic:**
```cpp
auto __hidden = my_pair;
auto& x = __hidden.first;  // Aliases
auto& y = __hidden.second;
```

**Implication**:
*   `x` and `y` are NOT variables; they are names referring to subobjects of the hidden variable.
*   If you use `auto& [x, y]`, the hidden variable is a reference.

---

# SECTION 2: OPTIONAL & VARIANT

## 2.1 std::optional - Safe Nullable Values

`std::optional` represents a value that may or may not be present.

### Basic optional Usage

```cpp
#include <optional>
#include <iostream>
using namespace std;

// Function that might not return a value
optional<int> parse_int(const string& s) {
    try {
        return stoi(s);
    } catch (...) {
        return nullopt;  // No value
    }
}

// Usage
auto result1 = parse_int("42");
if (result1.has_value()) {
    cout << "Value: " << result1.value() << "\n";
}

// Or using operator*
if (result1) {
    cout << "Value: " << *result1 << "\n";
}

// Default value
auto value = result1.value_or(0);  // 0 if no value
```

### optional with Complex Types

```cpp
struct User {
    int id;
    string name;
};

optional<User> find_user(int id) {
    if (id < 0) return nullopt;
    return User{id, "User" + to_string(id)};
}

// Usage
if (auto user = find_user(42)) {
    cout << user->name << "\n";
} else {
    cout << "User not found\n";
}
```

### optional Operations

```cpp
optional<int> opt(42);

// Check
if (opt) cout << "Has value\n";          // true
if (opt.has_value()) cout << "Has\n";    // true

// Access
cout << opt.value() << "\n";              // 42
cout << *opt << "\n";                     // 42
cout << opt.value_or(0) << "\n";          // 42

// Modify
opt.value() = 100;
cout << *opt << "\n";                     // 100

// Reset
opt = nullopt;
if (opt) cout << "Has value\n";          // false

// Or assign
opt = 99;
cout << *opt << "\n";                     // 99
```

### optional with Chaining

```cpp
optional<int> process(optional<int> input) {
    if (!input) return nullopt;
    return input.value() * 2;
}

auto result = process(optional<int>(5));
if (result) {
    cout << result.value() << "\n";  // 10
}

// Chain operations
auto chain = parse_int("42")
    .and_then([](int x) -> optional<int> { return x * 2; })
    .or_else([]() { return optional<int>(0); });
```

---

## 2.2 std::variant - Type-Safe Union

`std::variant` is a type-safe union that holds one of several types.

### Basic variant Usage

```cpp
#include <variant>
#include <iostream>
using namespace std;

// Can hold int, double, or string
variant<int, double, string> value;

// Store int
value = 42;
cout << get<int>(value) << "\n";  // 42

// Store double
value = 3.14;
cout << get<double>(value) << "\n";  // 3.14

// Store string
value = string("hello");
cout << get<string>(value) << "\n";  // hello

// Check type
cout << value.index() << "\n";  // 2 (string is third)
```

### variant with Type Checking

```cpp
void process(variant<int, double, string> value) {
    if (holds_alternative<int>(value)) {
        cout << "Integer: " << get<int>(value) << "\n";
    } else if (holds_alternative<double>(value)) {
        cout << "Double: " << get<double>(value) << "\n";
    } else if (holds_alternative<string>(value)) {
        cout << "String: " << get<string>(value) << "\n";
    }
}

process(42);              // "Integer: 42"
process(3.14);            // "Double: 3.14"
process("hello");         // "String: hello"
```

### variant with Visitor Pattern

```cpp
struct Visitor {
    void operator()(int i) const {
        cout << "Integer: " << i << "\n";
    }
    
    void operator()(double d) const {
        cout << "Double: " << d << "\n";
    }
    
    void operator()(const string& s) const {
        cout << "String: " << s << "\n";
    }
};

variant<int, double, string> v = 42;
visit(Visitor(), v);  // "Integer: 42"

v = 3.14;
visit(Visitor(), v);  // "Double: 3.14"

v = string("hello");
visit(Visitor(), v);  // "String: hello"
```

### variant with Lambdas (C++20 or using overload trick)

```cpp
// C++17 overload trick
template<typename... Ts>
struct overload : Ts... { using Ts::operator()...; };
template<typename... Ts>
overload(Ts...) -> overload<Ts...>;

variant<int, double, string> v = 42;

// Visit with lambdas
visit(overload{
    [](int i) { cout << "Integer: " << i << "\n"; },
    [](double d) { cout << "Double: " << d << "\n"; },
    [](const string& s) { cout << "String: " << s << "\n"; }
}, v);  // "Integer: 42"
```

### Practical variant Example

```cpp
variant<int, string> parse_value(const string& input) {
    try {
        return stoi(input);  // Try as int
    } catch (...) {
        return input;        // Return as string
    }
}

auto result = parse_value("42");
if (holds_alternative<int>(result)) {
    cout << "Parsed as int: " << get<int>(result) << "\n";
} else {
    cout << "Parsed as string: " << get<string>(result) << "\n";
}
```

---

# SECTION 3: STD::ANY

## 3.1 Type-Erased Storage with std::any

`std::any` can hold any copyable type.

### Basic any Usage

```cpp
#include <any>
#include <iostream>
using namespace std;

any value;

// Store different types
value = 42;
cout << any_cast<int>(value) << "\n";  // 42

value = 3.14;
cout << any_cast<double>(value) << "\n";  // 3.14

value = string("hello");
cout << any_cast<string>(value) << "\n";  // hello

// Check type
if (value.type() == typeid(string)) {
    cout << "It's a string\n";
}
```

### any with Type Checking

```cpp
any value = 42;

if (value.type() == typeid(int)) {
    cout << "Value: " << any_cast<int>(value) << "\n";
}

// Safe cast (throws if wrong type)
try {
    double d = any_cast<double>(value);  // Wrong type
} catch (const bad_any_cast& e) {
    cout << "Type mismatch: " << e.what() << "\n";
}

// Check before casting
if (value.type() == typeid(int)) {
    int i = any_cast<int>(value);
}
```

### any in Collections

```cpp
vector<any> data;
data.push_back(42);
data.push_back(3.14);
data.push_back(string("hello"));
data.push_back(vector<int>{1, 2, 3});

// Process
for (auto& item : data) {
    if (item.type() == typeid(int)) {
        cout << "Int: " << any_cast<int>(item) << "\n";
    } else if (item.type() == typeid(double)) {
        cout << "Double: " << any_cast<double>(item) << "\n";
    } else if (item.type() == typeid(string)) {
        cout << "String: " << any_cast<string>(item) << "\n";
    }
}
```

### any vs variant

```cpp
// variant<int, double, string>: Fixed types, type-safe
variant<int, double, string> v = 42;
cout << get<int>(v) << "\n";  // Type-safe, no runtime check needed

// any: Any type, runtime type checking
any a = 42;
cout << any_cast<int>(a) << "\n";  // Runtime check, potential exception

// Use variant when types are known
// Use any when types are truly dynamic
```

---

# SECTION 4: STD::STRING_VIEW

## 4.1 Non-Owning String References

`std::string_view` provides efficient, zero-copy string operations.

### Basic string_view

```cpp
#include <string_view>
#include <iostream>
using namespace std;

string s = "Hello, World!";
string_view sv = s;

cout << sv << "\n";           // "Hello, World!"
cout << sv.length() << "\n";  // 13
cout << sv[0] << "\n";        // 'H'
cout << sv.data() << "\n";    // "Hello, World!"
```

### string_view from Different Sources

```cpp
// From std::string
string s = "hello";
string_view sv1 = s;

// From C-string
const char* cstr = "world";
string_view sv2 = cstr;

// From string literal
string_view sv3 = "test";

// Substring
string_view sv4 = sv1.substr(1, 3);  // "ell"
```

### string_view Operations

```cpp
string_view sv = "Hello, World!";

// Searching
cout << sv.find("World") << "\n";      // 7
cout << sv.find(',') << "\n";          // 5

// Comparing
cout << sv.compare("Hello, World!") << "\n";  // 0 (equal)

// Prefix/suffix
cout << sv.starts_with("Hello") << "\n";  // true
cout << sv.ends_with("!") << "\n";        // true

// Substrings
auto hello = sv.substr(0, 5);           // "Hello"
auto world = sv.substr(7);              // "World!"

// Remove prefix/suffix (C++20)
// sv.remove_prefix(7);
// sv.remove_suffix(1);
```

### Performance Benefits of string_view

```cpp
// OLD: Copy string
void process_old(const string& s) {
    // s might be copied
}

// NEW: No copy
void process_new(string_view sv) {
    // sv references the string, no copy
}

// Usage
string data = "important";
process_old(data);  // Might copy
process_new(data);  // No copy!

// Works with literals too
process_new("temporary");  // No copy, no allocation
```

### string_view Limitations

```cpp
string_view sv = "hello";

// What you CAN'T do:
// sv[0] = 'H';                    // ERROR - can't modify
// sv.resize(3);                   // ERROR - can't resize
// string s(sv);                   // OK - explicit conversion needed

// Works only while source exists
string_view sv2;
{
    string temp = "danger";
    sv2 = temp;
    // temp destroyed here
}
// sv2 now points to destroyed string - DANGER!

// Safe way: Make a copy if needed
string copy(sv2);
```

### string_view Use Cases

```cpp
// Parse tokens without copying
string_view parse_token(string_view& input) {
    size_t pos = input.find(' ');
    if (pos == string_view::npos) {
        string_view token = input;
        input = "";
        return token;
    }
    string_view token = input.substr(0, pos);
    input = input.substr(pos + 1);
    return token;
}

// Check file extensions efficiently
bool is_cpp_file(string_view filename) {
    return filename.ends_with(".cpp") || 
           filename.ends_with(".h") ||
           filename.ends_with(".hpp");
}

// Efficient URL parsing
void parse_url(string_view url) {
    size_t protocol_end = url.find("://");
    string_view protocol = url.substr(0, protocol_end);
    // ... continue parsing
}
```

---

# SECTION 5: IF CONSTEXPR

## 5.1 Compile-Time Conditional Code

`if constexpr` allows branching at compile-time.

### Basic if constexpr

```cpp
#include <type_traits>

template<typename T>
void process(T value) {
    if constexpr (is_integral_v<T>) {
        cout << "Integer: " << value << "\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float: " << value << "\n";
    } else {
        cout << "Other type\n";
    }
}

process(42);        // "Integer: 42" - int branch only compiled
process(3.14);      // "Float: 3.14" - double branch only compiled
process("hello");   // "Other type"
```

### if constexpr Advantages

```cpp
// Before C++17: SFINAE (complex, verbose)
template<typename T>
enable_if_t<is_integral_v<T>>
old_way(T x) { cout << "Int\n"; }

template<typename T>
enable_if_t<is_floating_point_v<T>>
old_way(T x) { cout << "Float\n"; }

// After C++17: if constexpr (clean, readable)
template<typename T>
void new_way(T x) {
    if constexpr (is_integral_v<T>) {
        cout << "Int\n";
    } else if constexpr (is_floating_point_v<T>) {
        cout << "Float\n";
    }
}
```

### if constexpr with Complex Logic

```cpp
template<typename T>
void serialize(const T& value) {
    if constexpr (is_arithmetic_v<T>) {
        // Serialize numbers efficiently
        write_binary(value);
    } else if constexpr (is_same_v<T, string>) {
        // Serialize strings
        write_string(value);
    } else if constexpr (requires { value.size(); }) {
        // Serialize containers
        for (const auto& item : value) {
            serialize(item);
        }
    }
}
```

### if constexpr with Concepts (C++20-like)

```cpp
template<typename T>
void print_info(const T& value) {
    cout << "Type: " << typeid(T).name() << "\n";
    
    if constexpr (requires { value.size(); }) {
        cout << "Size: " << value.size() << "\n";
    }
    
    if constexpr (requires { value.data(); }) {
        cout << "Data pointer available\n";
    }
    
    if constexpr (is_default_constructible_v<T>) {
        T temp;  // Can create temporary
    }
}
```

## 5.2 The Death of SFINAE?

`if constexpr` replaces SFINAE for *implementation details*, but not for *overload resolution*.

**SFINAE (Old Way - C++11/14):**
```cpp
template <typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
gcd(T a, T b) { return b == 0 ? a : gcd(b, a % b); }

template <typename T>
typename std::enable_if<!std::is_integral<T>::value, T>::type
gcd(T a, T b) { static_assert(std::is_integral<T>::value, "GCD not for floats"); }
```

**if constexpr (New Way - C++17):**
```cpp
template <typename T>
T gcd(T a, T b) {
    if constexpr (std::is_integral_v<T>) {
        return b == 0 ? a : gcd(b, a % b);
    } else {
        static_assert(always_false<T>, "GCD not for floats");
    }
}
```
*Result*: Much cleaner, easier to debug errors.

---

# SECTION 6: FOLD EXPRESSIONS

## 6.1 Operating on Parameter Packs

Fold expressions simplify operations on variadic templates.

### Basic Fold Expressions

```cpp
#include <iostream>
using namespace std;

// Sum with fold
template<typename... Args>
int sum(Args... args) {
    return (... + args);  // Left fold: ((a + b) + c) + d
}

cout << sum(1, 2, 3, 4) << "\n";  // 10

// Product with fold
template<typename... Args>
int product(Args... args) {
    return (... * args);  // Left fold: ((a * b) * c) * d
}

cout << product(2, 3, 4) << "\n";  // 24

// Logical AND
template<typename... Args>
bool all_true(Args... args) {
    return (... && args);
}

cout << all_true(true, true, true) << "\n";      // true
cout << all_true(true, false, true) << "\n";     // false
```

### Fold Directions

```cpp
// Left fold: (... + args) = ((a + b) + c) + d
template<typename... Args>
int left_fold(Args... args) {
    return (... + args);
}

// Right fold: (args + ...) = a + (b + (c + d))
template<typename... Args>
int right_fold(Args... args) {
    return (args + ...);
}

// For addition, result is the same
left_fold(1, 2, 3);   // ((1 + 2) + 3) = 6
right_fold(1, 2, 3);  // (1 + (2 + 3)) = 6

// For subtraction, different!
// left: ((1 - 2) - 3) = -4
// right: (1 - (2 - 3)) = 2
```

### Fold with Default Value

```cpp
// No default value
template<typename... Args>
int sum_no_default(Args... args) {
    return (... + args);  // Error if no args
}

// With default value
template<typename... Args>
int sum_default(Args... args) {
    return (args + ... + 0);  // 0 if no args
}

sum_default();              // 0
sum_default(1, 2, 3);       // 6
```

### Practical Fold Examples

```cpp
// Print all arguments
template<typename... Args>
void print_all(Args... args) {
    ((cout << args << " "), ...);
}

print_all(1, "hello", 3.14, "world");
// Output: 1 hello 3.14 world

// Check if any true
template<typename... Args>
bool any_true(Args... args) {
    return (... || args);
}

any_true(false, false, true, false);  // true

// Maximum of values
template<typename... Args>
int maximum(Args... args) {
    return max({args...});  // Fold into initializer
}

cout << maximum(3, 1, 4, 1, 5, 9) << "\n";  // 9
```

## 6.2 Advanced Fold Expressions

The comma operator `,` is the most powerful fold operator. It evaluates LHS, discards it, then RHS.

### Calling Function on Pack
```cpp
template<typename... Args>
void log_all(Args... args) {
    (..., (cout << "[LOG] " << args << "\n")); 
}
```

### Validating All Arguments
```cpp
template<typename... Args>
bool validate_all(Args... args) {
    // Returns true if ALL args are valid
    return (... && (args.is_valid()));
}
```

---

# SECTION 7: CLASS TEMPLATE ARGUMENT DEDUCTION (CTAD)

## 7.1 Automatic Template Type Deduction

CTAD allows deduction of class template parameters from constructor arguments.

### Before CTAD (C++14)

```cpp
#include <vector>
#include <map>
using namespace std;

// Must specify types explicitly
vector<int> v{1, 2, 3};
pair<string, int> p("hello", 42);
map<string, int> m{{"a", 1}, {"b", 2}};
```

### With CTAD (C++17)

```cpp
// Types deduced automatically!
vector v{1, 2, 3};              // Deduced as vector<int>
pair p("hello", 42);             // Deduced as pair<string, int>
map m{pair("a", 1), pair("b", 2)}; // Deduced as map<string, int>

// Works with custom classes too
template<typename T, typename U>
struct Pair {
    T first;
    U second;
};

Pair p{42, 3.14};  // Deduced as Pair<int, double>
```

### CTAD with Custom Deduction Guides

```cpp
template<typename T>
struct Container {
    vector<T> data;
};

// Without guide: Would fail
// Container c{1, 2, 3};  // ERROR - can't deduce T

// With deduction guide
template<typename T>
Container(initializer_list<T>) -> Container<T>;

// Now it works!
Container c{1, 2, 3};  // Deduced as Container<int>
```

### Practical CTAD Examples

```cpp
// Multiple parameters
template<typename T, typename U>
class Pair {
public:
    Pair(T first, U second) : first(first), second(second) {}
    T first;
    U second;
};

Pair p{1, "hello"};  // Deduced as Pair<int, const char*>

// Arrays
array a{1, 2, 3, 4, 5};  // Deduced as array<int, 5>

// Aggregates
struct Point {
    int x, y;
};

Point pt{10, 20};  // No deduction needed (aggregate), but works
```

### CTAD Benefits

```cpp
// Reduces verbosity
auto v1 = vector<int>{1, 2, 3};  // C++11 way
auto v2 = vector{1, 2, 3};       // C++17 way

// More readable with complex types
map m1{pair<const int, string>{1, "a"}};  // Verbose
map m2{pair{1, "a"}};                      // Clean

// Especially useful with templates
template<typename Iter>
auto process(Iter first, Iter last) {
    vector v(first, last);  // Deduced type!
    return v;
}
```

---

# SECTION 8: STD::FILESYSTEM

## 8.1 Portable File System Operations

`std::filesystem` provides safe, portable file system access.

### Basic Path Operations

```cpp
#include <filesystem>
#include <iostream>
using namespace std;
namespace fs = filesystem;

// Create path
fs::path p1 = "/home/user/file.txt";
fs::path p2 = "relative/path/file.txt";

// Query components
cout << p1.filename() << "\n";      // "file.txt"
cout << p1.parent_path() << "\n";   // "/home/user"
cout << p1.extension() << "\n";     // ".txt"
cout << p1.stem() << "\n";          // "file"

// Normalize
cout << p1.lexically_normal() << "\n";  // Remove .. and .
```

### File Existence & Type Checking

```cpp
namespace fs = filesystem;

fs::path p = "/path/to/file";

if (fs::exists(p)) {
    cout << "Path exists\n";
}

if (fs::is_regular_file(p)) {
    cout << "Is a regular file\n";
    cout << "Size: " << fs::file_size(p) << " bytes\n";
}

if (fs::is_directory(p)) {
    cout << "Is a directory\n";
}

if (fs::is_symlink(p)) {
    cout << "Is a symbolic link\n";
}
```

### Directory Operations

```cpp
namespace fs = filesystem;

fs::path dir = "/path/to/directory";

// List directory contents
for (const auto& entry : fs::directory_iterator(dir)) {
    cout << entry.path().filename() << "\n";
    if (entry.is_directory()) {
        cout << "  (directory)\n";
    }
}

// Recursive directory listing
for (const auto& entry : fs::recursive_directory_iterator(dir)) {
    cout << entry.path().relative_path(dir) << "\n";
}
```

### File Operations

```cpp
namespace fs = filesystem;

fs::path src = "original.txt";
fs::path dst = "copy.txt";

// Copy file
fs::copy_file(src, dst);

// Move/rename
fs::rename(src, dst);

// Delete file
fs::remove(dst);

// Delete directory (must be empty)
fs::path dir = "empty_directory";
fs::remove(dir);

// Create directory
fs::path newdir = "new_directory";
fs::create_directory(newdir);

// Create nested directories
fs::create_directories("a/b/c/d");
```

### Practical filesystem Example

```cpp
#include <filesystem>
#include <fstream>
using namespace std;
namespace fs = filesystem;

// Find all C++ files in directory
void find_cpp_files(const fs::path& dir) {
    for (const auto& entry : fs::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension();
            if (ext == ".cpp" || ext == ".h" || ext == ".hpp") {
                cout << entry.path() << "\n";
            }
        }
    }
}

// Count lines in a file
int count_lines(const fs::path& file) {
    ifstream f(file);
    int count = 0;
    string line;
    while (getline(f, line)) count++;
    return count;
}

// Safe file backup
void backup_file(const fs::path& original) {
    if (!fs::exists(original)) {
        throw runtime_error("File not found");
    }
    
    fs::path backup = original;
    backup.replace_extension(
        backup.extension().string() + ".bak"
    );
    
    fs::copy_file(original, backup, 
                  fs::copy_options::overwrite_existing);
}
```

## 8.5 Filesystem Deep Dive

### Exception-Free API
Most `std::filesystem` functions have an overload taking `std::error_code&` to avoid exceptions.

```cpp
std::error_code ec;
if (fs::exists("/tmp/ghost", ec)) {
    // ...
}
if (ec) {
    std::cerr << "Error: " << ec.message() << "\n";
}
```

### Space & Permissions
```cpp
auto space = fs::space("/");
cout << "Free: " << space.free / 1024 / 1024 << " MB\n";

fs::permissions("file.txt", 
    fs::perms::owner_read | fs::perms::owner_write,
    fs::perm_options::add);
```

---

# SECTION 9: STD::INVOKE

## 9.1 Uniform Callable Invocation

`std::invoke` provides uniform way to call functions, methods, and functors.

### Basic invoke

```cpp
#include <functional>
#include <iostream>
using namespace std;

// Regular function
int add(int a, int b) { return a + b; }

// Invoke function
cout << invoke(add, 5, 3) << "\n";  // 8

// Function pointer
int (*fp)(int, int) = add;
cout << invoke(fp, 5, 3) << "\n";   // 8

// Lambda
auto lambda = [](int a, int b) { return a * b; };
cout << invoke(lambda, 5, 3) << "\n";  // 15

// Functor
struct Multiplier {
    int operator()(int a, int b) const { return a * b; }
};

Multiplier m;
cout << invoke(m, 5, 3) << "\n";  // 15
```

### invoke with Methods

```cpp
struct Calculator {
    int value = 0;
    
    int add(int x) { return value + x; }
    int multiply(int x) const { return value * x; }
};

Calculator calc{10};

// Invoke member function
cout << invoke(&Calculator::add, calc, 5) << "\n";     // 15
cout << invoke(&Calculator::multiply, calc, 3) << "\n"; // 30

// With pointer
Calculator* ptr = &calc;
cout << invoke(&Calculator::add, ptr, 5) << "\n";      // 15

// With shared_ptr, unique_ptr
auto up = make_unique<Calculator>();
up->value = 20;
cout << invoke(&Calculator::add, up.get(), 5) << "\n"; // 25
```

### invoke with Data Members

```cpp
struct Person {
    string name;
    int age;
};

Person p{"Alice", 30};

// Invoke data member access
cout << invoke(&Person::name, p) << "\n";  // "Alice"
cout << invoke(&Person::age, p) << "\n";   // 30

// Modify through invoke
invoke(&Person::age, p) = 31;
cout << p.age << "\n";  // 31
```

### Practical invoke Pattern

```cpp
// Create callable wrapper that works with anything
template<typename Func, typename... Args>
auto call_wrapper(Func&& f, Args&&... args) {
    return invoke(forward<Func>(f), forward<Args>(args)...);
}

// Use with different callables
call_wrapper(add, 5, 3);              // Function
call_wrapper(lambda, 5, 3);           // Lambda
call_wrapper(&Calculator::add, calc, 5); // Member function
```

---

# SECTION 10: PARALLEL ALGORITHMS

## 10.1 Parallel Algorithm Execution

C++17 adds parallel execution to standard algorithms.

### Execution Policies

```cpp
#include <algorithm>
#include <execution>
#include <vector>
using namespace std;

vector<int> v = {5, 2, 8, 1, 9, 3};

// Sequential (traditional)
sort(v.begin(), v.end());

// Parallel (may use multiple threads)
sort(execution::par, v.begin(), v.end());

// Parallel unsequenced (even less ordering guarantee)
sort(execution::par_unseq, v.begin(), v.end());

// Sequenced unsequenced (no vectorization)
sort(execution::unseq, v.begin(), v.end());
```

### Parallel Algorithm Examples

```cpp
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5};

// Parallel transform
transform(execution::par, v.begin(), v.end(), v.begin(),
    [](int x) { return x * 2; });

// Parallel accumulate
int sum = reduce(execution::par, v.begin(), v.end());

// Parallel find_if
auto it = find_if(execution::par, v.begin(), v.end(),
    [](int x) { return x > 5; });

// Parallel count_if
int even_count = count_if(execution::par, v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });
```

### Performance Considerations

```cpp
vector<int> large(1000000);

// Sequential: Simple, predictable
sort(large.begin(), large.end());

// Parallel: Faster for large data (overhead for small)
sort(execution::par, large.begin(), large.end());

// Parallel unsequenced: Allows compiler optimizations
sort(execution::par_unseq, large.begin(), large.end());
```

---

# SECTION 11: CORE LANGUAGE FEATURES

## 11.1 Additional C++17 Features

### Nested namespaces

```cpp
// C++14
namespace A {
    namespace B {
        namespace C {
            int x = 42;
        }
    }
}

// C++17
namespace A::B::C {
    int x = 42;
}
```

### Inline Variables

```cpp
// Header file
inline int global_var = 42;  // Definition, can be in header
inline vector<int> global_vec;

// No ODR (One Definition Rule) violation
// Can include this header in multiple translation units
```

### Structured Exception Handling (still mostly the same, but with improvements)

```cpp
try {
    // Code
} catch (const exception& e) {
    cout << e.what() << "\n";
}
```

### Deduced Return Types in Lambdas

```cpp
auto lambda = [](int x) -> auto {  // C++17
    return x * 2;  // Return type deduced
};
```

### std::byte

```cpp
#include <cstddef>

byte b1{42};
byte b2 = 0xAB_B;  // Literal
cout << (int)b1 << "\n";  // Cast to see value
```

---

# SECTION 12: LIBRARY IMPROVEMENTS

## 12.1 STL Enhancements in C++17

### std::optional Algorithms

```cpp
optional<int> opt{42};

opt.and_then([](int x) -> optional<int> {
    return x * 2;
}).or_else([]() { return optional<int>(0); });
```

### Improved Algorithms

```cpp
vector<int> v = {1, 2, 3, 4, 5};

// uninitialized operations now work with ranges
vector<int> v2(5);
uninitialized_copy(v.begin(), v.end(), v2.begin());

// reduce (like accumulate but parallel-friendly)
int sum = reduce(v.begin(), v.end());
```

### std::charconv

```cpp
#include <charconv>

// Fast, exception-safe number conversion
int x = 42;
char buffer[100];
auto result = to_chars(buffer, buffer + 100, x);

string str("123");
int value;
auto [ptr, ec] = from_chars(str.data(), str.data() + str.size(), value);
if (ec == errc()) {
    cout << value << "\n";  // 123
}
```

---

# SECTION 13: POLYMORPHIC MEMORY RESOURCES (PMR)

## 13.1 Introduction to std::pmr

C++17 introduces `std::pmr` (Polymorphic Memory Resources) in `<memory_resource>`, enabling efficient, customizable memory management without changing container types.

### The Problem with Traditional Allocators
Traditional allocators are part of the type signature: `std::vector<int, MyAlloc<int>>` is a different type from `std::vector<int>`.

`std::pmr` erases the allocator type, allowing containers with different allocation strategies to be used interchangeably.

```cpp
#include <vector>
#include <memory_resource>
#include <iostream>

// Use pmr namespace
namespace pmr = std::pmr;

void process_data(const pmr::vector<int>& data) {
    // Works with ANY allocator (stack, heap, pool)
    for (int x : data) std::cout << x << " ";
}

int main() {
    // 1. Default allocator (Heap)
    pmr::vector<int> heap_vec = {1, 2, 3};
    process_data(heap_vec);
    
    // 2. Stack allocator (Monotonic Buffer)
    std::array<std::byte, 1024> buffer;
    pmr::monotonic_buffer_resource pool{
        buffer.data(), buffer.size(), pmr::null_memory_resource()
    };
    
    pmr::vector<int> stack_vec(&pool);
    stack_vec.push_back(4);
    stack_vec.push_back(5);
    process_data(stack_vec);
    
    return 0;
}
```

## 13.2 Memory Resources

### Standard Memory Resources

```cpp
#include <memory_resource>

// 1. new_delete_resource (Global Heap)
auto* heap = std::pmr::new_delete_resource();

// 2. null_memory_resource (Throws bad_alloc)
auto* null = std::pmr::null_memory_resource();

// 3. monotonic_buffer_resource (Fast, no deallocation)
// Very fast for building complex structures, deallocates all at once
std::pmr::monotonic_buffer_resource fast_pool(heap);

// 4. synchronized_pool_resource (Thread-safe pool)
// Good for many small allocations of same size
std::pmr::synchronized_pool_resource thread_safe_pool(heap);

// 5. unsynchronized_pool_resource (Single-thread pool)
// Fastest for single-threaded small allocations
std::pmr::unsynchronized_pool_resource local_pool(heap);
```

### Chaining Resources

Memory resources can be chained. If a pool runs out of memory, it requests more from an "upstream" resource.

```cpp
#include <memory_resource>
#include <vector>

void chaining_example() {
    // Buffer on stack
    std::array<std::byte, 256> buffer;
    
    // Primary: Use stack buffer
    // Upstream: If buffer full, go to heap
    std::pmr::monotonic_buffer_resource mem_res(
        buffer.data(), buffer.size(), std::pmr::new_delete_resource()
    );
    
    std::pmr::vector<int> vec(&mem_res);
    
    // These go to stack buffer
    for (int i = 0; i < 50; i++) vec.push_back(i);
    
    // If we exceed buffer, it silently falls back to heap
}
```

### Performance Benefits

`std::pmr` allows easy implementation of **Arena Allocation** or **Stack Allocation** for standard containers, which can provide massive performance gains (cache locality, no malloc overhead) for short-lived complex data structures.

---

# SECTION 14: C++17 BEST PRACTICES

## What's Better with C++17

```cpp
// 1. Use structured bindings to unpack
auto [x, y, z] = tuple{1, 2, 3};

// 2. Use optional for nullable values
optional<int> result = process();
if (result) { cout << result.value() << "\n"; }

// 3. Use variant for type-safe unions
variant<int, string> value = 42;

// 4. Use string_view for non-owning strings
void process(string_view sv);  // No copy!

// 5. Use if constexpr for compile-time branching
if constexpr (is_integral_v<T>) { }

// 6. Use fold expressions for parameter packs
auto sum = (... + args);

// 7. Use filesystem for file operations
for (const auto& entry : fs::directory_iterator(dir)) { }

// 8. Use invoke for uniform callable invocation
invoke(func, args...);

// 9. Use parallel algorithms for performance
sort(execution::par, v.begin(), v.end());

// 10. Use CTAD for cleaner code
vector v{1, 2, 3};  // Not vector<int>{...}
```


---

# Volume III: Modern Mastery

## <a name="chapter-11-c20revolutionaryfeatures"></a>CHAPTER 11: C++20 REVOLUTIONARY FEATURES

## C++20 Overview & Revolutionary Scope

C++20 (finalized in 2020) is a **revolutionary language update** rivaling C++11 in magnitude.

### Timeline & Context
- **2011**: C++11 (first modern standard)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (latest)

### C++20 Philosophy
- **Revolutionize** generic programming with concepts
- **Simplify** iteration with ranges
- **Empower** asynchronous programming with coroutines
- **Standardize** previously non-standard patterns
- **Address** fundamental C++ limitations
- **Enable** modern programming paradigms

### Key Themes
1. **Concepts** - Readable, constrained templates
2. **Ranges** - Composable, lazy evaluation
3. **Coroutines** - Asynchronous, generator patterns
4. **Spaceship** - Three-way comparison
5. **Modules** - Modularity & faster compilation
6. **Designated Initializers** - Named struct initialization
7. **Format** - Type-safe string formatting
8. **Constraints** - Compile-time validation

### Why C++20 Matters
C++20 addresses fundamental limitations:
- ✅ Readable generic programming (concepts)
- ✅ Composable iteration (ranges)
- ✅ Async/await patterns (coroutines)
- ✅ Lazy evaluation (ranges with coroutines)
- ✅ Modular code (modules)
- ✅ Type-safe formatting (std::format)
- ✅ Compile-time validation (consteval)
- ✅ Powerful iteration patterns

---

# SECTION 1: CONCEPTS & CONSTRAINTS

## 1.1 Introduction to Concepts

Concepts are constraints on template parameters that make templates readable and enable better error messages.

### Basic Concept Definition

```cpp
#include <concepts>
using namespace std;

// Define a concept
template<typename T>
concept Integral = is_integral_v<T>;

// Use concept as constraint
template<Integral T>
void process(T value) {
    cout << "Integer: " << value << "\n";
}

process(42);              // OK - int satisfies Integral
process(3.14);            // ERROR - double doesn't satisfy Integral
// Error message is clear: doesn't satisfy Integral concept
```

### Standard Library Concepts

```cpp
#include <concepts>

// Predefined concepts
template<typename T>
concept Integer = integral<T>;  // std::integral

template<typename T>
concept Floating = floating_point<T>;  // std::floating_point

template<typename T>
concept Numeric = integral<T> || floating_point<T>;

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> convertible_to<bool>;
    { a == b } -> convertible_to<bool>;
};

// Usage
template<Comparable T>
T find_min(T a, T b) {
    return a < b ? a : b;
}

find_min(5, 3);           // OK
find_min("a", "b");       // OK - strings are comparable
// find_min(complex(1,2), complex(3,4));  // ERROR - complex not comparable
```

### Complex Concept Definition

```cpp
#include <concepts>
#include <ranges>

// Concept with multiple requirements
template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    typename T::iterator;
    typename T::const_iterator;
    { c.begin() } -> convertible_to<typename T::iterator>;
    { c.end() } -> convertible_to<typename T::iterator>;
    { c.size() } -> convertible_to<size_t>;
    { c.empty() } -> convertible_to<bool>;
};

// Use in function
template<Container C>
void print_container(const C& c) {
    for (const auto& elem : c) {
        cout << elem << " ";
    }
    cout << "\n";
}

vector<int> v{1, 2, 3};
print_container(v);  // OK

// Custom type
struct MyContainer {
    vector<int> data;
    using value_type = int;
    using iterator = vector<int>::iterator;
    using const_iterator = vector<int>::const_iterator;
    
    iterator begin() { return data.begin(); }
    iterator end() { return data.end(); }
    const_iterator begin() const { return data.begin(); }
    const_iterator end() const { return data.end(); }
    size_t size() const { return data.size(); }
    bool empty() const { return data.empty(); }
};

MyContainer mc;
print_container(mc);  // OK
```

### Concept Benefits

```cpp
// Before C++20: Complex error messages
template<typename T>
void process_old(T x) {
    // If T doesn't have operator+, error is confusing
    auto result = x + 5;
}

process_old("string");  // ERROR - cryptic, long error message

// After C++20: Clear error messages
template<typename T>
requires requires(T x) { x + 5; }
void process_new(T x) {
    auto result = x + 5;
}

process_new("string");  // ERROR - "string" doesn't satisfy concept
```

---

## 1.2 Requires Expressions

Requires expressions test compile-time properties of types.

### Basic Requires Expression

```cpp
#include <concepts>

template<typename T>
requires requires(T x) {
    x + 1;           // Must support addition
    x.size();        // Must have size() member
    { x == x };      // Must support equality
}
void process(T x);

// Can also write as concept
template<typename T>
concept Processable = requires(T x) {
    x + 1;
    x.size();
    { x == x };
};

template<Processable T>
void process2(T x);
```

### Requires with Return Type Checking

```cpp
#include <concepts>

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> convertible_to<T>;
};

template<typename T>
concept Multipliable = requires(T a, T b) {
    { a * b } -> convertible_to<T>;
};

template<typename T>
concept Arithmetic = Addable<T> && Multipliable<T>;

template<Arithmetic T>
T compute(T x, T y) {
    return (x + y) * (x - y);
}

cout << compute(5, 3) << "\n";        // OK - int is Arithmetic
cout << compute(2.5, 1.5) << "\n";    // OK - double is Arithmetic
```

### Practical Requires Examples

```cpp
// Check for operator[] and size()
template<typename T>
concept Indexable = requires(T t, size_t i) {
    { t[i] };
    { t.size() } -> convertible_to<size_t>;
};

// Check for specific method
template<typename T>
concept HasValue = requires(T t) {
    { t.value() };
};

// Check for const and non-const versions
template<typename T>
concept ConstIterable = requires(const T& t) {
    t.begin();
    t.end();
};

// Multi-type concepts
template<typename Iter, typename Sentinel>
concept SentinelFor = requires(Iter it, Sentinel s) {
    { it == s } -> convertible_to<bool>;
};
```

## 1.3 Concepts & Overload Resolution

Concepts participate in overload resolution. The compiler selects the **most constrained** template.

```cpp
template<typename T>
void process(T x) {
    cout << "Generic\n";
}

template<typename T> requires std::integral<T>
void process(T x) {
    cout << "Integral\n";
}

template<typename T> requires (std::integral<T> && sizeof(T) >= 4)
void process(T x) {
    cout << "Large Integral\n";
}

process(3.14);      // "Generic"
process((short)10); // "Integral"
process(100);       // "Large Integral" (int is >= 4 bytes)
```

---

# SECTION 2: RANGES LIBRARY

## 2.1 Introduction to Ranges

Ranges provide a composable, lazy way to work with sequences.

### Basic Range Operations

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Traditional algorithm
vector<int> result;
for (int x : v) {
    if (x % 2 == 0) {
        result.push_back(x * 2);
    }
}

// With ranges (composable, lazy)
auto result = v
    | ranges::views::filter([](int x) { return x % 2 == 0; })
    | ranges::views::transform([](int x) { return x * 2; });

// result is lazy - computation happens on iteration
for (int x : result) {
    cout << x << " ";  // 4 8 12 16 20
}
```

### Range Views

```cpp
#include <ranges>
#include <vector>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5};

// filter view
auto evens = v | ranges::views::filter([](int x) { return x % 2 == 0; });

// transform view
auto doubled = v | ranges::views::transform([](int x) { return x * 2; });

// take view (first N elements)
auto first3 = v | ranges::views::take(3);

// drop view (skip first N elements)
auto skip2 = v | ranges::views::drop(2);

// reverse view
auto reversed = v | ranges::views::reverse;

// iota view (generate sequence)
auto seq = ranges::views::iota(1, 11);  // 1..10

// join view (flatten nested ranges)
vector<vector<int>> matrix = {{1, 2}, {3, 4}, {5, 6}};
auto flattened = matrix | ranges::views::join;

// zip view (pair elements from two ranges)
vector<int> a = {1, 2, 3};
vector<string> b = {"a", "b", "c"};
auto zipped = ranges::views::zip(a, b);

for (auto [num, str] : zipped) {
    cout << num << ":" << str << " ";  // 1:a 2:b 3:c
}
```

### Range Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>
using namespace std;

vector<int> v = {3, 1, 4, 1, 5, 9};

// Range algorithms (work with ranges, not iterators)
ranges::sort(v);                    // In-place sort
ranges::reverse(v);                 // In-place reverse
ranges::fill(v, 0);                 // Fill with value

// Range algorithms with predicates
ranges::sort(v, ranges::greater{});  // Sort descending
auto it = ranges::find(v, 5);        // Find element
auto count = ranges::count_if(v, [](int x) { return x > 3; });

// Range operations
ranges::rotate(v.begin(), v.begin() + 2, v.end());
ranges::partition(v, [](int x) { return x % 2 == 0; });
```

### Composing Multiple Views

```cpp
#include <ranges>
#include <vector>
#include <iostream>
using namespace std;

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Chain multiple operations
auto result = v
    | ranges::views::filter([](int x) { return x > 2; })      // > 2
    | ranges::views::transform([](int x) { return x * x; })   // Square
    | ranges::views::take(4);                                   // First 4

// Process
for (int x : result) {
    cout << x << " ";  // 9 16 25 36
}

// All operations are lazy - no temporary vectors created
// Composition is clear and readable
```

## 2.2 Ranges Deep Dive

### Projections
Most range algorithms accept a "projection" argument to transform data *before* comparison.

```cpp
struct User { int id; string name; };
vector<User> users = {{2, "Bob"}, {1, "Alice"}};

// Sort by ID
ranges::sort(users, {}, &User::id);

// Sort by Name (descending)
ranges::sort(users, ranges::greater{}, &User::name);
```

### Dangling Iterators
Algorithms return `std::ranges::dangling` if the range is an rvalue (temporary) to prevent use-after-free.

```cpp
auto get_vector() { return vector{1, 2, 3}; }

auto it = ranges::find(get_vector(), 2); 
// Compile Error! 'it' would be dangling.
// The vector is destroyed at the end of the statement.
```

---

# SECTION 3: COROUTINES

## 3.1 Introduction to Coroutines

Coroutines enable asynchronous, generator, and lazy evaluation patterns.

### Basic Generator Coroutine

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator {
public:
    struct promise_type {
        T current_value;
        
        Generator get_return_object() {
            return Generator{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        suspend_always yield_value(T value) {
            current_value = value;
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    struct iterator {
        coroutine_handle<promise_type> handle;
        
        iterator(coroutine_handle<promise_type> h, bool done) 
            : handle(h) {
            if (done) {
                handle = nullptr;
            }
        }
        
        iterator& operator++() {
            handle.resume();
            if (handle.done()) {
                handle = nullptr;
            }
            return *this;
        }
        
        bool operator==(const iterator& other) const {
            return handle == other.handle;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
        
        T operator*() const {
            return handle.promise().current_value;
        }
    };
    
    iterator begin() {
        if (handle) {
            handle.resume();
        }
        return iterator{handle, !handle || handle.done()};
    }
    
    iterator end() {
        return iterator{nullptr, true};
    }
    
private:
    coroutine_handle<promise_type> handle;
    
    Generator(coroutine_handle<promise_type> h) : handle(h) {}
};

// Generator coroutine
Generator<int> count_up(int max) {
    for (int i = 1; i <= max; i++) {
        co_yield i;  // Yield value and suspend
    }
}

// Usage
int main() {
    for (int i : count_up(5)) {
        cout << i << " ";  // 1 2 3 4 5
    }
    return 0;
}
```

### Async Coroutine

```cpp
#include <coroutine>
#include <iostream>
#include <chrono>
using namespace std;

class Task {
public:
    struct promise_type {
        Task get_return_object() {
            return Task{coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        suspend_never initial_suspend() { return {}; }
        suspend_always final_suspend() noexcept { return {}; }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    coroutine_handle<promise_type> handle;
    
    Task(coroutine_handle<promise_type> h) : handle(h) {}
    
    ~Task() {
        if (handle) {
            handle.destroy();
        }
    }
};

// Async coroutine
Task async_work() {
    cout << "Starting work\n";
    co_await std::suspend_always{};  // Suspend and resume later
    cout << "Continuing work\n";
}

int main() {
    auto task = async_work();  // Starts coroutine
    // Coroutine is suspended
    task.handle.resume();       // Resume execution
    return 0;
}
```

### Practical Coroutine: Fibonacci Generator

```cpp
#include <coroutine>
#include <iostream>
using namespace std;

template<typename T>
class Generator { /* ... implementation ... */ };

Generator<int> fibonacci(int limit) {
    int a = 0, b = 1;
    while (a < limit) {
        co_yield a;
        int next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int i : fibonacci(100)) {
        cout << i << " ";  // 0 1 1 2 3 5 8 13 21 34 55 89
    }
    return 0;
}
```

## 3.2 Coroutines Deep Dive

A coroutine is a function that can suspend and resume.

### The Awaitable Interface
To `co_await x`, `x` must be an Awaitable.

```cpp
struct Awaiter {
    bool await_ready() { return false; } // Always suspend?
    
    void await_suspend(std::coroutine_handle<> h) {
        // Schedule resumption (e.g., on a thread pool)
        // h.resume(); 
    }
    
    int await_resume() { return 42; } // Result of co_await
};

Task coroutine() {
    int result = co_await Awaiter{}; // result = 42
}
```

### Symmetric Transfer
Returning a `coroutine_handle` from `await_suspend` performs a "tail-call" to resume another coroutine without consuming stack space.

```cpp
std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
    return other_handle; // Switch to other coroutine immediately
}
```

---

# SECTION 4: SPACESHIP OPERATOR (THREE-WAY COMPARISON)

## 4.1 The Spaceship Operator <=>

The spaceship operator performs three-way comparison and returns comparison category.

### Basic Spaceship Usage

```cpp
#include <compare>
#include <iostream>
using namespace std;

int a = 5, b = 10;

// Spaceship operator returns ordering
auto cmp = a <=> b;

if (cmp < 0) {
    cout << "a < b\n";
} else if (cmp > 0) {
    cout << "a > b\n";
} else {
    cout << "a == b\n";
}
```

### Spaceship with Custom Types

```cpp
#include <compare>

struct Person {
    string name;
    int age;
    
    // Default spaceship (compares as tuple)
    auto operator<=>(const Person&) const = default;
};

Person p1{"Alice", 30};
Person p2{"Bob", 25};

auto cmp = p1 <=> p2;
if (cmp < 0) cout << "p1 < p2\n";
if (cmp > 0) cout << "p1 > p2\n";
```

### Defaulted Comparison

```cpp
#include <compare>

struct Point {
    int x, y;
    
    // Default spaceship - compares lexicographically
    auto operator<=>(const Point&) const = default;
};

Point p1{1, 2};
Point p2{1, 2};
Point p3{2, 1};

cout << (p1 <=> p2 == 0) << "\n";     // true (equal)
cout << (p1 <=> p3 < 0) << "\n";      // true (p1 < p3)
```

### Comparison Categories

```cpp
#include <compare>
#include <iostream>

// Different comparison categories
struct Comparable {
    int value;
    
    // Returns std::strong_ordering (can do all operations)
    strong_ordering operator<=>(const Comparable& other) const {
        return value <=> other.value;
    }
};

struct PartiallyComparable {
    double value;
    
    // Returns std::partial_ordering (NaN is not comparable)
    partial_ordering operator<=>(const PartiallyComparable& other) const {
        return value <=> other.value;
    }
};

Comparable c1{5}, c2{10};
cout << (c1 <=> c2 < 0) << "\n";  // true

PartiallyComparable p1{1.5}, p2{2.5};
cout << (p1 <=> p2 < 0) << "\n";  // true
```

### Spaceship Benefits

```cpp
// Before C++20: Must define all comparison operators
struct Person {
    string name;
    int age;
    
    bool operator<(const Person& other) const {
        if (name != other.name) return name < other.name;
        return age < other.age;
    }
    
    bool operator<=(const Person& other) const { /* ... */ }
    bool operator>(const Person& other) const { /* ... */ }
    bool operator>=(const Person& other) const { /* ... */ }
    bool operator==(const Person& other) const { /* ... */ }
    bool operator!=(const Person& other) const { /* ... */ }
};

// After C++20: Default spaceship does all of it
struct Person {
    string name;
    int age;
    
    auto operator<=>(const Person&) const = default;
};
```

---

# SECTION 5: MODULES

## 5.1 Introduction to Modules

Modules provide better code organization and faster compilation.

### Module Definition

```cpp
// math_module.cppm (module interface unit)
export module math;

export int add(int a, int b) {
    return a + b;
}

export int multiply(int a, int b) {
    return a * b;
}

// Helper function (not exported)
int helper(int x) {
    return x * 2;
}
```

### Using Modules

```cpp
// main.cpp
import math;
#include <iostream>
using namespace std;

int main() {
    cout << add(5, 3) << "\n";              // OK - exported
    cout << multiply(5, 3) << "\n";         // OK - exported
    // cout << helper(5) << "\n";           // ERROR - not exported
    
    return 0;
}
```

### Module Partitions

```cpp
// math.cppm (main interface)
export module math;
export import :impl;

// math-impl.cppm (partition)
export module math:impl;

export struct Complex {
    double real, imag;
    
    Complex operator+(const Complex& other) const {
        return {real + other.real, imag + other.imag};
    }
};
```

### Module Benefits

```
// Before modules (header files):
// - Recompilation overhead
// - Macro pollution
// - Circular dependencies
// - Header guards boilerplate

// After modules:
// - Faster compilation (parse once)
// - No macro pollution
// - No circular dependency issues
// - Clean interface definition
```

## 5.2 Modules Deep Dive

### Global Module Fragment
For legacy headers that must be included before the module declaration.

```cpp
module; // Start fragment
#include <vector>
#include <string>

export module my_app; // End fragment, start module

export void process(std::vector<int>& v);
```

### Private Module Partition
Hiding implementation details within the same file.

```cpp
export module calculator;

export int add(int a, int b);

module :private; // Start private implementation

int helper(int x) { return x + 1; }

int add(int a, int b) {
    return helper(a) + helper(b) - 2;
}
```

---

# SECTION 6: DESIGNATED INITIALIZERS

## 6.1 Named Member Initialization

Designated initializers allow initializing struct/class members by name.

### Basic Designated Initializers

```cpp
#include <iostream>
using namespace std;

struct Point {
    int x;
    int y;
    int z;
};

// Before C++20: Order matters
Point p1{1, 2, 3};  // x=1, y=2, z=3

// After C++20: Can specify by name
Point p2{.x = 10, .y = 20, .z = 30};
Point p3{.y = 20, .x = 10, .z = 30};  // Order doesn't matter
Point p4{.x = 5, .z = 15};             // y defaults to 0

cout << p2.x << " " << p2.y << " " << p2.z << "\n";  // 10 20 30
```

### With Classes and Inheritance

```cpp
struct Base {
    int a;
};

struct Derived : Base {
    int b;
    int c;
};

// Designators for base and derived members
Derived d{.a = 1, .b = 2, .c = 3};

cout << d.a << " " << d.b << " " << d.c << "\n";  // 1 2 3
```

### Practical Designated Initializers

```cpp
struct Config {
    string name;
    int port;
    string host;
    bool ssl;
    int timeout;
};

// Clear intent - parameters obvious
Config cfg{
    .name = "server",
    .port = 8080,
    .host = "localhost",
    .ssl = true,
    .timeout = 30
};

// Much better than:
// Config cfg{"server", 8080, "localhost", true, 30};
```

---

# SECTION 7: CALENDAR & TIME ZONES

## 7.1 Advanced Chrono Features

C++20 adds comprehensive calendar and timezone support.

### Calendar Types

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Year, month, day
year y{2024};
month m{12};
day d{25};

// Construct date
auto date = y / m / d;  // 2024-12-25
cout << date << "\n";

// Current date
auto today = floor<days>(system_clock::now());
cout << "Today: " << today << "\n";

// Date arithmetic
auto tomorrow = date + days(1);
auto next_month = date + months(1);
auto next_year = date + years(1);
```

### Time Zones

```cpp
#include <chrono>
#include <iostream>
using namespace std;
using namespace chrono;

// Get timezone
const auto& tz = locate_zone("America/New_York");

// Current time in timezone
auto now = system_clock::now();
auto zoned_time = make_zoned(tz, now);

cout << "UTC: " << now << "\n";
cout << "NY: " << zoned_time << "\n";
```

### Formatted Time Output

```cpp
#include <chrono>
#include <format>
#include <iostream>
using namespace std;
using namespace chrono;

auto now = system_clock::now();

// Format with pattern
cout << format("{:%Y-%m-%d %H:%M:%S}", now) << "\n";
// Output: 2024-12-25 15:30:45
```

---

# SECTION 8: STD::FORMAT

## 8.1 Type-Safe String Formatting

`std::format` provides Python-like formatting without type unsafety.

### Basic format Usage

```cpp
#include <format>
#include <iostream>
using namespace std;

// Simple substitution
cout << format("Hello {}, you are {} years old", "Alice", 30) << "\n";
// Output: Hello Alice, you are 30 years old

// Positional arguments
cout << format("{1} {0}", "World", "Hello") << "\n";
// Output: Hello World

// Argument access by index
cout << format("{0} + {0} = {}", 5, 10) << "\n";
// Output: 5 + 5 = 10
```

### Formatting Specifications

```cpp
#include <format>
#include <iostream>
using namespace std;

int num = 255;
double pi = 3.14159;

// Hex, binary, octal
cout << format("{:x}", num) << "\n";           // ff (hex)
cout << format("{:b}", num) << "\n";           // 11111111 (binary)
cout << format("{:o}", num) << "\n";           // 377 (octal)

// Floating point precision
cout << format("{:.2f}", pi) << "\n";          // 3.14
cout << format("{:.5f}", pi) << "\n";          // 3.14159

// Padding and alignment
cout << format("{:>10}", "hello") << "\n";     // "     hello" (right)
cout << format("{:<10}", "hello") << "\n";     // "hello     " (left)
cout << format("{:^10}", "hello") << "\n";     // "  hello   " (center)

// Number formatting
cout << format("{:,}", 1234567) << "\n";       // 1,234,567 (with separator)
cout << format("{:e}", pi) << "\n";            // 3.14e+00 (scientific)
```

### Format with Custom Types

```cpp
#include <format>

struct Point {
    int x, y;
};

// Define formatter for Point
template<>
struct format_traits<Point> {
    static auto format(const Point& p) {
        return format_string("({}, {})", p.x, p.y);
    }
};

Point pt{10, 20};
cout << format("Point: {}", pt) << "\n";  // Point: (10, 20)
```

---

# SECTION 9: CONSTEVAL & CONSTINIT

## 9.1 Immediate Functions and Constants

### consteval - Immediate Functions

```cpp
#include <iostream>
using namespace std;

// Must be evaluated at compile-time
consteval int square(int x) {
    return x * x;
}

int main() {
    int arr[square(5)];           // OK - computed at compile-time
    cout << square(10) << "\n";   // OK - 100
    
    int x = 5;
    // cout << square(x) << "\n"; // ERROR - x is not compile-time constant
    
    return 0;
}
```

### constinit - Compile-Time Initialization

```cpp
#include <iostream>
using namespace std;

// Thread-local with compile-time initialization
thread_local constinit int counter = 0;

int main() {
    counter = 10;  // Can be modified at runtime
    cout << counter << "\n";  // 10
    
    return 0;
}
```

### Difference: constexpr vs consteval

```cpp
// constexpr: Can be evaluated at compile-time OR runtime
constexpr int add_constexpr(int a, int b) {
    return a + b;
}

// consteval: MUST be evaluated at compile-time
consteval int add_consteval(int a, int b) {
    return a + b;
}

int main() {
    int x = 5, y = 10;
    
    int c1 = add_constexpr(x, y);      // Runtime evaluation
    int c2 = add_constexpr(5, 10);     // Compile-time evaluation
    
    // int d1 = add_consteval(x, y);   // ERROR - must be compile-time
    int d2 = add_consteval(5, 10);     // OK - compile-time
    
    return 0;
}
```

---

# SECTION 10: LAMBDA ENHANCEMENTS

## 10.1 C++20 Lambda Improvements

### Default Constructible Lambdas

```cpp
#include <iostream>
using namespace std;

// C++20: Lambdas without captures can be default constructed
auto counter = [count = 0]() mutable { return ++count; };

// Can be default constructed
decltype(counter) c1;  // Default construct
c1();

// But lambdas with captures still can't
// auto [x] = 5;
// decltype([x]() {}) bad;  // ERROR
```

### Stateless Lambda as Template Parameter

```cpp
#include <iostream>
using namespace std;

template<auto F>
void call_func() {
    F();
}

// Stateless lambda as template argument
call_func<[]() { cout << "Hello\n"; }>();  // OK

// Stateful lambda (captures) can't be template argument
// auto y = 5;
// call_func<[y]() { cout << y; }>();  // ERROR
```

---

# SECTION 11: ADVANCED FEATURES

## 11.1 Additional C++20 Features

### Spaceship Operator with Library Support

```cpp
#include <compare>
#include <vector>

// All standard library types support spaceship
vector<int> v1{1, 2, 3};
vector<int> v2{1, 2, 4};

auto cmp = v1 <=> v2;
if (cmp < 0) cout << "v1 < v2\n";
```

### Bit Operations

```cpp
#include <bit>
#include <iostream>
using namespace std;

unsigned int x = 12;  // 0b1100

cout << bit_width(x) << "\n";           // 4 (bits needed)
cout << popcount(x) << "\n";            // 2 (number of 1s)
cout << countl_zero(x) << "\n";         // 28 (leading zeros on 32-bit)
cout << rotl(x, 2) << "\n";             // Rotate left
cout << rotr(x, 2) << "\n";             // Rotate right
cout << (x & ~(x - 1)) << "\n";         // Lowest set bit

// std::bit_cast (Safe type punning)
float f = 3.14f;
auto i = std::bit_cast<uint32_t>(f);  // Safe reinterpretation of bits
cout << std::hex << i << "\n";
```

### std::atomic_ref

`std::atomic_ref` allows atomic operations on non-atomic objects.

```cpp
#include <atomic>
#include <thread>
#include <vector>

void process(int& counter) {
    // Treat 'counter' as atomic for this scope
    std::atomic_ref<int> atomic_counter(counter);
    atomic_counter++;
}

int main() {
    int val = 0;
    std::vector<std::thread> threads;
    for(int i=0; i<10; ++i) threads.emplace_back(process, std::ref(val));
    for(auto& t : threads) t.join();
    return 0;
}
```

### Concepts in Standard Library

```cpp
#include <concepts>
#include <iostream>

// Standard concepts
static_assert(integral<int>);
static_assert(floating_point<double>);
static_assert(invocable<int(*)(int), int>);
static_assert(copyable<int>);
static_assert(assignable_from<int&, int>);

template<typename T>
requires copyable<T>
void copy_safe(const T& src, T& dst) {
    dst = src;
}
```

---

# SECTION 12: LIBRARY IMPROVEMENTS

## 12.1 STL Enhancements in C++20

### std::span (Non-owning Array View)

`std::span` provides a lightweight, non-owning view over a contiguous sequence of objects (like array, vector, or C-array).

```cpp
#include <span>
#include <vector>
#include <iostream>
#include <array>

void print_values(std::span<int> data) {
    for (int x : data) {
        std::cout << x << " ";
    }
    std::cout << "\n";
}

int main() {
    int arr[] = {1, 2, 3};
    std::vector<int> vec = {4, 5, 6};
    std::array<int, 3> std_arr = {7, 8, 9};

    // Works with all contiguous containers
    print_values(arr);        // 1 2 3
    print_values(vec);        // 4 5 6
    print_values(std_arr);    // 7 8 9
    
    // Sub-span (slicing)
    print_values(std::span(vec).subspan(1)); // 5 6
    
    return 0;
}
```

### std::semaphore

```cpp
#include <semaphore>
#include <thread>

counting_semaphore<3> sem(3);  // Max 3 concurrent

void worker() {
    sem.acquire();
    // Critical section (at most 3 threads)
    // Do work
    sem.release();
}
```

### std::latch & std::barrier

```cpp
#include <latch>
#include <barrier>
#include <thread>
#include <vector>

// Latch: one-time synchronization
latch finish(3);

void worker(latch& l) {
    // Do work
    l.count_down();
    l.wait();  // Wait for all to finish
}

// Barrier: reusable synchronization
barrier sync(3);

void barrier_worker(barrier& b) {
    while (true) {
        // Do work
        b.arrive_and_wait();  // Synchronize every iteration
    }
}
```

### std::source_location (Reflection for Logging)

```cpp
#include <source_location>
#include <iostream>

void log(const char* message, 
         const std::source_location location = std::source_location::current()) {
    std::cout << "Info: " << message << "\n"
              << "File: " << location.file_name() << "("
              << location.line() << ":" << location.column() << ")\n"
              << "Func: " << location.function_name() << "\n";
}

int main() {
    log("Something happened");
    return 0;
}
```

### std::osyncstream (Synchronized Output)

Prevents interleaved output from multiple threads.

```cpp
#include <syncstream>
#include <iostream>
#include <thread>

void worker(int id) {
    std::osyncstream(std::cout) << "Worker " << id << " is running\n";
}

int main() {
    std::thread t1(worker, 1);
    std::thread t2(worker, 2);
    t1.join(); t2.join();
    return 0;
}
```

### Ranges with Algorithms

```cpp
#include <ranges>
#include <vector>
#include <algorithm>

vector<int> v = {3, 1, 4, 1, 5};

// Ranges algorithms with pipes
v | ranges::views::sort
  | ranges::views::unique
  | ranges::views::take(3)

### std::jthread (Auto-joining Thread)

```cpp
#include <thread>
#include <iostream>
using namespace std;

void worker(std::stop_token st) {
    while (!st.stop_requested()) {
        cout << "Working...\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    cout << "Worker stopped\n";
}

int main() {
    // jthread automatically joins on destruction
    // and supports stop_token
    std::jthread t(worker);
    
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // t.request_stop() called automatically or manually
    return 0;
}
```
```

---

# SECTION 13: C++20 BEST PRACTICES

## What's Better with C++20

```cpp
// 1. Use concepts for readable templates
template<integral T>
void process(T x);

// 2. Use ranges for composable operations
auto result = v
    | ranges::views::filter([](int x) { return x > 0; })
    | ranges::views::transform([](int x) { return x * 2; });

// 3. Use coroutines for generators
Generator<int> count(int n) {
    for (int i = 0; i < n; i++) {
        co_yield i;
    }
}

// 4. Use spaceship for comparisons
auto cmp = a <=> b;

// 5. Use designated initializers
Config cfg{.host = "localhost", .port = 8080};

// 6. Use std::format for formatting
cout << format("Value: {:.2f}", value);

// 7. Use consteval for compile-time guarantees
consteval int compile_time_only(int x);

// 8. Use modules for better organization
export module app;
```

---

## <a name="chapter-12-c23latestfeatures"></a>CHAPTER 12: C++23 LATEST FEATURES

## C++23 Overview & Direction

C++23 (finalized in 2023) is a **refinement and enhancement** of C++20 with practical improvements.

### Timeline & Context
- **2011**: C++11 (revolutionary)
- **2014**: C++14 (refinement)
- **2017**: C++17 (major improvements)
- **2020**: C++20 (revolutionary leap)
- **2023**: C++23 (practical enhancements)

### C++23 Philosophy
- **Enhance** existing C++20 features
- **Fill gaps** in C++20 design
- **Improve** convenience and usability
- **Optimize** common patterns
- **Standardize** frequently-requested features
- **Fix** issues discovered in C++20

### Key Themes
1. **Output & Formatting** - std::print for easy output
2. **Error Handling** - std::expected for results
3. **Loop Control** - Enhanced for loops with ranges
4. **Memory Safety** - Better pointer/array handling
5. **Debugging** - Stack traces
6. **Templates** - Deducing this improvements
7. **Constexpr** - More compile-time power
8. **Library** - Quality of life improvements

### Why C++23 Matters
C++23 builds on C++20 strengths:
- ✅ Easier output without iostream overhead
- ✅ Type-safe error handling (std::expected)
- ✅ Better for loop control
- ✅ Debugging support (stack traces)
- ✅ More flexible subscript operator
- ✅ Improved constexpr capabilities
- ✅ More convenient library features
- ✅ Better optional support

---

# SECTION 1: STD::PRINT & FORMATTED OUTPUT

## 1.1 std::print - Simple Output

`std::print` provides easy, fast output without iostream overhead.

### Basic print Usage

```cpp
#include <print>
#include <iostream>

// Simple output (no newline by default)
std::print("Hello, World!");

// With newline
std::println("Hello, World!");

// With format
std::println("Number: {}, Float: {:.2f}", 42, 3.14159);

// To stderr
std::print(std::cerr, "Error: {}\n", "something went wrong");
std::println(std::cerr, "Error: {}", "something went wrong");
```

### print vs format vs iostream

```cpp
#include <print>
#include <format>
#include <iostream>

std::string msg = "Hello";
int value = 42;

// iostream (slow, verbose)
std::cout << msg << ": " << value << "\n";

// format (creates string, then print)
std::cout << std::format("{}: {}\n", msg, value);

// print (direct output, fast)
std::println("{}: {}", msg, value);
```

### print with File Streams

```cpp
#include <print>
#include <fstream>

std::ofstream file("output.txt");

// Direct to file
std::println(file, "Line 1: {}", 42);
std::println(file, "Line 2: {}", "test");

// Simpler than:
// file << "Line 1: " << 42 << "\n";
```

---

## 1.2 std::format Enhancements

### Format Improvements in C++23

```cpp
#include <format>

// More format options
double pi = 3.14159;

std::format("{:.2%}", 0.25);         // "25.00%" (percentage)
std::format("{:g}", 0.0001);         // General format
std::format("{:#x}", 255);           // "0xff" (with prefix)
std::format("{:_^10}", "test");      // "_____test" (custom fill)
```

---

# SECTION 2: DEDUCING THIS

## 2.1 Explicit Member Function Parameters

`Deducing this` allows capturing the type and constness of the object.

### Basic Deducing This

```cpp
#include <iostream>
using namespace std;

struct Counter {
    int count = 0;
    
    // Traditional
    void increment() {
        count++;
    }
    
    // With deducing this
    void increment_new(this auto& self) {
        self.count++;
    }
    
    // Value vs reference overloads now simple
    auto get_data(this auto& self) {
        return self.count;
    }
};

Counter c;
c.increment_new();
cout << c.get_data() << "\n";  // 1
```

### Deducing This for Const/Non-Const

```cpp
struct Data {
    int value = 42;
    
    // Single function handles both const and non-const
    auto& get(this auto& self) {
        return self.value;
    }
    
    // Before C++23: Need two overloads
    // int& get() { return value; }
    // const int& get() const { return value; }
};

Data d;
d.get() = 100;              // Mutable reference
cout << d.get() << "\n";    // 100

const Data cd;
cout << cd.get() << "\n";   // 100 (const reference)
```

### Practical Deducing This

```cpp
template<typename T>
struct Optional {
    T value;
    bool has_value_flag = false;
    
    // Works for both optional<T> and optional<const T>
    auto* get_if_value(this auto* self) {
        return self->has_value_flag ? &self->value : nullptr;
    }
};

Optional<int> opt;
opt.value = 42;
opt.has_value_flag = true;

if (auto* ptr = opt.get_if_value()) {
    cout << *ptr << "\n";  // 42
}
```

## 2.2 Deducing This - Beyond the Basics

### Recursive Lambdas
Previously, lambdas couldn't easily call themselves. Now they can via the explicit object parameter.

```cpp
auto fib = [](this auto&& self, int n) {
    if (n <= 1) return n;
    return self(n - 1) + self(n - 2);
};

cout << fib(10) << "\n"; // 55
```

### Replacing CRTP
The Curiously Recurring Template Pattern (CRTP) was used to inject functionality into derived classes. `Deducing this` simplifies it.

**Old CRTP:**
```cpp
template <typename Derived>
struct Addable {
    Derived& operator+=(const Derived& other) {
        static_cast<Derived*>(this)->value += other.value;
        return *static_cast<Derived*>(this);
    }
};
struct Int : Addable<Int> { int value; };
```

**New C++23 Way:**
```cpp
struct Addable {
    template <typename Self>
    auto& operator+=(this Self&& self, const Self& other) {
        self.value += other.value;
        return self;
    }
};
struct Int : Addable { int value; }; // No template parameter needed!
```

---

# SECTION 3: RANGE-BASED FOR LOOP ENHANCEMENTS

## 3.1 For Loop Initializers

C++23 allows initialization in range-based for loops.

### For Loop with Init

```cpp
#include <vector>
#include <iostream>
using namespace std;

vector<int> v1 = {1, 2, 3};
vector<int> v2 = {4, 5, 6};

// Traditional
vector<int> combined;
for (int x : v1) combined.push_back(x);
for (int x : v2) combined.push_back(x);

// C++23: Initialize in loop
for (auto v = vector<int>{1, 2, 3, 4, 5, 6}; int x : v) {
    cout << x << " ";
}

// More practical
for (auto file = open_file("data.txt"); auto line : file.lines()) {
    cout << line << "\n";
}
```

### For Loop with Init and Structured Binding

```cpp
map<string, vector<int>> data{
    {"a", {1, 2, 3}},
    {"b", {4, 5, 6}}
};

// Initialize and use with structured binding
for (auto it = data.begin(); auto [key, values] : data) {
    cout << key << ": ";
    for (int v : values) cout << v << " ";
    cout << "\n";
}
```

---

# SECTION 4: STD::EXPECTED

## 4.1 Result Type for Error Handling

`std::expected` represents either a value or an error.

### Basic expected Usage

```cpp
#include <expected>
#include <string>
#include <iostream>
using namespace std;

enum class ParseError { InvalidFormat, OutOfRange };

// Function returning expected
expected<int, ParseError> parse_int(const string& s) {
    try {
        return stoi(s);
    } catch (const invalid_argument&) {
        return unexpected(ParseError::InvalidFormat);
    } catch (const out_of_range&) {
        return unexpected(ParseError::OutOfRange);
    }
}

// Usage
auto result = parse_int("42");

if (result) {
    cout << "Value: " << result.value() << "\n";
} else {
    cout << "Error\n";
}
```

### expected with Transform

```cpp
#include <expected>

expected<int, string> get_value();

// Chain operations with transform
auto result = get_value()
    .transform([](int x) { return x * 2; })
    .transform_error([](const string& e) { return "Failed: " + e; });

if (result) {
    cout << result.value() << "\n";
} else {
    cout << result.error() << "\n";
}
```

### expected vs optional

```cpp
// optional: Has value or nothing
optional<int> opt = parse_int("42");
if (!opt) {
    // But why did it fail?
}

// expected: Has value or specific error
expected<int, ParseError> exp = parse_int("42");
if (!exp) {
    cout << "Error: " << static_cast<int>(exp.error()) << "\n";
}
```

---

# SECTION 5: STD::OPTIONAL IMPROVEMENTS

## 5.1 Enhanced optional Operations

### optional with Deref Operator

```cpp
#include <optional>

optional<int> opt{42};

// C++23: Monadic operations
auto result = opt
    .and_then([](int x) -> optional<int> { return x * 2; })
    .or_else([]() { return optional<int>(0); });

// C++23: Chaining
opt.transform([](int x) { return x + 10; })
   .and_then([](int x) -> optional<int> {
       if (x > 50) return x;
       return nullopt;
   });
```

### optional::value_or_else

```cpp
#include <optional>

optional<int> opt;

// Get value or call function to generate default
int value = opt.value_or_else([]() { return compute_default(); });

// More flexible than value_or
// value_or: int value = opt.value_or(0);
// value_or_else: int value = opt.value_or_else([]() { return expensive_computation(); });
```

---

# SECTION 6: MULTIDIMENSIONAL SUBSCRIPT OPERATOR

## 6.1 Multiple Index Support

C++23 allows multiple indices in subscript operator.

### Multi-Index Subscript

```cpp
#include <iostream>
using namespace std;

struct Matrix {
    int data[10][10];
    
    // C++23: Multi-index operator
    int& operator[](int row, int col) {
        return data[row][col];
    }
    
    int& operator[](int row, int col) const {
        return data[row][col];
    }
};

Matrix m;
m[2, 3] = 42;           // Two indices
cout << m[2, 3] << "\n"; // 42
```

### Dynamic 2D Array Wrapper

```cpp
template<typename T>
struct Array2D {
    vector<T> data;
    size_t width, height;
    
    Array2D(size_t w, size_t h) : width(w), height(h), data(w * h) {}
    
    T& operator[](size_t row, size_t col) {
        return data[row * width + col];
    }
    
    const T& operator[](size_t row, size_t col) const {
        return data[row * width + col];
    }
};

Array2D<int> grid(3, 3);
grid[1, 1] = 5;
cout << grid[1, 1] << "\n";  // 5
```

---

## 6.2 std::mdspan (Multidimensional View)

`std::mdspan` provides a non-owning multidimensional view of contiguous data.

### Basic mdspan Usage

```cpp
#include <mdspan>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> data = {
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12
    };

    // Create a 3x4 view over the data (C++23)
    // using MatrixView = std::mdspan<int, std::dextents<size_t, 2>>;
    // MatrixView m(data.data(), 3, 4);
    
    // m[1, 2] == 7 (row 1, col 2)
    // No copying of data involved!
    
    return 0;
}
```

## 6.3 mdspan Layouts

You can control how 2D indices map to the 1D memory.

*   `std::layout_right`: Row-major (default in C++). Index `(i, j)` is `i * N + j`.
*   `std::layout_left`: Column-major (Fortran/MATLAB). Index `(i, j)` is `j * M + i`.

```cpp
using RowMajor = std::mdspan<double, std::dextents<size_t, 2>, std::layout_right>;
using ColMajor = std::mdspan<double, std::dextents<size_t, 2>, std::layout_left>;

// Same data, different interpretation
std::vector<double> v = {1, 2, 3, 4}; // 2x2 matrix
RowMajor m_row(v.data(), 2, 2); // [[1, 2], [3, 4]]
ColMajor m_col(v.data(), 2, 2); // [[1, 3], [2, 4]]
```

---

# SECTION 7: STD::STACKTRACE

## 7.1 Runtime Stack Trace Capture

`std::stacktrace` provides runtime stack trace information.

### Basic Stacktrace

```cpp
#include <stacktrace>
#include <print>
#include <iostream>

void deep_function() {
    // Capture current stack trace
    auto trace = std::stacktrace::current();
    
    // Print trace
    std::println("Stack trace:");
    for (const auto& entry : trace) {
        std::println("  {}", entry);
    }
}

void middle_function() {
    deep_function();
}

void top_function() {
    middle_function();
}

int main() {
    top_function();
    return 0;
}

// Output:
// Stack trace:
//   top_function()
//   middle_function()
//   deep_function()
```

### Stacktrace in Error Handling

```cpp
#include <stacktrace>
#include <exception>

class TracedException : public std::exception {
    std::stacktrace trace;
    std::string message;
    
public:
    TracedException(const std::string& msg) 
        : trace(std::stacktrace::current()), message(msg) {}
    
    const char* what() const noexcept override {
        return message.c_str();
    }
    
    const std::stacktrace& get_trace() const {
        return trace;
    }
};

// Usage
void operation() {
    if (error_condition) {
        throw TracedException("Operation failed");
    }
}

try {
    operation();
} catch (const TracedException& e) {
    std::println("Error: {}", e.what());
    std::println("Trace: {}", e.get_trace());
}
```

---

# SECTION 8: CONSTEXPR ENHANCEMENTS

## 8.1 More Compile-Time Capabilities

### constexpr std::string

```cpp
#include <string>

// C++23: Can use std::string in constexpr context
constexpr std::string concat(const char* a, const char* b) {
    std::string result = a;
    result += b;
    return result;
}

constexpr auto msg = concat("Hello", " World");
// msg = "Hello World" at compile-time
```

### constexpr vector Operations

```cpp
#include <vector>

// C++23: vector works in constexpr
constexpr std::vector<int> make_sequence(int n) {
    std::vector<int> result;
    for (int i = 0; i < n; i++) {
        result.push_back(i);
    }
    return result;
}

constexpr auto seq = make_sequence(5);
// seq = {0, 1, 2, 3, 4} at compile-time
```

### constexpr Algorithms

```cpp
#include <algorithm>

// C++23: Algorithms work in constexpr
constexpr int compute() {
    std::vector<int> v{3, 1, 4, 1, 5, 9};
    std::sort(v.begin(), v.end());
    int sum = 0;
    for (int x : v) sum += x;
    return sum;
}

constexpr int result = compute();  // Computed at compile-time
```

---

# SECTION 9: ADAPTOR IMPROVEMENTS

## 9.1 Ranges::to Conversion

`std::ranges::to` converts ranges to containers.

### Basic ranges::to

```cpp
#include <ranges>
#include <vector>
#include <string>

vector<int> v = {1, 2, 3, 4, 5};

// Convert filtered range to vector
auto evens = v
    | std::ranges::views::filter([](int x) { return x % 2 == 0; })
    | std::ranges::to<std::vector>();

// Or with map
map<int, int> m{{1, 10}, {2, 20}, {3, 30}};
auto keys = m
    | std::ranges::views::keys
    | std::ranges::to<std::vector>();
```

### ranges::to with Construction Args

```cpp
#include <ranges>
#include <set>

vector<int> v = {3, 1, 4, 1, 5, 9, 2, 6};

// Convert to sorted unique set
auto unique_sorted = v
    | std::ranges::to<std::set>();

// Or to deque
auto d = v | std::ranges::to<std::deque>();

// With custom construction
auto s = v
    | std::ranges::to<std::set>(std::greater{});  // Descending
```

---

# SECTION 10: LIBRARY IMPROVEMENTS

## 10.1 Utility Improvements

### std::out_ptr & std::inout_ptr

```cpp
#include <memory>

// For converting unique_ptr to C API
unique_ptr<int> ptr;

// Before C++23: Complicated
// int* tmp = ptr.release();
// legacy_function(&tmp);
// ptr.reset(tmp);

// C++23: Simple
legacy_c_function(std::out_ptr(ptr));

// For bidirectional
inout_ptr(ptr);
```

### std::move_iterator Improvements

```cpp
#include <iterator>
#include <algorithm>

vector<string> v = {"a", "b", "c"};

// C++23: Cleaner move semantics
auto result = v
    | std::views::transform([](auto& s) { return std::move(s); });
```

### Bit Manipulation Improvements

```cpp
#include <bit>

unsigned int x = 5;  // 0b0101

// C++23 additions
std::byteswap(x);              // Byte swap
std::has_single_bit(x);        // Check if power of 2
std::bit_width(x);             // Bits needed
std::popcount(x);              // Count 1 bits
```

---

# SECTION 11: ATTRIBUTES & FEATURES

## 11.1 [[assume]] Attribute

`[[assume]]` allows providing hints to optimizer.

### Using assume

```cpp
void process(int x) {
    // Tell compiler to assume x > 0 (for optimization)
    [[assume(x > 0)]];
    
    // Compiler can optimize based on this
    if (x < 0) {
        // This branch won't be taken
        std::cout << "Negative\n";
    }
}

// Useful for performance-critical code
int* find_first(int* arr, int size) {
    [[assume(size > 0)]];  // Array has elements
    
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) return &arr[i];
    }
    return nullptr;
}
```

## 11.2 [[stdcall]] and ABI Attributes

```cpp
// Platform-specific calling conventions
#ifdef _WIN32
void __stdcall legacy_function() { }
[[gnu::stdcall]] void c_function();
#endif
```

---

# SECTION 12: STANDARD LIBRARY ADDITIONS

## 12.1 Container & Utility Additions

### std::debug_assert (Conditional Assertion)

```cpp
void function(int x) {
    // Debug assertion (disabled in release)
    _ASSERT(x > 0, "x must be positive");
    
    // Work with x
}
```

### std::repeat_view

```cpp
#include <ranges>

// Repeat a value
auto repeated = std::views::repeat(42, 5);
for (int x : repeated) {
    cout << x << " ";  // 42 42 42 42 42
}
```

### std::stride_view

```cpp
#include <ranges>

vector<int> v = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

// Take every Nth element
auto every_other = v | std::views::stride(2);
for (int x : every_other) {
    cout << x << " ";  // 0 2 4 6 8
}
```

### std::chunk_by

```cpp
#include <ranges>

vector<int> v = {1, 2, 2, 3, 3, 3, 4, 4};

// Group by predicate
auto chunks = v | std::views::chunk_by(std::equal_to{});
for (auto chunk : chunks) {
    cout << "[";
    for (int x : chunk) cout << x << " ";
    cout << "] ";
}
// Output: [1 ] [2 2 ] [3 3 3 ] [4 4 ]
```

### std::flat_map and std::flat_set

`std::flat_map` is a container adaptor that stores elements in sorted order in contiguous memory (like a vector).

```cpp
#include <flat_map>
#include <iostream>
#include <string>
#include <vector>

int main() {
    // Stores keys and values in separate vectors
    // Cache-friendly, fast iteration, binary search
    std::flat_map<int, std::string> map;
    
    map[1] = "one";
    map[3] = "three";
    map[2] = "two";  // Inserted in correct sorted position
    
    for (const auto& [key, val] : map) {
        std::cout << key << ": " << val << "\n";
    }
    // Output: 1: one, 2: two, 3: three
    
    return 0;
}
```

### std::generator (Synchronous Coroutine)

`std::generator` is the standard coroutine generator for synchronous sequences.

```cpp
#include <generator>
#include <iostream>
#include <ranges>

std::generator<int> fib(int n) {
    int a = 0, b = 1;
    while (n-- > 0) {
        co_yield a;
        auto next = a + b;
        a = b;
        b = next;
    }
}

int main() {
    for (int x : fib(10)) {
        std::cout << x << " ";
    }
    // Output: 0 1 1 2 3 5 8 13 21 34
    
    // Composable with ranges
    auto gen = fib(100) | std::views::filter([](int x) { return x % 2 == 0; });
    
    return 0;
}
```

### 12.3 Generator Internals

`std::generator` is a coroutine that:
1.  **Suspends on yield**: `co_yield` suspends execution and returns value to caller.
2.  **Promise Type**: Handles the `yield_value` call.
3.  **Recursive**: Can `co_yield` another generator (unlike basic coroutines).

```cpp
// Pseudocode of how it works
struct promise_type {
    int current_value;
    std::suspend_always yield_value(int value) {
        current_value = value;
        return {}; // Suspend
    }
};
```

---

# SECTION 13: C++23 BEST PRACTICES

## What's Better with C++23

```cpp
// 1. Use std::print for simple output
std::println("Value: {}", value);

// 2. Use std::expected for error handling
expected<int, Error> result = operation();

// 3. Use deducing this for simpler overloads
auto get(this auto& self) { return self.value; }

// 4. Use range-based for with init
for (auto data = load_data(); auto item : data) { }

// 5. Use multi-index subscript
matrix[row, col] = value;

// 6. Use std::stacktrace for debugging
auto trace = std::stacktrace::current();

// 7. Use constexpr string/vector
constexpr auto msg = std::string("hello");

// 8. Use std::ranges::to for conversions
auto set_result = v | std::ranges::to<std::set>();

// 9. Use [[assume]] for optimization hints
[[assume(pointer != nullptr)]];

// 10. Use monadic operations on optional
opt.transform([](int x) { return x * 2; });
```

---

## <a name="chapter-13-thefuturec26preview"></a>CHAPTER 13: THE FUTURE - C++26 PREVIEW

As of 2026, the C++26 standard is nearing finalization. Here are the transformative features likely to be included.

### 13.1 Static Reflection (std::meta)
Reflection allows a program to inspect and modify itself at compile-time. This eliminates the need for external code generators or macros for serialization, ORMs, and enum-to-string conversions.

```cpp
#include <meta>
#include <iostream>
#include <string_view>

struct Person {
    std::string name;
    int age;
    double salary;
};

// Generic serialization using C++26 Reflection
template<typename T>
void serialize(const T& obj) {
    constexpr auto type_info = ^T; // Reflection operator
    
    template for (constexpr auto member : std::meta::members_of(type_info)) {
        std::cout << std::meta::name_of(member) << ": " 
                  << obj.[:member:] << "\n"; // Splicing
    }
}

int main() {
    Person p{"Alice", 30, 95000.0};
    serialize(p); 
    // Output:
    // name: Alice
    // age: 30
    // salary: 95000
}
```

### 13.2 Contracts
Contracts provide a standardized way to specify preconditions, postconditions, and assertions, improving safety and optimizer information.

```cpp
// pre: Precondition (Caller must ensure)
// post: Postcondition (Function ensures upon return)
// assert: Internal check

int safe_divide(int a, int b) 
    pre { b != 0 }             // Contract: b must not be zero
    post(r) { r * b == a }     // Contract: result * divisor equals dividend
{
    return a / b;
}

// Modes:
// - enforce: Terminate if violated
// - observe: Log/Debug but continue
// - ignore: Optimizer hint (assume true)
```

### 13.3 Senders & Receivers (std::execution)
A unified framework for asynchronous execution, replacing raw threads, futures, and callbacks with a composable pipeline model.

```cpp
#include <execution>
#include <iostream>

using namespace std::execution;

int main() {
    scheduler auto sch = thread_pool_scheduler{};

    sender auto work = schedule(sch)
        | then([]{ return 42; })
        | then([](int i){ return i * 2; })
        | then([](int i){ std::cout << "Result: " << i << "\n"; });

    // Launch execution
    std::this_thread::sync_wait(std::move(work));
    
    return 0;
}
```

### 13.4 Linear Algebra (std::linalg)
Standardized BLAS (Basic Linear Algebra Subprograms) support for high-performance math.

```cpp
#include <linalg>
#include <mdspan>
#include <vector>

int main() {
    std::vector<double> A_vec(9), B_vec(3), C_vec(3);
    // ... fill vectors ...

    std::mdspan A(A_vec.data(), 3, 3);
    std::mdspan B(B_vec.data(), 3);
    std::mdspan C(C_vec.data(), 3);

    // Matrix-Vector Multiplication: C = A * B
    std::linalg::matrix_vector_product(A, B, C);
    
    return 0;
}
```

---

# Volume IV: Systems & Architecture


---
### Professional Notes: Low Level & Safety

#### Chapter 5: Bit Operators

Section 5.1: | - bitwise OR
int a = 5;     // 0101b  (0x05)
int b = 12;    // 1100b  (0x0C)
int c = a | b; // 1101b  (0x0D)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 5, b = 12, c = 13
Why
A bit wise OR operates on the bit level and uses the following Boolean truth table:
true OR true = true
true OR false = true
false OR false = false
When the binary value for a (0101) and the binary value for b (1100) are OR'ed together we get the binary value of
1101:
int a = 0 1 0 1
int b = 1 1 0 0 |
        ---------
int c = 1 1 0 1
The bit wise OR does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator |=:
int a = 5;  // 0101b  (0x05)
a |= 12;    // a = 0101b | 1101b
Section 5.2: ^ - bitwise XOR (exclusive OR)
int a = 5;     // 0101b  (0x05)
int b = 9;     // 1001b  (0x09)
int c = a ^ b; // 1100b  (0x0C)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 5, b = 9, c = 12
Why
A bit wise XOR (exclusive or) operates on the bit level and uses the following Boolean truth table:
true OR true = false
true OR false = true
false OR false = false
Notice that with an XOR operation true OR true = false where as with operations true AND/OR true = true,
hence the exclusive nature of the XOR operation.
Using this, when the binary value for a (0101) and the binary value for b (1001) are XOR'ed together we get the binary
value of 1100:
int a = 0 1 0 1
int b = 1 0 0 1 ^
        ---------
int c = 1 1 0 0
The bit wise XOR does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator ^=:
int a = 5;  // 0101b  (0x05)
a ^= 9;    // a = 0101b ^ 1001b
The bit wise XOR can be utilized in many ways and is often utilized in bit mask operations for encryption and
compression.
Note: The following example is often shown as an example of a nice trick. But should not be used in production
code (there are better ways std::swap() to achieve the same result).
You can also utilize an XOR operation to swap two variables without a temporary:
int a = 42;
int b = 64;
// XOR swap
a ^= b;
b ^= a;
a ^= b;
std::cout << "a = " << a << ", b = " << b << "\n";
To productionalize this you need to add a check to make sure it can be used.
void doXORSwap(int& a, int& b)
{
    // Need to add a check to make sure you are not swapping the same
    // variable with itself. Otherwise it will zero the value.
    if (&a != &b)
    {
        // XOR swap
        a ^= b;
        b ^= a;
        a ^= b;
    }
}
So though it looks like a nice trick in isolation it is not useful in real code. xor is not a base logical operation,but a
combination of others: a^c=~(a&c)&(a|c)
also in 2015+ compilers variables may be assigned as binary:
int cn=0b0111;
Section 5.3: & - bitwise AND
int a = 6;     // 0110b  (0x06)
int b = 10;    // 1010b  (0x0A)
int c = a & b; // 0010b  (0x02)
std::cout << "a = " << a << ", b = " << b << ", c = " << c << std::endl;
Output
a = 6, b = 10, c = 2
Why
A bit wise AND operates on the bit level and uses the following Boolean truth table:
TRUE  AND TRUE  = TRUE
TRUE  AND FALSE = FALSE
FALSE AND FALSE = FALSE
When the binary value for a (0110) and the binary value for b (1010) are AND'ed together we get the binary value of
0010:
int a = 0 1 1 0
int b = 1 0 1 0 &
        ---------
int c = 0 0 1 0
The bit wise AND does not change the value of the original values unless speciﬁcally assigned to using the bit wise
assignment compound operator &=:
int a = 5;  // 0101b  (0x05)
a &= 10;    // a = 0101b & 1010b
Section 5.4: << - left shift
int a = 1;      // 0001b
int b = a << 1; // 0010b
std::cout << "a = " << a << ", b = " << b << std::endl;
Output
a = 1, b = 2
Why
The left bit wise shift will shift the bits of the left hand value (a) the number speciﬁed on the right (1), essentially
padding the least signiﬁcant bits with 0's, so shifting the value of 5 (binary 0000 0101) to the left 4 times (e.g. 5 <<
4) will yield the value of 80 (binary 0101 0000). You might note that shifting a value to the left 1 time is also the same
as multiplying the value by 2, example:
int a = 7;
while (a < 200) {
    std::cout << "a = " << a << std::endl;
    a <<= 1;
}
a = 7;
while (a < 200) {
    std::cout << "a = " << a << std::endl;
    a *= 2;
}
But it should be noted that the left shift operation will shift all bits to the left, including the sign bit, example:
int a = 2147483647; // 0111 1111 1111 1111 1111 1111 1111 1111
int b = a << 1;     // 1111 1111 1111 1111 1111 1111 1111 1110
std::cout << "a = " << a << ", b = " << b << std::endl;
Possible output: a = 2147483647, b = -2
While some compilers will yield results that seem expected, it should be noted that if you left shift a signed number
so that the sign bit is aﬀected, the result is undeﬁned. It is also undeﬁned if the number of bits you wish to shift by
is a negative number or is larger than the number of bits the type on the left can hold, example:
int a = 1;
int b = a << -1;  // undefined behavior
char c = a << 20; // undefined behavior
The bit wise left shift does not change the value of the original values unless speciﬁcally assigned to using the bit
wise assignment compound operator <<=:
int a = 5;  // 0101b
a <<= 1;    // a = a << 1;
Section 5.5: >> - right shift
int a = 2;      // 0010b
int b = a >> 1; // 0001b
std::cout << "a = " << a << ", b = " << b << std::endl;
Output
a = 2, b = 1
Why
The right bit wise shift will shift the bits of the left hand value (a) the number speciﬁed on the right (1); it should be
noted that while the operation of a right shift is standard, what happens to the bits of a right shift on a signed
negative number is implementation deﬁned and thus cannot be guaranteed to be portable, example:
int a = -2;    
int b = a >> 1; // the value of b will be depend on the compiler
It is also undeﬁned if the number of bits you wish to shift by is a negative number, example:
int a = 1;
int b = a >> -1;  // undefined behavior
The bit wise right shift does not change the value of the original values unless speciﬁcally assigned to using the bit
wise assignment compound operator >>=:
int a = 2;  // 0010b
a >>= 1;    // a = a >> 1;

#### Chapter 6: Bit Manipulation

Section 6.1: Remove rightmost set bit
C-style bit-manipulation
template <typename T>
T rightmostSetBitRemoved(T n)
{
    // static_assert(std::is_integral<T>::value && !std::is_signed<T>::value, "type should be
unsigned"); // For c++11 and later
    return n & (n - 1);
}
Explanation
if n is zero, we have 0 & 0xFF..FF which is zero
else n can be written 0bxxxxxx10..00 and n - 1 is 0bxxxxxx011..11, so n & (n - 1) is 0bxxxxxx000..00.
Section 6.2: Set all bits
C-style bit-manipulation
x = -1; // -1 == 1111 1111 ... 1111b
(See here for an explanation of why this works and is actually the best approach.)
Using std::bitset
std::bitset<10> x;
x.set(); // Sets all bits to '1'
Section 6.3: Toggling a bit
C-style bit-manipulation
A bit can be toggled using the XOR operator (^).
// Bit x will be the opposite value of what it is currently
number ^= 1LL << x;
Using std::bitset
std::bitset<4> num(std::string("0100"));
num.flip(2); // num is now 0000
num.flip(0); // num is now 0001
num.flip();  // num is now 1110 (flips all bits)
Section 6.4: Checking a bit
C-style bit-manipulation
The value of the bit can be obtained by shifting the number to the right x times and then performing bitwise AND
(&) on it:
(number >> x) & 1LL;  // 1 if the 'x'th bit of 'number' is set, 0 otherwise
The right-shift operation may be implemented as either an arithmetic (signed) shift or a logical (unsigned) shift. If
number in the expression number >> x has a signed type and a negative value, the resulting value is
implementation-deﬁned.
If we need the value of that bit directly in-place, we could instead left shift the mask:
(number & (1LL << x));  // (1 << x) if the 'x'th bit of 'number' is set, 0 otherwise
Either can be used as a conditional, since all non-zero values are considered true.
Using std::bitset
std::bitset<4> num(std::string("0010"));
bool bit_val = num.test(1);  // bit_val value is set to true;
Section 6.5: Counting bits set
The population count of a bitstring is often needed in cryptography and other applications and the problem has
been widely studied.
The naive way requires one iteration per bit:
unsigned value = 1234;
unsigned bits = 0;  // accumulates the total number of bits set in `n`
for (bits = 0; value; value >>= 1)
  bits += value & 1;
A nice trick (based on Remove rightmost set bit ) is:
unsigned bits = 0;  // accumulates the total number of bits set in `n`
for (; value; ++bits)
  value &= value - 1;
It goes through as many iterations as there are set bits, so it's good when value is expected to have few nonzero
bits.
The method was ﬁrst proposed by Peter Wegner (in CACM 3 / 322 - 1960) and it's well known since it appears in C
Programming Language by Brian W. Kernighan and Dennis M. Ritchie.
This requires 12 arithmetic operations, one of which is a multication:
unsigned popcount(std::uint64_t x)
{
  const std::uint64_t m1  = 0x5555555555555555;  // binary: 0101...
  const std::uint64_t m2  = 0x3333333333333333;  // binary: 00110011..
  const std::uint64_t m4  = 0x0f0f0f0f0f0f0f0f;  // binary: 0000111100001111
  x -= (x >> 1) & m1;             // put count of each 2 bits into those 2 bits
  x = (x & m2) + ((x >> 2) & m2); // put count of each 4 bits into those 4 bits
  x = (x + (x >> 4)) & m4;        // put count of each 8 bits into those 8 bits
  return (x * h01) >> 56;  // left 8 bits of x + (x<<8) + (x<<16) + (x<<24) + ...
}
This kind of implementation has the best worst-case behavior (see Hamming weight for further details).
Many CPUs have a speciﬁc instruction (like x86's popcnt) and the compiler could oﬀer a speciﬁc (non standard)
built in function. E.g. with g++ there is:
int __builtin_popcount (unsigned x);
Section 6.6: Check if an integer is a power of 2
The n & (n - 1) trick (see Remove rightmost set bit) is also useful to determine if an integer is a power of 2:
bool power_of_2 = n && !(n & (n - 1));
Note that without the ﬁrst part of the check (n &&), 0 is incorrectly considered a power of 2.
Section 6.7: Setting a bit
C-style bit manipulation
A bit can be set using the bitwise OR operator (|).
// Bit x will be set
number |= 1LL << x;
Using std::bitset
set(x) or set(x,true) - sets bit at position x to 1.
std::bitset<5> num(std::string("01100"));
num.set(0);      // num is now 01101
num.set(2);      // num is still 01101
num.set(4,true); // num is now 11110
Section 6.8: Clearing a bit
C-style bit-manipulation
A bit can be cleared using the bitwise AND operator (&).
// Bit x will be cleared
number &= ~(1LL << x);
Using std::bitset
reset(x) or set(x,false) - clears the bit at position x.
std::bitset<5> num(std::string("01100"));
num.reset(2);     // num is now 01000
num.reset(0);     // num is still 01000
num.set(3,false); // num is now 00000
Section 6.9: Changing the nth bit to x
C-style bit-manipulation
// Bit n will be set if x is 1 and cleared if x is 0.
number ^= (-x ^ number) & (1LL << n);
Using std::bitset
set(n,val) - sets bit n to the value val.
std::bitset<5> num(std::string("00100"));
num.set(0,true);  // num is now 00101
num.set(2,false); // num is now 00001
Section 6.10: Bit Manipulation Application: Small to Capital
Letter
One of several applications of bit manipulation is converting a letter from small to capital or vice versa by choosing
a mask and a proper bit operation. For example, the a letter has this binary representation 01(1)00001 while its
capital counterpart has 01(0)00001. They diﬀer solely in the bit in parenthesis. In this case, converting the a letter
from small to capital is basically setting the bit in parenthesis to one. To do so, we do the following:
/****************************************
convert small letter to captial letter.
========================================
     a: 01100001
  mask: 11011111  <-- (0xDF)  11(0)11111
      :---------
a&mask: 01000001  <-- A letter
*****************************************/
The code for converting a letter to A letter is
#include <cstdio>
int main()
{
    char op1 = 'a';  // "a" letter (i.e. small case)
    int mask = 0xDF; // choosing a proper mask
    printf("a (AND) mask = A\n");
    printf("%c   &   0xDF = %c\n", op1, op1 & mask);
    return 0;
}
The result is
$ g++ main.cpp -o test1
$ ./test1
a (AND) mask = A
a   &   0xDF = A

#### Chapter 104: Undeﬁned Behavior

Section 104.1: Reading or writing through a null pointer 
Section 104.2: Using an uninitialized local variable 
Section 104.3: Accessing an out-of-bounds index 
Section 104.4: Deleting a derived object via a pointer to a base class that doesn't have a virtual destructor
Section 104.5: Extending the `std` or `posix` Namespace 
Section 104.6: Invalid pointer arithmetic 
Section 104.7: No return statement for a function with a non-void return type 
Section 104.8: Accessing a dangling reference 
Section 104.9: Integer division by zero 
Section 104.10: Shifting by an invalid number of positions 
Section 104.11: Incorrect pairing of memory allocation and deallocation 
Section 104.12: Signed Integer Overﬂow 
Section 104.13: Multiple non-identical deﬁnitions (the One Deﬁnition Rule) 
Section 104.14: Modifying a const object 
Section 104.15: Returning from a [[noreturn]] function 
Section 104.16: Inﬁnite template recursion 
Section 104.17: Overﬂow during conversion to or from ﬂoating point type 
Section 104.18: Modifying a string literal 
Section 104.19: Accessing an object as the wrong type 
Section 104.20: Invalid derived-to-base conversion for pointers to members 
Section 104.21: Destroying an object that has already been destroyed 
Section 104.22: Access to nonexistent member through pointer to member 
Section 104.23: Invalid base-to-derived static cast 
Section 104.24: Floating point overﬂow 
Section 104.25: Calling (Pure) Virtual Members From Constructor Or Destructor 
Section 104.26: Function call through mismatched function pointer type 

## <a name="chapter-14-advancedtopics"></a>CHAPTER 14: ADVANCED TOPICS

## TEMPLATE METAPROGRAMMING

## 1.1 Compile-Time Computation

Template metaprogramming enables computation at compile-time.

### Factorial at Compile-Time

```cpp
#include <iostream>
using namespace std;

// Base case
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

// Specialization (base case)
template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

int arr[Factorial<5>::value];  // Array of size 120!

cout << Factorial<10>::value << "\n";  // 3628800
```

### Fibonacci at Compile-Time

```cpp
template<int N>
struct Fib {
    static constexpr int value = Fib<N-1>::value + Fib<N-2>::value;
};

template<>
struct Fib<0> {
    static constexpr int value = 0;
};

template<>
struct Fib<1> {
    static constexpr int value = 1;
};

// Memoization to avoid exponential time
template<int N>
struct FibMemo {
    static constexpr int value = FibMemo<N-1>::value + FibMemo<N-2>::value;
};

static constexpr int fib20 = FibMemo<20>::value;  // Computed at compile-time
```

### Compile-Time Power Check

```cpp
template<unsigned int N>
struct IsPowerOfTwo {
    static constexpr bool value = (N > 0) && ((N & (N - 1)) == 0);
};

static_assert(IsPowerOfTwo<16>::value);
static_assert(!IsPowerOfTwo<7>::value);
```

---

## 1.2 Template Specialization

### Full Specialization

```cpp
// Primary template
template<typename T, typename U>
struct Pair {
    static void info() { cout << "Generic pair\n"; }
};

// Full specialization
template<>
struct Pair<int, string> {
    static void info() { cout << "int-string pair\n"; }
};

// Full specialization for pointers
template<typename T>
struct Pair<T*, T*> {
    static void info() { cout << "Two pointers\n"; }
};

Pair<int, string>::info();     // "int-string pair"
Pair<double, string>::info();  // "Generic pair"
Pair<int*, int*>::info();      // "Two pointers"
```

### Partial Specialization

```cpp
// Primary
template<typename T, typename U>
struct Container {
    static void type() { cout << "Generic\n"; }
};

// Partial specialization - same types
template<typename T>
struct Container<T, T> {
    static void type() { cout << "Same type\n"; }
};

// Partial specialization - pointers
template<typename T, typename U>
struct Container<T*, U*> {
    static void type() { cout << "Two pointers\n"; }
};

// Partial specialization - array
template<typename T, int N>
struct Container<T[N], T> {
    static void type() { cout << "Array and element\n"; }
};

Container<int, int>::type();           // "Same type"
Container<int*, double*>::type();      // "Two pointers"
Container<int[5], int>::type();        // "Array and element"
```

## 1.3 Advanced Metaprogramming Patterns

### Typelists
A list of types at compile-time, essential for ECS and Variant implementation.

```cpp
template<typename... Ts>
struct TypeList {};

// Length of list
template<typename List> struct Length;

template<typename... Ts>
struct Length<TypeList<Ts...>> {
    static constexpr size_t value = sizeof...(Ts);
};

// Access type at index
template<size_t N, typename List> struct At;

template<typename Head, typename... Tail>
struct At<0, TypeList<Head, Tail...>> {
    using type = Head;
};

template<size_t N, typename Head, typename... Tail>
struct At<N, TypeList<Head, Tail...>> {
    using type = typename At<N-1, TypeList<Tail...>>::type;
};

// Usage
using MyTypes = TypeList<int, float, double>;
static_assert(Length<MyTypes>::value == 3);
static_assert(std::is_same_v<At<1, MyTypes>::type, float>);
```

---

# SECTION 2: SFINAE & TYPE TRAITS

## 2.1 SFINAE - Substitution Failure Is Not An Error

SFINAE enables overload resolution based on template parameter substitution.

### Basic SFINAE

```cpp
#include <type_traits>

// Enable if T is integral
template<typename T>
enable_if_t<is_integral_v<T>>
process(T x) {
    cout << "Integer: " << x << "\n";
}

// Enable if T is floating point
template<typename T>
enable_if_t<is_floating_point_v<T>>
process(T x) {
    cout << "Float: " << x << "\n";
}

process(42);      // "Integer: 42"
process(3.14);    // "Float: 3.14"
```

### Detector Pattern

```cpp
// Detect if type has value_type
template<typename T, typename = void>
struct HasValueType : false_type {};

template<typename T>
struct HasValueType<T, void_t<typename T::value_type>> : true_type {};

// Usage
static_assert(HasValueType<vector<int>>::value);
static_assert(!HasValueType<int>::value);
```

### Advanced SFINAE with Multiple Conditions

```cpp
template<typename T>
enable_if_t<
    is_copy_constructible_v<T> &&
    is_move_constructible_v<T> &&
    is_equality_comparable_v<T>
>
smart_copy(const T& src, T& dst) {
    dst = src;
}
```

---

## 2.2 Type Traits Mastery

### Custom Type Traits

```cpp
// Check if type has a begin() and end()
template<typename T, typename = void>
struct IsContainer : false_type {};

template<typename T>
struct IsContainer<T, void_t<
    decltype(declval<T>().begin()),
    decltype(declval<T>().end())
>> : true_type {};

// Check if callable with specific signature
template<typename F, typename... Args>
struct IsCallable : false_type {};

template<typename F, typename... Args>
struct IsCallable<F, 
    void_t<decltype(declval<F>()(declval<Args>()...))>
> : true_type {};

// Usage
static_assert(IsContainer<vector<int>>::value);
static_assert(!IsContainer<int>::value);

auto lambda = [](int x) { return x; };
static_assert(IsCallable<decltype(lambda), int>::value);
```

---

# SECTION 3: EXPRESSION TEMPLATES

## 3.1 Lazy Evaluation Pattern

Expression templates defer computation until needed.

### Vector Operations

```cpp
#include <vector>
#include <iostream>

// Expression template for vector operations
template<typename Expr>
class VectorExpr {
public:
    double operator[](int i) const {
        return static_cast<const Expr&>(*this)[i];
    }
    
    int size() const {
        return static_cast<const Expr&>(*this).size();
    }
};

class Vector : public VectorExpr<Vector> {
private:
    std::vector<double> data;
    
public:
    Vector(int n) : data(n) {}
    
    double operator[](int i) const { return data[i]; }
    int size() const { return data.size(); }
    double& operator[](int i) { return data[i]; }
};

// Addition expression (no computation yet)
template<typename L, typename R>
class VectorSum : public VectorExpr<VectorSum<L, R>> {
private:
    const L& lhs;
    const R& rhs;
    
public:
    VectorSum(const L& l, const R& r) : lhs(l), rhs(r) {}
    
    double operator[](int i) const {
        return lhs[i] + rhs[i];
    }
    
    int size() const { return lhs.size(); }
};

// Operator overloading creates expression tree
template<typename L, typename R>
VectorSum<L, R> operator+(const VectorExpr<L>& l, const VectorExpr<R>& r) {
    return VectorSum<L, R>(static_cast<const L&>(l), static_cast<const R&>(r));
}

// Scalar multiplication
template<typename Expr>
class VectorScale : public VectorExpr<VectorScale<Expr>> {
private:
    const Expr& expr;
    double scale;
    
public:
    VectorScale(const Expr& e, double s) : expr(e), scale(s) {}
    
    double operator[](int i) const {
        return expr[i] * scale;
    }
    
    int size() const { return expr.size(); }
};

template<typename Expr>
VectorScale<Expr> operator*(double s, const VectorExpr<Expr>& e) {
    return VectorScale<Expr>(static_cast<const Expr&>(e), s);
}

// Usage
int main() {
    Vector v1(3), v2(3), v3(3);
    v1[0] = 1; v1[1] = 2; v1[2] = 3;
    v2[0] = 4; v2[1] = 5; v2[2] = 6;
    
    // No computation yet!
    auto expr = v1 + v2 + 2.0 * v3;
    
    // Computation happens here
    for (int i = 0; i < 3; i++) {
        cout << expr[i] << " ";
    }
    
    return 0;
}
```

## 3.2 Triggering Computation (Assignment)

To make `v3 = v1 + v2` work efficiently, we add an assignment operator to `Vector` that takes any `VectorExpr`.

```cpp
// Inside class Vector
template <typename Expr>
Vector& operator=(const VectorExpr<Expr>& expr) {
    if (size() != expr.size()) {
        // resize...
    }
    for (int i = 0; i < size(); ++i) {
        data[i] = expr[i]; // Evaluates tree at index i
    }
    return *this;
}
```
**Why is this fast?**
It expands to: `v3[i] = v1[i] + v2[i]`.
There are **zero temporary vectors** created. No allocations. Just one loop.

---

# SECTION 4: POLICY-BASED DESIGN

## 4.1 Template Policies

Policy-based design separates concerns using template parameters.

### Thread Safety Policy

```cpp
#include <mutex>
#include <memory>

// Policy: No synchronization
struct NoSync {
    void lock() {}
    void unlock() {}
};

// Policy: Mutex-based
struct MutexSync {
private:
    mutable std::mutex m;
    
public:
    void lock() { m.lock(); }
    void unlock() { m.unlock(); }
};

// Generic container with synchronization policy
template<typename T, typename SyncPolicy = NoSync>
class SafeVector : private SyncPolicy {
private:
    std::vector<T> data;
    
public:
    void push_back(const T& value) {
        this->lock();
        data.push_back(value);
        this->unlock();
    }
    
    T pop_back() {
        this->lock();
        T value = data.back();
        data.pop_back();
        this->unlock();
        return value;
    }
};

// Usage
SafeVector<int> single_threaded;  // No synchronization
SafeVector<int, MutexSync> multi_threaded;  // Thread-safe
```

### Storage Policy

```cpp
// Policy: Dynamic allocation
struct DynamicStorage {
    template<typename T>
    using Allocator = std::allocator<T>;
};

// Policy: Static allocation
template<int MaxSize>
struct StaticStorage {
    // Allocate from stack
};

template<typename T, typename StoragePolicy>
class Container : private StoragePolicy {
private:
    std::vector<T> data;
};
```

## 4.2 Policy-Based Smart Pointer (Advanced Example)

A smart pointer is defined by:
1.  **Storage**: How it stores the pointer (raw vs compressed).
2.  **Ownership**: Ref-counted vs Unique vs Linked.
3.  **Checking**: Assert on access vs No check.

```cpp
template <
    class T,
    template <class> class CheckingPolicy,
    template <class> class ThreadingPolicy
>
class SmartPtr : public CheckingPolicy<T>, public ThreadingPolicy<T> {
    T* ptr;
public:
    T* operator->() {
        CheckingPolicy<T>::check(ptr);
        ThreadingPolicy<T>::lock(); // Fake lock example
        return ptr;
    }
};
```
This approach allows generating thousands of smart pointer variants with zero runtime overhead (all resolved at compile-time).

---

# SECTION 5: MEMORY MANAGEMENT & OPTIMIZATION

## 5.1 Custom Allocators

```cpp
#include <memory>

template<typename T>
class ArenaAllocator {
private:
    static constexpr size_t ARENA_SIZE = 10000;
    char arena[ARENA_SIZE];
    char* current;
    
public:
    ArenaAllocator() : current(arena) {}
    
    T* allocate(size_t n) {
        if (current + n * sizeof(T) > arena + ARENA_SIZE) {
            throw std::bad_alloc();
        }
        T* ptr = reinterpret_cast<T*>(current);
        current += n * sizeof(T);
        return ptr;
    }
    
    void deallocate(T* p, size_t n) {
        // No deallocation for arena allocator
    }
};

// Usage
vector<int, ArenaAllocator<int>> fast_vector;
```

## 5.2 Small Object Optimization (SOO)

```cpp
template<typename T, size_t SmallSize = 32>
class SmallVector {
private:
    union {
        T* ptr;              // Dynamic allocation
        std::aligned_storage_t<SmallSize, alignof(T)> small;
    };
    
    size_t size_val;
    bool is_small;
    
public:
    SmallVector() : size_val(0), is_small(true) {}
    
    void push_back(const T& value) {
        if (size_val < SmallSize / sizeof(T)) {
            // Use small storage
            new (&small) T(value);
            is_small = true;
        } else {
            // Switch to dynamic
            if (is_small) {
                // Copy small to dynamic
                ptr = new T[size_val + 1];
                is_small = false;
            }
            ptr[size_val] = value;
        }
        size_val++;
    }
    
    ~SmallVector() {
        if (!is_small) {
            delete[] ptr;
        }
    }
};
```

---

# SECTION 6: CONCURRENCY & PARALLELISM

## 6.1 Advanced Threading Patterns

### Thread Pool

```cpp
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
private:
    vector<std::thread> workers;
    queue<std::function<void()>> tasks;
    mutex task_mutex;
    condition_variable cv;
    bool stop = false;
    
public:
    ThreadPool(size_t num_threads) {
        for (size_t i = 0; i < num_threads; i++) {
            workers.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    
                    {
                        unique_lock<mutex> lock(task_mutex);
                        cv.wait(lock, [this] { return !tasks.empty() || stop; });
                        
                        if (stop && tasks.empty()) return;
                        
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    
                    task();
                }
            });
        }
    }
    
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) {
        using return_type = std::invoke_result_t<F, Args...>;
        
        auto task = make_shared<std::packaged_task<return_type()>>(
            bind(forward<F>(f), forward<Args>(args)...)
        );
        
        auto result = task->get_future();
        
        {
            unique_lock<mutex> lock(task_mutex);
            tasks.emplace([task] { (*task)(); });
        }
        
        cv.notify_one();
        return result;
    }
    
    ~ThreadPool() {
        {
            unique_lock<mutex> lock(task_mutex);
            stop = true;
        }
        cv.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }
};

// Usage
ThreadPool pool(4);

auto future = pool.enqueue([](int x) { return x * 2; }, 42);
cout << future.get() << "\n";  // 84
```

### Lock-Free Queue

```cpp
#include <atomic>

template<typename T>
class LockFreeQueue {
private:
    struct Node {
        T value;
        atomic<Node*> next{nullptr};
    };
    
    atomic<Node*> head;
    atomic<Node*> tail;
    
public:
    LockFreeQueue() {
        auto dummy = new Node();
        head.store(dummy, memory_order_relaxed);
        tail.store(dummy, memory_order_relaxed);
    }
    
    void push(const T& value) {
        auto new_node = new Node{value};
        Node* old_tail = tail.load(memory_order_acquire);
        
        old_tail->next.store(new_node, memory_order_release);
        tail.store(new_node, memory_order_release);
    }
    
    bool pop(T& value) {
        Node* old_head = head.load(memory_order_acquire);
        Node* next = old_head->next.load(memory_order_acquire);
        
        if (next == nullptr) return false;
        
        value = next->value;
        head.store(next, memory_order_release);
        delete old_head;
        
        return true;
    }
};
```

---

# SECTION 7: TYPE ERASURE PATTERNS

## 7.1 Virtual Function-Based Type Erasure

```cpp
class AnyCallable {
private:
    struct Base {
        virtual ~Base() = default;
        virtual void call() = 0;
    };
    
    template<typename F>
    struct Derived : Base {
        F func;
        Derived(F f) : func(f) {}
        void call() override { func(); }
    };
    
    unique_ptr<Base> impl;
    
public:
    template<typename F>
    AnyCallable(F f) : impl(make_unique<Derived<F>>(f)) {}
    
    void operator()() {
        impl->call();
    }
};

// Usage
AnyCallable c1([](){ cout << "Lambda\n"; });
AnyCallable c2(std::bind(...));
AnyCallable c3(&function);

c1();  // "Lambda"
```

## 7.2 std::function Implementation

```cpp
#include <memory>

template<typename>
class Function;

template<typename R, typename... Args>
class Function<R(Args...)> {
private:
    struct Base {
        virtual ~Base() = default;
        virtual R call(Args...) = 0;
        virtual unique_ptr<Base> clone() = 0;
    };
    
    template<typename F>
    struct Derived : Base {
        F func;
        Derived(F f) : func(f) {}
        R call(Args... args) override {
            return func(args...);
        }
        unique_ptr<Base> clone() override {
            return make_unique<Derived>(*this);
        }
    };
    
    unique_ptr<Base> impl;
    
public:
    template<typename F>
    Function(F f) : impl(make_unique<Derived<F>>(f)) {}
    
    R operator()(Args... args) {
        return impl->call(args...);
    }
    
    Function(const Function& other) : impl(other.impl->clone()) {}
};
```

## 7.3 Concept-Based Type Erasure (C++20)

Instead of inheritance, we can erase types that satisfy a concept.

```cpp
#include <concepts>
#include <memory>

template<typename T>
concept Drawable = requires(T x) { x.draw(); };

class AnyDrawable {
    struct Concept {
        virtual ~Concept() = default;
        virtual void draw() = 0;
        virtual std::unique_ptr<Concept> clone() = 0;
    };

    template<Drawable T>
    struct Model : Concept {
        T data;
        Model(T x) : data(std::move(x)) {}
        void draw() override { data.draw(); }
        std::unique_ptr<Concept> clone() override { return std::make_unique<Model>(data); }
    };

    std::unique_ptr<Concept> pimpl;

public:
    template<Drawable T>
    AnyDrawable(T x) : pimpl(std::make_unique<Model<T>>(std::move(x))) {}
    
    AnyDrawable(const AnyDrawable& other) : pimpl(other.pimpl->clone()) {}
    
    void draw() { pimpl->draw(); }
};
```

---

# SECTION 8: CRTP (CURIOUSLY RECURRING TEMPLATE PATTERN)

## 8.1 Static Polymorphism

```cpp
// Base class template
template<typename Derived>
class Shape {
public:
    void draw() {
        static_cast<Derived*>(this)->draw_impl();
    }
    
    double area() {
        return static_cast<Derived*>(this)->area_impl();
    }
};

// Derived classes
class Circle : public Shape<Circle> {
private:
    double radius;
    
public:
    Circle(double r) : radius(r) {}
    
    void draw_impl() {
        cout << "Drawing circle\n";
    }
    
    double area_impl() {
        return 3.14159 * radius * radius;
    }
};

class Square : public Shape<Square> {
private:
    double side;
    
public:
    Square(double s) : side(s) {}
    
    void draw_impl() {
        cout << "Drawing square\n";
    }
    
    double area_impl() {
        return side * side;
    }
};

// Usage - no virtual function overhead!
Circle c(5);
c.draw();
cout << c.area() << "\n";

Square s(4);
s.draw();
cout << s.area() << "\n";
```

## 8.2 CRTP for Comparisons

```cpp
template<typename Derived>
class Comparable {
public:
    bool operator<(const Comparable& other) const {
        return static_cast<const Derived*>(this)->compare(
            static_cast<const Derived&>(other)
        ) < 0;
    }
    
    bool operator>(const Comparable& other) const {
        return other < *this;
    }
    
    bool operator<=(const Comparable& other) const {
        return !(other < *this);
    }
    
    bool operator>=(const Comparable& other) const {
        return !(*this < other);
    }
    
    bool operator==(const Comparable& other) const {
        return !(*this < other) && !(other < *this);
    }
    
    bool operator!=(const Comparable& other) const {
        return !(*this == other);
    }
};

class Value : public Comparable<Value> {
private:
    int val;
    
public:
    Value(int v) : val(v) {}
    
    int compare(const Value& other) const {
        return val - other.val;
    }
};

// Usage
Value v1(5), v2(10);
cout << (v1 < v2) << "\n";   // true
cout << (v1 == v2) << "\n";  // false
```

---

# SECTION 9: PERFECT FORWARDING & MOVE SEMANTICS

## 9.1 Forwarding Problems

```cpp
// The problem: losing information about lvalue/rvalue
template<typename T>
void bad_forward(T arg) {
    // arg is always lvalue
    sink(arg);  // Loses rvalue status
}

// Solution: universal references + std::forward
template<typename T>
void good_forward(T&& arg) {
    // Preserves lvalue/rvalue nature
    sink(std::forward<T>(arg));
}

// Double forwarding
template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));
}

template<typename T>
void double_wrapper(T&& arg) {
    wrapper(std::forward<T>(arg));
}
```

## 9.2 Move Semantics Implementation

```cpp
class String {
private:
    char* data;
    size_t size;
    
public:
    // Move constructor
    String(String&& other) noexcept 
        : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
    }
    
    // Move assignment
    String& operator=(String&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    // Copy constructor (for lvalues)
    String(const String& other)
        : size(other.size), data(new char[size + 1]) {
        strcpy(data, other.data);
    }
    
    // Copy assignment (for lvalues)
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size + 1];
            strcpy(data, other.data);
        }
        return *this;
    }
};
```

---

# SECTION 10: COMPILE-TIME PROGRAMMING

## 10.1 Tuple Operations at Compile-Time

```cpp
#include <tuple>
#include <iostream>

// Compile-time tuple iteration
namespace detail {
    template<typename F, typename Tuple, size_t... Is>
    void for_each_impl(F&& f, Tuple&& t, index_sequence<Is...>) {
        (..., f(get<Is>(forward<Tuple>(t))));
    }
}

template<typename F, typename Tuple>
void for_each(F&& f, Tuple&& t) {
    constexpr auto size = tuple_size_v<decay_t<Tuple>>;
    detail::for_each_impl(
        forward<F>(f),
        forward<Tuple>(t),
        make_index_sequence<size>{}
    );
}

// Usage
auto t = make_tuple(1, 2.5, "hello");
for_each([](auto x) { cout << x << " "; }, t);  // 1 2.5 hello
```

## 10.2 Compile-Time String Processing

```cpp
#include <array>
#include <string_view>

// Compile-time string hashing
constexpr size_t hash_compile_time(string_view s) {
    size_t h = 0;
    for (char c : s) {
        h = h * 31 + c;
    }
    return h;
}

// Compile-time string search
constexpr bool contains_compile_time(string_view s, string_view sub) {
    return s.find(sub) != string_view::npos;
}

// Usage in compile-time context
static_assert(contains_compile_time("hello world", "world"));
constexpr auto hash = hash_compile_time("key");
```

---

# SECTION 11: META-OBJECT PROTOCOL

## 11.1 Reflection-Like Patterns

```cpp
// Manual reflection without std::meta
struct Member {
    string_view name;
    string_view type;
    size_t offset;
};

template<typename T>
constexpr auto get_members() {
    // Specialize for each type
    return array<Member, 0>{};
}

template<>
constexpr auto get_members<Person>() {
    return array<Member, 3>{{
        {"name", "string", offsetof(Person, name)},
        {"age", "int", offsetof(Person, age)},
        {"email", "string", offsetof(Person, email)}
    }};
}

// Serialize any type
template<typename T>
string serialize(const T& obj) {
    string result;
    for (const auto& member : get_members<T>()) {
        result += member.name;
        result += ": ";
        // Serialize value...
    }
    return result;
}
```

## 11.2 Magic Enum (Reflection Hack)

Before C++26 Reflection, we use compiler intrinsics to get enum names.

```cpp
template <auto V>
constexpr std::string_view get_enum_name() {
    // Compiler-specific macros
    #ifdef __clang__
        return __PRETTY_FUNCTION__;
    #elif defined(__GNUC__)
        return __PRETTY_FUNCTION__;
    #elif defined(_MSC_VER)
        return __FUNCSIG__;
    #endif
}

enum class Color { Red, Green, Blue };

int main() {
    // Output contains "Color::Red" buried in the string
    std::cout << get_enum_name<Color::Red>() << "\n";
}
```
*Note: Libraries like `magic_enum` automate parsing this string.*

---

# SECTION 12: ADVANCED CONTAINER TECHNIQUES

## 12.1 COW (Copy-On-Write) String

```cpp
class CowString {
private:
    struct Buffer : public enable_shared_from_this<Buffer> {
        string data;
        
        Buffer(const string& s) : data(s) {}
    };
    
    shared_ptr<Buffer> buffer;
    
    void ensure_unique() {
        if (!buffer.unique()) {
            buffer = make_shared<Buffer>(*buffer);
        }
    }
    
public:
    CowString(const string& s = "")
        : buffer(make_shared<Buffer>(s)) {}
    
    const char* data() const {
        return buffer->data.c_str();
    }
    
    char& operator[](size_t i) {
        ensure_unique();
        return buffer->data[i];
    }
};
```

## 12.2 Intrusive Containers

```cpp
#include <list>

// Node that contains its own link
class IntrusiveNode {
public:
    IntrusiveNode* next = nullptr;
    IntrusiveNode* prev = nullptr;
};

template<typename T>
class IntrusiveList {
private:
    IntrusiveNode* head;
    IntrusiveNode* tail;
    
public:
    void push_back(T* node) {
        if (!head) {
            head = tail = node;
        } else {
            tail->next = node;
            node->prev = tail;
            tail = node;
        }
    }
};
```

---

# SECTION 13: ABI & BINARY COMPATIBILITY

## 13.1 Stable ABI Design

```cpp
// Version-stable interface
class StableObject {
private:
    // Use void* for opaque pointer
    void* impl;
    
public:
    StableObject();
    ~StableObject();
    
    // Stable functions
    void do_something(int x);
    int get_value() const;
    
    // Virtual table for future extensions
    virtual ~StableObject() = default;
};

// Implementation in separate library
class StableObjectImpl {
public:
    int value = 0;
    void do_something(int x);
    int get_value() const { return value; }
};

StableObject::StableObject() 
    : impl(new StableObjectImpl()) {}

StableObject::~StableObject() {
    delete static_cast<StableObjectImpl*>(impl);
}

void StableObject::do_something(int x) {
    static_cast<StableObjectImpl*>(impl)->do_something(x);
}

int StableObject::get_value() const {
    return static_cast<StableObjectImpl*>(impl)->get_value();
}
```

---

# SECTION 14: PERFORMANCE PROFILING & OPTIMIZATION

## 14.1 Benchmarking Framework

```cpp
#include <chrono>
#include <vector>

class Benchmark {
private:
    using Clock = chrono::high_resolution_clock;
    vector<chrono::nanoseconds> times;
    
public:
    template<typename F>
    void run(F func, int iterations = 1000) {
        for (int i = 0; i < iterations; i++) {
            auto start = Clock::now();
            func();
            auto end = Clock::now();
            times.push_back(chrono::duration_cast<chrono::nanoseconds>(end - start));
        }
    }
    
    void report() {
        if (times.empty()) return;
        
        sort(times.begin(), times.end());
        
        auto sum = 0LL;
        for (auto t : times) sum += t.count();
        
        cout << "Min: " << times.front().count() << " ns\n";
        cout << "Max: " << times.back().count() << " ns\n";
        cout << "Avg: " << (sum / times.size()) << " ns\n";
        cout << "Median: " << times[times.size() / 2].count() << " ns\n";
    }
};

// Usage
Benchmark bm;
bm.run([]() { /* test code */ }, 10000);
bm.report();
```

## 14.2 Cache-Friendly Algorithms

```cpp
// Cache-friendly data access pattern
template<typename T>
void cache_friendly_process(vector<T>& data) {
    // Process in cache-line aligned chunks
    const size_t CACHE_LINE = 64;  // 64 bytes typical
    const size_t CHUNK_SIZE = CACHE_LINE / sizeof(T);
    
    for (size_t i = 0; i < data.size(); i += CHUNK_SIZE) {
        // Process one cache line worth of data
        for (size_t j = i; j < min(i + CHUNK_SIZE, data.size()); j++) {
            process_element(data[j]);
        }
    }
}

// SIMD-friendly loop
void simd_process(int* data, size_t size) {
    // Align data for SIMD operations
    alignas(32) int buffer[32];
    
    for (size_t i = 0; i < size; i += 32) {
        // Process 32-byte chunks (8 ints)
        for (int j = 0; j < 8; j++) {
            data[i + j] = process(data[i + j]);
        }
    }
}
```

---

# SECTION 15: DOMAIN-SPECIFIC LANGUAGE DESIGN

## 15.1 Expression DSL

```cpp
// Domain-specific language for math expressions
class Expr {
public:
    virtual ~Expr() = default;
    virtual double evaluate() = 0;
};

class Constant : public Expr {
private:
    double value;
public:
    Constant(double v) : value(v) {}
    double evaluate() override { return value; }
};

class BinaryOp : public Expr {
private:
    shared_ptr<Expr> left, right;
    function<double(double, double)> op;
    
public:
    BinaryOp(shared_ptr<Expr> l, shared_ptr<Expr> r,
             function<double(double, double)> op)
        : left(l), right(r), op(op) {}
    
    double evaluate() override {
        return op(left->evaluate(), right->evaluate());
    }
};

// Operator overloading for DSL
auto operator+(shared_ptr<Expr> l, shared_ptr<Expr> r) {
    return make_shared<BinaryOp>(l, r, [](double a, double b) { return a + b; });
}

// Usage
auto expr = make_shared<Constant>(3) + make_shared<Constant>(4);
cout << expr->evaluate() << "\n";  // 7
```

---

# SECTION 16: MODERN DESIGN PATTERNS

Traditional GoF patterns often use inheritance. Modern C++ favors composition, templates, and lambdas.

## 16.1 Strategy Pattern (Functional Approach)

Instead of a class hierarchy, use `std::function` or templates.

```cpp
#include <functional>
#include <iostream>
#include <vector>

// Traditional: abstract base class Strategy
// Modern: std::function
using SortStrategy = std::function<void(std::vector<int>&)>;

class Sorter {
    SortStrategy strategy;
public:
    Sorter(SortStrategy s) : strategy(s) {}
    
    void sort(std::vector<int>& data) {
        if (strategy) strategy(data);
    }
};

int main() {
    std::vector<int> data = {5, 2, 9, 1};
    
    // Strategy 1: Lambda
    Sorter s1([](auto& v) { std::sort(v.begin(), v.end()); });
    
    // Strategy 2: Different logic
    Sorter s2([](auto& v) { std::sort(v.rbegin(), v.rend()); });
    
    s1.sort(data);
    return 0;
}
```

## 16.2 Visitor Pattern (std::variant)

Replace virtual functions with `std::variant` and `std::visit` for closed sets of types.

```cpp
#include <variant>
#include <iostream>
#include <vector>

struct Circle { double radius; };
struct Square { double side; };
using Shape = std::variant<Circle, Square>;

// Visitor
struct AreaVisitor {
    double operator()(const Circle& c) { return 3.14159 * c.radius * c.radius; }
    double operator()(const Square& s) { return s.side * s.side; }
};

int main() {
    std::vector<Shape> shapes = { Circle{2.0}, Square{3.0} };
    
    for (const auto& s : shapes) {
        // Apply visitor
        double area = std::visit(AreaVisitor{}, s);
        std::cout << "Area: " << area << "\n";
    }
    
    // With lambda (overloaded pattern)
    // See "Helper for std::visit" in many codebases
    return 0;
}
```

---

# SECTION 17: HARDWARE SYMPATHY

## 17.1 Cache Locality & False Sharing

CPUs load data in cache lines (typically 64 bytes).

### False Sharing
When two threads modify independent variables that sit on the *same* cache line, they invalidate each other's cache, destroying performance.

```cpp
#include <new>
#include <atomic>

struct BadCounter {
    std::atomic<int> a; // Thread 1 modifies
    std::atomic<int> b; // Thread 2 modifies
    // Likely on same cache line -> ping-pong effect
};

struct GoodCounter {
    alignas(64) std::atomic<int> a; // Forced to own cache line
    alignas(64) std::atomic<int> b;
};
```

## 17.2 Branch Prediction

CPUs try to guess which way an `if` will go. Modern C++20 provides attributes to help.

```cpp
void process(int* ptr) {
    if (!ptr) [[unlikely]] {
        // Compiler optimizes this block to be "cold"
        // CPU assumes this won't happen
        throw std::runtime_error("Null pointer");
    }
    
    // This "hot" path is optimized for fall-through
    if (ptr) [[likely]] {
        *ptr = 42;
    }
}
```

## 17.3 SIMD (Single Instruction, Multiple Data)

Using intrinsics (or libraries like `std::simd` in future) to process data in parallel lanes.

```cpp
// Example: Manual unrolling for auto-vectorization
void add_arrays(float* a, float* b, float* c, int n) {
    // Tell compiler pointers don't alias (C99 restrict, or implementation specific)
    // #pragma omp simd 
    for (int i = 0; i < n; ++i) {
        c[i] = a[i] + b[i];
    }
}
```

---

## <a name="chapter-15-productionprofessional"></a>CHAPTER 15: PRODUCTION & PROFESSIONAL

## LARGE-SCALE PROJECT ARCHITECTURE

## 1.1 Layered Architecture

```cpp
// src/layers/presentation/controller.h
#ifndef PRESENTATION_CONTROLLER_H
#define PRESENTATION_CONTROLLER_H

#include "../domain/user.h"
#include "../application/user_service.h"

namespace presentation {
    class UserController {
    private:
        application::UserService& service;
        
    public:
        UserController(application::UserService& s) : service(s) {}
        
        void create_user(const std::string& name, const std::string& email) {
            auto user = service.create(name, email);
            display_result(user);
        }
        
    private:
        void display_result(const domain::User& user);
    };
}

#endif
```

```cpp
// src/layers/application/user_service.h
#ifndef APPLICATION_USER_SERVICE_H
#define APPLICATION_USER_SERVICE_H

#include "../domain/user.h"
#include "../infrastructure/user_repository.h"

namespace application {
    class UserService {
    private:
        infrastructure::UserRepository& repo;
        
    public:
        UserService(infrastructure::UserRepository& r) : repo(r) {}
        
        domain::User create(const std::string& name, const std::string& email) {
            // Business logic
            domain::User user(name, email);
            validate_user(user);
            return repo.save(user);
        }
        
    private:
        void validate_user(const domain::User& user);
    };
}

#endif
```

```cpp
// src/layers/domain/user.h
#ifndef DOMAIN_USER_H
#define DOMAIN_USER_H

namespace domain {
    class User {
    private:
        int id;
        std::string name;
        std::string email;
        
    public:
        User(const std::string& n, const std::string& e)
            : id(0), name(n), email(e) {}
        
        // Domain methods
        bool is_valid() const;
        void update_email(const std::string& new_email);
    };
}

#endif
```

```cpp
// src/layers/infrastructure/user_repository.h
#ifndef INFRASTRUCTURE_USER_REPOSITORY_H
#define INFRASTRUCTURE_USER_REPOSITORY_H

#include "../domain/user.h"
#include "database_connection.h"

namespace infrastructure {
    class UserRepository {
    private:
        DatabaseConnection& db;
        
    public:
        UserRepository(DatabaseConnection& d) : db(d) {}
        
        domain::User save(const domain::User& user);
        std::optional<domain::User> find_by_id(int id);
        std::vector<domain::User> find_all();
    };
}

#endif
```

## 1.2 Microservices Architecture

```cpp
// Service 1: User Service
namespace user_service {
    class UserAPI {
    private:
        application::UserService& service;
        http::Server& server;
        
    public:
        void setup_routes() {
            server.post("/users", [this](const auto& req) {
                auto user = service.create(req.name, req.email);
                return http::Response::ok(user.to_json());
            });
            
            server.get("/users/:id", [this](const auto& req) {
                auto user = service.find(req.id);
                return http::Response::ok(user.to_json());
            });
        }
    };
}

// Service 2: Order Service
namespace order_service {
    class OrderAPI {
    private:
        application::OrderService& service;
        http::Client& http_client;
        
    public:
        void create_order(int user_id, const Order& order) {
            // Call user service
            auto user = http_client.get("http://user-service/users/" + std::to_string(user_id));
            
            // Create order
            service.create(user_id, order);
        }
    };
}
```

---

# SECTION 2: CODE ORGANIZATION & PROJECT STRUCTURE

## 2.1 Modern CMake Project Structure

```cmake
# CMakeLists.txt - Project root
cmake_minimum_required(VERSION 3.20)
project(MyProject VERSION 1.0.0 LANGUAGES CXX)

# C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project structure
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Find dependencies
find_package(Boost REQUIRED)
find_package(Catch2 REQUIRED)
```

```cmake
# src/CMakeLists.txt - Main library
add_library(mylib
    domain/user.cpp
    domain/order.cpp
    application/user_service.cpp
    infrastructure/user_repository.cpp
)

target_include_directories(mylib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(mylib
    PUBLIC Boost::system
    PRIVATE Boost::thread
)

# Executable
add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE mylib)
```

```cmake
# tests/CMakeLists.txt - Test suite
add_executable(tests
    test_user_service.cpp
    test_user_repository.cpp
)

target_link_libraries(tests
    PRIVATE mylib Catch2::Catch2WithMain
)

add_test(NAME AllTests COMMAND tests)
```

## 2.2 Header Organization

```cpp
// include/mylib/version.h
#ifndef MYLIB_VERSION_H
#define MYLIB_VERSION_H

#define MYLIB_VERSION_MAJOR 1
#define MYLIB_VERSION_MINOR 0
#define MYLIB_VERSION_PATCH 0

namespace mylib {
    struct Version {
        static constexpr int major = MYLIB_VERSION_MAJOR;
        static constexpr int minor = MYLIB_VERSION_MINOR;
        static constexpr int patch = MYLIB_VERSION_PATCH;
    };
}

#endif
```

```cpp
// include/mylib/mylib.h - Main header
#ifndef MYLIB_H
#define MYLIB_H

// Version
#include "mylib/version.h"

// Core components
#include "mylib/domain/user.h"
#include "mylib/domain/order.h"

// Services
#include "mylib/application/user_service.h"
#include "mylib/application/order_service.h"

// Infrastructure
#include "mylib/infrastructure/database.h"

// Re-export main classes
namespace mylib {
    using domain::User;
    using domain::Order;
    using application::UserService;
}

#endif
```

## 2.3 Modern CMake with Modules (C++20)

Using C++20 Modules requires CMake 3.28+.

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(ModulesDemo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Library with modules
add_library(math_engine)
target_sources(math_engine
    PUBLIC
        FILE_SET CXX_MODULES FILES
            src/math.cppm
            src/vector.cppm
)

# Executable consuming modules
add_executable(app main.cpp)
target_link_libraries(app PRIVATE math_engine)
```

---

# SECTION 3: BUILD SYSTEMS & COMPILATION

## 3.1 Conan Package Manager

```ini
# conanfile.txt
[requires]
boost/1.81.0
fmt/9.1.0
nlohmann_json/3.11.2
catch2/3.3.2

[generators]
CMakeDeps
CMakeToolchain

[options]
boost/*:shared=False
```

```python
# conanfile.py - Advanced
from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake

class MyProjectConan(ConanFile):
    name = "myproject"
    version = "1.0.0"
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    
    requires = "boost/1.81.0", "fmt/9.1.0"
    
    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
    
    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
    
    def package(self):
        cmake = CMake(self)
        cmake.install()
```

## 3.2 Incremental Build Optimization

```cmake
# Enable ccache for faster rebuilds
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CCACHE_PROGRAM}")
endif()

# Unity builds for faster compilation
set_target_properties(mylib PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_BATCH_SIZE 8
)

# Precompiled headers
target_precompile_headers(mylib PRIVATE
    <vector>
    <string>
    <memory>
    <boost/asio.hpp>
)
```

---

# SECTION 4: TESTING STRATEGIES

## 4.1 Unit Testing with Catch2

```cpp
#include <catch2/catch_all.hpp>
#include "mylib/application/user_service.h"
#include "test_fixtures.h"

TEST_CASE("UserService creates users correctly", "[user_service]") {
    auto repo = MockUserRepository();
    UserService service(repo);
    
    SECTION("Valid user creation") {
        auto user = service.create("John Doe", "john@example.com");
        
        REQUIRE(user.name == "John Doe");
        REQUIRE(user.email == "john@example.com");
        REQUIRE(repo.save_called == true);
    }
    
    SECTION("Invalid email rejects") {
        REQUIRE_THROWS_AS(
            service.create("John", "invalid-email"),
            InvalidEmailException
        );
    }
}

TEST_CASE("UserService handles duplicates", "[user_service]") {
    auto repo = MockUserRepository();
    UserService service(repo);
    
    service.create("John", "john@example.com");
    
    REQUIRE_THROWS_AS(
        service.create("Jane", "john@example.com"),
        DuplicateEmailException
    );
}
```

## 4.2 Integration Testing

```cpp
TEST_CASE("User creation workflow", "[integration]") {
    // Setup
    Database db = setup_test_database();
    UserRepository repo(db);
    UserService service(repo);
    
    // Execute
    auto user = service.create("John Doe", "john@example.com");
    auto fetched = repo.find_by_id(user.id);
    
    // Verify
    REQUIRE(fetched.has_value());
    REQUIRE(fetched->name == "John Doe");
    
    // Cleanup
    cleanup_test_database(db);
}
```

## 4.3 Test Fixtures & Mocks

```cpp
class UserRepositoryMock : public UserRepository {
public:
    bool save_called = false;
    std::vector<User> saved_users;
    
    User save(const User& user) override {
        save_called = true;
        User u = user;
        u.id = ++last_id;
        saved_users.push_back(u);
        return u;
    }
    
private:
    static int last_id;
};

class UserServiceTest {
protected:
    UserRepositoryMock repo;
    UserService service{repo};
};

TEST_CASE_METHOD(UserServiceTest, "Multiple users") {
    auto u1 = service.create("John", "john@example.com");
    auto u2 = service.create("Jane", "jane@example.com");
    
    REQUIRE(repo.saved_users.size() == 2);
}
```

## 4.4 Advanced Mocking with Google Mock (GMock)

For complex interactions, use GMock.

```cpp
#include <gmock/gmock.h>

class MockDB : public Database {
public:
    MOCK_METHOD(bool, connect, (string), (override));
    MOCK_METHOD(void, query, (string), (override));
};

TEST(DBTest, LoginSequence) {
    MockDB db;
    
    // Expect connect called once with "admin"
    EXPECT_CALL(db, connect("admin"))
        .Times(1)
        .WillOnce(testing::Return(true));
        
    // Expect query called any number of times
    EXPECT_CALL(db, query(testing::_))
        .Times(testing::AtLeast(0));
        
    UserManager mgr(&db);
    mgr.login("admin");
}
```

---

# SECTION 5: DEBUGGING & PROFILING

## 5.1 Debug Utilities

```cpp
// include/mylib/debug.h
#ifndef MYLIB_DEBUG_H
#define MYLIB_DEBUG_H

#include <iostream>
#include <source_location>

namespace mylib::debug {
    enum class Level { Debug, Info, Warning, Error };
    
    class Logger {
    private:
        static Level current_level;
        
    public:
        template<typename... Args>
        static void log(Level level, std::format_string<Args...> fmt, Args... args) {
            if (level < current_level) return;
            
            auto loc = std::source_location::current();
            std::cerr << std::format("[{}:{}:{}] {}",
                loc.file_name(), loc.line(), loc.column(),
                std::format(fmt, args...)
            ) << "\n";
        }
        
        static void set_level(Level l) { current_level = l; }
    };
    
    #ifdef DEBUG
        #define LOG_DEBUG(...) debug::Logger::log(debug::Level::Debug, __VA_ARGS__)
        #define LOG_INFO(...) debug::Logger::log(debug::Level::Info, __VA_ARGS__)
    #else
        #define LOG_DEBUG(...)
        #define LOG_INFO(...)
    #endif
}

#endif
```

## 5.2 Performance Profiling

```cpp
#include <chrono>
#include <map>

class PerformanceProfiler {
private:
    struct Measurement {
        std::chrono::nanoseconds total{0};
        int count = 0;
        std::chrono::nanoseconds min{LLONG_MAX};
        std::chrono::nanoseconds max{0};
    };
    
    std::map<std::string, Measurement> measurements;
    
public:
    class Scope {
    private:
        std::string name;
        PerformanceProfiler& profiler;
        std::chrono::high_resolution_clock::time_point start;
        
    public:
        Scope(std::string n, PerformanceProfiler& p) 
            : name(n), profiler(p), start(std::chrono::high_resolution_clock::now()) {}
        
        ~Scope() {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            profiler.record(name, duration);
        }
    };
    
    void record(const std::string& name, std::chrono::nanoseconds duration) {
        auto& m = measurements[name];
        m.total += duration;
        m.count++;
        m.min = std::min(m.min, duration);
        m.max = std::max(m.max, duration);
    }
    
    void report() const {
        std::cout << "Performance Report:\n";
        std::cout << std::string(60, '=') << "\n";
        
        for (const auto& [name, m] : measurements) {
            auto avg = m.total.count() / m.count;
            std::cout << std::format("{:<30} | Count: {:>5} | Avg: {:>10}ns | Min: {:>10}ns | Max: {:>10}ns\n",
                name, m.count, avg, m.min.count(), m.max.count());
        }
    }
};

// Usage
#define PROFILE(name) PerformanceProfiler::Scope _scope(name, get_profiler())

void process_data(const std::vector<int>& data) {
    PROFILE("process_data");
    
    {
        PROFILE("sort");
        std::sort(data.begin(), data.end());
    }
    
    {
        PROFILE("filter");
        // Filter implementation
    }
}
```

## 5.3 Memory Profiling with AddressSanitizer

```cmake
# CMakeLists.txt - AddressSanitizer configuration
option(ENABLE_SANITIZER "Enable AddressSanitizer" OFF)

if(ENABLE_SANITIZER)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()
```

---

# SECTION 6: VERSION CONTROL & COLLABORATION

## 6.1 Git Workflow

```bash
# .gitignore - Standard C++ project
build/
dist/
*.o
*.a
*.so
*.exe
.vscode/
.idea/
*.swp
CMakeLists.txt.user
*.qbs.user

# Git configuration - .git/config
[user]
    name = Developer
    email = dev@company.com

[core]
    editor = vim
    autocrlf = input

[pull]
    rebase = true

[branch]
    autosetuprebase = always
```

## 6.2 Feature Branch Workflow

```bash
# Main branch is production-ready
git checkout main
git pull origin main

# Create feature branch
git checkout -b feature/user-authentication

# Commit with conventional commits
git add src/
git commit -m "feat: implement JWT authentication"

# Rebase and squash commits
git rebase -i main

# Create PR and request review
git push origin feature/user-authentication
```

---

# SECTION 7: DOCUMENTATION & KNOWLEDGE TRANSFER

## 7.1 Code Documentation with Doxygen

```cpp
/**
 * @file user_service.h
 * @brief User service implementation for managing user lifecycle
 * @author John Doe
 * @version 1.0.0
 * @date 2024-01-15
 */

namespace application {
    /**
     * @class UserService
     * @brief Service for managing user operations
     * 
     * UserService provides business logic for user management including
     * creation, deletion, and modification. It validates input and
     * persists data through the UserRepository.
     * 
     * @example
     * @code
     * UserService service(repository);
     * auto user = service.create("John Doe", "john@example.com");
     * @endcode
     */
    class UserService {
    public:
        /**
         * @brief Creates a new user
         * 
         * @param name The user's full name
         * @param email The user's email address
         * 
         * @return User The created user object with assigned ID
         * 
         * @throws InvalidNameException if name is empty
         * @throws InvalidEmailException if email is invalid
         * @throws DuplicateEmailException if email already exists
         * 
         * @complexity O(n) where n is the number of existing users
         */
        User create(const std::string& name, const std::string& email);
        
        /**
         * @brief Updates user information
         * 
         * @param id User ID
         * @param name New name
         * @param email New email
         * 
         * @return User Updated user object
         * 
         * @throws UserNotFoundException if user doesn't exist
         * @throws InvalidEmailException if email is invalid
         */
        User update(int id, const std::string& name, const std::string& email);
    };
}
```

## 7.2 Architecture Decision Records (ADR)

```markdown
# ADR-001: Use Layered Architecture

## Status
Accepted

## Context
The system needs to be scalable, testable, and maintainable.
Different concerns (UI, business logic, data access) must be separated.

## Decision
We will implement a layered architecture with:
- Presentation Layer (Controllers, Views)
- Application Layer (Services, Use Cases)
- Domain Layer (Business Logic)
- Infrastructure Layer (Database, External Services)

## Consequences
### Positive
- Clear separation of concerns
- Easy to test (mock layers)
- Scalable architecture

### Negative
- More files to navigate
- Increased complexity for simple features
- Potential performance overhead from layer crossing

## Alternatives Considered
- Hexagonal (Ports & Adapters) - More complex, chosen layered for simplicity
- Microservices - Future consideration for scalability
```

---

# SECTION 8: SECURITY & SAFETY

## 8.1 Input Validation

```cpp
#include <regex>
#include <stdexcept>

namespace security {
    class InputValidator {
    public:
        static void validate_email(const std::string& email) {
            static const std::regex email_regex(
                R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)"
            );
            
            if (!std::regex_match(email, email_regex)) {
                throw std::invalid_argument("Invalid email format");
            }
        }
        
        static void validate_length(const std::string& str, 
                                   size_t min_length, size_t max_length) {
            if (str.length() < min_length || str.length() > max_length) {
                throw std::invalid_argument(
                    std::format("String length must be between {} and {}", 
                               min_length, max_length)
                );
            }
        }
        
        static void validate_integer_range(int value, int min, int max) {
            if (value < min || value > max) {
                throw std::out_of_range(
                    std::format("Value must be between {} and {}", min, max)
                );
            }
        }
    };
}
```

## 8.2 SQL Injection Prevention

```cpp
#include <sqlite3.h>

namespace database {
    class SafeQuery {
    private:
        sqlite3* db;
        
    public:
        std::vector<User> find_by_email_safe(const std::string& email) {
            const char* sql = "SELECT * FROM users WHERE email = ?";
            sqlite3_stmt* stmt;
            
            // Prepare statement (prevents SQL injection)
            int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
            if (rc != SQLITE_OK) {
                throw std::runtime_error("SQL error");
            }
            
            // Bind parameters safely
            sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
            
            std::vector<User> results;
            while (sqlite3_step(stmt) == SQLITE_ROW) {
                // Extract results
                int id = sqlite3_column_int(stmt, 0);
                const char* name = (const char*)sqlite3_column_text(stmt, 1);
                
                results.emplace_back(id, name, email);
            }
            
            sqlite3_finalize(stmt);
            return results;
        }
    };
}
```

---

# SECTION 9: PERFORMANCE ENGINEERING

## 9.1 Benchmarking Framework

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string s = "hello world";
        benchmark::DoNotOptimize(s);
    }
}
BENCHMARK(BM_StringCreation);

static void BM_VectorOperations(benchmark::State& state) {
    for (auto _ : state) {
        std::vector<int> v;
        for (int i = 0; i < 1000; i++) {
            v.push_back(i);
        }
        benchmark::DoNotOptimize(v);
    }
}
BENCHMARK(BM_VectorOperations);

BENCHMARK_MAIN();
```

## 9.2 Load Testing

```cpp
class LoadTester {
public:
    struct Result {
        int total_requests;
        std::chrono::milliseconds total_time;
        double requests_per_second;
        double avg_latency_ms;
        double p99_latency_ms;
    };
    
    Result run_load_test(
        std::function<void()> operation,
        int num_threads,
        int requests_per_thread
    ) {
        std::vector<std::thread> threads;
        std::vector<std::chrono::nanoseconds> latencies;
        std::mutex latencies_mutex;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int t = 0; t < num_threads; t++) {
            threads.emplace_back([&, t] {
                for (int r = 0; r < requests_per_thread; r++) {
                    auto op_start = std::chrono::high_resolution_clock::now();
                    operation();
                    auto op_end = std::chrono::high_resolution_clock::now();
                    
                    auto latency = std::chrono::duration_cast<std::chrono::nanoseconds>(
                        op_end - op_start
                    );
                    
                    {
                        std::lock_guard lock(latencies_mutex);
                        latencies.push_back(latency);
                    }
                }
            });
        }
        
        for (auto& t : threads) t.join();
        
        auto end = std::chrono::high_resolution_clock::now();
        auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        // Calculate percentiles
        std::sort(latencies.begin(), latencies.end());
        int p99_idx = (latencies.size() * 99) / 100;
        
        Result result;
        result.total_requests = num_threads * requests_per_thread;
        result.total_time = total_time;
        result.requests_per_second = result.total_requests / (total_time.count() / 1000.0);
        
        long long sum = 0;
        for (auto l : latencies) sum += l.count();
        result.avg_latency_ms = (sum / latencies.size()) / 1e6;
        result.p99_latency_ms = latencies[p99_idx].count() / 1e6;
        
        return result;
    }
};
```

---

# SECTION 10: ERROR HANDLING & RECOVERY

## 10.1 Exception Hierarchy

```cpp
#include <stdexcept>

namespace application {
    // Base exception
    class ApplicationError : public std::runtime_error {
    protected:
        int error_code;
        std::string error_context;
        
    public:
        ApplicationError(const std::string& msg, int code = -1)
            : std::runtime_error(msg), error_code(code) {}
        
        int get_error_code() const { return error_code; }
        void set_context(const std::string& ctx) { error_context = ctx; }
    };
    
    // Domain exceptions
    class DomainError : public ApplicationError {
    public:
        DomainError(const std::string& msg) 
            : ApplicationError(msg, 1001) {}
    };
    
    class ValidationError : public DomainError {
    public:
        ValidationError(const std::string& msg) 
            : DomainError("Validation: " + msg) {}
    };
    
    class InvalidEmailException : public ValidationError {
    public:
        InvalidEmailException(const std::string& email)
            : ValidationError("Invalid email: " + email) {}
    };
    
    // Repository exceptions
    class RepositoryError : public ApplicationError {
    public:
        RepositoryError(const std::string& msg) 
            : ApplicationError(msg, 2001) {}
    };
    
    class UserNotFoundException : public RepositoryError {
    public:
        UserNotFoundException(int id)
            : RepositoryError("User not found: " + std::to_string(id)) {}
    };
}
```

## 10.2 Error Recovery Patterns

```cpp
class RetryPolicy {
public:
    struct Config {
        int max_retries = 3;
        std::chrono::milliseconds initial_delay{100};
        double backoff_multiplier = 2.0;
        int max_delay_ms = 10000;
    };
    
    template<typename F>
    static auto execute_with_retry(F operation, const Config& config)
        -> std::invoke_result_t<F> {
        
        std::exception_ptr last_exception;
        auto delay = config.initial_delay;
        
        for (int attempt = 0; attempt <= config.max_retries; attempt++) {
            try {
                return operation();
            } catch (const std::exception& e) {
                last_exception = std::current_exception();
                
                if (attempt < config.max_retries) {
                    LOG_INFO("Retry attempt {} after {}ms", 
                            attempt + 1, delay.count());
                    std::this_thread::sleep_for(delay);
                    
                    delay = std::chrono::milliseconds(
                        std::min(static_cast<int>(delay.count() * config.backoff_multiplier),
                                config.max_delay_ms)
                    );
                }
            }
        }
        
        std::rethrow_exception(last_exception);
    }
};

// Usage
auto user = RetryPolicy::execute_with_retry(
    [&]() { return repository.find_user(id); },
    RetryPolicy::Config{.max_retries = 5}
);
```

---

# SECTION 11: DEPLOYMENT & DEVOPS

## 11.1 Docker Containerization

```dockerfile
# Dockerfile - Multi-stage build
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    git \
    libboost-all-dev

WORKDIR /build
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)
RUN cmake --install build --prefix /install

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libboost-system1.74.0 \
    ca-certificates

COPY --from=builder /install /usr/local

EXPOSE 8080
CMD ["/usr/local/bin/myapp"]
```

## 11.2 Kubernetes Deployment

```yaml
# deployment.yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: myapp
  labels:
    app: myapp
spec:
  replicas: 3
  selector:
    matchLabels:
      app: myapp
  template:
    metadata:
      labels:
        app: myapp
    spec:
      containers:
      - name: myapp
        image: myapp:1.0.0
        ports:
        - containerPort: 8080
        resources:
          requests:
            memory: "256Mi"
            cpu: "250m"
          limits:
            memory: "512Mi"
            cpu: "500m"
        livenessProbe:
          httpGet:
            path: /health
            port: 8080
          initialDelaySeconds: 30
          periodSeconds: 10
        readinessProbe:
          httpGet:
            path: /ready
            port: 8080
          initialDelaySeconds: 5
          periodSeconds: 5
```

## 11.3 CI/CD Pipeline (GitHub Actions)

Automate building and testing.

```yaml
name: C++ CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install Dependencies
      run: sudo apt-get install -y libboost-dev cmake
      
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=Release
      
    - name: Build
      run: cmake --build build
      
    - name: Test
      run: cd build && ctest --output-on-failure
```

---

# SECTION 12: CODE REVIEW & QUALITY

## 12.1 Code Review Checklist

```markdown
# Code Review Checklist

## Functionality
- [ ] Code implements the required feature
- [ ] Code handles all edge cases
- [ ] Error handling is appropriate
- [ ] Tests cover the implementation

## Code Quality
- [ ] Variable/function names are clear
- [ ] Code is DRY (Don't Repeat Yourself)
- [ ] Functions are appropriately sized
- [ ] Complexity is reasonable

## Performance
- [ ] No obvious performance issues
- [ ] Memory usage is appropriate
- [ ] Algorithms are efficient
- [ ] No memory leaks

## Security
- [ ] Input validation is present
- [ ] No SQL injection vulnerabilities
- [ ] No buffer overflows
- [ ] Sensitive data is protected

## Testing
- [ ] Unit tests are present
- [ ] Integration tests pass
- [ ] Test coverage is adequate
- [ ] Edge cases are tested

## Documentation
- [ ] Code is well-commented
- [ ] Public APIs are documented
- [ ] Changes are documented
- [ ] Architecture decisions are recorded
```

## 12.2 Static Code Analysis

```cmake
# CMakeLists.txt - Clang-Tidy integration
find_program(CLANG_TIDY clang-tidy)

if(CLANG_TIDY)
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}"
        "-checks=*"
        "-header-filter=.*"
        "-fix"
    )
endif()

# Cppcheck integration
find_program(CPPCHECK cppcheck)

if(CPPCHECK)
    add_custom_target(cppcheck
        COMMAND ${CPPCHECK}
            --enable=all
            --suppress=missingIncludeSystem
            ${CMAKE_SOURCE_DIR}/src
    )
endif()
```

---

# SECTION 13: TECHNICAL DEBT MANAGEMENT

## 13.1 Tracking Technical Debt

```cpp
// Technical debt marker
namespace technical_debt {
    /**
     * @deprecated Refactor this when performance is not critical.
     * Use optimized_algorithm_v2 instead.
     * 
     * @todo Refactor by Q2 2024
     * @complexity O(n²) - needs optimization
     */
    void inefficient_sort(std::vector<int>& data) {
        // Bubble sort - inefficient but simple
        for (size_t i = 0; i < data.size(); i++) {
            for (size_t j = 0; j < data.size() - 1; j++) {
                if (data[j] > data[j + 1]) {
                    std::swap(data[j], data[j + 1]);
                }
            }
        }
    }
    
    /**
     * @deprecated Temporary workaround for issue #123.
     * Remove when backend API is fixed.
     * @todo Track: https://github.com/team/project/issues/123
     */
    std::string get_user_name(int id) {
        // Workaround: return hardcoded values until backend is fixed
        static const std::map<int, std::string> workaround{
            {1, "John Doe"},
            {2, "Jane Smith"}
        };
        return workaround.at(id);
    }
}
```

## 13.2 Refactoring Strategy

```cpp
// Old code
class User {
public:
    void save_to_db(const std::string& connection_string) {
        // Direct database access - tightly coupled
    }
    
    void send_email(const std::string& subject, const std::string& body) {
        // Email sending logic mixed with domain logic
    }
};

// Refactored code
class User {
private:
    int id;
    std::string name;
    std::string email;
    
public:
    int get_id() const { return id; }
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};

// Separated concerns
class UserRepository {
public:
    void save(const User& user);
};

class UserEmailService {
public:
    void send_notification(const User& user, const std::string& message);
};
```

---

# SECTION 14: LEGACY CODE MODERNIZATION

## 14.1 Incremental Modernization

```cpp
// Legacy code (C++98 style)
class LegacyUser {
public:
    char* name;      // Raw pointer
    char* email;     // Raw pointer
    
    LegacyUser(const char* n, const char* e) {
        name = new char[strlen(n) + 1];
        strcpy(name, n);  // Unsafe
        email = new char[strlen(e) + 1];
        strcpy(email, e);  // Unsafe
    }
    
    ~LegacyUser() {
        delete[] name;
        delete[] email;
    }
};

// Step 1: Add modern wrapper
class ModernUserWrapper {
private:
    LegacyUser* legacy;
    
public:
    ModernUserWrapper(const std::string& name, const std::string& email) {
        legacy = new LegacyUser(name.c_str(), email.c_str());
    }
    
    ~ModernUserWrapper() { delete legacy; }
    
    std::string_view get_name() const { return legacy->name; }
    std::string_view get_email() const { return legacy->email; }
};

// Step 2: Full modernization
class ModernUser {
private:
    std::string name;
    std::string email;
    
public:
    ModernUser(std::string n, std::string e)
        : name(std::move(n)), email(std::move(e)) {}
    
    const std::string& get_name() const { return name; }
    const std::string& get_email() const { return email; }
};
```

---

# SECTION 15: LEADERSHIP & TEAM MANAGEMENT

## 15.1 Mentoring Framework

```markdown
# Mentoring Guidelines for C++ Teams

## Levels

### Junior Developer (0-1 year)
- Focus: Understanding fundamentals
- Guidance: Pair programming, code reviews, architecture training
- Goals: Contribute to features, improve C++ knowledge

### Mid-Level Developer (1-3 years)
- Focus: Mastering patterns and best practices
- Guidance: Design reviews, mentoring juniors, taking ownership
- Goals: Lead features, improve design decisions

### Senior Developer (3+ years)
- Focus: Architecture, leadership, knowledge sharing
- Guidance: Strategic decisions, cross-team collaboration
- Goals: Shape team direction, mentor other seniors

## Mentoring Actions
1. Code review with detailed feedback
2. Design discussions before implementation
3. Pair programming sessions
4. Knowledge sharing sessions
5. Challenge with progressively harder problems
```

## 15.2 Technical Decision Making

```markdown
# RFC (Request For Comments) Template

## Title
Brief description of the proposal

## Motivation
Why this change is needed

## Detailed Design
Technical approach and architecture

## Trade-offs
What we're giving up

## Alternatives
Other approaches considered

## Implementation Plan
Steps to implement

## Timeline
Expected completion

## Success Metrics
How we measure success

## Discussion
Team feedback period (1 week)

## Decision
Final decision and rationale
```

---

---

Final decision and rationale

```

---

## <a name="chapter-16-systemdesigncasestudiescedition"></a>CHAPTER 16: SYSTEM DESIGN CASE STUDIES (C++ EDITION)

Solving common interview system design problems using C++ primitives.

### 10.5.1 LRU Cache
**Problem**: Design a Least Recently Used cache with O(1) get and put.
**Solution**: Combine `std::list` (ordering) and `std::unordered_map` (lookup).

```cpp
#include <list>
#include <unordered_map>
#include <iostream>

template<typename Key, typename Value>
class LRUCache {
    size_t capacity;
    std::list<std::pair<Key, Value>> items;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> lookup;

public:
    LRUCache(size_t cap) : capacity(cap) {}

    void put(Key key, Value val) {
        if (lookup.find(key) != lookup.end()) {
            // Update: Move to front, update value
            items.splice(items.begin(), items, lookup[key]);
            lookup[key]->second = val;
            return;
        }

        if (items.size() == capacity) {
            // Evict: Remove back
            lookup.erase(items.back().first);
            items.pop_back();
        }

        // Insert: Push front
        items.emplace_front(key, val);
        lookup[key] = items.begin();
    }

    std::optional<Value> get(Key key) {
        if (lookup.find(key) == lookup.end()) return std::nullopt;
        // Access: Move to front
        items.splice(items.begin(), items, lookup[key]);
        return lookup[key]->second;
    }
};
```

### 10.5.2 Token Bucket Rate Limiter
**Problem**: Limit requests to N per second.
**Solution**: Refill tokens based on time elapsed.

```cpp
#include <chrono>
#include <mutex>

class TokenBucket {
    const long long capacity;
    const long long rate_per_sec;
    
    double tokens;
    std::chrono::steady_clock::time_point last_refill;
    std::mutex mtx;

public:
    TokenBucket(long long cap, long long rate) 
        : capacity(cap), rate_per_sec(rate), tokens(cap), 
          last_refill(std::chrono::steady_clock::now()) {}

    bool allow_request(int cost = 1) {
        std::lock_guard<std::mutex> lock(mtx);
        refill();
        
        if (tokens >= cost) {
            tokens -= cost;
            return true;
        }
        return false;
    }

private:
    void refill() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last_refill).count();
        
        double new_tokens = (duration * rate_per_sec) / 1000000.0;
        tokens = std::min((double)capacity, tokens + new_tokens);
        last_refill = now;
    }
};
```

## <a name="chapter-17-concurrencydesignpatterns"></a>CHAPTER 17: CONCURRENCY DESIGN PATTERNS

### 10.6.1 Active Object Pattern
Decouples method execution from invocation. The object owns a thread and a message queue.

```cpp
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ActiveObject {
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    std::thread worker;
    bool done = false;

public:
    ActiveObject() {
        worker = std::thread([this] { run(); });
    }

    ~ActiveObject() {
        { std::lock_guard lock(mtx); done = true; }
        cv.notify_one();
        worker.join();
    }

    void invoke(std::function<void()> task) {
        std::lock_guard lock(mtx);
        tasks.push(std::move(task));
        cv.notify_one();
    }

private:
    void run() {
        while (true) {
            std::unique_lock lock(mtx);
            cv.wait(lock, [this] { return !tasks.empty() || done; });
            
            if (done && tasks.empty()) return;
            
            auto task = std::move(tasks.front());
            tasks.pop();
            lock.unlock();
            
            task(); // Execute
        }
    }
};
```

### 10.6.2 Monitor Object (Thread-Safe Interface)
Ensure thread safety by locking in public methods and calling private implementation methods.

```cpp
class Monitor {
    mutable std::mutex mtx;
    int state = 0;

public:
    void update(int val) {
        std::lock_guard lock(mtx); // Lock here
        update_impl(val);
    }

private:
    // Expects lock to be held
    void update_impl(int val) {
        state = val;
    }
};
```

---

## <a name="chapter-18-thecbuildecosystemmastery"></a>CHAPTER 18: THE C++ BUILD ECOSYSTEM MASTERY

Writing code is half the battle. Building and debugging it is the rest.

### 30.1 Package Managers Deep Dive

#### vcpkg (Manifest Mode)
Create `vcpkg.json` in your root:
```json
{
  "name": "my-app",
  "version": "1.0.0",
  "dependencies": [
    "fmt",
    "nlohmann-json"
  ]
}
```
CMake integration:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake
```

#### Conan (conanfile.txt)
```ini
[requires]
fmt/9.1.0
nlohmann_json/3.11.2

[generators]
CMakeDeps
CMakeToolchain
```

### 30.2 Sanitizers: The Developer's Best Friend

#### AddressSanitizer (ASan)
Detects out-of-bounds, use-after-free.
`clang++ -fsanitize=address -g main.cpp`

**Example: Use-After-Free**
```cpp
int* p = new int(5);
delete p;
*p = 10; // ASan catches this instantly!
```

#### ThreadSanitizer (TSan)
Detects data races.
`clang++ -fsanitize=thread -g main.cpp`

**Example: Data Race**
```cpp
int counter = 0;
std::thread t1([&]{ counter++; });
std::thread t2([&]{ counter++; }); // TSan catches this race
t1.join(); t2.join();
```

#### UndefinedBehaviorSanitizer (UBSan)
Detects overflow, null dereference, alignment issues.
`clang++ -fsanitize=undefined -g main.cpp`

### 30.3 Profiling Tools

*   **perf (Linux)**: `perf record -g ./app` -> `perf report`.
*   **Valgrind (Massif)**: Heap profiler. `valgrind --tool=massif ./app`.
*   **Hotspot**: UI for perf.

---

# Volume V: High Performance & Low Latency

## <a name="chapter-19-lowlatencycoptimization"></a>CHAPTER 19: LOW-LATENCY C++ OPTIMIZATION

For HFT, Game Engines, and Real-Time Systems, every nanosecond counts.

### 17.1 CPU Pipelines & Branch Prediction
Modern CPUs are pipelined. A branch misprediction flushes the pipeline, costing 10-20 cycles.

**Optimization: Branchless Programming**
```cpp
// Branchy (Slow if unpredictable)
if (val > 100) val = 100;

// Branchless (Fast)
// Compiler might generate 'cmov' (Conditional Move) instruction
val = (val > 100) ? 100 : val;
```

**Benchmark: Sorted vs Unsorted Array Processing**
Processing a sorted array is faster due to successful branch prediction.

### 17.2 Data-Oriented Design (DoD)
Stop thinking in "Objects". Think in "Data Transforms".

**OOP (Array of Structures - AoS):**
```cpp
struct Entity {
    float x, y, z;
    int hp;
    // ...
};
vector<Entity> entities; 
// Updating 'x' loads 'hp' into cache (waste)
```

**DoD (Structure of Arrays - SoA):**
```cpp
struct Entities {
    vector<float> x, y, z;
    vector<int> hp;
};
// Updating 'x' loads only 'x' data (SIMD friendly, cache friendly)
```

### 17.3 Prefetching
Use `__builtin_prefetch` (GCC/Clang) or `_mm_prefetch` (Intel) to load data into L1 cache before it's needed.

```cpp
for (int i = 0; i < N; ++i) {
    __builtin_prefetch(&data[i + 16]); // Lookahead
    process(data[i]);
}
```

### 17.4 Micro-Benchmarking (Google Benchmark)
Don't guess; measure. `std::chrono` is often too noisy for nanosecond-scale operations.

```cpp
#include <benchmark/benchmark.h>

static void BM_StringCopy(benchmark::State& state) {
    std::string x = "hello";
    for (auto _ : state) {
        std::string copy = x;
        benchmark::DoNotOptimize(copy); // Prevent optimizing away
    }
}
BENCHMARK(BM_StringCopy);
```

### 17.5 System Warm-up
The first few thousand iterations of code are slow due to:
1.  **Instruction Cache Misses**: Code not yet in CPU cache.
2.  **Data Cache Misses**: Data not yet in L1/L2.
3.  **Branch Predictor**: Hasn't learned the patterns yet.
4.  **OS Page Faults**: Memory pages not yet committed.

**Strategy**: Run a "dummy" loop of your critical path 10,000 times before enabling the network listener or trading signal.

### 17.6 False Sharing Prevention
When two threads modify variables on the same cache line (64 bytes), they invalidate each other's L1 cache.

```cpp
#include <new>

struct SharedData {
    // Bad: a and b likely share a cache line
    std::atomic<int> a;
    std::atomic<int> b;
};

struct PaddedData {
    alignas(std::hardware_destructive_interference_size) std::atomic<int> a;
    alignas(std::hardware_destructive_interference_size) std::atomic<int> b;
};
```

---

## <a name="chapter-20-lowlatencysystemarchitecture"></a>CHAPTER 20: LOW-LATENCY SYSTEM ARCHITECTURE

Designing systems where microseconds matter (Trading, Real-time AdTech).

### 24.1 The Disruptor Pattern (C++ Implementation)
A high-performance inter-thread messaging library. Key concept: **Single-Writer Ring Buffer** with no locks.

```cpp
template<typename T, size_t Size>
class Disruptor {
    std::array<T, Size> ring_buffer;
    alignas(64) std::atomic<int64_t> cursor{-1}; // Cache line padded
    
public:
    template<typename F>
    void publish(F&& factory) {
        int64_t current = cursor.load(std::memory_order_relaxed);
        int64_t next = current + 1;
        
        // Write data (no contention for single writer)
        factory(ring_buffer[next & (Size - 1)]);
        
        // Commit
        cursor.store(next, std::memory_order_release);
    }
    
    // Consumer tracks its own sequence...
};
```

### 24.2 Kernel Bypass Networking (Concept)
Standard OS networking (interrupts, context switches) adds 10-50us latency.
**Solution**: Map the NIC (Network Interface Card) directly to user-space memory (DPDK, Solarflare OpenOnload).

*   **Zero Copy**: Packet data goes from NIC -> CPU L3 Cache -> User Buffer.
*   **Polling**: Instead of interrupts, one CPU core spins (`while(true)`) checking the NIC ring.

### 24.3 OS Tuning for C++
Your code is only as fast as the OS allows.

1.  **CPU Isolation (`isolcpus`)**: Isolate cores from the OS scheduler so your thread never gets preempted.
2.  **Huge Pages**: Use 2MB or 1GB pages to reduce TLB (Translation Lookaside Buffer) misses.
    ```cpp
    void* ptr = mmap(NULL, size, PROT_READ|PROT_WRITE, 
                     MAP_PRIVATE|MAP_ANONYMOUS|MAP_HUGETLB, -1, 0);
    ```
3.  **Disable C-States**: Prevent CPU from going to sleep (power save) which causes wake-up latency.

### 24.4 Zero-Copy Serialization (Cap'n Proto / FlatBuffers)
Avoid parsing JSON/XML. Access data directly from the binary buffer.

```cpp
// FlatBuffers schema compiled to C++ header
// No parsing step! Pointers just point to the right offsets.
auto monster = GetMonster(buffer_pointer);
auto hp = monster->hp(); // Immediate access
auto pos = monster->pos();
```

### 24.5 LMAX Disruptor Internals

The key to Disruptor's speed is the **Sequence Barrier**.

1.  **Cursor**: Monotonically increasing number (atomic).
2.  **Barrier**: Consumers wait until `cursor >= my_sequence`.
3.  **Wait Strategy**:
    *   `BusySpinWaitStrategy`: Loops `while(cursor < seq)`. 100% CPU, 0ns latency.
    *   `YieldingWaitStrategy`: Loops but calls `std::this_thread::yield()`.
    *   `BlockingWaitStrategy`: Uses `std::condition_variable` (slowest).

---

## <a name="chapter-21-extremelowlatencyhardwaremastery"></a>CHAPTER 21: EXTREME LOW LATENCY & HARDWARE MASTERY

To achieve sub-microsecond latency, you must program the hardware, not just the language.

### 31.1 CPU Architecture & Cache Topology
*   **L1 Cache**: ~32KB, 3-4 cycles. Per core.
*   **L2 Cache**: ~256KB-1MB, 10-12 cycles. Per core.
*   **L3 Cache**: ~10MB+, 40-70 cycles. Shared across cores.
*   **RAM**: 100+ cycles.

**Optimization Goal**: Stay in L1/L2.
**Technique**: Minimize object size, use contiguous memory (arrays), align data to cache lines (64 bytes).

### 31.2 NUMA (Non-Uniform Memory Access)
On multi-socket servers, accessing RAM attached to another CPU socket is slow.
*   **Solution**: Pin threads to cores. Allocate memory on the local node.
*   **Tool**: `numactl --cpunodebind=0 --membind=0 ./app`

### 31.3 Compiler Optimizations (The "Free Lunch")
*   `-O3`: Aggressive optimization.
*   `-march=native`: Use instructions available on the build machine (AVX2, AVX-512).
*   `-flto` (Link Time Optimization): Optimize across translation units (inlining across .cpp files).
*   **PGO (Profile Guided Optimization)**:
    1.  Compile with `-fprofile-generate`.
    2.  Run the app (training run).
    3.  Recompile with `-fprofile-use`.

### 31.4 Lock-Free Stack Implementation (Wait-Free Push)
A classic interview and system component.

```cpp
template<typename T>
struct Node {
    T data;
    Node* next;
    Node(const T& d) : data(d), next(nullptr) {}
};

template<typename T>
class LockFreeStack {
    std::atomic<Node<T>*> head{nullptr};

public:
    void push(const T& data) {
        Node<T>* new_node = new Node<T>(data);
        new_node->next = head.load(std::memory_order_relaxed);
        
        // CAS Loop
        while (!head.compare_exchange_weak(
            new_node->next, 
            new_node,
            std::memory_order_release, 
            std::memory_order_relaxed));
    }

    bool pop(T& result) {
        Node<T>* old_head = head.load(std::memory_order_acquire);
        
        while (old_head && !head.compare_exchange_weak(
            old_head,
            old_head->next,
            std::memory_order_acquire,
            std::memory_order_relaxed));
            
        if (!old_head) return false;
        
        result = old_head->data;
        // Note: Deletion in lock-free requires Hazard Pointers or RCU!
        // Leaking here for simplicity of example.
        return true;
    }
};
```

### 31.5 Measurable Performance Targets
Define Service Level Objectives (SLOs) in percentiles.
*   **p50 (Median)**: Typical case.
*   **p99**: The "slow" case (1 in 100).
*   **p99.9**: The tail latency (1 in 1000). Crucial for HFT.

**Example Target**:
"Order processing must have p99 latency < 5 microseconds."

---

## <a name="chapter-22-advancedsimdavx2avx512"></a>CHAPTER 22: ADVANCED SIMD (AVX2 & AVX-512)

Data Parallelism: Processing 8 or 16 numbers in a single CPU cycle.

### 32.1 SIMD Basics & Registers
*   **SSE**: 128-bit (4 floats). XMM registers.
*   **AVX2**: 256-bit (8 floats). YMM registers.
*   **AVX-512**: 512-bit (16 floats). ZMM registers.

### 32.2 Intrinsics Example (Vector Addition)
Using `<immintrin.h>`.

```cpp
#include <immintrin.h>

void add_avx2(float* a, float* b, float* c, int N) {
    // Process 8 floats at a time
    for (int i = 0; i < N; i += 8) {
        // Load
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // Operation
        __m256 vc = _mm256_add_ps(va, vb);
        
        // Store
        _mm256_storeu_ps(&c[i], vc);
    }
}
```
*   `_mm256_loadu_ps`: Load Unaligned Packed Single-precision.
*   `_mm256_add_ps`: Add packed singles.

### 32.3 Measurable Outcome
*   **Objective**: Convert a scalar loop to AVX2.
*   **Success Metric**: 4x-8x speedup on large arrays (memory bandwidth permitting).

---

## <a name="chapter-23-custommemoryallocators"></a>CHAPTER 23: CUSTOM MEMORY ALLOCATORS

`malloc` and `new` are general-purpose and slow (locks, fragmentation). Real-time systems use custom allocators.

### 33.1 Linear Allocator (Arena)
The absolute fastest allocator. O(1). Zero overhead.

```cpp
class LinearAllocator {
    char* start;
    char* current;
    size_t size;
public:
    LinearAllocator(size_t s) : size(s) {
        start = new char[s];
        current = start;
    }
    
    void* allocate(size_t n) {
        if (current + n > start + size) return nullptr;
        void* ptr = current;
        current += n;
        return ptr;
    }
    
    void reset() { current = start; } // Free ALL at once
};
```
*   **Use Case**: Per-frame game memory, Request-scoped web server memory.

### 33.2 Pool Allocator
Fixed-size blocks. No external fragmentation. O(1) malloc/free.

```cpp
struct Chunk { Chunk* next; };

class PoolAllocator {
    Chunk* head = nullptr;
public:
    void* allocate() {
        if (!head) return ::operator new(sizeof(Chunk)); // Or expand pool
        Chunk* ptr = head;
        head = head->next;
        return ptr;
    }
    
    void deallocate(void* ptr) {
        Chunk* chunk = static_cast<Chunk*>(ptr);
        chunk->next = head;
        head = chunk;
    }
};
```

---

# APPENDICES

## Appendix A: C++ Keywords & Operators Reference

### Essential Keywords (Non-Exhaustive)
*   **alignas / alignof**: Memory alignment queries and specifications.
*   **asm**: Inline assembly block.
*   **auto**: Type deduction (C++11).
*   **const / volatile**: cv-qualifiers for type safety and hardware access.
*   **constexpr / consteval / constinit**: Compile-time constant specifications.
*   **decltype**: Inspect declared type of an entity.
*   **explicit**: Prevent implicit conversions in constructors.
*   **export**: Module interface export (C++20).
*   **friend**: Allow access to private members.
*   **inline**: Suggest inlining to compiler; allow definition in header.
*   **mutable**: Allow modification of member in const object.
*   **noexcept**: Specifier for functions that don't throw.
*   **nullptr**: Null pointer literal (C++11).
*   **operator**: Overload operators.
*   **requires**: Constraint clause for Concepts (C++20).
*   **static_assert**: Compile-time assertion.
*   **template**: Define generic classes/functions.
*   **thread_local**: Storage duration specifier.
*   **typeid**: Runtime type identification (RTTI).
*   **typename**: Declare a type parameter in templates.
*   **virtual**: Declare virtual function for polymorphism.

### Special Operators
*   `::` Scope resolution
*   `->*` Pointer to member selection
*   `<=>` Three-way comparison (Spaceship) (C++20)
*   `co_await`, `co_yield`, `co_return` Coroutine operators (C++20)

---

## Appendix B: Common Acronyms

*   **ABI**: Application Binary Interface.
*   **API**: Application Programming Interface.
*   **COW**: Copy On Write.
*   **CRTP**: Curiously Recurring Template Pattern.
*   **CTAD**: Class Template Argument Deduction.
*   **UB**: Undefined Behavior (Avoid at all costs!).
*   **IB**: Implementation-defined Behavior.
*   **IIFE**: Immediately Invoked Function Expression (often with lambdas).
*   **NrvO / RVO**: (Named) Return Value Optimization.
*   **ODR**: One Definition Rule.
*   **PIMPL**: Pointer to Implementation (Opaque Pointer).
*   **RAII**: Resource Acquisition Is Initialization.
*   **RTTI**: Run-Time Type Information.
*   **SFINAE**: Substitution Failure Is Not An Error.
*   **SOO / SSO**: Small Object/String Optimization.
*   **STL**: Standard Template Library.
*   **TMP**: Template Metaprogramming.
*   **TU**: Translation Unit.

---

## Appendix C: Recommended Tooling

### Compilers
*   **GCC (GNU Compiler Collection)**: Standard on Linux.
*   **Clang/LLVM**: Excellent error messages, widely used on macOS/Linux.
*   **MSVC (Microsoft Visual C++)**: Standard on Windows.

### Build Systems
*   **CMake**: The industry standard meta-build system.
*   **Meson**: Modern, fast, Python-based.
*   **Bazel**: Google's build system, good for monorepos.

### Package Managers
*   **Conan**: Decentralized package manager for C/C++.
*   **vcpkg**: Microsoft's C++ library manager.

### Static Analysis & Sanitizers
*   **AddressSanitizer (ASan)**: Detects memory errors (buffer overflows, use-after-free).
*   **UndefinedBehaviorSanitizer (UBSan)**: Detects undefined behavior.
*   **ThreadSanitizer (TSan)**: Detects data races.
*   **Clang-Tidy**: Linter and static analysis tool.
*   **Cppcheck**: Static analysis tool.

---

## Appendix D: Common C++ Traps & Pitfalls

### I. General & Syntax Traps
1.  **Most Vexing Parse**
    *   *Issue*: `MyClass obj();` declares a function returning `MyClass`, not a default-constructed object.
    *   *Fix*: Use brace initialization: `MyClass obj{};`.

2.  **The "dangling else" Problem**
    *   *Issue*: Nested `if-else` without braces can associate `else` with the wrong `if`.
    *   *Fix*: Always use braces `{}` for control structures.

3.  **Integer Division**
    *   *Issue*: `1/2` results in `0` (integer), not `0.5`.
    *   *Fix*: Cast one operand to float/double: `1.0/2` or `static_cast<double>(1)/2`.

4.  **Loop Variable Type Mismatch**
    *   *Issue*: `for (unsigned i = v.size() - 1; i >= 0; --i)` causes an infinite loop because `unsigned` is never negative.
    *   *Fix*: Use `int` (and cast size) or standard iterators/ranges.

5.  **Shadowing Variables**
    *   *Issue*: Declaring a local variable with the same name as a member or outer variable hides the outer one.
    *   *Fix*: Enable compiler warnings (`-Wshadow`) and use `this->member` if necessary.

### II. Pointers, References & Memory
6.  **Object Slicing**
    *   *Issue*: Assigning a `Derived` object to a `Base` value slices off the derived part.
    *   *Fix*: Use pointers `Base*` or references `Base&` for polymorphism.

7.  **Dangling References**
    *   *Issue*: Returning a reference to a local stack variable.
    *   *Fix*: Return by value or use smart pointers/dynamic allocation.

8.  **Iterator Invalidation**
    *   *Issue*: Adding elements to a `std::vector` may reallocate memory, invalidating all pointers/iterators to elements.
    *   *Fix*: Don't cache iterators across mutating operations; use `reserve()` if possible.

9.  **`delete` vs `delete[]`**
    *   *Issue*: Mismatching `new` with `delete[]` or `new[]` with `delete` causes undefined behavior.
    *   *Fix*: Use `std::vector` or `std::unique_ptr` instead of manual management.

10. **Use-After-Move**
    *   *Issue*: Accessing an object after `std::move()` (except for reassignment/destruction).
    *   *Fix*: Treat moved-from objects as empty; do not read their state.

### III. Classes & OOP
11. **Virtual Destructor Missing**
    *   *Issue*: Deleting a derived class via a base pointer when the base destructor is not `virtual` leaks derived resources.
    *   *Fix*: Always mark base class destructors `virtual` (or `protected` if non-polymorphic).

12. **Calling Virtual Functions in Constructor/Destructor**
    *   *Issue*: Calls the *base* class version, not the derived one, because the derived part isn't initialized/is already destroyed.
    *   *Fix*: Use two-phase initialization or factory methods.

13. **Copy Constructor/Assignment Missing**
    *   *Issue*: Classes managing raw pointers will default to shallow copy (double free error).
    *   *Fix*: Follow the **Rule of Three/Five/Zero**.

14. **Initialization Order**
    *   *Issue*: Members are initialized in *declaration order*, not initializer list order.
    *   *Fix*: Keep initializer list order identical to member declaration order to avoid warnings.

### IV. Concurrency
15. **Data Races**
    *   *Issue*: Multiple threads accessing shared memory without synchronization (at least one writer).
    *   *Fix*: Use `std::mutex`, `std::atomic`, or `std::shared_mutex`.

16. **Deadlocks**
    *   *Issue*: Two threads waiting on each other's locks.
    *   *Fix*: Acquire locks in a consistent global order; use `std::scoped_lock` (C++17) to lock multiple mutexes safely.

17. **False Sharing**
    *   *Issue*: Independent atomic variables on the same cache line degrade performance due to cache coherency protocols.
    *   *Fix*: Use `alignas(hardware_destructive_interference_size)` to pad variables.

### V. Modern C++ & Macros
18. **`std::vector<bool>` Weirdness**
    *   *Issue*: It's a template specialization (bitfield), not a vector of bools. Returns a proxy object, not `bool&`.
    *   *Fix*: Use `std::deque<bool>` or `std::vector<char>` if you need real references.

19. **Auto Type Deduction**
    *   *Issue*: `auto` drops references and `const`.
    *   *Fix*: Use `auto&` or `const auto&` explicitly when needed.

20. **Macro Side Effects**
    *   *Issue*: `#define MAX(a,b) ((a) > (b) ? (a) : (b))` evaluates arguments twice. `MAX(x++, y)` increments `x` twice.
    *   *Fix*: Use `inline` functions or templates instead of macros.

21. **Static Initialization Order Fiasco**
    *   *Issue*: Global objects in different files have undefined initialization order.
    *   *Fix*: Use the "Construct On First Use" idiom (Meyers Singleton).

---

## Appendix H: Professional C++ Idioms

### 1. RAII (Resource Acquisition Is Initialization)
*   **Concept**: Bind resource lifecycle to object lifecycle. Constructor acquires, destructor releases.
*   **Use Case**: Memory, file handles, mutex locks, sockets.
*   **Example**: `std::lock_guard`, `std::unique_ptr`.

### 2. Pimpl (Pointer to Implementation)
*   **Concept**: Hide private members in a separate class/struct, accessed via a pointer.
*   **Benefit**: ABI stability, reduced compilation times (header dependency changes don't trigger rebuilds of clients).
*   **Pattern**:
    ```cpp
    class Widget {
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    public:
        Widget();
        ~Widget(); // Defined in .cpp where Impl is visible
    };
    ```

### 3. Copy-and-Swap
*   **Concept**: Implement assignment operator in terms of copy constructor and swap.
*   **Benefit**: Strong Exception Safety guarantee; removes code duplication.
*   **Pattern**:
    ```cpp
    T& operator=(T other) { // Pass by value (copy)
        swap(*this, other);
        return *this;
    }
    ```

### 4. NVI (Non-Virtual Interface)
*   **Concept**: Public interface is non-virtual; virtual functions are private/protected.
*   **Benefit**: Separation of interface (pre/post-conditions) from implementation.
*   **Pattern**:
    ```cpp
    class Base {
    public:
        void doWork() {
            // Pre-condition logic
            doWorkImpl();
            // Post-condition logic
        }
    private:
        virtual void doWorkImpl() = 0;
    };
    ```

### 5. Erase-Remove Idiom
*   **Concept**: Standard way to remove elements from a `std::vector` (before C++20 `std::erase`).
*   **Pattern**: `v.erase(std::remove(v.begin(), v.end(), value), v.end());`

### 6. SFINAE (Substitution Failure Is Not An Error)
*   **Concept**: Remove functions from overload resolution set if types don't match constraints.
*   **Modern Replacement**: C++20 Concepts (`requires`).

### 7. CRTP (Curiously Recurring Template Pattern)
*   **Concept**: Class `Derived` inherits from `Base<Derived>`.
*   **Use Case**: Static polymorphism (compile-time), adding functionality (mixins) without vtable overhead.
*   **Example**: `std::enable_shared_from_this`.

---

## Appendix E: C++ Interview Cheat Sheet

### Core Concepts
1.  **Virtual Functions**: Enable runtime polymorphism via vtable/vptr. Destructors must be virtual in base classes.
2.  **Smart Pointers**:
    *   `unique_ptr`: Exclusive ownership, no overhead.
    *   `shared_ptr`: Shared ownership, ref-counted (atomic), control block overhead.
    *   `weak_ptr`: Non-owning reference to `shared_ptr` (breaks cycles).
3.  **Move Semantics**: Transfers resources (pointers) instead of deep copying. Enabled by rvalue references (`&&`) and `std::move`.
4.  **RAII**: Resource Acquisition Is Initialization. Constructor acquires, destructor releases. Core to C++ safety.
5.  **Cast Types**:
    *   `static_cast`: Compile-time safe conversions.
    *   `dynamic_cast`: Runtime checked downcasting (requires RTTI).
    *   `reinterpret_cast`: Bitwise reinterpretation (unsafe).
    *   `const_cast`: Remove/add constness.

### Modern C++ (C++11/14/17/20)
1.  **Lambdas**: Anonymous function objects. Capture `[=]`, `[&]`, or move-only `[x = std::move(y)]`.
2.  **Auto**: Type deduction. Always initialize.
3.  **Structured Bindings (C++17)**: `auto [x, y] = pair;`
4.  **Concepts (C++20)**: Constrain templates for better errors/readability.
5.  **Coroutines (C++20)**: Functions that can suspend/resume.

### System Design Questions
1.  **Vector vs List**: Vector (contiguous, cache-friendly) is almost always better than List (node-based, cache misses) unless aggressive splicing is needed.
2.  **Map vs Unordered Map**: Map (BST, O(log n), sorted) vs Unordered Map (Hash Table, O(1) avg, unsorted).
3.  **Handling 1M connections**: Use non-blocking I/O (epoll/kqueue) or `io_uring`, not one thread per connection.
4.  **Memory Layout**: Stack (local vars) vs Heap (dynamic) vs Data (globals) vs Text (code).

### Quick Coding
*   **Implement Singleton**: Use static local variable (Thread-safe in C++11+).
*   **Implement String Class**: Handle deep copy, move semantics, and destructor.
*   **Reverse Linked List**: Classic pointer manipulation.

---

## Appendix F: The C++ Standard Evolution Matrix

### 1. Versioned Changelog

#### **C++98 (ISO/IEC 14882:1998)** - *The Foundation*
**Released:** 1998
*   **Core:** Templates, Exceptions, Namespaces, `bool` type, `cast` operators (`static_cast`, etc.), `mutable`, `explicit`.
*   **STL:** Containers (`vector`, `list`, `map`, `set`, `deque`), Algorithms (`sort`, `find`, `transform`), Iterators, Strings (`std::string`), I/O Streams (`iostream`).
*   **Memory:** `std::auto_ptr` (Deprecated in C++11).

#### **C++03 (ISO/IEC 14882:2003)** - *The Bug Fix*
**Released:** 2003
*   **Focus:** Defect Report (DR) fixes for C++98 to ensure consistency across compilers.
*   **Features:** Value initialization `T()`, fixes to `std::vector` contiguous memory guarantee.

#### **C++11 (ISO/IEC 14882:2011)** - *The Modern Revolution*
**Released:** September 2011
*   **Language:** `auto`, `nullptr`, Range-based `for`, Lambda expressions, Rvalue references (`&&`) & Move semantics, Variadic templates, `constexpr` (limited), `decltype`, Uniform initialization `{}`, `static_assert`, `override`, `final`, `enum class`.
*   **Concurrency:** `std::thread`, `std::mutex`, `std::atomic`, `std::future`, `std::async`.
*   **Library:** Smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`), `std::array`, `std::tuple`, `std::unordered_map/set`, `std::regex`, `std::chrono`.

#### **C++14 (ISO/IEC 14882:2014)** - *The Refinement*
**Released:** December 2014
*   **Language:** Generic lambdas (`auto` params), Relaxed `constexpr` (loops/variables allowed), Binary literals (`0b1010`), Digit separators (`1'000`), Variable templates, Return type deduction.
*   **Library:** `std::make_unique`, `std::shared_timed_mutex`, `std::integer_sequence`, `std::exchange`, `std::quoted`.

#### **C++17 (ISO/IEC 14882:2017)** - *The Major Update*
**Released:** December 2017
*   **Language:** Structured bindings `auto [x,y] = p;`, `if constexpr`, Fold expressions `(... + args)`, Class Template Argument Deduction (CTAD), Inline variables, `__has_include`.
*   **Library:** `std::filesystem`, `std::optional`, `std::variant`, `std::any`, `std::string_view`, Parallel Algorithms (`std::execution::par`), `std::invoke`, `std::byte`, `std::pmr` (Polymorphic Memory Resources).

#### **C++20 (ISO/IEC 14882:2020)** - *The Gigantic Leap*
**Released:** December 2020
*   **Language:** Concepts (Constraints), Modules (`import/export`), Coroutines (`co_await`), Three-way comparison (`<=>`), Designated initializers `{.x=1}`, `consteval` (Immediate functions), `constinit`, Range-based for with init.
*   **Library:** Ranges (`std::ranges`), `std::span`, `std::format`, `std::jthread`, `std::stop_token`, `std::barrier`, `std::latch`, `std::semaphore`, `std::bit_cast`, `std::source_location`, Calendars & Timezones.

#### **C++23 (ISO/IEC 14882:2023)** - *The Completion*
**Released:** October 2023
*   **Language:** Deducing `this` (Explicit object parameter), `if consteval`, Multidimensional subscript `m[1,2]`, Static `operator()`, `auto(x)` decay copy.
*   **Library:** `std::print`, `std::println`, `std::expected` (Error handling), `std::mdspan`, `std::flat_map`, `std::flat_set`, `std::generator` (Synchronous coroutines), `std::stacktrace`, `std::stdatomic.h`.

### 2. Feature Matrix

| Feature | C++98 | C++11 | C++14 | C++17 | C++20 | C++23 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Memory** | `auto_ptr` | `unique_ptr` | `make_unique` | `pmr` | `shared_ptr` atomic | `out_ptr` |
| **Variables** | Type req. | `auto` | Var templates | Structured Bindings | `constinit` | - |
| **Loops** | `for(;;)` | Range-for | - | - | Range-for init | - |
| **Templates** | Basic | Variadic | Variable | Fold Expressions | Concepts | Deducing `this` |
| **Lambdas** | - | Basic | Generic | `constexpr` | Template | Recursive |
| **Concurrency** | - | `thread` | `shared_lock` | Parallel Algos | `jthread`, Latches | `stdatomic.h` |
| **String** | `string` | `to_string` | `quoted` | `string_view` | `format` | `print` |
| **Metaprog.** | Traits | `static_assert` | `integer_seq` | `if constexpr` | `consteval` | `if consteval` |
| **Modules** | - | - | - | - | **Modules** | `std` module |
| **Coroutines** | - | - | - | - | **Async** | `generator` |

### 3. Timeline & Release Accuracy

| Standard | ISO Publication | Codename | Compiler Flag (GCC/Clang) |
| :--- | :--- | :--- | :--- |
| **C++98** | 1998-09 | C++98 | `-std=c++98` |
| **C++03** | 2003-10 | C++03 | `-std=c++03` |
| **C++11** | 2011-09 | C++0x | `-std=c++11` |
| **C++14** | 2014-12 | C++1y | `-std=c++14` |
| **C++17** | 2017-12 | C++1z | `-std=c++17` |
| **C++20** | 2020-12 | C++2a | `-std=c++20` |
| **C++23** | 2023-10 | C++2b | `-std=c++23` |
| **C++26** | *Expected 2026* | C++2c | `-std=c++26` / `-std=c++2c` |

---

## Appendix G: C++ Standard Library Headers Reference

### Concepts & Utilities
*   `<concepts>` (C++20): Fundamental concepts library.
*   `<coroutine>` (C++20): Coroutine support library.
*   `<functional>`: Function objects, binder, and reference wrappers.
*   `<memory>`: Smart pointers and allocators.
*   `<tuple>`: Tuple library.
*   `<type_traits>`: Compile-time type information.
*   `<utility>`: Utility components (`std::pair`, `std::move`).

### Containers
*   `<array>` (C++11): Fixed-size array class.
*   `<deque>`: Double-ended queue.
*   `<list>`: Doubly-linked list.
*   `<map>`: Associative containers (Red-Black Tree).
*   `<queue>`: Queue adapter.
*   `<set>`: Associative containers (Red-Black Tree).
*   `<stack>`: Stack adapter.
*   `<unordered_map>` (C++11): Hash map.
*   `<vector>`: Dynamic array.
*   `<span >` (C++20): Non-owning view of contiguous memory.

### Algorithms & Iterators
*   `<algorithm>`: Algorithms that operate on ranges.
*   `<execution>` (C++17): Parallel algorithms.
*   `<iterator>`: Iterator primitives.
*   `<numeric>`: Numeric operations (`accumulate`, `reduce`).
*   `<ranges>` (C++20): Range primitives and views.

### Concurrency
*   `<atomic>` (C++11): Atomic operations.
*   `<barrier>` (C++20): Barriers.
*   `<condition_variable>` (C++11): Condition variables.
*   `<future>` (C++11): Futures and promises.
*   `<latch>` (C++20): Latches.
*   `<mutex>` (C++11): Mutual exclusion primitives.
*   `<semaphore>` (C++20): Semaphores.
*   `<shared_mutex>` (C++14): Shared mutexes.
*   `<thread>` (C++11): Thread class.

### Input/Output
*   `<filesystem>` (C++17): File system operations.
*   `<format>` (C++20): Formatting library.
*   `<fstream>`: File stream classes.
*   `<iostream>`: Standard I/O stream objects.
*   `<print>` (C++23): Print functions.
*   `<sstream>`: String stream classes.

### Numerics & Math
*   `<bit>` (C++20): Bit manipulation.
*   `<complex>`: Complex number arithmetic.
*   `<random>` (C++11): Random number generation.
*   `<ratio>` (C++11): Compile-time rational arithmetic.
*   `<valarray>`: Class for representing and manipulating arrays of values.
*   `<numbers>` (C++20): Mathematical constants.

# Volume VI: Deep Internals

## <a name="chapter-24-cunderthehood"></a>CHAPTER 24: C++ UNDER THE HOOD

To truly master C++, you must understand what the compiler generates.

### 14.1 Object Layout & ABI (Itanium C++ ABI)
How does `virtual` work?

```cpp
class Base {
    int64_t id;
public:
    virtual void func() {}
};

class Derived : public Base {
    int64_t data;
public:
    void func() override {}
};
```

**Memory Layout (64-bit system):**
```text
[ vptr (8 bytes) ] -> [ vtable for Base ]
[ id   (8 bytes) ]
```
For `Derived`:
```text
[ vptr (8 bytes) ] -> [ vtable for Derived ]
[ id   (8 bytes) ]
[ data (8 bytes) ]
```
*   **vptr**: Hidden pointer added to classes with virtual functions.
*   **vtable**: Static table of function pointers.
*   **Alignment**: Data is padded to align with word boundaries.

### 14.2 Small String Optimization (SSO)
`std::string` doesn't always allocate heap memory.

```cpp
std::string s = "Hello"; // 5 chars
// Layout typically (24-32 bytes):
// [ size (8) ] [ capacity (8) ] [ pointer (8) ]  <-- Normal mode
// [ size (1) ] [ ... chars 22 bytes ...     ]  <-- SSO mode (Union)
```
Strings shorter than 15-22 chars (depending on libc++) live entirely on the stack.

### 14.3 Return Value Optimization (RVO)
Copy elision is mandatory in C++17.

```cpp
struct BigObject { int data[1000]; };

BigObject create() {
    BigObject obj;
    // ... fill obj ...
    return obj; // No copy, no move. Constructed directly in caller's stack frame.
}

BigObject x = create();
```

---

## <a name="chapter-25-masteringthememorymodel"></a>CHAPTER 25: MASTERING THE MEMORY MODEL

The C++ Memory Model defines how threads interact through memory.

### 15.1 Atomicity vs Ordering
*   **Atomicity**: An operation is indivisible (all or nothing).
*   **Ordering**: The order in which operations are observed by other threads.

`std::atomic<int>` guarantees atomicity, but `memory_order` controls ordering.

### 15.2 Memory Orders Deep Dive

1.  **`memory_order_relaxed`**: No ordering constraints. Only atomicity.
    *   Use for: Incrementing stats counters.
    ```cpp
    cnt.fetch_add(1, std::memory_order_relaxed);
    ```

2.  **`memory_order_acquire`**: Read operation.
    *   Guarantee: No reads/writes in the current thread can be reordered *before* this load.
    *   Use with: Release.

3.  **`memory_order_release`**: Write operation.
    *   Guarantee: No reads/writes in the current thread can be reordered *after* this store.
    *   Use for: Publishing data.

4.  **`memory_order_seq_cst`** (Default): Sequentially Consistent.
    *   Guarantee: A total global ordering exists. Expensive.

### 15.3 The Happens-Before Relationship
If Operation A *happens-before* Operation B:
1.  A is sequenced before B (same thread).
2.  A *synchronizes-with* B (inter-thread, e.g., A releases, B acquires).

**Example: Lock-Free Flag**
```cpp
std::atomic<int> data = 0;
std::atomic<bool> ready = false;

void producer() {
    data.store(42, std::memory_order_relaxed);
    ready.store(true, std::memory_order_release); // "Publish"
}

void consumer() {
    while (!ready.load(std::memory_order_acquire)); // "Acquire"
    assert(data.load(std::memory_order_relaxed) == 42); // Guaranteed 42
}
```

---

## <a name="chapter-26-writingaccompilerbasics"></a>CHAPTER 26: WRITING A C++ COMPILER (BASICS)

To understand C++, build a toy compiler.

### 18.1 Lexical Analysis (Tokenizer)
Converting source code into tokens.

```cpp
enum class TokenType { Int, Identifier, Plus, Minus, End };

struct Token {
    TokenType type;
    std::string text;
};

std::vector<Token> tokenize(std::string_view source) {
    std::vector<Token> tokens;
    // ... implementation ...
    return tokens;
}
```

### 18.2 Parsing (Recursive Descent)
Building an Abstract Syntax Tree (AST).

```cpp
struct ASTNode { virtual ~ASTNode() = default; };
struct BinaryExpr : ASTNode {
    std::unique_ptr<ASTNode> left, right;
    char op;
};

// parseExpression() calls parseTerm(), etc.
```

### 18.3 Semantic Analysis (Types & Scopes)
Before generating code, we must validate it.

**Symbol Table:**
```cpp
struct Symbol { string type; };
using Scope = map<string, Symbol>;
vector<Scope> scopes; // Stack of scopes

void enter_scope() { scopes.push_back({}); }
void exit_scope() { scopes.pop_back(); }
```

**Type Checking:**
Recursively visit the AST.
*   `BinaryExpr`: Check left.type == right.type.
*   `Variable`: Check if exists in symbol table.

---

## <a name="chapter-27-writingagarbagecollector"></a>CHAPTER 27: WRITING A GARBAGE COLLECTOR

C++ has RAII, but implementing a GC teaches you about the stack and object graph.

### 29.1 Mark-and-Sweep Basics
1.  **Roots**: Pointers on the stack/globals.
2.  **Mark**: Traverse object graph from roots, marking reachable objects.
3.  **Sweep**: Iterate heap, free unmarked objects.

```cpp
struct GCObject {
    bool marked = false;
    virtual ~GCObject() = default;
};

class VM {
    std::vector<GCObject*> heap;
    std::vector<GCObject*> roots; // Pointers currently on stack
    
public:
    void mark() {
        for (auto* obj : roots) mark_object(obj);
    }
    
    void mark_object(GCObject* obj) {
        if (!obj || obj->marked) return;
        obj->marked = true;
        // ... traverse children ...
    }
    
    void sweep() {
        auto it = std::remove_if(heap.begin(), heap.end(), [](GCObject* obj) {
            if (!obj->marked) {
                delete obj;
                return true;
            }
            obj->marked = false; // Reset for next cycle
            return false;
        });
        heap.erase(it, heap.end());
    }
};
```

---

## <a name="chapter-28-thestandardlibraryfromscratch"></a>CHAPTER 28: THE STANDARD LIBRARY FROM SCRATCH

Implementing core STL components to understand their cost.

### 19.1 Implementing my::vector
Managing raw memory, growth, and construction.

```cpp
template<typename T>
class Vector {
    T* data = nullptr;
    size_t sz = 0;
    size_t cap = 0;
    
public:
    void push_back(const T& val) {
        if (sz == cap) {
            reallocate(cap == 0 ? 1 : cap * 2);
        }
        new (data + sz) T(val); // Placement new
        sz++;
    }
    
private:
    void reallocate(size_t new_cap) {
        T* new_data = static_cast<T*>(::operator new(new_cap * sizeof(T)));
        // Move old elements...
        // Delete old memory...
        data = new_data;
        cap = new_cap;
    }
};
```

### 19.2 Implementing my::shared_ptr
Understanding the Control Block.

```cpp
template<typename T>
class SharedPtr {
    T* ptr;
    struct ControlBlock {
        std::atomic<int> ref_count{1};
    } *cb;
    
public:
    SharedPtr(T* p) : ptr(p), cb(new ControlBlock()) {}
    
    SharedPtr(const SharedPtr& other) {
        ptr = other.ptr;
        cb = other.cb;
        if (cb) cb->ref_count++;
    }
    
    ~SharedPtr() {
        if (cb && --cb->ref_count == 0) {
            delete ptr;
            delete cb;
        }
    }
};
```

---

# Volume VII: Specialized Domains

## <a name="chapter-29-distributedc"></a>CHAPTER 29: DISTRIBUTED C++

Moving beyond a single process: Networking, RPC, and Consensus.

### 16.1 Serialization (Binary Protocols)
Efficiently packing data for network transmission.

```cpp
#include <vector>
#include <cstring>
#include <string>

// Simple Binary Serializer
class Buffer {
    std::vector<uint8_t> data;
public:
    template<typename T>
    void write(const T& val) {
        static_assert(std::is_trivially_copyable_v<T>);
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&val);
        data.insert(data.end(), ptr, ptr + sizeof(T));
    }

    void write_string(const std::string& s) {
        write<uint32_t>(s.size());
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(s.data());
        data.insert(data.end(), ptr, ptr + s.size());
    }
    
    const uint8_t* begin() const { return data.data(); }
    size_t size() const { return data.size(); }
};
```

### 16.2 RPC (Remote Procedure Call) Concept
Calling a function on another machine.

**Stub Interface:**
```cpp
// User Code
// auto result = service.Add(5, 3);

// Generated Stub
int Add(int a, int b) {
    Buffer buf;
    buf.write(101); // Function ID for 'Add'
    buf.write(a);
    buf.write(b);
    return network.send_and_wait(buf); // Blocks
}
```

### 16.3 Consensus (Raft Basics)
Distributed systems need to agree on state.

**Raft State Machine:**
```cpp
enum class State { Follower, Candidate, Leader };

struct Node {
    State state = State::Follower;
    int current_term = 0;
    int voted_for = -1;
    
    void on_timeout() {
        if (state == State::Follower) {
            state = State::Candidate;
            current_term++;
            voted_for = my_id;
            request_votes();
        }
    }
};
```

---

## <a name="chapter-30-networkingfromscratch"></a>CHAPTER 30: NETWORKING FROM SCRATCH

Understanding `asio` requires understanding BSD Sockets.

### 28.1 Berkeley Sockets API
The foundation of the Internet.

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    
    int new_socket = accept(server_fd, nullptr, nullptr);
    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    
    // Send HTTP response
    const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello!";
    write(new_socket, hello, strlen(hello));
    
    close(new_socket);
    close(server_fd);
    return 0;
}
```

### 28.2 Non-Blocking I/O & Epoll (Linux)
How Nginx/Node.js handle 10k connections.

```cpp
// 1. Create epoll instance
int epoll_fd = epoll_create1(0);

// 2. Add server socket
epoll_event event;
event.events = EPOLLIN; // Read available
event.data.fd = server_fd;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

// 3. Event Loop
while (true) {
    epoll_event events[10];
    int event_count = epoll_wait(epoll_fd, events, 10, -1);
    for (int i = 0; i < event_count; i++) {
        if (events[i].data.fd == server_fd) {
            // Accept new connection...
        } else {
            // Read data...
        }
    }
}
```

---

## <a name="chapter-31-cinthecloud"></a>CHAPTER 31: C++ IN THE CLOUD

Modern C++ is a first-class citizen in Cloud Native architectures.

### 20.1 Microservices with C++
Using frameworks like **Drogon** or **Userver** (Yandex) for high-throughput services.

**Example: Simple HTTP Endpoint (Drogon style)**
```cpp
// Controller
void Handler::get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback) {
    auto resp = HttpResponse::newHttpResponse();
    resp->setBody("Hello from High-Performance Microservice!");
    callback(resp);
}
```

### 20.2 Serverless C++ (AWS Lambda)
Using the AWS Lambda C++ Runtime to run native binaries.
*   **Cold Start**: < 5ms (vs 100ms+ for Java/Node).
*   **Cost**: Lower duration due to speed.

```cpp
#include <aws/lambda-runtime/runtime.h>

aws::lambda_runtime::invocation_response handler(aws::lambda_runtime::invocation_request const& req) {
    return aws::lambda_runtime::invocation_response::success("Processed!", "application/json");
}

int main() {
    aws::lambda_runtime::run_handler(handler);
    return 0;
}
```

---

## <a name="chapter-32-crossplatformdevelopment"></a>CHAPTER 32: CROSS-PLATFORM DEVELOPMENT

Write once, run everywhere (Desktop, Web, Mobile).

### 21.1 WebAssembly (Wasm) with Emscripten
Compiling C++ to run in the browser.

```bash
emcc main.cpp -o index.html -s WASM=1
```

```cpp
#include <emscripten/emscripten.h>

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    int add(int a, int b) {
        return a + b; // callable from JavaScript
    }
}
```

### 21.2 Mobile C++ (Android NDK & JNI)
Integrating C++ with Java/Kotlin.

```cpp
#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_myapp_MainActivity_stringFromJNI(JNIEnv* env, jobject /* this */) {
    return env->NewStringUTF("Hello from C++");
}
```

---

## <a name="chapter-33-guidevelopmentwithc"></a>CHAPTER 33: GUI DEVELOPMENT WITH C++

Building desktop applications and tools.

### 22.1 Qt Framework (Retained Mode)
Qt uses a unique Signal/Slot mechanism (via MOC - Meta-Object Compiler).

```cpp
// MainWindow.h
class MainWindow : public QMainWindow {
    Q_OBJECT // Macro for MOC
public:
    MainWindow(QWidget *parent = nullptr);
public slots:
    void handleButton(); // Slot
};

// MainWindow.cpp
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QPushButton *button = new QPushButton("Click me", this);
    connect(button, &QPushButton::clicked, this, &MainWindow::handleButton);
}
```

### 22.2 Dear ImGui (Immediate Mode)
Ideal for game engines and internal tools. Re-renders UI every frame.

```cpp
// Main Loop
void Render() {
    ImGui::Begin("Debug Tools");
    static float col[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::ColorEdit3("Background Color", col);
    if (ImGui::Button("Reset")) {
        col[0] = col[1] = col[2] = 0.0f;
    }
    ImGui::End();
}
```

---

## <a name="chapter-34-scientificcomputinggpu"></a>CHAPTER 34: SCIENTIFIC COMPUTING & GPU

C++ is the language of high-performance math.

### 23.1 Eigen (Linear Algebra)
Template-heavy library that avoids temporaries using Expression Templates.

```cpp
#include <Eigen/Dense>
using Eigen::MatrixXd;

void solve_system() {
    MatrixXd A(3, 3);
    A << 1, 2, 3,
         4, 5, 6,
         7, 8, 10;
    
    Eigen::VectorXd b(3);
    b << 3, 3, 4;
    
    Eigen::VectorXd x = A.colPivHouseholderQr().solve(b);
}
```

### 23.2 CUDA (GPU Programming)
Running C++ directly on NVIDIA GPUs.

```cpp
// Kernel (runs on GPU)
__global__ void vectorAdd(float* A, float* B, float* C, int N) {
    int i = blockDim.x * blockIdx.x + threadIdx.x;
    if (i < N) C[i] = A[i] + B[i];
}

// Host (runs on CPU)
void launch_kernel(float* d_A, float* d_B, float* d_C, int N) {
    int threadsPerBlock = 256;
    int blocksPerGrid = (N + threadsPerBlock - 1) / threadsPerBlock;
    vectorAdd<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, N);
}
```

---

## <a name="chapter-35-interoperability"></a>CHAPTER 35: INTEROPERABILITY

C++ is the dark matter of the software universe: it binds everything together.

### 35.1 Python Bindings (pybind11)
Bridging the gap between Python's ease of use and C++'s raw power.
*   **The Problem:** Python objects are ref-counted (`PyObject*`). C++ has RAII. Who owns the pointer?
*   **Return Value Policies:**
    *   `py::return_value_policy::copy`: Python gets a copy (Safe).
    *   `py::return_value_policy::reference`: Python references C++ memory (Dangous if C++ deletes it).
    *   `py::return_value_policy::take_ownership`: Python takes over `delete`.

```cpp
#include <pybind11/pybind11.h>
namespace py = pybind11;

struct Pet {
    std::string name;
    Pet(const std::string &name) : name(name) { }
};

PYBIND11_MODULE(example, m) {
    py::class_<Pet>(m, "Pet")
        .def(py::init<const std::string &>())
        .def_readwrite("name", &Pet::name);
}
```

### 35.2 Java Native Interface (JNI)
The bridge to the Enterprise (and Android).
*   **Cost:** Crossing the JVM boundary is expensive (pointer chasing, pinning objects).
*   **Pitfall:** `JNIEnv*` is thread-local. Do not share it between threads.
*   **Local References:** JNI creates local refs for every object returned. If you don't `DeleteLocalRef` in a loop, the JVM OOMs.

```cpp
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_MyClass_nativeMethod(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Hello from C++");
}
```

### 35.3 Stable C ABI
To speak to Rust, C#, or Go, use the lingua franca: C.
*   **`extern "C"`**: Disables C++ name mangling (e.g., `_Z3foov` becomes `foo`).
*   **Struct Layout:** Use `StandardLayoutType` structs (no virtuals, all public).

---

## <a name="chapter-36-securityengineering"></a>CHAPTER 36: SECURITY ENGINEERING

### 36.1 Fuzzing (libFuzzer / AFL++)
Unit tests test what you *know*. Fuzzing tests what you *don't*.
*   **Coverage-Guided:** The fuzzer instruments binaries to see which inputs explore new code paths.
*   **Sanitizers:** Always fuzz with AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan) enabled.

### 36.2 Cryptography & Timing Attacks
NEVER write your own crypto. Use `libsodium` or `BoringSSL`.
*   **The Trap:** `memcmp(hash1, hash2, 32)` exits early if the first byte differs.
*   **The Attack:** Attacker measures time. If it takes longer, they guessed the first byte right.
*   **The Fix:** Constant-time comparison.
    ```cpp
    // libsodium's constant time check
    if (crypto_verify_32(hash1, hash2) != 0) { /* Error */ }
    ```

### 36.3 Side-Channel Mitigations (Spectre)
Modern CPUs execute instructions speculatively.
*   **Scenario:** `if (x < array_len) { val = array[x]; }`
*   **Attack:** CPU predicts `true`, loads `array[x]` (where `x` is out of bounds secret). Even if checks fail, `array[x]` is now in L1 cache.
*   **Mitigation:** `LFENCE` (Load Fence) or `std::clamp` indices to 0 on failure (masking).

---

## <a name="chapter-37-specializeddomains"></a>CHAPTER 37: SPECIALIZED DOMAINS

### 37.1 Game Development (Data-Oriented Design)
OOP is cache-poison. Games use **Entity-Component-Systems (ECS)**.
*   **AoS (Array of Structs):** `[Pos, Vel], [Pos, Vel]` -> Bad stride.
*   **SoA (Structure of Arrays):** `[Pos, Pos], [Vel, Vel]` -> SIMD friendly.

### 37.2 Embedded Systems
*   **Memory-Mapped I/O:** Casting integer addresses to pointers.
*   **`volatile`**: Tells compiler "Hardware changes this value, do not optimize reads".
*   **Freestanding Implementation:** No OS, no heap (`malloc` is a myth).

### 37.3 High-Frequency Trading (HFT)
*   **Kernel Bypass:** `Solarflare OpenOnload` maps NIC ring buffers directly to userspace, skipping the Linux Kernel (save ~2-3 microseconds).
*   **Warm-up:** Pinging order gateways to ensure the TCP congestion window is open and CPU is in C0 state (max turbo).

### 37.4 Automotive (MISRA C++ / AUTOSAR)
*   **No Dynamic Memory:** All containers have fixed capacity (`etl::vector` or `boost::static_vector`).
*   **No Exceptions:** `try-catch` adds binary size and non-deterministic unwind paths. Use `std::expected` or error codes.
*   **Static Analysis:** Code must pass MISRA-2008 rules (e.g., "Rule 5-0-15: Array indexing shall be checked").

```cpp
// Stack-only pattern (Automotive safe)
template <typename T, size_t N>
class FixedVector {
    T data[N];
    size_t count = 0;
public:
    void push_back(const T& val) {
        if (count < N) data[count++] = val;
    }
};
```

---

## FINAL COMPREHENSIVE CHECKLIST

### C++98/03 Foundation
- ☐ Variables and basic types
- ☐ Operators and control flow
- ☐ Functions and overloading
- ☐ Arrays and pointers
- ☐ Classes and objects
- ☐ Constructors and destructors
- ☐ Inheritance
- ☐ Virtual functions and polymorphism
- ☐ STL containers (vector, list, map, set)
- ☐ Algorithms
- ☐ Strings and I/O

### C++11 Major Features
- ☐ Auto type deduction
- ☐ nullptr and nullptr_t
- ☐ Uniform initialization {}
- ☐ Range-based for loops
- ☐ Smart pointers (unique_ptr, shared_ptr)
- ☐ Move semantics
- ☐ Rvalue references
- ☐ Lambda functions
- ☐ Variadic templates
- ☐ std::array and std::tuple
- ☐ std::unordered_map and std::unordered_set

### C++14 Improvements
- ☐ Generic lambdas
- ☐ Return type deduction
- ☐ std::make_unique
- ☐ Digit separators (1'000'000)
- ☐ decltype(auto)

### C++17 Modern Features
- ☐ Structured bindings
- ☐ std::optional
- ☐ std::variant
- ☐ std::any
- ☐ if constexpr
- ☐ Fold expressions
- ☐ Filesystem library
- ☐ Parallel algorithms
- ☐ std::string_view

### C++20 Revolutionary
- ☐ Concepts and constraints
- ☐ Ranges
- ☐ Coroutines
- ☐ Modules
- ☐ Spaceship operator <=>
- ☐ Designated initializers
- ☐ Requires expressions

### C++23 Latest
- ☐ Deducing this
- ☐ std::expected
- ☐ Literal classes in constexpr

### Advanced Concepts
- ☐ Template metaprogramming
- ☐ CRTP (Curiously Recurring Template Pattern)
- ☐ SFINAE (Substitution Failure Is Not An Error)
- ☐ Type traits
- ☐ Memory management (stack vs heap)
- ☐ Smart pointers (unique_ptr, shared_ptr, weak_ptr)
- ☐ Move semantics and forwarding
- ☐ Perfect forwarding with std::forward

### Concurrency
- ☐ Threading basics
- ☐ Mutexes and locks
- ☐ Condition variables
- ☐ Atomic operations
- ☐ Memory ordering
- ☐ Lock-free programming

### Performance & Optimization
- ☐ Memory profiling
- ☐ Cache optimization
- ☐ SIMD and vectorization
- ☐ Compiler flags (-O2, -O3)
- ☐ Profiling tools (perf, valgrind)

### STL Mastery
- ☐ All container types
- ☐ All algorithms
- ☐ Iterators
- ☐ Custom comparators
- ☐ Ranges (C++20)

### Design Patterns
- ☐ Singleton
- ☐ Factory
- ☐ Observer
- ☐ Strategy
- ☐ CRTP

### Professional Development
- ☐ CMake build system
- ☐ Testing frameworks (Google Test)
- ☐ Debugging (gdb)
- ☐ Profiling
- ☐ Code organization
- ☐ RAII pattern
- ☐ Error handling
- ☐ Memory leak detection

---

## Key Insights for C++ Mastery

1. **RAII** = Guaranteed resource cleanup
2. **Smart Pointers** = No manual memory management
3. **Move Semantics** = Zero-copy optimization
4. **Const Correctness** = Prevents bugs, enables optimizations
5. **Templates** = Compile-time computation
6. **Concepts** (C++20) = Type-safe constraints
7. **Ranges** (C++20) = Composable algorithms
8. **Coroutines** (C++20) = Async programming made easy
9. **Atomic Operations** = Safe concurrent access
10. **Performance** = Measure, profile, then optimize

---

## Learning Path

1. **Week 1-2**: C++98 basics (variables, control flow, functions)
2. **Week 3-4**: Classes and OOP (constructors, inheritance, polymorphism)
3. **Week 5**: STL containers and algorithms
4. **Week 6-7**: C++11 (smart pointers, lambdas, move semantics)
5. **Week 8**: C++14 and C++17 features
6. **Week 9**: C++20 features (concepts, ranges)
7. **Week 10+**: Advanced topics (metaprogramming, concurrency, optimization)

---

## Resources

### Official Documentation
- cppreference.com - C++ standard library reference
- en.cppreference.com - Excellent resource
- isocpp.org - C++ standards committee

### Books
- "A Tour of C++" by Bjarne Stroustrup
- "Effective Modern C++" by Scott Meyers
- "C++ Concurrency in Action" by Anthony Williams

### Practice
- LeetCode.com
- HackerRank.com
- Codeforces.com
- ProjectEuler.net

---

**You are now equipped to master C++ from absolute zero to expert level!** 🚀

*Last Updated: December 2025*
*C++ Versions Covered: C++98 through C++23*

---

# Volume VIII: Expert Mastery

## <a name="chapter-38-abaproblemmemoryreclamation"></a>CHAPTER 38: ABA PROBLEM & MEMORY RECLAMATION

In the rarefied air of lock-free programming, the **ABA Problem** is the dragon that guards the gate. Conquering it requires understanding the very fabric of memory lifecycles.

### 1. The ABA Problem Explained
The Compare-And-Swap (CAS) primitive (`std::atomic::compare_exchange_weak`) checks if a value is *equal* to an expected value. It does **not** check if it is the *same* object.

**The Scenario:**
1.  Thread 1 reads pointer `A` from a lock-free stack top.
2.  Thread 1 is preempted.
3.  Thread 2 pops `A`, frees it, then pushes `B`, then pushes a *new* object allocated at address `A` (recycled memory).
4.  Thread 1 wakes up, performs CAS. The address is still `A`. CAS succeeds.
5.  **Catastrophe:** Thread 1 has popped the *new* `A`, but its local logic assumes it's the *old* `A` (e.g., pointing to `B` as the next node). The stack is now corrupted.

### 2. Solution I: Tagged Pointers (Version Counters)
Pack a version counter into the unused bits of a pointer (usually top 16 bits on 64-bit systems).
*   **Mechanism:** Every modification increments the counter. `Ptr(A, v1)` != `Ptr(A, v2)`.
*   **Limitation:** Reduces addressable memory space; requires platform-specific bit manipulation.

```cpp
// Example of a 64-bit tagged pointer
struct TaggedPtr {
    uint64_t data; // 48 bits pointer, 16 bits tag

    TaggedPtr(void* ptr, uint16_t tag) {
        data = (reinterpret_cast<uint64_t>(ptr) & 0x0000FFFFFFFFFFFF) | (static_cast<uint64_t>(tag) << 48);
    }
    
    void* get_ptr() const { return reinterpret_cast<void*>(data & 0x0000FFFFFFFFFFFF); }
    uint16_t get_tag() const { return static_cast<uint16_t>(data >> 48); }
};
```

### 3. Solution II: Hazard Pointers (The Gold Standard)
A **Hazard Pointer (HP)** is a thread-local signal saying "I am reading this object, do not delete it."

**The Protocol:**
1.  **Reader:** publish the pointer `P` to a thread-local HP slot.
2.  **Reader:** Verify `P` is still in the data structure. If not, retry.
3.  **Writer (Deleter):** Unlink `P` from the structure.
4.  **Writer:** Check all other threads' HPs.
    *   If `P` is found in any HP, add `P` to a "Retire List" (do not `delete` yet).
    *   If `P` is not found, `delete` immediately.
5.  **Cleanup:** Periodically scan the Retire List and free objects no longer protected by HPs.

*   **Pros:** Wait-free readers, deterministic memory bound.
*   **Cons:** Heavy memory barrier usage (Store-Load fence needed after publishing HP).

### 4. Solution III: Epoch-Based Reclamation (EBR)
Used by `malloc` implementations and databases (like Silo).
*   **Concept:** A Global Epoch counter (E) and per-thread Local Epochs (e_t).
*   **Operation:**
    1.  Global Epoch `E` increments periodically.
    2.  Threads update `e_t = E` when entering a critical section.
    3.  Objects retired in Epoch `E` can be safely deleted when all threads have reached Epoch `E+1` or higher.
*   **Pros:** Extremely fast (just checking integers).
*   **Cons:** One stalled thread prevents *all* memory reclamation (OOM risk).

---

## <a name="chapter-39-templatemetaprogrammingpatterns"></a>CHAPTER 39: TEMPLATE METAPROGRAMMING PATTERNS

Moving computation from runtime to compile-time saves cycles and enables zero-cost abstractions.

### 1. Expression Templates (Lazy Evaluation)
Avoid temporary objects in math operations.
**Naive:** `Vector sum = A + B + C;` creates `tmp = A+B`, then `sum = tmp+C`. Allocations!
**Expression Template:** `A + B` returns a lightweight `Sum<Vec, Vec>` object.
```cpp
template <typename L, typename R>
struct Sum {
    const L& l; const R& r;
    auto operator[](size_t i) const { return l[i] + r[i]; }
};

template <typename L, typename R>
auto operator+(const L& l, const R& r) {
    return Sum<L, R>{l, r};
}

// Usage
// Vector result = A + B + C; 
// Becomes: result[i] = A[i] + B[i] + C[i] in a single loop!
```

### 2. Type Erasure (The `std::any` Pattern)
Polymorphism without inheritance.
*   **Technique:** Hold a `void*` or a `unique_ptr<Base>`, where `Base` is an abstract class inside a templated wrapper.
*   **Example:** `std::function`, `std::any`.

### 3. The Detection Idiom (void_t)
Check if a type has a member function or typedef at compile time.
```cpp
template <typename, typename = std::void_t<>>
struct has_serialize : std::false_type {};

template <typename T>
struct has_serialize<T, std::void_t<decltype(std::declval<T>().serialize())>> : std::true_type {};

static_assert(has_serialize<MyClass>::value, "MyClass must implement serialize()");
```
*Note: In C++20, simply use Concepts.*

### 4. Policy-Based Design
Compose behavior via template arguments.
```cpp
template <typename T, typename CheckingPolicy, typename ThreadingPolicy>
class SmartPtr : public CheckingPolicy, public ThreadingPolicy {
    T* ptr;
    // ...
};
// User chooses: SmartPtr<int, NoCheck, MultiThreaded>
```

---

## <a name="chapter-40-highperformancedatastructures"></a>CHAPTER 40: HIGH-PERFORMANCE DATA STRUCTURES

When `std::unordered_map` is too slow, we descend into the hardware.

### 1. The Disruptor (Ring Buffer on Steroids)
A lock-free ring buffer designed for high-throughput messaging (LMAX Trading).
*   **Key Concept:** Pre-allocated memory, sequence numbers, and "barriers".
*   **False Sharing Prevention:** Padding sequence counters to 64 bytes (cache line).
*   **Batching:** Consumers process up to the known "published" sequence.

### 2. Swiss Table (Open Addressing + Metadata)
Used in `absl::flat_hash_map`.
*   **Structure:** Arrays of control bytes (metadata) and data slots.
*   **Control Byte:** 7 bits of hash + 1 bit for empty/deleted.
*   **SIMD Probing:** Load 16 control bytes into a vector register (SSE/AVX). Compare all 16 tags in parallel to find the slot.
*   **Result:** Drastically fewer cache misses than chaining.

### 3. Burst Tries / Judy Arrays
Cache-efficient digital trees (tries) for integer keys.
*   **Idea:** Nodes dynamically change type based on population (Linear list -> Bitmap -> Sub-trie).

### 4. Slot Map
O(1) insertion, deletion, and access with stable "handles" (indices) instead of pointers.
*   **Generational Indices:** Handle = `[Index | Generation]`. Prevents "Dangling Reference" equivalent (accessing a slot that was re-used for a new object).

---

## <a name="chapter-41-realtimeaudiosignalprocessing"></a>CHAPTER 41: REAL-TIME AUDIO & SIGNAL PROCESSING

**The Golden Rule:** In the audio callback, thou shalt not:
1.  **Block** (No Mutexes).
2.  **Allocate** (No `malloc`/`new`).
3.  **Perform I/O** (No file reads, no `printf`).

### 1. Lock-Free IPC (SPSC Queue)
Communication between the UI Thread (writes parameters) and Audio Thread (reads parameters) must be wait-free.
*   **Structure:** Single-Producer Single-Consumer Circular Buffer.
*   **Atomic Indices:** `head` (write) and `tail` (read) are `std::atomic<size_t>`.

### 2. SIMD for DSP
Digital Signal Processing (Filters, FFT) is purely math-bound.
*   **Auto-vectorization:** Help the compiler (restrict pointers, fixed loop bounds).
*   **Intrinsics:** Manually using `<immintrin.h>` for AVX-512 processing of 16 samples per cycle.
*   **Biquad Filter:** The workhorse of EQ.
    ```cpp
    // Processing 4 stereo channels (8 floats) at once with AVX
    __m256 samples = _mm256_load_ps(input_ptr);
    __m256 result = _mm256_add_ps(_mm256_mul_ps(samples, b0), ...);
    ```

### 3. Double Buffering
For spectral analysis (FFT) which requires blocks (e.g., 1024 samples), while audio comes in small chunks (e.g., 64 samples).
*   **Technique:** Fill Buffer A. When full, signal worker thread to process A, swap to filling Buffer B.

---

## <a name="chapter-42-roboticsros2development"></a>CHAPTER 42: ROBOTICS & ROS2 DEVELOPMENT

Robotics is where Soft Real-Time (Navigation) meets Hard Real-Time (Motor Control).

### 1. ROS2 Architecture & DDS
Robot Operating System 2 (ROS2) runs on top of DDS (Data Distribution Service).
*   **Nodes:** Independent processes.
*   **Topics:** Pub/Sub channels.
*   **Services:** RPC-style calls.

### 2. Zero-Copy Transport (Iceoryx)
Standard ROS2 serialization is slow for large data (LiDAR point clouds, 4K video).
*   **Solution:** Shared Memory.
*   **Mechanism:**
    1.  Publisher requests a memory chunk from shared segment.
    2.  Publisher writes data directly.
    3.  Publisher sends the *offset* (pointer) to Subscriber.
    4.  Subscriber reads directly. **Zero copies.**

### 3. Real-Time Executors
Standard ROS2 executors can suffer from priority inversion.
*   **Callback-group-level Executor:** Prioritize "Safety Stop" topic callbacks over "Camera Logging" callbacks.

### 4. Custom Allocators (`std::pmr`)
In the real-time control loop (1kHz+), heap fragmentation is fatal.
*   **Pattern:** Use `std::pmr::monotonic_buffer_resource` on the stack for message generation.

---

## <a name="chapter-43-machinelearninginfrastructure"></a>CHAPTER 43: MACHINE LEARNING INFRASTRUCTURE

Deep Learning frameworks (PyTorch, TensorFlow) are C++ engines wrapped in Python.

### 1. Tensor Memory Layout
A Tensor is a block of memory + a "View".
*   **Strides:** The number of elements to skip to reach the next index in a dimension.
    *   `Element(i, j) = data[i * stride_i + j * stride_j]`
*   **Contiguity:** Transposing a tensor (`A.T`) usually just modifies strides, touching **zero** data.

### 2. Broadcasting Implementation
How to add `[32, 1]` vector to `[32, 100]` matrix?
*   **Virtual Expansion:** Set stride for the dimension of size `1` to `0`. Accessing that dimension repeatedly reads the same value.

### 3. Automatic Differentiation (Autograd)
*   **Computational Graph:** Directed Acyclic Graph (DAG) of operations.
*   **Reverse Mode (Backprop):**
    1.  Forward pass: Compute `y = f(x)`, store intermediate values (tape).
    2.  Backward pass: Compute `dL/dx` using stored values and chain rule.
*   **Implementation:** `Node` class with `virtual Tensor backward(Tensor grad)`.

### 4. Operator Fusion
Optimizing `ReLU(Add(MatMul(A, B), C))` into a single kernel launch to minimize VRAM bandwidth.

---

## <a name="chapter-44-databaseinternalslsmtrees"></a>CHAPTER 44: DATABASE INTERNALS (LSM TREES)

How RocksDB and LevelDB achieve millions of writes per second.

### 1. The Write Problem
Random writes to disk (B-Tree update) are slow (IOPS bottleneck). Sequential writes are fast.

### 2. LSM Tree (Log-Structured Merge Tree)
*   **MemTable:** In-memory sorted structure (SkipList or Red-Black Tree).
    *   Writes go here first (fast RAM access).
    *   WAL (Write Ahead Log) on disk for durability.
*   **Immutable MemTable:** When MemTable is full, it becomes immutable and is flushed to disk.
*   **SSTable (Sorted String Table):** The flushed file on disk. Key-Value pairs sorted by Key.
*   **Compaction:** Background process merges multiple SSTables, discarding overwritten/deleted keys (Leveled Compaction).

### 3. Bloom Filters
To read a key, we might have to check *all* SSTables. Slow!
*   **Optimization:** Each SSTable has a Bloom Filter.
*   **Check:** If Bloom says "No", key is definitely not in this file. Skip it.

### 4. Memory Mapped I/O (`mmap`)
Mapping the SSTable file directly into virtual address space. The OS manages paging.
*   **Benefits:** Zero-copy from disk cache to user space.
*   **Risks:** `SIGBUS` if file is truncated; lack of control over eviction.

---

# Volume IX: Final Reference

## <a name="chapter-45-theultimatealgorithmreference"></a>CHAPTER 45: THE ULTIMATE ALGORITHM REFERENCE

Stop writing loops. Use the STL.

### 27.1 Non-Modifying Sequence Operations
*   `std::all_of(begin, end, pred)`: True if all match.
*   `std::any_of(begin, end, pred)`: True if any match.
*   `std::none_of(begin, end, pred)`: True if none match.
*   `std::for_each(begin, end, func)`: Apply function to all.
*   `std::count(begin, end, val)`: Count occurrences.
*   `std::mismatch(b1, e1, b2)`: Find first difference.
*   `std::find(begin, end, val)`: Linear search.

### 27.2 Modifying Sequence Operations
*   `std::copy(b, e, out)`: Copy range.
*   `std::transform(b, e, out, op)`: Map function over range.
*   `std::generate(b, e, gen)`: Fill with generator.
*   `std::remove_if(b, e, pred)`: **Erase-Remove Idiom** step 1. Move valid elements to front.
*   `std::replace(b, e, old, new)`: Replace values.
*   `std::unique(b, e)`: Remove consecutive duplicates.

### 27.3 Partitioning
*   `std::partition(b, e, pred)`: Reorder so true predicates come first. O(N).
*   `std::stable_partition`: Preserves relative order.

### 27.4 Sorting
*   `std::sort(b, e)`: IntroSort (Quick + Heap + Insertion). O(N log N).
*   `std::partial_sort(b, mid, e)`: Top K elements sorted.
*   `std::nth_element(b, nth, e)`: Element at `nth` is what it would be if sorted. O(N).

### 27.5 Binary Search (On Sorted Ranges)
*   `std::lower_bound(b, e, val)`: First element `>=` val.
*   `std::upper_bound(b, e, val)`: First element `>` val.
*   `std::binary_search(b, e, val)`: True/False existence.

### 27.6 Numeric Operations (<numeric>)
*   `std::iota(b, e, start)`: Fill with 0, 1, 2...
*   `std::accumulate(b, e, init)`: Sum (fold left).
*   `std::reduce(b, e)`: Parallelizable sum (C++17).
*   `std::inner_product`: Dot product.

---

## <a name="chapter-46-capstoneprojecthighperformanceorderbook"></a>CHAPTER 46: CAPSTONE PROJECT - HIGH-PERFORMANCE ORDER BOOK

This capstone project integrates C++20/23 features into a realistic high-frequency trading (HFT) component. It demonstrates Modules, Concepts, Ranges, Coroutines, and modern error handling.

### Project Structure
```text
order_book/
├── src/
│   ├── types.cppm        (Module: Common types)
│   ├── order.cppm        (Module: Order definition)
│   ├── book.cppm         (Module: OrderBook logic)
│   └── main.cpp          (Entry point)
├── CMakeLists.txt
└── README.md
```

### 1. Types Module (types.cppm)
```cpp
export module types;

import <cstdint>;
import <compare>;

export namespace hft {
    using Price = int64_t;
    using Quantity = uint32_t;
    using OrderId = uint64_t;

    enum class Side : uint8_t { Buy, Sell };
}
```

### 2. Order Module (order.cppm)
```cpp
export module order;

import types;
import <format>;
import <string>;

export namespace hft {
    struct Order {
        OrderId id;
        Side side;
        Price price;
        Quantity quantity;

        // C++20 Spaceship for easy comparison
        auto operator<=>(const Order&) const = default;
        
        // C++23 Deducing This for generic accessors (example)
        template<typename Self>
        auto&& get_price(this Self&& self) {
            return std::forward<Self>(self).price;
        }
    };
}

// C++20 Formatter specialization
template<>
struct std::formatter<hft::Order> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    auto format(const hft::Order& o, format_context& ctx) const {
        return std::format_to(ctx.out(), "[ID:{}] {} @ {}", 
            o.id, (o.side == hft::Side::Buy ? "BUY" : "SELL"), o.price);
    }
};
```

### 3. Order Book Module (book.cppm)
```cpp
export module book;

import types;
import order;
import <vector>;
import <map>;
import <ranges>;
import <algorithm>;
import <expected>;
import <print>;
import <coroutine>;

export namespace hft {

    // C++20 Concept for Order Container
    template<typename T>
    concept OrderContainer = requires(T c) {
        c.push_back(std::declval<Order>());
        c.size();
    };

    class OrderBook {
    private:
        // Use std::flat_map (C++23) for cache locality if available, 
        // else std::map. Simulated here as vector for simplicity + ranges
        std::vector<Order> bids;
        std::vector<Order> asks;

    public:
        // C++23 std::expected for error handling
        std::expected<void, std::string> add_order(Order o) {
            if (o.quantity == 0) return std::unexpected("Invalid quantity");
            
            auto& side_vec = (o.side == Side::Buy) ? bids : asks;
            side_vec.push_back(o);
            
            // Keep sorted (simplified)
            std::ranges::sort(side_vec, {}, &Order::price);
            if (o.side == Side::Buy) std::ranges::reverse(side_vec);
            
            return {};
        }

        // C++20 Coroutine Generator to stream top orders
        // Note: Requires <generator> (C++23) or custom implementation
        // Here we simulate a simple generator pattern or use ranges
        auto top_levels(Side side, int depth) const {
            const auto& vec = (side == Side::Buy) ? bids : asks;
            return vec | std::views::take(depth);
        }

        void print_book() const {
            std::println("--- Order Book ---");
            std::println("ASKS:");
            for (const auto& o : asks | std::views::reverse) std::println("  {}", o);
            std::println("BIDS:");
            for (const auto& o : bids) std::println("  {}", o);
            std::println("------------------");
        }
    };
}
```

### 4. Main Application (main.cpp)
```cpp
import book;
import order;
import types;
import <print>;

int main() {
    hft::OrderBook book;

    book.add_order({1, hft::Side::Buy, 100, 10});
    book.add_order({2, hft::Side::Buy, 99, 5});
    book.add_order({3, hft::Side::Sell, 101, 20});
    book.add_order({4, hft::Side::Sell, 102, 15});

    book.print_book();
    
    // Demonstrate Error Handling
    if (auto res = book.add_order({5, hft::Side::Buy, 100, 0}); !res) {
        std::println(stderr, "Error adding order: {}", res.error());
    }

    return 0;
}
```

---

---


# APPENDICES

## Appendix A: C++ Keywords & Operators Reference

### Essential Keywords (Non-Exhaustive)
*   **alignas / alignof**: Memory alignment queries and specifications.
*   **asm**: Inline assembly block.
*   **auto**: Type deduction (C++11).
*   **const / volatile**: cv-qualifiers for type safety and hardware access.
*   **constexpr / consteval / constinit**: Compile-time constant specifications.
*   **decltype**: Inspect declared type of an entity.
*   **explicit**: Prevent implicit conversions in constructors.
*   **export**: Module interface export (C++20).
*   **friend**: Allow access to private members.
*   **inline**: Suggest inlining to compiler; allow definition in header.
*   **mutable**: Allow modification of member in const object.
*   **noexcept**: Specifier for functions that don't throw.
*   **nullptr**: Null pointer literal (C++11).
*   **operator**: Overload operators.
*   **requires**: Constraint clause for Concepts (C++20).
*   **static_assert**: Compile-time assertion.
*   **template**: Define generic classes/functions.
*   **thread_local**: Storage duration specifier.
*   **typeid**: Runtime type identification (RTTI).
*   **typename**: Declare a type parameter in templates.
*   **virtual**: Declare virtual function for polymorphism.

### Special Operators
*   `::` Scope resolution
*   `->*` Pointer to member selection
*   `<=>` Three-way comparison (Spaceship) (C++20)
*   `co_await`, `co_yield`, `co_return` Coroutine operators (C++20)

---

## Appendix B: Common Acronyms

*   **ABI**: Application Binary Interface.
*   **API**: Application Programming Interface.
*   **COW**: Copy On Write.
*   **CRTP**: Curiously Recurring Template Pattern.
*   **CTAD**: Class Template Argument Deduction.
*   **UB**: Undefined Behavior (Avoid at all costs!).
*   **IB**: Implementation-defined Behavior.
*   **IIFE**: Immediately Invoked Function Expression (often with lambdas).
*   **NrvO / RVO**: (Named) Return Value Optimization.
*   **ODR**: One Definition Rule.
*   **PIMPL**: Pointer to Implementation (Opaque Pointer).
*   **RAII**: Resource Acquisition Is Initialization.
*   **RTTI**: Run-Time Type Information.
*   **SFINAE**: Substitution Failure Is Not An Error.
*   **SOO / SSO**: Small Object/String Optimization.
*   **STL**: Standard Template Library.
*   **TMP**: Template Metaprogramming.
*   **TU**: Translation Unit.

---

## Appendix C: Recommended Tooling

### Compilers
*   **GCC (GNU Compiler Collection)**: Standard on Linux.
*   **Clang/LLVM**: Excellent error messages, widely used on macOS/Linux.
*   **MSVC (Microsoft Visual C++)**: Standard on Windows.

### Build Systems
*   **CMake**: The industry standard meta-build system.
*   **Meson**: Modern, fast, Python-based.
*   **Bazel**: Google's build system, good for monorepos.

### Package Managers
*   **Conan**: Decentralized package manager for C/C++.
*   **vcpkg**: Microsoft's C++ library manager.

### Static Analysis & Sanitizers
*   **AddressSanitizer (ASan)**: Detects memory errors (buffer overflows, use-after-free).
*   **UndefinedBehaviorSanitizer (UBSan)**: Detects undefined behavior.
*   **ThreadSanitizer (TSan)**: Detects data races.
*   **Clang-Tidy**: Linter and static analysis tool.
*   **Cppcheck**: Static analysis tool.

---

## Appendix D: Common C++ Traps & Pitfalls

### I. General & Syntax Traps
1.  **Most Vexing Parse**
    *   *Issue*: `MyClass obj();` declares a function returning `MyClass`, not a default-constructed object.
    *   *Fix*: Use brace initialization: `MyClass obj{};`.

2.  **The "dangling else" Problem**
    *   *Issue*: Nested `if-else` without braces can associate `else` with the wrong `if`.
    *   *Fix*: Always use braces `{}` for control structures.

3.  **Integer Division**
    *   *Issue*: `1/2` results in `0` (integer), not `0.5`.
    *   *Fix*: Cast one operand to float/double: `1.0/2` or `static_cast<double>(1)/2`.

4.  **Loop Variable Type Mismatch**
    *   *Issue*: `for (unsigned i = v.size() - 1; i >= 0; --i)` causes an infinite loop because `unsigned` is never negative.
    *   *Fix*: Use `int` (and cast size) or standard iterators/ranges.

5.  **Shadowing Variables**
    *   *Issue*: Declaring a local variable with the same name as a member or outer variable hides the outer one.
    *   *Fix*: Enable compiler warnings (`-Wshadow`) and use `this->member` if necessary.

### II. Pointers, References & Memory
6.  **Object Slicing**
    *   *Issue*: Assigning a `Derived` object to a `Base` value slices off the derived part.
    *   *Fix*: Use pointers `Base*` or references `Base&` for polymorphism.

7.  **Dangling References**
    *   *Issue*: Returning a reference to a local stack variable.
    *   *Fix*: Return by value or use smart pointers/dynamic allocation.

8.  **Iterator Invalidation**
    *   *Issue*: Adding elements to a `std::vector` may reallocate memory, invalidating all pointers/iterators to elements.
    *   *Fix*: Don't cache iterators across mutating operations; use `reserve()` if possible.

9.  **`delete` vs `delete[]`**
    *   *Issue*: Mismatching `new` with `delete[]` or `new[]` with `delete` causes undefined behavior.
    *   *Fix*: Use `std::vector` or `std::unique_ptr` instead of manual management.

10. **Use-After-Move**
    *   *Issue*: Accessing an object after `std::move()` (except for reassignment/destruction).
    *   *Fix*: Treat moved-from objects as empty; do not read their state.

### III. Classes & OOP
11. **Virtual Destructor Missing**
    *   *Issue*: Deleting a derived class via a base pointer when the base destructor is not `virtual` leaks derived resources.
    *   *Fix*: Always mark base class destructors `virtual` (or `protected` if non-polymorphic).

12. **Calling Virtual Functions in Constructor/Destructor**
    *   *Issue*: Calls the *base* class version, not the derived one, because the derived part isn't initialized/is already destroyed.
    *   *Fix*: Use two-phase initialization or factory methods.

13. **Copy Constructor/Assignment Missing**
    *   *Issue*: Classes managing raw pointers will default to shallow copy (double free error).
    *   *Fix*: Follow the **Rule of Three/Five/Zero**.

14. **Initialization Order**
    *   *Issue*: Members are initialized in *declaration order*, not initializer list order.
    *   *Fix*: Keep initializer list order identical to member declaration order to avoid warnings.

### IV. Concurrency
15. **Data Races**
    *   *Issue*: Multiple threads accessing shared memory without synchronization (at least one writer).
    *   *Fix*: Use `std::mutex`, `std::atomic`, or `std::shared_mutex`.

16. **Deadlocks**
    *   *Issue*: Two threads waiting on each other's locks.
    *   *Fix*: Acquire locks in a consistent global order; use `std::scoped_lock` (C++17) to lock multiple mutexes safely.

17. **False Sharing**
    *   *Issue*: Independent atomic variables on the same cache line degrade performance due to cache coherency protocols.
    *   *Fix*: Use `alignas(hardware_destructive_interference_size)` to pad variables.

### V. Modern C++ & Macros
18. **`std::vector<bool>` Weirdness**
    *   *Issue*: It's a template specialization (bitfield), not a vector of bools. Returns a proxy object, not `bool&`.
    *   *Fix*: Use `std::deque<bool>` or `std::vector<char>` if you need real references.

19. **Auto Type Deduction**
    *   *Issue*: `auto` drops references and `const`.
    *   *Fix*: Use `auto&` or `const auto&` explicitly when needed.

20. **Macro Side Effects**
    *   *Issue*: `#define MAX(a,b) ((a) > (b) ? (a) : (b))` evaluates arguments twice. `MAX(x++, y)` increments `x` twice.
    *   *Fix*: Use `inline` functions or templates instead of macros.

21. **Static Initialization Order Fiasco**
    *   *Issue*: Global objects in different files have undefined initialization order.
    *   *Fix*: Use the "Construct On First Use" idiom (Meyers Singleton).

---

## Appendix H: Professional C++ Idioms

### 1. RAII (Resource Acquisition Is Initialization)
*   **Concept**: Bind resource lifecycle to object lifecycle. Constructor acquires, destructor releases.
*   **Use Case**: Memory, file handles, mutex locks, sockets.
*   **Example**: `std::lock_guard`, `std::unique_ptr`.

### 2. Pimpl (Pointer to Implementation)
*   **Concept**: Hide private members in a separate class/struct, accessed via a pointer.
*   **Benefit**: ABI stability, reduced compilation times (header dependency changes don't trigger rebuilds of clients).
*   **Pattern**:
    ```cpp
    class Widget {
        struct Impl;
        std::unique_ptr<Impl> pImpl;
    public:
        Widget();
        ~Widget(); // Defined in .cpp where Impl is visible
    };
    ```

### 3. Copy-and-Swap
*   **Concept**: Implement assignment operator in terms of copy constructor and swap.
*   **Benefit**: Strong Exception Safety guarantee; removes code duplication.
*   **Pattern**:
    ```cpp
    T& operator=(T other) { // Pass by value (copy)
        swap(*this, other);
        return *this;
    }
    ```

### 4. NVI (Non-Virtual Interface)
*   **Concept**: Public interface is non-virtual; virtual functions are private/protected.
*   **Benefit**: Separation of interface (pre/post-conditions) from implementation.
*   **Pattern**:
    ```cpp
    class Base {
    public:
        void doWork() {
            // Pre-condition logic
            doWorkImpl();
            // Post-condition logic
        }
    private:
        virtual void doWorkImpl() = 0;
    };
    ```

### 5. Erase-Remove Idiom
*   **Concept**: Standard way to remove elements from a `std::vector` (before C++20 `std::erase`).
*   **Pattern**: `v.erase(std::remove(v.begin(), v.end(), value), v.end());`

### 6. SFINAE (Substitution Failure Is Not An Error)
*   **Concept**: Remove functions from overload resolution set if types don't match constraints.
*   **Modern Replacement**: C++20 Concepts (`requires`).

### 7. CRTP (Curiously Recurring Template Pattern)
*   **Concept**: Class `Derived` inherits from `Base<Derived>`.
*   **Use Case**: Static polymorphism (compile-time), adding functionality (mixins) without vtable overhead.
*   **Example**: `std::enable_shared_from_this`.

---

## Appendix E: C++ Interview Cheat Sheet

### Core Concepts
1.  **Virtual Functions**: Enable runtime polymorphism via vtable/vptr. Destructors must be virtual in base classes.
2.  **Smart Pointers**:
    *   `unique_ptr`: Exclusive ownership, no overhead.
    *   `shared_ptr`: Shared ownership, ref-counted (atomic), control block overhead.
    *   `weak_ptr`: Non-owning reference to `shared_ptr` (breaks cycles).
3.  **Move Semantics**: Transfers resources (pointers) instead of deep copying. Enabled by rvalue references (`&&`) and `std::move`.
4.  **RAII**: Resource Acquisition Is Initialization. Constructor acquires, destructor releases. Core to C++ safety.
5.  **Cast Types**:
    *   `static_cast`: Compile-time safe conversions.
    *   `dynamic_cast`: Runtime checked downcasting (requires RTTI).
    *   `reinterpret_cast`: Bitwise reinterpretation (unsafe).
    *   `const_cast`: Remove/add constness.

### Modern C++ (C++11/14/17/20)
1.  **Lambdas**: Anonymous function objects. Capture `[=]`, `[&]`, or move-only `[x = std::move(y)]`.
2.  **Auto**: Type deduction. Always initialize.
3.  **Structured Bindings (C++17)**: `auto [x, y] = pair;`
4.  **Concepts (C++20)**: Constrain templates for better errors/readability.
5.  **Coroutines (C++20)**: Functions that can suspend/resume.

### System Design Questions
1.  **Vector vs List**: Vector (contiguous, cache-friendly) is almost always better than List (node-based, cache misses) unless aggressive splicing is needed.
2.  **Map vs Unordered Map**: Map (BST, O(log n), sorted) vs Unordered Map (Hash Table, O(1) avg, unsorted).
3.  **Handling 1M connections**: Use non-blocking I/O (epoll/kqueue) or `io_uring`, not one thread per connection.
4.  **Memory Layout**: Stack (local vars) vs Heap (dynamic) vs Data (globals) vs Text (code).

### Quick Coding
*   **Implement Singleton**: Use static local variable (Thread-safe in C++11+).
*   **Implement String Class**: Handle deep copy, move semantics, and destructor.
*   **Reverse Linked List**: Classic pointer manipulation.

---

## Appendix F: The C++ Standard Evolution Matrix

### 1. Versioned Changelog

#### **C++98 (ISO/IEC 14882:1998)** - *The Foundation*
**Released:** 1998
*   **Core:** Templates, Exceptions, Namespaces, `bool` type, `cast` operators (`static_cast`, etc.), `mutable`, `explicit`.
*   **STL:** Containers (`vector`, `list`, `map`, `set`, `deque`), Algorithms (`sort`, `find`, `transform`), Iterators, Strings (`std::string`), I/O Streams (`iostream`).
*   **Memory:** `std::auto_ptr` (Deprecated in C++11).

#### **C++03 (ISO/IEC 14882:2003)** - *The Bug Fix*
**Released:** 2003
*   **Focus:** Defect Report (DR) fixes for C++98 to ensure consistency across compilers.
*   **Features:** Value initialization `T()`, fixes to `std::vector` contiguous memory guarantee.

#### **C++11 (ISO/IEC 14882:2011)** - *The Modern Revolution*
**Released:** September 2011
*   **Language:** `auto`, `nullptr`, Range-based `for`, Lambda expressions, Rvalue references (`&&`) & Move semantics, Variadic templates, `constexpr` (limited), `decltype`, Uniform initialization `{}`, `static_assert`, `override`, `final`, `enum class`.
*   **Concurrency:** `std::thread`, `std::mutex`, `std::atomic`, `std::future`, `std::async`.
*   **Library:** Smart pointers (`unique_ptr`, `shared_ptr`, `weak_ptr`), `std::array`, `std::tuple`, `std::unordered_map/set`, `std::regex`, `std::chrono`.

#### **C++14 (ISO/IEC 14882:2014)** - *The Refinement*
**Released:** December 2014
*   **Language:** Generic lambdas (`auto` params), Relaxed `constexpr` (loops/variables allowed), Binary literals (`0b1010`), Digit separators (`1'000`), Variable templates, Return type deduction.
*   **Library:** `std::make_unique`, `std::shared_timed_mutex`, `std::integer_sequence`, `std::exchange`, `std::quoted`.

#### **C++17 (ISO/IEC 14882:2017)** - *The Major Update*
**Released:** December 2017
*   **Language:** Structured bindings `auto [x,y] = p;`, `if constexpr`, Fold expressions `(... + args)`, Class Template Argument Deduction (CTAD), Inline variables, `__has_include`.
*   **Library:** `std::filesystem`, `std::optional`, `std::variant`, `std::any`, `std::string_view`, Parallel Algorithms (`std::execution::par`), `std::invoke`, `std::byte`, `std::pmr` (Polymorphic Memory Resources).

#### **C++20 (ISO/IEC 14882:2020)** - *The Gigantic Leap*
**Released:** December 2020
*   **Language:** Concepts (Constraints), Modules (`import/export`), Coroutines (`co_await`), Three-way comparison (`<=>`), Designated initializers `{.x=1}`, `consteval` (Immediate functions), `constinit`, Range-based for with init.
*   **Library:** Ranges (`std::ranges`), `std::span`, `std::format`, `std::jthread`, `std::stop_token`, `std::barrier`, `std::latch`, `std::semaphore`, `std::bit_cast`, `std::source_location`, Calendars & Timezones.

#### **C++23 (ISO/IEC 14882:2023)** - *The Completion*
**Released:** October 2023
*   **Language:** Deducing `this` (Explicit object parameter), `if consteval`, Multidimensional subscript `m[1,2]`, Static `operator()`, `auto(x)` decay copy.
*   **Library:** `std::print`, `std::println`, `std::expected` (Error handling), `std::mdspan`, `std::flat_map`, `std::flat_set`, `std::generator` (Synchronous coroutines), `std::stacktrace`, `std::stdatomic.h`.

### 2. Feature Matrix

| Feature | C++98 | C++11 | C++14 | C++17 | C++20 | C++23 |
| :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **Memory** | `auto_ptr` | `unique_ptr` | `make_unique` | `pmr` | `shared_ptr` atomic | `out_ptr` |
| **Variables** | Type req. | `auto` | Var templates | Structured Bindings | `constinit` | - |
| **Loops** | `for(;;)` | Range-for | - | - | Range-for init | - |
| **Templates** | Basic | Variadic | Variable | Fold Expressions | Concepts | Deducing `this` |
| **Lambdas** | - | Basic | Generic | `constexpr` | Template | Recursive |
| **Concurrency** | - | `thread` | `shared_lock` | Parallel Algos | `jthread`, Latches | `stdatomic.h` |
| **String** | `string` | `to_string` | `quoted` | `string_view` | `format` | `print` |
| **Metaprog.** | Traits | `static_assert` | `integer_seq` | `if constexpr` | `consteval` | `if consteval` |
| **Modules** | - | - | - | - | **Modules** | `std` module |
| **Coroutines** | - | - | - | - | **Async** | `generator` |

### 3. Timeline & Release Accuracy

| Standard | ISO Publication | Codename | Compiler Flag (GCC/Clang) |
| :--- | :--- | :--- | :--- |
| **C++98** | 1998-09 | C++98 | `-std=c++98` |
| **C++03** | 2003-10 | C++03 | `-std=c++03` |
| **C++11** | 2011-09 | C++0x | `-std=c++11` |
| **C++14** | 2014-12 | C++1y | `-std=c++14` |
| **C++17** | 2017-12 | C++1z | `-std=c++17` |
| **C++20** | 2020-12 | C++2a | `-std=c++20` |
| **C++23** | 2023-10 | C++2b | `-std=c++23` |
| **C++26** | *Expected 2026* | C++2c | `-std=c++26` / `-std=c++2c` |

---

## Appendix G: C++ Standard Library Headers Reference

### Concepts & Utilities
*   `<concepts>` (C++20): Fundamental concepts library.
*   `<coroutine>` (C++20): Coroutine support library.
*   `<functional>`: Function objects, binder, and reference wrappers.
*   `<memory>`: Smart pointers and allocators.
*   `<tuple>`: Tuple library.
*   `<type_traits>`: Compile-time type information.
*   `<utility>`: Utility components (`std::pair`, `std::move`).

### Containers
*   `<array>` (C++11): Fixed-size array class.
*   `<deque>`: Double-ended queue.
*   `<list>`: Doubly-linked list.
*   `<map>`: Associative containers (Red-Black Tree).
*   `<queue>`: Queue adapter.
*   `<set>`: Associative containers (Red-Black Tree).
*   `<stack>`: Stack adapter.
*   `<unordered_map>` (C++11): Hash map.
*   `<vector>`: Dynamic array.
*   `<span >` (C++20): Non-owning view of contiguous memory.

### Algorithms & Iterators
*   `<algorithm>`: Algorithms that operate on ranges.
*   `<execution>` (C++17): Parallel algorithms.
*   `<iterator>`: Iterator primitives.
*   `<numeric>`: Numeric operations (`accumulate`, `reduce`).
*   `<ranges>` (C++20): Range primitives and views.

### Concurrency
*   `<atomic>` (C++11): Atomic operations.
*   `<barrier>` (C++20): Barriers.
*   `<condition_variable>` (C++11): Condition variables.
*   `<future>` (C++11): Futures and promises.
*   `<latch>` (C++20): Latches.
*   `<mutex>` (C++11): Mutual exclusion primitives.
*   `<semaphore>` (C++20): Semaphores.
*   `<shared_mutex>` (C++14): Shared mutexes.
*   `<thread>` (C++11): Thread class.

### Input/Output
*   `<filesystem>` (C++17): File system operations.
*   `<format>` (C++20): Formatting library.
*   `<fstream>`: File stream classes.
*   `<iostream>`: Standard I/O stream objects.
*   `<print>` (C++23): Print functions.
*   `<sstream>`: String stream classes.

### Numerics & Math
*   `<bit>` (C++20): Bit manipulation.
*   `<complex>`: Complex number arithmetic.
*   `<random>` (C++11): Random number generation.
*   `<ratio>` (C++11): Compile-time rational arithmetic.
*   `<valarray>`: Class for representing and manipulating arrays of values.
*   `<numbers>` (C++20): Mathematical constants.





