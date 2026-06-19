# Part XIV: Mastery

*The culmination of knowledge.*

# Chapter 49: The Ultimate Algorithm Reference

> *A C++ programmer who writes raw `for` loops is a C++ programmer who does not know the Standard Library.*

Throughout this book, we have emphasized the importance of `<algorithm>`. Sean Parent famously coined the phrase **"No Raw Loops"**. If you are writing a `for` loop, you are likely reinventing a wheel that already exists in the standard library, but yours is probably less efficient and more bug-prone.

This chapter serves as your Godhood cheat sheet for the C++ Standard Library Algorithms.

---

## 49.1 Querying Ranges

When you need to ask a question about a collection of data.

*   **`std::all_of` / `std::any_of` / `std::none_of`**
    Checks if elements match a predicate.
    ```cpp
    bool all_even = std::all_of(v.begin(), v.end(), [](int i){ return i % 2 == 0; });
    ```
*   **`std::count` / `std::count_if`**
    Counts occurrences.
*   **`std::find` / `std::find_if`**
    Finds the first element matching a value or predicate. Returns an iterator (or `v.end()` if not found).
*   **`std::binary_search`**
    Returns `true` if a sorted range contains a value. (O(log N)).
*   **`std::lower_bound` / `std::upper_bound`**
    Finds the insertion point in a sorted range. `lower_bound` is the most powerful tool in competitive programming.

## 49.2 Modifying Ranges

When you need to change the data in place.

*   **`std::copy` / `std::copy_if`**
    Copies elements from one range to another.
*   **`std::transform`**
    Applies a function to every element (the C++ equivalent of `map()` in JavaScript/Python).
    ```cpp
    std::transform(v.begin(), v.end(), v.begin(), [](int i) { return i * 2; });
    ```
*   **`std::generate`**
    Fills a range by repeatedly calling a function (e.g., a random number generator).
*   **`std::replace` / `std::replace_if`**
    Swaps specific values for new values.
*   **`std::remove` / `std::remove_if`**
    Pushes elements to the back of the vector and returns a new `end` iterator. *Must be followed by `v.erase()` (The Erase-Remove Idiom).*
    ```cpp
    // Erase all odd numbers:
    v.erase(std::remove_if(v.begin(), v.end(), [](int i){ return i % 2 != 0; }), v.end());
    ```

## 49.3 Sorting and Partitioning

When you need to reorder data.

*   **`std::sort`**
    O(N log N) IntroSort. Unstable (does not preserve original order of equal elements).
*   **`std::stable_sort`**
    Preserves original order of equal elements. Slightly slower.
*   **`std::partial_sort`**
    If you only need the Top 10 items in a list of 1,000,000, use this. It is significantly faster than sorting the whole list.
    ```cpp
    std::partial_sort(v.begin(), v.begin() + 10, v.end());
    ```
*   **`std::nth_element`**
    If you only need to find the Median, use this. It places the Nth element in its correct sorted position in O(N) time without fully sorting the array.
*   **`std::partition`**
    Moves all elements satisfying a predicate to the front of the range. (e.g., "Put all active users first, inactive users last").

## 49.4 Numeric Algorithms (`<numeric>`)

These algorithms live in a different header but are incredibly powerful.

*   **`std::accumulate`**
    Sums up a range (or "reduces" it using a custom operation like multiplication).
    ```cpp
    int sum = std::accumulate(v.begin(), v.end(), 0); 
    ```
*   **`std::reduce` (C++17)**
    The parallel-friendly version of `accumulate`.
*   **`std::inner_product`**
    Multiplies two arrays together element-by-element and sums the result (Dot Product).
*   **`std::iota`**
    Fills a range with sequentially increasing values (e.g., 0, 1, 2, 3...).

## 49.5 Set Operations

These require the input ranges to be **sorted**.

*   **`std::set_union`**: Combines two sorted ranges.
*   **`std::set_intersection`**: Finds elements present in both sorted ranges.
*   **`std::set_difference`**: Finds elements in the first range that are NOT in the second.

## 49.6 C++20 Ranges Recap

Remember that C++20 added `std::ranges`. Every algorithm listed above has a `ranges` equivalent that eliminates iterator boilerplate.

Instead of:
```cpp
std::sort(v.begin(), v.end());
```
You write:
```cpp
std::ranges::sort(v);
```

Furthermore, Ranges allow composition via Views:
```cpp
// Take the first 5 even numbers, multiply by 2
auto result = v | std::views::filter([](int i) { return i % 2 == 0; })
                | std::views::transform([](int i) { return i * 2; })
                | std::views::take(5);
```

---

With the algorithms mastered, you are no longer writing boilerplate; you are composing logic. 

There is only one thing left to do. It is time to prove your Godhood. We move to **Chapter 50: The Capstone Project**.
