# Chapter 16: Algorithms

> *Why write loops when the compiler can do it better?*

The true power of the STL does not lie in its Containers. It lies in the `<algorithm>` header. 

For decades, programmers wrote manual `for` loops to find elements, count occurrences, or copy arrays. The creators of C++ realized that 90% of all loops fall into a few dozen mathematical patterns. 

By standardizing these patterns into Algorithms, C++ achieved three things:
1.  **Readability**: `std::count(begin, end, 5)` is instantly understandable. A 6-line `for` loop requires you to read and mentally simulate the code.
2.  **Correctness**: Manual loops are prone to "Off-by-One" errors. STL algorithms are mathematically proven to be correct.
3.  **Performance**: STL algorithms are heavily optimized for the hardware, often using vectorization (SIMD) under the hood to process multiple elements per CPU cycle.

**The Rule of Godhood:** If you are writing a `for` loop, stop. Ask yourself: *"Is there an STL algorithm that does this?"* Usually, the answer is yes.

---

## 16.1 The Algorithm Philosophy: `[first, last)`

Every STL algorithm operates on a range of elements defined by two Iterators. 

These ranges are always **Half-Open**, written mathematically as `[first, last)`.
This means the algorithm will process the element at `first`, and continue until it reaches `last`, but it will **NOT** process the element at `last`.

Why half-open?
1.  **Empty Ranges are Safe**: If `first == last`, the range is inherently empty. The loop `while (first != last)` will simply never execute, preventing crashes.
2.  **Distance is Easy**: The number of elements is exactly `last - first`.

```cpp
std::vector<int> v = {10, 20, 30};
// v.begin() points to 10
// v.end() points to the imaginary slot AFTER 30.
std::sort(v.begin(), v.end()); // Sorts the whole vector.
```

## 16.2 Non-Modifying Algorithms

These algorithms look at your data but never change it.

*   `std::find(begin, end, value)`: Returns an iterator to the first occurrence of `value`. If not found, it returns the `end` iterator.
*   `std::count(begin, end, value)`: Returns the number of times `value` appears.
*   `std::all_of`, `std::any_of`, `std::none_of` (C++11): Checks if elements match a condition.

```cpp
#include <algorithm>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v = {1, 5, 9, 5, 3};

    // How many 5s?
    int fives = std::count(v.begin(), v.end(), 5); // Returns 2

    // Does it contain a 9?
    auto it = std::find(v.begin(), v.end(), 9);
    if (it != v.end()) {
        std::cout << "Found 9 at index " << std::distance(v.begin(), it) << "\n";
    }
}
```

## 16.3 Modifying Algorithms

These algorithms change the data within the container.

*   `std::copy(begin, end, destination_begin)`: Safely copies data.
*   `std::replace(begin, end, old_val, new_val)`: Swaps all `old_val` with `new_val`.
*   `std::reverse(begin, end)`: Flips the order of elements in place.

> [!WARNING]
> **The Destination Trap**
> Algorithms like `std::copy` do NOT allocate memory! If your destination container is empty, `std::copy` will overwrite unallocated memory and crash your program. You must ensure the destination is already the correct size, or use a `std::back_inserter`.

## 16.4 Sorting Algorithms

Sorting is arguably the most common algorithm in Computer Science.

*   `std::sort(begin, end)`: The workhorse. Usually implemented as **Introsort** (a hybrid of Quicksort, Heapsort, and Insertion Sort). It provides an average complexity of $O(N \log N)$ and is insanely fast.
*   `std::stable_sort(begin, end)`: Like `std::sort`, but guarantees that if two elements are equal, their original relative order is preserved. It is slightly slower and requires extra memory.
*   `std::partial_sort(begin, middle, end)`: Need to find the Top 10 players on a leaderboard of 1,000,000 users? Sorting the whole array is a waste of time. `partial_sort` only sorts the elements up to `middle`, leaving the rest unsorted.

```cpp
std::vector<int> v = {9, 2, 7, 1, 8, 3};

// Sort the whole thing: {1, 2, 3, 7, 8, 9}
std::sort(v.begin(), v.end());

// Sort descending by passing a custom comparison function (or lambda)
std::sort(v.begin(), v.end(), std::greater<int>()); 
```

## 16.5 Binary Search (On Sorted Ranges)

If your data is already sorted, searching it linearly with `std::find` is terribly inefficient ($O(N)$). You should use Binary Search ($O(\log N)$).

*   `std::binary_search(begin, end, val)`: Returns `true` if `val` exists, `false` otherwise.
*   `std::lower_bound(begin, end, val)`: Returns an iterator to the **first** element that is $\ge$ `val`.
*   `std::upper_bound(begin, end, val)`: Returns an iterator to the **first** element that is $>$ `val`.

> [!CAUTION]
> **The Unsorted Danger**
> Calling `std::binary_search` on an unsorted container results in **Undefined Behavior**. It might return false when the element exists, or it might crash. Always verify your range is sorted first.

## 16.6 Numeric Algorithms

Hidden away in the `<numeric>` header are algorithms specifically designed for math.

*   `std::accumulate(begin, end, initial_value)`: Adds up all the elements.
*   `std::inner_product`: Calculates the dot product of two ranges.
*   `std::iota` (C++11): Fills a range with sequentially increasing values (e.g., 1, 2, 3, 4, 5).

```cpp
#include <numeric>
#include <vector>

int main() {
    std::vector<int> v = {10, 20, 30};
    
    // Sum the vector, starting with an initial sum of 0
    int total = std::accumulate(v.begin(), v.end(), 0); // total is 60
}
```

---

You now possess the vocabulary of the STL. However, to truly unlock the power of algorithms like `std::sort` or `std::find_if`, you need to pass them custom logic. In the archaic days of C++98, this required writing clunky "Functor" classes. 

In C++11, the language was revolutionized by the introduction of inline functions. In the next chapter, we enter the modern era with **Lambdas and Functional Programming**.
