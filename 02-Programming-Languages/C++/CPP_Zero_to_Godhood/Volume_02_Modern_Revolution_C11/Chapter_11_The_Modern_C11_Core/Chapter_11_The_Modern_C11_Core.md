# Chapter 11: The Modern C++11 Core

> *The "Modern Revolution" begins here. C++11 redefined the language — this chapter covers the everyday syntax and type-system features you will reach for in every modern translation unit.*

C++11 was the largest revision in the language's history. This chapter establishes the **core syntax and type-system** features: type inference, uniform initialization, range iteration, the class-authoring keywords, compile-time evaluation, and compile-time assertions. The heavier machinery — move semantics (Chapter 12), lambdas (Chapter 13), variadics (Chapter 14), the library expansion (Chapter 15), concurrency (Chapter 16), and the advanced literal/union/alignment features (Chapter 17) — builds on the foundation laid here.

---

## Table of Contents

- [11.1 Type Inference and Safety](#111-type-inference-and-safety)
  - [11.1.1 `auto`: Automatic Type Deduction](#1111-auto-automatic-type-deduction)
  - [11.1.2 `decltype`: Inspecting Types](#1112-decltype-inspecting-types)
  - [11.1.3 Trailing Return Types](#1113-trailing-return-types)
  - [11.1.4 `nullptr`: The Null Pointer Literal](#1114-nullptr-the-null-pointer-literal)
- [11.2 Initialization Uniformity](#112-initialization-uniformity)
  - [11.2.1 Uniform (Brace) Initialization](#1121-uniform-brace-initialization)
  - [11.2.2 Narrowing Prevention](#1122-narrowing-prevention)
  - [11.2.3 `std::initializer_list`](#1123-stdinitializer_list)
- [11.3 Control Flow and Iteration](#113-control-flow-and-iteration)
  - [11.3.1 Range-Based For Loops](#1131-range-based-for-loops)
- [11.4 Class Authoring Features](#114-class-authoring-features)
  - [11.4.1 Explicit Overrides: `override` and `final`](#1141-explicit-overrides-override-and-final)
  - [11.4.2 Defaulted and Deleted Functions](#1142-defaulted-and-deleted-functions)
  - [11.4.3 Strongly-Typed Enums (`enum class`)](#1143-strongly-typed-enums-enum-class)
  - [11.4.4 Delegating Constructors](#1144-delegating-constructors)
- [11.5 `constexpr`: Compile-Time Evaluation](#115-constexpr-compile-time-evaluation)
- [11.6 `static_assert`: Compile-Time Assertions](#116-static_assert-compile-time-assertions)
- [11.7 Professional Insights](#117-professional-insights)

---

## 11.1 Type Inference and Safety

### 11.1.1 `auto`: Automatic Type Deduction

C++11 introduced **`auto`** to let the compiler deduce the type of a variable from its initializer. This removes redundant type spelling (especially for iterator and template-heavy code) and guarantees the variable has exactly the initializer's type.

```cpp
// Listing 11.1: auto deduction basics
auto i = 42;          // int
auto d = 3.14;        // double
auto s = "hello";     // const char*
auto& ref = i;        // int&
const auto* ptr = &i; // const int*
```

`auto` uses the same deduction rules as template type deduction. Two rules dominate day-to-day use:

**Rule 1 — Reference and top-level `const` are dropped by default.**

```cpp
// Listing 11.2: auto drops references and top-level const
int x = 0;
int& y = x;
auto z = y;   // z is int, NOT int& — the reference is dropped

const int cx = 10;
auto copy = cx; // int — the const is dropped (it is a copy)
```

**Rule 2 — Re-add qualifiers explicitly with `auto&` / `const auto&`.**

```cpp
// Listing 11.3: preserving qualifiers
const int cx = 10;
const auto& ref = cx; // const int& — binds without copying
auto&& uref = 42;     // forwarding/universal reference (see Chapter 12)
```

**When to use `auto`:** iterators, range-for loop variables, lambda storage, and any expression whose type is verbose or implementation-defined. **When to avoid it:** public interfaces where the concrete type documents intent, and cases where an unexpected deduction (e.g. a proxy type like `std::vector<bool>::reference`) would surprise the reader.

### 11.1.2 `decltype`: Inspecting Types

Unlike `auto`, **`decltype`** yields the *exact declared type* of an expression — including references and `const`. It does not strip qualifiers.

```cpp
// Listing 11.4: decltype preserves the exact type
int x = 0;
decltype(x)   y = 5;   // int        — decltype of a name: its declared type
decltype((x)) z = y;   // int&       — decltype of a parenthesized lvalue expression
```

The `(x)` subtlety is the classic gotcha: a bare name gives the declared type; **a parenthesized lvalue expression** gives an lvalue reference. This matters when computing return types.

### 11.1.3 Trailing Return Types

When a function's return type depends on its parameters, the return type cannot be named before the parameters are in scope. The **trailing return type** syntax (`auto f(...) -> type`) solves this, typically paired with `decltype`.

```cpp
// Listing 11.5: trailing return type with decltype
template<typename T, typename U>
auto add(T a, U b) -> decltype(a + b) {
    return a + b;
}
// add(1, 2.5) returns double; add(1, 2) returns int
```

> **C++14 forward reference:** C++14 allows plain `auto` return-type deduction (`auto add(T a, U b) { return a + b; }`), making the trailing `-> decltype(...)` unnecessary in many cases. In C++11 it is mandatory.

### 11.1.4 `nullptr`: The Null Pointer Literal

`nullptr` is a dedicated null-pointer literal of type `std::nullptr_t`. It replaces `NULL` and `0`, eliminating overload-resolution ambiguity.

```cpp
// Listing 11.6: nullptr resolves overload ambiguity
void f(int);
void f(char*);

f(0);       // Calls f(int)
f(NULL);    // Implementation-defined (NULL is usually an integer 0)
f(nullptr); // Calls f(char*) — unambiguous
```

`nullptr` converts implicitly to any pointer type but **not** to an integral type, which is exactly the safety property `0`/`NULL` lacked.

---

## 11.2 Initialization Uniformity

### 11.2.1 Uniform (Brace) Initialization

C++11 introduced a single, consistent initialization syntax using braces (`{}`) that works for scalars, aggregates, containers, and dynamically-allocated objects.

```cpp
// Listing 11.7: brace initialization everywhere
int x{5};                 // Direct-list-initialization
int y = {5};              // Copy-list-initialization
std::vector<int> v{1, 2, 3};
std::map<int, std::string> m{{1, "a"}, {2, "b"}};
int* p = new int[3]{1, 2, 3};
```

### 11.2.2 Narrowing Prevention

Brace initialization **forbids narrowing conversions** — implicit conversions that lose information. This catches a whole class of silent bugs.

```cpp
// Listing 11.8: narrowing is a hard error inside braces
int a = 3.14;   // Compiles (with a warning); a becomes 3
int b{3.14};    // ERROR: narrowing conversion from double to int
char c{300};    // ERROR: 300 does not fit in char
```

> **Pitfall — the "most vexing" brace surprise:** for types with an `initializer_list` constructor, braces *prefer* it. `std::vector<int> v(10, 5)` makes ten 5s, but `std::vector<int> v{10, 5}` makes the two-element list `{10, 5}`. Choose parentheses vs braces deliberately.

### 11.2.3 `std::initializer_list`

`std::initializer_list<T>` is the mechanism that lets your own types accept a brace-enclosed list, exactly as the standard containers do.

```cpp
// Listing 11.9: accepting a brace list in a constructor
#include <initializer_list>

class MyVector {
public:
    MyVector(std::initializer_list<int> list) {
        for (int value : list) {
            // append value...
        }
    }
};

MyVector mv{1, 2, 3, 4}; // Calls the initializer_list constructor
```

An `initializer_list` is a lightweight view over a compiler-generated, read-only array; its elements are `const` and it is cheap to copy (it copies only the pointer and length, not the elements).

---

## 11.3 Control Flow and Iteration

### 11.3.1 Range-Based For Loops

The range-based `for` loop is syntactic sugar for iterating over anything with `begin()`/`end()` (or a C array). It eliminates iterator boilerplate and off-by-one errors.

```cpp
// Listing 11.10: the three idiomatic range-for forms
std::vector<int> v = {1, 2, 3};

for (int x : v)          { /* by value — a copy each iteration */ }
for (auto& x : v)        { x *= 2; }     // by reference — modify in place
for (const auto& x : v)  { use(x); }     // by const reference — read-only, no copy
```

**Rule of thumb:** use `const auto&` to read, `auto&` to modify, and a plain value type only for cheap-to-copy elements. The loop expands to a call to `begin(range)`/`end(range)`, so it works on standard containers, C arrays, `std::initializer_list`, and any type that provides those iterators.

---

## 11.4 Class Authoring Features

### 11.4.1 Explicit Overrides: `override` and `final`

These two contextual keywords make inheritance intent compiler-checked.

- **`override`** asserts that the function really does override a base `virtual`. A signature mismatch becomes a compile error instead of a silently-new function.
- **`final`** prevents further overriding of a virtual function, or further derivation from a class.

```cpp
// Listing 11.11: override and final
class Base {
    virtual void foo(int);
};

class Derived : public Base {
    void foo(int) override;     // OK — matches Base::foo
    // void foo(float) override; // ERROR — no matching base virtual
};

class Last final : public Base { // Cannot be inherited from
    void foo(int) final;          // Cannot be overridden further
};
```

Always write `override` on intended overrides: it is free, self-documenting, and converts the single most common inheritance bug (a typo'd signature creating a new function) into a hard error.

### 11.4.2 Defaulted and Deleted Functions

C++11 lets you explicitly request or suppress the compiler-generated special member functions.

```cpp
// Listing 11.12: =default and =delete
class Widget {
public:
    Widget() = default; // Explicitly generate the default constructor

    // Make the type non-copyable
    Widget(const Widget&)            = delete;
    Widget& operator=(const Widget&) = delete;
};
```

`= default` documents intent and keeps the function *trivial* (important for `constexpr`/POD properties), while still letting you declare other constructors. `= delete` removes a function from overload resolution entirely — use it to forbid copying, or to ban specific argument types (`void f(double) = delete;` rejects `f(3.14)`).

### 11.4.3 Strongly-Typed Enums (`enum class`)

Scoped enumerations fix the three defects of C-style enums: they do not leak their enumerators into the enclosing scope, they do not implicitly convert to `int`, and they let you fix the underlying type.

```cpp
// Listing 11.13: enum class
enum class Color : char { Red, Green, Blue }; // Underlying type fixed to char

Color c = Color::Red;   // Enumerators are scoped: Color::Red
// int i = c;           // ERROR — no implicit conversion to int
int i = static_cast<int>(c); // Explicit conversion is allowed
```

Fixing the underlying type (`: char`, `: unsigned`, etc.) guarantees size and enables forward declaration of the enum — valuable for ABI-stable headers in large systems.

### 11.4.4 Delegating Constructors

A constructor may **delegate** to another constructor of the same class, eliminating duplicated initialization logic.

```cpp
// Listing 11.14: delegating constructors
class Box {
    int w, h;
public:
    Box(int width, int height) : w(width), h(height) {}
    Box() : Box(1, 1) {} // Delegates to the two-argument constructor
};
```

The delegating constructor's body runs *after* the target constructor completes. You cannot both delegate and initialize a member in the same member-initializer list.

> **Related C++11 class features** — inheriting constructors (`using Base::Base;`) and non-static data member initializers (`int x = 0;` in the class body) are covered in **Chapter 17**, alongside the advanced literal, union, and alignment features.

---

## 11.5 `constexpr`: Compile-Time Evaluation

**`constexpr`** marks a variable or function as evaluable at compile time, enabling its result to be used in constant expressions (array bounds, template arguments, `case` labels).

```cpp
// Listing 11.15: a C++11 constexpr function
constexpr int square(int x) {
    return x * x;     // C++11: body must be a single return statement
}

int array[square(5)]; // OK — size 25 computed at compile time
constexpr int n = square(8); // Forced compile-time evaluation
```

**C++11 restriction:** a `constexpr` function body may contain essentially only a single `return` statement (plus `typedef`s, `static_assert`s, and `using` declarations) — no loops, no local variables, no branches except the ternary `?:`. Recursion is the standard workaround.

```cpp
// Listing 11.16: compile-time factorial via recursion (C++11 style)
constexpr long factorial(int n) {
    return n <= 1 ? 1 : n * factorial(n - 1);
}
static_assert(factorial(5) == 120, "math is broken");
```

> **C++14 forward reference:** C++14 relaxes these rules dramatically — `constexpr` functions may use loops, local variables, and multiple statements, making the recursive workaround unnecessary.

A `constexpr` function called with non-constant arguments simply runs at runtime, so the keyword is "compile-time *if possible*," never a penalty.

---

## 11.6 `static_assert`: Compile-Time Assertions

`static_assert` validates a compile-time boolean condition and aborts compilation with a message if it fails. It is the compile-time analogue of `assert` and a cornerstone of template metaprogramming (Chapter 14).

```cpp
// Listing 11.17: static_assert
static_assert(sizeof(void*) == 8, "64-bit platform required");

template<typename T>
struct Wrapper {
    static_assert(std::is_default_constructible<T>::value,
                  "Wrapper requires a default-constructible T");
    T value;
};
```

Because it fires during compilation, `static_assert` costs nothing at runtime and catches violated assumptions (type sizes, trait requirements, configuration constants) before a binary is ever produced.

> **C++17 forward reference:** the message argument became optional in C++17; in C++11 it is mandatory.

---

## 11.7 Professional Insights

**`auto` and low-latency code.** `auto` never introduces a hidden conversion — it binds the exact type of the initializer. This makes it *safer* than spelling a type that might trigger an implicit narrowing or a temporary. The one trap is proxy types (`vector<bool>`, expression templates): `auto x = vec[i];` may capture a proxy rather than the logical value. Prefer `auto` for clarity but know your value types.

**Brace-init in hot paths.** Narrowing prevention is a compile-time check with zero runtime cost, and uniform initialization compiles to the same code as the equivalent direct initialization. There is no performance reason to avoid braces — only the `initializer_list`-preference pitfall (§11.2.2) to keep in mind.

**`enum class` for ABI stability.** Fixing the underlying type lets you forward-declare enums in headers, decoupling translation units and shrinking rebuild times — a real win in large systems. Strong typing also prevents the accidental mixing of unrelated enumerations that plagues flag-heavy systems code.

**`constexpr` pushes work off the runtime budget.** In HFT and kernel paths, every cycle counts; moving computation (lookup tables, bit masks, dimension calculations) into `constexpr` shifts it from runtime to compile time entirely. Even with the C++11 single-return restriction, recursion covers a surprising amount of ground.

**`static_assert` as a contract.** Encoding type and platform assumptions as `static_assert`s turns "works on my machine" into a compiler-enforced contract that travels with the code.
