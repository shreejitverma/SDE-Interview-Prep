# CHAPTER 18: C14 TEMPLATES AND METAPROGRAMMING


# C++14 TEMPLATES & METAPROGRAMMING

C++14 simplified template metaprogramming (TMP) by introducing variable templates and utility classes that replaced complex, recursive boilerplate with cleaner, more intuitive syntax.

## 1. Variable Templates

Before C++14, if you wanted a templated constant (like `pi`), you had to wrap it in a `struct` or a `constexpr` function. Variable templates allow direct templating of variables.

### 1.1 Mathematical Constants and Type Traits
This feature is heavily used in the standard library and mathematical libraries.

```cpp
template<typename T>
constexpr T pi = T(3.1415926535897932385L);

// Usage
float  f_pi = pi<float>;
double d_pi = pi<double>;

// Type traits (Internal simplification)
template <typename T>
constexpr bool is_floating_point_v = std::is_floating_point<T>::value;
```

### 1.2 Professional Note: `_v` Suffixes
C++14 (and later C++17) introduced `_v` aliases for most type traits. Instead of writing `std::is_integral<T>::value`, you can write `std::is_integral_v<T>`. This reduces noise in complex template expressions.

## 2. `std::integer_sequence` & The Indices Trick

Meta-programming often involves working with variadic templates and tuples. `std::integer_sequence` provides a way to generate a sequence of integers at compile-time.

### 2.1 Unpacking a Tuple
The "Indices Trick" is the classic use case: converting a `std::tuple` into a pack of arguments for a function.

```cpp
template<typename F, typename Tuple, std::size_t... I>
auto apply_impl(F f, Tuple&& t, std::index_sequence<I...>) {
    return f(std::get<I>(std::forward<Tuple>(t))...);
}

template<typename F, typename Tuple>
auto apply(F f, Tuple&& t) {
    using Indices = std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>;
    return apply_impl(f, std::forward<Tuple>(t), Indices{});
}

// Result: apply(func, make_tuple(1, 2)) calls func(1, 2)
```

**Godhood Insight:** `std::index_sequence` (an alias for `std::integer_sequence<size_t, ...>`) is the glue that connects the "Value World" (Tuples) to the "Pack World" (Variadic Templates).

## 3. Alias Templates and `_t` Suffixes

C++14 introduced alias templates for all type traits in `<type_traits>`.

### 3.1 Reducing `typename ...::type` Boilerplate
In C++11, using a trait that returns a type required the `typename` keyword and the `::type` suffix. C++14 added `_t` versions.

```cpp
// C++11 (Verbosely painful)
typename std::enable_if<Condition, T>::type

// C++14 (Clean and readable)
std::enable_if_t<Condition, T>

// Example: SFINAE made easy
template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
void only_integers(T val) { /* ... */ }
```

**Professional Note:** Always prefer the `_t` and `_v` versions in modern C++. They are not just shorter; they are conceptually cleaner because they treat the trait as a function that returns a type/value directly.
