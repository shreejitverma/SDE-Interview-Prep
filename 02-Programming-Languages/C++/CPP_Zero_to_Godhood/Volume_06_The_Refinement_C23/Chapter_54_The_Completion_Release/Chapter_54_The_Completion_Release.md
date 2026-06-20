# Chapter 54: C++23 — The Completion Release

> C++20 was a revolution: concepts, ranges, coroutines, and modules arrived together and changed the shape of the language. But a revolution leaves loose ends. C++23 is the release that ties them off. It is not defined by one headline feature; it is defined by *closure* — finishing the standard library's coroutine story with `std::generator`, completing the ranges library that shipped half-built, giving error handling a first-class vocabulary with `std::expected`, and making the day-one ergonomics (`std::print`, `import std;`, deducing `this`) finally usable. This chapter frames the volume: what kind of release C++23 is, why "ergonomics and completion" is the right lens, and how the chapters ahead are organized.

## Table of Contents

1. [The Shape of a Completion Release](#541-the-shape-of-a-completion-release)
2. [The Three Debts C++23 Repays](#542-the-three-debts-c23-repays)
3. [What C++23 Is *Not*](#543-what-c23-is-not)
4. [Why This Matters for Low-Latency and Systems Work](#544-why-this-matters-for-low-latency-and-systems-work)
5. [How This Volume Is Organized](#545-how-this-volume-is-organized)
6. [Compiler and Library Support in 2026](#546-compiler-and-library-support-in-2026)
7. [Professional Insights](#547-professional-insights)

---

## 54.1 The Shape of a Completion Release

The C++ standards committee has settled into a three-year cadence, and within that cadence the releases alternate in character. **C++11 and C++20 were *major* releases** — each redefined how idiomatic C++ is written. **C++14, C++17, and C++23 are *minor* releases** in the formal sense, meaning they introduce no single feature on the scale of move semantics or concepts. But "minor" badly understates C++23. It is the release that takes the half-finished cathedral of C++20 and installs the doors, the windows, and the plumbing.

Consider the state of C++20 as actually shipped:

- **Coroutines** were standardized as a *language* mechanism, but the standard library shipped *zero* coroutine types. You could write `co_yield`, but you had to hand-roll the promise type, the `generator`, and the iterator glue yourself. C++23 ships `std::generator`.
- **Ranges** shipped with a foundational set of views and algorithms, but the everyday tools — `zip`, `enumerate`, `chunk`, `slide`, fold algorithms, and the all-important `ranges::to` for materializing a view into a container — were cut for time. C++23 ships them.
- **`std::format`** gave us a type-safe formatting engine, but no convenient way to actually *write* the result to a stream or the console. You still reached for `std::cout << std::format(...)`. C++23 ships `std::print` and `std::println`.

A completion release is one whose value is measured not by what it adds but by what it *finishes*. The features below are deliberately unglamorous: each one removes a workaround you have been carrying since 2020.

---

## 54.2 The Three Debts C++23 Repays

It helps to group the work of C++23 around three outstanding debts.

### 54.2.1 The Ergonomics Debt

Modern C++ had accumulated boilerplate that everyone tolerated because the alternatives were worse. **Deducing `this`** (Chapter 55) collapses the four-overload ref-qualifier explosion and the CRTP boilerplate into a single explicit object parameter. **`std::print`** (Chapter 61) ends the `cout`-versus-`printf` compromise. **`import std;`** (Chapter 68) replaces dozens of `#include` lines with one import. **`size_t` literals** and **multidimensional `operator[]`** (Chapter 66) remove papercuts that have nagged numeric code for a decade.

### 54.2.2 The Error-Handling Debt

C++ has had two unsatisfying options for reporting failure: exceptions (unpredictable control flow, banned in many low-latency and embedded codebases) and sentinel/`optional` returns (which discard *why* the operation failed). **`std::expected<T, E>`** (Chapter 56) gives a value-based channel that carries the error reason, and the **monadic operations** — `and_then`, `transform`, `or_else` — let you compose fallible operations without the nested-`if` "pyramid of doom." This is arguably the most consequential library addition in the release for systems programmers.

### 54.2.3 The Standard-Library-Completeness Debt

The largest body of work is simply *finishing the libraries C++20 started*. `std::generator` completes coroutines (Chapter 58). The new range views and fold algorithms complete ranges (Chapters 59–60). Flat containers (Chapter 62), `std::stacktrace` (Chapter 63), `std::move_only_function` (Chapter 64), and the extended floating-point types (Chapter 67) fill long-standing gaps that forced teams onto Boost, Abseil, or in-house code.

---

## 54.3 What C++23 Is *Not*

It is as important to know the boundaries of this release as its contents.

- **C++23 is not reflection.** Static reflection and code injection were targeted but did not land; they are C++26 material and are covered in the next volume.
- **C++23 is not `std::execution` (senders/receivers).** The structured-concurrency framework slipped to C++26.
- **C++23 is not contracts.** Contract programming was repeatedly deferred and is, again, C++26.
- **C++23 is not pattern matching.** The `inspect` proposal did not make it.

Throughout this volume you will see **version-trap callouts** flagging features that look like they belong to C++23 but are actually C++26 — a discipline that matters enormously when you target a specific `-std` flag in production. The source folder for this volume contained a "next frontier" chapter describing exactly these C++26 features; that material is deliberately left out of this volume and reserved for the C++26 volume.

---

## 54.4 Why This Matters for Low-Latency and Systems Work

For the high-frequency trading, kernel, and large-scale-distributed audiences this book targets, C++23 is unusually well-aligned with your constraints:

- **`std::expected`** gives you exception-free error propagation with zero hidden allocation and a deterministic control-flow cost — exactly what a hot path or a `-fno-exceptions` build needs.
- **Flat containers** trade pointer-chasing trees for cache-friendly contiguous storage, the same transformation you have been doing by hand with sorted `vector`s.
- **`std::mdspan`** (Chapter 57) gives you a zero-overhead, non-owning, layout-aware multidimensional view — the missing primitive for numerical kernels that previously demanded a third-party tensor library.
- **`std::print`** writes directly to the underlying file descriptor without constructing an intermediate `std::string`, making formatted logging cheaper.
- **Expanded `constexpr`** (Chapter 65) moves more work to compile time, including `constexpr` `std::unique_ptr` and a swath of `<cmath>`.

The recurring theme: C++23 lets you delete the workaround and keep the performance.

---

## 54.5 How This Volume Is Organized

The chapters are sequenced from highest-impact to most-specialized:

| Chapters | Theme |
|---|---|
| 55 | Deducing `this` — the marquee core-language feature |
| 56–58 | The big library completions: `expected`, `mdspan`, `generator` |
| 59–60 | Completing the ranges library: new views, then folds/`ranges::to`/search |
| 61–62 | Modern output (`print`) and flat containers |
| 63–64 | Diagnostics, lifetime utilities, and functional/type utilities |
| 65–66 | Compile-time refinements and the remaining core-language conveniences |
| 67–68 | Extended floating-point types and standard-library modules |

Every chapter follows the same structure: motivation (the problem the feature solves), mechanics (how it works and the rules that govern it), edge cases and pitfalls, a low-latency/performance perspective, and complete worked code. Where C++23 merely finishes a C++20 feature, we recap just enough of the C++20 baseline to make the delta clear — the full treatment lives in Volume 5.

---

## 54.6 Compiler and Library Support in 2026

As of this writing, C++23 is broadly usable in production with attention to a few gaps:

- **Core language** features (deducing `this`, `if consteval`, multidimensional subscript, `size_t` literals) are shipping in GCC 13+, Clang 17+, and MSVC 19.3x.
- **`std::expected`**, **`std::print`**, **`std::mdspan`**, and the **new ranges views/algorithms** are available across the three major standard libraries (libstdc++, libc++, and MSVC STL), with `std::print`'s direct-to-FD fast path and `std::generator` arriving slightly later in libc++.
- **`import std;`** has the most uneven support and the strongest build-system dependency; treat it as the one feature to validate carefully on your toolchain before adopting (Chapter 68).
- **Extended floating-point types** (`<stdfloat>`) depend on hardware/ABI support and are the most platform-conditional addition (Chapter 67).

The practical guidance: compile with `-std=c++23` (or `/std:c++23preview` / `/std:c++latest` on MSVC), and gate the two riskiest features — `import std;` and `<stdfloat>` — behind feature-test macros (`__cpp_lib_modules`, `__STDCPP_FLOAT32_T__`, etc.).

---

## 54.7 Professional Insights

**Treat C++23 as the release that lets you delete code, not add it.** The highest-leverage way to adopt C++23 is to grep your codebase for the workarounds it obsoletes — hand-rolled generators, the CRTP base classes, the `cout << format` idiom, the four-overload accessor pattern, the sorted-`vector`-as-map — and replace them with the standard facility. Each replacement removes a maintenance liability and usually improves performance.

**Adopt `std::expected` first; it changes how you design APIs.** Of everything in this release, `expected` has the deepest ripple effect, because it lets you make "this operation can fail" a visible part of a function's type rather than a runtime surprise. Codebases that ban exceptions gain a principled error channel for the first time; codebases that use exceptions gain a cheaper one for expected failures. Plan for it to propagate through your interfaces.

**Pin the standard and audit feature-test macros, because "C++23" is not monolithic.** Different features matured on different timelines across the three toolchains. Do not assume that because deducing `this` compiles, `import std;` and `<stdfloat>` will too. Use `__has_include`, the `__cpp_*` feature-test macros, and CI matrix builds across your target compilers to verify exactly which subset of C++23 you can rely on — this is the single most common way C++23 adoption goes wrong in practice.

**Keep the C++26 line bright.** Several of the most-hyped "modern C++" features people associate with this era — reflection, senders/receivers, contracts, pattern matching — are *not* in C++23. Knowing precisely where the C++23 boundary lies prevents you from writing code that fails to compile under the `-std=c++23` flag your build system actually passes.
