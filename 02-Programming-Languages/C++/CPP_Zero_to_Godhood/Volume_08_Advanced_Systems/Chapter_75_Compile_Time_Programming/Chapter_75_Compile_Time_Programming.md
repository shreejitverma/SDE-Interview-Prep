# Chapter 75: Compile-Time Programming

Compile-time programming moves computation from the running process into the translation pipeline: tables are precomputed, invariants are proven, and code paths are eliminated before a single instruction executes. The problem it solves is twofold — *zero runtime cost* for work whose inputs are known at build time, and *correctness by construction* via checks the program cannot even link without satisfying. This chapter develops `constexpr`/`consteval`, the type-traits machinery they build on, and the cost model that decides when shifting work to the compiler pays and when it merely inflates build time.

## Chapter Roadmap

- 75.1 Why Compute at Compile Time
- 75.2 The `constexpr` Evolution
- 75.3 `consteval` and `constinit`
- 75.4 Type Traits: Queries and Transformations
- 75.5 `std::integral_constant` and Value-as-Type
- 75.6 `if constexpr` and Compile-Time Branching
- 75.7 Static Reflection (C++26 Preview)
- 75.8 The Cost Model and Hazards

---

## 75.1 Why Compute at Compile Time

Three distinct payoffs motivate pushing work into translation:

1. **Zero runtime cost.** A lookup table built by a `constexpr` function is materialised into `.rodata` at compile time; the program just reads it. A CRC table, a sine LUT, or a perfect-hash for a keyword set costs nothing at runtime.
2. **Correctness by construction.** `static_assert` and `consteval` turn class-of-bug into class-of-build-error: a malformed format string, an out-of-range enum, or a unit mismatch fails to compile rather than failing in production.
3. **Specialisation.** Knowing a value at compile time lets the optimiser propagate constants, unroll loops, and eliminate dead branches — the compile-time result *feeds* code generation.

> **Why this matters / cost model.** The runtime saving is real and unconditional, but it is purchased with **compiler time and memory**: a `constexpr` evaluation runs in the front-end's interpreter, which is orders of magnitude slower than native execution and bounded by `-fconstexpr-steps` / `-fconstexpr-ops-limit`. Computing a million-entry table at compile time can add seconds to every build. The rule of thumb: precompute at compile time when the input is genuinely fixed and the table is small-to-moderate; otherwise generate it once at startup.

---

## 75.2 The `constexpr` Evolution

`constexpr` marks a function or variable as *usable* in a constant expression. The set of constructs permitted inside one has widened across every standard:

| Standard | Permitted inside `constexpr` |
|---|---|
| C++11 | Single `return`; no loops, no local mutation — effectively pure recursion |
| C++14 | Loops, local variables, multiple statements, mutation of locals |
| C++17 | `if constexpr`; lambdas are implicitly `constexpr` when eligible |
| C++20 | `try`/`catch` (no throw at compile time), `virtual` calls, *transient* dynamic allocation (`std::vector`, `std::string`), `std::is_constant_evaluated()` |
| C++23 | `constexpr` for much more of the standard library; relaxed `goto`/labels; non-literal variables in non-taken branches |

```cpp
// Min standard: C++14 (loop + local in constexpr). Portable.
constexpr unsigned long long factorial(unsigned n) {
    unsigned long long acc = 1;
    for (unsigned i = 2; i <= n; ++i) acc *= i;
    return acc;
}

constexpr auto F10 = factorial(10);   // computed during translation
static_assert(F10 == 3628800);
```
*Listing 75.1 — A C++14-style imperative `constexpr` function.*

A crucial subtlety: a `constexpr` function is **not required** to run at compile time. It runs at compile time only when used in a constant-expression context (e.g. initialising a `constexpr` variable, an array bound, or a template argument); otherwise it is an ordinary function. `std::is_constant_evaluated()` (C++20) lets a function take a different path in each case:

```cpp
// Min standard: C++20. Portable.
#include <type_traits>
constexpr double power(double b, int e) {
    if (std::is_constant_evaluated()) {
        double r = 1.0;                 // exact, branch-friendly path for the compiler
        for (int i = 0; i < e; ++i) r *= b;
        return r;
    } else {
        return __builtin_pow(b, e);     // fast runtime intrinsic (non-portable builtin)
    }
}
```
*Listing 75.2 — Dual-mode evaluation. The builtin is GCC/Clang-specific; flagged as non-portable.*

> **Why this matters.** `is_constant_evaluated()` resolves a genuine tension: the algorithm that is *fast at runtime* (an intrinsic, a SIMD path, a table the compiler cannot evaluate) is often not the one that is *legal or cheap at compile time*. Beware the classic trap: `if constexpr (std::is_constant_evaluated())` is always-true and is a bug — the function name returns a runtime-evaluated bool, so it must be a plain `if`. C++23's `if consteval` makes the correct form unambiguous.

---

## 75.3 `consteval` and `constinit`

**`consteval`** (C++20) declares an *immediate function*: every call must produce a constant — if it cannot be evaluated at compile time, the program is ill-formed.

```cpp
// Min standard: C++20. Portable.
consteval int square(int n) { return n * n; }

constexpr int a = square(5);   // OK
int runtime = 10;
// int b = square(runtime);    // ERROR: runtime is not a constant expression
```
*Listing 75.3 — `consteval` forces compile-time evaluation.*

Use `consteval` when a function *must* be erased before runtime — building a validated configuration object, parsing a format string, or constructing a compile-time-checked literal. It is the difference between "may run at compile time" (`constexpr`) and "must" (`consteval`).

**`constinit`** (C++20) asserts that a variable with static storage is initialised by a constant expression, eliminating the **static initialisation order fiasco** and any runtime initialisation guard for that object. It does not make the variable `const` — only its *initialisation* is compile-time.

> **Why this matters / cost model.** A non-`constinit` function-local `static` carries a hidden thread-safe guard (an atomic check on every access in some ABIs) and a namespace-scope non-constant `static` introduces dynamic init ordered unpredictably across TUs. `constinit` removes both: the object lands fully formed in `.data`/`.rodata` with no runtime initialisation code and no ordering hazard.

---

## 75.4 Type Traits: Queries and Transformations

`<type_traits>` is the foundation library for metaprogramming: a catalogue of compile-time predicates and type transformations. They are the primitive operations from which the techniques of Chapter 74 are built.

**Queries** (yield a `bool` value via `::value` / `_v`):

```cpp
// Min standard: C++17 (the _v suffixes). Portable.
#include <type_traits>
static_assert(std::is_integral_v<int>);
static_assert(std::is_same_v<int, int>);
static_assert(std::is_base_of_v<std::ios_base, std::ostream>);
static_assert(std::is_trivially_copyable_v<int>);     // gates memcpy-style optimisation
```

**Transformations** (yield a type via `::type` / `_t`):

| Trait | Effect |
|---|---|
| `remove_const_t<T>` | strips top-level `const` |
| `remove_reference_t<T>` | `T& → T`, `T&& → T` |
| `decay_t<T>` | arrays→pointers, functions→pointers, removes cv-ref (the by-value parameter transform) |
| `conditional_t<B,T,F>` | compile-time `if`: selects `T` or `F` |
| `common_type_t<Ts...>` | the type an expression of those types would yield |

```cpp
// Min standard: C++14 (alias forms). Portable.
using T1 = std::remove_const_t<const int>;          // int
using T2 = std::decay_t<int[10]>;                    // int*
using T3 = std::conditional_t<sizeof(void*) == 8, long, int>;  // 64-bit: long
```
*Listing 75.4 — Type transformations as compile-time functions on types.*

> **Why this matters.** Traits like `is_trivially_copyable` are not academic: the standard library and your own generic code branch on them to choose `memcpy` over element-wise copy, to elide destructors, and to enable relocation optimisations. A trait query is the compile-time evidence that licenses an aggressive code path. Getting a trait wrong (e.g. assuming trivial copyability of a type that owns a resource) is a memory-safety bug, not a slowdown.

---

## 75.5 `std::integral_constant` and Value-as-Type

`std::integral_constant<T, v>` wraps a compile-time value as a *type*, giving it a `::value` member and an implicit conversion. `std::true_type` and `std::false_type` are its most-used specialisations and are the return types of the entire trait library.

```cpp
// Min standard: C++11. Portable.
#include <type_traits>
using Two  = std::integral_constant<int, 2>;
using Four = std::integral_constant<int, 4>;
static_assert(Two::value + Two::value == Four::value);
```
*Listing 75.5 — A value carried in the type system.*

The value-as-type encoding is what makes **tag dispatch** work: by mapping a trait to `true_type`/`false_type` you can select an overload at compile time without `if constexpr`, which is essential on pre-C++17 toolchains and still clearer for some dispatch tables.

---

## 75.6 `if constexpr` and Compile-Time Branching

`if constexpr` (C++17) discards the not-taken branch *during instantiation* — the discarded branch need not even compile for the current type. This collapses what previously required two SFINAE overloads or tag dispatch into a single readable function.

```cpp
// Min standard: C++17. Portable.
#include <type_traits>
#include <string>

template <typename T>
std::string describe(const T& x) {
    if constexpr (std::is_integral_v<T>) {
        return "integer:" + std::to_string(x);
    } else if constexpr (std::is_floating_point_v<T>) {
        return "float:" + std::to_string(x);
    } else {
        return "other";
    }
}
```
*Listing 75.6 — One function, statically-selected bodies.*

> **Why this matters.** The not-taken branch is removed before type-checking the body, so `std::to_string(x)` is never instantiated for a `T` that lacks it. With a plain `if`, *both* branches must compile for every `T`, which fails. `if constexpr` is the single most important ergonomic improvement to compile-time branching and should be the default over SFINAE for in-body dispatch.

---

## 75.7 Static Reflection (C++26 Preview)

Today, querying a type's members or an enum's enumerators requires macros, external code generation, or libraries such as `magic_enum` (which exploit compiler-specific `__PRETTY_FUNCTION__` parsing). C++26 introduces **static reflection**: a standard, first-class facility to obtain a `std::meta::info` handle for an entity and splice it back into code.

```cpp
// Min standard: C++26 (proposed syntax; not yet stable across compilers).
// constexpr auto r = ^^MyClass;                  // reflect a type
// template for (constexpr auto m : std::meta::nonstatic_data_members_of(r))
//     std::cout << std::meta::identifier_of(m) << '\n';
```
*Listing 75.7 — Proposed reflection. Syntax is provisional and flagged as non-portable/unstable.*

> **Why this matters.** Reflection eliminates the largest remaining source of hand-written boilerplate in C++: serialisation, ORM mapping, enum-to-string, and dependency injection are today either macro-driven or generated by external tools. Standard reflection lets these be ordinary `constexpr` code, type-checked and debuggable. Until it ships and stabilises, treat reflection-dependent designs as experimental and isolate them behind a thin interface.

---

## 75.8 The Cost Model and Hazards

| Concern | Reality | Guidance |
|---|---|---|
| Compile time | `constexpr` runs in a slow front-end interpreter, bounded by step limits | Precompute only small/medium tables; profile with `-ftime-trace` |
| Memory in the compiler | Large compile-time data structures live in the front-end's heap | Watch for OOM on big `constexpr` containers |
| Transient allocation | C++20 allows `constexpr` `new`, but it must be freed *before* evaluation ends | Cannot leak heap state into runtime; persistence requires `constexpr`-friendly fixed storage |
| `is_constant_evaluated` misuse | Using it in `if constexpr` is always-true (a bug) | Use plain `if`, or `if consteval` (C++23) |
| Floating-point determinism | Compile-time FP follows strict IEEE rules; runtime may use FMA/different rounding | A value computed at compile time can differ in the last bit from the runtime path |

> **Why this matters.** The headline risk of compile-time programming is silently trading runtime speed for build speed. A team that precomputes everything can find its incremental builds taking minutes. Measure the compile-time cost the same way you measure runtime cost, and reserve `consteval`/large `constexpr` evaluation for cases where the correctness guarantee or the zero-runtime-cost table genuinely justifies it. The disciplines that follow — the memory model, lock-free design, allocators — all assume this same habit of *measuring the cost you are actually paying*.
