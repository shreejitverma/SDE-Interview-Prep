# CHAPTER 35: C23 CONTAINERS AND VIEWS


# C++23 DATA STRUCTURES

### 1. `std::mdspan`
A non-owning multidimensional view over contiguous memory. It is the cornerstone for modern C++ linear algebra and scientific computing, allowing you to treat a flat `std::vector` as a 2D, 3D, or ND matrix.
```cpp
#include <mdspan>
#include <vector>

std::vector<int> data = {1,2,3,4,5,6};
// View data as a 2x3 matrix
std::mdspan matrix(data.data(), 2, 3);

matrix[1, 2] = 42; // Uses C++23 multidimensional subscript
```

### 2. `std::flat_map` and `std::flat_set`
Node-based containers (`std::map`, `std::set`) have terrible cache locality. Flat containers provide the same API but are backed by contiguous `std::vector`s, meaning binary search lookup is highly optimized for the CPU cache.
```cpp
#include <flat_map>

std::flat_map<int, std::string> cache_friendly_map;
cache_friendly_map[1] = "A"; // O(N) insert, but O(log N) cache-friendly lookup
```

### 3. New Range Adaptors
C++23 dramatically expands the `<ranges>` library.
*   **`views::enumerate`**: Python-like index + value iteration.
    ```cpp
    for (auto [index, value] : std::views::enumerate(vec)) { ... }
    ```
*   **`views::zip`**: Iterate over multiple ranges simultaneously.
*   **`views::chunk` / `views::slide`**: Process ranges in blocks or sliding windows.
