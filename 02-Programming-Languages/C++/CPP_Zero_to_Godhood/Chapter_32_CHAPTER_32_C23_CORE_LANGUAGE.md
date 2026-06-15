# CHAPTER 32: C23 CORE LANGUAGE


# C++23 CORE LANGUAGE UPGRADES

C++23 is the "Ergonomics" release. It polishes the massive changes introduced in C++20, removing boilerplate and completing the modern C++ paradigm.

### 1. Deducing `this` (Explicit Object Parameters)
One of the most revolutionary changes for class design. It allows a member function to explicitly declare the object it is called on as its first parameter.
*   **Recursive Lambdas**: A lambda can now easily call itself without `std::function` overhead.
    ```cpp
    auto fib = [](this auto self, int n) -> int {
        return n <= 1 ? n : self(n - 1) + self(n - 2);
    };
    ```
*   **Simplifying CRTP & Forwarding**: You no longer need 4 overloads (`&`, `const&`, `&&`, `const&&`) or complex CRTP inheritance to perfectly forward a member.
    ```cpp
    template<typename Self>
    auto&& get_data(this Self&& self) {
        // Automatically preserves const/ref qualifiers of the object
        return std::forward<Self>(self).data; 
    }
    ```

### 2. Syntactic Ergonomics
*   **Multidimensional Subscript Operator**: You can now pass multiple arguments to `operator[]`, which is crucial for linear algebra (pairs perfectly with `std::mdspan`).
    ```cpp
    struct Matrix {
        double& operator[](size_t row, size_t col) { return data[row * cols + col]; }
    };
    matrix[1, 2] = 42.0; 
    ```
*   **`if consteval`**: A cleaner, standardized way to execute different code depending on whether the function is evaluated at compile time or run time.
    ```cpp
    constexpr double power(double d, int p) {
        if consteval { return compile_time_pow(d, p); }
        else { return std::pow(d, p); } // Run time
    }
    ```
*   **`auto(x)` and `auto{x}` (Decay Copy)**: Explicitly requests a PR-value copy of a variable, forcing decay (useful in generic macros and templates).
    ```cpp
    void f(auto& x) {
        auto copy = auto(x); // Explicit copy, stripping references
    }
    ```
*   **Static `operator()`**: Lambdas that don't capture anything can now have a `static` call operator, allowing them to be passed as traditional C function pointers seamlessly.
    ```cpp
    auto f = [] static (int x) { return x * 2; };
    ```
*   **Size_t Literal Suffixes**: Use `z` for signed `ssize_t` and `uz` for unsigned `size_t`.
    ```cpp
    for (auto i = 0uz; i < vec.size(); ++i) {} 
    ```
*   **Labels at the end of compound statements**: You no longer need to put a dummy statement after a label at the end of a block.
