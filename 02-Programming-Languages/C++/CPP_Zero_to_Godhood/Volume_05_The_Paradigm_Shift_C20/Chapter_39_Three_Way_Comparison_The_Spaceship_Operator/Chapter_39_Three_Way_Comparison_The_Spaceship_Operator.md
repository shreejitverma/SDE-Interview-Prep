# Chapter 39: Three-Way Comparison — The Spaceship Operator

> *C++20's three-way comparison operator `<=>` — the "spaceship" — replaces the six hand-written comparison operators most types needed with a single defaulted declaration, and gives the language a principled vocabulary for the three kinds of ordering a type can have. This chapter covers the spaceship operator, the ordering category types it returns, defaulted versus custom comparisons, the operator-rewriting rules that make `a < b` work from a lone `<=>`, and the performance subtleties that matter when you write comparisons by hand.*

Before C++20, a type that wanted to be ordered had to define `==`, `!=`, `<`, `>`, `<=`, and `>=` — six operators, each a maintenance hazard and an opportunity for inconsistency. The spaceship operator collapses this to one function whose return value encodes the full ordering, and the compiler **rewrites** the relational operators in terms of it. Equally important is the type system around it: `strong_ordering`, `weak_ordering`, and `partial_ordering` let a type state precisely what kind of order it provides, which the standard library reads to make correct decisions.

---

## Table of Contents

- [39.1 The Problem: Six Operators Per Type](#391-the-problem-six-operators-per-type)
- [39.2 The Spaceship Operator and Defaulted Comparison](#392-the-spaceship-operator-and-defaulted-comparison)
- [39.3 The Three Ordering Categories](#393-the-three-ordering-categories)
- [39.4 Operator Rewriting and Synthesized Candidates](#394-operator-rewriting-and-synthesized-candidates)
- [39.5 == Is Special: Why Equality Is Separate](#395--is-special-why-equality-is-separate)
- [39.6 Custom Three-Way Comparisons](#396-custom-three-way-comparisons)
- [39.7 Member-wise Semantics and Ordering Strength Deduction](#397-member-wise-semantics-and-ordering-strength-deduction)
- [39.8 Performance Considerations](#398-performance-considerations)
- [39.9 Professional Insights](#399-professional-insights)

---

## 39.1 The Problem: Six Operators Per Type

A fully ordered C++ type historically required six operator overloads, conventionally implemented by routing five of them through one or two primitives. The boilerplate was mechanical, error-prone (an inverted comparison in one of six bodies), and forced library authors toward CRTP helpers like `boost::operators` just to avoid repetition.

```cpp
// Listing 39.1: the pre-C++20 boilerplate — six operators by hand
struct Version {
    int major, minor, patch;

    bool operator==(const Version& o) const {
        return major==o.major && minor==o.minor && patch==o.patch;
    }
    bool operator!=(const Version& o) const { return !(*this == o); }
    bool operator<(const Version& o) const {
        if (major != o.major) return major < o.major;
        if (minor != o.minor) return minor < o.minor;
        return patch < o.patch;
    }
    bool operator>(const Version& o) const  { return o < *this; }
    bool operator<=(const Version& o) const { return !(o < *this); }
    bool operator>=(const Version& o) const { return !(*this < o); }
};
```

Every line is a place to introduce an asymmetry bug, and the lexicographic `<` must be kept manually consistent with `==`. C++20 eliminates the entire block.

---

## 39.2 The Spaceship Operator and Defaulted Comparison

The **three-way comparison operator** `<=>` compares two values and returns an *ordering* object describing their relationship. Defaulting it generates a member-wise lexicographic comparison and, crucially, makes the compiler synthesize all the relational operators from it.

```cpp
// Listing 39.2: the entire Listing 39.1 replaced by two defaulted lines
#include <compare>

struct Version {
    int major, minor, patch;
    auto operator<=>(const Version&) const = default;   // generates <, >, <=, >=
    bool operator==(const Version&) const = default;    // generates ==, !=
};

// All of these now compile and are correct:
//   v1 < v2,  v1 >= v2,  v1 == v2,  v1 != v2, ...
```

`= default` on `<=>` produces a **member-wise, in-declaration-order, lexicographic** comparison: it compares `major`, then `minor`, then `patch`, stopping at the first member that differs — exactly the hand-written logic, generated correctly by construction. Including `<compare>` is required: it defines the ordering return types.

The minimal complete spelling for a type that wants the full set of comparisons is the two defaulted declarations above; Section 39.5 explains why `==` is listed separately.

---

## 39.3 The Three Ordering Categories

`<=>` does not return a `bool` or an `int` — it returns one of three **ordering category** types from `<compare>`, each expressing a different strength of order. The category is itself meaningful: it tells generic code what guarantees the comparison provides.

| Return type | Meaning | Key property |
|-------------|---------|--------------|
| `std::strong_ordering` | total order; equal values are **substitutable** | `a == b` ⇒ `f(a)` and `f(b)` indistinguishable |
| `std::weak_ordering` | total order; equivalent values need **not** be substitutable | equivalence classes (e.g. case-insensitive strings) |
| `std::partial_ordering` | some pairs are **unordered** | floating point: `NaN <=> x` is `unordered` |

```cpp
// Listing 39.3: the named result values of each category
#include <compare>

void inspect(std::strong_ordering o) {
    if (o == std::strong_ordering::less)    { /* a < b */ }
    if (o == std::strong_ordering::equal)   { /* a == b, fully substitutable */ }
    if (o == std::strong_ordering::greater) { /* a > b */ }
}

void inspect_fp(std::partial_ordering o) {
    if (o == std::partial_ordering::unordered) { /* e.g. involves NaN */ }
}
```

`strong_ordering` has `less`/`equal`/`greater`; `weak_ordering` has `less`/`equivalent`/`greater`; `partial_ordering` adds `unordered`. The distinction between `equal` (strong) and `equivalent` (weak) is the substitutability question: two case-insensitively-equal strings are *equivalent* but not *equal* — `"Foo"` and `"foo"` compare equivalent yet are observably different. Built-in integers yield `strong_ordering`; `float`/`double` yield `partial_ordering` because of NaN.

---

## 39.4 Operator Rewriting and Synthesized Candidates

The reason one `<=>` suffices is **operator rewriting**: when the compiler sees a relational expression and finds no exact operator, it rewrites it in terms of `<=>` compared against the literal `0`.

```cpp
// Listing 39.4: how the compiler rewrites relational operators
// Source you write:          Compiler rewrites to:
//   a < b           →          (a <=> b) < 0
//   a > b           →          (a <=> b) > 0
//   a <= b          →          (a <=> b) <= 0
//   a >= b          →          (a <=> b) >= 0
//   a > b           →          0 < (b <=> a)    // synthesized (reversed) candidate
```

Comparing an ordering object against `0` is well-defined: `less` is "< 0", `greater` is "> 0", `equal`/`equivalent` is "== 0". The compiler also considers **synthesized (reversed) candidates** — if you wrote `a <=> b` for a heterogeneous pair, `b < a` can be answered by reversing `a <=> b`. This is why a single `friend auto operator<=>(const A&, const B&)` makes *all eight* relational comparisons between `A` and `B` (in both orders) work. The rewrite happens only for `<`, `>`, `<=`, `>=`; equality has its own path.

---

## 39.5 == Is Special: Why Equality Is Separate

A defaulted `<=>` does **not** by itself generate `==`. Equality and ordering are decoupled in C++20 for both performance and semantic reasons, so `==`/`!=` are rewritten only from `operator==`, never from `<=>`.

```cpp
// Listing 39.5: equality must be defaulted separately (and often should be)
#include <compare>
#include <string>
#include <vector>

struct Record {
    int id;
    std::vector<std::string> tags;

    // Ordering: lexicographic over members.
    auto operator<=>(const Record&) const = default;
    // Equality: defaulted separately — and far cheaper for early mismatch.
    bool operator==(const Record&) const = default;
};
```

The motivation is performance, especially for containers like `std::string` and `std::vector`: `==` can **short-circuit on size** (different lengths ⇒ unequal, in O(1)), whereas `<=>` must compare element-by-element to determine order. Forcing `==` to go through `<=>` would discard that optimization. There is a convenience, though: if you default `<=>` and do *not* declare `==`, the compiler will **also implicitly default `==`** for you when the class has no `<=>` written by hand other than the defaulted one — but the explicit, robust habit is to default both. `!=` is always rewritten from `==`.

---

## 39.6 Custom Three-Way Comparisons

When the default member-wise order is wrong, write `<=>` by hand and return the appropriate category. The body typically delegates to members' own `<=>`.

```cpp
// Listing 39.6: a custom <=> with a chosen ordering category
#include <compare>
#include <string>
#include <cctype>

struct CaseInsensitive {
    std::string s;

    std::weak_ordering operator<=>(const CaseInsensitive& o) const {
        // Equivalent (not equal) values: "Foo" and "foo" -> weak_ordering.
        auto lower = [](std::string x){
            for (char& c : x) c = static_cast<char>(std::tolower(c));
            return x;
        };
        return lower(s) <=> lower(o.s);   // string's <=> is strong; we narrow to weak
    }
    bool operator==(const CaseInsensitive& o) const {
        return (*this <=> o) == 0;
    }
};
```

Choosing the return category is a design decision, not a formality: returning `weak_ordering` documents that equivalent-but-distinct values exist, which generic code (and the next maintainer) can rely on. You can also use the standard helper `std::compare_three_way` for generic contexts, and `std::strong_order` / `std::weak_order` / `std::partial_order` customization-point objects to obtain an ordering for types (including floating point with a total order) in a uniform way.

---

## 39.7 Member-wise Semantics and Ordering Strength Deduction

When `<=>` is defaulted with `auto` return, the compiler **deduces the common ordering category** from the members. The result is the *weakest* category among all members' comparison categories — the common type computed by `std::common_comparison_category`.

```cpp
// Listing 39.7: deduced category is the weakest among members
#include <compare>

struct A { int n;    auto operator<=>(const A&) const = default; }; // strong (int)
struct B { double d; auto operator<=>(const B&) const = default; }; // partial (double)

struct Mixed {
    int    n;     // contributes strong_ordering
    double d;     // contributes partial_ordering
    auto operator<=>(const Mixed&) const = default;
    // deduced return type: std::partial_ordering  (weakest member wins)
};
```

This deduction is why an `auto` return on a defaulted `<=>` is usually right: the type that comes out correctly reflects the strongest guarantee the data actually supports. If you want to *assert* a specific category, name it explicitly as the return type instead of `auto` — the compiler then verifies the members can supply at least that strength and errors if they cannot.

---

## 39.8 Performance Considerations

Three-way comparison is generally **zero-overhead** versus hand-written operators when defaulted — the compiler generates the same member-wise comparison you would have written, fully inlined. The subtleties that matter in hot paths:

- **Prefer `==` for equality checks on containers.** Because `==` short-circuits on size while `<=>` does not, `if (a == b)` on vectors/strings can be dramatically faster than `if ((a <=> b) == 0)`. Default both operators so the fast `==` path exists.
- **A single `<=>` call yields all relations.** When you need ordering, computing `auto c = a <=> b;` once and branching on `c` against `0` avoids recomputing the comparison for `<` then `>` then `==` — relevant in sort comparators and tree-balancing code where the comparison runs in the inner loop.
- **`partial_ordering` carries an extra state.** Comparisons that can be `unordered` must represent four outcomes, not three; in the rare hot loop over floating-point keys, know that `partial_ordering` branching is marginally heavier than `strong_ordering`, and that NaN keys make ordered containers ill-formed regardless.
- **Defaulted comparisons are `constexpr`-friendly.** A defaulted `<=>`/`==` is usable in constant expressions when the members are, enabling compile-time ordered lookups.

---

## 39.9 Professional Insights

**Default both `<=>` and `==`; do not rely on the implicit `==`.** While the language will implicitly default `==` alongside a defaulted `<=>` in the common case, writing both `auto operator<=>(...) const = default;` and `bool operator==(...) const = default;` is the unambiguous, review-friendly habit. It also guarantees the size-short-circuiting fast `==` exists for container members, which the implicit path may not make obvious to readers.

**Choose the ordering category deliberately — it is part of your type's contract.** `strong_ordering` promises substitutability; `weak_ordering` signals equivalence classes; `partial_ordering` admits unordered pairs. Generic algorithms and the next engineer read this. A case-insensitive string returning `strong_ordering` is a latent bug, because it claims `"Foo"` and `"foo"` are interchangeable when they are not. Return `weak_ordering` and say what you mean.

**Compute `<=>` once and reuse the result in comparison-heavy code.** In sort comparators, balanced-tree inserts, and merge loops, capture `auto c = a <=> b;` and branch on `c` rather than issuing separate `<` and `==` calls. This halves the comparison work in the hottest loops — exactly where ordering cost concentrates in HFT-style sorted structures.

**Remember floating-point keys yield `partial_ordering`, and NaN breaks ordered containers.** A struct with a `double` member deduces `partial_ordering`, and a `NaN` key makes `std::map`/`std::set` behavior undefined because the strict-weak-ordering precondition is violated. If you must key on floating point, either guarantee no NaNs or use `std::strong_order` (which imposes a total order over all floats, including NaN) as the comparator.

**Reach for `<=>` to delete CRTP comparison helpers and reduce ABI surface.** Pre-C++20 codebases carried `boost::operators`-style mixins or hand-rolled six-operator blocks purely to avoid boilerplate. Replacing them with defaulted `<=>`/`==` removes that machinery, shrinks the inline footprint, and eliminates a whole class of "one of six operators is subtly inconsistent" bugs — a clean, low-risk modernization win.
