# C14 FUNCTIONS AND LAMBDAS


# C++14 FUNCTIONS & LAMBDAS

## 1. Generic Lambdas

C++14 allows `auto` in lambda parameters, effectively making them templates.

```cpp
auto print = [](auto x) {
    std::cout << x << "\n";
};

print(10);      // int
print("hello"); // const char*
```

This is shorthand for a struct with a templated `operator()`.

## 2. Generalized Lambda Captures (Init-Capture)

C++14 allows initializing variables inside the lambda capture clause. This is crucial for **move-only types**.

```cpp
auto ptr = std::make_unique<int>(10);

// Move ptr into lambda
auto lambda = [p = std::move(ptr)]() {
    std::cout << *p << "\n";
};
// ptr is now nullptr
```

## 3. Automatic Return Type Deduction

Functions can deduce their return type from the return statement.

```cpp
auto add(int a, int b) {
    return a + b; // Deduced as int
}

// decltype(auto) preserves references
int& getRef(int& x) { return x; }

decltype(auto) forwardRef(int& x) {
    return getRef(x); // Returns int& (auto would return int)
}
```


---
