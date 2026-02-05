# C++11 REVOLUTION


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

## AUTO & TYPE DEDUCTION

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

## SMART POINTERS - MEMORY REVOLUTION

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

## MOVE SEMANTICS & RVALUE REFERENCES

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
// 1. If T is deduced from template parameter  Universal reference
// 2. Otherwise  Rvalue reference
```

---

## LAMBDA FUNCTIONS

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

## VARIADIC TEMPLATES

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

## RANGE-BASED FOR LOOPS

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

## UNIFORM INITIALIZATION

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

## NULLPTR & STRONGLY TYPED ENUMS

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

## TUPLE, ARRAY, & UNORDERED CONTAINERS

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

## DECLTYPE & TYPE TRAITS

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

## CONCURRENCY & THREADING

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

## REGULAR EXPRESSIONS

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

## NEW LIBRARY FEATURES

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
