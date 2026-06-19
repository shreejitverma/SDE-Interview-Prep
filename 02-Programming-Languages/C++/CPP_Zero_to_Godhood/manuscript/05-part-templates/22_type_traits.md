# Chapter 22: Type Traits and Compile-Time Introspection

> *Asking the compiler questions about your types.*

When you write a normal function, you inspect *values*. You write `if (x > 0)` or `if (player.isAlive())`. You make decisions based on runtime data.

When you write a Template, you inspect *types*. You might want a template to do one thing if `T` is an `int`, and something completely different if `T` is a pointer. 

C++ provides an entire standard library header dedicated to asking the compiler questions about types: `<type_traits>`.

---

## 22.1 Asking Questions: The `is_` Family

The `<type_traits>` header provides dozens of compile-time structures that evaluate to either `true` or `false` based on the type you pass in.

```cpp
#include <type_traits>
#include <iostream>

int main() {
    // 1. Primary Type Categories
    std::cout << std::is_integral<int>::value << "\n";       // 1 (true)
    std::cout << std::is_floating_point<int>::value << "\n"; // 0 (false)
    std::cout << std::is_pointer<int*>::value << "\n";       // 1 (true)
    
    // 2. Type Properties
    std::cout << std::is_const<const int>::value << "\n";    // 1 (true)
    std::cout << std::is_unsigned<unsigned int>::value;      // 1 (true)
}
```

Because these are evaluated at compile time, they have zero runtime overhead. They are literally replaced by `1` or `0` before the program even runs.

## 22.2 The `_v` Suffix [C++17]

Writing `::value` at the end of every trait is annoying and clutters the code. C++17 introduced variable templates (which we learned about in Chapter 19) to create a much cleaner syntax: the `_v` suffix.

```cpp
// Old C++11 Way
bool a = std::is_class<std::string>::value;

// Modern C++17 Way
bool b = std::is_class_v<std::string>; 
```
*Always use the `_v` suffix in modern code.*

## 22.3 Type Relationships

You can also ask the compiler to compare two types and tell you how they relate.

```cpp
// Are these the exact same type?
std::is_same_v<int, int32_t>;      // true (usually)
std::is_same_v<int, const int>;    // false (const changes the type!)

class Base {}; class Derived : public Base {};

// Does one inherit from the other?
std::is_base_of_v<Base, Derived>;  // true

// Can one be safely converted to the other?
std::is_convertible_v<int, double>; // true
std::is_convertible_v<std::string, int>; // false
```

## 22.4 Modifying Types

Type traits aren't just for asking questions; they can also be used to actively *modify* types during compilation. Instead of returning a `true`/`false` boolean, these traits return a new type.

```cpp
// 1. Remove properties
std::remove_const<const int>::type;       // Results in 'int'
std::remove_reference<int&>::type;        // Results in 'int'
std::remove_pointer<int*>::type;          // Results in 'int'

// 2. Add properties
std::add_pointer<int>::type;              // Results in 'int*'
```

### The `_t` Suffix [C++14]
Just like `_v` replaced `::value`, the `_t` suffix replaces `::type` using alias templates.

```cpp
// Old C++11 Way
using MyType = typename std::remove_reference<int&>::type;

// Modern C++14 Way
using MyType = std::remove_reference_t<int&>;
```

### The Ultimate Modifier: `std::decay_t`
When you pass an array to a function, it "decays" into a pointer. When you pass a function, it decays into a function pointer. If you want a template to perfectly simulate what the compiler does to a type when passing it by value, you use `std::decay_t`. It removes const, removes references, and decays arrays to pointers.

```cpp
std::decay_t<const int&>; // Results in 'int'
std::decay_t<int[10]>;    // Results in 'int*'
```

## 22.5 Compile-Time Logic: `std::conditional`

If you want to choose between two different types based on a compile-time condition, you use `std::conditional_t` (the compile-time equivalent of the ternary operator `? :`).

```cpp
// If the first argument is true, choose int. If false, choose float.
using MyNumber = std::conditional_t<true, int, float>; // MyNumber is 'int'

// A practical example:
template <typename T>
class Wrapper {
    // If T is massive, store a pointer to it. If T is small, store it directly.
    using StorageType = std::conditional_t<(sizeof(T) > 8), T*, T>;
    
    StorageType data;
};
```

## 22.6 `decltype` and `std::declval`

Sometimes you don't have a type; you have an *expression*, and you want to know what type it will produce if executed.

The `decltype` keyword answers the question: *"If I ran this code, what type would it return?"*

```cpp
int x = 5;
double y = 3.14;

decltype(x + y) result; // x+y is a double, so 'result' is declared as a double.
```

But what if you are inside a template and you want to test calling a method on `T`, but `T` doesn't have a default constructor? You can't instantiate it to test it!

Enter `std::declval<T>()`. This is a magical, compile-time-only function that pretends to create an instance of `T` out of thin air so you can test expressions on it.

```cpp
struct NoDefault {
    NoDefault(int x) {} // Requires an int
    double do_math();
};

// We want to know what do_math() returns, but we can't create a NoDefault object easily.
// std::declval fakes the object creation at compile-time!
using ReturnType = decltype( std::declval<NoDefault>().do_math() ); // ReturnType is double
```

---

With Type Traits, Variadics, and Concepts, you now possess the complete arsenal of C++ Metaprogramming. You can write code that writes itself, perfectly optimized for any scenario, completely verified before the program ever runs.

This brings us to the end of Part V. You are now officially crossing the threshold from Intermediate to Advanced. In the next section, we will explore the massive language upgrades that defined the "Modern Era" of C++: The C++11, 14, and 17 Revolutions.
