# The Complete C++ Programmer's Guide: From Zero to Godhood (C++98 to C++23)

## Table of Contents

### PART 1: ABSOLUTE BASICS (C++98)
1. [Getting Started](#getting-started)
2. [Basic Types & Variables](#basic-types--variables)
3. [Operators & Control Flow](#operators--control-flow)
4. [Functions](#functions)
5. [Arrays & Pointers](#arrays--pointers)

### PART 2: OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS
6. [Classes & Objects](#classes--objects)
   1. [The Four Pillars of OOP](#the-four-pillars-of-oop)
   2. [Classes & Objects - Complete Mastery](#classes--objects---complete-mastery)
   3. [Encapsulation](#encapsulation)
   4. [Inheritance - All Types](#inheritance---all-types)
   5. [Polymorphism](#polymorphism)
   6. [Abstraction](#abstraction)
   7. [Advanced Class Features](#advanced-class-features)
   8. [Access Modifiers & Friend Classes](#access-modifiers--friend-classes)
   9. [Static Members](#static-members)
   10. [Const Correctness in OOP](#const-correctness-in-oop)
   11. [Operator Overloading](#operator-overloading)
   12. [SOLID Principles](#solid-principles)
7. [Constructors & Destructors](#constructors--destructors)
8. [Inheritance](#inheritance)
9.  [Virtual Functions & Polymorphism](#virtual-functions--polymorphism)

### PART 3: C++98/03 STANDARD LIBRARY
10. [Standard Template Library (STL)](#standard-template-library)
    1. [Introduction to STL](#introduction-to-stl)
    2. [STL Components Overview](#stl-components-overview)
    3. [Containers - Complete Reference](#containers---complete-reference)
    4. [Iterators - Deep Dive](#iterators---deep-dive)
    5. [Algorithms - Complete Reference](#algorithms---complete-reference)
    6. [Strings - Master Guide](#strings---master-guide)
    7. [File I/O - Complete Coverage](#file-io---complete-coverage)
    8. [Function Objects & Comparators](#function-objects--comparators)
    9. [STL Best Practices](#stl-best-practices)
    
11. [Containers (vector, list, map, set)](#containers)
12. [Algorithms](#algorithms)
13. [Strings](#strings)
14. [File I/O](#file-io)

### PART 4: C++11 REVOLUTION
1. [C++11 Overview & History](#c11-overview--history)
2. [Auto & Type Deduction](#auto--type-deduction)
3. [Smart Pointers - Memory Revolution](#smart-pointers---memory-revolution)
4. [Move Semantics & Rvalue References](#move-semantics--rvalue-references)
5. [Lambda Functions - Anonymous Powerhouses](#lambda-functions---anonymous-powerhouses)
6. [Variadic Templates](#variadic-templates)
7. [Range-Based For Loops](#range-based-for-loops)
8. [Uniform Initialization](#uniform-initialization)
9. [nullptr & Strongly Typed Enums](#nullptr--strongly-typed-enums)
10. [Tuple, Array, & unordered Containers](#tuple-array--unordered-containers)
11. [decltype & Type Traits](#decltype--type-traits)
12. [Concurrency & Threading](#concurrency--threading)
13. [Regular Expressions](#regular-expressions)
14. [New Library Features](#new-library-features)

### PART 5: C++14 ENHANCEMENTS
1. [C++14 Overview & Philosophy](#c14-overview--philosophy)
2. [Generic Lambdas](#generic-lambdas)
3. [Return Type Deduction](#return-type-deduction)
4. [Auto for Variables in Lambdas](#auto-for-variables-in-lambdas)
5. [Binary Literals & Digit Separators](#binary-literals--digit-separators)
6. [std::make_unique](#stdmake_unique)
7. [Relaxed constexpr Restrictions](#relaxed-constexpr-restrictions)
8. [Variable Templates](#variable-templates)
9. [Aggregate Member Initialization](#aggregate-member-initialization)
10. [Member Function ref/const-ref Qualifiers](#member-function-refconst-ref-qualifiers)
11. [std::integer_sequence](#stdinteger_sequence)
12. [Deprecated Features](#deprecated-features)
13. [Library Improvements](#library-improvements)
14. [C++14 Best Practices](#c14-best-practices)

### PART 6: C++17 MODERN FEATURES
1. [C++17 Overview & Significance](#c17-overview--significance)
2. [Structured Bindings](#structured-bindings)
3. [Optional & Variants](#optional--variants)
4. [std::any](#stdany)
5. [std::string_view](#stdstring_view)
6. [If constexpr](#if-constexpr)
7. [Fold Expressions](#fold-expressions)
8. [Class Template Argument Deduction (CTAD)](#class-template-argument-deduction)
9. [std::filesystem](#stdfilesystem)
10. [std::invoke](#stdinvoke)
11. [Parallel Algorithms](#parallel-algorithms)
12. [Structured Types & More](#structured-types--more)
13. [Core Language Features](#core-language-features)
14. [Library Improvements](#library-improvements)

### PART 7: C++20 REVOLUTIONARY FEATURES
1. [C++20 Overview & Revolutionary Scope](#c20-overview--revolutionary-scope)
2. [Concepts & Constraints](#concepts--constraints)
3. [Ranges Library](#ranges-library)
4. [Coroutines](#coroutines)
5. [Spaceship Operator (Three-Way Comparison)](#spaceship-operator)
6. [Modules](#modules)
7. [Designated Initializers](#designated-initializers)
8. [Calendar & Time Zones](#calendar--time-zones)
9. [std::format](#stdformat)
10. [Atomic Shared Pointers](#atomic-shared-pointers)
11. [Consteval & constinit](#consteval--constinit)
12. [Lambdas Enhancements](#lambdas-enhancements)
13. [Requires Expressions](#requires-expressions)
14. [Advanced Features & Library](#advanced-features--library)

### PART 8: C++23 LATEST FEATURES

1. [C++23 Overview & Direction](#c23-overview--direction)
2. [std::print & Formatted Output](#stdprint--formatted-output)
3. [Deducing This](#deducing-this)
4. [Range-Based For Loop Enhancements](#range-based-for-loop-enhancements)
5. [std::expected](#stdexpected)
6. [std::optional Improvements](#stdoptional-improvements)
7. [Multidimensional Arrays & Subscript](#multidimensional-arrays--subscript)
8. [std::stacktrace](#stdstacktrace)
9. [constexpr Enhancements](#constexpr-enhancements)
10. [Adaptor Improvements](#adaptor-improvements)
11. [Library Improvements](#library-improvements)
12. [Attributes & Deprecations](#attributes--deprecations)
13. [C++23 Best Practices](#c23-best-practices)


### PART 9: ADVANCED TOPICS
1. [Template Metaprogramming](#template-metaprogramming)
2. [SFINAE & Type Traits](#sfinae--type-traits)
3. [Expression Templates](#expression-templates)
4. [Policy-Based Design](#policy-based-design)
5. [Memory Management & Optimization](#memory-management--optimization)
6. [Concurrency & Parallelism](#concurrency--parallelism)
7. [Type Erasure Patterns](#type-erasure-patterns)
8. [CRTP (Curiously Recurring Template Pattern)](#crtp-curiously-recurring-template-pattern)
9. [Perfect Forwarding & Move Semantics](#perfect-forwarding--move-semantics)
10. [Compile-Time Programming](#compile-time-programming)
11. [Meta-Object Protocol](#meta-object-protocol)
12. [Advanced Container Techniques](#advanced-container-techniques)
13. [ABI & Binary Compatibility](#abi--binary-compatibility)
14. [Performance Profiling & Optimization](#performance-profiling--optimization)
15. [Domain-Specific Language Design](#domain-specific-language-design)

### PART 10: PRODUCTION & PROFESSIONAL
1. [Large-Scale Project Architecture](#large-scale-project-architecture)
2. [Code Organization & Project Structure](#code-organization--project-structure)
3. [Build Systems & Compilation](#build-systems--compilation)
4. [Testing Strategies](#testing-strategies)
5. [Debugging & Profiling](#debugging--profiling)
6. [Version Control & Collaboration](#version-control--collaboration)
7. [Documentation & Knowledge Transfer](#documentation--knowledge-transfer)
8. [Security & Safety](#security--safety)
9. [Performance Engineering](#performance-engineering)
10. [Error Handling & Recovery](#error-handling--recovery)
11. [Deployment & DevOps](#deployment--devops)
12. [Code Review & Quality](#code-review--quality)
13. [Technical Debt Management](#technical-debt-management)
14. [Legacy Code Modernization](#legacy-code-modernization)
15. [Leadership & Team Management](#leadership--team-management)

---

## PART 1: ABSOLUTE BASICS (C++98)

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

---

## PART 2: OBJECT-ORIENTED PROGRAMMING FUNDAMENTALS

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

---

## PART 3: C++98/03 STANDARD LIBRARY

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
┌─────────────────────────────────────────────┐
│          STL (Standard Template Library)    │
├─────────────────────────────────────────────┤
│                                             │
│  ┌──────────────┐  ┌──────────────┐       │
│  │ CONTAINERS   │  │  ITERATORS   │       │
│  ├──────────────┤  ├──────────────┤       │
│  │ • Sequence   │  │ • Input      │       │
│  │ • Associative│  │ • Output     │       │
│  │ • Adapters   │  │ • Forward    │       │
│  └──────────────┘  │ • Bidirectional
│                     │ • Random Access│     │
│  ┌──────────────┐  └──────────────┘       │
│  │ ALGORITHMS   │  ┌──────────────┐       │
│  ├──────────────┤  │FUNCTION OBJ. │       │
│  │ • Searching  │  ├──────────────┤       │
│  │ • Sorting    │  │ • Predicates │       │
│  │ • Modifying  │  │ • Comparators│       │
│  │ • Numeric    │  │ • Functors   │       │
│  └──────────────┘  └──────────────┘       │
└─────────────────────────────────────────────┘
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

### Iterator Adapters

```cpp
#include <iterator>

vector<int> v = {1, 2, 3};

// Output iterator
copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
// Output: 1 2 3

// Insert iterator
vector<int> v1 = {1, 2, 3};
vector<int> v2 = {4, 5};

copy(v1.begin(), v1.end(), back_inserter(v2));
// v2: {4, 5, 1, 2, 3}

// Front insert
list<int> lst;
copy(v1.begin(), v1.end(), front_inserter(lst));
// lst: {3, 2, 1}

// Insert
vector<int> result;
copy(v1.begin(), v1.end(), inserter(result, result.end()));
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

## PART 4: C++11 REVOLUTION

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

## PART 5: C++14 ENHANCEMENTS

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

## PART 6: C++17 MODERN FEATURES


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

# SECTION 13: C++17 BEST PRACTICES

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

## PART 7: C++20 REVOLUTIONARY FEATURES


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

## PART 8: C++23 LATEST FEATURES

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

## PART 9: ADVANCED TOPICS

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

## PART 10: PRODUCTION & PROFESSIONAL

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

## Version Timeline

| Version | Year | Major Features |
|---------|------|---|
| C++98 | 1998 | STL, Templates, OOP |
| C++03 | 2003 | Minor fixes |
| C++11 | 2011 | Auto, Smart Pointers, Lambdas, Move Semantics |
| C++14 | 2014 | Generic Lambdas, std::make_unique |
| C++17 | 2017 | Structured Bindings, std::optional, Ranges (partial), Filesystem |
| C++20 | 2020 | Concepts, Full Ranges, Modules, Coroutines, Spaceship Operator |
| C++23 | 2023 | Deducing this, std::expected, Pattern Matching (exploring) |

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
