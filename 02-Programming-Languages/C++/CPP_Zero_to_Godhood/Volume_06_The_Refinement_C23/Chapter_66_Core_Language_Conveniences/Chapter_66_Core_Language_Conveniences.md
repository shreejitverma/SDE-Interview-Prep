# Chapter 66: Core Language Conveniences

> Beyond deducing `this` (Chapter 55) and `if consteval` (Chapter 65), C++23 makes a dozen smaller core-language changes, each removing a specific papercut. Multidimensional `operator[]` enables `m[i, j]`; static `operator()`/`operator[]` shrink stateless functors; `auto(x)` gives a clean decay-copy; `z`/`uz` literals end the signed/unsigned loop-counter warning; the preprocessor gains `#elifdef`/`#elifndef`/`#warning`; string literals gain named and delimited escapes; the range-`for` temporary-lifetime footgun is fixed; `[[assume]]` standardizes optimization hints; and several quieter cleanups land. This chapter is the catalog of those conveniences.

## Table of Contents

1. [Multidimensional `operator[]`](#661-multidimensional-operator)
2. [Static `operator()` and `operator[]`](#662-static-operator-and-operator)
3. [`auto(x)` and `auto{x}` — Explicit Decay-Copy](#663-autox-and-autox--explicit-decay-copy)
4. [`size_t` Literals: `z` and `uz`](#664-size_t-literals-z-and-uz)
5. [Preprocessor: `#elifdef`, `#elifndef`, `#warning`](#665-preprocessor-elifdef-elifndef-warning)
6. [String-Literal Escapes: Named and Delimited](#666-string-literal-escapes-named-and-delimited)
7. [Range-`for` Temporary Lifetime, `[[assume]]`, and Other Cleanups](#667-range-for-temporary-lifetime-assume-and-other-cleanups)
8. [Professional Insights](#668-professional-insights)

---

## 66.1 Multidimensional `operator[]`

Before C++23, `operator[]` could take exactly **one** argument, so a matrix or tensor type had to fake multi-index access with `operator()` (`m(i, j)`) or chained `[]` returning proxy objects (`m[i][j]`, with a proxy per level). C++23 lets `operator[]` take **any number of arguments**, so `m[i, j]` and `cube[i, j, k]` are first-class:

```cpp
#include <vector>
#include <cstddef>

struct Matrix {
    std::vector<double> data;
    std::size_t cols;

    double& operator[](std::size_t r, std::size_t c) {   // multidimensional subscript
        return data[r * cols + c];
    }
};

int main() {
    Matrix m{std::vector<double>(12), 4};
    m[1, 2] = 3.14;        // clean two-index syntax
}
```

This is the core-language feature that `std::mdspan` (Chapter 57) is built on. Note a related cleanup: the old comma-operator-inside-`[]` meaning (`a[b, c]` once meant `a[(b, c)]`) was deprecated in C++20 precisely to free this syntax, and C++23 now gives it the multidimensional meaning.

---

## 66.2 Static `operator()` and `operator[]`

A stateless functor — a lambda or a function object with no captures or data members — still, pre-C++23, received a hidden `this` pointer on every call, even though it had no state to access. C++23 lets you declare `operator()` (and `operator[]`) as **`static`**, so no `this` is passed; the call becomes an ordinary static dispatch.

```cpp
struct Hash {
    static std::size_t operator()(int x) { return x * 2654435761u; }  // no implicit this
};

// A stateless lambda can request the same:
auto doubler = [](int x) static { return x * 2; };
```

For a comparator or hash functor passed to an algorithm and invoked in a hot loop, dropping the unused `this` parameter can reduce register pressure and enable slightly better codegen. It is a micro-optimization, but a free one for stateless callables, and it documents the absence of state.

---

## 66.3 `auto(x)` and `auto{x}` — Explicit Decay-Copy

Generic code sometimes needs to force a **decay-copy**: produce a prvalue copy of an lvalue, stripping references and cv-qualifiers, so that subsequent operations act on an independent value rather than aliasing the original. The standard library had an internal `decay-copy` helper for this; C++23 exposes it as syntax: **`auto(x)`** (and the braced `auto{x}`) creates a prvalue decay-copy of `x`.

The classic motivation is mutating a container while referring to one of its own elements: passing a *reference* to an element into an operation that may reallocate or erase is a self-aliasing bug, but `auto(x)` makes an independent copy first.

```cpp
#include <vector>
#include <algorithm>

void demo(std::vector<int>& v) {
    // Erase all elements equal to the FIRST element's value.
    // auto(v.front()) copies the value before erase invalidates it.
    std::erase(v, auto(v.front()));
}
```

`auto(x)` is more readable than the old workarounds (`static_cast<std::decay_t<decltype(x)>>(x)` or an explicit named copy) and makes the intent — "I want an independent copy here" — explicit at the point of use.

---

## 66.4 `size_t` Literals: `z` and `uz`

The signed/unsigned mismatch in loop counters is a perennial source of compiler warnings: `for (int i = 0; i < v.size(); ++i)` compares a signed `int` to an unsigned `size_t`. Writing the counter as `size_t` meant either an unwieldy `std::size_t i = 0` or a cast. C++23 adds literal suffixes:

- **`z`** produces a `std::ptrdiff_t` (the *signed* `size_t`-width type).
- **`uz`** (or `zu`) produces a `std::size_t` (unsigned).

```cpp
#include <vector>

void demo(const std::vector<int>& v) {
    for (auto i = 0uz; i < v.size(); ++i) { /* i is size_t — no warning */ }
    for (auto j = 0z;  j < std::ssize(v); ++j) { /* j is ptrdiff_t (signed) */ }
}
```

With `0uz`, the loop counter's type matches `size()` exactly, so the comparison is warning-free and the index type is correct without a cast. The `z` suffix pairs naturally with `std::ssize` (Volume 5) when you want a *signed* index.

---

## 66.5 Preprocessor: `#elifdef`, `#elifndef`, `#warning`

The preprocessor gains three long-overdue directives:

- **`#elifdef NAME`** is shorthand for `#elif defined(NAME)`, and **`#elifndef NAME`** for `#elif !defined(NAME)` — flattening the deeply-nested `#if/#else/#if` chains that platform and feature detection otherwise require.
- **`#warning "message"`** emits a diagnostic *warning* (not an error) at preprocessing time — the standardization of a directive every major compiler already supported as an extension. It is the counterpart to the long-standard `#error`.

```cpp
#ifdef USE_BACKEND_A
    // ...
#elifdef USE_BACKEND_B          // was: #elif defined(USE_BACKEND_B)
    // ...
#elifndef DISABLE_BACKEND       // was: #elif !defined(DISABLE_BACKEND)
    #warning "No backend selected; falling back to the reference implementation"
#endif
```

These are small, but they make conditional-compilation blocks — which every cross-platform codebase has in abundance — markedly cleaner and let you surface non-fatal configuration notices through the standard `#warning`.

---

## 66.6 String-Literal Escapes: Named and Delimited

C++23 modernizes how characters are spelled inside string and character literals:

- **Named universal character escapes:** `\N{NAME}` denotes a Unicode code point by its official Unicode *name* rather than its hex value. `"\N{GREEK SMALL LETTER ALPHA}"` is `α`, and `"\N{LATIN SMALL LETTER E WITH ACUTE}"` is `é` — self-documenting where `α` is opaque.
- **Delimited escape sequences:** the numeric escapes gain brace-delimited forms that remove the old ambiguity about where the escape ends. `\x{...}` for hex, `\o{...}` for octal, and `\u{...}` for a Unicode code point of any width: `"\x{1F600}"`, `"\o{777}"`, `"\u{1F600}"`. The braces make the boundary explicit, fixing the classic `"\x41B"` problem where it is unclear whether `B` is part of the hex value or a following character.

```cpp
#include <print>

int main() {
    std::println("{}", "caf\N{LATIN SMALL LETTER E WITH ACUTE}");  // café
    std::println("{}", "\u{1F600}");                                // 😀 (delimited)
    char c = '\x{41}';                                              // 'A', unambiguous
    std::println("{}", c);
}
```

These remove a real class of bugs in code that embeds Unicode or precise byte values in literals — common in protocol, rendering, and internationalization code.

---

## 66.7 Range-`for` Temporary Lifetime, `[[assume]]`, and Other Cleanups

Several remaining changes fix footguns and standardize hints:

- **Lifetime extension of temporaries in range-`for`.** The notorious `for (auto e : getVector()[0])` bug — where a temporary in the range *initializer* (here the `vector` returned by `getVector()`, of which `[0]` is taken) was destroyed before the loop ran, leaving the loop iterating a dangling range — is **fixed**. C++23 extends the lifetime of *all* temporaries in the range expression to cover the entire loop. Code that was silently undefined now works as written.
- **`[[assume(expr)]]`** standardizes the optimization hint that `expr` is `true`, replacing vendor-specific `__builtin_assume` / `__assume`. Like `std::unreachable` (Chapter 64), it is a promise to the optimizer, not a runtime check — a false assumption is UB.
- **Simpler implicit move.** A move-eligible id-expression in a `return` or `throw` is now consistently treated as an xvalue, so more returns move rather than copy without an explicit `std::move`, and the rules are simpler than the tangle C++20 left.
- **Attributes on lambdas** are now permitted on the lambda's `operator()` (e.g. `[](int x) [[nodiscard]] { ... }`).
- **CTAD from inherited constructors**, **mandated UTF-8 as the source encoding** for the basic character set, **whitespace trimming before line-splice (`\`) continuations**, and the **removal of garbage-collection support** (the never-implemented `std::declare_reachable` family and the `pointer_safety` machinery from C++11) are the quieter cleanups that round out the release.

**Listing 66.1: Fixed range-`for` lifetime and `[[assume]]`.**

```cpp
#include <vector>
#include <print>

std::vector<std::vector<int>> getGrid() { return {{1, 2, 3}, {4, 5, 6}}; }

int divide(int x) {
    [[assume(x > 0)]];          // promise the optimizer x is positive
    return 100 / x;             // lets it skip the divide-by-zero / sign handling
}

int main() {
    // The temporary grid from getGrid() now lives for the whole loop (C++23).
    for (int e : getGrid()[0])  // was a dangling-range bug pre-C++23
        std::print("{} ", e);   // 1 2 3
    std::println("");
    std::println("{}", divide(4));   // 25
}
```

> **Version-trap flag:** every feature in this chapter — multidimensional `operator[]`, static `operator()`/`operator[]`, `auto(x)`, `z`/`uz` literals, `#elifdef`/`#elifndef`/`#warning`, named/delimited escapes, the range-`for` lifetime fix, `[[assume]]`, simpler implicit move, attributes on lambdas, CTAD-from-inherited-constructors, and the GC-support removal — is **C++23**. The range-`for` fix in particular *changes the behavior of existing code* (from UB to defined), which is a rare and welcome kind of breaking change.

---

## 66.8 Professional Insights

**The range-`for` temporary-lifetime fix is the one to internalize, because it silently repairs latent bugs.** The `for (auto x : f().g())` pattern was undefined behavior whenever `f()` returned a temporary container, and it bit people regularly — often intermittently, since it sometimes "worked." C++23 extends every temporary in the range expression to the loop's lifetime, turning that UB into correct behavior. When you move a codebase to `-std=c++23`, this is a free correctness upgrade; just be aware that code which appeared to work by luck is now actually guaranteed to.

**Adopt `m[i, j]` and `std::mdspan` together; they are the same idea at two levels.** The multidimensional subscript is the language hook, and `mdspan` is its flagship library consumer. For any type that models a grid, matrix, or tensor, define `operator[](i, j, ...)` so call sites read like the mathematics, and prefer `mdspan` over hand-rolled index arithmetic for views over flat buffers. The combination eliminates the proxy-object `m[i][j]` hacks and the duplicated stride math that plagued numeric C++.

**Use `0uz`/`0z` and `auto(x)` as small, deliberate clarity wins.** The `uz`/`z` suffixes make loop-counter types match `size()`/`ssize()` exactly, retiring the signed/unsigned warning without casts; `auto(x)` states "independent copy" at the point of use and prevents self-aliasing bugs when mutating a container by one of its own elements. These are minor individually, but they remove exactly the kinds of subtle mismatch and aliasing errors that survive code review.

**Treat `[[assume]]` with the same caution as `std::unreachable`: it is a UB contract, not an assertion.** A correct `[[assume(expr)]]` genuinely speeds up hot code by letting the optimizer drop checks and specialize on the assumed fact, but a *false* assumption is undefined behavior, not a caught error. Use it only where the condition is provably true (typically just after validation), document why, and never as a substitute for a real check on untrusted input. The named/delimited escapes and `#warning`, by contrast, are pure safety and clarity wins you can adopt without reservation.
