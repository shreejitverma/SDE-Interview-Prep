# Chapter 60: Completing Ranges II — Folds, `ranges::to`, and Search

> If the new *views* of Chapter 59 are how you build a lazy pipeline, the additions in this chapter are how you *finish* one: collapse it to a single value with the new **fold algorithms**, materialize it into a concrete container with **`ranges::to`**, or interrogate it with the new **search algorithms** that C++20 forgot. Together they close the last large gaps in the ranges library — the eager, range-based reductions and conversions that every real program needs at the end of a pipeline.

## Table of Contents

1. [The Missing Endpoints of a Pipeline](#601-the-missing-endpoints-of-a-pipeline)
2. [`ranges::to` — Materializing a Range into a Container](#602-rangesto--materializing-a-range-into-a-container)
3. [The Fold Family: `fold_left` and Friends](#603-the-fold-family-fold_left-and-friends)
4. [Choosing the Right Fold](#604-choosing-the-right-fold)
5. [New Range Search: `contains`, `find_last`, `starts_with`, `ends_with`](#605-new-range-search-contains-find_last-starts_with-ends_with)
6. [Performance of Eager Range Operations](#606-performance-of-eager-range-operations)
7. [Professional Insights](#607-professional-insights)

---

## 60.1 The Missing Endpoints of a Pipeline

A ranges pipeline built from views is lazy and produces *another view*. At some point you must leave the lazy world: turn the view into a `vector`, reduce it to a sum, or ask a yes/no question about it. C++20 left this final step surprisingly bare. There was no standard way to say "collect this view into a `std::vector`" — you wrote a manual loop or an awkward `std::vector(v.begin(), v.end())` that did not even work for non-common views. There was no range-based `accumulate` in the `std::ranges` namespace. And the search algorithms had gaps: no `ranges::contains`, no `find_last`, no `starts_with` on arbitrary ranges.

C++23 fills all three:

1. **`std::ranges::to`** materializes any range into a container of your choosing.
2. **The fold algorithms** (`fold_left`, `fold_right`, and variants) provide range-native, constrained reductions — the modern replacement for `std::accumulate`.
3. **New search algorithms** (`contains`, `contains_subrange`, `find_last`, `starts_with`, `ends_with`) round out range interrogation.

---

## 60.2 `ranges::to` — Materializing a Range into a Container

`std::ranges::to<Container>()` converts any range — including a lazy view pipeline — into a concrete container. It is the standard, allocating endpoint that turns "describe a computation" into "run it and keep the result."

```cpp
#include <ranges>
#include <vector>

int main() {
    auto v = std::views::iota(0, 5) | std::ranges::to<std::vector>();  // {0,1,2,3,4}
}
```

Three forms cover the common needs:

- **`r | std::ranges::to<std::vector>()`** — deduce the element type from the range and build a `std::vector` of it. You may name the full type (`to<std::vector<int>>()`) or let the value type be deduced (`to<std::vector>()`, using C++23's class-template-argument deduction support in `to`).
- **It works for any container**, not just `vector`: `to<std::set>()`, `to<std::map>()`, `to<std::string>()`, `to<std::list>()` — anything constructible from a range or an iterator pair.
- **It nests.** A range of ranges can be materialized into a container of containers: `to<std::vector<std::vector<int>>>()` recursively converts each inner range too.

**Listing 60.1: Materializing a pipeline, including nested conversion.**

```cpp
#include <ranges>
#include <vector>
#include <string>
#include <print>

int main() {
    // Flat: even squares into a vector.
    auto squares = std::views::iota(1, 6)
        | std::views::filter([](int n){ return n % 2 == 1; })
        | std::views::transform([](int n){ return n * n; })
        | std::ranges::to<std::vector>();          // {1, 9, 25}
    std::println("{}", squares);

    // Nested: chunk a range, then materialize chunks into vectors of vectors.
    auto grid = std::views::iota(0, 6)
        | std::views::chunk(2)
        | std::ranges::to<std::vector<std::vector<int>>>();  // {{0,1},{2,3},{4,5}}
    std::println("{}", grid);
}
```

`ranges::to` also forwards extra constructor arguments (allocators, comparators) to the target container, so you can build, say, a `std::vector` with a custom allocator directly from a pipeline.

---

## 60.3 The Fold Family: `fold_left` and Friends

A **fold** reduces a range to a single value by repeatedly applying a binary operation, threading an accumulator through the elements. C++23 adds a family of `std::ranges::fold_*` algorithms — the range-native, properly-constrained successors to `std::accumulate` (which lives in `<numeric>` and is iterator-pair-based, easy to misuse, and not in the `ranges` namespace).

The five members:

| Algorithm | Direction | Initial value | Returns |
|---|---|---|---|
| `fold_left(r, init, f)` | left to right | explicit `init` | the final accumulator |
| `fold_right(r, init, f)` | right to left | explicit `init` | the final accumulator |
| `fold_left_first(r, f)` | left to right | first element | `optional` (empty if range empty) |
| `fold_right_last(r, f)` | right to left | last element | `optional` (empty if range empty) |
| `fold_left_with_iter(r, init, f)` | left to right | explicit `init` | `{iterator, value}` pair |

`fold_left` is the workhorse: it computes `f(... f(f(init, e0), e1) ..., en)`. `fold_right` associates the other way, which matters for non-associative operations and for building right-nested structures. The `_first`/`_last` variants seed the accumulator from the range itself and therefore return an `std::optional` to handle the empty-range case safely — no more "what is the identity element?" puzzle. `fold_left_with_iter` additionally hands back the end iterator, useful when folding a sub-range and continuing from where you stopped.

**Listing 60.2: The fold family in action.**

```cpp
#include <ranges>
#include <vector>
#include <functional>
#include <print>

int main() {
    std::vector<int> v{1, 2, 3, 4};

    int sum = std::ranges::fold_left(v, 0, std::plus{});          // 10
    int prod = std::ranges::fold_left(v, 1, std::multiplies{});   // 24

    // No identity needed; safe on empty ranges via optional.
    std::optional<int> maxv = std::ranges::fold_left_first(v,
        [](int a, int b){ return a > b ? a : b; });               // 4

    std::println("sum={} prod={} max={}", sum, prod, *maxv);

    // fold_right associates the other way: 1 - (2 - (3 - (4 - 0))) = -2
    int alt = std::ranges::fold_right(v, 0, std::minus{});
    std::println("fold_right minus = {}", alt);                   // -2
}
```

Because the fold algorithms are constrained with concepts, a type mismatch between the accumulator and the operation is a clear compile-time error, not the silent narrowing that `std::accumulate(v.begin(), v.end(), 0)` famously inflicts (the classic "summed doubles into an `int`" bug).

---

## 60.4 Choosing the Right Fold

A quick decision guide:

- Reducing with an **explicit identity** (sum from 0, product from 1, concatenation from `""`)? Use **`fold_left`** — it is the default and the most optimizable.
- The operation is **non-associative** or you need right-nesting (building a right-leaning list, right-to-left subtraction/division semantics)? Use **`fold_right`**.
- There is **no natural identity** and the range may be empty (max, min, "first wins")? Use **`fold_left_first`** / **`fold_right_last`** and handle the returned `optional`.
- You are folding a **prefix** and need to resume from the stopping point? Use **`fold_left_with_iter`**.

In practice `fold_left` covers the overwhelming majority of reductions and should be your reflex; the others are precision tools for the cases its signature cannot express.

---

## 60.5 New Range Search: `contains`, `find_last`, `starts_with`, `ends_with`

C++20's `std::ranges` algorithms shipped `find`, `find_if`, `search`, and `mismatch`, but several everyday queries were missing. C++23 adds them:

- **`std::ranges::contains(r, value)`** — `true` if `value` appears in `r`. The readable replacement for `ranges::find(r, value) != r.end()`.
- **`std::ranges::contains_subrange(r, sub)`** — `true` if `sub` appears as a contiguous subsequence of `r` (substring-style search over arbitrary ranges).
- **`std::ranges::find_last(r, value)`**, **`find_last_if(r, pred)`**, **`find_last_if_not(r, pred)`** — return a subrange starting at the *last* match, filling the long-standing absence of a reverse `find` that does not require manually reversing the range.
- **`std::ranges::starts_with(r, prefix)`** and **`std::ranges::ends_with(r, suffix)`** — test prefix/suffix relationships over *any* ranges, generalizing the `std::string` methods of the same name (Chapter 64) to arbitrary sequences.

**Listing 60.3: The new range queries.**

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<int> v{1, 2, 3, 2, 5};

    std::println("{}", std::ranges::contains(v, 3));                 // true
    std::println("{}", std::ranges::contains_subrange(v, std::vector{3, 2}));  // true
    std::println("{}", std::ranges::starts_with(v, std::vector{1, 2}));        // true
    std::println("{}", std::ranges::ends_with(v, std::vector{2, 5}));          // true

    auto last2 = std::ranges::find_last(v, 2);   // subrange at the LAST 2 (index 3)
    std::println("last 2 at index {}", last2.begin() - v.begin());  // 3
}
```

Each takes the usual optional projection and comparator, so you can search by a member or under a custom equality without transforming the range first.

> **Version-trap flag:** `std::ranges::to`, the entire `fold_*` family, and the search additions (`contains`, `contains_subrange`, `find_last*`, `starts_with`, `ends_with`) are **C++23**. C++20's `<numeric>` `std::accumulate` is unconstrained and iterator-based — do not confuse it with `ranges::fold_left`. The `std::string`/`string_view` `contains` member is also C++23 but is a separate facility (Chapter 64).

---

## 60.6 Performance of Eager Range Operations

These algorithms are *eager* — they run to completion when called — so their cost model differs from the lazy views:

- **`ranges::to` allocates once (ideally).** When the range models `sized_range`, `to` can reserve the exact capacity up front, so materializing a pipeline into a `vector` is a single allocation plus the per-element move/copy. When the size is unknown (a `filter` chain), it grows like `push_back`. Prefer pipelines whose final size is computable, or reserve manually, when the allocation matters.
- **Folds are tight loops.** `fold_left` over a contiguous range with a simple operation compiles to the same accumulator loop you would hand-write, and with a visible operation the optimizer can often vectorize it. `fold_right` cannot always be reversed for free on non-bidirectional ranges, so prefer `fold_left` unless the associativity genuinely requires otherwise.
- **The search algorithms are linear and short-circuiting.** `contains`, `starts_with`, and the `find_last` family stop as early as the query allows; `find_last` over a bidirectional range is implemented to scan from the end, so it does not pay to materialize a reversed copy yourself.

The general rule: build the computation lazily with views (no allocation, fused), and pay the single eager cost only at the endpoint — one `ranges::to` allocation, or one fold pass — rather than materializing intermediate containers between stages.

---

## 60.7 Professional Insights

**`ranges::to` is the piece that makes the whole ranges library usable in production code.** A lazy pipeline is elegant, but most code eventually needs a real container to store, return, or hand to an ABI boundary. Before C++23, that final hop was a manual loop that undercut the readability the pipeline bought you. With `to`, the entire computation — including the materialization — reads as one declarative expression, and the library reserves capacity for you when it can. Adopt it as the default terminator for any pipeline whose result outlives the expression.

**Replace `std::accumulate` with `fold_left`, and let the type system catch your reduction bugs.** The canonical `std::accumulate(v.begin(), v.end(), 0)` silently narrows a sum of `double`s to `int`; `ranges::fold_left` is concept-constrained and rejects the mismatch at compile time. Beyond safety, `fold_left` takes the range directly (no `.begin()/.end()`), composes with views, and expresses the empty-range case honestly through the `_first`/`_last` variants' `optional`. There is little reason to reach for `accumulate` in new code.

**Use the `_first`/`_last` folds to retire the "what is the identity?" question.** Reductions like max, min, or "first non-default wins" have no natural seed value, and inventing one (`INT_MIN`?) is both ugly and a source of bugs on empty input. `fold_left_first` seeds from the data and returns an `optional`, making the empty case a value you must handle rather than a silent wrong answer. This is the principled way to express identity-free reductions.

**Prefer the named search algorithms over hand-rolled equivalents for both clarity and correctness.** `ranges::contains(r, x)` says exactly what `ranges::find(r, x) != r.end()` means but cannot be gotten subtly wrong, and `find_last` scans from the end rather than reversing a copy. These are small functions, but standardizing on them removes a category of off-by-one and end-iterator mistakes, and they accept projections so you rarely need to pre-transform the range to search it.
