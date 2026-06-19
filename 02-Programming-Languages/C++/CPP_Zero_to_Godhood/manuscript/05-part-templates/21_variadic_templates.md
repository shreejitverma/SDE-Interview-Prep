# Chapter 21: Variadic Templates and Fold Expressions

> *Templates that accept an infinite number of arguments.*

In C, if you wanted a function to take any number of arguments, you used `stdarg.h` (the technology behind `printf`). But `printf` is inherently unsafe. If you pass an `int` but accidentally use `%s` in the format string, the program will crash at runtime. The compiler cannot help you because `printf` bypasses the type system completely.

C++11 introduced **Variadic Templates**, a way to write functions and classes that accept an arbitrary number of arguments, with 100% compile-time type safety.

---

## 21.1 Parameter Packs

To create a variadic template, you use an ellipsis (`...`) to define a **Parameter Pack**.

```cpp
// Ts is a "Template Parameter Pack" (A list of types)
// args is a "Function Parameter Pack" (A list of variables)
template <typename... Ts>
void print_all(Ts... args) {
    // How do we actually use 'args'?
}
```

If you call `print_all(1, 3.14, "hello")`, the compiler deduces the pack `Ts` as `<int, double, const char*>`. 

But how do you actually print them? You can't just use a `for` loop because the types are different. `args` is not an array; it is a compile-time list.

## 21.2 The C++11 Way: Recursive Unpacking

In C++11, the only way to process a parameter pack was using a functional-programming technique: recursion.

You define a "base case" function, and a "recursive" function that peels off one argument at a time.

```cpp
#include <iostream>

// 1. The Base Case (Terminator)
void print_all() {
    std::cout << "\n";
}

// 2. The Recursive Step
// 'First' peels off the first argument. 
// 'Rest' contains the remaining arguments.
template <typename First, typename... Rest>
void print_all(First first_arg, Rest... remaining_args) {
    std::cout << first_arg << " ";
    
    // Pack Expansion: Unpack the rest and call the function again
    print_all(remaining_args...); 
}

int main() {
    print_all(1, 3.14, "Godhood"); 
    // Calls: print_all(1, [3.14, "Godhood"])
    // Calls: print_all(3.14, ["Godhood"])
    // Calls: print_all("Godhood", [])
    // Calls: print_all() -> prints newline
}
```

This works, but it's incredibly tedious to write two separate functions just to loop over a few variables.

## 21.3 The C++17 Way: Fold Expressions

C++17 revolutionized variadic templates by introducing **Fold Expressions**. Fold expressions allow you to apply a binary operator (like `+`, `*`, or `<<`) to every element in a parameter pack instantly, without recursion.

```cpp
// C++17 Fold Expression
template <typename... Ts>
void print_all(Ts... args) {
    // Unary Left Fold: (std::cout << ... << args)
    (std::cout << ... << args) << "\n";
}
```

Want a function that sums an infinite number of numbers?

```cpp
template <typename... Ts>
auto sum(Ts... args) {
    return (... + args); // Unary Left Fold: (arg1 + arg2 + arg3...)
}

int total = sum(1, 2, 3, 4, 5); // 15
```

## 21.4 `sizeof...` — Counting Elements

You can ask the compiler exactly how many items are inside a pack using the `sizeof...` operator.

```cpp
template <typename... Ts>
void analyze(Ts... args) {
    std::cout << "You passed " << sizeof...(args) << " arguments.\n";
}
```

## 21.5 Pack Indexing [C++26]

For 15 years, if you wanted to get the 3rd element in a parameter pack, you had to write insane metaprogramming loops to "peel" elements off until you reached the 3rd one. 

C++26 finally introduces **Pack Indexing**. You can now treat a parameter pack like an array and index directly into it using `...[index]`.

```cpp
template <typename... Ts>
void print_second_element(Ts... args) {
    // Ensure there are at least two elements
    static_assert(sizeof...(args) >= 2);
    
    // Access the element at index 1 (the second element)
    std::cout << args...[1] << "\n";
}

int main() {
    print_second_element(10, 20, 30); // Prints 20
}
```

## 21.6 `std::tuple` and `std::apply`

A parameter pack only exists at compile time. What if you want to store a list of different types inside an object and pass it around at runtime?

You use `std::tuple`, which is a generalization of `std::pair` that can hold N elements.

```cpp
#include <tuple>
#include <iostream>

std::tuple<int, double, std::string> my_data(42, 3.14, "Alice");

// Accessing elements (must use compile-time constants)
std::cout << std::get<0>(my_data) << "\n"; // 42
std::cout << std::get<2>(my_data) << "\n"; // "Alice"
```

If you have a function `process(int, double, std::string)`, and you want to pass your tuple into it, you use `std::apply` (C++17). `std::apply` instantly cracks the tuple open and spreads its contents as arguments to the function.

```cpp
void process(int a, double b, std::string c) {
    std::cout << "Processing: " << a << ", " << b << ", " << c << "\n";
}

int main() {
    std::tuple my_data(42, 3.14, "Alice"); // CTAD deduces types
    
    std::apply(process, my_data); 
}
```

## 21.7 `std::integer_sequence`

How does `std::apply` actually work under the hood? It uses a compile-time metaprogramming trick called `std::integer_sequence`.

A `std::integer_sequence` is literally just a compile-time list of integers (e.g., `0, 1, 2`). By generating an index sequence that matches the size of a tuple, metaprogrammers can use a fold expression to call `std::get<0>`, `std::get<1>`, and `std::get<2>` simultaneously.

```cpp
// Generating a sequence of 0, 1, 2
using Indices = std::make_index_sequence<3>; 
```
*Note: You rarely use this directly in modern C++ unless you are writing deep library infrastructure.*

---

Variadic templates give you the power to build type-safe logging systems, infinite-argument math functions, and flexible generic containers. 

But sometimes, when writing templates, we need to ask the compiler extremely specific questions. *"Is this type a pointer?"*, *"Is this type an integer?"*, *"Is this class copyable?"*. In the next chapter, we look at the ultimate reflection tool: **Type Traits**.
