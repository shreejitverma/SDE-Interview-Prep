# Chapter 57: `std::mdspan` — Multidimensional Views

> A C++ programmer who needed a matrix or a tensor has always faced an awkward gap: the language gives you a flat, contiguous `std::vector<double>` and the math gives you `A[i][j]`, but nothing standard bridges the two without either nested `vector`s (pointer-chasing, cache-hostile) or hand-rolled index arithmetic scattered across the code. `std::mdspan` closes that gap. It is a **non-owning, multidimensional view** over a contiguous block of memory, parameterized by its extents, its memory layout, and its access policy — and it has *zero* runtime overhead beyond the index arithmetic you would have written anyway. It is the missing primitive for numerical kernels, and it leans directly on the C++23 multidimensional `operator[]`.

## Table of Contents

1. [The Problem: Flat Memory, Multidimensional Intent](#571-the-problem-flat-memory-multidimensional-intent)
2. [The Four Template Parameters](#572-the-four-template-parameters)
3. [Extents: Static, Dynamic, and Mixed](#573-extents-static-dynamic-and-mixed)
4. [Indexing with the Multidimensional Subscript](#574-indexing-with-the-multidimensional-subscript)
5. [Layouts: `layout_right`, `layout_left`, `layout_stride`](#575-layouts-layout_right-layout_left-layout_stride)
6. [Slicing with `submdspan`](#576-slicing-with-submdspan)
7. [Performance: A Genuine Zero-Overhead View](#577-performance-a-genuine-zero-overhead-view)
8. [Professional Insights](#578-professional-insights)

---

## 57.1 The Problem: Flat Memory, Multidimensional Intent

Consider a 3×4 matrix of `int`. The cache-friendly representation is a single contiguous allocation of twelve integers. But contiguous storage gives you only one-dimensional access: `v[k]`. To read row 1, column 2 you must compute `v[1 * 4 + 2]` by hand, and that `* cols` arithmetic — the *layout* — gets duplicated at every access site, with every off-by-one and row-major/column-major confusion that implies.

The alternatives were all unsatisfying. `std::vector<std::vector<int>>` gives `m[i][j]` syntax but stores each row in a separate heap allocation, destroying locality and adding a pointer indirection per access. A bespoke `Matrix` class works but is non-standard, non-interoperable, and reinvented in every codebase.

`std::mdspan` (header `<mdspan>`) provides the standard answer: keep the single contiguous buffer, and wrap a *view* around it that knows its shape and how to map multidimensional indices to flat offsets. The view owns nothing — it is a pointer plus a tiny amount of shape metadata — so it is cheap to copy and pass by value.

**Listing 57.1: Wrapping a flat buffer as a 3×4 view.**

```cpp
#include <mdspan>
#include <vector>
#include <print>

int main() {
    std::vector<int> v(12);                 // contiguous storage, owns the memory
    auto view = std::mdspan(v.data(), 3, 4); // non-owning 3x4 view over it

    view[1, 2] = 99;                         // C++23 multidimensional subscript
    std::println("element [1,2] = {}", view[1, 2]);
    std::println("rows = {}, cols = {}", view.extent(0), view.extent(1));
}
```

---

## 57.2 The Four Template Parameters

The full type is `std::mdspan<T, Extents, LayoutPolicy, AccessorPolicy>`. Each parameter is a separable policy:

| Parameter | Role | Common default |
|---|---|---|
| `T` | element type the view exposes | (required) |
| `Extents` | the rank and the per-dimension sizes (static, dynamic, or mixed) | `std::dextents<IndexType, Rank>` |
| `LayoutPolicy` | how an index tuple maps to a linear offset | `std::layout_right` (row-major) |
| `AccessorPolicy` | how a linear offset becomes a reference (the indirection model) | `std::default_accessor<T>` |

The deduction guide used in Listing 57.1 (`std::mdspan(ptr, 3, 4)`) fills in `layout_right` and `default_accessor` automatically and produces a fully dynamic `extents`. You only reach for the explicit form when you want static extents, a column-major layout, a custom stride, or a custom accessor (for example, a strided-hardware or atomic accessor).

This policy decomposition is what makes `mdspan` general enough to be a standard vocabulary type: the same view abstraction serves dense row-major matrices, Fortran-style column-major arrays, strided sub-blocks, and exotic memory backends, by swapping one policy without touching call sites.

---

## 57.3 Extents: Static, Dynamic, and Mixed

The `extents` object encodes the rank (number of dimensions) and the size of each. A dimension's size can be fixed at compile time or supplied at runtime, and you can mix the two — the defining flexibility of `mdspan`.

- **Fully dynamic:** `std::dextents<std::size_t, 2>` — a rank-2 view whose two extents are runtime values. This is what the `(ptr, 3, 4)` deduction produces.
- **Fully static:** `std::extents<std::size_t, 3, 4>` — both sizes baked into the type. The shape costs zero bytes of storage and the index arithmetic uses compile-time constants the optimizer can fold.
- **Mixed:** `std::extents<std::size_t, std::dynamic_extent, 4>` — a runtime number of rows, a compile-time-fixed 4 columns. The sentinel `std::dynamic_extent` marks the runtime slots.

```cpp
#include <mdspan>

// 3x4, both extents known at compile time -> shape stored in zero bytes.
using Static34 = std::mdspan<int, std::extents<std::size_t, 3, 4>>;

// N rows (runtime) x 4 columns (compile time).
using RowsBy4  = std::mdspan<int, std::extents<std::size_t, std::dynamic_extent, 4>>;
```

The performance lever here is real: every extent you make static is an extent the view need not store and the compiler can treat as a constant in the offset computation. For fixed-size tiles in a kernel, fully static extents make `mdspan` indistinguishable from hand-written constant-stride code.

---

## 57.4 Indexing with the Multidimensional Subscript

`mdspan` is the headline consumer of the C++23 multidimensional `operator[]`. You index with a comma-separated list inside a single pair of brackets:

```cpp
view[i, j]        // rank-2
cube[i, j, k]     // rank-3
```

This `[i, j]` syntax is *new in C++23*; before it, the committee's mdspan prototypes had to use `view(i, j)` with `operator()`. The bracket form reads like the mathematical notation and, crucially, is the same `operator[](size_t, size_t, ...)` mechanism described in Chapter 66 — `mdspan` is the canonical motivating use case for that core-language change.

Supporting members for shape introspection:

- `view.rank()` — number of dimensions (a compile-time constant).
- `view.extent(d)` — size of dimension `d`.
- `view.size()` — total number of elements (product of extents).
- `view.data_handle()` — the underlying pointer.
- `view.stride(d)` — the layout's stride for dimension `d`.

> **Version-trap flag:** both `std::mdspan` itself and the `view[i, j]` multidimensional subscript it relies on are C++23. Under `-std=c++20`, neither the header nor the comma-in-brackets syntax exists. `submdspan` (Section 57.6) shipped slightly behind the core `mdspan` in some standard libraries — check `__cpp_lib_submdspan` if you depend on it.

---

## 57.5 Layouts: `layout_right`, `layout_left`, `layout_stride`

The **layout policy** is the function that turns an index tuple into a flat offset, and it is where `mdspan` earns its interoperability.

- **`layout_right`** (the default) is **row-major**: the last index varies fastest, so element `[i, j]` of an `R×C` view sits at offset `i*C + j`. This matches C, C++, and NumPy's default.
- **`layout_left`** is **column-major**: the first index varies fastest, offset `i + j*R`. This matches Fortran, BLAS/LAPACK, and MATLAB — so a `layout_left` `mdspan` can view a buffer handed to you by a Fortran numerical library *without copying or transposing it*.
- **`layout_stride`** lets you specify an explicit stride per dimension, which is how you view a non-contiguous sub-block of a larger array (every other column, a padded image row, a diagonal band).

**Listing 57.2: The same buffer, two layouts, two meanings.**

```cpp
#include <mdspan>
#include <array>
#include <print>

int main() {
    std::array<int, 6> buf{1, 2, 3, 4, 5, 6};

    std::mdspan<int, std::extents<int, 2, 3>, std::layout_right> row{buf.data()};
    std::mdspan<int, std::extents<int, 2, 3>, std::layout_left>  col{buf.data()};

    // Same memory, different index->offset mapping:
    std::println("row-major [1,0] = {}", row[1, 0]); // offset 1*3+0 = 3 -> 4
    std::println("col-major [1,0] = {}", col[1, 0]); // offset 1+0*2 = 1 -> 2
}
```

Being able to choose the layout policy is precisely what lets `mdspan` serve as a neutral interchange type between row-major C++ data and column-major numerical libraries.

---

## 57.6 Slicing with `submdspan`

`submdspan` produces a new `mdspan` that views a *region* of an existing one — a row, a column, a contiguous tile, or a strided sub-grid — without copying any elements. You describe each dimension's selection with one of:

- a single index, which *drops* that dimension (selecting one row of a matrix yields a rank-1 view);
- `std::full_extent`, which keeps the whole dimension;
- a `std::strided_slice` or a `{offset, count}` pair, which keeps a sub-range.

**Listing 57.3: Extracting a row and a sub-block as views.**

```cpp
#include <mdspan>
#include <vector>
#include <print>

int main() {
    std::vector<int> v(12);
    for (int i = 0; i < 12; ++i) v[i] = i;
    auto m = std::mdspan(v.data(), 3, 4);

    // Row 1 as a rank-1 view (the row dimension is dropped by the integer index).
    auto row1 = std::submdspan(m, 1, std::full_extent);
    std::println("row1[2] = {}", row1[2]);            // element [1,2] = 6

    // The 2x2 top-left block as a rank-2 view.
    auto block = std::submdspan(m, std::pair{0, 2}, std::pair{0, 2});
    std::println("block[1,1] = {}", block[1, 1]);     // element [1,1] = 5
}
```

Because the result is itself an `mdspan`, slices compose and can be passed to any function templated on `mdspan` — the basis for writing blocked, cache-tiled numerical algorithms with no manual offset bookkeeping.

---

## 57.7 Performance: A Genuine Zero-Overhead View

`std::mdspan` is the rare abstraction that is *exactly* as fast as the code it replaces:

- **No allocation, no ownership.** The view is a pointer plus shape metadata. Copying or passing it by value is trivial, and it never touches the heap.
- **Static extents cost nothing and fold.** Every compile-time extent is stored in zero bytes and becomes a constant in the offset computation, which the optimizer folds. A fully static `mdspan` compiles to the identical address arithmetic you would hand-write.
- **The index math is the only cost — and it is the irreducible cost.** `view[i, j]` lowers to `*(ptr + i*stride0 + j*stride1)`, the same multiply-add you would have written manually; the abstraction adds nothing on top.
- **It enables better algorithms.** Because slicing is free, you can express cache-tiled and blocked kernels naturally, which is usually a far larger win than any per-access micro-cost.

The one thing to respect is **aliasing and bounds**: `mdspan` does not own its memory and does no bounds checking in release builds, so the buffer must outlive every view of it and the extents must match the real allocation. Treat an `mdspan` exactly as you would a raw pointer with respect to lifetime — it has the same hazards and the same speed.

---

## 57.8 Professional Insights

**Reach for `mdspan` the moment you would otherwise write `i * cols + j`.** That manual stride arithmetic is the single most common source of indexing bugs in numerical C++, and `mdspan` centralizes it in one audited, layout-aware mapping. The payoff is not performance — the math is identical — it is *correctness* and the elimination of duplicated, error-prone offset code.

**Make extents static wherever the size is fixed.** A static `std::extents<…, 3, 4>` stores its shape in zero bytes and turns the stride into a compile-time constant the optimizer can fold into the addressing mode. For fixed-size tiles, blocks, and small matrices on a hot path, static extents are the difference between "as fast as hand-written" and "actually hand-written, but readable."

**Use `layout_left` to interoperate with the Fortran world for free.** BLAS, LAPACK, and most heavyweight numerical libraries are column-major. A `layout_left` `mdspan` views their buffers directly, with no transpose and no copy — a capability that previously demanded either a data conversion or a parallel set of column-major helpers. This is `mdspan`'s quiet superpower in scientific and quantitative codebases.

**Treat `mdspan` lifetime with raw-pointer discipline.** Because it is non-owning and unchecked, a dangling `mdspan` is exactly as dangerous as a dangling pointer, with none of the warnings an owning container would give you. Establish the invariant that the backing storage strictly outlives every view, prefer creating views close to their use, and never return an `mdspan` that outlives the buffer it points into.
