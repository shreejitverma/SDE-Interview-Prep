# Chapter 23: C++17 Core Language Features

# C++17 CORE LANGUAGE UPGRADES

## 1. Structured Bindings

Unpack tuples, pairs, structs, and arrays directly into named variables.

### 1.1 Unpacking Tuples and Pairs

```cpp
#include <tuple>
#include <iostream>

std::tuple<int, double, std::string> get_data() {
    return {1, 3.14, "hello"};
}

int main() {
    // Old way (C++11/14)
    // int i; double d; std::string s;
    // std::tie(i, d, s) = get_data();

    // C++17 Structured Binding
    auto [id, val, name] = get_data();

    std::cout << id << ", " << val << ", " << name << "\n";
}
```

### 1.2 Unpacking Structs

```cpp
struct Point {
    int x, y;
};

Point p = {10, 20};
auto [px, py] = p; // px = 10, py = 20
```

### 1.3 Modifiers (`const`, `&`)

```cpp
std::pair<int, int> p = {1, 2};

auto& [refX, refY] = p; // References to p.first, p.second
refX = 10; // Modifies p

const auto& [cRefX, cRefY] = p; // Const references
```

## 2. `if constexpr`

Compile-time branching. Discards the non-taken branch at compile time, allowing instantiation of templates that would otherwise fail.

```cpp
#include <type_traits>

template<typename T>
auto get_value(T t) {
    if constexpr (std::is_pointer_v<T>) {
        return *t; // Only compiled if T is a pointer
    } else {
        return t;  // Only compiled if T is NOT a pointer
    }
}

int main() {
    int x = 5;
    int* ptr = &x;

    get_value(x);   // Returns int
    get_value(ptr); // Returns int (dereferenced)
}
```

## 3. Init-Statements for `if` and `switch`

Limit the scope of variables to the `if` or `switch` block.

```cpp
// Old way
{
    auto it = map.find(key);
    if (it != map.end()) {
        // use it
    }
} // it leaks scope or requires extra braces

// C++17 way
if (auto it = map.find(key); it != map.end()) {
    // use it
    std::cout << it->second;
} // it destroyed here

// Switch example
switch (auto status = get_status(); status) {
    case OK: break;
    case ERROR: break;
}
```

## 4. Inline Variables

Allows defining variables in headers without "multiple definition" errors. Replaces the `static` member workaround.

```cpp
#pragma once

// C++14: Linker error if included in multiple .cpp files
// int global_config = 5;

// C++17: Safe! The linker merges all definitions.
inline int global_config = 5;

struct MyClass {
    // Static member initialization in-class!
    static inline double tolerance = 0.001;
};
```

## 5. Nested Namespaces

Simplified syntax for deep namespace nesting.

```cpp
// Old
namespace A {
    namespace B {
        namespace C {
            // ...
        }
    }
}

// C++17
namespace A::B::C {
    // ...
}
```

## 6. Attributes

Standardized attributes to hint compiler behavior.

*   `[[nodiscard]]`: Warn if return value is ignored.
```cpp
    [[nodiscard]] int calculate_important_value();
```
*   `[[maybe_unused]]`: Suppress "unused variable" warnings.
```cpp
    [[maybe_unused]] int x = 5;
```
*   `[[fallthrough]]`: Suppress "implicit fallthrough" warnings in switch cases.
```cpp
    switch (device_state) {
        case State::INIT:
            initialize_device();
            [[fallthrough]]; // Directs compiler that fallthrough is intentional
        case State::RUNNING:
            run_process();
            break;
    }
```

