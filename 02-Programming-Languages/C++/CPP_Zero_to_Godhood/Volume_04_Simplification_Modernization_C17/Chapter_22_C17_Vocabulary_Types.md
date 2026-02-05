# C++17 VOCABULARY TYPES

These types provide standard ways to represent optionality, alternatives, and type-erased values, replacing many custom implementations.

## 1. `std::string_view`

A non-owning reference to a string (or substring). **Zero-copy** string operations.

### 1.1 Efficiency

```cpp
// Bad: Copies string (potentially expensive allocation)
void print_str(std::string s) {
    std::cout << s << "\n";
}

// Good: No copy, no allocation
void print_view(std::string_view sv) {
    std::cout << sv << "\n";
}

int main() {
    const char* cstr = "Hello World";
    // print_str(cstr); // Creates std::string (allocates!)
    print_view(cstr);   // No allocation
    
    std::string s = "Hello World";
    print_view(s);      // Works with std::string too
    
    // Substrings are cheap!
    std::string_view sub = std::string_view(cstr).substr(0, 5); 
    print_view(sub);
}
```

### 1.2 Caveats
*   **Non-owning:** Ensure the underlying string outlives the view.
*   **Not null-terminated:** Do not pass `.data()` to C APIs unless you are sure.

## 2. `std::optional`

Represents a value that may or may not be present. Replaces pointers for nullable values or "magic values" (-1, "").

```cpp
#include <optional>

std::optional<int> find_even(const std::vector<int>& v) {
    for (int x : v) {
        if (x % 2 == 0) return x;
    }
    return std::nullopt; // or {}
}

int main() {
    auto res = find_even({1, 3, 5});
    if (res) { // or res.has_value()
        std::cout << *res; // or res.value() (throws if empty)
    } else {
        std::cout << "Not found";
    }
    
    // Value or default
    std::cout << res.value_or(0); 
}
```

## 3. `std::variant`

A type-safe union. Can hold one of several distinct types.

```cpp
#include <variant>

std::variant<int, float, std::string> v;

v = 10;
v = 3.14f;
v = "hello";

// Accessing
try {
    std::string s = std::get<std::string>(v);
    // int i = std::get<int>(v); // Throws std::bad_variant_access
} catch (...) {}

// std::visit (The Visitor Pattern)
std::visit([](auto&& arg) {
    std::cout << arg << "\n";
}, v);
```

## 4. `std::any`

A type-safe container for *single* values of any type. (Like `void*` but safe).

```cpp
#include <any>

std::any a = 1;
a = std::string("hello");

try {
    std::string s = std::any_cast<std::string>(a);
} catch (const std::bad_any_cast& e) {
    std::cout << e.what();
}
```
