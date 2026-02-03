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

> "To understand the future of C++, one must first master its past. The foundations laid in 1998 still govern the machine code generated today."

## 1.1 Introduction to C++

### A Brief History of Power
C++ was created by Bjarne Stroustrup at Bell Labs in 1979 as "C with Classes". It was designed to add object-oriented features to C without sacrificing performance.

*   **C++98**: The first ISO standard. Established the core language, OOP, and the STL (Standard Template Library).
*   **C++03**: A bug-fix release.
*   **C++11**: "Modern C++". Move semantics, `auto`, lambdas, smart pointers. The biggest shift in the language's history.
*   **C++14/17**: Refinements. `constexpr`, structured bindings, parallel algorithms.
*   **C++20**: The "Big Four": Concepts, Ranges, Coroutines, Modules.
*   **C++23**: `std::print`, `import std`, explicit object parameters ("Deducing this").
*   **C++26**: (Upcoming) Reflection, Contracts, Senders/Receivers.

### Why C++?
1.  **Zero-Overhead Abstraction**: You don't pay for what you don't use.
2.  **Hardware Control**: Direct memory access, pointers, bit manipulation.
3.  **Performance**: Used in HFT (High-Frequency Trading), Game Engines, Operating Systems.

---

## 1.2 Your First C++ Program

The classic "Hello, World" reveals the compilation structure.

```cpp
// hello.cpp
#include <iostream>  // Preprocessor directive

// Entry point of the program
int main() {
    // std::cout: Standard Character Output
    // <<: Insertion operator
    // std::endl: Inserts newline and FLUSHES the buffer
    std::cout << "Hello, World!" << std::endl;
    
    return 0; // Return success status to OS
}
```

### Deep Dive: `std::endl` vs `\n`
*   `\n`: Just a newline character. Fast.
*   `std::endl`: Newline character + `std::flush`. Slower.
*   **Best Practice**: Prefer `\n` for performance unless you *need* to flush (e.g., logging crash info).

---

## 1.3 Fundamental Types & Variables

C++ is a strongly-typed language. Every variable has a type, and that type determines the memory layout.

### Integer Types (Data Model: LP64 on modern Unix)
| Type | Min Size | Typical Size | Description |
|------|----------|--------------|-------------|
| `char` | 1 byte | 1 byte | Character/Byte |
| `short` | 2 bytes | 2 bytes | Short integer |
| `int` | 2 bytes | 4 bytes | Standard integer |
| `long` | 4 bytes | 8 bytes (64-bit OS) | Long integer |
| `long long` | 8 bytes | 8 bytes | Extended long integer (C++11) |

### Floating Point Types (IEEE 754)
| Type | Precision | Typical Size |
|------|-----------|--------------|
| `float` | ~7 digits | 4 bytes |
| `double` | ~15 digits | 8 bytes |
| `long double` | >15 digits | 16 bytes (x86 extended) |

### The `void` Type
*   **Incomplete type**: Cannot hold a value.
*   **Uses**: Function return type (returns nothing), `void*` (generic pointer).

### Variable Initialization (The Evolution)
```cpp
int a = 5;          // C-style assignment (Copy initialization)
int b(5);           // Constructor initialization (Direct initialization)
int c{5};           // Uniform initialization (C++11) - Prevents narrowing!
int d = {5};        // Copy-list initialization
```

### Scope & Lifetime
1.  **Local Scope (Automatic Storage)**: Lives until the block `}` ends. Stored on the **Stack**.
2.  **Global/Static Scope (Static Storage)**: Lives for the entire program duration. Initialized before `main`. Stored in **Data/BSS**.
3.  **Dynamic Scope**: Controlled manually (`new`/`delete`). Stored on the **Heap**.

---

## 1.4 Operators & Expressions

### Arithmetic & Compound Assignment
Standard: `+`, `-`, `*`, `/`, `%`.
Compound: `+=`, `-=`, `*=`, `/=`, `%=`.

### Comparison & Logical
*   `==`, `!=`, `<`, `>`, `<=`, `>=`.
*   `&&` (AND), `||` (OR), `!` (NOT).
*   **Short-Circuit Evaluation**: In `A && B`, if `A` is false, `B` is never evaluated.

### The Ternary Operator `? :`
The only ternary operator in C++.
```cpp
int max = (a > b) ? a : b;
```
*Note*: `? :` yields an lvalue in C++ (you can assign to it!), unlike C.
```cpp
(a > b ? a : b) = 10; // Valid C++, sets the larger variable to 10
```

### Bitwise Operators (The Systems Programmer's Weapon)
Essential for flags, masks, and optimization.

| Operator | Name | Description | Example (A=5 `0101`, B=3 `0011`) |
|----------|------|-------------|-----------------------------------|
| `&` | AND | Both bits must be 1 | `A & B` = 1 (`0001`) |
| `|` | OR | At least one bit 1 | `A | B` = 7 (`0111`) |
| `^` | XOR | Different bits = 1 | `A ^ B` = 6 (`0110`) |
| `~` | NOT | Inverts all bits | `~A` = -6 (Two's comp) |
| `<<` | Left Shift | Multiply by 2^N | `A << 1` = 10 (`1010`) |
| `>>` | Right Shift | Divide by 2^N | `A >> 1` = 2 (`0010`) |

**Common Bitwise Tricks:**
```cpp
// Check if Odd: (x & 1)
// Set Nth bit: x |= (1 << N)
// Clear Nth bit: x &= ~(1 << N)
// Toggle Nth bit: x ^= (1 << N)
// Check if Power of 2: (x > 0) && !(x & (x - 1))
```

---

## 1.5 Control Flow

### If, Else, Switch
The standard decision structures.
**Switch Fallthrough**: C++ switch cases fall through automatically unless `break` is used.
```cpp
switch (val) {
    case 1:
        doSomething();
        // FALLTHROUGH (intentional) - often marked with comment or [[fallthrough]] in C++17
    case 2:
        doMore();
        break;
}
```

### Loops: While, Do-While, For
```cpp
// Canonical For Loop
for (int i = 0; i < 10; ++i) { // Prefer pre-increment ++i for iterators
    if (i == 5) continue; // Skip to next iteration
    if (i == 8) break;    // Exit loop
    std::cout << i;
}
```

### The `goto` Statement
Widely reviled, but useful for:
1.  Breaking out of nested loops.
2.  Error cleanup patterns (common in C/Linux Kernel, less common in C++ due to RAII).
```cpp
    for(;;) {
        for(;;) {
            if (error) goto cleanup;
        }
    }
cleanup:
    release_resources();
```

---

## 1.6 Functions

### Declaration vs Definition
*   **Declaration (Prototype)**: Tells compiler function exists (`int add(int, int);`).
*   **Definition**: The actual code.

### Calling Conventions
1.  **Pass by Value**: Copy is made. Expensive for large objects. `void f(int x);`
2.  **Pass by Pointer**: Address is passed. Efficient. `void f(int* x);`
3.  **Pass by Reference**: Alias to original. Efficient + Cleaner syntax. `void f(int& x);`
4.  **Pass by Const Reference**: Efficient read-only access. `void f(const string& s);`

### Default Arguments
Can act as simple overloading.
```cpp
void log(const char* msg, int level = 1); // level defaults to 1
```

### Function Overloading
Same name, different parameter signature.
```cpp
int add(int a, int b);
double add(double a, double b);
```
*Note*: Return type is NOT part of the signature. You cannot overload based only on return type.

### Inline Functions
`inline` hint suggests the compiler replace the call with the function body to save overhead.
```cpp
inline int square(int x) { return x * x; }
```
*Modern Reality*: Compilers ignore this keyword for optimization (they decide freely), but `inline` is crucial for **ODR (One Definition Rule)** to allow function definitions in header files.

### Recursion
A function calling itself. Must have a base case to avoid **Stack Overflow**.
```cpp
int factorial(int n) {
    return (n <= 1) ? 1 : n * factorial(n - 1);
}
```

---

## 1.7 Pointers & References

### Pointers: The Sword of C++
A pointer stores a memory address.

```cpp
int x = 10;
int* p = &x;     // & = Address-of operator
int val = *p;    // * = Dereference operator (get value at address)
```

**Pointer Arithmetic**:
`p + 1` increases the address by `sizeof(T)`.
*   If `p` is `int*` (4 bytes) at `0x1000`, `p+1` is `0x1004`.

**Generic Pointer (`void*`)**:
Can hold any address, but cannot be dereferenced directly. Must cast first.

**Null Pointers**:
*   C++98: `NULL` (usually defined as `0`).
*   C++11: `nullptr` (type-safe). **Always use `nullptr` in modern code.**

### Pointers to Pointers
```cpp
int** pp = &p; // Stores address of a pointer
```

### References: The Shield
A reference is an **alias** for an existing variable.
```cpp
int& ref = x; // ref IS x. 
```
*   Must be initialized upon declaration.
*   Cannot be null.
*   Cannot be reseated (changed to refer to another variable).
*   Syntactic sugar for a const pointer (usually implemented that way).

---

## 1.8 Arrays

### Static Arrays (Stack)
Fixed size, determined at compile time.
```cpp
int arr[5] = {1, 2, 3}; // Remaining elements 0-initialized
```
**Array Decay**: An array name decays into a pointer to its first element when passed to a function. `void f(int arr[])` is exactly `void f(int* arr)`.

### Multidimensional Arrays
```cpp
int matrix[3][3]; // Row-major order (contiguous in memory)
```

### Dynamic Arrays (Heap)
Allocated at runtime.
```cpp
int size = 10;
int* heapArr = new int[size];
// ... use ...
delete[] heapArr; // MUST use delete[], not delete!
```

---

## 1.9 User-Defined Types

### Structures (`struct`)
Groups variables under one name. In C++, `struct` and `class` are identical except default access (struct=public, class=private).
```cpp
struct Point {
    int x;
    int y;
};
```

### Enumerations (`enum`)
Defines a set of named integer constants.
```cpp
enum Color { RED, GREEN, BLUE }; // RED=0, GREEN=1...
Color c = RED;
```

### Unions (`union`)
All members share the **same memory location**. Size is the size of the largest member.
```cpp
union Data {
    int i;
    float f;
    char c;
}; // Useful for low-level memory reinterpretation
```

---

## 1.10 The Preprocessor

Runs before the compiler. Handles text manipulation.

1.  **#include**: Pastes file content.
2.  **#define**: Text replacement macros.
    ```cpp
    #define MAX(a,b) ((a) > (b) ? (a) : (b))
    ```
    *Danger*: Macros have no scope or type safety.
3.  **Conditional Compilation**:
    ```cpp
    #ifdef DEBUG
        log("Debug mode");
    #endif
    ```
4.  **#pragma**: Compiler-specific directives (e.g., `#pragma once`).

---

## 1.11 C-Style Casting

Before `static_cast`, `reinterpret_cast`, etc., there was only the C-cast.
```cpp
double d = 3.14;
int i = (int)d; // Truncates
```
It is powerful but dangerous. It tries every possible conversion (const_cast, static_cast, reinterpret_cast) until one works.

---
## <a name="chapter-2-theccompilationexecutionmodel"></a>CHAPTER 2: THE C++ COMPILATION & EXECUTION MODEL

To truly understand C++, you must understand how your code transforms from text to a running process. This section demystifies the "black box" of the compiler and the runtime environment.

### 2.1 The Build Pipeline: From Source to Binary

The "compilation" process actually consists of four distinct stages:

1.  **Preprocessing**: Text substitution.
    *   Removes comments.
    *   Expands macros (`#define`).
    *   Includes headers (`#include`) recursively.
    *   Handles conditionals (`#ifdef`).
    *   *Tool*: `cpp` or `g++ -E`.
    *   *Output*: A single "Translation Unit" (pure C++ source code).

2.  **Compilation**: Syntax to Assembly.
    *   **Lexical Analysis**: Tokenizes source (e.g., `int`, `main`, `{`).
    *   **Parsing**: Builds Abstract Syntax Tree (AST).
    *   **Semantic Analysis**: Type checking, overload resolution, template instantiation.
    *   **Optimization**: Dead code elimination, loop unrolling, inlining (O1, O2, O3).
    *   **Code Generation**: Generates assembly code for the target architecture (x86_64, ARM64).
    *   *Tool*: `cc1plus` or `g++ -S`.
    *   *Output*: Assembly file (`.s` or `.asm`).

3.  **Assembly**: Assembly to Machine Code.
    *   Translates mnemonics (`MOV`, `ADD`) to opcodes (`0x89`, `0x01`).
    *   *Tool*: `as`.
    *   *Output*: Object file (`.o` or `.obj`). Contains machine code but with *unresolved symbols*.

4.  **Linking**: Combining Object Files.
    *   Combines multiple `.o` files and static libraries (`.a`/`.lib`).
    *   Resolves symbols: Matches function calls in one TU to definitions in another.
    *   Relocates addresses: Adjusts internal pointers.
    *   *Tool*: `ld`.
    *   *Output*: Executable (`.exe` or ELF/Mach-O) or Shared Library (`.so`/`.dll`).

### 2.2 Translation Units (TU) & Linkage

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

### 2.3 The One Definition Rule (ODR)

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

### 2.4 Name Mangling & ABI

#### Name Mangling
In C, function names are used directly (`_add`). In C++, functions can be overloaded, so the compiler must generate unique names including parameter types.
*   `void foo(int)` -> `_Z3fooi` (Itanium ABI example)
*   `void foo(double)` -> `_Z3food`

**Implication**: You cannot link C code against C++ code directly without `extern "C"`, which disables mangling.

#### Application Binary Interface (ABI)
Defines how binary code interacts (calling convention, class layout, vtable layout).
*   **Stability**: C++ does NOT guarantee a stable ABI between compiler versions (e.g., MSVC 2017 vs 2019 might differ, though they try to be compatible).
*   **Standard Layout**: `extern "C"` functions use the C ABI, which is stable.

### 2.5 Process Memory Layout

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

### 2.6 Program Startup: Before main()

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

### 2.7 Deep Dive into Data Representation

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

## <a name="chapter-3-objectorientedprogrammingfundamentals"></a>CHAPTER 3: OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS

Object-Oriented Programming (OOP) in C++ is not just about syntax; it's about modeling complex systems through **abstraction**, **encapsulation**, **inheritance**, and **polymorphism**.

### 3.1 Classes & Objects

A **Class** is a blueprint defining data (attributes) and behavior (methods). An **Object** is an instance of a class.

```cpp
class Car {
    // By default, members are private in class (public in struct)
    int speed; 

public:
    // Member function
    void accelerate() {
        this->speed += 10; // 'this' is a pointer to the current object
    }
};
```

#### The `this` Pointer
*   `this` is a hidden pointer passed to all non-static member functions.
*   Type: `Car* const` (pointer to mutable Car) or `const Car* const` (in const methods).

### 3.2 Encapsulation & Access Control

Control who sees your data. This is the first line of defense against bugs.

1.  **public**: Accessible by everyone.
2.  **private**: Accessible only by the class itself (and friends).
3.  **protected**: Accessible by the class and derived classes.

#### Friend Declarations
A mechanism to bypass encapsulation. Use sparingly!
```cpp
class Box {
    int width;
public:
    friend void printWidth(Box box); // Friend function
    friend class BoxFactory;         // Friend class
};
```

### 3.3 The Object Lifecycle

#### Constructors
1.  **Default Constructor**: `ClassName()`. Generated if no other constructor is defined.
2.  **Parameterized Constructor**: `ClassName(int x)`.
3.  **Copy Constructor**: `ClassName(const ClassName& other)`. Creates a new object from an existing one.
4.  **Move Constructor** (C++11): `ClassName(ClassName&& other)`. Transfers resources.

#### Member Initializer Lists
**ALWAYS** use them.
```cpp
// GOOD
Car(int s) : speed(s) {} 

// BAD (Double initialization: default construct + assign)
Car(int s) { speed = s; } 
```

#### Destructors
Cleans up resources.
*   Name: `~ClassName()`.
*   **Virtual Destructors**: Essential if you delete derived objects via base pointers.

#### Advanced Constructors (C++11)
*   **Delegating Constructors**: One constructor calls another.
    ```cpp
    Point() : Point(0, 0) {} // Calls Point(int, int)
    ```
*   **Inheriting Constructors**: `using Base::Base;`.

### 3.4 The Rule of Three, Five, and Zero

The golden rules of C++ resource management.

1.  **Rule of Three (C++98)**: If you implement a Destructor, Copy Constructor, or Copy Assignment Operator, you likely need all three.
    *   *Why?* You are probably managing a raw pointer/handle.

2.  **Rule of Five (C++11)**: Add Move Constructor and Move Assignment Operator to the Rule of Three.
    *   *Why?* To support efficient transfers of ownership.

3.  **Rule of Zero (Modern Best Practice)**: Do **NOT** declare any of these 5 functions manually.
    *   *How?* Use resource handles like `std::string`, `std::vector`, `std::unique_ptr` that handle their own memory.

```cpp
// Rule of Five Example
class Buffer {
    int* data;
    size_t size;

public:
    // 1. Destructor
    ~Buffer() { delete[] data; }

    // 2. Copy Constructor (Deep Copy)
    Buffer(const Buffer& other) : size(other.size), data(new int[other.size]) {
        std::copy(other.data, other.data + size, data);
    }

    // 3. Copy Assignment (Copy-and-Swap Idiom)
    Buffer& operator=(Buffer other) {
        swap(*this, other);
        return *this;
    }

    // 4. Move Constructor (Transfer ownership)
    Buffer(Buffer&& other) noexcept : data(nullptr), size(0) {
        swap(*this, other);
    }

    // 5. Move Assignment (Handled by Copy Assignment via by-value param, or explicit)
    //    Here, operator=(Buffer other) handles both if defined correctly!
    
    friend void swap(Buffer& a, Buffer& b) noexcept {
        std::swap(a.data, b.data);
        std::swap(a.size, b.size);
    }
};
```

### 3.5 Inheritance

#### Types of Inheritance
1.  **Single**: `class Dog : public Animal`
2.  **Multiple**: `class Duck : public Bird, public Fish` (Careful!)
3.  **Multilevel**: `class A -> class B -> class C`
4.  **Hierarchical**: `class Shape -> (Circle, Square)`

#### The Diamond Problem (Virtual Inheritance)
If `B` and `C` inherit from `A`, and `D` inherits from `B` and `C`, `D` has two copies of `A`.
*   **Fix**: `class B : virtual public A`.

### 3.6 Polymorphism

#### Static Polymorphism (Compile-Time)
*   **Function Overloading**: Same name, different args.
*   **Templates**: `template <typename T>`.
*   **CRTP (Curiously Recurring Template Pattern)**: High-performance static polymorphism.

#### Dynamic Polymorphism (Run-Time)
*   **Virtual Functions**: Functions resolved at runtime via **vtable**.
*   **Override**: `void foo() override;` (C++11) - Ensures you are actually overriding.
*   **Final**: `void foo() final;` (C++11) - Prevents further overriding.
*   **Pure Virtual Function**: `virtual void foo() = 0;`. Makes the class **Abstract** (Interface).

#### How Virtual Functions Work (Under the Hood)
1.  Compiler adds a hidden pointer (`vptr`) to the class.
2.  `vptr` points to a static table (`vtable`) of function pointers for that class.
3.  Calling `obj->method()` becomes `obj->vptr[index]()`.
4.  **Cost**: One pointer overhead per object + one indirect lookup per call + instruction cache miss potential.

### 3.7 Casting & RTTI

Run-Time Type Information (RTTI) allows checking types at runtime.

1.  **dynamic_cast<T*>**: Safely converts pointers down the hierarchy. Returns `nullptr` on failure. Requires polymorphic class (at least one virtual function).
    ```cpp
    Base* b = new Derived();
    if (Derived* d = dynamic_cast<Derived*>(b)) {
        // Success
    }
    ```
2.  **typeid**: Returns `std::type_info`.
    ```cpp
    if (typeid(*b) == typeid(Derived)) { ... }
    ```

### 3.8 SOLID Principles in C++

1.  **Single Responsibility**: A class should have one reason to change.
2.  **Open/Closed**: Open for extension (inheritance), closed for modification.
3.  **Liskov Substitution**: Derived classes must be substitutable for base classes without breaking behavior.
4.  **Interface Segregation**: Many specific interfaces are better than one general-purpose interface.
5.  **Dependency Inversion**: Depend on abstractions (Abstract Classes), not concretions.

---
## <a name="chapter-4-deepobjectmodelvirtualization"></a>CHAPTER 4: DEEP OBJECT MODEL & VIRTUALIZATION

Understanding the "C++ Object Model" distinguishes a user from a master. This section explains what the compiler generates for your classes, how memory is laid out, and the hidden costs of abstractions.

### 4.1 Memory Layout, Alignment & Padding

In C++, objects are just blocks of memory. The compiler arranges members to satisfy **alignment requirements**.

#### Alignment Rules
*   Every type has an alignment requirement (usually equal to its size, up to the processor's word size).
*   `char`: 1 byte.
*   `int`: 4 bytes.
*   `double`: 8 bytes.
*   A member must start at an address divisible by its alignment.

#### Padding
The compiler inserts "padding bytes" to ensure alignment.

```cpp
struct PoorlyOrdered {
    char a;     // 1 byte
                // 3 bytes PADDING (to align 'b' to 4)
    int b;      // 4 bytes
    char c;     // 1 byte
                // 3 bytes PADDING (to align structure size to 4)
};
// sizeof = 1 + 3 + 4 + 1 + 3 = 12 bytes

struct WellOrdered {
    int b;      // 4 bytes
    char a;     // 1 byte
    char c;     // 1 byte
                // 2 bytes PADDING (to align structure size to 4)
};
// sizeof = 4 + 1 + 1 + 2 = 8 bytes
```

**God-Tier Tip**: Sort members from Largest to Smallest to minimize padding and cache wastage.

### 4.2 The Cost of Polymorphism (vptr & vtable)

When you use `virtual` functions, the compiler implements a dynamic dispatch mechanism.

#### 1. vptr (Virtual Pointer)
*   A hidden pointer added to the *layout* of every object instance of a polymorphic class.
*   Usually sits at the very beginning (offset 0) of the object.
*   Size: 8 bytes (on 64-bit systems).

#### 2. vtable (Virtual Table)
*   A **static** array of function pointers created *per class* (not per object).
*   Stores the address of the most-derived function for each virtual method.

#### How a Virtual Call Works
`obj->draw()` translates roughly to:
```cpp
// 1. Get the vptr (at start of object)
void** vptr = *(void***)obj; 

// 2. Look up the function address in the vtable (index known at compile time)
void (*func)() = (void (*)(void))vptr[0];

// 3. Call the function
func();
```

**Performance Implications**:
*   **Space**: Extra pointer per object + one table per class.
*   **Time**: One extra memory load (dereference) + Indirect branch (can cause pipeline stall/misprediction).
*   **Inlining**: Virtual functions generally *cannot* be inlined (unless the compiler can prove the type at compile time).
## <a name="chapter-5-c9803standardlibrary"></a>CHAPTER 5: STANDARD LIBRARY (STL) MASTERY

The Standard Template Library (STL) is the heart of modern C++. It provides reusable, high-performance components. Mastery of the STL is non-negotiable.

### 5.1 The STL Architecture
The STL separates data (Containers) from operations (Algorithms) via an interface (Iterators).
*   **Containers**: Manage memory and hold objects.
*   **Algorithms**: Process elements.
*   **Iterators**: Glue that binds them.

---

### 5.2 Sequence Containers

Ordered collections where position depends on insertion time.

#### 1. `std::vector` (Dynamic Array)
The default container. **Always use vector unless you have a specific reason not to.**
*   **Layout**: Contiguous memory.
*   **Access**: O(1).
*   **Insert/Delete (Back)**: O(1) amortized.
*   **Insert/Delete (Middle/Front)**: O(N) - shifts elements.

**God-Tier Knowledge: Capacity vs. Size**
*   `size()`: Number of elements.
*   `capacity()`: Allocated memory.
*   `reserve(n)`: Pre-allocates memory to avoid reallocations. **Critical for performance.**
*   **Growth Factor**: Usually 2x or 1.5x.

**Iterator Invalidation**:
*   Reallocation (growing beyond capacity) invalidates **ALL** iterators/pointers.
*   Insertion/Erasure invalidates iterators **after** the point of modification.

#### 2. `std::deque` (Double-Ended Queue)
*   **Layout**: A sequence of fixed-size memory blocks (chunked array).
*   **Access**: O(1) (pointer indirection overhead).
*   **Insert/Delete (Front/Back)**: O(1).
*   **Use Case**: When you need to push/pop from both ends.

**Iterator Invalidation**:
*   Insertion at ends: Invalidates iterators, but **NOT** references/pointers.
*   Insertion in middle: Invalidates everything.

#### 3. `std::list` (Doubly Linked List)
*   **Layout**: Nodes scattered in memory.
*   **Access**: O(N).
*   **Insert/Delete**: O(1) anywhere (if you have the iterator).
*   **Use Case**: Frequent splicing/sorting in place. **Cache unfriendly**. Avoid unless necessary.

---

### 5.3 Associative Containers

Sorted collections based on keys. Implemented as **Red-Black Trees** (Self-balancing BST).
*   **Search/Insert/Delete**: O(log N).
*   **Ordering**: Keys are always sorted.

#### 1. `std::set` / `std::multiset`
*   Set of unique keys (multiset allows duplicates).
*   Keys are `const` (cannot modify key in-place, must erase and re-insert).

#### 2. `std::map` / `std::multimap`
*   Key-Value pairs (`std::pair<const Key, Value>`).
*   `operator[]`: inserts default value if key not found! Use `find()` for checking existence.

**Iterator Invalidation**:
*   Insertion/Deletion ONLY invalidates iterators to the affected element. All others remain valid. (Tree structure stability).

---

### 5.4 Container Adapters
Wrappers that restrict interfaces.

1.  **`std::stack`**: LIFO (Last-In, First-Out). Adapts vector/deque/list.
2.  **`std::queue`**: FIFO (First-In, First-Out). Adapts deque/list.
3.  **`std::priority_queue`**: Heap data structure. O(log N) push/pop. O(1) top. Max-heap by default.

---

### 5.5 Unordered Containers (C++11)
Hash Tables.
*   `unordered_set`, `unordered_map`, etc.
*   **Performance**: O(1) average, O(N) worst case (collisions).
*   **Requirement**: Key must be hashable.

---

### 5.6 Iterators

Iterators are pointer-like objects.

#### Hierarchy (Capabilities)
1.  **Input Iterator**: Read-only, single pass (e.g., `istream_iterator`).
2.  **Output Iterator**: Write-only, single pass (e.g., `ostream_iterator`).
3.  **Forward Iterator**: Read/Write, multi-pass (e.g., `forward_list`).
4.  **Bidirectional Iterator**: Can move back `--it` (e.g., `list`, `map`, `set`).
5.  **Random Access Iterator**: Jump `it + n`, compare `<` (e.g., `vector`, `deque`, `array`, raw pointers).

#### Operations
*   `*it`: Dereference.
*   `++it`: Next element.
*   `it->member`: Member access.

---

### 5.7 Algorithms

Defined in `<algorithm>`. They operate on ranges `[begin, end)`.

#### Non-Modifying
*   `find(begin, end, val)`: O(N).
*   `count(begin, end, val)`: O(N).
*   `binary_search(begin, end, val)`: O(log N). **Requires sorted range.**

#### Modifying
*   `copy(src_begin, src_end, dest)`: Copies range.
*   `transform(begin, end, out, func)`: Applies function to elements.
*   `remove_if(begin, end, pred)`: **Erase-Remove Idiom**.
    ```cpp
    // Removes elements but doesn't resize container!
    auto it = std::remove(v.begin(), v.end(), 99);
    // Must call erase to actually shrink
    v.erase(it, v.end());
    ```

#### Sorting
*   `sort(begin, end)`: O(N log N). Uses Introsort (QuickSort + HeapSort).
*   `stable_sort(begin, end)`: Preserves order of equal elements.

---

### 5.8 Strings (`std::string`)

A specialization of `basic_string<char>`. Effectively a `vector<char>` optimized for text.

*   **SSO (Small String Optimization)**: Short strings (e.g., < 15/23 chars) are stored directly in the object, avoiding heap allocation.
*   **c_str()**: Returns null-terminated C-string `const char*`.
*   **string_view** (C++17): Lightweight, non-owning reference to a string. **Prefer this for function parameters.**

---

### 5.9 I/O Streams

*   `cin`, `cout`, `cerr` (unbuffered error), `clog` (buffered error).
*   `stringstream`: In-memory formatting.
*   `fstream`: File I/O.
    *   RAII: Files close automatically in destructor.

```cpp
std::ifstream file("data.txt");
if (file) {
    std::string line;
    while (std::getline(file, line)) {
        process(line);
    }
}
```

---
## <a name="chapter-6-stlinternalsdeepdive"></a>CHAPTER 6: STL INTERNALS DEEP DIVE

To master the STL, you must understand what happens under the hood. This chapter explores the memory layout, complexity guarantees, and hidden mechanisms of the standard library.

### 6.1 The Truth About std::vector
`std::vector` is a dynamic array. It guarantees contiguous memory compatible with C arrays.

*   **Layout**: Three pointers (usually):
    *   `start`: Points to first element.
    *   `finish`: Points to one-past-the-last active element (size).
    *   `end_of_storage`: Points to end of allocated buffer (capacity).
    *   *Size*: `finish - start`. *Capacity*: `end_of_storage - start`.

*   **Growth Strategy**: Geometric growth.
    *   When `size() == capacity()`, a new buffer is allocated (usually **2x** or **1.5x** larger).
    *   **Elements are MOVED** (if `noexcept` move constructor exists) or copied to the new buffer.
    *   Old buffer is destroyed.
    *   *Cost*: Amortized O(1) push_back, but worst-case O(N) during reallocation.

### 6.2 The std::deque Implementation
`std::deque` (Double-Ended Queue) is **NOT** a contiguous array.

*   **Layout**: A "Map" (dynamic array) of pointers to fixed-size "Chunks" (memory blocks).
    *   Iterators are complex smart pointers that track {current_chunk, current_index}.
*   **Performance**:
    *   O(1) random access (requires double dereference: Map -> Chunk -> Element).
    *   O(1) push/pop at BOTH ends (no full reallocation needed, just allocate a new chunk).
*   **Cache Locality**: Worse than vector, better than list.

### 6.3 Why std::list is (Almost) Always Wrong
`std::list` is a Doubly Linked List.

*   **Layout**: Nodes allocated individually on the heap.
    *   `struct Node { T val; Node* prev; Node* next; }`
*   **The Cache Problem**: Nodes are scattered in memory (heap fragmentation). Traversing a list causes constant **Cache Misses**.
*   **Benchmark**: Iterating a `vector` is orders of magnitude faster than a `list`, even for large types, due to CPU prefetching.
*   **Use Case**: Only when you need **Reference Stability** (insertions never invalidate pointers/references to other elements) or frequent slicing/merging.

### 6.4 Associative Containers (Map/Set)
`std::map`, `std::set`, `std::multimap`, `std::multiset`.

*   **Implementation**: Balanced Binary Search Tree (usually **Red-Black Tree**).
*   **Node Layout**: `struct Node { T val; Node* left; Node* right; Node* parent; Color color; }`
*   **Complexity**: O(log N) for insert, lookup, delete.
*   **Overhead**: 3 pointers + 1 enum per element (significant memory overhead per node).

### 6.5 Unordered Containers (Hash Maps)
`std::unordered_map`, `std::unordered_set`.

*   **Implementation**: Array of "Buckets" (usually Linked Lists).
    *   Hash function maps `Key` -> `Bucket Index`.
    *   Collisions handled by **Chaining** (linked list in bucket).
*   **Rehashing**: When `load_factor() > max_load_factor()`, the bucket array grows, and **ALL** elements are rehashed and moved.
*   **Cache**: Poor (linked list traversal in buckets).

### 6.6 Allocators
Every STL container takes an optional template parameter: `Allocator`.
`template <class T, class Allocator = std::allocator<T>> class vector;`

*   **Role**: Abstraction of memory allocation/deallocation.
*   **std::allocator**: Uses `new` and `delete`.
*   **Custom Allocators**: Used for:
    *   Shared memory (inter-process communication).
    *   Memory pools (fast allocation for node-based containers).
    *   Debugging/Tracking memory usage.

### 6.7 Exception Safety Guarantees
STL operations provide specific guarantees if an exception is thrown (e.g., during copying T).

1.  **Basic Guarantee**: Invariants are preserved, and no resources leak. The container might be empty or in a valid but unspecified state.
2.  **Strong Guarantee**: Transactional semantics. If an operation fails, the container remains **unchanged**. (e.g., `vector::push_back`).
3.  **Nothrow Guarantee**: The operation will never throw (e.g., `swap`, `move` constructors).

### 6.8 Iterator Invalidation Cheat Sheet

| Container | Operation | Invalidates |
| :--- | :--- | :--- |
| **Vector** | Capacity Change | **ALL** (Iterators, Pointers, References) |
| **Vector** | Insert/Erase | Current & After |
| **Deque** | Insert/Erase (ends) | Iterators only (Refs valid!) |
| **Deque** | Insert/Erase (middle) | **ALL** |
| **List** | Insert/Erase | Only deleted element |
| **Map/Set** | Insert/Erase | Only deleted element |
| **Unordered** | Rehash | **ALL** |
| **Unordered** | Insert (no rehash) | None |

---
## <a name="chapter-7-c11revolution"></a>CHAPTER 7: C++11 REVOLUTION

The C++11 standard was a massive upgrade. This is where modern C++ begins!

---

## C++11 Overview & History

**C++11** (also called C++0x) is the most significant C++ update since the language's creation:

### Timeline
- **1998**: C++98 released (first standard)
- **2003**: C++03 (minor fixes)
- **2011**: C++11 (revolutionary update - 13 years later!)
- **2014**: C++14 (maintenance release)
- **2017**: C++17 (major update)
- **2020**: C++20 (revolutionary)
- **2023**: C++23 (latest)

### Why C++11 Matters
C++11 introduced **70+ new features**, transforming C++ from error-prone to modern and safe.

### Key Themes
1. **Memory Safety** - Smart pointers
2. **Performance** - Move semantics
3. **Readability** - Auto, lambdas, range-based for
4. **Correctness** - nullptr, strongly-typed enums
5. **Flexibility** - Variadic templates
6. **Concurrency** - Threads, atomic operations
7. **Convenience** - Uniform initialization, tuple

---

# SECTION 1: AUTO & TYPE DEDUCTION

## 1.1 Auto Keyword

The `auto` keyword instructs the compiler to deduce the type from context.

### Basic Auto

```cpp
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// Type deduction - compiler figures out the type
auto x = 5;                      // int
auto y = 3.14;                   // double
auto z = "hello";                // const char*
auto s = string("hello");        // string
auto v = vector<int>{1, 2, 3};   // vector<int>

// Without auto (verbose)
vector<int>::iterator it1 = v.begin();

// With auto (concise)
auto it2 = v.begin();            // Same type, much cleaner!

// Auto in loops
for (auto val : v) {             // Type deduced as int
    cout << val << " ";
}

// Auto with complex types
map<string, vector<int>> m;
auto it = m.begin();             // Type: map<string, vector<int>>::iterator
```

### Auto With Pointers & References

```cpp
int value = 42;

auto ptr = &value;               // int* (pointer)
auto ref = value;                // int (by value)
auto& ref2 = value;              // int& (reference)
auto* ptr2 = &value;             // int* (explicit pointer)

const int cv = 10;
auto a = cv;                     // int (const lost!)
auto b = &cv;                    // const int*
const auto c = cv;               // const int (preserve const)

// Reference to const
const auto& d = cv;              // const int&
```

### Auto in Templates

```cpp
template<typename T>
void process(T value) {
    auto copy = value;           // Type is T
    // ...
}

// Without auto - would need explicit template parameter
template<typename T>
void oldWay(T value) {
    T copy = value;              // Must use T explicitly
}
```

### Auto Type Deduction Rules

```cpp
// Rule 1: Const is stripped unless explicitly written
const int ci = 5;
auto a = ci;                     // int (const stripped)
const auto b = ci;               // const int (const kept)

// Rule 2: Reference is stripped for value
int i = 10;
auto& ref = i;
auto c = ref;                    // int (& stripped)
auto d = &i;                     // int*

// Rule 3: Initializer list
auto e = {1, 2, 3};              // initializer_list<int>
vector<int> v = {1, 2, 3};       // vector<int> (different!)

// Rule 4: Function return type
auto add = [](int a, int b) { return a + b; };  // Return type deduced
```

### Auto Benefits & Limitations

**Benefits:**
- Reduces verbosity
- Refactoring-friendly (type changes automatically)
- Prevents narrowing conversions
- Works with complex types

**Limitations:**
- Readability can suffer (what's the type?)
- Can hide bugs
- Not available in C++98/03

**Best Practices:**
```cpp
// Good - obvious type
auto count = 0;                  // int - clear

// Questionable - unclear type
auto result = calculateSomething();  // What type?

// Solution - use explicit type or meaningful name
int result = calculateCount();    // Clear!
auto result_count = calculateCount();  // Name clarifies
```

---

## 1.2 decltype

The `decltype` keyword queries the type of an expression.

```cpp
#include <iostream>
using namespace std;

int x = 5;

// Get type of x
decltype(x) y = 10;              // int y = 10

// Get type of expression
auto result = 5 + 3;             // int
decltype(5 + 3) z = 20;          // int z = 20

// With references
int& ref = x;
decltype(ref) ref2 = x;          // int& ref2 = x

// Complex types
vector<int> v;
decltype(v.begin()) it = v.begin();  // vector<int>::iterator

// Function return type deduction (C++11)
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}

// Type matching
int i = 5;
double d = 3.14;
decltype(i + d) result = i + d;  // double (int + double = double)

// With function calls
int getNumber() { return 42; }
decltype(getNumber()) num = getNumber();  // int

// Advanced - trailing return type
template<typename T, typename U>
auto multiply(T a, U b) -> decltype(a * b) {
    return a * b;
}

// Checking types match
static_assert(is_same<decltype(x), int>::value);  // Compile-time check
```

### decltype vs auto

```cpp
// auto - deduces from initializer
auto a = 5;                      // int

// decltype - deduces from type
int i = 5;
decltype(i) b = 10;              // int

// Difference with references
int& ref = i;
auto c = ref;                    // int (reference stripped)
decltype(ref) d = i;             // int& (reference preserved)

// Practical use - return type deduction
template<typename T, typename U>
auto divide(T a, U b) -> decltype(a / b) {
    return a / b;
}
```

---

# SECTION 2: SMART POINTERS - MEMORY REVOLUTION

## 2.1 Unique Pointer (unique_ptr)

**unique_ptr** - exclusive ownership of dynamic object. One owner only.

### Basic Usage

```cpp
#include <memory>
using namespace std;

// Old way (C++98) - manual management, error-prone
int* old_ptr = new int(42);
cout << *old_ptr << "\n";
delete old_ptr;                  // Easy to forget!
old_ptr = nullptr;

// New way (C++11) - automatic management
unique_ptr<int> smart_ptr(new int(42));
cout << *smart_ptr << "\n";
// Automatically deleted when smart_ptr goes out of scope

// Better - use make_unique (C++14)
auto ptr = make_unique<int>(42);
cout << *ptr << "\n";
// Automatically deleted
```

### Unique Pointer Operations

```cpp
class Resource {
public:
    Resource() { cout << "Resource created\n"; }
    ~Resource() { cout << "Resource destroyed\n"; }
    void use() { cout << "Using resource\n"; }
};

// Create unique_ptr
unique_ptr<Resource> ptr1(new Resource());
unique_ptr<Resource> ptr2 = make_unique<Resource>();

// Access
ptr1->use();                     // Arrow operator
(*ptr1).use();                   // Dereference
if (ptr1) { }                    // Check null
ptr1.get();                      // Get raw pointer

// Move semantics (ownership transfer)
unique_ptr<Resource> ptr3 = move(ptr1);
// Now ptr3 owns the resource
// ptr1 is now null
cout << (ptr1 ? "owns" : "empty") << "\n";  // "empty"

// Array version
unique_ptr<int[]> arr(new int[10]);
arr[0] = 5;
arr[1] = 10;
// Automatically deleted with delete[]

// Reset
ptr3.reset();                    // Delete and become null
ptr3.reset(new Resource());      // Delete old, manage new

// Release (give up ownership)
Resource* raw = ptr3.release();  // Ownership transferred to you!
delete raw;                      // You must delete it

// Custom deleter
unique_ptr<FILE, decltype(&fclose)> file(fopen("test.txt", "r"), &fclose);
// File automatically closed
```

### Unique Pointer with Custom Deleters

```cpp
class Database {
public:
    Database() { cout << "DB connected\n"; }
    ~Database() { cout << "DB disconnected\n"; }
};

// Custom deleter function
void closeDB(Database* db) {
    cout << "Closing database...\n";
    delete db;
}

// Using custom deleter
unique_ptr<Database, decltype(&closeDB)> db(
    new Database(),
    &closeDB
);
// Output: DB connected
// When db goes out of scope:
// Output: Closing database...
//         DB disconnected

// With lambda deleter
auto deleter = [](Database* db) {
    cout << "Lambda deleting database\n";
    delete db;
};
unique_ptr<Database, decltype(deleter)> db2(new Database(), deleter);
```

### Unique Pointer in Collections

```cpp
vector<unique_ptr<Resource>> resources;

resources.push_back(make_unique<Resource>());
resources.push_back(make_unique<Resource>());
resources.push_back(make_unique<Resource>());

// Iterate and use
for (auto& res : resources) {
    res->use();
}
// All automatically cleaned up when vector destroyed
```

---

## 2.2 Shared Pointer (shared_ptr)

**shared_ptr** - shared ownership. Reference counting tracks multiple owners.

### Basic Usage

```cpp
#include <memory>

class Resource {
public:
    Resource() { cout << "Created\n"; }
    ~Resource() { cout << "Destroyed\n"; }
};

// Create shared_ptr
shared_ptr<Resource> ptr1 = make_shared<Resource>();
cout << ptr1.use_count() << "\n";              // 1

// Copy creates new reference
shared_ptr<Resource> ptr2 = ptr1;
cout << ptr1.use_count() << "\n";              // 2
cout << ptr2.use_count() << "\n";              // 2

// Create third reference
shared_ptr<Resource> ptr3 = ptr1;
cout << ptr1.use_count() << "\n";              // 3

// When ptr3 goes out of scope
{
    shared_ptr<Resource> ptr4 = ptr1;
    cout << ptr1.use_count() << "\n";          // 4
}
cout << ptr1.use_count() << "\n";              // 3 - ptr4 destroyed

// When all references gone, resource deleted
ptr1 = nullptr;
cout << ptr2.use_count() << "\n";              // 2
ptr2 = nullptr;
cout << ptr3.use_count() << "\n";              // 1
ptr3 = nullptr;
// Output: Destroyed (all references gone)
```

### Shared Pointer Operations

```cpp
shared_ptr<Resource> ptr = make_shared<Resource>();

// Access
ptr->use();
(*ptr).use();

// Check null
if (ptr) { }
if (ptr.get()) { }

// Reference counting
ptr.use_count();                 // Number of owners

// Reset
ptr.reset();                     // Decrement count, may delete

// Get raw pointer
Resource* raw = ptr.get();       // Don't delete raw!

// Convert to raw and create new shared_ptr from same resource
// WARNING: This is dangerous!
Resource* raw2 = ptr.get();
// shared_ptr<Resource> ptr2(raw2);  // DANGER! Two separate reference counts!

// Safe way - use enable_shared_from_this
class SafeResource : public enable_shared_from_this<SafeResource> {
public:
    shared_ptr<SafeResource> getSelf() {
        return shared_from_this();
    }
};
```

### Shared Pointer with Arrays & Custom Deleters

```cpp
// Array version
shared_ptr<int[]> arr(new int[10]);  // C++20
arr[0] = 5;

// Custom deleter
auto deleter = [](FILE* f) {
    cout << "Closing file\n";
    if (f) fclose(f);
};

shared_ptr<FILE> file(fopen("test.txt", "r"), deleter);
// File automatically closed when last reference gone
```

### Shared Pointers in Collections

```cpp
vector<shared_ptr<Resource>> resources;

resources.push_back(make_shared<Resource>());
resources.push_back(make_shared<Resource>());

shared_ptr<Resource> copy = resources[0];
cout << resources[0].use_count() << "\n";      // 2 (vector + copy)

// When we erase
resources.erase(resources.begin());
cout << copy.use_count() << "\n";              // Still 1 (copy still owns it)
```

---

## 2.3 Weak Pointer (weak_ptr)

**weak_ptr** - non-owning reference to object owned by shared_ptr. Prevents circular references.

### Circular Reference Problem

```cpp
// PROBLEM: Circular references cause memory leak
class Node {
public:
    shared_ptr<Node> next;
};

auto node1 = make_shared<Node>();
auto node2 = make_shared<Node>();

node1->next = node2;
node2->next = node1;  // CIRCULAR REFERENCE!

// When we set node1 and node2 to null:
node1 = nullptr;
node2 = nullptr;
// Leak! Both nodes are never deleted because they keep owning each other
```

### Solution with Weak Pointer

```cpp
class Node {
public:
    shared_ptr<Node> next;
    weak_ptr<Node> prev;  // Non-owning reference
};

auto node1 = make_shared<Node>();
auto node2 = make_shared<Node>();

node1->next = node2;           // node2 owned by node1
node2->prev = node1;           // node1 not owned by node2

// When node1 out of scope, node2 is deleted
// When node2 out of scope, node1 is deleted (if no other owners)
// No leak!
```

### Weak Pointer Usage

```cpp
weak_ptr<Resource> weak = ptr;  // Create weak_ptr from shared_ptr

// weak_ptr doesn't increment reference count
cout << ptr.use_count() << "\n";       // 1 (not incremented by weak)

// To use weak_ptr, must lock it to get shared_ptr
if (auto shared = weak.lock()) {
    // shared_ptr valid, resource still exists
    shared->use();
} else {
    // Resource was deleted
    cout << "Resource deleted\n";
}

// Check if expired
if (weak.expired()) {
    cout << "Resource deleted\n";
}
```

### Practical Circular Reference Solutions

```cpp
// Parent-Child relationship
class Parent;

class Child {
    weak_ptr<Parent> parent;  // Non-owning back-reference
public:
    void setParent(shared_ptr<Parent> p) {
        parent = p;
    }
};

class Parent {
    shared_ptr<Child> child;  // Owns child
public:
    void setChild(shared_ptr<Child> c) {
        child = c;
    }
};

auto parent = make_shared<Parent>();
auto child = make_shared<Child>();

parent->setChild(child);
child->setParent(parent);
// No circular reference! Safe to use
```

---

## Smart Pointer Comparison

```
                unique_ptr    shared_ptr    weak_ptr
Ownership       Exclusive     Shared        None
Copy            No (move)     Yes           No
Reference Count No            Yes           No
Use Count       N/A           Yes           No
Overhead        Minimal       Medium        Medium
Thread Safe     No            Yes (atomic)  Yes (atomic)
Circular Ref    No issue      Problem       Solution

When to use:
unique_ptr      - Single owner, exclusive access
shared_ptr      - Multiple owners needed
weak_ptr        - Break circular references
```

---

# SECTION 3: MOVE SEMANTICS & RVALUE REFERENCES

## 3.1 Lvalue vs Rvalue

Understanding the difference is crucial for C++11.

```cpp
// LVALUE - has memory address, survives beyond expression
int x = 5;            // x is lvalue (persists)
int& ref = x;         // Can bind lvalue reference

// RVALUE - temporary, doesn't have persistent storage
int y = x + 3;        // (x+3) is rvalue (temporary)
// int& ref2 = (x+3); // ERROR! Can't bind lvalue ref to rvalue

// Rvalue reference binding (C++11)
int&& rref = x + 3;   // OK! Rvalue reference to temporary
cout << rref << "\n"; // 8

// But can't bind to lvalue
int a = 10;
// int&& rref2 = a;   // ERROR! Can't bind to lvalue
```

### Lvalue & Rvalue Examples

```cpp
int a = 5;

// Lvalue expressions (have addresses)
a;                    // lvalue
a + 0;                // rvalue (result is temporary)
a = 10;               // lvalue (a itself is lvalue)
a++;                  // rvalue (returns temporary copy)
++a;                  // lvalue (returns reference to a)

// Function returning by reference - lvalue
int& getRef() { static int x; return x; }
getRef() = 20;        // lvalue - can assign to

// Function returning by value - rvalue
int getValue() { return 42; }
// getValue() = 20;   // ERROR - rvalue can't be assigned to

// String examples
string s = "hello";   // s is lvalue
string t = s;         // s is lvalue
string u = "world";   // "world" is rvalue
// u = getValue();     // rvalue expression
```

---

## 3.2 Move Constructor & Move Assignment

Move semantics avoid expensive copying by transferring ownership.

### Before C++11 - Expensive Copy

```cpp
class String {
private:
    char* data;
    size_t size;

public:
    // Copy constructor - EXPENSIVE
    String(const String& other) {
        cout << "Copy constructor\n";
        size = other.size;
        data = new char[size + 1];
        strcpy(data, other.data);  // Copy memory
    }

    // Assignment - EXPENSIVE
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size + 1];
            strcpy(data, other.data);
        }
        return *this;
    }

    ~String() {
        delete[] data;
    }
};

// This calls copy constructor - expensive!
String s1 = "hello";
String s2 = s1;        // Copy all data
```

### With C++11 - Efficient Move

```cpp
class String {
private:
    char* data;
    size_t size;

public:
    // Copy constructor
    String(const String& other) {
        cout << "Copy constructor\n";
        size = other.size;
        data = new char[size + 1];
        strcpy(data, other.data);
    }

    // MOVE constructor (C++11) - EFFICIENT
    String(String&& other) noexcept {
        cout << "Move constructor\n";
        data = other.data;         // Transfer ownership
        size = other.size;
        other.data = nullptr;      // Leave other empty
        other.size = 0;
    }

    // Copy assignment
    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            size = other.size;
            data = new char[size + 1];
            strcpy(data, other.data);
        }
        return *this;
    }

    // MOVE assignment (C++11) - EFFICIENT
    String& operator=(String&& other) noexcept {
        cout << "Move assignment\n";
        if (this != &other) {
            delete[] data;
            data = other.data;     // Transfer ownership
            size = other.size;
            other.data = nullptr;  // Leave other empty
            other.size = 0;
        }
        return *this;
    }

    ~String() {
        delete[] data;
    }
};

// Compiler chooses move constructor (rvalue)
String s1 = String("hello");  // Move constructor called, not copy!

// Explicit move
String s2 = "world";
String s3 = move(s2);         // Move assignment called
```

### Move Constructor Details

```cpp
class Vector {
private:
    int* data;
    int size;

public:
    // Regular constructor
    Vector(int n) : size(n), data(new int[n]) {}

    // Move constructor signature: T(T&& other) noexcept
    Vector(Vector&& other) noexcept
        : size(other.size), data(other.data) {
        other.data = nullptr;   // Critical: leave source empty!
        other.size = 0;
    }

    // Move assignment
    Vector& operator=(Vector&& other) noexcept {
        if (this != &other) {
            delete[] data;      // Clean up old data
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }

    ~Vector() {
        delete[] data;
    }
};

Vector createVector(int n) {
    Vector v(n);
    // Initialize...
    return v;  // Move constructor called, not copy
}

Vector v = createVector(100);  // Efficient! Only one construction
```

### std::move

Use `std::move()` to force move semantics on lvalues.

```cpp
#include <utility>

Vector v1(100);
Vector v2(200);

// Copy
v2 = v1;  // Copy assignment (v1 still valid)

// Move
v2 = move(v1);  // Move assignment (v1 becomes invalid)

// After move, v1 is valid but empty
cout << v1.size() << "\n";  // 0 (moved from)

// Perfect for returning from function
Vector getVector() {
    Vector v(100);
    // Initialize...
    return v;  // Compiler elides copy, uses move anyway
}

// Container operations use move
vector<Vector> vv;
Vector v3(50);
vv.push_back(move(v3));  // Move, not copy

// Array operations
string s1 = "hello";
string s2 = move(s1);    // Move string data
cout << s1 << "\n";      // Empty (s1 moved from)
```

---

## 3.3 Perfect Forwarding

Pass parameters to another function preserving their value category (lvalue/rvalue).

### The Problem

```cpp
// Without perfect forwarding, you lose the value category
template<typename T>
void wrapper(T& arg) {
    // arg is always lvalue
    process(arg);  // Can't call rvalue version
}

// Solution: Perfect forwarding with std::forward
template<typename T>
void perfectWrapper(T&& arg) {
    // arg is universal reference
    // std::forward preserves whether arg was lvalue or rvalue
    process(forward<T>(arg));
}
```

### Implementation

```cpp
#include <utility>

void process(int& x) {
    cout << "Lvalue reference\n";
}

void process(int&& x) {
    cout << "Rvalue reference\n";
}

// WITHOUT perfect forwarding
template<typename T>
void wrapper1(T arg) {
    process(arg);  // arg is lvalue, always calls lvalue version
}

// WITH perfect forwarding
template<typename T>
void wrapper2(T&& arg) {
    process(forward<T>(arg));  // Forwards correctly
}

int main() {
    int x = 5;

    wrapper1(x);          // Lvalue reference (correct)
    wrapper1(10);         // Lvalue reference (WRONG! Should be rvalue)

    wrapper2(x);          // Lvalue reference (correct)
    wrapper2(10);         // Rvalue reference (correct!)

    return 0;
}
```

### Perfect Forwarding with Multiple Parameters

```cpp
template<typename T1, typename T2>
void forward_to_process(T1&& arg1, T2&& arg2) {
    process(forward<T1>(arg1), forward<T2>(arg2));
}

// Works correctly with all combinations:
forward_to_process(lval1, lval2);  // Both forwarded as lvalues
forward_to_process(rval1, rval2);  // Both forwarded as rvalues
forward_to_process(lval1, rval2);  // Mixed - each forwarded correctly
```

### Universal Reference

The parameter `T&&` is called a **universal reference** (or forwarding reference) when T is a template parameter.

```cpp
// Universal reference - can be lvalue or rvalue
template<typename T>
void universal(T&& param) { }

// Rvalue reference - always rvalue
void notUniversal(int&& param) { }  // Not template

class X {
    // This is rvalue reference (not universal, because class is known)
    void method(int&& x) { }
};

// When you see T&&:
// 1. If T is deduced from template parameter → Universal reference
// 2. Otherwise → Rvalue reference
```

---

# SECTION 4: LAMBDA FUNCTIONS

## 4.1 Lambda Basics

Lambda functions are anonymous functions that can capture variables.

```cpp
#include <iostream>
using namespace std;

// Simplest lambda - no parameters, no capture
auto greet = []() {
    cout << "Hello!\n";
};
greet();

// Lambda with parameters
auto add = [](int a, int b) {
    return a + b;
};
cout << add(5, 3) << "\n";  // 8

// Lambda with return type (usually not needed)
auto multiply = [](int a, int b) -> int {
    return a * b;
};

// Lambda with auto parameter (C++14)
auto square = [](auto x) {
    return x * x;
};
cout << square(5) << "\n";      // 25
cout << square(3.14) << "\n";   // 9.8596
```

### Lambda with Capture

Capture variables from enclosing scope.

```cpp
int global_x = 10;

// Capture by value [=]
auto cap_value = [global_x]() {
    cout << global_x << "\n";   // Captures value at creation
    // global_x = 20;           // ERROR - can't modify captured value
};

// Capture by reference [&]
auto cap_ref = [&global_x]() {
    cout << global_x << "\n";   // Uses current value
    global_x = 20;              // OK - modifies original
};

// Capture by value with default [=, &x]
int x = 5, y = 10;
auto mixed1 = [=, &x]() {
    // y captured by value, x by reference
};

// Capture by reference with default [&, =x]
auto mixed2 = [&, x]() {
    // x captured by value, others by reference
};

// Specific captures
auto specific = [x, y, &global_x]() {
    // x, y by value; global_x by reference
};
```

### Lambda Capture Examples

```cpp
#include <vector>

vector<int> v = {1, 2, 3, 4, 5};
int multiplier = 2;

// Capture multiplier for use in lambda
vector<int> result;

// Method 1: Capture by value
transform(v.begin(), v.end(), back_inserter(result),
    [multiplier](int x) {
        return x * multiplier;  // multiplier captured
    });

// Method 2: Capture by reference
int sum = 0;
for_each(v.begin(), v.end(),
    [&sum](int x) {
        sum += x;  // Modifies original sum
    });

// Method 3: Implicit capture
int factor = 3;
auto calc = [=]() {
    return v[0] * factor;  // factor automatically captured
};

// Method 4: Mutable lambda
auto counter = [count = 0]() mutable {
    return ++count;
};
cout << counter() << "\n";  // 1
cout << counter() << "\n";  // 2
cout << counter() << "\n";  // 3
```

---

## 4.2 Lambda with Algorithms

Lambdas are perfect for use with STL algorithms.

```cpp
#include <algorithm>
#include <vector>

vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

// Find first even number
auto it = find_if(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; });
cout << *it << "\n";  // 2

// Count numbers > 5
int count = count_if(v.begin(), v.end(),
    [](int x) { return x > 5; });
cout << count << "\n";  // 5

// Transform: square each element
transform(v.begin(), v.end(), v.begin(),
    [](int x) { return x * x; });

// Sort with custom comparator
sort(v.begin(), v.end(),
    [](int a, int b) { return a > b; });  // Descending

// Remove if
v.erase(remove_if(v.begin(), v.end(),
    [](int x) { return x % 2 == 0; }), v.end());
```

---

## 4.3 Advanced Lambda Features

### Init Capture (Generalized Capture - C++14)

```cpp
auto ptr = make_unique<int>(42);

// Can't capture unique_ptr directly (move-only)
// auto lambda = [ptr = move(ptr)]() { };  // ERROR in C++11

// C++14 - init capture allows move
auto lambda = [ptr = move(ptr)]() {
    cout << *ptr << "\n";
};
// Now lambda owns the unique_ptr!

// Other init capture examples
auto value_capture = [x = 5]() { return x; };  // Initializes x to 5
auto copy_capture = [v = original_vector]() { };  // Copies vector
```

### Recursive Lambdas

```cpp
// Lambdas can call themselves if captured
function<int(int)> factorial;  // Forward declare
factorial = [&factorial](int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
};

cout << factorial(5) << "\n";  // 120
```

### Const and Mutable Lambdas

```cpp
// By default, lambdas are const
auto const_lambda = [x = 0]() {
    // x = 1;  // ERROR - can't modify captured variables
};

// Mutable lambda can modify captured variables
auto mutable_lambda = [x = 0]() mutable {
    x++;  // OK - modifies copy of x
    return x;
};

cout << mutable_lambda() << "\n";  // 1
cout << mutable_lambda() << "\n";  // 2 (x persists)
```

## 4.4 Lambda Under the Hood

When you write a lambda, the compiler generates a class (functor) for you.

**Source Code:**
```cpp
int factor = 10;
auto lambda = [factor](int x) { return x * factor; };
```

**Compiler Generated Code (Approximate):**
```cpp
class __Lambda_1 {
private:
    int m_factor; // Captured variable
public:
    __Lambda_1(int factor) : m_factor(factor) {}
    
    // operator() is const by default!
    int operator()(int x) const {
        return x * m_factor;
    }
};

// Usage
__Lambda_1 lambda(factor);
```

**Mutable Lambda:**
If you use `mutable`, the `operator()` is NOT `const`, allowing modification of `m_factor`.

**Capture by Reference:**
```cpp
class __Lambda_Ref {
    int& m_ref; // Pointer under the hood
public:
    __Lambda_Ref(int& ref) : m_ref(ref) {}
    int operator()(int x) const { return x * m_ref; }
};
```

---

# SECTION 5: VARIADIC TEMPLATES

## 5.1 Parameter Packs

Templates that accept variable number of parameters.

### Basic Variadic Template

```cpp
#include <iostream>
using namespace std;

// Base case (template specialization for no parameters)
template<typename T>
T sum(T val) {
    return val;
}

// Recursive case (typename... is parameter pack)
template<typename T, typename... Rest>
T sum(T first, Rest... rest) {
    return first + sum(rest...);
}

int main() {
    cout << sum(1, 2, 3, 4, 5) << "\n";              // 15
    cout << sum(1.5, 2.5, 3.0) << "\n";              // 7.0
    cout << sum("hello") << "\n";                    // "hello"

    return 0;
}
```

### Understanding Parameter Packs

```cpp
// template<typename... Args>
//     ^         ^       ^
//     |         |       |
//    keyword  pattern  name

template<typename... Types>
void printTypes() {
    // Types is a parameter pack
}

// Calling with different numbers of parameters
printTypes<int>();                    // Types = <int>
printTypes<int, double>();            // Types = <int, double>
printTypes<int, double, string>();    // Types = <int, double, string>
```

### Pack Expansion

```cpp
// sizeof... operator
template<typename... Args>
void printCount(Args... args) {
    cout << "Number of arguments: " << sizeof...(Args) << "\n";
    cout << "Number of values: " << sizeof...(args) << "\n";
}

printCount(1, 2, 3);        // Both print: 3
printCount("a", 1.5);       // Both print: 2
printCount();               // Both print: 0
```

### Variadic Function Print Example

```cpp
#include <iostream>
using namespace std;

// Base case - end recursion
void print() { }

// Recursive case
template<typename T, typename... Rest>
void print(T first, Rest... rest) {
    cout << first << " ";
    print(rest...);  // Pack expansion - recursively process rest
}

int main() {
    print(1, 2, 3, 4, 5);              // 1 2 3 4 5
    print("hello", 3.14, 42);          // hello 3.14 42
    print();                           // (nothing)

    return 0;
}
```

### Fold Expressions (C++17)

Modern way to process parameter packs.

```cpp
// C++17 - Fold expressions (much cleaner!)
template<typename... Args>
auto sum(Args... args) {
    return (... + args);  // Left fold: ((a + b) + c) + d
}

template<typename... Args>
auto product(Args... args) {
    return (... * args);  // Left fold: ((a * b) * c) * d
}

cout << sum(1, 2, 3, 4) << "\n";        // 10
cout << product(2, 3, 4) << "\n";       // 24

// Other fold directions
// (args + ...) is right fold
// (args * ...) with no operator is right fold
```

---

# SECTION 6: RANGE-BASED FOR LOOPS

## 6.1 Iterating Over Containers

```cpp
#include <vector>
#include <string>

vector<int> v = {1, 2, 3, 4, 5};

// Old way (C++98)
for (int i = 0; i < v.size(); i++) {
    cout << v[i] << " ";
}

// Range-based for (C++11)
for (int val : v) {
    cout << val << " ";
}

// With auto (cleaner)
for (auto val : v) {
    cout << val << " ";
}

// By reference (to modify)
for (auto& val : v) {
    val *= 2;  // Modifies original
}

// Const reference (efficient, can't modify)
for (const auto& val : v) {
    cout << val << " ";
}
```

### Range-Based For with Different Containers

```cpp
// Vector
vector<int> vec = {1, 2, 3};
for (auto x : vec) { cout << x << " "; }

// Array
int arr[] = {4, 5, 6};
for (auto x : arr) { cout << x << " "; }

// String
string s = "hello";
for (auto c : s) { cout << c << " "; }  // h e l l o

// Map
map<string, int> m = {{"a", 1}, {"b", 2}};
for (auto [key, value] : m) {  // C++17 structured binding
    cout << key << ": " << value << " ";
}

// Set
set<int> s = {10, 20, 30};
for (auto x : s) { cout << x << " "; }

// String iteration
string text = "hello";
for (char c : text) {
    cout << c << " ";  // h e l l o
}
```

### Custom Range-Based For

```cpp
// Custom container with iterator support
class MyContainer {
private:
    int data[5] = {1, 2, 3, 4, 5};

public:
    int* begin() { return data; }
    int* end() { return data + 5; }
};

MyContainer container;
for (auto val : container) {
    cout << val << " ";  // Works with range-based for!
}

// Or with const
class MyList {
private:
    vector<int> items;

public:
    MyList() : items({10, 20, 30}) {}
    
    auto begin() { return items.begin(); }
    auto end() { return items.end(); }
    auto begin() const { return items.cbegin(); }
    auto end() const { return items.cend(); }
};
```

---

# SECTION 7: UNIFORM INITIALIZATION

## 7.1 Brace Initialization

```cpp
#include <vector>
#include <string>

// Before C++11
int a = 5;
vector<int> v;
v.push_back(1);
v.push_back(2);
v.push_back(3);

// C++11 Uniform Initialization
int b{5};                          // Direct initialization
vector<int> v2{1, 2, 3, 4, 5};     // List initialization
string s{"hello"};                 // Constructor call

// Arrays
int arr[5] = {1, 2, 3};            // Old way
int arr2[5]{1, 2, 3};              // New way

// Structs
struct Point {
    int x, y;
};
Point p{10, 20};                   // Uniform initialization

// Classes
class Rectangle {
public:
    Rectangle(int w, int h) {}
};
Rectangle r{100, 50};              // Uniform initialization
```

### Initializer Lists

```cpp
#include <initializer_list>

// Container initialization
vector<int> v{1, 2, 3, 4, 5};      // Built-in support
set<int> s{5, 3, 1, 4, 2};

// Custom class with initializer_list
class Numbers {
private:
    vector<int> data;

public:
    Numbers(initializer_list<int> init) : data(init) {}
};

Numbers nums{1, 2, 3, 4, 5};

// Function with initializer_list
void printNumbers(initializer_list<int> nums) {
    for (int n : nums) {
        cout << n << " ";
    }
}

printNumbers({10, 20, 30});
```

---

# SECTION 8: NULLPTR & STRONGLY TYPED ENUMS

## 8.1 nullptr

```cpp
// Before C++11 - NULL was problematic
#define NULL 0
int* ptr = NULL;               // Actually 0, not a pointer!
void func(int x) { }
void func(int* ptr) { }
func(NULL);                    // Ambiguous! Which func?

// C++11 - nullptr
int* ptr = nullptr;            // Actual null pointer type
void func(int* ptr) { }
func(nullptr);                 // Clear - calls pointer version

// nullptr conversions
bool b = nullptr;              // ERROR - can't convert to bool
if (ptr == nullptr) { }        // OK
if (!ptr) { }                  // OK - nullptr is falsy
if (ptr) { }                   // OK - checks if not null

// nullptr type
nullptr_t null_val = nullptr;
```

## 8.2 Strongly Typed Enums

```cpp
// Old enum (C++98) - pollutes namespace
enum Color {
    RED,      // Color::RED? RED?
    GREEN,
    BLUE
};
int x = RED;                   // Implicit conversion to int!

// New scoped enum (C++11)
enum class Status {
    OK,                        // Must use Status::OK
    ERROR,
    PENDING
};

Status s = Status::OK;
// int i = Status::OK;         // ERROR - no implicit conversion

// Enum with specific type
enum class Priority : unsigned char {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3
};

Priority p = Priority::HIGH;
unsigned char value = (unsigned char)p;  // Explicit cast
```

---

# SECTION 9: TUPLE, ARRAY, & UNORDERED CONTAINERS

## 9.1 Tuple

```cpp
#include <tuple>

// Creating tuple
tuple<int, string, double> t1(42, "hello", 3.14);
auto t2 = make_tuple(100, "world", 2.71);

// Accessing elements
cout << get<0>(t1) << "\n";       // 42
cout << get<1>(t1) << "\n";       // "hello"
cout << get<2>(t1) << "\n";       // 3.14

// Unpacking (C++17)
auto [num, str, dec] = t1;
cout << num << ", " << str << ", " << dec << "\n";

// Tuple size
cout << tuple_size<decltype(t1)>::value << "\n";  // 3

// Compare tuples
auto t3 = make_tuple(42, "hello", 3.14);
cout << (t1 == t3) << "\n";       // 1 (true)

// Tuple of references
int a = 5, b = 10;
auto t4 = tie(a, b);              // Tuple of references
get<0>(t4) = 20;
cout << a << "\n";                // 20
```

## 9.2 Array

```cpp
#include <array>

// std::array - type-safe, fixed-size array
array<int, 5> arr{1, 2, 3, 4, 5};

// Access elements
cout << arr[0] << "\n";           // 1
cout << arr.at(1) << "\n";        // 2

// Iteration
for (auto x : arr) {
    cout << x << " ";
}

// Size operations
cout << arr.size() << "\n";       // 5
cout << arr.empty() << "\n";      // false

// Use with algorithms
sort(arr.begin(), arr.end());
reverse(arr.begin(), arr.end());

// Compare arrays
array<int, 5> arr2{1, 2, 3, 4, 5};
cout << (arr == arr2) << "\n";    // false (different order after sort)

// Multi-dimensional
array<array<int, 3>, 3> matrix{
    {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}}
};
cout << matrix[0][0] << "\n";     // 1
```

## 9.3 Unordered Containers

```cpp
#include <unordered_map>
#include <unordered_set>

// Unordered Map (hash table)
unordered_map<string, int> map{
    {"one", 1}, {"two", 2}, {"three", 3}
};

map["four"] = 4;                  // Insert
cout << map["one"] << "\n";       // 1
map.erase("two");                 // Remove

// Iteration (order is arbitrary)
for (const auto& [key, value] : map) {
    cout << key << ": " << value << "\n";
}

// Hash info
cout << map.bucket_count() << "\n";     // Number of buckets
cout << map.load_factor() << "\n";      // Load factor
cout << map.max_load_factor() << "\n";  // Max load factor

// Unordered Set
unordered_set<int> set{5, 3, 1, 4, 2};

if (set.count(3)) {
    cout << "3 found\n";
}

// Custom hash function
struct StringHash {
    size_t operator()(const string& s) const {
        hash<string> h;
        return h(s);
    }
};

unordered_set<string, StringHash> customSet{"a", "b", "c"};
```

---

# SECTION 10: DECLTYPE & TYPE TRAITS

## 10.1 decltype

Already covered in detail in Section 1.2, but here are more examples:

```cpp
int x = 5;
decltype(x) y = 10;                // int

vector<int> v;
decltype(v.begin()) it = v.begin(); // vector<int>::iterator

// In template context
template<typename T, typename U>
auto divide(T a, U b) -> decltype(a / b) {
    return a / b;
}

// Type checking
cout << is_same<decltype(x), int>::value << "\n";  // true
cout << is_same<decltype(3.14), double>::value << "\n";  // true
```

## 10.2 Type Traits

```cpp
#include <type_traits>

// Checking types
cout << is_integral<int>::value << "\n";           // true
cout << is_integral<double>::value << "\n";        // false
cout << is_floating_point<double>::value << "\n";  // true
cout << is_pointer<int*>::value << "\n";           // true
cout << is_reference<int&>::value << "\n";         // true

// Removing qualifiers
remove_const<const int>::type a = 5;  // int
remove_reference<int&>::type b = 10;  // int

// Adding qualifiers
add_const<int>::type c = 5;           // const int
add_pointer<int>::type d = &a;        // int*

// Checking relationships
cout << is_same<int, int>::value << "\n";          // true
cout << is_same<int, double>::value << "\n";       // false

// Conditional types
conditional<true, int, double>::type e = 5;  // int
conditional<false, int, double>::type f = 3.14;  // double

// Function traits
template<typename T>
struct is_iterable {
    static constexpr bool value = 
        is_integral_v<T> || is_floating_point_v<T>;
};

// SFINAE (Substitution Failure Is Not An Error)
template<typename T>
enable_if_t<is_integral<T>::value>
process(T value) {
    cout << "Processing integer: " << value << "\n";
}

template<typename T>
enable_if_t<is_floating_point<T>::value>
process(T value) {
    cout << "Processing float: " << value << "\n";
}

process(42);      // "Processing integer: 42"
process(3.14);    // "Processing float: 3.14"
```

---

# SECTION 11: CONCURRENCY & THREADING

## 11.1 Threads

```cpp
#include <thread>
#include <iostream>

// Simple thread
void workerFunction() {
    cout << "Worker thread executing\n";
}

int main() {
    // Create and launch thread
    thread t(workerFunction);
    
    // Wait for thread to finish
    t.join();
    
    cout << "Main thread continues\n";
    
    return 0;
}
```

### Threading with Parameters

```cpp
#include <thread>
#include <iostream>

void add(int a, int b) {
    cout << a << " + " << b << " = " << (a + b) << "\n";
}

int main() {
    // Pass parameters to thread function
    thread t1(add, 5, 3);
    thread t2(add, 10, 20);
    
    t1.join();
    t2.join();
    
    return 0;
}
```

### Thread with Lambda

```cpp
#include <thread>

int main() {
    int value = 42;
    
    thread t([value]() {
        cout << "Lambda captured: " << value << "\n";
    });
    
    t.join();
    
    return 0;
}
```

### Multiple Threads

```cpp
#include <thread>
#include <vector>

int main() {
    vector<thread> threads;
    
    // Create multiple threads
    for (int i = 0; i < 5; i++) {
        threads.emplace_back([i]() {
            cout << "Thread " << i << " executing\n";
        });
    }
    
    // Wait for all
    for (auto& t : threads) {
        t.join();
    }
    
    return 0;
}
```

---

## 11.2 Mutex & Locks

```cpp
#include <thread>
#include <mutex>

int counter = 0;
mutex mtx;

void incrementCounter() {
    for (int i = 0; i < 100000; i++) {
        lock_guard<mutex> lock(mtx);  // RAII locking
        counter++;
    }
}

int main() {
    thread t1(incrementCounter);
    thread t2(incrementCounter);
    
    t1.join();
    t2.join();
    
    cout << "Counter: " << counter << "\n";  // 200000
    
    return 0;
}
```

---

## 11.3 Atomic Operations

```cpp
#include <atomic>
#include <thread>

atomic<int> counter(0);

void incrementAtomic() {
    for (int i = 0; i < 100000; i++) {
        counter++;  // Atomic operation, thread-safe
    }
}

int main() {
    thread t1(incrementAtomic);
    thread t2(incrementAtomic);
    
    t1.join();
    t2.join();
    
    cout << "Counter: " << counter << "\n";  // 200000
    
    return 0;
}
```

---

# SECTION 12: REGULAR EXPRESSIONS

## 12.1 Basic Regex

```cpp
#include <regex>
#include <string>
#include <iostream>

// Create regex pattern
regex pattern("\\d+");  // Match one or more digits

string text = "The number is 42";

// Check if matches
if (regex_search(text, pattern)) {
    cout << "Found digits\n";
}

// Extract matches
smatch match;
if (regex_search(text, match, pattern)) {
    cout << "Matched: " << match[0] << "\n";  // "42"
}

// Replace
string result = regex_replace(text, pattern, "XXX");
cout << result << "\n";  // "The number is XXX"

// Multiple matches
pattern = regex("\\d+");
string text2 = "Numbers: 10, 20, 30";

sregex_iterator iter(text2.begin(), text2.end(), pattern);
sregex_iterator end;

while (iter != end) {
    cout << iter->str() << "\n";  // 10, 20, 30
    ++iter;
}
```

---

# SECTION 13: NEW LIBRARY FEATURES

## 13.1 Standard Library Additions

### Chrono Library

```cpp
#include <chrono>
#include <thread>

using namespace std::chrono;

// Time point
auto start = high_resolution_clock::now();

// Simulate work
this_thread::sleep_for(milliseconds(100));

auto end = high_resolution_clock::now();

// Duration
auto elapsed = duration_cast<milliseconds>(end - start);
cout << "Elapsed: " << elapsed.count() << " ms\n";
```

### Random Number Generation

```cpp
#include <random>
#include <iostream>

// Seed
random_device rd;
mt19937 gen(rd());

// Distribution
uniform_int_distribution<> dis(1, 6);  // Roll dice

// Generate numbers
for (int i = 0; i < 10; i++) {
    cout << dis(gen) << " ";  // 1-6
}

// Different distributions
normal_distribution<> normal(0, 1);  // Mean 0, stddev 1
uniform_real_distribution<> real(0.0, 1.0);  // 0.0-1.0

cout << normal(gen) << "\n";
cout << real(gen) << "\n";
```

### Function

```cpp
#include <functional>

// Store any callable
function<int(int, int)> f1 = [](int a, int b) { return a + b; };
function<int(int, int)> f2 = plus<int>();

cout << f1(5, 3) << "\n";  // 8
cout << f2(5, 3) << "\n";  // 8

// Function vector
vector<function<void()>> tasks;

tasks.push_back([]() { cout << "Task 1\n"; });
tasks.push_back([]() { cout << "Task 2\n"; });

for (auto& task : tasks) {
    task();
}
```

---

## <a name="chapter-8-advancedmovesemanticsvaluecategories"></a>CHAPTER 8: ADVANCED MOVE SEMANTICS & VALUE CATEGORIES

"Move Semantics" is often misunderstood. It's not magic; it's type casting.

### 4.5.1 The C++17 Value Category Taxonomy
Everything in C++ is an Expression, and every expression has a **Type** and a **Value Category**.

```text
        Expression
       /          \
   glvalue      rvalue
   /    \      /     \
lvalue   xvalue   prvalue
```

1.  **lvalue (Identity + Movable? No)**: Has a name, persists beyond expression.
    *   `int x; x` is an lvalue.
    *   `std::string s; s` is an lvalue.
2.  **prvalue (Pure Rvalue)**: No name, temporary, initializes an object.
    *   `10`, `true`, `nullptr`.
    *   `std::string("hello")` (constructor call).
3.  **xvalue (eXpiring Value)**: Has identity, but can be moved from.
    *   Result of `std::move(x)`.
    *   Rvalue reference cast `static_cast<T&&>(x)`.

**glvalue** = lvalue + xvalue (Has Identity)
**rvalue** = prvalue + xvalue (Can be moved from)

### 4.5.2 std::move and std::forward Internals

**`std::move`**: Does NOT move. It unconditionally casts to rvalue reference.
```cpp
template<typename T>
typename remove_reference<T>::type&& move(T&& t) noexcept {
    return static_cast<typename remove_reference<T>::type&&>(t);
}
```

**`std::forward`**: Conditionally casts to rvalue reference *only if* the argument was initialized with an rvalue.
Used for **Perfect Forwarding**.

```cpp
template<typename T>
T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}
```

### 4.5.3 Reference Collapsing Rules
When templates meet references, types collapse:

*   `T& &`   -> `T&`
*   `T& &&`  -> `T&`
*   `T&& &`  -> `T&`
*   `T&& &&` -> `T&&` (The only way to get an rvalue reference)

This is why `T&&` in a template is a **Universal Reference** (Forwarding Reference). It can become `T&` (lvalue) or `T&&` (rvalue).

---

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

### 1. Object Slicing
Passing a derived object by value to a function expecting a base class strips off the derived part.
*   **Fix**: Pass by reference (`Base&`) or pointer (`Base*`).

### 2. Iterator Invalidation
Modifying a container (e.g., `vector::push_back`) can invalidate existing iterators if reallocation occurs.
*   **Fix**: Do not cache iterators across mutating operations; check container documentation.

### 3. Dangling References
Returning a reference to a local variable.
*   **Fix**: Return by value or ensure the referenced object outlives the reference (e.g., static/heap).

### 4. Static Initialization Order Fiasco
Global objects in different translation units have no defined initialization order.
*   **Fix**: Use the "Construct On First Use" idiom (static variable inside a function).

### 5. Most Vexing Parse
`MyClass obj();` is a function declaration, not an object instantiation.
*   **Fix**: Use brace initialization `MyClass obj{};`.

### 6. Undefined Behavior (UB)
Signed integer overflow, dereferencing null, accessing out of bounds.
*   **Fix**: Use Sanitizers (ASan, UBSan) and perform bounds checking (`at()` vs `[]`).

### 7. Resource Leaks
Manual `new`/`delete` usage often leads to leaks.
*   **Fix**: Always use RAII (`std::unique_ptr`, `std::vector`, etc.).

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

C++ rarely lives in isolation. It powers Python, Java, and Browsers.

### 25.1 Python Bindings with pybind11
Expose C++ performance to Python scripts.

```cpp
#include <pybind11/pybind11.h>

int add(int i, int j) { return i + j; }

PYBIND11_MODULE(example, m) {
    m.doc() = "pybind11 example plugin";
    m.def("add", &add, "A function which adds two numbers");
}
// In Python: import example; example.add(1, 2)
```

### 25.2 Stable C ABI for DLLs
To share code between compilers (MSVC/GCC) or languages (C#, Rust), use `extern "C"`.

```cpp
// header.h
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) void* CreateInstance();
__declspec(dllexport) void DestroyInstance(void* ptr);

#ifdef __cplusplus
}
#endif
```

---

## <a name="chapter-36-securityengineering"></a>CHAPTER 36: SECURITY ENGINEERING

Writing fast code is easy. Writing fast *and* secure code is Godhood.

### 26.1 Fuzzing (libFuzzer)
Fuzzing involves feeding random, invalid inputs to your program to find crashes.

```cpp
// fuzz_target.cc
#include <cstdint>
#include <cstddef>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    // Call your function here
    // parse_packet(Data, Size);
    return 0; // Non-zero return values are reserved for future use.
}
```
Compile with: `clang++ -fsanitize=fuzzer fuzz_target.cc`

### 26.2 Secure Coding Practices (SEI CERT C++)
1.  **Do not use `strcpy`, `sprintf`**: Use `std::string` or `snprintf` with bounds.
2.  **Avoid Raw Pointers**: Use `std::unique_ptr` to prevent Use-After-Free (UAF).
3.  **Validate External Input**: Never trust network packets or file headers.
4.  **Integer Overflow**: Use `std::checked_*` (if available) or manual checks for arithmetic on untrusted data.

### 26.3 Exploit Mitigation
*   **ASLR**: Address Space Layout Randomization.
*   **DEP**: Data Execution Prevention (NX bit).
*   **Stack Canaries**: Compiler inserts a sentinel value on stack to detect overflow.

---

## <a name="chapter-37-specializeddomains"></a>CHAPTER 37: SPECIALIZED DOMAINS

This section explores how C++ is applied in specific high-demand industries.

### 12.1 Game Development (ECS Pattern)
In game development, the **Entity-Component-System (ECS)** pattern is preferred over inheritance for cache locality and composition.

```cpp
#include <vector>
#include <optional>
#include <iostream>

// Component: Pure Data
struct Position { float x, y; };
struct Velocity { float dx, dy; };

// System: Logic
class PhysicsSystem {
public:
    void update(std::vector<Position>& pos, const std::vector<Velocity>& vel, float dt) {
        for (size_t i = 0; i < pos.size(); ++i) {
            pos[i].x += vel[i].dx * dt;
            pos[i].y += vel[i].dy * dt;
        }
    }
};

// Entity is just an ID (index)
int main() {
    // Structure of Arrays (SoA) for cache efficiency
    std::vector<Position> positions(1000);
    std::vector<Velocity> velocities(1000);
    
    PhysicsSystem physics;
    
    // Game Loop
    for (int frame = 0; frame < 60; ++frame) {
        physics.update(positions, velocities, 0.016f);
    }
    
    return 0;
}
```

### 12.2 Embedded Systems
Embedded C++ often disables exceptions and RTTI, relying on `constexpr` and templates.

```cpp
#include <cstdint>

// Memory-Mapped I/O helper
template<uintptr_t Address, typename T = volatile uint32_t>
struct Reg {
    static void write(T value) {
        *reinterpret_cast<T*>(Address) = value;
    }
    
    static T read() {
        return *reinterpret_cast<T*>(Address);
    }
};

// Hardware Abstraction Layer
struct LED {
    static constexpr uintptr_t GPIO_BASE = 0x40020000;
    static constexpr uintptr_t ODR_OFFSET = 0x14;
    
    static void on() {
        Reg<GPIO_BASE + ODR_OFFSET>::write(1 << 5);
    }
    
    static void off() {
        Reg<GPIO_BASE + ODR_OFFSET>::write(0);
    }
};

// Compile-time configuration check
static_assert(sizeof(uint32_t) == 4, "Must be 32-bit system");
```

### 12.3 High-Frequency Trading (HFT)
HFT focuses on **low latency** (nanoseconds matter).

**Key Techniques:**
1.  **Kernel Bypass**: Use DPDK or Solarflare OpenOnload to skip OS networking stack.
2.  **Lock-Free Structures**: Single-Producer Single-Consumer (SPSC) ring buffers.
3.  **Warm-up**: Pre-run code to ensure CPU cache is hot and branch predictors are trained.
4.  **No Dynamic Allocation**: `std::vector` is forbidden in the hot path. Use `std::array` or pre-allocated pools.

```cpp
// SPSC Ring Buffer Concept (Simplified)
template<typename T, size_t Size>
class RingBuffer {
    std::array<T, Size> buffer;
    alignas(64) std::atomic<size_t> head{0}; // Cache-line padding
    alignas(64) std::atomic<size_t> tail{0};
    
public:
    bool push(const T& val) {
        size_t current_tail = tail.load(std::memory_order_relaxed);
        size_t next_tail = (current_tail + 1) % Size;
        
        if (next_tail == head.load(std::memory_order_acquire)) return false; // Full
        
        buffer[current_tail] = val;
        tail.store(next_tail, std::memory_order_release);
        return true;
    }
    
    bool pop(T& val) {
        size_t current_head = head.load(std::memory_order_relaxed);
        if (current_head == tail.load(std::memory_order_acquire)) return false; // Empty
        
        val = buffer[current_head];
        head.store((current_head + 1) % Size, std::memory_order_release);
        return true;
    }
};
```

### 12.4 Automotive & Aerospace (MISRA C++)
Safety-critical systems (ISO 26262) have strict rules.

1.  **No Dynamic Allocation**: `new`/`malloc` are banned to prevent fragmentation.
2.  **No Exceptions**: Code must have predictable control flow.
3.  **Static Analysis**: Heavy reliance on tools like Coverity.

```cpp
// Stack-only pattern
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

In the realm of lock-free programming, the **ABA Problem** is a silent killer. Hazard pointers and Epoch-Based Reclamation are the standard solutions.

## <a name="chapter-39-templatemetaprogrammingpatterns"></a>CHAPTER 39: TEMPLATE METAPROGRAMMING PATTERNS

Template Metaprogramming (TMP) is about moving computation from runtime to compile-time. We cover Expression Templates and the void_t pattern.

## <a name="chapter-40-highperformancedatastructures"></a>CHAPTER 40: HIGH-PERFORMANCE DATA STRUCTURES

High-performance data structures focus on cache locality and lock-free concurrency. Key topics: Disruptor Pattern and Treiber Stack.

## <a name="chapter-41-realtimeaudiosignalprocessing"></a>CHAPTER 41: REAL-TIME AUDIO & SIGNAL PROCESSING

Real-time audio demands deterministic latency. The "Audio Callback" is a strictly no-block zone. We cover SIMD in DSP.

## <a name="chapter-42-roboticsros2development"></a>CHAPTER 42: ROBOTICS & ROS2 DEVELOPMENT

Robotics combines high-level logic with hard real-time constraints. We explore ROS2, C++20, and Zero-Copy IPC with Iceoryx.

## <a name="chapter-43-machinelearninginfrastructure"></a>CHAPTER 43: MACHINE LEARNING INFRASTRUCTURE

Machine Learning in C++ is about efficiency. We cover Tensor memory layout, strides, and interfacing with BLAS/MKL libraries.

## <a name="chapter-44-databaseinternalslsmtrees"></a>CHAPTER 44: DATABASE INTERNALS (LSM TREES)

Database Internals focus on LSM Trees, MemTables, WAL, and SSTables. We explain how RocksDB-style engines achieve extreme write throughput.

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

### 1. Object Slicing
Passing a derived object by value to a function expecting a base class strips off the derived part.
*   **Fix**: Pass by reference (`Base&`) or pointer (`Base*`).

### 2. Iterator Invalidation
Modifying a container (e.g., `vector::push_back`) can invalidate existing iterators if reallocation occurs.
*   **Fix**: Do not cache iterators across mutating operations; check container documentation.

### 3. Dangling References
Returning a reference to a local variable.
*   **Fix**: Return by value or ensure the referenced object outlives the reference (e.g., static/heap).

### 4. Static Initialization Order Fiasco
Global objects in different translation units have no defined initialization order.
*   **Fix**: Use the "Construct On First Use" idiom (static variable inside a function).

### 5. Most Vexing Parse
`MyClass obj();` is a function declaration, not an object instantiation.
*   **Fix**: Use brace initialization `MyClass obj{};`.

### 6. Undefined Behavior (UB)
Signed integer overflow, dereferencing null, accessing out of bounds.
*   **Fix**: Use Sanitizers (ASan, UBSan) and perform bounds checking (`at()` vs `[]`).

### 7. Resource Leaks
Manual `new`/`delete` usage often leads to leaks.
*   **Fix**: Always use RAII (`std::unique_ptr`, `std::vector`, etc.).

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





