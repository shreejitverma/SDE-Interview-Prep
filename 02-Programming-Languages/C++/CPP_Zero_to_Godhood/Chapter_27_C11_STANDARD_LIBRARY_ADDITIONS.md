# C++11 STANDARD LIBRARY ADDITIONS


C++11 massively expanded the STL.

---

## 1. UNORDERED CONTAINERS (HASH MAPS)

C++98 `std::map` is a tree (O(log n)). C++11 added hash tables (O(1) average).

- `std::unordered_map`
- `std::unordered_set`
- `std::unordered_multimap`
- `std::unordered_multiset`

```cpp
#include <unordered_map>
std::unordered_map<std::string, int> scores;
scores["Alice"] = 100; // O(1)
```

They require a Hash function for the key type.

---

## 2. STD::ARRAY

`std::array` is a fixed-size wrapper around C-style arrays. It doesn't decay to a pointer automatically and knows its size.

```cpp
#include <array>
std::array<int, 5> arr = {1, 2, 3, 4, 5};
// arr.size() is 5
```

Prefer over C-arrays (`int arr[5]`).

---

## 3. STD::TUPLE

Generalization of `std::pair` for N elements.

```cpp
#include <tuple>
auto t = std::make_tuple(10, "Hello", 3.14);
int i = std::get<0>(t);
```

---

## 4. REGULAR EXPRESSIONS

`std::regex` provides regex matching and replacement.

```cpp
#include <regex>
std::regex pattern(R"(\d+)"); // Matches digits
bool match = std::regex_match("123", pattern);
```

---

## 5. CHRONO (TIME)

Type-safe time library.

```cpp
#include <chrono>
auto start = std::chrono::high_resolution_clock::now();
// ... work ...
auto end = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
```

---

## 6. RANDOM

Better random number generation than `rand()`.

```cpp
#include <random>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(1, 6); // Dice roll
int roll = dis(gen);
```

