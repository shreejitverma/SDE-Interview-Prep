# C++11 METAPROGRAMMING


C++11 made Template Metaprogramming (TMP) usable by mere mortals.

---

## 1. VARIADIC TEMPLATES

Templates that accept an arbitrary number of arguments.

```cpp
template<typename T>
T sum(T t) { return t; }

template<typename T, typename... Args>
T sum(T t, Args... args) {
    return t + sum(args...);
}

// sum(1, 2, 3, 4) -> 10
```

---

## 2. TYPE TRAITS

The `<type_traits>` header allows compile-time inspection of types.

```cpp
#include <type_traits>

static_assert(std::is_integral<int>::value, "Int must be integral");
static_assert(std::is_pointer<int*>::value, "Must be pointer");
```

Used heavily with **SFINAE** (`std::enable_if`) to restrict templates.

---

## 3. CONSTEXPR (INTRODUCTION)

`constexpr` functions can be evaluated at compile-time.

```cpp
constexpr int square(int x) { return x * x; }

int array[square(5)]; // Valid! Size 25 at compile time.
```

In C++11, `constexpr` functions were very limited (single return statement). C++14 relaxed this.

