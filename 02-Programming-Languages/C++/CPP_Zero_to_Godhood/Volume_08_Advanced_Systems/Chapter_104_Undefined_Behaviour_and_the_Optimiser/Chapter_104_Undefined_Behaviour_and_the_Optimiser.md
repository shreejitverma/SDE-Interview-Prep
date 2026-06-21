# Chapter 104: Undefined Behaviour and the Optimiser

Undefined behaviour is the bargain at the heart of C++'s performance: the standard declares certain operations to have *no defined meaning*, and in exchange the optimizer is permitted to assume they never happen — which is exactly what lets it generate fast code. But the same bargain means a single UB anywhere licenses the optimizer to do *anything*, including deleting your safety checks and miscompiling code that "looks fine." This chapter explains what UB is, how the optimizer exploits it, the common sources (signed overflow, out-of-bounds, strict aliasing, the ODR, data races), and the distinction from merely *unspecified* or *implementation-defined* behaviour — because mistaking these three is itself a source of bugs.

## Chapter Roadmap

- 104.1 The Three Kinds of "It Depends"
- 104.2 What Undefined Behaviour Actually Means
- 104.3 How the Optimiser Exploits UB
- 104.4 The Common Sources of UB
- 104.5 Strict Aliasing
- 104.6 The One Definition Rule
- 104.7 Avoiding and Detecting UB

---

## 104.1 The Three Kinds of "It Depends"

The standard defines three distinct categories of non-portable behaviour, and conflating them is a common error:

| Category | Meaning | Example |
|---|---|---|
| **Implementation-defined** | Compiler chooses, must document, consistent | `sizeof(int)`, right-shift of negatives |
| **Unspecified** | Compiler chooses, need not document, may vary | Order of evaluation of function arguments |
| **Undefined (UB)** | *No* requirements whatsoever; anything may happen | Signed overflow, out-of-bounds access, null deref |

> **Why this matters.** Implementation-defined behaviour is *safe but non-portable* — `sizeof(int)` is 4 on your platform and your program works, it just may differ elsewhere; you can rely on it within a platform. Unspecified behaviour is *safe but non-deterministic* — the order of argument evaluation may vary, so don't depend on it, but it won't corrupt your program. **Undefined behaviour is categorically different**: it is not "some value you can't predict," it is "the standard imposes *no constraints at all*," and the optimizer is entitled to assume it never occurs. Treating UB like unspecified behaviour ("it'll just give some garbage value") is the fundamental misunderstanding this chapter exists to correct — UB can delete code, corrupt unrelated data, and "time-travel."

---

## 104.2 What Undefined Behaviour Actually Means

When a program executes UB, the C++ standard places *no requirements* on its behaviour — not just at the point of the UB, but for the *entire execution*. The optimizer's foundational assumption is that **UB never happens**, so it freely transforms code based on that assumption.

```cpp
// Min standard: C++11. Each line is UB — the program has NO defined behaviour if reached.
int overflow(int x)  { return x + 1 > x; }   // signed overflow is UB -> compiler may return `true` ALWAYS
int deref(int* p)    { int v = *p; if (p) return v; return 0; }  // deref then null-check: p assumed non-null
int oob(int* a)      { return a[1000000]; }  // out-of-bounds: UB, no bounds exist
```
*Listing 104.1 — Each function contains UB the optimizer exploits to "simplify" the code in surprising ways.*

> **Why this matters.** `x + 1 > x` looks always-true mathematically, but for `INT_MAX` it overflows — which is UB — so the compiler is allowed to assume overflow *never happens* and compile the function to `return true;` unconditionally, breaking the overflow check the programmer intended. In `deref`, the optimizer sees `*p` (which would be UB if `p` were null), concludes `p` *cannot* be null (or the program already had UB), and therefore deletes the `if (p)` check as redundant. This is not malice; it is the optimizer correctly reasoning from the contract: *you promised no UB, so I may assume this branch is dead.* The lesson is that UB is not a runtime accident with a local effect — it is a *compile-time license* the optimizer cashes in.

---

## 104.3 How the Optimiser Exploits UB

The optimizer uses the no-UB assumption in three powerful and surprising ways:

- **Assuming conditions:** if a code path would be UB unless condition C holds, the optimizer assumes C and optimizes accordingly (the null-check deletion above).
- **Deleting "impossible" code:** code reachable only via a UB path is dead and removed.
- **"Time-travel":** because UB poisons the *whole* execution, the optimizer may move the effects of UB *earlier* — a side effect before the UB can be reordered or eliminated as if the UB had already happened. Observable behaviour *before* the UB is not guaranteed to be preserved.

> **Why this matters / cost model.** This is the deep, counterintuitive truth: UB is not "undefined *from this point*," it is undefined *for the entire program execution that reaches it*. A `printf` that executes just before a guaranteed-UB statement may not run, because the optimizer, having proven the path leads to UB, may treat the whole path as unreachable. This is why "it worked in debug but broke in release" so often traces to UB: `-O0` doesn't exploit the assumptions, `-O2` does. And it is the same as-if-rule mechanism from Chapter 89 — the optimizer preserves observable behaviour *only for programs with defined behaviour*; once you breach the contract, it owes you nothing. The performance you get from no-overflow, no-aliasing, no-null assumptions is real, but it is conditional on you upholding your side.

---

## 104.4 The Common Sources of UB

The UB you will actually encounter:

- **Signed integer overflow** — `INT_MAX + 1` (unsigned overflow is *defined* wraparound; signed is UB — the asymmetry matters).
- **Out-of-bounds access** — indexing past an array/container, dangling iterators.
- **Null/invalid pointer dereference**, and **use-after-free** / **use-after-scope** (dangling references — Chapter 97).
- **Uninitialized reads** — reading an uninitialized variable.
- **Data races** — concurrent unsynchronised access with a write (Chapter 76).
- **Invalid downcasts**, violating `static_cast` preconditions (Chapter 74's CRTP hazard).
- **Strict aliasing violations** (§104.5) and **misaligned access** (Chapter 79).
- **Infinite loops with no side effects** — the compiler may assume forward progress and remove or transform them.

> **Why this matters.** This list is the catalogue of bugs that "work until they don't" — they pass tests, run for months, then miscompile after a compiler upgrade or an `-O3` flip, because the optimizer's exploitation of the UB changed. Note the *defined* counterexamples that make the boundaries clear: **unsigned** overflow wraps (defined), so hash functions and ring-buffer indices use unsigned deliberately; `memcpy` between types is the *defined* way to reinterpret bytes (vs the UB of a type-punning union or `reinterpret_cast` read). Knowing which operations are UB versus defined is not pedantry — it is the difference between code the optimizer can safely transform and code it will silently break.

---

## 104.5 Strict Aliasing

The **strict aliasing rule** says the compiler may assume that pointers of *different* types do not point to the same memory (with specific exceptions: `char*`/`std::byte*` may alias anything, and similar types alias). This lets the optimizer keep a value in a register across a write through an unrelated-typed pointer, a significant optimization.

```cpp
// Min standard: C++11. Strict-aliasing violation -> UB.
float reinterpret_bad(int x) {
    return *reinterpret_cast<float*>(&x);   // UB: reading an int's bytes through a float* aliases differently
}
float reinterpret_ok(int x) {
    float f;
    std::memcpy(&f, &x, sizeof f);          // DEFINED: memcpy is the legal type-pun; compiles to no-op at -O2
    return f;
}
// C++20: std::bit_cast<float>(x) is the clean, constexpr-friendly form.
```
*Listing 104.2 — Type-punning through a mismatched pointer is UB; `memcpy`/`bit_cast` is the defined equivalent (and just as fast).*

> **Why this matters / cost model.** Strict aliasing is a performance feature: because the compiler may assume an `int*` and a `float*` don't overlap, it need not reload values after every cross-type write, enabling register allocation and vectorisation. The cost is that the classic type-pun (`*reinterpret_cast<float*>(&x)`) is UB that may produce wrong results at `-O2` while working at `-O0`. The fix is *free*: `std::memcpy` (or C++20 `std::bit_cast`) expresses the same reinterpretation legally, and the optimizer recognises it and emits *zero* extra instructions — you get the bit-reinterpretation you wanted with defined behaviour. This is the standard pattern for reading wire formats and doing bit-level manipulation (and connects to `start_lifetime_as`, Chapter 97). Compile with `-fstrict-aliasing` warnings (`-Wstrict-aliasing`) to catch violations.

---

## 104.6 The One Definition Rule

The **One Definition Rule (ODR)** requires that every entity (function, variable, type) have exactly *one* definition across the whole program (with a precise exception for `inline` entities and templates, which may be defined identically in multiple TUs). Violating it — two *different* definitions of the same symbol — is UB, and typically *not diagnosed*: the linker silently picks one.

```cpp
// Min standard: C++11. ODR violation across two TUs -> UB, usually undiagnosed.
// a.cpp:  struct S { int x; };      void f(S s);
// b.cpp:  struct S { int x, y; };   // SAME NAME, DIFFERENT layout -> ODR violation
// The linker merges them; calls to f() read a mismatched layout -> silent corruption.
```
*Listing 104.3 — An ODR violation from two incompatible definitions of `S`. The linker does not catch it.*

> **Why this matters.** The ODR is the rule behind the linker errors of Chapter 102 (`multiple definition` is the *diagnosed* ODR violation) and behind a class of *undiagnosed* silent-corruption bugs (two different definitions of a type/inline function, often from inconsistent `#define`s, build flags, or header versions across TUs). Because the standard makes it UB *without requiring a diagnostic*, the linker is free to merge incompatible definitions and the program reads garbage. This is the deep reason the ABI-mismatch hazards of Chapter 102 are so dangerous, and why build hygiene — identical headers, consistent flags, no incompatible macros affecting type layout — is a correctness requirement, not a style preference. `inline` functions and templates *must* be defined identically in every TU that uses them, or you have an ODR violation the tools won't catch.

---

## 104.7 Avoiding and Detecting UB

| Source | Detection / Avoidance |
|---|---|
| Signed overflow | UBSan; use unsigned where wrap is intended; `-ftrapv` |
| Out-of-bounds, use-after-free | ASan; `.at()`; `std::span`; bounds checks in debug |
| Uninitialized reads | MSan; initialize at declaration; `-Wmaybe-uninitialized` |
| Data races | TSan (Chapter 105) |
| Strict aliasing | `memcpy`/`bit_cast`; `-Wstrict-aliasing` |
| ODR | Consistent headers/flags; LTO sometimes catches mismatches |
| General | UBSan in CI; `-Wall -Wextra`; treat warnings as errors |

> **The discipline.** Undefined behaviour is the source of C++'s speed *and* its most insidious bugs, and the two are inseparable: the optimizer is fast *because* it assumes no UB, so the price of that speed is upholding the contract everywhere. The practical defence is twofold: *understand* the boundary (know that signed overflow, OOB, strict-aliasing violations, data races, and ODR breaches are UB, and use the defined alternatives — unsigned wrap, `memcpy`/`bit_cast`, atomics, consistent definitions); and *detect* what slips through with the sanitizers and warnings of the next chapter, run in CI on every build. UB that "works today" is a latent miscompilation waiting for an optimizer change — the only safe amount is zero, and the only way to be sure is to test for it. The next chapter is the tooling that makes that testing systematic.
