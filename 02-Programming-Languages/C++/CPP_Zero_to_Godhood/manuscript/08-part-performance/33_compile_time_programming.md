# Chapter 33: Compile-Time Programming

> *The fastest code is code that doesn't run.*

The ultimate performance optimization in C++ is forcing the compiler to do the math for you. If a calculation is done at compile time, the result is baked directly into the final executable as a hardcoded constant. At runtime, the execution time is literally zero.

This philosophy is unique to C++. In interpreted languages (like Python) or JIT-compiled languages (like Java), the line between compile-time and run-time is blurred. In C++, the line is absolute, and we exploit it ruthlessly.

---

## 33.1 The Philosophy of Zero-Overhead Abstractions

Bjarne Stroustrup's foundational rule for C++ is the **Zero-Overhead Principle**:
1. What you don't use, you don't pay for.
2. What you do use, you couldn't hand-code any better.

This principle drives the evolution of the `constexpr` keyword.

## 33.2 The Evolution of `constexpr`

The `constexpr` keyword was introduced in C++11 to tell the compiler: *"This function might be evaluable at compile time."*

### C++11: The Dark Ages
In C++11, a `constexpr` function could only consist of a **single `return` statement**. No loops, no local variables. If you wanted to do anything complex, you had to use recursion and ternary operators.

```cpp
constexpr int factorial(int n) {
    return n <= 1 ? 1 : (n * factorial(n - 1));
}
```

### C++14: The Awakening
C++14 removed the single-return restriction. You could use `for` loops, `if` statements, and local variables.

```cpp
constexpr int factorial(int n) {
    int result = 1;
    for (int i = 1; i <= n; ++i) result *= i;
    return result;
}
```

### C++17 and C++20: The Golden Age
C++17 allowed lambdas to be `constexpr`.
C++20 blew the doors wide open. In C++20, a `constexpr` function can:
*   Allocate memory dynamically (`new`/`delete`, `std::vector`, `std::string`) as long as the memory is freed before the compile-time evaluation finishes (Transient Allocation).
*   Use `virtual` functions and polymorphism.
*   Use `try/catch` blocks (though actually throwing an exception stops compilation).

You can now parse JSON files, generate lookup tables, and sort arrays entirely during compilation.

## 33.3 `constexpr` vs `consteval` vs `constinit`

`constexpr` means a function *can* be evaluated at compile time. But if you pass it a variable that is only known at runtime, the compiler silently downgrades it to a normal runtime function.

```cpp
int x; std::cin >> x;
int y = factorial(x); // Runs at runtime. The compiler doesn't warn you!
```

To give developers more control, C++20 introduced two new keywords:

*   **`consteval` (Immediate Functions)**: This function **MUST** be evaluated at compile time. If you pass it a runtime variable, compilation fails. 
*   **`constinit`**: Ensures a variable is initialized at compile time (fixing the Static Initialization Order Fiasco), but allows the variable to be mutated later at runtime.

## 33.4 `std::is_constant_evaluated` and `if consteval`

Sometimes you want a function to do one thing at compile time, and a completely different thing at runtime. For example, at compile time you might use a slow, standard `for` loop, but at runtime you want to use blazing-fast SIMD intrinsic assembly instructions (which the compiler can't execute during compilation).

In C++20, you used `std::is_constant_evaluated()`. In C++23, this was upgraded to a native language feature: **`if consteval`**.

```cpp
constexpr double custom_sqrt(double x) {
    if consteval {
        // Compile-time logic: Use Newton-Raphson approximation
        return newton_raphson(x);
    } else {
        // Runtime logic: Use the hardware CPU instruction
        return __builtin_sqrt(x); 
    }
}
```

## 33.5 Compile-Time String Hashing

String comparisons are slow. In game engines or command routers, comparing `"move_forward" == input` takes a lot of CPU cycles. 

Instead, we use `consteval` to hash strings at compile time.

```cpp
consteval uint32_t hash_string(std::string_view s) {
    uint32_t hash = 2166136261u;
    for (char c : s) {
        hash ^= c;
        hash *= 16777619;
    }
    return hash;
}

// The compiler calculates the hash and replaces this entire 
// line with: uint32_t my_cmd = 3289045761u;
uint32_t my_cmd = hash_string("move_forward"); 
```

Now, at runtime, you are just comparing two 32-bit integers, which takes 1 CPU cycle.

---

## 33.6 Link-Time and Profile-Guided Optimization

Not all optimizations happen in the code you write. The compiler has two final tricks.

### Link-Time Optimization (LTO)
Normally, the compiler compiles each `.cpp` file in isolation. If `math.cpp` has a function `add()`, and `main.cpp` calls `add()`, the compiler cannot inline it because it can't see the implementation. 
LTO delays optimization until the Linker combines all the files. The Linker looks at the entire program at once and can aggressively inline functions across different `.cpp` files, removing function call overhead.

### Profile-Guided Optimization (PGO)
Even with LTO, the compiler has to guess which `if` branches are the most common. 
With PGO:
1.  You compile your program with special tracking flags.
2.  You run the program with representative user data. The program records exactly which branches are taken and which functions are called the most.
3.  You feed this data back into the compiler and compile a second time. The compiler uses the real-world data to perfectly optimize branch prediction and instruction caching.

PGO can yield a "free" 10-15% performance boost in massive applications like web browsers or database engines.

---

We have now conquered the C++ language, the memory model, and the hardware. In the final phases of this book, we will step back and look at the big picture: **Software Architecture and Design**.
