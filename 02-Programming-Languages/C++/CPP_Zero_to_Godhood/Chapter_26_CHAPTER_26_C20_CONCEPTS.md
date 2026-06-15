# CHAPTER 26: C20 CONCEPTS


# C++20 CONCEPTS

## 1. The Problem with Templates

Before C++20, template errors were notoriously verbose and hard to decipher ("template error explosion"). If you passed a type that didn't support a required operation (like `+` or `.begin()`), the error would occur deep inside the template implementation, often screens away from the actual call site.

## 2. What are Concepts?

Concepts are named sets of requirements for template arguments. They act as predicates that are evaluated at compile time.

### 2.1 Defining a Concept

```cpp
#include <concepts>

// A concept that checks if T is integral
template<typename T>
concept Integral = std::is_integral_v<T>;

// A concept that checks if T is addable
template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::convertible_to<T>;
};
```

### 2.2 Using Concepts (`requires` clause)

```cpp
template<typename T>
requires Integral<T>
T add(T a, T b) {
    return a + b;
}

// Shorthand syntax
template<Integral T>
T subtract(T a, T b) {
    return a - b;
}

// Terse syntax (C++20 style)
auto multiply(Integral auto a, Integral auto b) {
    return a * b;
}
```

## 3. The `requires` Expression

The core mechanism for defining ad-hoc constraints.

```cpp
template<typename T>
concept Printable = requires(T x) {
    std::cout << x; // Expression must be valid
};

template<typename T>
concept Container = requires(T c) {
    typename T::value_type;
    { c.size() } -> std::same_as<size_t>;
    { c.begin() };
    { c.end() };
};
```

## 4. Standard Concepts (`<concepts>`)

C++20 provides a rich library of predefined concepts.

*   **Core:** `std::same_as`, `std::derived_from`, `std::convertible_to`
*   **Arithmetic:** `std::integral`, `std::floating_point`, `std::signed_integral`
*   **Object:** `std::movable`, `std::copyable`, `std::regular`
*   **Callable:** `std::invocable`, `std::predicate`

## 5. Constraint Based Overloading

Concepts allow overloading function templates based on properties of types (replacing SFINAE/`enable_if`).

```cpp
void print(std::integral auto x) {
    std::cout << "Integer: " << x << "\n";
}

void print(std::floating_point auto x) {
    std::cout << "Float: " << x << "\n";
}

print(5);   // Calls integral version
print(3.14); // Calls floating_point version
```
