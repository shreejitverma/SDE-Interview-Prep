# C++14 TEMPLATES & METAPROGRAMMING

## 1. Variable Templates

Templates for variables, not just functions or classes.

```cpp
template<typename T>
constexpr T pi = T(3.1415926535897932385);

int main() {
    float f = pi<float>;
    double d = pi<double>;
}
```

## 2. `decltype(auto)`

Deduces type exactly as `decltype` would, but without repeating the expression.

*   `auto`: Deduces type (decaying references).
*   `decltype(auto)`: Deduces type and value category (preserves references).

```cpp
int x = 5;
int& ref = x;

auto a = ref;           // int
decltype(auto) b = ref; // int&
```

## 3. Standard Library Metafunctions

*   `std::integer_sequence`
*   `std::index_sequence`
*   `std::make_index_sequence`

Useful for unpacking tuples or variadic templates.

```cpp
template<typename Tuple, size_t... Is>
void print_tuple(const Tuple& t, std::index_sequence<Is...>) {
    ((std::cout << std::get<Is>(t) << " "), ...);
}
```
