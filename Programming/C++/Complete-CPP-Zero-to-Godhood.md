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
7. [Constructors & Destructors](#constructors--destructors)
8. [Inheritance](#inheritance)
9. [Virtual Functions & Polymorphism](#virtual-functions--polymorphism)

### PART 3: C++98/03 STANDARD LIBRARY
10. [Standard Template Library (STL)](#standard-template-library)
11. [Containers (vector, list, map, set)](#containers)
12. [Algorithms](#algorithms)
13. [Strings](#strings)
14. [File I/O](#file-io)

### PART 4: C++11 REVOLUTION
15. [Auto & Type Deduction (C++11)](#auto--type-deduction-c11)
16. [Smart Pointers (C++11)](#smart-pointers-c11)
17. [Move Semantics (C++11)](#move-semantics-c11)
18. [Lambda Functions (C++11)](#lambda-functions-c11)
19. [Variadic Templates (C++11)](#variadic-templates-c11)
20. [Rvalue References (C++11)](#rvalue-references-c11)

### PART 5: C++14 ENHANCEMENTS
21. [Generic Lambdas (C++14)](#generic-lambdas-c14)
22. [Return Type Deduction (C++14)](#return-type-deduction-c14)
23. [std::make_unique (C++14)](#stdmake-unique-c14)
24. [Digit Separators (C++14)](#digit-separators-c14)

### PART 6: C++17 MODERN FEATURES
25. [Structured Bindings (C++17)](#structured-bindings-c17)
26. [Optional, Variant, Any (C++17)](#optional-variant-any-c17)
27. [If Constexpr (C++17)](#if-constexpr-c17)
28. [Fold Expressions (C++17)](#fold-expressions-c17)
29. [Filesystem Library (C++17)](#filesystem-library-c17)
30. [Parallel Algorithms (C++17)](#parallel-algorithms-c17)

### PART 7: C++20 REVOLUTIONARY FEATURES
31. [Concepts & Constraints (C++20)](#concepts--constraints-c20)
32. [Ranges (C++20)](#ranges-c20)
33. [Modules (C++20)](#modules-c20)
34. [Coroutines (C++20)](#coroutines-c20)
35. [Spaceship Operator (C++20)](#spaceship-operator-c20)
36. [Designated Initializers (C++20)](#designated-initializers-c20)

### PART 8: C++23 LATEST FEATURES
37. [Deducing this (C++23)](#deducing-this-c23)
38. [Literal Classes in constexpr (C++23)](#literal-classes-in-constexpr-c23)
39. [std::expected (C++23)](#stdexpected-c23)
40. [Pattern Matching (Exploring C++23)](#pattern-matching-exploring-c23)

### PART 9: ADVANCED TOPICS
41. [Template Metaprogramming](#template-metaprogramming)
42. [Memory Management Deep Dive](#memory-management-deep-dive)
43. [Concurrency & Threading](#concurrency--threading)
44. [Performance Optimization](#performance-optimization)
45. [Design Patterns](#design-patterns)

### PART 10: PRODUCTION & PROFESSIONAL
46. [Testing & Debugging](#testing--debugging)
47. [Build Systems](#build-systems)
48. [Profiling & Optimization](#profiling--optimization)
49. [Best Practices](#best-practices)

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

## Auto & Type Deduction (C++11)

### Auto Keyword (C++11)

```cpp
#include <iostream>
#include <vector>
#include <map>

int main() {
    // Before C++11: verbose
    // std::vector<int>::iterator it = v.begin();
    
    // C++11: auto keyword
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto it = v.begin();  // Deduced as std::vector<int>::iterator
    
    // Auto with different types
    auto x = 5;           // int
    auto y = 3.14;        // double
    auto z = "hello";     // const char*
    
    // Trailing return type
    auto add = [](int a, int b) { return a + b; };
    
    std::cout << add(5, 3) << "\n";  // 8
    
    return 0;
}
```

---

## Smart Pointers (C++11)

### unique_ptr (C++11)

```cpp
#include <iostream>
#include <memory>

class Resource {
public:
    Resource() { std::cout << "Resource created\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
    void use() { std::cout << "Using resource\n"; }
};

int main() {
    // Before C++11: Manual deletion
    // Resource* r = new Resource();
    // r->use();
    // delete r;  // Easy to forget!
    
    // C++11: unique_ptr (exclusive ownership)
    {
        std::unique_ptr<Resource> ptr(new Resource());
        ptr->use();
    }  // Automatically deleted
    
    // Better: std::make_unique
    {
        auto ptr = std::make_unique<Resource>();
        ptr->use();
    }  // Automatically deleted
    
    return 0;
}
```

### shared_ptr (C++11)

```cpp
#include <iostream>
#include <memory>

class Resource {
public:
    Resource() { std::cout << "Resource created\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
};

int main() {
    // C++11: shared_ptr (reference counting)
    {
        auto ptr1 = std::make_shared<Resource>();
        std::cout << "Use count: " << ptr1.use_count() << "\n";  // 1
        
        auto ptr2 = ptr1;  // Share ownership
        std::cout << "Use count: " << ptr1.use_count() << "\n";  // 2
        
        ptr2 = nullptr;
        std::cout << "Use count: " << ptr1.use_count() << "\n";  // 1
    }  // ptr1 destroyed, resource freed
    
    return 0;
}
```

---

## Move Semantics (C++11)

### Rvalue References (C++11)

```cpp
#include <iostream>
#include <vector>

class Vector {
public:
    int* data;
    size_t size;
    
    // Copy constructor (slow)
    Vector(const Vector& other) : size(other.size) {
        data = new int[size];
        std::copy(other.data, other.data + size, data);
        std::cout << "Copy constructor\n";
    }
    
    // Move constructor (fast) - C++11
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        other.data = nullptr;
        other.size = 0;
        std::cout << "Move constructor\n";
    }
    
    ~Vector() {
        delete[] data;
    }
};

int main() {
    Vector v1;
    v1.data = new int[10];
    v1.size = 10;
    
    Vector v2 = v1;                    // Copy
    Vector v3 = std::move(v1);         // Move
    
    // After move, v1 is in "valid but unspecified state"
    
    return 0;
}
```

---

## Lambda Functions (C++11)

### Basic Lambda (C++11)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>

int main() {
    // C++11: Lambda functions
    auto add = [](int a, int b) { return a + b; };
    
    std::cout << add(5, 3) << "\n";  // 8
    
    // Lambda with capture
    int multiplier = 5;
    auto multiply = [multiplier](int x) { return x * multiplier; };
    
    std::cout << multiply(3) << "\n";  // 15
    
    // Use with algorithms
    std::vector<int> nums = {1, 2, 3, 4, 5};
    std::transform(nums.begin(), nums.end(), nums.begin(),
                   [](int x) { return x * x; });
    
    for (int n : nums) {
        std::cout << n << " ";  // 1 4 9 16 25
    }
    
    return 0;
}
```

---

## Variadic Templates (C++11)

### Parameter Packs (C++11)

```cpp
#include <iostream>

// C++11: Variadic templates
template<typename T>
T sum(T val) {
    return val;
}

template<typename T, typename... Args>
T sum(T first, Args... rest) {
    return first + sum(rest...);
}

int main() {
    std::cout << sum(1, 2, 3, 4, 5) << "\n";          // 15
    std::cout << sum(1.5, 2.5, 3.0) << "\n";          // 7.0
    
    return 0;
}
```

---

## Rvalue References (C++11)

### Perfect Forwarding (C++11)

```cpp
#include <iostream>
#include <utility>

void process(int& x) {
    std::cout << "Lvalue reference\n";
}

void process(int&& x) {
    std::cout << "Rvalue reference\n";
}

// C++11: Perfect forwarding with std::forward
template<typename T>
void wrapper(T&& arg) {
    process(std::forward<T>(arg));
}

int main() {
    int x = 5;
    wrapper(x);         // Lvalue reference
    wrapper(10);        // Rvalue reference
    
    return 0;
}
```

---

## PART 5: C++14 ENHANCEMENTS

## Generic Lambdas (C++14)

```cpp
#include <iostream>

int main() {
    // C++14: Generic lambda with auto parameters
    auto multiply = [](auto a, auto b) { return a * b; };
    
    std::cout << multiply(5, 3) << "\n";         // 15 (int)
    std::cout << multiply(2.5, 4.0) << "\n";    // 10.0 (double)
    
    return 0;
}
```

## Return Type Deduction (C++14)

```cpp
#include <iostream>

// C++14: Return type deduction
auto add(int a, int b) {
    return a + b;
}

auto divide(double a, double b) {
    return a / b;
}

int main() {
    std::cout << add(5, 3) << "\n";           // 8
    std::cout << divide(10.0, 2.0) << "\n";  // 5.0
    
    return 0;
}
```

## std::make_unique (C++14)

```cpp
#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass(int x) { std::cout << "Constructed with " << x << "\n"; }
};

int main() {
    // C++14: std::make_unique (exception-safe)
    auto ptr = std::make_unique<MyClass>(42);
    
    return 0;
}
```

---

## PART 6: C++17 MODERN FEATURES

## Structured Bindings (C++17)

```cpp
#include <iostream>
#include <tuple>
#include <map>

int main() {
    // C++17: Structured bindings
    std::tuple<int, double, std::string> t = {42, 3.14, "hello"};
    auto [x, y, z] = t;
    
    std::cout << x << ", " << y << ", " << z << "\n";
    
    // With maps
    std::map<std::string, int> ages = {{"Alice", 30}, {"Bob", 25}};
    
    for (auto [name, age] : ages) {
        std::cout << name << ": " << age << "\n";
    }
    
    return 0;
}
```

## Optional, Variant, Any (C++17)

```cpp
#include <iostream>
#include <optional>
#include <variant>
#include <any>

// C++17: std::optional (replaces returning nullptr)
std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

// C++17: std::variant (type-safe union)
std::variant<int, double, std::string> get_value(int type) {
    if (type == 0) return 42;
    else if (type == 1) return 3.14;
    else return "hello";
}

int main() {
    // Optional
    if (auto result = divide(10, 2)) {
        std::cout << "Result: " << *result << "\n";
    }
    
    // Variant
    auto val = get_value(1);
    if (auto* d = std::get_if<double>(&val)) {
        std::cout << "Double: " << *d << "\n";
    }
    
    // std::any (type erasure)
    std::any x = 42;
    std::cout << std::any_cast<int>(x) << "\n";
    
    return 0;
}
```

## If Constexpr (C++17)

```cpp
#include <iostream>
#include <type_traits>

// C++17: if constexpr (compile-time decision)
template<typename T>
void print_type(T value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integer: " << value << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Float: " << value << "\n";
    } else {
        std::cout << "Other type\n";
    }
}

int main() {
    print_type(42);        // Integer
    print_type(3.14);      // Float
    print_type("hello");   // Other type
    
    return 0;
}
```

## Fold Expressions (C++17)

```cpp
#include <iostream>

// C++17: Fold expressions
template<typename... Args>
auto sum(Args... args) {
    return (... + args);  // Left fold
}

int main() {
    std::cout << sum(1, 2, 3, 4, 5) << "\n";     // 15
    
    return 0;
}
```

## Filesystem Library (C++17)

```cpp
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    // C++17: Filesystem library
    auto current_path = fs::current_path();
    std::cout << "Current path: " << current_path << "\n";
    
    // Create directory
    fs::create_directory("test_dir");
    
    // List files
    for (const auto& entry : fs::directory_iterator(current_path)) {
        std::cout << entry.path().filename() << "\n";
    }
    
    return 0;
}
```

## Parallel Algorithms (C++17)

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>

int main() {
    // C++17: Parallel algorithms
    std::vector<int> v(1000000);
    
    // Sequential sort
    std::sort(v.begin(), v.end());
    
    // Parallel sort (faster!)
    std::sort(std::execution::par, v.begin(), v.end());
    
    // Parallel and vectorized
    std::sort(std::execution::par_unseq, v.begin(), v.end());
    
    return 0;
}
```

---

## PART 7: C++20 REVOLUTIONARY FEATURES

## Concepts & Constraints (C++20)

```cpp
#include <iostream>
#include <concepts>

// C++20: Concepts
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic T>
T add(T a, T b) {
    return a + b;
}

template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
};

template<Comparable T>
T minimum(T a, T b) {
    return a < b ? a : b;
}

int main() {
    std::cout << add(5, 3) << "\n";           // 8 (OK)
    std::cout << add(2.5, 3.5) << "\n";       // 6.0 (OK)
    // std::cout << add("a", "b") << "\n";    // Error: std::string not Arithmetic
    
    std::cout << minimum(5, 3) << "\n";       // 3
    
    return 0;
}
```

## Ranges (C++20)

```cpp
#include <iostream>
#include <vector>
#include <ranges>

int main() {
    // C++20: Ranges (composable operations)
    std::vector<int> nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    auto result = nums
        | std::views::filter([](int n) { return n % 2 == 0; })
        | std::views::transform([](int n) { return n * n; });
    
    for (int n : result) {
        std::cout << n << " ";  // 4 16 36 64 100
    }
    
    return 0;
}
```

## Modules (C++20)

```cpp
// math.cpp
export module math;

export int add(int a, int b) {
    return a + b;
}

// main.cpp
import math;
#include <iostream>

int main() {
    std::cout << add(5, 3) << "\n";  // 8
    return 0;
}
```

## Coroutines (C++20)

```cpp
#include <iostream>
#include <coroutine>

// C++20: Coroutines (simplified example)
struct Generator {
    struct promise_type {
        int value;
        
        Generator get_return_object() {
            return Generator{std::coroutine_handle<promise_type>::from_promise(*this)};
        }
        
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() { return {}; }
        
        std::suspend_always yield_value(int v) {
            value = v;
            return {};
        }
        
        void return_void() {}
        void unhandled_exception() {}
    };
    
    std::coroutine_handle<promise_type> handle;
};

// Generator get_numbers() {
//     co_yield 1;
//     co_yield 2;
//     co_yield 3;
// }
```

## Spaceship Operator (C++20)

```cpp
#include <iostream>
#include <compare>

class Person {
public:
    std::string name;
    int age;
    
    // C++20: Spaceship operator <=> (three-way comparison)
    auto operator<=>(const Person&) const = default;
};

int main() {
    Person p1{"Alice", 30};
    Person p2{"Bob", 25};
    
    if ((p1 <=> p2) > 0) {
        std::cout << "p1 > p2\n";
    }
    
    return 0;
}
```

## Designated Initializers (C++20)

```cpp
#include <iostream>

struct Point {
    int x;
    int y;
    int z;
};

int main() {
    // C++20: Designated initializers
    Point p = {.x = 1, .y = 2, .z = 3};
    
    std::cout << p.x << ", " << p.y << ", " << p.z << "\n";
    
    return 0;
}
```

---

## PART 8: C++23 LATEST FEATURES

## Deducing this (C++23)

```cpp
#include <iostream>

class MyClass {
public:
    // C++23: Deducing this
    void func(this auto&& self) {
        std::cout << "Called\n";
    }
};

int main() {
    MyClass obj;
    obj.func();
    
    return 0;
}
```

## std::expected (C++23)

```cpp
#include <iostream>

// Similar to Rust's Result type
template<typename T, typename E>
class Expected {
    // Holds either value or error
};

// Modern error handling (C++23)
// std::expected<int, std::string> divide(int a, int b) {
//     if (b == 0) return std::unexpected("Division by zero");
//     return a / b;
// }
```

---

## PART 9: ADVANCED TOPICS

## Template Metaprogramming

### Type Traits & SFINAE

```cpp
#include <iostream>
#include <type_traits>

// Determine if type is integral
template<typename T>
void process(T value, std::enable_if_t<std::is_integral_v<T>>* = nullptr) {
    std::cout << "Integer: " << value << "\n";
}

// Overload for floating point
template<typename T>
void process(T value, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
    std::cout << "Float: " << value << "\n";
}

int main() {
    process(42);         // Integer version
    process(3.14);       // Float version
    
    return 0;
}
```

---

## Memory Management Deep Dive

### Stack vs Heap

```cpp
#include <iostream>

int main() {
    // Stack (fast, limited size, automatic cleanup)
    int stack_var = 5;
    int stack_array[1000];
    
    // Heap (slow, large, manual cleanup)
    int* heap_var = new int(5);
    int* heap_array = new int[1000];
    
    delete heap_var;
    delete[] heap_array;
    
    return 0;
}
```

---

## Concurrency & Threading

### Basic Threading

```cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

std::mutex mtx;
std::atomic<int> counter(0);

void worker() {
    for (int i = 0; i < 1000000; ++i) {
        counter++;
    }
}

int main() {
    std::thread t1(worker);
    std::thread t2(worker);
    
    t1.join();
    t2.join();
    
    std::cout << "Counter: " << counter << "\n";  // 2000000
    
    return 0;
}
```

---

## Performance Optimization

### Compiler Optimization Flags

```bash
# No optimization (debug)
g++ -O0 program.cpp -o program

# Level 1 optimization
g++ -O1 program.cpp -o program

# Level 2 (common)
g++ -O2 program.cpp -o program

# Level 3 (aggressive)
g++ -O3 program.cpp -o program

# Size optimization
g++ -Os program.cpp -o program

# Fast math (can break strict IEEE)
g++ -O3 -ffast-math program.cpp -o program
```

---

## Design Patterns

### Singleton Pattern

```cpp
#include <iostream>

class Singleton {
private:
    static Singleton* instance;
    Singleton() {}
public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
```

---

## PART 10: PRODUCTION & PROFESSIONAL

## Testing & Debugging

### Google Test Framework

```cpp
#include <gtest/gtest.h>

int add(int a, int b) { return a + b; }

TEST(MathTest, AddPositive) {
    EXPECT_EQ(add(2, 3), 5);
    ASSERT_EQ(add(0, 0), 0);
}

TEST(MathTest, AddNegative) {
    EXPECT_EQ(add(-2, -3), -5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
```

---

## Build Systems

### CMake Example

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)

add_executable(main src/main.cpp src/utils.cpp)

target_include_directories(main PRIVATE include)

# Link libraries
target_link_libraries(main PRIVATE pthread)
```

---

## Profiling & Optimization

### Performance Profiling

```bash
# Compile with debug symbols
g++ -g -O2 program.cpp -o program

# Profile with perf
perf record ./program
perf report

# Memory profiling with valgrind
valgrind --leak-check=full ./program

# Cache profiling
valgrind --tool=cachegrind ./program
```

---

## Best Practices

### Code Organization

```cpp
// header.h
#ifndef HEADER_H
#define HEADER_H

class MyClass {
public:
    void method();
private:
    int data;
};

#endif

// source.cpp
#include "header.h"

void MyClass::method() {
    // Implementation
}
```

### RAII Pattern

```cpp
#include <iostream>
#include <fstream>

class FileHandler {
private:
    std::ofstream file;
public:
    FileHandler(const std::string& filename) {
        file.open(filename);
    }
    
    ~FileHandler() {
        if (file.is_open()) {
            file.close();
        }
    }
    
    void write(const std::string& data) {
        file << data << "\n";
    }
};

int main() {
    {
        FileHandler f("output.txt");
        f.write("Hello, World!");
    }  // File automatically closed
    
    return 0;
}
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
