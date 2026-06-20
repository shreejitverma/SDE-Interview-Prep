# CHAPTER 21: C17 TEMPLATE METAPROGRAMMING


# C++17 TEMPLATE METAPROGRAMMING ENHANCEMENTS

## 1. Fold Expressions

Simplifies variadic template unpacking. No more recursion needed for basic operations.

### 1.1 Unary Folds

```cpp
template<typename... Args>
auto sum(Args... args) {
    return (... + args); // Unary left fold: ((arg1 + arg2) + arg3) ...
}

int result = sum(1, 2, 3, 4); // 10
```

### 1.2 Binary Folds

```cpp
template<typename... Args>
void print(Args... args) {
    (std::cout << ... << args) << "\n"; // (cout << arg1) << arg2 ...
}
```

### 1.3 Operators
Supported operators: `+`, `-`, `*`, `/`, `%`, `^`, `&`, `|`, `=`, `<<`, `>>`, `+=`, `-=`, etc., `==`, `!=`, `<`, `>`, `&&`, `||`, `,`, `.*`, `->*`.

## 2. Class Template Argument Deduction (CTAD)

Compiler deduces template arguments for class templates from the constructor.

```cpp
#include <vector>
#include <tuple>
#include <mutex>

// C++14: Types explicit
std::pair<int, double> p1(1, 3.14);
std::vector<int> v1 = {1, 2, 3};
std::lock_guard<std::mutex> lk(mtx);

// C++17: Types deduced
std::pair p2(1, 3.14); // pair<int, double>
std::vector v2 = {1, 2, 3}; // vector<int>
std::lock_guard lk2(mtx); // lock_guard<mutex>
```

### 2.1 User-Defined Deduction Guides

Sometimes the compiler needs help to deduce the correct type.

```cpp
template<typename T>
struct Wrapper {
    T value;
    Wrapper(T v) : value(v) {}
};

// Deduction guide: "If constructed with const char*, deduce string"
Wrapper(const char*) -> Wrapper<std::string>;

Wrapper w("hello"); // Wrapper<std::string>, not Wrapper<const char*>
```

## 3. `auto` Non-Type Template Parameters

Templates can deduce the type of non-type parameters.

```cpp
template<auto Value>
void print_value() {
    std::cout << Value << "\n";
}

int main() {
    print_value<42>();   // Value is int 42
    print_value<'c'>();  // Value is char 'c'
}
```

## 4. `std::invoke`

Uniform way to invoke any callable (function pointer, functor, lambda, member function pointer).

```cpp
#include <functional>

struct Foo {
    void bar(int i) { std::cout << "Foo::bar " << i << "\n"; }
    int data = 10;
};

void free_func(int i) { std::cout << "free_func " << i << "\n"; }

int main() {
    Foo f;

    // Call member function
    std::invoke(&Foo::bar, f, 1);

    // Access member data
    std::cout << std::invoke(&Foo::data, f) << "\n";

    // Call free function
    std::invoke(free_func, 2);
}
```
