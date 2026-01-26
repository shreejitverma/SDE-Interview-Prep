# The Ultimate Advanced C++ Programmer's Guide: From Mastery to Godhood

## Table of Contents

### Part 1: Core Language Features
1. [Modern C++ Fundamentals](#modern-c-fundamentals)
2. [Memory Management](#memory-management)
3. [Smart Pointers](#smart-pointers)
4. [Move Semantics & Rvalue References](#move-semantics--rvalue-references)
5. [Template Metaprogramming](#template-metaprogramming)
6. [STL Containers & Algorithms](#stl-containers--algorithms)

### Part 2: Object-Oriented Programming
7. [Advanced OOP Concepts](#advanced-oop-concepts)
8. [Design Patterns in C++](#design-patterns-in-c)
9. [CRTP & Static Polymorphism](#crtp--static-polymorphism)
10. [Virtual Functions & Inheritance](#virtual-functions--inheritance)

### Part 3: Advanced Features
11. [Functional Programming in C++](#functional-programming-in-c)
12. [Lambda Functions & Closures](#lambda-functions--closures)
13. [Variadic Templates](#variadic-templates)
14. [Concepts & Constraints](#concepts--constraints)
15. [Concurrency & Threading](#concurrency--threading)

### Part 4: Performance & Optimization
16. [Memory Layout & Cache Optimization](#memory-layout--cache-optimization)
17. [SIMD & Vectorization](#simd--vectorization)
18. [Lock-Free Programming](#lock-free-programming)
19. [Profiling & Optimization](#profiling--optimization)

### Part 5: Advanced Data Structures
20. [STL Deep Dive](#stl-deep-dive)
21. [Custom Data Structures](#custom-data-structures)
22. [Algorithms & Complexity](#algorithms--complexity)

### Part 6: System Programming
23. [File I/O & System Calls](#file-io--system-calls)
24. [Network Programming](#network-programming)
25. [Low-Level Programming](#low-level-programming)

### Part 7: Professional Development
26. [Metaprogramming & Reflection](#metaprogramming--reflection)
27. [Testing & Debugging](#testing--debugging)
28. [Build Systems & Package Management](#build-systems--package-management)
29. [Production Best Practices](#production-best-practices)

---

## PART 1: CORE LANGUAGE FEATURES

## Modern C++ Fundamentals

### C++17, C++20, C++23 Features

```cpp
// Structured Bindings (C++17)
#include <tuple>
#include <map>

std::map<std::string, int> data = {{"alice", 30}, {"bob", 25}};

for (auto& [name, age] : data) {
    std::cout << name << ": " << age << "\n";
}

// Nested structured bindings
struct Point { double x, y; };
struct Line { Point a, b; };

Line line{{1.0, 2.0}, {3.0, 4.0}};
auto [a, b] = line;
auto [ax, ay] = a;

// if constexpr (C++17)
template<typename T>
void print(const T& value) {
    if constexpr (std::is_same_v<T, int>) {
        std::cout << "Integer: " << value << "\n";
    } else if constexpr (std::is_same_v<T, std::string>) {
        std::cout << "String: " << value << "\n";
    }
}

// Fold Expressions (C++17)
template<typename... Args>
int sum(Args... args) {
    return (args + ...);  // Fold left: args[0] + args[1] + ... + args[n]
}

sum(1, 2, 3, 4, 5);  // 15

// Optional (C++17)
#include <optional>

std::optional<int> maybe_divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

if (auto result = maybe_divide(10, 2)) {
    std::cout << "Result: " << *result << "\n";
} else {
    std::cout << "Division by zero\n";
}

// std::variant (C++17)
#include <variant>

std::variant<int, std::string, double> value;

value = 42;
if (auto* i = std::get_if<int>(&value)) {
    std::cout << "Int: " << *i << "\n";
}

value = std::string("hello");
std::visit([](auto&& v) {
    std::cout << "Visited: " << v << "\n";
}, value);

// Ranges (C++20)
#include <ranges>

std::vector<int> nums = {1, 2, 3, 4, 5};

// Filter and transform
auto result = nums
    | std::views::filter([](int n) { return n % 2 == 0; })
    | std::views::transform([](int n) { return n * n; });

for (int n : result) {
    std::cout << n << " ";  // 4 16
}

// Concepts (C++20)
template<typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template<Arithmetic T>
T add(T a, T b) {
    return a + b;
}

// Spaceship Operator (C++20)
#include <compare>

struct Person {
    std::string name;
    int age;
    
    auto operator<=>(const Person&) const = default;
};

Person p1{"alice", 30};
Person p2{"bob", 25};

if ((p1 <=> p2) > 0) {
    std::cout << "p1 > p2\n";
}
```

### Type Deduction & Auto

```cpp
// auto keyword (deduction)
auto x = 5;              // int
auto y = 3.14;           // double
auto z = "hello";        // const char*

// std::decay to remove references/const
template<typename T>
void process(T&& value) {
    using Decayed = std::decay_t<T>;
}

// decltype (C++11)
int x = 5;
decltype(x) y = 10;      // y is int

auto func = []() { return 3.14; };
decltype(func()) result; // double

// trailing return type
template<typename T1, typename T2>
auto add(T1 a, T2 b) -> decltype(a + b) {
    return a + b;
}
```

---

## Memory Management

### Stack vs Heap

```cpp
#include <iostream>
#include <memory>

// Stack allocation (automatic, small, fast)
void stack_example() {
    int stack_array[1000];      // 4 KB on stack
    struct LocalStruct {
        double data[100];
    } local;                    // Automatic cleanup
}  // All memory automatically freed

// Heap allocation (manual/manual, large, slower)
void heap_example() {
    int* heap_array = new int[1000000];  // 4 MB on heap
    
    // Must manually free
    delete[] heap_array;
    
    // Or use smart pointers (C++11+)
    auto smart_array = std::make_unique<int[]>(1000000);
}  // Auto freed when smart_array goes out of scope

// RAII Pattern (Resource Acquisition Is Initialization)
class File {
    FILE* handle;
public:
    File(const char* name) {
        handle = fopen(name, "r");
        if (!handle) throw std::runtime_error("File not found");
    }
    
    ~File() {
        if (handle) fclose(handle);
    }
    
    // Prevent copying
    File(const File&) = delete;
    File& operator=(const File&) = delete;
};

void process_file() {
    File f("data.txt");
    // File automatically closed when f goes out of scope
}
```

### Memory Leak Detection

```cpp
#include <valgrind/valgrind.h>

void leak_example() {
    int* leaked = new int[100];
    // Forgot to delete - memory leak!
}

// Compile with: g++ -g -O0 program.cpp -o program
// Run with: valgrind --leak-check=full ./program
// Output: Reports all memory leaks with line numbers

// Using sanitizers (clang/gcc)
// Compile: g++ -fsanitize=address -g program.cpp -o program
// Run: ./program
// Output: Prints memory leaks and buffer overflows
```

---

## Smart Pointers

### unique_ptr: Exclusive Ownership

```cpp
#include <memory>
#include <iostream>

class Resource {
public:
    Resource() { std::cout << "Resource created\n"; }
    ~Resource() { std::cout << "Resource destroyed\n"; }
    void use() { std::cout << "Using resource\n"; }
};

void unique_ptr_example() {
    // Create unique_ptr
    std::unique_ptr<Resource> ptr1(new Resource());
    
    // Or use make_unique (preferred, exception-safe)
    auto ptr2 = std::make_unique<Resource>();
    
    ptr2->use();
    
    // Move ownership
    std::unique_ptr<Resource> ptr3 = std::move(ptr2);
    // ptr2 is now nullptr
    
    if (ptr2 == nullptr) {
        std::cout << "ptr2 is null\n";
    }
    
    // Array variant
    auto array = std::make_unique<int[]>(100);
    array[0] = 42;
}
// Output:
// Resource created
// Resource created
// Using resource
// ptr2 is null
// Resource destroyed
// Resource destroyed

// unique_ptr with custom deleter
class CustomDeleter {
public:
    void operator()(Resource* ptr) const {
        std::cout << "Custom delete\n";
        delete ptr;
    }
};

std::unique_ptr<Resource, CustomDeleter> ptr(new Resource());
// Calls CustomDeleter when destroyed
```

### shared_ptr: Shared Ownership

```cpp
#include <memory>

void shared_ptr_example() {
    {
        auto ptr1 = std::make_shared<Resource>();
        std::cout << "Reference count: " << ptr1.use_count() << "\n";  // 1
        
        auto ptr2 = ptr1;
        std::cout << "Reference count: " << ptr1.use_count() << "\n";  // 2
        
        auto ptr3 = ptr1;
        std::cout << "Reference count: " << ptr1.use_count() << "\n";  // 3
        
        ptr2 = nullptr;
        std::cout << "Reference count: " << ptr1.use_count() << "\n";  // 2
    }
    // Resource destroyed when all shared_ptrs go out of scope
}

// Detecting cycles (use weak_ptr)
class Node {
    std::shared_ptr<Node> next;
    std::weak_ptr<Node> prev;  // Break cycle with weak_ptr
public:
    void set_next(std::shared_ptr<Node> n) {
        next = n;
        if (n) n->prev = std::weak_ptr<Node>(shared_from_this());
    }
};
```

### weak_ptr: Non-Owning References

```cpp
#include <memory>

void weak_ptr_example() {
    auto shared = std::make_shared<Resource>();
    std::weak_ptr<Resource> weak = shared;
    
    std::cout << "Use count: " << shared.use_count() << "\n";  // 1 (not 2)
    
    // weak_ptr doesn't own, doesn't increment counter
    
    if (auto locked = weak.lock()) {
        // Resource still alive
        locked->use();
    } else {
        std::cout << "Resource destroyed\n";
    }
}

// Observer pattern without cycles
class Observable {
    std::vector<std::weak_ptr<Observer>> observers;
public:
    void notify() {
        for (auto& obs : observers) {
            if (auto o = obs.lock()) {
                o->update();
            }
        }
    }
};
```

---

## Move Semantics & Rvalue References

### Rvalue References

```cpp
#include <iostream>
#include <utility>

class Vector {
    int* data;
    size_t size;
public:
    Vector(size_t n) : data(new int[n]), size(n) {
        std::cout << "Constructor\n";
    }
    
    // Copy constructor
    Vector(const Vector& other) : data(new int[other.size]), size(other.size) {
        std::cout << "Copy constructor\n";
        std::copy(other.data, other.data + size, data);
    }
    
    // Move constructor (takes rvalue reference)
    Vector(Vector&& other) noexcept : data(other.data), size(other.size) {
        std::cout << "Move constructor\n";
        other.data = nullptr;
        other.size = 0;
    }
    
    ~Vector() {
        delete[] data;
    }
};

void move_example() {
    Vector v1(100);        // Constructor
    Vector v2 = v1;        // Copy constructor
    Vector v3 = std::move(v1);  // Move constructor
    
    // After move, v1 is in "valid but unspecified state"
}

// Rvalue references extend lifetime
const int& ref = 5;  // Error: can't bind rvalue to lvalue reference
const int& ref = std::move(5);  // Error: still can't bind to const lvalue ref

int&& rref = 5;      // OK: rvalue reference
rref = 10;           // Can modify

// std::forward (perfect forwarding)
template<typename T>
void wrapper(T&& arg) {
    // std::forward preserves whether arg is lvalue or rvalue
    process(std::forward<T>(arg));
}
```

### Move-Only Types

```cpp
class MoveOnly {
public:
    MoveOnly() = default;
    
    // Delete copy operations
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;
    
    // Allow move operations
    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

void move_only_example() {
    std::vector<std::unique_ptr<int>> vec;
    
    vec.push_back(std::make_unique<int>(42));  // Move OK
    auto moved = std::move(vec[0]);             // Move OK
    
    // Can't copy unique_ptrs
    // auto copied = vec[0];  // Error!
}
```

---

## Template Metaprogramming

### Compile-Time Computation

```cpp
#include <type_traits>

// Factorial at compile time
template<int N>
struct Factorial {
    static constexpr int value = N * Factorial<N-1>::value;
};

template<>
struct Factorial<0> {
    static constexpr int value = 1;
};

constexpr int fact5 = Factorial<5>::value;  // Computed at compile time

// Fibonacci at compile time
constexpr int fibonacci(int n) {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

constexpr int fib10 = fibonacci(10);  // 55

// Type lists
template<typename... Types>
struct TypeList {};

template<typename List>
struct Length;

template<typename... Types>
struct Length<TypeList<Types...>> {
    static constexpr size_t value = sizeof...(Types);
};

using MyTypes = TypeList<int, double, std::string>;
constexpr size_t len = Length<MyTypes>::value;  // 3

// SFINAE (Substitution Failure Is Not An Error)
#include <iostream>

// Version 1: for integers
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void print(T value) {
    std::cout << "Integer: " << value << "\n";
}

// Version 2: for floating point
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
void print(T value) {
    std::cout << "Float: " << value << "\n";
}

// If constexpr
template<typename T>
void generic_print(const T& value) {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integer: " << value << "\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Float: " << value << "\n";
    } else {
        std::cout << "Other type\n";
    }
}
```

### Template Specialization

```cpp
#include <vector>

// Primary template
template<typename T>
class Container {
public:
    void info() { std::cout << "Generic container\n"; }
};

// Specialization for bool (std::vector<bool> is special-cased)
template<>
class Container<bool> {
public:
    void info() { std::cout << "Specialized for bool\n"; }
};

// Partial specialization for pointers
template<typename T>
class Container<T*> {
public:
    void info() { std::cout << "Specialized for pointers\n"; }
};

// Partial specialization for vectors
template<typename T>
class Container<std::vector<T>> {
public:
    void info() { std::cout << "Specialized for vectors\n"; }
};

void specialization_example() {
    Container<int> c1;
    c1.info();  // Generic container
    
    Container<bool> c2;
    c2.info();  // Specialized for bool
    
    Container<int*> c3;
    c3.info();  // Specialized for pointers
    
    Container<std::vector<double>> c4;
    c4.info();  // Specialized for vectors
}
```

---

## STL Containers & Algorithms

### Container Characteristics

```cpp
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

void container_characteristics() {
    // vector: dynamic array
    // - O(1) random access
    // - O(1) push_back amortized
    // - O(n) insert/erase in middle
    std::vector<int> v;
    v.push_back(1);
    v.reserve(1000);  // Pre-allocate
    
    // list: doubly-linked list
    // - O(n) random access
    // - O(1) insert/erase (if iterator known)
    std::list<int> l;
    auto it = l.insert(l.end(), 5);  // Insert before end
    
    // deque: double-ended queue
    // - O(1) random access
    // - O(1) push_front, push_back
    // - O(n) insert/erase in middle
    std::deque<int> d;
    d.push_front(0);
    d.push_back(1);
    
    // set: sorted, unique keys
    // - O(log n) insert/find/erase
    // - Red-black tree
    std::set<int> s;
    s.insert(5);
    s.insert(3);
    s.insert(7);
    
    // unordered_set: hash table
    // - O(1) average insert/find/erase
    // - O(n) worst case
    std::unordered_set<int> us;
    us.insert(5);
    
    // map: key-value pairs, sorted
    // - O(log n) operations
    std::map<std::string, int> m;
    m["alice"] = 30;
    
    // unordered_map: hash-based key-value
    // - O(1) average operations
    std::unordered_map<std::string, int> um;
    um["alice"] = 30;
}
```

### Algorithms

```cpp
#include <algorithm>
#include <numeric>

void algorithm_examples() {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    // Sorting
    std::sort(v.begin(), v.end());
    std::sort(v.begin(), v.end(), std::greater<int>());
    std::stable_sort(v.begin(), v.end());
    
    // Searching
    auto it = std::find(v.begin(), v.end(), 5);
    auto it2 = std::lower_bound(v.begin(), v.end(), 5);
    auto it3 = std::binary_search(v.begin(), v.end(), 5);
    
    // Modifying
    std::fill(v.begin(), v.end(), 0);
    std::transform(v.begin(), v.end(), v.begin(), 
                   [](int x) { return x * 2; });
    std::reverse(v.begin(), v.end());
    std::rotate(v.begin(), v.begin() + 3, v.end());
    
    // Removing
    auto new_end = std::remove(v.begin(), v.end(), 5);
    v.erase(new_end, v.end());  // Erase removed elements
    
    // Numeric algorithms
    int sum = std::accumulate(v.begin(), v.end(), 0);
    int product = std::accumulate(v.begin(), v.end(), 1,
                                   std::multiplies<int>());
    
    // Counting
    int count = std::count(v.begin(), v.end(), 5);
    int count_if = std::count_if(v.begin(), v.end(),
                                  [](int x) { return x % 2 == 0; });
    
    // All/Any/None
    bool all_positive = std::all_of(v.begin(), v.end(),
                                     [](int x) { return x > 0; });
    bool any_negative = std::any_of(v.begin(), v.end(),
                                     [](int x) { return x < 0; });
    bool none_zero = std::none_of(v.begin(), v.end(),
                                   [](int x) { return x == 0; });
}
```

---

## PART 2: OBJECT-ORIENTED PROGRAMMING

## Advanced OOP Concepts

### Virtual Functions & Polymorphism

```cpp
#include <iostream>
#include <memory>

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
    virtual double area() const = 0;
    
    // Non-virtual interface (NVI) idiom
    void display() const {
        std::cout << "Drawing: ";
        draw();
        std::cout << "Area: " << area() << "\n";
    }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double r) : radius(r) {}
    
    void draw() const override {
        std::cout << "Circle\n";
    }
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double w, double h) : width(w), height(h) {}
    
    void draw() const override {
        std::cout << "Rectangle\n";
    }
    
    double area() const override {
        return width * height;
    }
};

void polymorphism_example() {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<Circle>(5.0));
    shapes.push_back(std::make_unique<Rectangle>(4.0, 6.0));
    
    for (auto& shape : shapes) {
        shape->display();
    }
}

// RTTI (Runtime Type Information)
void rtti_example() {
    std::unique_ptr<Shape> shape = std::make_unique<Circle>(5.0);
    
    // typeid
    std::cout << "Type: " << typeid(*shape).name() << "\n";
    
    // dynamic_cast
    if (auto circle = dynamic_cast<Circle*>(shape.get())) {
        std::cout << "It's a circle\n";
    }
}
```

### Multiple Inheritance & Ambiguity

```cpp
class A {
public:
    virtual void func() { std::cout << "A::func\n"; }
};

class B {
public:
    virtual void func() { std::cout << "B::func\n"; }
};

// Diamond problem
class C : public A, public B {
public:
    void func() override {
        std::cout << "C::func\n";
    }
};

// Virtual inheritance (solve diamond problem)
class D : virtual public A, virtual public B {
};

void multiple_inheritance_example() {
    C c;
    c.func();  // Calls C::func
    
    // Specify which base
    static_cast<A*>(&c)->func();  // A::func
    static_cast<B*>(&c)->func();  // B::func
}
```

---

## Design Patterns in C++

### Singleton Pattern

```cpp
class Singleton {
private:
    static Singleton* instance;
    Singleton() = default;
public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
    
    // Thread-safe version (C++11)
    static Singleton& getInstanceThreadSafe() {
        static Singleton instance;
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;

// Usage
auto& singleton = Singleton::getInstanceThreadSafe();
```

### Observer Pattern

```cpp
#include <vector>
#include <memory>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(const std::string& event) = 0;
};

class Subject {
private:
    std::vector<std::weak_ptr<Observer>> observers;
public:
    void attach(std::shared_ptr<Observer> obs) {
        observers.push_back(obs);
    }
    
    void notify(const std::string& event) {
        for (auto& obs : observers) {
            if (auto o = obs.lock()) {
                o->update(event);
            }
        }
    }
};

class ConcreteObserver : public Observer {
    std::string name;
public:
    ConcreteObserver(const std::string& n) : name(n) {}
    
    void update(const std::string& event) override {
        std::cout << name << " received: " << event << "\n";
    }
};
```

### Strategy Pattern

```cpp
class Algorithm {
public:
    virtual ~Algorithm() = default;
    virtual int execute(int a, int b) = 0;
};

class AddStrategy : public Algorithm {
public:
    int execute(int a, int b) override { return a + b; }
};

class MultiplyStrategy : public Algorithm {
public:
    int execute(int a, int b) override { return a * b; }
};

class Context {
private:
    std::unique_ptr<Algorithm> strategy;
public:
    void setStrategy(std::unique_ptr<Algorithm> s) {
        strategy = std::move(s);
    }
    
    int executeStrategy(int a, int b) {
        return strategy->execute(a, b);
    }
};
```

---

## CRTP & Static Polymorphism

### Curiously Recurring Template Pattern

```cpp
#include <iostream>

// Base class template
template<typename Derived>
class Base {
public:
    void interface() {
        static_cast<Derived*>(this)->implementation();
    }
};

// Derived class
class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived implementation\n";
    }
};

void crtp_example() {
    Derived d;
    d.interface();  // Calls Derived::implementation without virtual overhead
}

// Static polymorphism benefits
template<typename T>
class Calculator : public Base<T> {
public:
    void compute() {
        static_cast<T*>(this)->interface();
    }
};

class FastCalculator : public Calculator<FastCalculator> {
public:
    void implementation() {
        std::cout << "Fast computation\n";
    }
};

// No virtual function overhead, compile-time dispatch
```

---

## PART 3: ADVANCED FEATURES

## Functional Programming in C++

### Function Objects & std::function

```cpp
#include <functional>
#include <vector>
#include <algorithm>

// Function object (functor)
class Multiplier {
    int factor;
public:
    Multiplier(int f) : factor(f) {}
    
    int operator()(int x) const {
        return x * factor;
    }
};

// std::function (type-erased function)
void function_example() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    std::vector<int> result;
    
    // Function pointer
    std::function<int(int)> func1 = [](int x) { return x * 2; };
    
    // Functor
    std::function<int(int)> func2 = Multiplier(3);
    
    // std::bind
    std::function<int(int)> func3 = std::bind(std::multiplies<int>(), 5, std::placeholders::_1);
    
    // Apply to vector
    std::transform(v.begin(), v.end(), std::back_inserter(result), func1);
}

// Callback pattern
class Event {
private:
    std::vector<std::function<void(const std::string&)>> callbacks;
public:
    void subscribe(std::function<void(const std::string&)> cb) {
        callbacks.push_back(cb);
    }
    
    void fire(const std::string& message) {
        for (auto& cb : callbacks) {
            cb(message);
        }
    }
};
```

### Lambda Functions & Closures

```cpp
#include <iostream>

void lambda_example() {
    int multiplier = 5;
    
    // Capture by value
    auto lambda_by_value = [multiplier](int x) {
        return x * multiplier;
    };
    
    // Capture by reference
    auto lambda_by_ref = [&multiplier](int x) {
        multiplier = 10;  // Modifies original
        return x * multiplier;
    };
    
    // Capture all by value
    auto lambda_all_value = [=](int x) {
        return x * multiplier;
    };
    
    // Capture all by reference
    auto lambda_all_ref = [&](int x) {
        return x * multiplier;
    };
    
    // Generic lambda (C++14)
    auto generic = [](auto x) {
        return x * 2;
    };
    
    std::cout << generic(5) << "\n";        // 10
    std::cout << generic(3.14) << "\n";     // 6.28
    
    // Lambda with auto return type (C++14)
    auto compute = [](int a, int b) {
        return a + b;
    };
}

// Mutable lambda
void mutable_lambda_example() {
    int counter = 0;
    
    auto increment = [counter]() mutable {
        counter++;
        return counter;
    };
    
    std::cout << increment() << "\n";  // 1
    std::cout << increment() << "\n";  // 2 (captures by value, so copy incremented)
}

// Lambda as function pointer
using FuncPtr = int(*)(int);

FuncPtr get_func() {
    // Lambda without captures can decay to function pointer
    return [](int x) { return x * 2; };
}
```

---

## Variadic Templates

### Parameter Packs

```cpp
#include <iostream>
#include <tuple>

// Sum all arguments
template<typename T>
T sum(T val) {
    return val;
}

template<typename T, typename... Args>
T sum(T first, Args... rest) {
    return first + sum(rest...);
}

void variadic_example() {
    std::cout << sum(1, 2, 3, 4, 5) << "\n";        // 15
    std::cout << sum(1.5, 2.5, 3.0) << "\n";        // 7.0
}

// Fold expressions (C++17)
template<typename... Args>
auto sum_fold(Args... args) {
    return (... + args);  // Left fold
}

// Index sequences
template<typename Tuple, size_t... I>
auto tuple_to_vector(Tuple& t, std::index_sequence<I...>) {
    return std::vector{std::get<I>(t)...};
}

template<typename Tuple>
auto tuple_to_vector(Tuple& t) {
    return tuple_to_vector(t, std::make_index_sequence<std::tuple_size_v<Tuple>>());
}

// Variadic function wrapper
template<typename Func, typename... Args>
auto call_and_log(Func f, Args... args) {
    std::cout << "Calling function with " << sizeof...(args) << " arguments\n";
    return f(args...);
}
```

---

## Concepts & Constraints

### C++20 Concepts

```cpp
#include <concepts>

// Concept definition
template<typename T>
concept Comparable = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
};

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

// Using concepts
template<Comparable T>
T minimum(T a, T b) {
    return a < b ? a : b;
}

template<Addable T>
T add_three_times(T a, T b, T c) {
    return a + b + c;
}

// Concept with associated types
template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    typename T::iterator;
    { c.size() } -> std::convertible_to<size_t>;
    { c.begin() } -> std::convertible_to<typename T::iterator>;
};

template<Container C>
void process_container(C& c) {
    for (auto& item : c) {
        // Process item
    }
}
```

---

## Concurrency & Threading

### Thread Basics

```cpp
#include <thread>
#include <iostream>
#include <chrono>

void worker_thread(int id) {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Worker " << id << ": " << i << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void threading_example() {
    std::thread t1(worker_thread, 1);
    std::thread t2(worker_thread, 2);
    
    t1.join();  // Wait for thread to complete
    t2.join();
}

// Thread with lambda
void thread_lambda_example() {
    std::thread t([](int id) {
        std::cout << "Thread " << id << "\n";
    }, 1);
    
    t.join();
}
```

### Synchronization: Mutex & Lock

```cpp
#include <mutex>
#include <thread>
#include <vector>

class Counter {
private:
    int value = 0;
    mutable std::mutex mtx;
public:
    void increment() {
        std::lock_guard<std::mutex> lock(mtx);
        value++;
    }
    
    int get() const {
        std::lock_guard<std::mutex> lock(mtx);
        return value;
    }
};

void mutex_example() {
    Counter counter;
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.push_back(std::thread([&counter]() {
            for (int j = 0; j < 1000; ++j) {
                counter.increment();
            }
        }));
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    std::cout << "Counter: " << counter.get() << "\n";  // 10000
}

// Unique lock for more control
void unique_lock_example() {
    std::mutex mtx;
    int shared = 0;
    
    std::unique_lock<std::mutex> lock(mtx);
    shared = 42;
    
    lock.unlock();  // Early release
    
    // Can do non-critical work here
    
    lock.lock();    // Re-acquire
    std::cout << shared << "\n";
}
```

### Condition Variables

```cpp
#include <condition_variable>
#include <mutex>
#include <thread>
#include <queue>

class ThreadSafeQueue {
private:
    std::queue<int> queue;
    mutable std::mutex mtx;
    std::condition_variable cv;
public:
    void push(int value) {
        {
            std::lock_guard<std::mutex> lock(mtx);
            queue.push(value);
        }
        cv.notify_one();  // Wake one waiting thread
    }
    
    int pop() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty(); });
        
        int value = queue.front();
        queue.pop();
        return value;
    }
};

void producer_consumer_example() {
    ThreadSafeQueue q;
    
    // Producer
    std::thread producer([&q]() {
        for (int i = 0; i < 10; ++i) {
            q.push(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    });
    
    // Consumer
    std::thread consumer([&q]() {
        for (int i = 0; i < 10; ++i) {
            std::cout << "Consumed: " << q.pop() << "\n";
        }
    });
    
    producer.join();
    consumer.join();
}
```

### Atomic Operations

```cpp
#include <atomic>
#include <thread>

void atomic_example() {
    std::atomic<int> counter(0);
    
    auto increment = [&counter]() {
        for (int i = 0; i < 1000000; ++i) {
            counter++;  // Atomic increment
        }
    };
    
    std::thread t1(increment);
    std::thread t2(increment);
    
    t1.join();
    t2.join();
    
    std::cout << "Counter: " << counter << "\n";  // 2000000 (guaranteed correct)
}

// Memory ordering
void memory_ordering_example() {
    std::atomic<int> x(0), y(0);
    std::atomic<bool> ready(false);
    
    std::thread t1([&]() {
        x.store(1, std::memory_order_relaxed);
        y.store(1, std::memory_order_release);
        ready.store(true, std::memory_order_release);
    });
    
    std::thread t2([&]() {
        while (!ready.load(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
        // x and y are guaranteed to be visible
    });
    
    t1.join();
    t2.join();
}
```

---

## PART 4: PERFORMANCE & OPTIMIZATION

## Memory Layout & Cache Optimization

### Cache-Friendly Data Structures

```cpp
#include <vector>
#include <chrono>

// Bad: Cache misses
struct Node {
    int value;
    Node* next;
};

// Good: Cache-friendly
struct CacheArray {
    std::vector<int> values;
};

void cache_example() {
    // Linked list (poor cache locality)
    Node* head = new Node();
    Node* current = head;
    for (int i = 1; i < 10000; ++i) {
        current->next = new Node();
        current = current->next;
        current->value = i;
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    int sum = 0;
    current = head;
    for (int i = 0; i < 10000; ++i) {
        sum += current->value;
        current = current->next;
    }
    auto linked_time = std::chrono::high_resolution_clock::now() - start;
    
    // Array (excellent cache locality)
    std::vector<int> arr(10000);
    for (int i = 0; i < 10000; ++i) {
        arr[i] = i;
    }
    
    start = std::chrono::high_resolution_clock::now();
    sum = 0;
    for (int i = 0; i < 10000; ++i) {
        sum += arr[i];
    }
    auto array_time = std::chrono::high_resolution_clock::now() - start;
    
    // Array is typically 10-100x faster due to cache locality
}

// Structure of Arrays (SoA) vs Array of Structures (AoS)
struct AoS {  // Array of Structures
    struct Particle {
        float x, y, z;      // Position
        float vx, vy, vz;   // Velocity
        float mass;         // Mass
    };
    std::vector<Particle> particles;
};

struct SoA {  // Structure of Arrays
    std::vector<float> x, y, z;
    std::vector<float> vx, vy, vz;
    std::vector<float> mass;
};

// SoA is more cache-friendly for SIMD and bulk operations
```

### Memory Alignment

```cpp
#include <iostream>

struct Unaligned {
    char c;      // 1 byte
    int i;       // 4 bytes
    double d;    // 8 bytes
};

struct Aligned {
    double d;    // 8 bytes
    int i;       // 4 bytes
    char c;      // 1 byte
};

void alignment_example() {
    std::cout << "Unaligned size: " << sizeof(Unaligned) << "\n";  // 16 (padding)
    std::cout << "Aligned size: " << sizeof(Aligned) << "\n";      // 16
    
    // Explicit alignment
    struct alignas(16) CacheAligned {
        int data[4];
    };
    
    static_assert(sizeof(CacheAligned) % 16 == 0);
}
```

---

## SIMD & Vectorization

### Auto Vectorization

```cpp
#include <vector>
#include <algorithm>

// This can be auto-vectorized by modern compilers
void add_vectors(const std::vector<float>& a,
                 const std::vector<float>& b,
                 std::vector<float>& c) {
    // Compiler generates SIMD instructions
    // for (size_t i = 0; i < a.size(); ++i) {
    //     c[i] = a[i] + b[i];
    // }
    std::transform(a.begin(), a.end(), b.begin(), c.begin(),
                   std::plus<float>());
}

// Explicit SIMD with intrinsics (SSE, AVX)
#include <immintrin.h>

void simd_add(const float* a, const float* b, float* c, int n) {
    for (int i = 0; i < n; i += 8) {
        // Load 8 floats from each array
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        
        // Add 8 floats in parallel
        __m256 vc = _mm256_add_ps(va, vb);
        
        // Store result
        _mm256_storeu_ps(&c[i], vc);
    }
}

// Compiler hints
void vectorize_hint(std::vector<float>& data) {
    #pragma omp simd  // OpenMP SIMD pragma
    for (size_t i = 1; i < data.size(); ++i) {
        data[i] = data[i-1] + data[i];
    }
}
```

---

## Lock-Free Programming

### Atomic Compare-and-Swap

```cpp
#include <atomic>

class LockFreeStack {
private:
    struct Node {
        int value;
        Node* next;
        Node(int v) : value(v), next(nullptr) {}
    };
    
    std::atomic<Node*> top{nullptr};
public:
    void push(int value) {
        Node* new_node = new Node(value);
        Node* old_top;
        
        do {
            old_top = top.load(std::memory_order_relaxed);
            new_node->next = old_top;
        } while (!top.compare_exchange_weak(old_top, new_node,
                                            std::memory_order_release,
                                            std::memory_order_relaxed));
    }
    
    bool pop(int& result) {
        Node* old_top;
        Node* new_top;
        
        do {
            old_top = top.load(std::memory_order_acquire);
            if (!old_top) return false;
            new_top = old_top->next;
        } while (!top.compare_exchange_weak(old_top, new_top,
                                            std::memory_order_release,
                                            std::memory_order_acquire));
        
        result = old_top->value;
        delete old_top;
        return true;
    }
};
```

---

## Profiling & Optimization

### Performance Profiling

```cpp
#include <chrono>
#include <iostream>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed_ms() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(end - start).count();
    }
    
    ~Timer() {
        std::cout << "Elapsed: " << elapsed_ms() << " ms\n";
    }
};

void profiling_example() {
    {
        Timer t;
        // Code to profile
        int sum = 0;
        for (int i = 0; i < 1000000; ++i) {
            sum += i;
        }
    }  // Destructor prints elapsed time
}

// Using perf (Linux)
// g++ -O0 -g program.cpp -o program
// perf record ./program
// perf report

// Using valgrind (cachegrind)
// valgrind --tool=cachegrind ./program
// cg_annotate cachegrind.out.* program.cpp

// Using Intel VTune
// vtune -collect performance ./program
```

### Optimization Techniques

```cpp
// 1. Inline functions
inline int add(int a, int b) {
    return a + b;
}

// 2. Compile-time constants
constexpr int BUFFER_SIZE = 1024;

template<int N>
class Buffer {
    int data[N];  // Size known at compile time
};

// 3. Loop unrolling
void unrolled_sum(const int* arr, int n, int& result) {
    result = 0;
    
    // Process 4 elements at a time
    for (int i = 0; i + 3 < n; i += 4) {
        result += arr[i] + arr[i+1] + arr[i+2] + arr[i+3];
    }
    
    // Handle remainder
    for (int i = (n/4)*4; i < n; ++i) {
        result += arr[i];
    }
}

// 4. Avoid exceptions in hot loops
void no_exceptions_hot_path(const std::vector<int>& v) {
    if (v.empty()) return;  // Check precondition
    
    for (int i = 0; i < v.size(); ++i) {
        // No exception-throwing operations
        process(v[i]);
    }
}

// 5. Use appropriate compiler flags
// g++ -O3 -march=native -ffast-math program.cpp -o program
```

---

## PART 5: ADVANCED DATA STRUCTURES

## STL Deep Dive

### Container Internals

```cpp
#include <vector>
#include <deque>
#include <memory>

// Vector internals
template<typename T>
class SimpleVector {
private:
    T* data;
    size_t capacity;
    size_t size;
public:
    SimpleVector() : data(nullptr), capacity(0), size(0) {}
    
    void push_back(const T& value) {
        if (size >= capacity) {
            // Exponential growth (typically 1.5x or 2x)
            size_t new_capacity = capacity == 0 ? 1 : capacity * 2;
            T* new_data = new T[new_capacity];
            
            if (data) {
                std::copy(data, data + size, new_data);
                delete[] data;
            }
            
            data = new_data;
            capacity = new_capacity;
        }
        
        data[size++] = value;
    }
    
    T& operator[](size_t i) { return data[i]; }
    size_t get_size() const { return size; }
};

// Deque internals (block-based)
template<typename T, size_t BLOCK_SIZE = 512>
class SimpleDeque {
private:
    std::vector<T*> blocks;
    size_t first_block = 0, first_offset = 0;
    size_t size = 0;
public:
    void push_front(const T& value) {
        if (first_offset == 0 && size > 0) {
            // Need new block at front
            blocks.insert(blocks.begin(), new T[BLOCK_SIZE]);
            first_block++;
            first_offset = BLOCK_SIZE - 1;
        } else if (size == 0) {
            blocks.push_back(new T[BLOCK_SIZE]);
            first_offset = 0;
        } else {
            first_offset--;
        }
        
        blocks[first_block][first_offset] = value;
        size++;
    }
};
```

---

## Custom Data Structures

### Balanced Binary Search Tree

```cpp
#include <memory>

template<typename T>
class AVLTree {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> left, right;
        int height = 1;
        
        int get_balance() const {
            int left_h = left ? left->height : 0;
            int right_h = right ? right->height : 0;
            return left_h - right_h;
        }
        
        void update_height() {
            int left_h = left ? left->height : 0;
            int right_h = right ? right->height : 0;
            height = 1 + std::max(left_h, right_h);
        }
    };
    
    std::unique_ptr<Node> root;
    
    std::unique_ptr<Node> rotate_right(std::unique_ptr<Node> y) {
        auto x = std::move(y->left);
        y->left = std::move(x->right);
        x->right = std::move(y);
        
        x->right->update_height();
        x->update_height();
        
        return x;
    }
    
    std::unique_ptr<Node> rotate_left(std::unique_ptr<Node> x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        
        y->left->update_height();
        y->update_height();
        
        return y;
    }
    
    std::unique_ptr<Node> insert_impl(std::unique_ptr<Node> node, const T& value) {
        if (!node) {
            node = std::make_unique<Node>();
            node->value = value;
            return node;
        }
        
        if (value < node->value) {
            node->left = insert_impl(std::move(node->left), value);
        } else {
            node->right = insert_impl(std::move(node->right), value);
        }
        
        node->update_height();
        
        int balance = node->get_balance();
        
        // Left-left
        if (balance > 1 && value < node->left->value) {
            return rotate_right(std::move(node));
        }
        
        // Right-right
        if (balance < -1 && value >= node->right->value) {
            return rotate_left(std::move(node));
        }
        
        // Left-right
        if (balance > 1 && value >= node->left->value) {
            node->left = rotate_left(std::move(node->left));
            return rotate_right(std::move(node));
        }
        
        // Right-left
        if (balance < -1 && value < node->right->value) {
            node->right = rotate_right(std::move(node->right));
            return rotate_left(std::move(node));
        }
        
        return node;
    }
public:
    void insert(const T& value) {
        root = insert_impl(std::move(root), value);
    }
};
```

---

## Algorithms & Complexity

### Sorting Algorithms

```cpp
#include <vector>
#include <algorithm>

template<typename T>
class SortingAlgorithms {
public:
    // Bubble Sort O(n²)
    static void bubble_sort(std::vector<T>& arr) {
        for (size_t i = 0; i < arr.size(); ++i) {
            for (size_t j = 0; j < arr.size() - i - 1; ++j) {
                if (arr[j] > arr[j+1]) {
                    std::swap(arr[j], arr[j+1]);
                }
            }
        }
    }
    
    // Quick Sort O(n log n) average
    static void quick_sort(std::vector<T>& arr, int low, int high) {
        if (low < high) {
            int pi = partition(arr, low, high);
            quick_sort(arr, low, pi - 1);
            quick_sort(arr, pi + 1, high);
        }
    }
    
    // Merge Sort O(n log n)
    static void merge_sort(std::vector<T>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            merge_sort(arr, left, mid);
            merge_sort(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }
    
private:
    static int partition(std::vector<T>& arr, int low, int high) {
        T pivot = arr[high];
        int i = low - 1;
        
        for (int j = low; j < high; ++j) {
            if (arr[j] < pivot) {
                ++i;
                std::swap(arr[i], arr[j]);
            }
        }
        
        std::swap(arr[i+1], arr[high]);
        return i + 1;
    }
    
    static void merge(std::vector<T>& arr, int left, int mid, int right) {
        std::vector<T> temp;
        int i = left, j = mid + 1;
        
        while (i <= mid && j <= right) {
            if (arr[i] <= arr[j]) {
                temp.push_back(arr[i++]);
            } else {
                temp.push_back(arr[j++]);
            }
        }
        
        while (i <= mid) temp.push_back(arr[i++]);
        while (j <= right) temp.push_back(arr[j++]);
        
        for (int i = 0; i < temp.size(); ++i) {
            arr[left + i] = temp[i];
        }
    }
};
```

---

## PART 6: SYSTEM PROGRAMMING

## File I/O & System Calls

### File Operations

```cpp
#include <fstream>
#include <iostream>
#include <string>

void file_operations() {
    // Text file writing
    {
        std::ofstream out("output.txt");
        if (!out) {
            std::cerr << "Error opening file\n";
            return;
        }
        
        out << "Line 1\n";
        out << "Line 2\n";
    }  // File auto-closed
    
    // Text file reading
    {
        std::ifstream in("output.txt");
        std::string line;
        
        while (std::getline(in, line)) {
            std::cout << line << "\n";
        }
    }
    
    // Binary file
    {
        std::ofstream out("binary.bin", std::ios::binary);
        int data[] = {1, 2, 3, 4, 5};
        out.write(reinterpret_cast<const char*>(data), sizeof(data));
    }
    
    {
        std::ifstream in("binary.bin", std::ios::binary);
        int data[5];
        in.read(reinterpret_cast<char*>(data), sizeof(data));
    }
}

// Low-level system calls
#include <unistd.h>
#include <fcntl.h>

void low_level_io() {
    // Open file
    int fd = open("file.txt", O_WRONLY | O_CREAT, 0644);
    
    // Write
    const char* data = "Hello, World!";
    write(fd, data, 13);
    
    // Close
    close(fd);
    
    // Read
    fd = open("file.txt", O_RDONLY);
    char buffer[100];
    size_t bytes = read(fd, buffer, 100);
    close(fd);
}
```

---

## Network Programming

### TCP Socket

```cpp
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <unistd.h>

// Server
void tcp_server() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    
    int client_fd = accept(server_fd, nullptr, nullptr);
    
    char buffer[1024] = {0};
    read(client_fd, buffer, 1024);
    std::cout << "Received: " << buffer << "\n";
    
    const char* response = "Hello, Client!";
    send(client_fd, response, strlen(response), 0);
    
    close(client_fd);
    close(server_fd);
}

// Client
void tcp_client() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
    
    connect(sock, (struct sockaddr*)&address, sizeof(address));
    
    const char* message = "Hello, Server!";
    send(sock, message, strlen(message), 0);
    
    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    std::cout << "Received: " << buffer << "\n";
    
    close(sock);
}
```

---

## Low-Level Programming

### Bit Operations

```cpp
#include <iostream>
#include <bitset>

void bit_operations() {
    // Bit manipulation
    unsigned int x = 5;      // 0101
    unsigned int y = 3;      // 0011
    
    std::cout << (x & y) << "\n";  // AND: 0001 = 1
    std::cout << (x | y) << "\n";  // OR: 0111 = 7
    std::cout << (x ^ y) << "\n";  // XOR: 0110 = 6
    std::cout << (~x) << "\n";     // NOT
    
    std::cout << (x << 1) << "\n"; // Left shift: 1010 = 10
    std::cout << (x >> 1) << "\n"; // Right shift: 0010 = 2
    
    // Bit manipulation tricks
    bool is_power_of_2(int n) {
        return n > 0 && (n & (n - 1)) == 0;
    }
    
    int count_set_bits(int n) {
        int count = 0;
        while (n) {
            count += n & 1;
            n >>= 1;
        }
        return count;
    }
    
    // Using std::bitset
    std::bitset<32> bits(5);
    std::cout << bits << "\n";
    std::cout << bits.count() << "\n";  // Number of set bits
}
```

---

## PART 7: PROFESSIONAL DEVELOPMENT

## Metaprogramming & Reflection

### Type Traits

```cpp
#include <type_traits>
#include <iostream>

template<typename T>
void print_type_info() {
    if constexpr (std::is_integral_v<T>) {
        std::cout << "Integral type\n";
    } else if constexpr (std::is_floating_point_v<T>) {
        std::cout << "Floating point type\n";
    } else {
        std::cout << "Other type\n";
    }
    
    if constexpr (std::is_const_v<T>) {
        std::cout << "Const\n";
    }
    
    if constexpr (std::is_pointer_v<T>) {
        std::cout << "Pointer\n";
    }
    
    std::cout << "Is trivially copyable: " << std::is_trivially_copyable_v<T> << "\n";
}

// Enable/Disable based on type
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void process(T value) {
    std::cout << "Processing integer: " << value << "\n";
}

// Specialization for floating point
template<typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
void process(T value) {
    std::cout << "Processing float: " << value << "\n";
}
```

---

## Testing & Debugging

### Unit Testing with Google Test

```cpp
#include <gtest/gtest.h>

class Calculator {
public:
    int add(int a, int b) { return a + b; }
    int subtract(int a, int b) { return a - b; }
};

class CalculatorTest : public ::testing::Test {
protected:
    Calculator calc;
};

TEST_F(CalculatorTest, Add) {
    EXPECT_EQ(calc.add(2, 3), 5);
    ASSERT_EQ(calc.add(0, 0), 0);
}

TEST_F(CalculatorTest, Subtract) {
    EXPECT_EQ(calc.subtract(5, 3), 2);
}

TEST(CalculatorTest, Parametrized) {
    Calculator calc;
    EXPECT_EQ(calc.add(1, 1), 2);
    EXPECT_EQ(calc.add(5, -1), 4);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

// Compile: g++ -std=c++17 test.cpp -lgtest -lgtest_main -o test
// Run: ./test
```

### Debugging

```cpp
#include <iostream>
#include <cassert>
#include <stdexcept>

void debugging_techniques() {
    // Assertions (removed in release build)
    int x = 5;
    assert(x > 0);
    
    // Exceptions
    if (x < 0) {
        throw std::invalid_argument("x must be positive");
    }
    
    // Logging
    std::cerr << "Debug: x = " << x << "\n";
}

// Using gdb
// g++ -g -O0 program.cpp -o program
// gdb ./program
// (gdb) break main
// (gdb) run
// (gdb) step
// (gdb) next
// (gdb) print x
// (gdb) continue
```

---

## Build Systems & Package Management

### CMake

```cmake
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(main main.cpp src/utils.cpp src/worker.cpp)

# Library
add_library(mylib STATIC src/mylib.cpp)
target_include_directories(mylib PUBLIC include)

# Link
target_link_libraries(main mylib)

# Find external packages
find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(main Boost::system)

# Compiler flags
if(MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install
install(TARGETS main DESTINATION bin)
```

### Package Management: Conan

```
[requires]
boost/1.80.0
zlib/1.2.13
openssl/3.0.0

[generators]
CMakeDeps
CMakeToolchain

[options]
boost:shared=True
openssl:shared=True
```

---

## Production Best Practices

### Error Handling

```cpp
#include <stdexcept>
#include <iostream>

class ApplicationError : public std::runtime_error {
public:
    ApplicationError(const std::string& msg) : std::runtime_error(msg) {}
};

class DatabaseError : public ApplicationError {
public:
    DatabaseError(const std::string& msg) : ApplicationError("Database: " + msg) {}
};

void robust_code() {
    try {
        // Your code
        throw DatabaseError("Connection failed");
    } catch (const DatabaseError& e) {
        std::cerr << "Database error: " << e.what() << "\n";
    } catch (const ApplicationError& e) {
        std::cerr << "Application error: " << e.what() << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
    }
}

// RAII for cleanup
class Logger {
    FILE* file;
public:
    Logger(const char* path) {
        file = fopen(path, "w");
        if (!file) throw std::runtime_error("Cannot open log file");
    }
    
    ~Logger() {
        if (file) fclose(file);
    }
    
    void log(const std::string& message) {
        fprintf(file, "%s\n", message.c_str());
    }
};
```

### Optimization Checklist

```cpp
// 1. Use const correctly
const int* ptr;              // Pointer can change, data is const
int* const ptr;              // Pointer is const, data can change
const int* const ptr;        // Both const

// 2. Use references to avoid copying
void process(const std::vector<int>& data) {
    // data is const reference, no copy
}

// 3. Use move semantics
std::vector<int> create_vector() {
    std::vector<int> v;
    // ...
    return v;  // Move, not copy
}

// 4. Inline small functions
inline int square(int x) {
    return x * x;
}

// 5. Use constexpr when possible
constexpr int buffer_size = 1024;

// 6. Minimize allocations
void loop() {
    std::vector<int> buffer;
    buffer.reserve(1000);  // Pre-allocate
    
    for (int i = 0; i < 1000000; ++i) {
        buffer.clear();
        // Use buffer
    }
}
```

---

## FINAL CHECKLIST FOR C++ GODHOOD

### Fundamentals
- ☐ Modern C++ (C++17, C++20, C++23 features)
- ☐ Type deduction and auto
- ☐ Memory management (stack vs heap)
- ☐ Smart pointers (unique_ptr, shared_ptr, weak_ptr)
- ☐ Move semantics and rvalue references
- ☐ RAII pattern

### Object-Oriented Programming
- ☐ Virtual functions and polymorphism
- ☐ Multiple inheritance and diamond problem
- ☐ Abstract base classes
- ☐ Design patterns (Singleton, Observer, Strategy, Factory)
- ☐ CRTP (Curiously Recurring Template Pattern)

### Templates & Metaprogramming
- ☐ Function templates
- ☐ Class templates
- ☐ Template specialization
- ☐ Variadic templates
- ☐ Fold expressions
- ☐ SFINAE and enable_if
- ☐ Concepts (C++20)
- ☐ Type traits

### Advanced Features
- ☐ Functional programming (lambda, std::function)
- ☐ Closures and capture
- ☐ Generic lambdas (C++14)
- ☐ std::optional and std::variant
- ☐ Structured bindings
- ☐ if constexpr

### STL & Containers
- ☐ Container characteristics (vector, list, deque, set, map)
- ☐ Algorithms (sort, find, transform, etc.)
- ☐ Iterators
- ☐ Custom comparators
- ☐ Ranges (C++20)

### Concurrency
- ☐ Threads
- ☐ Mutex and locks
- ☐ Condition variables
- ☐ Atomic operations
- ☐ Lock-free programming
- ☐ Memory ordering

### Performance & Optimization
- ☐ Memory profiling
- ☐ Cache optimization
- ☐ Memory alignment
- ☐ SIMD and vectorization
- ☐ Lock-free data structures
- ☐ Compiler optimizations
- ☐ Profiling tools

### System Programming
- ☐ File I/O
- ☐ System calls
- ☐ Network programming (sockets)
- ☐ Bit operations
- ☐ Low-level programming

### Professional Development
- ☐ Error handling and exceptions
- ☐ Build systems (CMake)
- ☐ Package management (Conan)
- ☐ Testing (Google Test)
- ☐ Debugging (gdb, valgrind)
- ☐ Code organization
- ☐ Documentation

### Data Structures & Algorithms
- ☐ Standard data structures
- ☐ Custom implementations (linked lists, trees, graphs)
- ☐ Sorting algorithms
- ☐ Searching algorithms
- ☐ Graph algorithms
- ☐ Dynamic programming

---

## Key Insights for C++ Mastery

1. **RAII** = Resource safety guaranteed
2. **Smart pointers** = No manual memory management
3. **Move semantics** = Zero-copy optimization
4. **Templates** = Compile-time computation & type safety
5. **Const correctness** = Prevents bugs, enables optimizations
6. **STL algorithms** = Composable, efficient operations
7. **Concurrency** = Atomic operations, lock-free design
8. **Performance** = Measure, profile, optimize iteratively
9. **Design patterns** = Proven solutions to common problems
10. **Modern C++** = Safer, faster, more expressive

---

**You are now ready to become the best C++ programmer in the universe!** 🚀

---

## Additional Resources

### Must-Read Books
- "A Tour of C++" by Bjarne Stroustrup
- "Effective Modern C++" by Scott Meyers
- "C++ Concurrency in Action" by Anthony Williams
- "The C++ Programming Language" by Bjarne Stroustrup

### Online Resources
- cppreference.com (C++ standard library reference)
- isocpp.org (C++ standards committee)
- cpp-patterns.com (Design patterns in C++)
- github.com/microsoft/GSL (Guidelines Support Library)

### Practice & Tools
- LeetCode: https://leetcode.com/
- HackerRank: https://www.hackerrank.com/
- Codeforces: https://codeforces.com/
- Project Euler: https://projecteuler.net/

---

*Last Updated: December 2025*
*C++ Version: C++20 (with C++23 notes)*
