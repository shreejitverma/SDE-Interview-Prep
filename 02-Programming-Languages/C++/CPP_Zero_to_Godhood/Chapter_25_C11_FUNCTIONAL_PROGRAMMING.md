# C++11 FUNCTIONAL PROGRAMMING


C++11 brought functional programming paradigms to the language, centered around Lambdas.

---

## 1. LAMBDA EXPRESSIONS

Lambdas are anonymous function objects.

### 1.1 Syntax

`[ captures ] ( params ) -> ret { body }`

```cpp
auto add = [](int a, int b) { return a + b; };
int sum = add(1, 2);
```

### 1.2 Captures

- `[]`: No capture.
- `[=]`: Capture everything by value (copy).
- `[&]`: Capture everything by reference.
- `[x]`: Capture x by value.
- `[&x]`: Capture x by reference.

```cpp
int factor = 10;
auto multiply = [factor](int n) { return n * factor; }; // factor captured by value
```

### 1.3 Mutable Lambdas

By default, value captures are `const`. Use `mutable` to modify them.

```cpp
int x = 0;
auto increment = [x]() mutable { return ++x; }; // x is internal state
```

---

## 2. STD::FUNCTION

`std::function` is a polymorphic wrapper for *any* callable (function pointer, lambda, functor, bind result).

```cpp
#include <functional>

void print(int i) { std::cout << i; }

std::function<void(int)> func;
func = print;
func = [](int i) { std::cout << i * 2; };
```

It has runtime overhead (virtual function call, possible allocation). Prefer templates or auto if possible.

---

## 3. STD::BIND

`std::bind` performs partial application of functions.

```cpp
#include <functional>
using namespace std::placeholders;

int sub(int a, int b) { return a - b; }

// Bind second argument to 5
auto sub5 = std::bind(sub, _1, 5); 
// sub5(10) calls sub(10, 5) -> 5
```

**Note:** Lambdas mostly replaced `std::bind` in modern C++ because they are clearer and faster (compiler optimization).

