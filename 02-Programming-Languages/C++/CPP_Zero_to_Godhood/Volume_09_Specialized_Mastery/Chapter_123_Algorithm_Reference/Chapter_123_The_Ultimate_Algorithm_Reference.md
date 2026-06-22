# Chapter 123: The Ultimate Algorithm Reference

The single highest-leverage habit in everyday C++ is also the simplest: **stop writing raw loops, and use the standard algorithms.** The `<algorithm>` and `<numeric>` headers encode decades of correct, optimised, well-named implementations of the operations you reach for constantly — and using them produces code that is shorter, less bug-prone, often faster, and instantly recognisable to any C++ engineer. This chapter is a structured reference to the standard algorithms, organised by what they do, with the cost model and the C++20 Ranges evolution that makes them composable.

## Chapter Roadmap

- 123.1 Why Algorithms Over Raw Loops
- 123.2 Non-Modifying Sequence Operations
- 123.3 Modifying Sequence Operations
- 123.4 Partitioning and Sorting
- 123.5 Binary Search on Sorted Ranges
- 123.6 Numeric Operations
- 123.7 Ranges: Composable Algorithms (C++20)

---

## 123.1 Why Algorithms Over Raw Loops

A hand-written loop is an opportunity for an off-by-one error, an unclear intent, and a missed optimization. A standard algorithm states its *intent* in its name (`std::find`, `std::transform`, `std::accumulate`), is implemented correctly once in the library, and is often better-optimised than a naive loop (vectorised, specialised for trivially-copyable types).

> **Why this matters.** "No raw loops" (Sean Parent's famous guideline) is the most actionable everyday-C++ advice there is. A loop that searches, copies, counts, or transforms hides its purpose behind mechanics; the named algorithm *is* the purpose, so the reader (and you, in six months) sees `std::any_of(v, is_valid)` instead of decoding a loop with a flag and a `break`. Beyond clarity, the library implementations are correct (no off-by-one, no iterator-invalidation bug) and optimised (the standard library's `std::copy` becomes `memmove` for trivially-copyable types; `std::reduce` can vectorise and parallelise). The cost is essentially zero — algorithms inline to the same or better code (Chapter 89) — so this is pure upside: more readable, more correct, often faster.

---

## 123.2 Non-Modifying Sequence Operations

These inspect a range without changing it:

| Algorithm | Effect | Complexity |
|---|---|---|
| `std::all_of(b, e, pred)` | true if *all* elements match | O(n) |
| `std::any_of(b, e, pred)` | true if *any* element matches | O(n) |
| `std::none_of(b, e, pred)` | true if *no* element matches | O(n) |
| `std::for_each(b, e, fn)` | apply `fn` to each element | O(n) |
| `std::count(b, e, val)` | number of elements equal to `val` | O(n) |
| `std::find(b, e, val)` | iterator to first `val` (linear search) | O(n) |
| `std::mismatch(b1, e1, b2)` | first position where two ranges differ | O(n) |

> **Why this matters.** These replace the most common loops with a single, intention-revealing call. `std::any_of(v.begin(), v.end(), is_error)` says "is there any error?" far more clearly than a loop with a flag. Note the complexity: `std::find` is *linear* (O(n)) because it scans — if the data is *sorted*, use binary search (§123.5) for O(log n) instead. Choosing the right algorithm requires knowing both what it does *and* its cost, which is why this reference pairs each with its complexity.

---

## 123.3 Modifying Sequence Operations

These transform a range or produce a transformed copy:

| Algorithm | Effect |
|---|---|
| `std::copy(b, e, out)` | copy a range to `out` |
| `std::transform(b, e, out, op)` | apply `op` to each element, write results to `out` (a map) |
| `std::generate(b, e, gen)` | fill a range by calling `gen()` |
| `std::remove_if(b, e, pred)` | move "kept" elements to the front; returns the new logical end |
| `std::replace(b, e, old, new)` | replace all `old` with `new` |
| `std::unique(b, e)` | collapse *consecutive* duplicates; returns the new logical end |

> **Why this matters.** Two of these embody a famous idiom and a famous trap. **`std::transform`** is `map` — the functional building block for applying a function across a collection. **`std::remove_if`** is the first half of the **erase-remove idiom**, and it is the classic beginner trap: `remove_if` does *not* erase anything (it cannot — algorithms operate on iterators, not containers); it *moves* the kept elements to the front and returns an iterator to the new end, leaving the tail in an unspecified state. You must follow it with `container.erase(remove_if(...), container.end())` to actually shrink the container — or, in C++20, use `std::erase_if(container, pred)` which does both. Likewise `std::unique` only removes *consecutive* duplicates, so the range must be *sorted* first to remove all duplicates. Knowing what these *don't* do is as important as what they do.

---

## 123.4 Partitioning and Sorting

| Algorithm | Effect | Complexity |
|---|---|---|
| `std::partition(b, e, pred)` | reorder so matching elements come first | O(n) |
| `std::stable_partition(b, e, pred)` | partition, preserving relative order | O(n log n) |
| `std::sort(b, e)` | sort (introsort: quick+heap+insertion) | O(n log n) |
| `std::stable_sort(b, e)` | sort, preserving equal elements' order | O(n log n) |
| `std::partial_sort(b, mid, e)` | sort only the top `(mid - b)` elements | O(n log k) |
| `std::nth_element(b, nth, e)` | place the `nth` element as if fully sorted | O(n) average |

> **Why this matters / cost model.** Choosing the *right* sort-family algorithm is a real performance decision. `std::sort` is **introsort** — quicksort with a heapsort fallback (to guarantee O(n log n) worst case) and insertion sort for small ranges — and is the right default. But if you only need the *top k* elements, `std::partial_sort` is cheaper (O(n log k)), and if you only need the *single* element that would be at position `nth` (e.g. the median, or a percentile — Chapter 103's tail latency!), `std::nth_element` does it in O(n) *average*, far cheaper than a full sort. This is the algorithmic-efficiency lesson of Chapter 80: don't sort the whole array to find the top 10 — use `partial_sort`; don't sort to find the median — use `nth_element`. The library gives you the specialised, cheaper algorithm for the narrower question.

---

## 123.5 Binary Search on Sorted Ranges

On a **sorted** range, these find elements in O(log n):

| Algorithm | Returns |
|---|---|
| `std::lower_bound(b, e, val)` | iterator to first element `>= val` |
| `std::upper_bound(b, e, val)` | iterator to first element `> val` |
| `std::binary_search(b, e, val)` | `bool`: does `val` exist? |
| `std::equal_range(b, e, val)` | the `[lower_bound, upper_bound)` range of all `val` |

> **Why this matters / cost model.** Binary search is O(log n) versus linear `std::find`'s O(n) — but it has a *precondition* the type system does not enforce: **the range must be sorted** (by the same comparator). Using binary search on an unsorted range is undefined behaviour (silently wrong results, not a crash). This is the classic trade: a sorted `std::vector` with `lower_bound` is often *faster* than a hash map or `std::set` for lookup-heavy, insert-rarely workloads, because the vector is contiguous and cache-friendly (Chapter 109) and binary search has excellent locality — but you pay O(n) to keep it sorted on insert. `lower_bound`/`upper_bound` are also the tools for *range* queries ("all elements between X and Y") that a hash map cannot answer. Match the structure to the query mix: sorted vector + binary search for read-heavy ordered lookups, hash map for insert-heavy point lookups.

---

## 123.6 Numeric Operations

From `<numeric>`:

| Algorithm | Effect |
|---|---|
| `std::iota(b, e, start)` | fill with `start, start+1, start+2, ...` |
| `std::accumulate(b, e, init)` | left fold (sum by default); *sequential* |
| `std::reduce(b, e)` | sum, but *unordered* — parallelisable (C++17) |
| `std::inner_product(b1, e1, b2, init)` | dot product |
| `std::transform_reduce(...)` | fused map-then-reduce (C++17) |

> **Why this matters / cost model.** The `accumulate` vs `reduce` distinction is a genuine performance and correctness point. `std::accumulate` is a *left fold* — it applies the operation strictly left-to-right, so it is sequential and the order is guaranteed (which matters for floating-point, where addition is not associative — Chapter 75). `std::reduce` (C++17) makes *no* ordering guarantee, which frees it to be *parallelised and vectorised* (with a parallel execution policy, `std::reduce(std::execution::par, ...)` runs across cores) — but means floating-point results may differ slightly from `accumulate`'s due to reassociation (the same FP-associativity caveat as `-ffast-math`, Chapter 86). The rule: use `accumulate` when order matters (sequential, deterministic FP); use `reduce` when it doesn't and you want parallelism. `transform_reduce` fuses a map and a reduce into one pass (the expression-template/fusion idea of Chapter 108) — e.g. a dot product is a `transform_reduce` with multiply-then-add.

---

## 123.7 Ranges: Composable Algorithms (C++20)

C++20 **Ranges** modernise the algorithms in two ways: they take a *range* directly (no `begin()`/`end()` pairs), and they compose via *views* — lazy, pipeable adaptors that chain operations without intermediate containers.

```cpp
// Min standard: C++20. Ranges: pass containers directly, and compose with lazy views.
#include <ranges>
#include <vector>
std::vector<int> v = {1, 2, 3, 4, 5, 6};
// Old: two algorithm calls + a temporary. Ranges: one lazy, fused pipeline.
auto result = v | std::views::filter([](int x){ return x % 2 == 0; })   // keep evens
                | std::views::transform([](int x){ return x * x; });    // square them
// `result` is a lazy view; iterating it runs filter+transform in ONE pass, no temporary vector.
```
*Listing 123.1 — C++20 Ranges: composable, lazy views fuse operations into a single pass with no temporaries.*

> **Why this matters / cost model.** Ranges fix two long-standing pain points. The *ergonomic* one: `std::ranges::sort(v)` instead of `std::sort(v.begin(), v.end())` — no more iterator-pair boilerplate, and a mismatched-iterator bug becomes impossible. The *performance and clarity* one: **views are lazy and composable**, so `v | filter | transform` produces a pipeline that, when iterated, applies both operations *element-by-element in a single pass* with *no intermediate container* — the same temporary-elimination benefit as expression templates (Chapter 108), but as a standard, general facility. A pre-Ranges equivalent would `copy_if` into a temporary vector, then `transform` it into another — two passes and two allocations. The lazy view fuses them. The caveat is the familiar one: a view holds references to its underlying range, so (like expression templates) a view outliving its source dangles. Ranges are the modern default for algorithm composition.

> **The discipline.** The standard algorithms are the single most underused productivity-and-performance tool in everyday C++: they make code shorter, clearer, correct-by-construction, and often faster, at zero abstraction cost. The discipline is to *reach for the named algorithm first* — and to choose the *right* one by its cost model: `find` is linear but `lower_bound` is logarithmic on sorted data; `sort` is overkill when `nth_element` or `partial_sort` answers the narrower question; `accumulate` is sequential but `reduce` parallelises; and C++20 Ranges compose them into lazy, fused, allocation-free pipelines. Knowing this catalogue — *and* each algorithm's complexity and preconditions — turns "write a loop" into "name the operation," which is the everyday face of C++ mastery. The volume now ends where it must: a capstone that builds a real system from everything in this book.
