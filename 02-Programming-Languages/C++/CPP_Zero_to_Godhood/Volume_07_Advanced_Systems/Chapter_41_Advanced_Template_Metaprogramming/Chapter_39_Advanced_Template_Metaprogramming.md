# ADVANCED TEMPLATE METAPROGRAMMING

## 1. The Evolution of TMP

Template Metaprogramming (TMP) is the art of using the compiler to generate code.

*   **C++98:** Recursive struct instantiation, `enum` hacks. (Hard).
*   **C++11:** `constexpr`, `static_assert`, `using`, Variadic Templates. (Better).
*   **C++14:** Variable templates, `auto` return type. (Cleaner).
*   **C++17:** `if constexpr`, Fold expressions, `void_t`. (Powerful).
*   **C++20:** Concepts (`requires`). (The Holy Grail).

## 2. SFINAE (Substitution Failure Is Not An Error)

Before Concepts, SFINAE was the only way to constrain templates.

### 2.1 `std::enable_if`

```cpp
#include <type_traits>
#include <iostream>

// Enable only for integral types
template <typename T>
typename std::enable_if<std::is_integral<T>::value, void>::type
process(T t) {
    std::cout << "Integral: " << t << "\n";
}

// Enable only for floating point
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, void>::type
process(T t) {
    std::cout << "Float: " << t << "\n";
}
```

### 2.2 The `void_t` Trick (C++17)

Detecting if a type has a member function.

```cpp
template <typename T, typename = void>
struct has_print : std::false_type {};

template <typename T>
struct has_print<T, std::void_t<decltype(std::declval<T>().print())>> : std::true_type {};

static_assert(has_print<MyClass>::value, "MyClass must have print()");
```

## 3. Curiously Recurring Template Pattern (CRTP)

Static polymorphism. The base class knows the derived class type at compile time.

```cpp
template <typename Derived>
class Base {
public:
    void interface() {
        // Compile-time dispatch
        static_cast<Derived*>(this)->implementation();
    }
};

class Derived : public Base<Derived> {
public:
    void implementation() {
        std::cout << "Derived impl\n";
    }
};
```

**Use Case:** Mixins, adding functionality (like equality operators) without virtual overhead.

## 4. Policy-Based Design

Designing classes that take "policies" (strategy classes) as template arguments to define behavior.

```cpp
template <typename OutputPolicy, typename LanguagePolicy>
class HelloWorld : public OutputPolicy, public LanguagePolicy {
public:
    void run() {
        print(message()); // OutputPolicy::print, LanguagePolicy::message
    }
};
```

## 5. Modern TMP with Concepts (C++20)

Replacing SFINAE with readable constraints.

```cpp
template<typename T>
concept Printable = requires(T t) {
    { t.print() } -> std::same_as<void>;
};

void process(Printable auto& obj) {
    obj.print();
}
```

