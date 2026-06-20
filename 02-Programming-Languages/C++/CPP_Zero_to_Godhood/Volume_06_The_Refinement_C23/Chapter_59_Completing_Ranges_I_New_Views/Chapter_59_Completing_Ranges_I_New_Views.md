# Chapter 59: Completing Ranges I — The New Views

> C++20's `<ranges>` shipped with a deliberately minimal set of views because the library was enormous and time was short. The everyday adaptors that programmers reach for constantly — pairing a range with its indices, walking two ranges in lockstep, sliding a window across a sequence, breaking a range into fixed-size chunks — were all cut. C++23 restores them. This chapter covers the new *views*: lazy, composable, non-owning range adaptors that compute their elements on demand. Together with the new algorithms of Chapter 60, they make the C++ ranges library finally feel complete.

## Table of Contents

1. [Why So Many Views Were Missing](#591-why-so-many-views-were-missing)
2. [`views::enumerate` — Index/Value Pairs](#592-viewsenumerate--indexvalue-pairs)
3. [`views::zip` and `views::zip_transform`](#593-viewszip-and-viewszip_transform)
4. [`views::adjacent`, `views::pairwise`, and `views::adjacent_transform`](#594-viewsadjacent-viewspairwise-and-viewsadjacent_transform)
5. [`views::chunk`, `views::slide`, and `views::chunk_by`](#595-viewschunk-viewsslide-and-viewschunk_by)
6. [`views::stride`, `views::cartesian_product`, `views::join_with`](#596-viewsstride-viewscartesian_product-viewsjoin_with)
7. [`views::repeat`, `views::as_const`, `views::as_rvalue`](#597-viewsrepeat-viewsas_const-viewsas_rvalue)
8. [Custom Adaptors with `range_adaptor_closure`](#598-custom-adaptors-with-range_adaptor_closure)
9. [Professional Insights](#599-professional-insights)

---

## 59.1 Why So Many Views Were Missing

The ranges library that landed in C++20 was a subset of Eric Niebler's `range-v3` reference library, pared down to what the committee could specify and ship with confidence. The casualties were not obscure: `zip`, `enumerate`, `chunk`, `slide`, and `cartesian_product` are among the most-used adaptors in any range-based codebase, and their absence forced C++20 users back to index-based loops for exactly the patterns ranges were supposed to eliminate.

C++23 adds roughly a dozen new views. They share the properties that make views worth using: each is **lazy** (elements are computed only when iterated), **non-owning** (a view refers to its underlying range; it does not copy it), and **composable** (views chain with `|`, building a pipeline that is fused into a single pass). None allocates; the cost of a view chain is the cost of the iteration plus the per-element transformation, with no intermediate containers.

A reference summary before the detail:

| View | Yields |
|---|---|
| `enumerate` | `(index, element)` tuples |
| `zip` | tuples drawn one-per-input-range, in lockstep |
| `zip_transform` | result of a function applied across the zipped tuples |
| `adjacent<N>` / `pairwise` | sliding tuples of `N` (pairwise = `N==2`) consecutive elements |
| `adjacent_transform<N>` | a function applied to each adjacent window |
| `chunk` | non-overlapping sub-ranges of fixed size |
| `slide` | overlapping windows of fixed size |
| `chunk_by` | sub-ranges split where a binary predicate is false |
| `stride` | every *n*-th element |
| `cartesian_product` | tuples of the cross-product of several ranges |
| `join_with` | a flattened range of ranges, with a delimiter inserted |
| `repeat` | one value (optionally `n` times), lazily |
| `as_const` / `as_rvalue` | the same elements as `const` / as rvalues |

---

## 59.2 `views::enumerate` — Index/Value Pairs

`std::views::enumerate` pairs each element with its zero-based index, yielding tuples `(index, element)`. It is the standard, bug-free answer to "I need both the value and its position," replacing the manual counter that so often drifts out of sync with the iteration.

**Listing 59.1: Enumerating a range.**

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<char> v{'a', 'b', 'c'};
    for (auto [i, x] : std::views::enumerate(v))
        std::println("{}: {}", i, x);     // 0: a / 1: b / 2: c
}
```

The index is the view's own `difference_type`, not necessarily `size_t`; bind it with `auto` and let the structured binding deduce it. Each pair's element member is a *reference* into the underlying range, so you can mutate through it if the range is mutable.

---

## 59.3 `views::zip` and `views::zip_transform`

`std::views::zip` iterates several ranges **in lockstep**, yielding a tuple with one element from each on every step, and stopping at the end of the *shortest* input. It replaces the classic parallel-index loop over several same-length vectors.

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<int>         ids{1, 2, 3};
    std::vector<std::string> names{"ann", "bob", "cy"};

    for (auto [id, name] : std::views::zip(ids, names))
        std::println("{} -> {}", id, name);
}
```

`std::views::zip_transform` goes one step further: instead of yielding the tuple, it applies a function to the zipped elements and yields the *result*. It is `zip` followed by `transform`, fused into one view.

**Listing 59.2: Element-wise vector addition with `zip_transform`.**

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<int> a{1, 2, 3}, b{10, 20, 30};

    auto sums = std::views::zip_transform(std::plus{}, a, b);
    for (int s : sums) std::print("{} ", s);   // 11 22 33
    std::println("");
}
```

Both stop at the shortest input, which makes them safe for ranges of differing length without an explicit length check.

---

## 59.4 `views::adjacent`, `views::pairwise`, and `views::adjacent_transform`

`std::views::adjacent<N>` slides a window of `N` *consecutive* elements across a single range, yielding an `N`-tuple at each position. `std::views::pairwise` is the named shortcut for `adjacent<2>`. Where `slide` (next section) takes a runtime window size and yields sub-*ranges*, `adjacent<N>` takes a compile-time `N` and yields fixed-size *tuples* — better suited to fixed-arity element-wise work.

`std::views::adjacent_transform<N>` applies a function to each adjacent window and yields the result — the natural tool for finite differences, moving relationships between neighbors, and similar stencil computations.

**Listing 59.3: Consecutive differences via `adjacent_transform`.**

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<int> v{1, 4, 9, 16, 25};

    // pairwise differences: 4-1, 9-4, 16-9, 25-16
    auto diffs = std::views::adjacent_transform<2>(v, [](int lo, int hi){ return hi - lo; });
    for (int d : diffs) std::print("{} ", d);   // 3 5 7 9
    std::println("");
}
```

The window arity is part of the type (`<2>`), so the lambda's parameter count must match `N` exactly.

---

## 59.5 `views::chunk`, `views::slide`, and `views::chunk_by`

These three carve a range into sub-ranges, differing in *how* they cut.

- **`std::views::chunk(n)`** partitions into **non-overlapping** blocks of size `n` (the last block may be shorter). Use it for batching — processing a stream `n` items at a time.
- **`std::views::slide(n)`** produces **overlapping** windows of exactly `n` consecutive elements, advancing by one each step. Use it for moving averages and any fixed-width windowed statistic where you want the window as a sub-range.
- **`std::views::chunk_by(pred)`** splits the range wherever the binary predicate `pred(prev, cur)` returns `false` — i.e. it groups maximal runs of adjacent elements that satisfy the relation. Use it to group consecutive equal (or related) elements without sorting.

**Listing 59.4: Batching, windowing, and grouping.**

```cpp
#include <ranges>
#include <vector>
#include <print>

int main() {
    std::vector<int> v{1, 2, 3, 4, 5};

    for (auto block : std::views::chunk(v, 2))      // [1,2] [3,4] [5]
        std::println("chunk: {}", block);

    for (auto win : std::views::slide(v, 3))        // [1,2,3] [2,3,4] [3,4,5]
        std::println("slide: {}", win);

    std::vector<int> w{1, 1, 2, 2, 2, 3};
    for (auto grp : std::views::chunk_by(w, std::equal_to{}))  // [1,1] [2,2,2] [3]
        std::println("group: {}", grp);
}
```

Each yielded chunk/window/group is itself a view (a sub-range), so it composes further or prints directly via C++23 range formatting (Chapter 61).

---

## 59.6 `views::stride`, `views::cartesian_product`, `views::join_with`

- **`std::views::stride(n)`** yields every `n`-th element (indices 0, n, 2n, …) — downsampling without copying.
- **`std::views::cartesian_product(r1, r2, …)`** yields every combination, one element drawn from each input range — the lazy equivalent of nested loops, useful for grid generation and exhaustive parameter sweeps.
- **`std::views::join_with(r, delim)`** flattens a range-of-ranges into a single range, inserting `delim` (a single element or a range) between the inner ranges — the range-native `string`/sequence join.

**Listing 59.5: Stride, product, and delimited join.**

```cpp
#include <ranges>
#include <vector>
#include <string>
#include <print>

int main() {
    std::vector<int> v{0, 1, 2, 3, 4, 5, 6};
    for (int x : v | std::views::stride(2)) std::print("{} ", x);  // 0 2 4 6
    std::println("");

    for (auto [a, b] : std::views::cartesian_product(std::vector{1, 2}, std::vector{'x', 'y'}))
        std::print("({},{}) ", a, b);     // (1,x) (1,y) (2,x) (2,y)
    std::println("");

    std::vector<std::string> parts{"a", "b", "c"};
    std::string joined;
    for (char c : parts | std::views::join_with('-')) joined += c;  // "a-b-c"
    std::println("{}", joined);
}
```

---

## 59.7 `views::repeat`, `views::as_const`, `views::as_rvalue`

- **`std::views::repeat(value)`** is a *factory* (not an adaptor) producing an endlessly repeated `value`; `std::views::repeat(value, n)` repeats it `n` times. It is the lazy counterpart to filling a container, and pairs naturally with `zip` to attach a constant column to another range.
- **`std::views::as_const`** yields the underlying elements as `const`, the view-level analogue of `std::as_const` — useful to hand a read-only view to an interface without copying.
- **`std::views::as_rvalue`** yields each element as an rvalue (`std::move`-ing it on access), so a downstream algorithm or `ranges::to` *moves* elements out of the source instead of copying them — the efficient way to drain a container into another.

**Listing 59.6: Moving elements out of a source with `as_rvalue`.**

```cpp
#include <ranges>
#include <vector>
#include <string>

int main() {
    std::vector<std::string> src{"alpha", "beta", "gamma"};

    std::vector<std::string> dst;
    for (auto&& s : src | std::views::as_rvalue)
        dst.push_back(std::move(s));   // strings are moved, not copied
    // src's strings are now in a moved-from state
}
```

---

## 59.8 Custom Adaptors with `range_adaptor_closure`

C++23 also exposes the machinery that makes `|` work, so your own range adaptors can participate in pipelines as first-class citizens. Deriving a callable from **`std::ranges::range_adaptor_closure`** marks it as a pipeable closure: `r | my_adaptor` then becomes `my_adaptor(r)`, exactly like the standard views.

**Listing 59.7: A custom pipeable adaptor.**

```cpp
#include <ranges>
#include <vector>
#include <print>

// A closure that sums-as-you-go is an algorithm; here we make a simple
// pipeable adaptor that keeps only elements above a captured threshold.
struct above_t : std::ranges::range_adaptor_closure<above_t> {
    int threshold;
    auto operator()(std::ranges::viewable_range auto&& r) const {
        return std::views::filter(std::forward<decltype(r)>(r),
                                  [t = threshold](int x){ return x > t; });
    }
};

int main() {
    above_t above{2};
    std::vector<int> v{1, 2, 3, 4};
    for (int x : v | above) std::print("{} ", x);   // 3 4
    std::println("");
}
```

Inheriting from `range_adaptor_closure` is the supported, standard way to integrate custom adaptors — previously this relied on implementation-specific base classes.

> **Version-trap flag:** every view in this chapter — `enumerate`, `zip`, `zip_transform`, `adjacent`, `pairwise`, `adjacent_transform`, `chunk`, `slide`, `chunk_by`, `stride`, `cartesian_product`, `join_with`, `repeat`, `as_const`, `as_rvalue` — and `std::ranges::range_adaptor_closure` are **C++23**. None exist under `-std=c++20`, which shipped only `filter`, `transform`, `take`, `drop`, `join`, `split`, `reverse`, `elements`, `keys`, `values`, `common`, and the basic factories.

---

## 59.9 Professional Insights

**The new views finally let you write index-free code for the patterns that needed indices in C++20.** `enumerate`, `zip`, `slide`, and `chunk` eliminate exactly the manual-counter and parallel-index loops that are the richest source of off-by-one and length-mismatch bugs. When you find yourself writing `for (size_t i = 0; i < a.size(); ++i)` to walk two arrays together or to look at neighbors, reach for `zip` or `adjacent` — the resulting code states intent and cannot desynchronize its indices.

**Remember that views are lazy and non-owning — mind dangling and re-traversal cost.** A view holds a reference to its source; if the source is a temporary that dies, the view dangles, exactly like an `mdspan` or a `string_view`. And because views recompute on each pass, iterating a `filter|transform` chain twice does the work twice. When you need the result more than once, or need to outlive the source, materialize with `ranges::to` (Chapter 60) — but until then, the lazy chain is allocation-free and single-pass.

**Prefer `adjacent<N>`/`adjacent_transform<N>` for fixed-arity neighbor work and `slide(n)`/`chunk(n)` for runtime windows.** The compile-time-arity views yield tuples the optimizer can fully unroll and keep in registers, ideal for stencils and finite differences; the runtime-size views yield sub-ranges, ideal for streaming batches and moving windows whose width is a parameter. Choosing the right one is both a clarity and a performance decision.

**Use `as_rvalue` to drain containers and `range_adaptor_closure` to extend the pipeline.** `views::as_rvalue` turns a copy-out into a move-out for free, which matters when relocating ranges of strings or other heap-owning elements. And when a transformation recurs across your codebase, packaging it as a `range_adaptor_closure` makes it compose with `|` like a native view — turning ad-hoc helper functions into reusable, readable pipeline stages.
