# C++20 RANGES

## 1. The End of Iterator pairs

Ranges allow operating on a "range" (something with a begin and end) directly, composing algorithms using pipe syntax (`|`).

```cpp
#include <ranges>
#include <vector>
#include <iostream>
#include <algorithm>

namespace views = std::views;
namespace ranges = std::ranges;

int main() {
    std::vector<int> nums = {1, 2, 3, 4, 5, 6};

    // Old Way
    // std::vector<int> temp;
    // std::copy_if(nums.begin(), nums.end(), std::back_inserter(temp), [](int i){ return i % 2 == 0; });
    // for(auto& x : temp) x *= x;

    // C++20 Ranges
    auto result = nums 
        | views::filter([](int i){ return i % 2 == 0; }) // Keep evens
        | views::transform([](int i){ return i * i; });  // Square them
        
    for (int i : result) {
        std::cout << i << " "; // 4 16 36
    }
}
```

## 2. Views vs Actions

*   **Views:** Lazy. They adapt the iteration logic but don't own data. Computation happens *during* iteration. (e.g., `views::filter`, `views::reverse`).
*   **Actions:** Eager. They modify the container immediately. (Not fully standardized in C++20, mostly views).

## 3. Projections

Algorithms now accept a "projection" to transform data before comparison.

```cpp
struct User { int id; std::string name; };
std::vector<User> users = {{2, "Bob"}, {1, "Alice"}};

// Sort by ID
ranges::sort(users, {}, &User::id);

// Sort by Name
ranges::sort(users, {}, &User::name);
```

## 4. Dangling Iterators Protection

Ranges algorithms prevent returning iterators to temporary objects that would die immediately.

```cpp
auto get_vec() { return std::vector{1, 2, 3}; }

auto it = ranges::max_element(get_vec()); // Error! 
// Returns std::ranges::dangling instead of an invalid iterator.
```
