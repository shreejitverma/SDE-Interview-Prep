# TEMPLATE METAPROGRAMMING & POWER FEATURES

## 1. Variadic Templates

Templates that accept an arbitrary number of parameters.

```cpp
// Base case (recursion terminator)
void print() {}

// Recursive step
template<typename T, typename... Args>
void print(T first, Args... args) {
    std::cout << first << " ";
    print(args...); // Unpack
}

int main() {
    print(1, "hello", 3.14);
}
```

### 1.1 Parameter Packing (`...`)
*   `typename... Args`: Template parameter pack.
*   `Args... args`: Function parameter pack.
*   `sizeof...(Args)`: Number of arguments.

---

## 2. Type Traits

Compile-time type inspection and modification.

```cpp
#include <type_traits>

static_assert(std::is_integral<int>::value, "Must be int");
static_assert(std::is_pointer<int*>::value, "Must be ptr");

// Conditional compilation (SFINAE)
template<typename T>
typename std::enable_if<std::is_integral<T>::value>::type
process(T x) {
    // Only compiles for integers
}
```

---

## 3. `using` Aliases

Replaces `typedef`, works with templates.

```cpp
template<typename T>
using Dictionary = std::map<std::string, T>;

Dictionary<int> scores;
```

---

## 4. `std::tuple`

Generalization of `std::pair` to N elements.

```cpp
#include <tuple>

std::tuple<int, double, std::string> t(1, 3.14, "hi");
int i = std::get<0>(t);
```
