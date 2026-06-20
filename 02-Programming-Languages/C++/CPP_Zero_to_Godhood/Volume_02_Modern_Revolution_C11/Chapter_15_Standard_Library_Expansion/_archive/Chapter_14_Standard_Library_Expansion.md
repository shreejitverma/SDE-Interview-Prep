# THE C++11 STANDARD LIBRARY EXPANSION

## 1. Unordered Containers

Hash maps/sets. O(1) average access.

*   `std::unordered_map`
*   `std::unordered_set`
*   `std::unordered_multimap`
*   `std::unordered_multiset`

Requires a hash function for the key type.

## 2. `std::array`

Fixed-size, stack-allocated array. Wrapper around C-style array with STL interface.

```cpp
std::array<int, 5> arr = {1, 2, 3, 4, 5};
// Bounds checking available: arr.at(10) throws
// Knows its size: arr.size()
// Doesn't decay to pointer automatically
```

## 3. `std::regex`

Regular expression support for pattern matching and text manipulation.

```cpp
#include <regex>
#include <iostream>
#include <string>

int main() {
    std::string text = "Contact: user@example.com";
    std::regex email_regex("(\\w+)@(\\w+)\\.com");
    
    // 1. Search: Find first occurrence
    std::smatch matches;
    if (std::regex_search(text, matches, email_regex)) {
        std::cout << "Found: " << matches[0] << std::endl;
        std::cout << "User: " << matches[1] << std::endl;
    }
    
    // 2. Match: Must match entire string
    bool is_full_match = std::regex_match(text, email_regex); // false
    
    // 3. Replace:
    std::string new_text = std::regex_replace(text, email_regex, "REDACTED");
    
    return 0;
}
```

---
### Professional Notes: Regular Expressions

#### 1. Regex Grammar Flavors
`std::regex` supports different syntax standards. The default is **ECMAScript**.
*   `std::regex_constants::basic`: POSIX Basic Regular Expressions.
*   `std::regex_constants::extended`: POSIX Extended Regular Expressions.
*   `std::regex_constants::awk`, `grep`, `egrep`.

#### 2. Performance Trap: `std::regex` is Heavy
In many implementations (including GCC), `std::regex` is notoriously slow and can cause massive binary size increases due to complex template instantiations.
*   **Optimization**: Always pre-compile your regex objects outside of tight loops.
*   **Alternatives**: If performance is critical, consider `RE2` or `Boost.Regex`.

#### 3. Capture Groups and `smatch`
Capture groups allow you to extract specific sub-strings.
*   `matches[0]` is the entire match.
*   `matches[1], matches[2]...` are the captured sub-patterns.
*   Use `std::ssub_match` to get individual pieces without creating new strings immediately.

---

## 4. `std::chrono`

Type-safe time library.

*   **Clocks:** `system_clock`, `steady_clock`.
*   **Durations:** `milliseconds`, `seconds`.
*   **Time Points:** `time_point`.

```cpp
auto start = std::chrono::steady_clock::now();
// ...
auto end = std::chrono::steady_clock::now();
auto diff = end - start;
```

## 5. `std::random`

Better random number generation than `rand()`.

```cpp
#include <random>

std::random_device rd;
std::mt19937 gen(rd()); // Mersenne Twister
std::uniform_int_distribution<> dis(1, 6);

int roll = dis(gen);
```

