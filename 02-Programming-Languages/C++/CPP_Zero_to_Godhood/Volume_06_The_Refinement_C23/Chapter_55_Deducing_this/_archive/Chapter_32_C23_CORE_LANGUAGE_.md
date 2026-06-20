# C23 CORE LANGUAGE


# C++23 CORE LANGUAGE UPGRADES

## 1. Deducing `this` (Explicit Object Parameter)

This is one of the most significant changes in C++23. It allows member functions to deduce the value category (`const`, `&`, `&&`) of the object they are called on, without writing 4+ overloads.

### 1.1 The Problem (C++20 and older)

To handle lvalues, const lvalues, rvalues, and const rvalues, you needed overloads:

```cpp
struct Widget {
    void process() &;       // lvalue
    void process() const&;  // const lvalue
    void process() &&;      // rvalue
    void process() const&&; // const rvalue
};
```

### 1.2 The Solution

Pass `this` as an explicit argument.

```cpp
struct Widget {
    // 'self' deduces the type of the object (e.g., Widget&, const Widget&, Widget&&)
    template <typename Self>
    void process(this Self&& self) {
        // Forward self to another function
        handle(std::forward<Self>(self)); 
    }
};
```

### 1.3 Recursive Lambdas

Deducing `this` allows lambdas to be recursive without `std::function` or hacks.

```cpp
auto fib = [](this auto&& self, int n) {
    if (n <= 1) return n;
    return self(n - 1) + self(n - 2);
};

int result = fib(10); // 55
```

## 2. `if consteval`

A safer version of `if (std::is_constant_evaluated())`.

```cpp
consteval int compile_time_algo(int n) { return n * n; }
int runtime_algo(int n) { return n * n; }

constexpr int heavy_math(int n) {
    if consteval {
        return compile_time_algo(n); // Executed ONLY at compile time
    } else {
        return runtime_algo(n);      // Executed at runtime
    }
}
```

## 3. Multidimensional Subscript Operator (`operator[]`)

C++23 finally allows multiple arguments in `[]`.

```cpp
struct Matrix {
    std::vector<double> data;
    size_t cols;

    double& operator[](size_t r, size_t c) {
        return data[r * cols + c];
    }
};

Matrix m;
m[1, 2] = 3.14; // Clean syntax!
```

## 4. `auto(x)`: Decay Copy

Explicitly create a prvalue copy of an lvalue. Useful in generic code to prevent accidental referencing.

```cpp
void process(const auto& x) {
    auto copy = auto(x); // Explicit decay-copy
    // ...
}
```

## 5. Literal Suffixes for `size_t`

*   `uz` or `z` for `size_t` (unsigned equivalent of `ptrdiff_t`).
*   `z` for `ptrdiff_t` (signed).

```cpp
auto s = 100uz; // std::size_t
auto p = 100z;  // std::ptrdiff_t
```

## 6. `#warning`

Standardized preprocessor warning.

```cpp
#warning "This feature is experimental"
```


---
