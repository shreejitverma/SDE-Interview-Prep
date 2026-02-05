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

Regular expression support.

```cpp
#include <regex>
std::regex r("(\w+)@(\w+)\.com");
```

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

