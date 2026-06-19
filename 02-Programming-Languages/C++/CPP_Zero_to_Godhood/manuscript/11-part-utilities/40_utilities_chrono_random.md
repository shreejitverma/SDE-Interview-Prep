# Part XI: Standard Utilities

*The tools you need to build the tools you want.*

# Chapter 40: Utilities, Chrono, and Random

> *Don't reinvent the wheel. Just `#include` it.*

The C++ Standard Library is primarily famous for its Containers (`std::vector`, `std::map`) and its Algorithms (`std::sort`, `std::find`). But hidden inside headers like `<utility>`, `<tuple>`, `<chrono>`, and `<random>` are essential building blocks that prevent you from writing tedious, bug-prone boilerplate.

---

## 40.1 Pairs and Tuples

Before C++11, if you wanted a function to return two values, you either had to pass parameters by reference to modify them, or you had to define a throwaway `struct`. 
`std::pair` and `std::tuple` solve this.

### `std::pair`
A `std::pair` stores exactly two heterogeneous values. It is famously used by `std::map`, which stores key-value pairs.
```cpp
#include <utility>

std::pair<int, std::string> get_user() {
    return {42, "Godhood"}; // C++11 Uniform Initialization
}

auto user = get_user();
std::cout << "ID: " << user.first << " Name: " << user.second;
```

### `std::tuple`
If you need more than two values, use a `std::tuple` (introduced in C++11).
```cpp
#include <tuple>

std::tuple<int, std::string, double> get_data() {
    return {1, "Alice", 99.9};
}

auto data = get_data();
// Accessing tuples requires compile-time indices
std::cout << std::get<1>(data); // Prints "Alice"
```

**Structured Binding (C++17):**
Accessing tuples via `std::get` is ugly. C++17 fixed this with Structured Bindings, which unpacks pairs, tuples, and structs instantly into named variables.
```cpp
auto [id, name, score] = get_data();
std::cout << name; // Prints "Alice"
```

## 40.2 Vocabulary Types (C++17)

Historically, C++ had a major problem expressing "nothing." If a function `find_user()` failed, what did it return? A `nullptr`? `-1`? An empty string? 

C++17 introduced three "Vocabulary Types" to standardize these concepts.

### 1. `std::optional` (The "Maybe" Type)
Replaces `nullptr` and magic values. It either holds a value, or it holds `std::nullopt`.
```cpp
#include <optional>

std::optional<int> divide(int a, int b) {
    if (b == 0) return std::nullopt;
    return a / b;
}

auto result = divide(10, 2);
if (result.has_value()) {
    std::cout << result.value(); // Safe
}
// Or, provide a default fallback:
std::cout << result.value_or(0);
```

### 2. `std::variant` (The Type-Safe Union)
A C-style `union` can hold different types of data in the same memory location, but it doesn't remember *which* type is currently active. If you read it wrong, your program crashes.
`std::variant` is a type-safe union that always knows what it holds.
```cpp
#include <variant>

// Can hold an int OR a float OR a string
std::variant<int, float, std::string> v = "Hello";

if (std::holds_alternative<std::string>(v)) {
    std::cout << std::get<std::string>(v);
}
```

### 3. `std::any` (The Type-Safe `void*`)
Can hold *anything*.
```cpp
#include <any>

std::any a = 42;
a = std::string("Now I am a string");

std::cout << std::any_cast<std::string>(a);
```

## 40.3 Time with `<chrono>`

Before C++11, measuring time required using the C-style `<time.h>`, which was notoriously platform-dependent and unsafe.

`<chrono>` is a completely type-safe library. If you try to add `seconds` to `milliseconds` and assign it to `hours`, the compiler will automatically handle the math, or throw an error if the conversion loses precision.

### Clocks, Time Points, and Durations
*   **`std::chrono::system_clock`**: The wall-clock time. Can be adjusted by the user or NTP (Network Time Protocol). Do not use this for measuring performance!
*   **`std::chrono::steady_clock`**: A clock that only ever moves forward. Guaranteed never to be adjusted. Perfect for benchmarking.

```cpp
#include <chrono>
#include <iostream>

using namespace std::chrono; // Allows literals like 5s, 10ms

int main() {
    auto start = steady_clock::now();
    
    // Do heavy work...
    
    auto end = steady_clock::now();
    
    // Type-safe subtraction yields a Duration
    auto diff = end - start;
    
    // Cast to milliseconds
    std::cout << "Took: " << duration_cast<milliseconds>(diff).count() << "ms\n";
}
```

### C++20 Calendars and Timezones
C++20 expanded `<chrono>` to handle dates and timezones flawlessly, including leap years and daylight saving time.
```cpp
using namespace std::chrono;
year_month_day date = 2026y / June / 18d; // Type-safe date creation!
```

## 40.4 Random Numbers with `<random>`

For decades, C++ programmers used `rand()` and `srand(time(NULL))`.
**Do not use `rand()`.** It is mathematically flawed, predictable, not thread-safe, and generates terrible statistical distributions.

C++11 introduced `<random>`. It splits random number generation into two parts:
1.  **The Engine**: Generates the raw, random bits. (Usually `std::mt19937`, the Mersenne Twister).
2.  **The Distribution**: Shapes the raw bits into a statistical shape (Uniform, Normal, Poisson, etc.) within a specific range.

```cpp
#include <random>

// 1. Get true entropy from the OS to seed the engine
std::random_device rd; 

// 2. Initialize the Engine (Mersenne Twister)
std::mt19937 gen(rd()); 

// 3. Define the Distribution (A fair 6-sided die)
std::uniform_int_distribution<int> dist(1, 6);

// 4. Generate the number
int roll = dist(gen);
```

---

With these utilities in hand, we are finally ready to dive into the most complex and powerful patterns C++ has to offer. We move to the culmination of our template knowledge: **Part XII: Advanced Systems and Meta-Programming**.
