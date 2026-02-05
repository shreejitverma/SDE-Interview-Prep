# THE MODERN C++11 CORE: SYNTAX & TYPE SYSTEM

## 1. Type Inference & Safety

### 1.1 `auto`: Automatic Type Deduction
C++11 introduced `auto` to let the compiler deduce the type of a variable from its initializer.

```cpp
auto i = 42;        // int
auto d = 3.14;      // double
auto s = "hello";   // const char*
auto& ref = i;      // int&
const auto* ptr = &i; // const int*
```

**Crucial Rules:**
1.  **Reference Dropping:** `auto` drops top-level references and `const` by default (like template deduction).
    ```cpp
    int x = 0;
    int& y = x;
    auto z = y; // z is int, NOT int&
    ```
2.  **Keeping Qualifiers:** Use `auto&` or `const auto&` to preserve them.
    ```cpp
    const int cx = 10;
    auto copy = cx;       // int (const dropped)
    const auto& ref = cx; // const int&
    ```

### 1.2 `decltype`: Inspecting Types
Unlike `auto`, `decltype` gives the *exact* declared type of an expression, including references and const.

```cpp
int x = 0;
decltype(x) y = 5;      // int
decltype((x)) z = y;    // int& (because (x) is an lvalue expression)
```

**Use Case: Trailing Return Types**
Allows return types to depend on parameter types.

```cpp
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
```

### 1.3 `nullptr`: The Null Pointer Literal
Replaces `NULL` and `0`. Type-safe and unambiguous.

```cpp
void f(int);
void f(char*);

f(0);       // Calls f(int) -> Ambiguity resolved!
f(NULL);    // Implementation-defined (often f(int))
f(nullptr); // Calls f(char*)
```

---

## 2. Initialization Uniformity

### 2.1 Uniform Initialization (Brace Initialization)
Consistent syntax for initializing everything.

```cpp
int x{5};               // Direct initialization
int y = {5};            // Copy list initialization
std::vector<int> v{1, 2, 3};
std::map<int, std::string> m{{1, "a"}, {2, "b"}};
```

**Narrowing Prevention:**
```cpp
int x = 3.14;  // Compiles (warning), x becomes 3
// int y{3.14}; // ERROR: Narrowing conversion not allowed
```

### 2.2 `std::initializer_list`
Allows objects to accept a list of elements (used in constructors of containers).

```cpp
#include <initializer_list>

class MyVector {
public:
    MyVector(std::initializer_list<int> list) {
        for (int x : list) {
            // ...
        }
    }
};
```

---

## 3. Control Flow & Iteration

### 3.1 Range-Based For Loops
Syntactic sugar for iterating over arrays and containers.

```cpp
std::vector<int> v = {1, 2, 3};

// By Value (Copy)
for (int x : v) { /* ... */ }

// By Reference (Modify)
for (auto& x : v) { x *= 2; }

// By Const Reference (Read-only, avoids copy)
for (const auto& x : v) { /* ... */ }
```

Works on anything with `begin()` and `end()` iterators (or arrays).

---

## 4. Class Features

### 4.1 Explicit Overrides (`override`, `final`)
Compiler-checked inheritance safety.

*   `override`: Ensures you are actually overriding a base virtual function.
*   `final`: Prevents further overriding or inheritance.

```cpp
class Base {
    virtual void foo(int);
};

class Derived : public Base {
    void foo(int) override;   // OK
    // void foo(float) override; // Error: signature mismatch
};

class Last final : public Base { // Cannot be inherited from
    void foo(int) final;         // Cannot be overridden
};
```

### 4.2 Defaulted and Deleted Functions
Control compiler-generated functions.

```cpp
class Widget {
public:
    Widget() = default; // Force generation of default constructor
    
    // Disable copying
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
};
```

### 4.3 Strongly Typed Enums (`enum class`)
Scoped, strongly typed, and safe.

```cpp
enum class Color : char { Red, Green, Blue }; // Underlying type char

Color c = Color::Red;
// int i = c; // Error: No implicit conversion
```

### 4.4 Delegating Constructors
One constructor calls another.

```cpp
class Box {
    int w, h;
public:
    Box(int width, int height) : w(width), h(height) {}
    Box() : Box(1, 1) {} // Delegate
};
```

---

## 5. `constexpr` (C++11 Version)
Compile-time constants and functions. In C++11, `constexpr` functions must contain a *single return statement*.

```cpp
constexpr int square(int x) {
    return x * x;
}

int array[square(5)]; // Valid: array size 25 determined at compile time
```

---

## 6. Static Assert
Compile-time assertions.

```cpp
static_assert(sizeof(void*) == 8, "64-bit system required");
```
