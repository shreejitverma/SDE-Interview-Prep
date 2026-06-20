# Chapter 33: Concepts II — The Standard Concepts Library and Constraint Subsumption

> *Chapter 32 covered how to write and apply constraints. This chapter covers the two things that make concepts a system rather than a syntax: the rich library of predefined concepts in `<concepts>` (and across `<iterator>`, `<ranges>`), and the subsumption rules that let the compiler order overloads by specificity. Together they are why you rarely need to write a concept from scratch and why constrained overloading "just picks the right one."*

The standard concepts library encodes decades of accumulated wisdom about type categories — same-ness, convertibility, the regular/semiregular hierarchy, callable kinds, the iterator and range taxonomies. Knowing this vocabulary means you compose existing concepts instead of reinventing them, and your constraints automatically interoperate with the standard algorithms and ranges. Subsumption is the formal partial ordering over constraints that resolves "which overload is more specialized" — the mechanism that makes `signed_integral` win over `integral` for a signed argument.

---

## Table of Contents

- [33.1 Why a Standard Concepts Library Matters](#331-why-a-standard-concepts-library-matters)
- [33.2 Core Language Concepts](#332-core-language-concepts)
- [33.3 Comparison Concepts](#333-comparison-concepts)
- [33.4 Object and Lifetime Concepts: The Regular Hierarchy](#334-object-and-lifetime-concepts-the-regular-hierarchy)
- [33.5 Callable Concepts](#335-callable-concepts)
- [33.6 Iterator and Range Concepts](#336-iterator-and-range-concepts)
- [33.7 Constraint Normalization and Atomic Constraints](#337-constraint-normalization-and-atomic-constraints)
- [33.8 Subsumption and the Partial Ordering of Constraints](#338-subsumption-and-the-partial-ordering-of-constraints)
- [33.9 Pitfalls: When Subsumption Does Not Apply](#339-pitfalls-when-subsumption-does-not-apply)
- [33.10 Professional Insights](#3310-professional-insights)

---

## 33.1 Why a Standard Concepts Library Matters

A concept is only as useful as its reusability. If every team invented its own `Addable`, `Comparable`, and `Iterator`, constraints would not compose across libraries. C++20 therefore ships a **standard vocabulary of concepts** in `<concepts>`, with the iterator and range concepts in `<iterator>` and `<ranges>`. These are the predicates the standard algorithms themselves are constrained with, so building your interfaces from them makes your code interoperate with `std::ranges::sort`, `std::ranges::find`, and the views library for free.

The library is organized into families: **core language** concepts (same/convertible/derived), **comparison** concepts, **object** concepts (the movable/copyable/regular hierarchy), **callable** concepts, and the **iterator/range** concepts. We take them in turn.

---

## 33.2 Core Language Concepts

These express fundamental type relationships. The most important is `std::same_as`, which is deliberately defined symmetrically (see Section 33.7 for why that matters to subsumption).

| Concept | Holds when |
|---------|-----------|
| `std::same_as<T, U>` | `T` and `U` are the same type |
| `std::derived_from<D, B>` | `D` is publicly and unambiguously derived from `B` (or same) |
| `std::convertible_to<From, To>` | `From` is implicitly and explicitly convertible to `To` |
| `std::common_reference_with<T, U>` | `T` and `U` share a common reference type |
| `std::common_with<T, U>` | `T` and `U` share a common type |
| `std::integral<T>` | `T` is an integral type |
| `std::signed_integral<T>` | integral and signed |
| `std::unsigned_integral<T>` | integral and unsigned |
| `std::floating_point<T>` | `T` is a floating-point type |
| `std::assignable_from<L, R>` | `R` can be assigned to an lvalue of type `L` |
| `std::swappable<T>` | `std::ranges::swap` works on `T` |

```cpp
// Listing 33.1: composing core concepts
#include <concepts>

template<typename T>
concept Arithmetic = std::integral<T> || std::floating_point<T>;

template<typename From, typename To>
    requires std::convertible_to<From, To>
To narrow_cast(From f) { return static_cast<To>(f); }
```

Note `signed_integral` is defined as `integral<T> && std::is_signed_v<T>`, i.e. it is built *from* `integral` — this is what makes it **subsume** `integral` and win overload resolution for signed types.

---

## 33.3 Comparison Concepts

These underpin ordering and equality, and connect directly to the three-way comparison machinery of Chapter 39.

| Concept | Meaning |
|---------|---------|
| `std::equality_comparable<T>` | `==` and `!=` are valid and behave as an equivalence |
| `std::equality_comparable_with<T, U>` | cross-type equality |
| `std::totally_ordered<T>` | `<`, `>`, `<=`, `>=`, `==` give a total order |
| `std::totally_ordered_with<T, U>` | cross-type total order |
| `std::three_way_comparable<T>` | `<=>` is valid (with a given comparison category) |
| `std::three_way_comparable_with<T, U>` | cross-type spaceship |

```cpp
// Listing 33.2: constraining on comparability
#include <concepts>

template<std::totally_ordered T>
const T& max_of(const T& a, const T& b) {
    return (a < b) ? b : a;
}
```

---

## 33.4 Object and Lifetime Concepts: The Regular Hierarchy

This family captures the value-semantic categories that drive container and algorithm requirements. They form a layered hierarchy, each building on the previous:

```cpp
// Listing 33.3: the object-concept hierarchy (definitions paraphrased from the standard)
// movable        = move-constructible + move-assignable + swappable
// copyable       = movable + copy-constructible + copy-assignable
// semiregular    = copyable + default_initializable
// regular        = semiregular + equality_comparable
```

| Concept | Adds |
|---------|------|
| `std::movable<T>` | move construct/assign + swappable + object type |
| `std::copyable<T>` | + copy construct/assign |
| `std::semiregular<T>` | + default-initializable |
| `std::regular<T>` | + equality-comparable |

**`std::regular`** is the gold standard: a type that behaves like a built-in `int` — default-constructible, copyable, and equality-comparable. Constraining a generic container's element type on `std::semiregular` or `std::regular` documents and enforces exactly the value semantics you depend on.

```cpp
// Listing 33.4: a generic component that requires regular value semantics
#include <concepts>

template<std::regular T>
class ValueCache {
    T cached{};          // requires default_initializable
public:
    bool matches(const T& x) const { return x == cached; }  // requires equality_comparable
    void store(const T& x) { cached = x; }                   // requires copyable
};
```

---

## 33.5 Callable Concepts

These constrain function-like parameters — the modern replacement for unconstrained `F&&` template parameters in higher-order functions.

| Concept | Holds when |
|---------|-----------|
| `std::invocable<F, Args...>` | `f(args...)` is valid |
| `std::regular_invocable<F, Args...>` | invocable and equality-preserving (semantic) |
| `std::predicate<F, Args...>` | invocable and result is boolean-testable |
| `std::relation<R, T, U>` | binary predicate over `T` and `U` |
| `std::strict_weak_order<R, T, U>` | a relation modeling a strict weak ordering |

```cpp
// Listing 33.5: constraining a higher-order function on std::predicate
#include <concepts>
#include <vector>

template<typename T, std::predicate<const T&> Pred>
std::size_t count_if_simple(const std::vector<T>& v, Pred pred) {
    std::size_t n = 0;
    for (const auto& x : v) if (pred(x)) ++n;
    return n;
}
```

The difference between `invocable` and `predicate` is meaningful: `predicate` additionally requires the result to be usable as a boolean condition, so a callable returning `void` is `invocable` but not a `predicate` — and the diagnostic says so.

---

## 33.6 Iterator and Range Concepts

The iterator concepts (in `<iterator>`) formalize the historical iterator categories as actual concepts, and the range concepts (in `<ranges>`) build on them. This is the taxonomy the ranges library and `std::ranges::*` algorithms are written against.

| Iterator concept | Range concept | Capability |
|------------------|---------------|------------|
| `std::input_iterator` | `std::ranges::input_range` | single-pass read |
| `std::forward_iterator` | `std::ranges::forward_range` | multi-pass |
| `std::bidirectional_iterator` | `std::ranges::bidirectional_range` | `--` |
| `std::random_access_iterator` | `std::ranges::random_access_range` | `+ n`, `[]`, `<` |
| `std::contiguous_iterator` | `std::ranges::contiguous_range` | elements contiguous in memory |
| `std::output_iterator<T>` | `std::ranges::output_range<T>` | write `T` |

Related range concepts include `std::ranges::sized_range` (size in O(1)), `std::ranges::common_range` (begin/end same type), `std::ranges::view` (cheap to copy, non-owning), and `std::ranges::borrowed_range` (iterators outlive the range expression — Chapter 35).

```cpp
// Listing 33.6: constraining an algorithm on the right iterator strength
#include <ranges>
#include <concepts>

// Only compiles for ranges whose elements live contiguously (vector, array, span):
template<std::ranges::contiguous_range R>
auto* raw_data(R&& r) {
    return std::ranges::data(r);   // pointer to first element
}
```

Constraining on the *weakest* category that suffices is the discipline here: write algorithms against `input_range` if one pass is enough, and only demand `random_access_range` when you genuinely index or seek.

---

## 33.7 Constraint Normalization and Atomic Constraints

Before the compiler can compare two constraints for specificity, it **normalizes** them into a canonical form: a tree of conjunctions (`&&`) and disjunctions (`||`) of **atomic constraints**. An atomic constraint is an expression that cannot be broken down further — typically the expansion of a single concept's body down to its irreducible boolean expressions, paired with the mapping of template parameters.

The crucial rule: **two atomic constraints are considered identical only if they come from the same source expression** (the same concept definition, syntactically). This is why `std::same_as` is defined the way it is, and why the standard concepts are layered (`signed_integral` literally contains `integral`'s atomic constraint). Subsumption can only "see through" composition that is expressed in terms of concepts and `&&`/`||`; it cannot reason about arbitrary `bool` expressions.

```cpp
// Listing 33.7: layering makes the atomic constraint of 'integral' appear inside 'signed_integral'
// std::integral<T>        => is_integral_v<T>                    (atomic A)
// std::signed_integral<T> => integral<T> && is_signed_v<T>      (atomic A && atomic B)
// Because signed_integral CONTAINS A, it subsumes integral.
```

---

## 33.8 Subsumption and the Partial Ordering of Constraints

**Subsumption** is the formal relation used to decide that one constrained declaration is *at least as constrained* as another. Constraint *P* subsumes constraint *Q* if, by the rules of normalization, *P*'s normal form logically implies *Q*'s. When two function template overloads are both viable and one's constraints subsume the other's (but not vice versa), the **more-constrained** overload is selected.

```cpp
// Listing 33.8: subsumption selects the most-constrained viable overload
#include <concepts>
#include <iostream>

template<std::integral T>          // less constrained
void process(T) { std::cout << "integral\n"; }

template<std::signed_integral T>   // more constrained: subsumes integral
void process(T) { std::cout << "signed_integral\n"; }

int main() {
    process(42);    // int is signed -> "signed_integral" (more constrained wins)
    process(42u);   // unsigned -> only the integral overload is viable -> "integral"
}
```

This is the mechanism that lets you write a family of overloads from general to specific and trust the compiler to dispatch to the tightest match — the clean replacement for the carefully hand-tuned, mutually-exclusive `enable_if` conditions that the same effect once required.

---

## 33.9 Pitfalls: When Subsumption Does Not Apply

Subsumption operates on **concepts and parenthesized atomic constraints**, not on type traits used directly. Two common surprises:

```cpp
// Listing 33.9: type-trait constraints do NOT subsume each other
#include <type_traits>

template<typename T>
    requires std::is_integral_v<T>
void f(T);   // (1)

template<typename T>
    requires (std::is_integral_v<T> && std::is_signed_v<T>)
void f(T);   // (2)

// f(42) is AMBIGUOUS: is_signed_v is a different atomic constraint, and the two
// requires-clauses are unrelated atomic expressions -> neither subsumes the other.
```

The fix is to wrap the traits in **named concepts** so the layering is visible to normalization:

```cpp
// Listing 33.10: wrapping traits in concepts restores subsumption
#include <concepts>

template<std::integral T> void g(T);          // (1)
template<std::signed_integral T> void g(T);   // (2) subsumes (1)
// g(42) -> calls (2) unambiguously.
```

The second classic pitfall: `&&` in a requires-clause builds a *conjunction of atomic constraints* (good for subsumption), but writing the same logic as a single `bool` (e.g., a helper `constexpr bool`) collapses it into **one opaque atomic constraint** that subsumes nothing. Always express constraint composition with concepts and `&&`/`||`, never by precomputing a `bool`.

---

## 33.10 Professional Insights

**Build from the standard concepts; do not reinvent them.** `std::regular`, `std::invocable`, `std::totally_ordered`, and the iterator/range concepts encode subtle, correct requirements that took the committee years to settle. Composing them makes your interfaces interoperate with `std::ranges` algorithms automatically and spares you from re-deriving, say, the exact difference between `movable` and `copyable`.

**Constrain on the weakest sufficient concept.** Demanding `random_access_range` when `forward_range` would do needlessly excludes valid callers (lists, forward views). The strength of your constraint is part of your API contract — make it exactly as strong as the algorithm requires, no stronger.

**Express composition with concepts and `&&`/`||`, never with a precomputed `bool`.** Subsumption can only order constraints that are visible to normalization. The moment you collapse a constraint into a single `constexpr bool` helper, you get one opaque atomic constraint that subsumes nothing — and your general/specific overloads become ambiguous instead of ordered. This is the single most common reason "my more-constrained overload isn't being picked."

**Wrap type traits in named concepts at interface boundaries.** A bare `requires std::is_integral_v<T>` works but does not participate in subsumption against other trait-based constraints. Promoting traits to concepts (`std::integral`, or your own named wrapper) is what turns a pile of unrelated `enable_if`-style conditions into a properly ordered overload set.

**Reach for `std::regular` as the default element constraint.** When writing a generic container or value-holding component, `std::regular` (or `std::semiregular` when default-construction is needed but equality is not) precisely captures "behaves like a built-in value type." It documents the contract, enforces it at the boundary, and produces a clean diagnostic when a caller supplies a move-only or non-comparable type.
