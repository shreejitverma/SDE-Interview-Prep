# Chapter 34: Ranges I — Views, Range Adaptors, and Lazy Pipelines

> *The ranges library is the second pillar and the one that most changes how you express data transformations. It replaces the iterator-pair calling convention of the classic `<algorithm>` with composable, lazy pipelines built from the pipe operator. This chapter covers the range abstraction, what a view is and why it is cheap, the standard adaptors, and the lazy-evaluation model that makes a chain of `filter | transform | take` compile down to a single pass with no intermediate containers.*

For two decades, calling `std::copy_if` then a manual squaring loop meant naming `begin()`/`end()` twice and materializing a temporary vector between stages. Ranges collapse that into `data | views::filter(even) | views::transform(square)` — a declarative pipeline that allocates nothing and computes each element on demand during iteration. This chapter is the foundation; Chapter 35 covers the range *algorithms* (`std::ranges::*`), projections, and the borrowed/dangling model.

---

## Table of Contents

- [34.1 The End of Iterator Pairs](#341-the-end-of-iterator-pairs)
- [34.2 Ranges, Views, and the Core Concepts](#342-ranges-views-and-the-core-concepts)
- [34.3 The Pipe Operator and Range Adaptors](#343-the-pipe-operator-and-range-adaptors)
- [34.4 The Standard Views Catalogue](#344-the-standard-views-catalogue)
- [34.5 Lazy Evaluation: What Actually Happens During Iteration](#345-lazy-evaluation-what-actually-happens-during-iteration)
- [34.6 Generating Views: iota and Friends](#346-generating-views-iota-and-friends)
- [34.7 Decomposing and Splitting: join, split, elements](#347-decomposing-and-splitting-join-split-elements)
- [34.8 Performance and the Materialization Question](#348-performance-and-the-materialization-question)
- [34.9 Professional Insights](#349-professional-insights)

---

## 34.1 The End of Iterator Pairs

The classic standard algorithms take two iterators delimiting a range. This convention is verbose, error-prone (mismatched `begin`/`end` from different containers compile and then corrupt memory), and **non-composable** — the output of one algorithm cannot be piped into the next without an intermediate container.

```cpp
// Listing 34.1: the old way vs. the ranges way
#include <vector>
#include <algorithm>
#include <ranges>
#include <iostream>

int main() {
    std::vector<int> nums{1, 2, 3, 4, 5, 6};

    // Old way: a temporary container and two passes
    std::vector<int> temp;
    std::copy_if(nums.begin(), nums.end(), std::back_inserter(temp),
                 [](int i){ return i % 2 == 0; });
    for (auto& x : temp) x = x * x;

    // C++20 ranges: one lazy pipeline, no temporary, single pass
    namespace views = std::views;
    auto result = nums
        | views::filter([](int i){ return i % 2 == 0; })   // keep evens
        | views::transform([](int i){ return i * i; });    // square them

    for (int i : result) std::cout << i << ' ';   // 4 16 36
}
```

The ranges version is shorter, names the data once, cannot mismatch iterators, and — critically — performs no intermediate allocation.

---

## 34.2 Ranges, Views, and the Core Concepts

A **range** is anything you can iterate: it provides `begin()` and `end()` (via `std::ranges::begin`/`end`). Every standard container is a range. Formally, `std::ranges::range<T>` holds when those calls are valid.

A **view** is a special kind of range that is **cheap to copy/move and non-owning** — it refers to elements stored elsewhere and adapts how they are traversed. `std::ranges::view<T>` requires the type to be movable in O(1) and to not own its elements. Views are the building blocks of pipelines; because copying a view is cheap, pipelines can pass them around freely.

| Term | Owns data? | Copy cost | Example |
|------|-----------|-----------|---------|
| **Container** (range) | yes | O(n) | `std::vector<int>` |
| **View** | no | O(1) | `std::views::filter(v, pred)` |

Key range concepts beyond `range` and `view` (introduced in Chapter 33): `sized_range` (O(1) `size()`), `common_range` (`begin`/`end` same type), `viewable_range` (can be safely adapted into a view), and the iterator-strength ladder (`input_range` … `contiguous_range`).

---

## 34.3 The Pipe Operator and Range Adaptors

A **range adaptor** is an object that takes a range and produces a view. C++20 overloads `operator|` so that `range | adaptor` is equivalent to `adaptor(range)`, enabling left-to-right reading of a transformation chain. Adaptors are **partial-application-friendly**: `views::filter(pred)` (one argument) yields an adaptor closure that you pipe a range into later.

```cpp
// Listing 34.2: two equivalent spellings — function-call form and pipe form
#include <ranges>
#include <vector>

std::vector<int> v{1,2,3,4,5};
namespace vws = std::views;

// Function-call form
auto a = vws::transform(vws::filter(v, [](int x){ return x>2; }),
                        [](int x){ return x*10; });

// Pipe form (identical result, reads left-to-right)
auto b = v | vws::filter([](int x){ return x>2; })
           | vws::transform([](int x){ return x*10; });
```

The pipe form is preferred for readability: data flows left to right through each stage, exactly mirroring how you describe the operation in prose ("take v, keep those greater than 2, multiply by 10").

---

## 34.4 The Standard Views Catalogue

C++20 ships a core set of views in `std::views`. The table below is the C++20 set; note that several popular views (`zip`, `enumerate`, `chunk`, `slide`, `join_with`) are **C++23 and are not available here** — a frequent version trap.

| View | Effect |
|------|--------|
| `views::all` | view over an entire range (the identity adaptor) |
| `views::filter(pred)` | elements satisfying `pred` |
| `views::transform(f)` | `f` applied to each element |
| `views::take(n)` | first `n` elements |
| `views::take_while(pred)` | leading elements while `pred` holds |
| `views::drop(n)` | all but the first `n` |
| `views::drop_while(pred)` | skip leading elements while `pred` holds |
| `views::reverse` | reversed traversal (needs bidirectional) |
| `views::join` | flatten a range-of-ranges by one level |
| `views::split(delim)` | split a range on a delimiter into subranges |
| `views::elements<N>` | the Nth element of each tuple-like element |
| `views::keys` / `views::values` | `elements<0>` / `elements<1>` (for maps/pairs) |
| `views::iota(a[, b])` | the sequence `a, a+1, …` (bounded or infinite) |
| `views::single(x)` | a one-element view |
| `views::empty<T>` | an empty view of `T` |
| `views::counted(it, n)` | `n` elements starting at iterator `it` |
| `views::common` | adapt to a `common_range` (matching begin/end types) |

```cpp
// Listing 34.3: composing several adaptors into one pipeline
#include <ranges>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v{5, 1, 8, 2, 9, 3, 7};
    namespace vws = std::views;

    auto pipeline = v
        | vws::filter([](int x){ return x > 2; })   // 5 8 9 3 7
        | vws::transform([](int x){ return x - 1; }) // 4 7 8 2 6
        | vws::take(3)                                // 4 7 8
        | vws::reverse;                               // 8 7 4

    for (int x : pipeline) std::cout << x << ' ';     // 8 7 4
}
```

---

## 34.5 Lazy Evaluation: What Actually Happens During Iteration

The single most important property of views is **laziness**: building a pipeline does no work. `v | filter(p) | transform(f)` does not iterate `v`, does not call `p` or `f`, and allocates nothing. The work happens **element-by-element during iteration of the result** — when you advance the pipeline's iterator, it pulls one element through every stage.

```cpp
// Listing 34.4: laziness means each element flows through the whole chain on demand
#include <ranges>
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v{1, 2, 3, 4};
    namespace vws = std::views;

    auto pipe = v
        | vws::filter([](int x){ std::cout << "filter " << x << '\n'; return x % 2 == 0; })
        | vws::transform([](int x){ std::cout << "transform " << x << '\n'; return x * x; });

    // Nothing printed yet — the pipeline is built but not run.
    for (int x : pipe) std::cout << "got " << x << "\n";
    // Output interleaves filter/transform per element: the chain is single-pass and lazy.
}
```

Two consequences follow. First, **infinite views are usable** as long as something downstream bounds them (`views::iota(0) | views::take(5)`). Second, the optimizer typically fuses the whole chain into one loop with no per-stage container — the lazy model is what enables the zero-allocation claim. The opposite of lazy is **eager** "actions" that modify a container in place; C++20 ships only views (lazy), not actions — actions remain a range-v3 extension.

---

## 34.6 Generating Views: iota and Friends

Not every range comes from a container. **`views::iota`** generates an arithmetic sequence lazily, which combined with `take` replaces the classic counting `for` loop and feeds index-driven pipelines.

```cpp
// Listing 34.5: iota generates sequences; bounded and infinite forms
#include <ranges>
#include <vector>
#include <iostream>

int main() {
    namespace vws = std::views;

    // Bounded: [0, 5)
    for (int i : vws::iota(0, 5)) std::cout << i << ' ';   // 0 1 2 3 4
    std::cout << '\n';

    // Infinite, bounded downstream by take:
    auto first_squares = vws::iota(1)                       // 1, 2, 3, ... (infinite)
                       | vws::transform([](int n){ return n*n; })
                       | vws::take(5);                       // 1 4 9 16 25
    for (int s : first_squares) std::cout << s << ' ';
}
```

`views::single(x)` and `views::empty<T>` are the degenerate generators (one element / none), useful as neutral elements when a function must return a view in all branches.

---

## 34.7 Decomposing and Splitting: join, split, elements

Three adaptors restructure rather than filter or map.

```cpp
// Listing 34.6: join flattens, split tokenizes, keys/values project map entries
#include <ranges>
#include <vector>
#include <string>
#include <map>
#include <iostream>

int main() {
    namespace vws = std::views;

    // join: flatten a range-of-ranges by one level
    std::vector<std::vector<int>> nested{{1,2},{3,4},{5}};
    for (int x : nested | vws::join) std::cout << x << ' ';   // 1 2 3 4 5
    std::cout << '\n';

    // split: tokenize a string_view on a delimiter (each token is a subrange)
    std::string text = "a,bb,ccc";
    for (auto tok : text | vws::split(','))
        std::cout << std::string_view(tok.begin(), tok.end()) << '|';  // a|bb|ccc|
    std::cout << '\n';

    // keys/values: project the .first/.second of each pair
    std::map<std::string,int> m{{"x",1},{"y",2}};
    for (auto& k : m | vws::keys)   std::cout << k << ' ';   // x y
    for (auto& val : m | vws::values) std::cout << val << ' '; // 1 2
}
```

`views::elements<N>` generalizes `keys`/`values` to any tuple-like element (e.g., the 2nd field of a `std::tuple`). Note a C++20 sharp edge: `views::split` produces subrange tokens whose iterator type is not always a `contiguous_iterator`, so constructing a `string_view` from a token may need `std::string_view(tok.begin(), tok.end())` rather than a pointer-based constructor — and the older `lazy_split` behavior differs; this was smoothed in later standards.

---

## 34.8 Performance and the Materialization Question

Lazy pipelines are usually as fast as a hand-written loop, because the optimizer inlines each stage's per-element call. But there are real considerations for low-latency work:

- **No intermediate allocation** is the headline win: `filter | transform` never builds a temporary vector.
- **Re-traversal cost**: a `filter` view recomputes its predicate every time you iterate (and `begin()` on a filter view is O(n) to find the first match). If you iterate a filtered view repeatedly, materialize it once.
- **Materialization in C++20**: there is **no `std::ranges::to`** (that is C++23). To turn a view into a container, construct from iterators or loop:

```cpp
// Listing 34.7: materializing a view into a container in C++20 (no ranges::to yet)
#include <ranges>
#include <vector>

auto view = std::views::iota(1, 6) | std::views::transform([](int n){ return n*n; });

// C++20 idioms to materialize:
std::vector<int> out1(view.begin(), view.end());    // works when the view is a common_range...
std::vector<int> out2;                               // ...otherwise loop:
for (int x : view) out2.push_back(x);

// (C++23 would allow: auto out = view | std::ranges::to<std::vector>(); — NOT C++20)
```

The iterator-pair constructor requires the view to be a `common_range` (matching `begin`/`end` types); pipe through `views::common` first if it is not, or use the explicit loop. **Compile time** is the other cost: deeply nested pipelines instantiate large template types and can slow builds — a reason to keep hot-path pipelines readable and not pathologically deep.

---

## 34.9 Professional Insights

**Default to pipelines for clarity, verify codegen for hot paths.** A `filter | transform | take` chain reads like a specification and usually compiles to the same loop you would have written by hand. In a latency-critical inner loop, confirm that with the disassembler once — laziness plus inlining almost always delivers, but a stray non-inlinable lambda or a `filter` whose `begin()` is O(n) can surprise you.

**Materialize a filtered view you iterate more than once.** `views::filter` re-evaluates its predicate on every traversal and pays O(n) to find its first element. If you build a filtered view and loop over it repeatedly, snapshot it into a `vector` once; otherwise you silently pay the filter cost each pass.

**Mind the C++20 materialization gap.** `std::ranges::to` does not exist until C++23. In C++20, materialize with the iterator-pair constructor (after `views::common` if needed) or an explicit `push_back` loop. Reaching for `ranges::to` under `-std=c++20` is one of the most common version-trap compile errors.

**Know which views are not in C++20.** `zip`, `enumerate`, `chunk`, `slide`, `join_with`, and `ranges::to` are C++23. If your design wants them, either emulate (e.g., index with `views::iota` for a poor-man's enumerate) or gate behind a feature-test macro — do not assume they ship with C++20.

**Exploit laziness deliberately with infinite generators.** `views::iota(0)` plus a downstream `take`/`take_while` expresses bounded-from-an-unbounded-source cleanly and allocation-free. It is the idiomatic C++20 replacement for index loops and is a natural source for feeding coroutine generators (Chapter 37).
