# CHAPTER 34: C23 MONADIC OPERATIONS AND EXPECTED


# C++23 MONADIC OPERATIONS & EXPECTED

## 1. `std::expected`

The standard way to return values or errors, replacing integer error codes and exceptions in high-performance paths.

### 1.1 Basics

```cpp
#include <expected>
#include <string>

enum class Error { InvalidInput, ConnectionFailed };

std::expected<int, Error> parse_int(std::string_view input) {
    if (input.empty()) return std::unexpected(Error::InvalidInput);
    return std::stoi(std::string(input));
}

int main() {
    auto result = parse_int("42");

    if (result) {
        std::cout << "Value: " << *result;
    } else {
        std::cout << "Error: " << (int)result.error();
    }
}
```

### 1.2 Monadic Chain

`and_then`, `transform`, `or_else`.

```cpp
auto process = parse_int("42")
    .and_then([](int i) { return parse_int("10"); }) // Chain potentially failing op
    .transform([](int i) { return i * 2; })          // Transform value on success
    .or_else([](Error e) { return std::expected<int, Error>(0); }); // Handle error
```

## 2. Monadic Operations for `std::optional`

C++23 adds monadic methods to `std::optional` (C++17).

```cpp
std::optional<int> get_id();
std::optional<std::string> get_name(int id);

auto name = get_id()
    .and_then(get_name)
    .value_or("Unknown");
```
