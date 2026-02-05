# C++23 CONTAINERS & VIEWS

## 1. `std::mdspan`

A non-owning, multidimensional view of a contiguous memory block. Crucial for scientific computing, linear algebra, and image processing.

### 1.1 Basics

```cpp
#include <mdspan>
#include <vector>

int main() {
    std::vector<int> data(12); // 3x4 matrix
    std::mdspan m(data.data(), 3, 4);
    
    m[1, 2] = 42; // Set row 1, col 2
}
```

### 1.2 Layouts

*   `std::layout_right` (Row-Major): C/C++ default. Last index varies fastest.
*   `std::layout_left` (Column-Major): Fortran/BLAS compatible. First index varies fastest.

## 2. `std::flat_map` and `std::flat_set`

Associative containers implemented as sorted vectors.

*   **Pros:** contiguous memory, cache-friendly, faster iteration/lookup for small-to-medium datasets.
*   **Cons:** O(N) insertion/deletion (vs O(log N) for `std::map`). Iterators invalidated on insertion.

```cpp
#include <flat_map>

std::flat_map<int, std::string> m;
m[1] = "One"; // Insert
```

## 3. Ranges Enhancements

*   `std::ranges::to`: Convert a range to a container.
    ```cpp
    auto v = some_view | std::ranges::to<std::vector>();
    ```
*   `views::zip`: Iterate multiple ranges in lockstep.
    ```cpp
    std::vector<int> a = {1, 2}, b = {3, 4};
    for (auto [x, y] : std::views::zip(a, b)) {
        // x=1, y=3 ...
    }
    ```
*   `views::enumerate`: Index + Value.
    ```cpp
    for (auto [idx, val] : std::views::enumerate(data)) {
        // ...
    }
    ```
