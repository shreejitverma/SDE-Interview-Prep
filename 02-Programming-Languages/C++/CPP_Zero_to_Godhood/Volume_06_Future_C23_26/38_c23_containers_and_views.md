# Chapter 38: Containers & Views (mdspan)

# C++23 DATA STRUCTURES & RANGES

### 1. `std::mdspan`
A multidimensional non-owning span with a layout policy. The `operator[i, j]` from C++23 is directly used here. It is the cornerstone for modern linear algebra.
```cpp
#include <mdspan>
std::vector<int> v(12);
auto view = std::mdspan(v.data(), 3, 4);
view[1, 2] = 99;
```

### 2. Flat Containers
*   **std::flat_map / std::flat_set**: Sorted associative containers backed by contiguous storage (vectors). They offer drastically better cache performance for read-heavy workloads compared to tree-based maps.
    ```cpp
    #include <flat_map>
    std::flat_map<std::string, int> m; 
    m["a"] = 1;
    ```

### 3. Extensive Range Updates
C++23 dramatically expands the `<ranges>` library with new views and algorithms.
*   **std::ranges::to**: Converts any range into a specified container type, with optional nesting.
    ```cpp
    auto v = std::views::iota(0, 5) | std::ranges::to<std::vector>();
    ```
*   **New Views**: 
    *   `views::enumerate`: Yields index/value pairs.
    *   `views::zip`: Iterate over multiple ranges simultaneously.
    *   `views::chunk` / `views::slide`: Process ranges in blocks or sliding windows.
    *   *Others*: `adjacent`, `cartesian_product`, `join_with`, `repeat`, `stride`.
    ```cpp
    for(auto [i, x] : std::views::enumerate(v)){ std::println("{}: {}", i, x); }
    ```
*   **New Algorithms**: `fold_left`, `fold_right`, `contains`, `find_last`, `starts_with`.
    ```cpp
    auto sum = std::ranges::fold_left(v, 0, std::plus{});
    ```

