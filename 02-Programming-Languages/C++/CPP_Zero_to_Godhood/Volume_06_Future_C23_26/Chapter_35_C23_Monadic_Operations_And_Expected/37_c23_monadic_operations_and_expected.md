# Chapter 37: Monadic Operations & std::expected

# C++23 FUNCTIONAL ERROR HANDLING

### 1. `std::expected`

`std::expected<T, E>` is the modern way to return either a valid result (`T`) or an error (`E`). It is vastly superior to `std::optional` (which hides the error reason) and exceptions (which have unpredictable control flow overhead).

```cpp
#include <expected>

enum class Error { NotFound, PermissionDenied };

std::expected<std::string, Error> read_file(int id) {
    if (id < 0) return std::unexpected(Error::NotFound);
    return "File Content";
}
```

### 2. Monadic Operations

C++23 introduces functional monadic chaining for both `std::optional` and `std::expected`, eliminating the "Pyramid of Doom" (nested `if` checks).

*   **`and_then`**: Called if the object contains a value. Must return another optional/expected.
*   **`transform`**: Called if the object contains a value. Can return a raw value (auto-wrapped).
*   **`or_else`**: Called if the object contains an error/is empty.

```cpp
std::optional<User> get_user();
std::optional<std::string> get_email(const User&);

auto email = get_user()
    .and_then(get_email)
    .transform([](auto e){ return e + " verified"; })
    .or_else([]{ return std::optional{"Unknown"}; });
```

