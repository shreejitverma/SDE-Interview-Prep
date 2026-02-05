# FUNCTIONAL PROGRAMMING IN C++11

## 1. Lambda Expressions

Anonymous functions defined inline.

### 1.1 Basic Syntax
`[captures](params) -> return_type { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int result = add(2, 3);
```

### 1.2 Capture Lists
Controls access to outer scope variables.

*   `[]`: No capture.
*   `[x]`: Capture `x` by value (copy).
*   `[&x]`: Capture `x` by reference.
*   `[=]`: Capture all by value.
*   `[&]`: Capture all by reference.
*   `[this]`: Capture class members.

```cpp
int x = 10;
auto addX = [x](int y) { return x + y; }; // x is read-only inside
```

### 1.3 Mutable Lambdas
By default, value captures are `const`. `mutable` allows modification.

```cpp
int x = 0;
auto increment = [x]() mutable { return ++x; }; // Modifies local copy
```

---

## 2. `std::function`

A polymorphic wrapper for any callable (function pointer, lambda, functor).

```cpp
#include <functional>

void freeFunc(int) {}

std::function<void(int)> f;
f = freeFunc;
f = [](int x) {}; 
```

**Cost:** Can incur heap allocation and virtual call overhead.

---

## 3. `std::bind`

Binds arguments to function parameters (Partial Application).

```cpp
int add(int a, int b) { return a + b; }

// Creates a function taking 1 argument (placeholder _1)
auto add5 = std::bind(add, 5, std::placeholders::_1); 
// add5(10) calls add(5, 10)
```

*Note: Lambdas largely replace `std::bind` in modern C++ due to readability and optimization.*
