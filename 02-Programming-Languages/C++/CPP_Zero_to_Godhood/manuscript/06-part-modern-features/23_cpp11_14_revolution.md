# Part VI: Modern C++ Features Tour

*The features that transformed C++ from C++11 through C++26.*

# Chapter 23: The C++11/14 Revolution

> *The standard that changed everything.*

For over a decade after C++98, the language stagnated. It was powerful, but it was incredibly verbose, prone to leaks, and lacked native support for modern hardware capabilities (like multicore threading).

Then came C++11. It wasn't just an update; it was a revolution. It fundamentally changed how C++ was written, creating what we now call **Modern C++**. Three years later, C++14 released as a massive "bug fix" and polish pass for C++11. 

This chapter is a rapid-fire tour of the core language upgrades that made C++ usable again.

---

## 23.1 `auto` and `decltype`

Before C++11, iterating over a map looked like this:
```cpp
for (std::map<std::string, std::vector<int>>::const_iterator it = m.begin(); it != m.end(); ++it)
```

C++11 introduced `auto`, allowing the compiler to deduce the type of a variable from its initializer.

```cpp
auto x = 5;          // int
auto y = 3.14;       // double
auto name = "Alice"; // const char*

// auto drops references and const!
const int c = 10;
auto a1 = c;         // int (copied)
const auto& a2 = c;  // const int& (reference maintained)
```

While `auto` deduces a type, `decltype` allows you to extract the *exact* type of an expression, perfectly preserving references and `const`.

```cpp
int x = 0;
decltype(x) y = 5;   // y is an int
decltype((x)) z = y; // z is an int& (because (x) is an expression)
```

## 23.2 Uniform Initialization `{}`

Before C++11, initialization was a mess. You used `=` for ints, `()` for constructors, and `{}` for arrays. C++11 unified this with **Brace Initialization**.

```cpp
int x{5};
std::string s{"Hello"};
std::vector<int> v{1, 2, 3}; // Enabled by std::initializer_list
```

**Crucial Benefit**: Brace initialization prevents *narrowing conversions*.
```cpp
int a = 3.14; // Compiles (Warning), truncates to 3
int b{3.14};  // ERROR: Narrowing conversion blocked!
```

## 23.3 `nullptr`

For 30 years, C++ used `NULL` (which was secretly just `#define NULL 0`). This caused horrible overload resolution bugs. C++11 introduced `nullptr`, a dedicated, strongly-typed null pointer constant.

```cpp
void process(int);
void process(char*);

process(NULL);    // Called process(int)! (Disaster)
process(nullptr); // Calls process(char*) safely
```
*Rule of Godhood: Never use `NULL` or `0` for pointers. Always use `nullptr`.*

## 23.4 Scoped Enums (`enum class`)

Old C-style enums leaked their names into the surrounding scope, and implicitly converted to integers, causing silent bugs. C++11 introduced `enum class`.

```cpp
enum class Color : uint8_t { Red, Green, Blue };
enum class Alert { Red, Yellow }; // No name collision!

Color c = Color::Red;
// int val = c; // ERROR: No implicit conversion to int
```

## 23.5 Range-Based `for`

Combined with `auto`, C++11 finally added a modern loop syntax for arrays and containers.

```cpp
std::vector<int> data = {1, 2, 3};

for (const auto& val : data) { // Read-only
    std::cout << val << " ";
}

for (auto& val : data) { // Modify
    val *= 2;
}
```

## 23.6 `static_assert`

Assertions that run during compilation. If the condition is false, the code refuses to compile.

```cpp
static_assert(sizeof(void*) == 8, "This code requires a 64-bit OS.");
```

## 23.7 `constexpr` Functions

Functions that can be executed entirely during compilation, leaving zero overhead at runtime.

```cpp
constexpr int square(int x) {
    return x * x;
}

// Evaluated by the compiler. 'arr' is exactly 25 elements.
int arr[square(5)]; 
```
*(Note: In C++11, a `constexpr` function was limited to a single `return` statement. C++14 lifted this restriction, allowing loops and local variables).*

## 23.8 `alignas` and `alignof`

Hardware caches love aligned data. C++11 gave developers direct control over memory alignment.

```cpp
alignas(32) struct Vector4 { // Force 32-byte alignment
    float x, y, z, w;
};

std::cout << alignof(Vector4); // Prints 32
```

## 23.9 Ref-Qualified Member Functions

You can restrict a member function so it can only be called if the object is an lvalue (persistent) or an rvalue (temporary).

```cpp
class Data {
public:
    void print() & { std::cout << "I am a persistent lvalue.\n"; }
    void print() && { std::cout << "I am a temporary rvalue.\n"; }
};

Data d;
d.print();         // Calls & version
Data().print();    // Calls && version
```

## 23.10 The Right Angle Bracket Fix

In C++98, `std::vector<std::vector<int>>` was a syntax error. You had to put a space between the closing brackets `> >`, otherwise the compiler parsed it as the bitwise shift operator `>>`. C++11 finally fixed this parsing bug.

## 23.11 Attributes

C++11 introduced standardized attributes inside `[[ ]]` to give hints to the compiler.

*   `[[noreturn]]`: Tells the compiler a function never returns (e.g., `exit(1)` or an infinite loop).
*   `[[deprecated("Use v2")]]`: Issues a warning if someone calls the function `[C++14]`.

## 23.12 The C++14 Polish Pass

C++14 was a minor release that polished the rough edges of C++11. Key additions included:

1.  **Return Type Deduction**: You no longer need `-> decltype(...)` for `auto` functions.
    ```cpp
    auto add(int a, int b) { return a + b; } // Deduces int
    ```
2.  **Generic Lambdas**: Lambdas can use `auto` parameters.
    ```cpp
    auto multiply = [](auto a, auto b) { return a * b; };
    ```
3.  **`std::make_unique`**: C++11 forgot to include this alongside `std::make_shared`. C++14 fixed it.
4.  **Binary Literals and Digit Separators**: 
    ```cpp
    int bin = 0b1010_1111_0000;
    long mass = 1'000'000'000; // The apostrophe is ignored by the compiler
    ```

---

C++11 and C++14 laid the foundation. We had RAII, Move Semantics, Lambdas, and `auto`. 

But the language was still missing standard tools for daily tasks like reading the filesystem or returning optional values. In the next chapter, we look at **C++17**, the standard that finally gave C++ a modern standard library vocabulary.
