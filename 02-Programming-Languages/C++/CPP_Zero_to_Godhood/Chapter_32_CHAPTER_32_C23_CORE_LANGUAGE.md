# CHAPTER 32: C23 CORE LANGUAGE


# C++23 CORE LANGUAGE UPGRADES

C++23 is the "Ergonomics" release. It polishes the massive changes introduced in C++20, removing boilerplate and completing the modern C++ paradigm.

### 1. Deducing `this` (Explicit Object Parameters)
One of the most revolutionary changes for class design. It makes the implicit `this` pointer explicit as a named parameter.
*   **Recursive Lambdas**: A lambda can easily call itself without `std::function` overhead.
    ```cpp
    auto fib = [](this auto self, int n) -> int {
        return n <= 1 ? n : self(n - 1) + self(n - 2);
    };
    ```
*   **Simplifying CRTP & Forwarding**: You no longer need 4 overloads (`&`, `const&`, `&&`, `const&&`) to perfectly forward a member.
    ```cpp
    template<typename Self>
    auto&& get_data(this Self&& self) {
        return std::forward<Self>(self).data; 
    }
    ```

### 2. Syntactic Ergonomics & Operations
*   **Multidimensional operator[]**: You can now pass multiple arguments to `operator[]`, enabling clean multi-index syntax (`arr[i, j]`).
    ```cpp
    struct Matrix {
        double& operator[](size_t r, size_t c) { return data[r * cols + c]; }
    };
    ```
*   **if consteval**: A cleaner language-level replacement for `if (std::is_constant_evaluated())`. The body can call `consteval` functions directly.
    ```cpp
    constexpr int f(int i){ 
        if consteval { return i * 2; } 
        else { return i; } 
    }
    ```
*   **auto(x) / auto{x} (Decay Copy)**: Creates a decay-copy of an expression as a prvalue. Replaces the internal `decay_copy` workaround.
    ```cpp
    std::erase(v.begin(), v.end(), auto(v.front()));
    ```
*   **Static operator() and operator[]**: Lambdas and functors without state can declare these operators `static`, allowing the compiler to omit passing the hidden `this` pointer.
    ```cpp
    auto fn = [](int x) static { return x * 2; };
    ```
*   **uz / z literals**: New literal suffixes (`uz` for `size_t`, `z` for `ptrdiff_t`), eliminating signed/unsigned mismatch warnings in loop counters.
    ```cpp
    for (auto i = 0uz; i < v.size(); ++i){}
    ```

### 3. Preprocessor & Imports
*   **import std;**: The holy grail. The entire C++ standard library is importable as a single module unit, eliminating dozens of `#include` directives.
    ```cpp
    import std;
    int main(){ std::println("Hello C++23!"); }
    ```
*   **#elifdef / #elifndef**: Chain preprocessor directives cleanly, removing deeply nested conditionals.
*   **#warning**: Standardizes the widely-supported `#warning` preprocessor diagnostic.

### 4. Safety & Optimization
*   **Lifetime extension of temporaries in range-for**: Temporaries created in the range-initializer now live for the full duration of the loop, fixing a massive UB footgun.
    ```cpp
    for (auto e : getVector()[0]) {} // Now safe!
    ```
*   **[[assume(expr)]] attribute**: Tells the compiler that `expr` is always true. Replaces vendor extensions like `__builtin_assume`.
*   **Simpler implicit move**: A move-eligible id-expression in a `return` or `throw` is always treated as an xvalue.
*   **constexpr relaxations**: Static `constexpr` local variables and `std::unique_ptr` are now allowed in `constexpr` contexts.
