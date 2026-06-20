# Chapter 38: Modules — The Death of Headers

> *Modules are the fourth pillar and the most disruptive to the build pipeline. For forty years C++ has shipped code as headers — raw text spliced into every translation unit by the preprocessor. Modules replace that text-substitution model with compiled, importable components that parse once, leak nothing, and let the compiler skip the redundant work that dominates large-project build times. This chapter covers what is wrong with headers, the full module syntax (interface units, partitions, the global module fragment), and the build-system and ABI consequences that make modules an operational change, not just a syntactic one.*

The shift from `#include` to `import` is the difference between *pasting* a dependency's source into your file and *linking against* a precompiled representation of it. That single change attacks the three chronic header pathologies — quadratic re-parsing, macro leakage, and ODR fragility — at their root. But it also inverts the build: the compiler must now build dependencies in topological order before it can compile a consumer, which is why modules are as much a toolchain story as a language story.

---

## Table of Contents

- [38.1 The Death of Headers: Why the Model Had to Change](#381-the-death-of-headers-why-the-model-had-to-change)
- [38.2 Basic Module Syntax: Interface Units](#382-basic-module-syntax-interface-units)
- [38.3 Importing a Module](#383-importing-a-module)
- [38.4 Module Implementation Units](#384-module-implementation-units)
- [38.5 Module Partitions](#385-module-partitions)
- [38.6 The Global Module Fragment](#386-the-global-module-fragment)
- [38.7 Header Units and the Migration Bridge](#387-header-units-and-the-migration-bridge)
- [38.8 Build-System Implications and the BMI Pipeline](#388-build-system-implications-and-the-bmi-pipeline)
- [38.9 ABI, the std Module, and Tooling Maturity](#389-abi-the-std-module-and-tooling-maturity)
- [38.10 Professional Insights](#3810-professional-insights)

---

## 38.1 The Death of Headers: Why the Model Had to Change

Headers (`#include`) are **text substitution**. The preprocessor literally pastes the included file's text into the including file before the compiler ever runs. This is slow, fragile, and leaks macros and symbols. Modules (`import`) are **compiled components** — parsed once into a binary representation the compiler reads directly.

The three chronic header pathologies:

- **Slow compilation.** A 10,000-line header included in 100 translation units is parsed 100 times. Compilation cost scales with (headers × consumers), not with the amount of code actually written — the root cause of multi-hour builds in large codebases.
- **Macro leaks.** A macro defined in one header affects all code textually following the `#include`, including other headers. Order-dependence and accidental token replacement (the classic `#define max ...` breaking `std::max`) are endemic.
- **ODR violations and ABI fragility.** Different compiler flags or include orders across translation units can produce subtly different definitions of the "same" entity, breaking the One Definition Rule and binary compatibility in ways that surface only at link time or runtime.

Modules fix all three structurally: an interface is parsed **once** into a Binary Module Interface (BMI); names that are not exported are **invisible** to importers (no leakage); and the imported entity is the *same* compiled entity for every importer (no ODR drift). `import` is also order-independent — unlike `#include`, importing module B then A is identical to A then B.

---

## 38.2 Basic Module Syntax: Interface Units

A **module interface unit** declares a module and lists what it exports. By convention it carries the extension `.cppm` (Clang, and CMake's preference) or `.ixx` (MSVC); GCC accepts `.cc`/`.cpp` with a flag. The `export module NAME;` line is the **module declaration** and must precede all other declarations except the global module fragment.

```cpp
// Listing 38.1: a module interface unit (math.cppm)
export module math;          // module declaration

export int add(int a, int b) // exported: visible to importers
{
    return a + b;
}

int internal_helper()        // not exported: private to the module
{
    return 42;
}
```

Only entities prefixed with `export` (or inside an `export { ... }` block) are visible to code that imports the module. `internal_helper` exists and is usable *within* the module's units, but an importer cannot name it — encapsulation enforced by the language, not by header hygiene conventions. You may also export a whole namespace or a group:

```cpp
// Listing 38.2: exporting groups and namespaces
export module geometry;

export namespace geo {       // every name in this namespace is exported
    struct Point { double x, y; };
    double norm(Point p);
}

export {                     // export block: several declarations at once
    int  area(int w, int h);
    int  perimeter(int w, int h);
}
```

---

## 38.3 Importing a Module

A consumer brings a module's exported names into scope with `import`. There is no header guard, no include order, and no macro exposure.

```cpp
// Listing 38.3: importing and using a module (main.cpp)
import math;
import <iostream>;           // a header unit (see Section 38.7)

int main() {
    std::cout << add(1, 2) << "\n";   // 3
    // internal_helper();             // error: undeclared identifier — not exported
}
```

`import math;` makes `add` available but leaves `internal_helper` invisible. Critically, **macros do not cross `import`** — if `math` internally `#define`d something, the importer never sees it. (Header units, Section 38.7, are the one exception: they *do* export macros, by design, to bridge legacy headers.)

---

## 38.4 Module Implementation Units

An interface unit can declare functions and define them elsewhere in a **module implementation unit** — the moral equivalent of splitting a header's declarations from a `.cpp`'s definitions, but without re-parsing. An implementation unit names the module *without* `export`:

```cpp
// Listing 38.4a: interface declares, does not define (math.cppm)
export module math;
export int add(int a, int b);     // declaration only
export int multiply(int a, int b);
```

```cpp
// Listing 38.4b: implementation unit defines (math_impl.cpp)
module math;                       // NOT 'export module' — this is an impl unit

int add(int a, int b)      { return a + b; }
int multiply(int a, int b) { return a * b; }
```

The implementation unit implicitly imports the interface, so it sees the declarations. Changing a definition in the implementation unit does **not** invalidate the interface's BMI, so consumers need not recompile — a key incremental-build advantage over headers, where touching an inline definition triggers a rebuild of every includer.

---

## 38.5 Module Partitions

Large modules can be split into **partitions** — sub-units of a single module, named `module:partition`. Partitions let a big module be authored across several files while still presenting one module name to the outside world.

```cpp
// Listing 38.5a: an interface partition / internal partition (math_impl.cppm)
module math:impl;                 // partition ':impl' of module 'math'

int heavy_computation() { return 100; }
```

```cpp
// Listing 38.5b: the primary interface unit assembles the module (math.cppm)
export module math;               // primary module interface unit
import :impl;                     // import the partition (same module)

export int compute() {
    return heavy_computation();   // visible: partitions share the module's scope
}
```

There are two kinds: **interface partitions** (`export module math:part;`) whose exports are re-exported by the primary unit, and **internal/implementation partitions** (`module math:part;`) that are private to the module. Partitions are imported with the leading-colon form `import :part;` and are visible only within the same module — they are an internal decomposition tool, not a public surface.

---

## 38.6 The Global Module Fragment

Real code must still consume legacy headers (`<vector>`, third-party C libraries). The **global module fragment** — introduced by a bare `module;` line at the very top of the file — is the only place `#include` is allowed in a module interface, and its contents belong to the *global module*, not to your module.

```cpp
// Listing 38.6: global module fragment for legacy headers (geometry.cppm)
module;                           // global module fragment begins
#include <vector>                 // legacy headers go here, before the declaration
#include <cmath>

export module geometry;           // module declaration ends the fragment

export double distance(double x, double y) {
    return std::sqrt(x * x + y * y);   // std::sqrt usable, but NOT re-exported
}
```

Entities pulled in via the global module fragment are **not** re-exported — an importer of `geometry` gets `distance` but not `std::vector` or `std::sqrt`. The fragment exists purely so a module can *use* legacy headers internally without leaking them. The ordering rule is strict: `module;`, then `#include`s and preprocessor directives only, then `export module NAME;`.

---

## 38.7 Header Units and the Migration Bridge

A **header unit** lets you `import` an existing header as if it were a module: `import <vector>;` or `import "myheader.h";`. The compiler synthesizes a module-like BMI from the header. Unlike a named module, a header unit **does** export the header's macros — the deliberate concession that makes it a migration bridge rather than a clean module.

```cpp
// Listing 38.7: header units vs named-module import
import <iostream>;        // header unit: faster than #include, exports macros too
import <vector>;          // header unit
import math;              // named module: no macros, fully encapsulated

// vs the legacy path:
// #include <iostream>    // text substitution, re-parsed every TU
```

Header units are the incremental on-ramp: you can convert a project's *consumption* of standard and third-party headers to `import` (gaining parse-once speed) long before you convert your own code into named modules. They are, however, less encapsulated than named modules and depend on the toolchain having the header pre-built as a unit — treat them as a transition tool, not the destination.

---

## 38.8 Build-System Implications and the BMI Pipeline

Modules **invert the build**. With headers, any translation unit can be compiled independently and in any order — the preprocessor makes each TU self-contained. With modules, the compiler must produce a module's **Binary Module Interface (BMI)** *before* it can compile anything that imports it. The build system must therefore:

1. **Scan** sources to discover `export module` / `import` declarations and build a **dependency graph**.
2. **Topologically order** compilation so each module's BMI is produced before its importers compile.
3. **Compile interface units to BMIs** (`.pcm` on Clang, `.ifc` on MSVC, `.gcm` on GCC), then compile consumers against those BMIs.

```text
Listing 38.8: the BMI build pipeline (conceptual)

  math.cppm ──scan──▶ "exports module math"
       │
       ├─ compile interface ─▶ math.pcm   (Binary Module Interface)
       │
  main.cpp  ──scan──▶ "imports math"  ──depends-on──▶ math.pcm
       │
       └─ compile (reads math.pcm) ─▶ main.o ─▶ link ─▶ executable
```

A BMI is **compiler- and flag-specific**: a `.pcm` from Clang is unreadable by GCC, and even the same compiler with different flags may invalidate it. BMIs are build artifacts, never checked in. This is why modules require a build system that understands the two-phase scan-then-compile model — **CMake 3.26+** (with `3.28+` for solid `import std;` support), Build2, XMake, and MSBuild. A bare `g++ *.cpp` no longer works for a modular project; the dependency scan is mandatory.

---

## 38.9 ABI, the std Module, and Tooling Maturity

Three operational realities temper the modules story in the C++20 timeframe:

- **`import std;` is C++23, not C++20.** C++20 standardized the module *language feature* but did **not** standardize a module for the standard library. To `import` the standard library under C++20 you use header units (`import <vector>;`) or vendor-specific std-module support. The portable `import std;` / `import std.compat;` arrived in C++23.
- **No standardized BMI format / ABI.** The committee deliberately did not specify the on-disk BMI format, leaving it to vendors. BMIs do not interoperate across compilers and are not a distribution format — you ship source (or libraries + BMIs built locally), not BMIs.
- **Tooling maturity lagged the standard.** Compiler and build-system support for modules trailed the 2020 standard by years. As of the mid-2020s, Clang, MSVC, and GCC all support named modules, and CMake 3.28+ handles them, but corner cases (especially mixing modules with legacy headers and PCH) still require care. Treat modules adoption as a project-level migration with toolchain version floors, not a free per-file upgrade.

The encapsulation and build-speed wins are real and large, but they are unlocked by an aligned toolchain. Plan adoption around concrete compiler and CMake versions, and stage it: header units first, then leaf modules, then the dependency core.

---

## 38.10 Professional Insights

**Adopt modules bottom-up, starting with header units.** Converting your *consumption* of `<iostream>`, `<vector>`, and stable third-party headers to `import` (header units) buys parse-once compile speed with almost no risk. Convert your own code to named modules leaf-first — modules with no internal dependencies — then work inward. A big-bang rewrite of a large codebase to modules is high-risk; the incremental path captures most of the speed win early.

**Pin toolchain versions before committing to modules.** Modules need a build system that does the scan-then-compile dependency ordering: CMake 3.28+ is the practical floor for serious work (3.26/3.27 are rougher), alongside a recent Clang/MSVC/GCC. Write the minimum versions into your project's requirements; a contributor on an older toolchain cannot build a modular project at all, unlike a header-only one.

**Never check in or distribute BMIs.** A `.pcm`/`.ifc`/`.gcm` is tied to one compiler, version, and flag set — it is a local build artifact, like a `.o`. Ship source. If you distribute a binary library, consumers build the BMI locally against their toolchain. Treating a BMI as portable is the modules equivalent of shipping someone else's object files and hoping the ABI matches.

**Use the global module fragment narrowly and remember it does not re-export.** Put legacy `#include`s in the `module;` fragment only when a module genuinely needs them internally, and rely on the fact that they are *not* re-exported to keep your module's surface clean. If consumers need those names, export your own thin wrappers — do not reach for header units to leak them through.

**Know the C++20/23 line: no `import std;` in C++20.** Under a strict C++20 build, import the standard library via header units, not `import std;` — that and `import std.compat;` are C++23. Code that assumes `import std;` will fail on a conforming C++20 toolchain, the most common surprise when porting module examples written against later standards.
