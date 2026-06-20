# CHAPTER 40: COMPILE TIME PROGRAMMING


# COMPILE-TIME PROGRAMMING

## 1. `constexpr` Evolution

*   **C++11:** Single return statement. Very restricted.
*   **C++14:** Loops, local variables, multiple returns allowed.
*   **C++17:** `if constexpr`, lambdas can be constexpr.
*   **C++20:** Virtual functions, `try-catch`, dynamic allocation (`std::vector`, `std::string`) allowed in constexpr context (transient allocation).

## 2. `consteval` (C++20)

Forces immediate execution. If it can't run at compile time, it's an error.

```cpp
consteval int square(int n) { return n * n; }

int x = square(5); // OK
int y = 10;
// int z = square(y); // Error: y is not a constant expression
```

## 3. Type Traits (`<type_traits>`)

The building blocks of metaprogramming.

### 3.1 Queries
*   `is_integral_v<T>`
*   `is_same_v<T, U>`
*   `is_base_of_v<Base, Derived>`

### 3.2 Transformations
*   `remove_const_t<T>`
*   `decay_t<T>` (Arrays -> pointers, functions -> pointers, remove cv-ref)
*   `conditional_t<Bool, T, F>` (Compile time IF)

## 4. `std::integral_constant`

Wraps a compile-time value as a type.

```cpp
using Two = std::integral_constant<int, 2>;
using Four = std::integral_constant<int, 4>;

static_assert(Two::value + Two::value == Four::value);
```

## 5. Reflection (C++26 Preview)

Currently, we use libraries like `magic_enum` or macro hacks. C++26 brings static reflection.

```cpp
// C++26 Syntax (Proposed)
// constexpr auto info = ^MyClass;
// for (auto member : info.data_members()) ...
```
