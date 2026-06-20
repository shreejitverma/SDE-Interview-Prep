# Chapter 68: Standard Library Modules — `import std;`

> C++20 gave the language *modules* — a real compilation-unit boundary to replace textual `#include`. But it did not modularize the standard library itself, so the headline benefit (importing the whole standard library as a precompiled unit instead of re-parsing tens of thousands of lines per translation unit) was unavailable. C++23 closes that gap with two named modules: **`import std;`** brings in the entire C++ standard library, and **`import std.compat;`** adds the C library names in the global namespace as well. This is the single line that lets a C++23 program replace its forest of `#include`s with one import — and, where the toolchain cooperates, compile dramatically faster.

## Table of Contents

1. [The Cost of Textual Inclusion](#681-the-cost-of-textual-inclusion)
2. [`import std;` and `import std.compat;`](#682-import-std-and-import-stdcompat)
3. [What the Modules Export](#683-what-the-modules-export)
4. [Mixing Imports with Includes](#684-mixing-imports-with-includes)
5. [Build-System and Toolchain Reality](#685-build-system-and-toolchain-reality)
6. [Performance: Why This Is the Compile-Time Feature](#686-performance-why-this-is-the-compile-time-feature)
7. [Professional Insights](#687-professional-insights)

---

## 68.1 The Cost of Textual Inclusion

`#include` is a preprocessor copy-paste: every `#include <vector>` literally pastes the entire contents of `<vector>` — and everything *it* includes, transitively — into the translation unit before the compiler proper ever runs. A single `#include <algorithm>` can pull in well over 100,000 lines of declarations. Worse, this happens **per translation unit**: in a project with 500 `.cpp` files each including the common standard headers, the compiler parses those same hundreds of thousands of lines 500 times over. Include guards prevent re-inclusion *within* one translation unit, but do nothing across translation units.

This is the dominant cost in many C++ build times, and it is pure waste — the same standard headers, parsed and semantically analyzed identically, again and again. C++20 modules attacked the problem in principle: a module is compiled *once* into a binary module interface (BMI) and then *imported* — a fast lookup of already-analyzed declarations — rather than re-parsed. But until C++23 there was no standard module *for the standard library*, so the largest and most universally-included body of code in every program stayed textual.

---

## 68.2 `import std;` and `import std.compat;`

C++23 defines two standard-library modules:

- **`import std;`** exports the entire C++ standard library — everything in namespace `std` from `<vector>`, `<string>`, `<algorithm>`, `<print>`, `<ranges>`, and all the rest — as one named module. It also exports the C library facilities under their `std::` names (`std::printf`, `std::size_t`).
- **`import std.compat;`** exports everything `import std;` does, **plus** the C library names in the *global* namespace (`::printf`, `::size_t`, `::fopen`). It exists for code that calls C functions unqualified, as decades of C and C-influenced C++ do.

**Listing 68.1: An entire program's includes replaced by one import.**

```cpp
import std;            // the whole standard library, as one module

int main() {
    std::vector<int> v{5, 3, 8, 1, 9, 2};
    std::ranges::sort(v);

    for (auto [i, x] : std::views::enumerate(v))
        std::println("v[{}] = {}", i, x);   // <print>, <ranges>, <vector> — all imported
}
```

That one `import std;` replaces what would otherwise be `#include <vector>`, `<algorithm>`, `<ranges>`, `<print>`, and their transitive baggage. The rule of thumb: **use `import std;` for new C++ code; use `import std.compat;` when porting a codebase that relies on unqualified C names.**

---

## 68.3 What the Modules Export

The named modules export the standard library's **public interface** with the same names, namespaces, and semantics you already know — `import std;` is not a different library, just a different *delivery mechanism* for the same `std`. A few details matter in practice:

- **Macros are not exported.** Modules export declarations, not preprocessor macros. So `assert` (from `<cassert>`), `errno`, `offsetof`, feature-test macros, and `static_assert`-adjacent macro machinery are **not** brought in by `import std;`. If you need a standard macro, you still `#include` the corresponding header for it. This is the most common surprise when migrating.
- **It is all-or-nothing at module granularity.** You import the whole standard library, not a single header's worth. In exchange for losing the (largely illusory) fine-grained control of per-header includes, you get a single fast import — and the linker/optimizer still only emits what you use, so binary size is unaffected.
- **Everything keeps its `std::` qualification.** `import std;` does not dump names into the global namespace; you still write `std::vector`, `std::println`. Only `import std.compat;` adds the *C* names globally, matching `<cxxx>`-plus-`<xxx.h>` legacy behavior.

---

## 68.4 Mixing Imports with Includes

Real migrations are incremental, so the standard is careful about combining the two worlds in one program:

- You **may** mix `import std;` with `#include` of standard headers across a program, and even reach the same entities both ways — the `std::vector` you get from `#include <vector>` and the one from `import std;` are the *same* type, so they interoperate seamlessly.
- The one firm rule: **`import` declarations must appear before any other declarations** in a translation unit (after the optional `module;` preamble), so imports go at the top, above your own code — though they may follow `#include`s. A common, safe migration pattern is to `import std;` for the standard library while still `#include`-ing third-party and your own project headers.

**Listing 68.2: A pragmatic mixed translation unit.**

```cpp
import std;                 // standard library via module

#include "my_project/widget.hpp"   // your own headers stay as includes
#include <cassert>                 // included for the assert MACRO (not exported)

int main() {
    Widget w;
    assert(w.valid());      // macro from <cassert>
    std::println("{}", w.name());
}
```

---

## 68.5 Build-System and Toolchain Reality

This is the feature where "the standard says X" and "your toolchain does X today" diverge the most, so engineering expectations must be set carefully:

- **The BMI must be built first.** `import std;` requires the `std` module's binary interface to be compiled for your exact compiler, version, and flags before any TU that imports it. Module interfaces are *not* portable across compilers or even incompatible flag sets, so the build system must produce the BMI as a dependency, in the right order, with matching flags.
- **Build-system support is the gating factor, not the compiler.** As of the mid-2020s the major compilers (recent GCC, Clang, MSVC) ship the `std` module, but driving it cleanly requires build-system cooperation. CMake added explicit support for `import std;` (behind an opt-in for the standard-library module), and Build2 and MSBuild handle it; ad-hoc Makefiles generally need manual BMI rules. Practically, whether you can use `import std;` today depends mostly on your build system and toolchain versions.
- **Order-of-declarations and flag consistency are enforced.** Because the BMI bakes in compilation flags, mixing TUs built with mismatched flags against one `std` BMI is an error, not a silent mismatch — which is safer, but means the build must be coherent.

> **Version-trap flag:** `import std;` and `import std.compat;` are **C++23** and require a toolchain *and build system* that support standard-library modules. C++20 introduced modules and `import` but **not** the `std` named module — that is the C++23 addition. Macros (`assert`, `errno`, feature-test macros) are not exported by `import std;`; `#include` the relevant header when you need them.

---

## 68.6 Performance: Why This Is the Compile-Time Feature

The motivation is almost entirely **build throughput**:

- **Parse once, import many.** The standard library headers are compiled into a BMI a single time; every translation unit then *imports* pre-analyzed declarations instead of re-parsing six-figure line counts. On heavily-templated codebases that include large standard headers everywhere, full-build and incremental-build times can drop substantially — frequently-cited figures are large, though the exact win is workload- and toolchain-dependent.
- **Better incremental builds.** Touching a source file no longer forces the compiler to re-chew the standard library for that TU; it reuses the BMI. The marginal cost of an edit-compile cycle shrinks.
- **Order-independent, macro-clean semantics.** Because modules export declarations rather than text, `import std;` is immune to the classic `#include`-order bugs and to macros from one header silently mangling another. The result compiles more predictably, not just faster.
- **No runtime cost or benefit.** This is purely a translation-time mechanism. The emitted code, inlining, and binary are identical to the `#include` equivalent — `import std;` changes how fast you build, not how fast you run.

---

## 68.7 Professional Insights

**Adopt `import std;` for new code once your build system supports it — the win is real and the cost is a one-line change.** Replacing the standard-header forest with a single import removes the single largest source of redundant parsing in most C++ builds, and because the imported `std` is the *same* library you already use, the migration is mechanical rather than semantic. The gating question is never "is my code ready" but "does my build system drive the `std` BMI correctly" — so the first move is to confirm your CMake/Build2/MSBuild version and compiler support it, then flip new translation units over.

**Default to `import std;`, and reserve `import std.compat;` for codebases steeped in unqualified C names.** The plain `std` module keeps everything properly `std::`-qualified, which is what you want for clean modern code. `std.compat` exists to ease porting code that calls `printf`, `memcpy`, or `size_t` without the `std::` prefix — useful as a transitional crutch, but if you are writing fresh code, prefer `import std;` and the qualified names so you are not silently depending on global C symbols.

**Remember that macros do not come through the module — this is the migration gotcha that bites everyone.** `import std;` brings declarations, not the preprocessor, so `assert`, `errno`, `offsetof`, and the feature-test macros are simply absent until you `#include` their header. When a previously-working `assert` suddenly fails to compile after switching to `import std;`, the fix is a one-line `#include <cassert>`, not a redesign. Plan for a hybrid translation unit — `import std;` for the library, `#include` for the handful of macros and for your own and third-party headers.

**Treat this as a build-engineering feature, and verify the toolchain story before committing a team to it.** Unlike most language features, `import std;`'s usability is dominated by build-system and toolchain maturity, not by the standard text: BMIs are non-portable, flag-sensitive, and must be built in dependency order. Before mandating it across a project, confirm that every supported compiler/build-system combination produces and consumes the `std` BMI cleanly in CI, and keep an `#include`-based fallback path until that is proven — the payoff is faster builds, but only on infrastructure that handles modules correctly.
