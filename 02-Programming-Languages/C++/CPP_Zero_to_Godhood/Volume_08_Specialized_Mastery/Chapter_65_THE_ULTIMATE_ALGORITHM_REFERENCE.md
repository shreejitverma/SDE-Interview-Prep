# THE ULTIMATE ALGORITHM REFERENCE


Stop writing loops. Use the STL.

### 27.1 Non-Modifying Sequence Operations
*   `std::all_of(begin, end, pred)`: True if all match.
*   `std::any_of(begin, end, pred)`: True if any match.
*   `std::none_of(begin, end, pred)`: True if none match.
*   `std::for_each(begin, end, func)`: Apply function to all.
*   `std::count(begin, end, val)`: Count occurrences.
*   `std::mismatch(b1, e1, b2)`: Find first difference.
*   `std::find(begin, end, val)`: Linear search.

### 27.2 Modifying Sequence Operations
*   `std::copy(b, e, out)`: Copy range.
*   `std::transform(b, e, out, op)`: Map function over range.
*   `std::generate(b, e, gen)`: Fill with generator.
*   `std::remove_if(b, e, pred)`: **Erase-Remove Idiom** step 1. Move valid elements to front.
*   `std::replace(b, e, old, new)`: Replace values.
*   `std::unique(b, e)`: Remove consecutive duplicates.

### 27.3 Partitioning
*   `std::partition(b, e, pred)`: Reorder so true predicates come first. O(N).
*   `std::stable_partition`: Preserves relative order.

### 27.4 Sorting
*   `std::sort(b, e)`: IntroSort (Quick + Heap + Insertion). O(N log N).
*   `std::partial_sort(b, mid, e)`: Top K elements sorted.
*   `std::nth_element(b, nth, e)`: Element at `nth` is what it would be if sorted. O(N).

### 27.5 Binary Search (On Sorted Ranges)
*   `std::lower_bound(b, e, val)`: First element `>=` val.
*   `std::upper_bound(b, e, val)`: First element `>` val.
*   `std::binary_search(b, e, val)`: True/False existence.

### 27.6 Numeric Operations (<numeric>)
*   `std::iota(b, e, start)`: Fill with 0, 1, 2...
*   `std::accumulate(b, e, init)`: Sum (fold left).
*   `std::reduce(b, e)`: Parallelizable sum (C++17).
*   `std::inner_product`: Dot product.

---
