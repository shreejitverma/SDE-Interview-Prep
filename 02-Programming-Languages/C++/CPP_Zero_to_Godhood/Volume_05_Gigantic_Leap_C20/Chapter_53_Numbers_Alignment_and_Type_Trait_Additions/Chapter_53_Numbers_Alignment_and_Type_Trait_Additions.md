# Chapter 53: Numbers, Alignment, and Type-Trait Additions

> *The final cluster of C++20 standard-library additions serves numeric and generic code: the `<numbers>` header provides correctly-typed mathematical constants (`std::numbers::pi`, `e`, `sqrt2`) to end the `#define M_PI` and hand-typed-digits era; `std::midpoint` and `std::lerp` give overflow-safe averaging and interpolation; `std::assume_aligned` lets you promise the optimizer a pointer's alignment; and a batch of new type traits (`remove_cvref`, `type_identity`, `is_bounded_array`, `common_reference`) refine template metaprogramming. This chapter closes the library tour.*

These additions cater to the numeric and generic-programming corners of C++. For decades, `pi` came from the non-standard, untyped `M_PI` macro (which is not even guaranteed to exist) or from a literal with however many digits the author remembered; `<numbers>` makes it a properly-typed, full-precision constant. `(a + b) / 2` — the most natural way to average two numbers — silently overflows for large integers and is a real bug in binary search; `std::midpoint` fixes it. And the type-traits additions remove boilerplate that every serious template library previously hand-rolled.

---

## Table of Contents

- [53.1 The <numbers> Header: Mathematical Constants](#531-the-numbers-header-mathematical-constants)
- [53.2 std::midpoint: Overflow-Safe Averaging](#532-stdmidpoint-overflow-safe-averaging)
- [53.3 std::lerp: Linear Interpolation](#533-stdlerp-linear-interpolation)
- [53.4 std::assume_aligned: Promising Alignment to the Optimizer](#534-stdassume_aligned-promising-alignment-to-the-optimizer)
- [53.5 Type-Trait Additions: remove_cvref and type_identity](#535-type-trait-additions-remove_cvref-and-type_identity)
- [53.6 Type-Trait Additions: is_bounded_array and common_reference](#536-type-trait-additions-is_bounded_array-and-common_reference)
- [53.7 Professional Insights](#537-professional-insights)

---

## 53.1 The <numbers> Header: Mathematical Constants

`<numbers>` (namespace `std::numbers`) provides mathematical constants as `constexpr` variable templates, defaulting to `double` but available for any floating-point type. This replaces the non-standard `M_PI` macros and error-prone hand-typed literals.

```cpp
// Listing 53.1: typed, full-precision mathematical constants
#include <numbers>

double pi      = std::numbers::pi;          // 3.14159265358979... (double)
float  pi_f    = std::numbers::pi_v<float>; // float-precision pi
long double pl = std::numbers::pi_v<long double>;

double e       = std::numbers::e;           // Euler's number
double sqrt2   = std::numbers::sqrt2;       // square root of 2
double ln2     = std::numbers::ln2;         // natural log of 2
double phi     = std::numbers::phi;         // golden ratio
double inv_pi  = std::numbers::inv_pi;      // 1/pi

double area = std::numbers::pi * r * r;     // no #define, no magic digits
```

| Constant | Value |
|----------|-------|
| `pi` | π |
| `e` | Euler's number |
| `sqrt2`, `sqrt3` | √2, √3 |
| `ln2`, `ln10` | natural logs |
| `log2e`, `log10e` | logs of e |
| `phi` | golden ratio |
| `inv_pi`, `inv_sqrt2` | reciprocals |
| `egamma` | Euler–Mascheroni constant |

The `_v<T>` suffixed templates (`pi_v<float>`) give the constant at a chosen precision, so generic numeric code can request the constant matching its working type — eliminating the precision-loss bug of computing in `float` but seeding from a `double` literal. All are `constexpr`, usable in constant expressions and `static_assert`.

---

## 53.2 std::midpoint: Overflow-Safe Averaging

`std::midpoint(a, b)` (header `<numeric>`) computes the midpoint of two numbers **without overflow**, fixing the classic `(a + b) / 2` bug where the intermediate sum exceeds the type's range. It is the correct way to bisect in binary search and tree algorithms.

```cpp
// Listing 53.2: averaging without overflow
#include <numeric>
#include <cstdint>

std::int32_t a = 2'000'000'000;
std::int32_t b = 2'000'000'000;

// BUG: a + b overflows int32 (4e9 > INT32_MAX ~2.1e9) -> undefined behavior.
// int mid_bad = (a + b) / 2;

// CORRECT: midpoint never overflows; computes a + (b - a)/2 internally.
std::int32_t mid = std::midpoint(a, b);   // 2'000'000'000

// Works for pointers too — midpoint of a range, for binary search:
int arr[100];
int* lo = arr;
int* hi = arr + 100;
int* m  = std::midpoint(lo, hi);          // arr + 50, no pointer overflow
```

The textbook binary-search bug — `mid = (low + high) / 2` overflowing for large indices — went unfixed in countless codebases (famously including the JDK) for years. `std::midpoint` computes `a + (b - a) / 2`, which cannot overflow, and rounds toward `a` for integers. It is overloaded for integers, floating-point, and pointers (giving the midpoint element of a range), making it the single correct primitive for bisection.

---

## 53.3 std::lerp: Linear Interpolation

`std::lerp(a, b, t)` (header `<cmath>`) computes the linear interpolation `a + t*(b - a)`, but with guarantees the naive formula lacks: it is **monotonic**, **bounded**, and exact at the endpoints, even in floating-point.

```cpp
// Listing 53.3: well-behaved linear interpolation
#include <cmath>

double a = 0.0, b = 10.0;

double mid     = std::lerp(a, b, 0.5);    // 5.0
double quarter = std::lerp(a, b, 0.25);   // 2.5
double start   = std::lerp(a, b, 0.0);    // exactly a
double end     = std::lerp(a, b, 1.0);    // exactly b (guaranteed)
double beyond  = std::lerp(a, b, 2.0);    // 20.0 — extrapolation (t outside [0,1])
```

The naive `a + t*(b - a)` is not guaranteed to return exactly `b` when `t == 1.0` due to rounding, and can be non-monotonic near the endpoints — real problems in animation and signal processing where you need `lerp(a, b, 1.0) == b` precisely. `std::lerp` guarantees the endpoints are exact and the result is monotonic in `t`, and it handles `t` outside `[0, 1]` as extrapolation. It is `constexpr` and the correct choice wherever interpolation feeds further computation that assumes these properties.

---

## 53.4 std::assume_aligned: Promising Alignment to the Optimizer

`std::assume_aligned<N>(ptr)` (header `<memory>`) returns the same pointer but tells the compiler it is aligned to at least `N` bytes, enabling vectorization and aligned-load optimizations the compiler could not otherwise prove.

```cpp
// Listing 53.4: informing the optimizer of alignment
#include <memory>
#include <cstddef>

void scale(float* data, std::size_t n, float k) {
    // Promise the compiler 'data' is 64-byte aligned (e.g. cache-line / AVX-512).
    float* aligned = std::assume_aligned<64>(data);
    for (std::size_t i = 0; i < n; ++i)
        aligned[i] *= k;          // compiler may now emit aligned SIMD loads/stores
}
```

`assume_aligned` is a pure optimization hint — it generates no code and returns its argument unchanged, but the compiler may now assume the alignment and emit faster aligned vector instructions instead of unaligned (or scalar) fallbacks. **It is a hard promise, not a request:** if the pointer is *not* actually aligned to `N`, the behavior is undefined and you will get misaligned-access crashes or corruption. Use it only when you control the allocation (e.g. `alignas`, `std::aligned_alloc`, or an aligned allocator guarantees the alignment) — never on a pointer of unknown provenance.

---

## 53.5 Type-Trait Additions: remove_cvref and type_identity

C++20 adds `std::remove_cvref` — strip `const`/`volatile` and reference in one step, the single most common trait composition in generic code — and `std::type_identity`, which establishes a non-deduced context.

```cpp
// Listing 53.5: remove_cvref and type_identity
#include <type_traits>

// remove_cvref_t<T> = remove_cv_t<remove_reference_t<T>> — strip all three at once.
static_assert(std::is_same_v<std::remove_cvref_t<const int&>, int>);
static_assert(std::is_same_v<std::remove_cvref_t<volatile int&&>, int>);

template <typename T>
void store(T&& value) {
    using Stored = std::remove_cvref_t<T>;   // the bare value type for a forwarding ref
    Stored copy = std::forward<T>(value);
}

// type_identity_t<T> is just T, but it is a NON-DEDUCED context:
template <typename T>
void clamp_to(T value, std::type_identity_t<T> lo, std::type_identity_t<T> hi);

// Now T is deduced ONLY from the first argument; the bounds don't fight deduction:
// clamp_to(5, 0, 10);        // T = int from 5; 0 and 10 are not used to deduce T
// clamp_to(5.0, 0, 10);      // T = double from 5.0; 0/10 convert to double silently
```

`remove_cvref_t<T>` is the idiomatic "what is the underlying value type of this forwarding reference?" trait — previously written as the nested `remove_cv_t<remove_reference_t<T>>` in every metaprogramming library. `type_identity_t<T>` yields `T` unchanged but blocks template argument deduction at that position, letting you control *which* arguments drive deduction — useful for excluding parameters (like bounds or defaults) from the deduction process so they convert implicitly instead.

---

## 53.6 Type-Trait Additions: is_bounded_array and common_reference

C++20 adds `std::is_bounded_array`/`is_unbounded_array` to distinguish `T[N]` from `T[]`, and `std::common_reference` — the trait underpinning the Ranges and iterator concepts for finding a reference type two types can bind to.

```cpp
// Listing 53.6: array classification and common_reference
#include <type_traits>

static_assert(std::is_bounded_array_v<int[10]>);     // true  — known size
static_assert(!std::is_bounded_array_v<int[]>);      // false — unknown size
static_assert(std::is_unbounded_array_v<int[]>);     // true

// common_reference_t finds a reference type both can bind to — the basis of the
// iterator/range concepts (e.g. reconciling reference and value types).
using CR = std::common_reference_t<int&, const int&>;   // const int&
static_assert(std::is_same_v<CR, const int&>);
```

`is_bounded_array` and `is_unbounded_array` let generic code branch on whether an array type carries a compile-time size — relevant for the `make_shared<T[]>` machinery and array-aware utilities. `common_reference` is more foundational: it computes a reference type to which references of two types can both bind, and it is the trait the Ranges library uses to relate an iterator's `reference` and `value_type` (the `common_reference_with` concept). You rarely invoke it directly, but it is why generic algorithms over proxy iterators and heterogeneous reference types compose correctly in C++20.

---

## 53.7 Professional Insights

**Replace `M_PI` and hand-typed constant literals with `<numbers>`.** `M_PI` is non-standard (POSIX, not ISO C++), often requires a feature macro to even appear, and is untyped; a literal like `3.14159f` silently loses precision. `std::numbers::pi` and its `_v<T>` precision-parameterized forms are standard, full-precision, `constexpr`, and request the exact type your computation works in — eliminating the seed-precision bug where a `double` literal feeds a `float` calculation. There is no reason for a magic numeric constant in new code.

**Use `std::midpoint` for every bisection — this fixes a real, famous bug.** `(low + high) / 2` overflows for large integer indices, the defect that lurked in binary-search and merge-sort implementations across the industry for decades. `std::midpoint` computes `a + (b - a)/2`, which cannot overflow, and is overloaded for pointers so it bisects ranges directly. Make it the default for any "find the middle of two values" operation; the naive average should not appear in code that handles large or untrusted inputs.

**Reach for `std::lerp` whenever interpolation feeds further computation.** The hand-written `a + t*(b-a)` is not guaranteed exact at the endpoints and can be non-monotonic near them — defects that surface as visible glitches in animation and as accumulated error in signal processing. `std::lerp` guarantees `lerp(a, b, 1.0) == b` exactly and monotonic behavior in `t`, so downstream code relying on those properties stays correct. Prefer it over the manual formula except in the tightest inner loops where you have measured the difference.

**Treat `std::assume_aligned` as a hard, dangerous promise, not a hint.** It unlocks aligned SIMD codegen and costs nothing when correct, but it is undefined behavior if the pointer is not actually aligned to the asserted boundary — a crash or silent corruption, often only on certain CPUs. Use it exclusively on pointers whose alignment you guarantee through the allocation path (`alignas`, aligned allocators, `aligned_alloc`), and document that guarantee at the call site. Never apply it to a pointer of external or unknown provenance.

**Adopt `std::remove_cvref_t` as the standard forwarding-reference value trait, and know the niche of the others.** `remove_cvref_t<T>` replaces the ubiquitous `remove_cv_t<remove_reference_t<T>>` boilerplate in every template that stores or inspects a forwarded value — use it by default. `type_identity_t` is the precise tool for excluding a parameter from deduction (bounds, defaults), `is_bounded_array` for array-size-aware generic code, and `common_reference` is the foundation the Ranges concepts stand on even though you seldom name it directly. Together these traits make C++20 generic code shorter and the concept machinery behind Ranges work.
