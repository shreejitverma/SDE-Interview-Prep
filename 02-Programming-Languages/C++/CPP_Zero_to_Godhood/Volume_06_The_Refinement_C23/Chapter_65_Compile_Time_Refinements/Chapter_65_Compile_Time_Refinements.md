# Chapter 65: Compile-Time Refinements — `if consteval` and Expanded `constexpr`

> Each C++ release since C++11 has pushed more computation into the compiler, and C++23 continues the march on two fronts. First, **`if consteval`** gives a clean, safe way for a function to behave differently when it is being evaluated at compile time versus run time — fixing the subtle footguns of the old `std::is_constant_evaluated()` idiom. Second, C++23 **relaxes the rules of `constexpr`** substantially: more of the language is legal inside `constexpr` functions (non-literal variables, `goto`, labels, `static`/`thread_local` locals), and far more of the standard library is now usable at compile time, including `constexpr` `std::unique_ptr`, much of `<cmath>`, and `std::bitset`. Together they widen the range of code you can run before `main` even starts.

## Table of Contents

1. [`is_constant_evaluated()` and Its Trap](#651-is_constant_evaluated-and-its-trap)
2. [`if consteval`: The Fix](#652-if-consteval-the-fix)
3. [Relaxed `constexpr` Function Bodies](#653-relaxed-constexpr-function-bodies)
4. [`constexpr` `std::unique_ptr` and Compile-Time Allocation](#654-constexpr-stdunique_ptr-and-compile-time-allocation)
5. [`constexpr` Library Growth: `<cmath>`, `bitset`, `type_info`](#655-constexpr-library-growth-cmath-bitset-type_info)
6. [Performance: Paying at Compile Time Instead of Run Time](#656-performance-paying-at-compile-time-instead-of-run-time)
7. [Professional Insights](#657-professional-insights)

---

## 65.1 `is_constant_evaluated()` and Its Trap

C++20 introduced `std::is_constant_evaluated()` so a single function could choose a compile-time-friendly path (say, a constexpr-safe algorithm) or a runtime-optimized path (one using intrinsics or `reinterpret_cast` that `constexpr` forbids). It looked like this:

```cpp
constexpr int f(int i) {
    if (std::is_constant_evaluated()) { /* compile-time path */ }
    else { /* runtime path */ }
}
```

The trap is that `std::is_constant_evaluated()` is an ordinary runtime function call that *always returns a `bool`*, and writing it as the condition of a normal `if` invites two classic mistakes:

1. **`if constexpr (std::is_constant_evaluated())`** — this is *always* `true`, because in a `constexpr if` the condition is itself a constant expression being evaluated, so the runtime branch is silently discarded. A subtle, compiler-warning-worthy bug.
2. You **cannot call a `consteval` (immediate) function** in the "compile-time" branch, because from the language's perspective you are still in a normal runtime `if`, and immediate functions may only appear in constant-evaluation contexts.

`if consteval` was added to address both.

---

## 65.2 `if consteval`: The Fix

`if consteval { A } else { B }` is a dedicated language construct (not a function call) that selects branch `A` during constant evaluation and branch `B` otherwise. Crucially, **inside the `consteval` branch you may call immediate (`consteval`) functions directly**, because the language knows that branch executes only during constant evaluation.

**Listing 65.1: `if consteval` selecting a compile-time-only algorithm.**

```cpp
#include <print>

consteval int compile_time_algo(int n) { return n * n; }   // immediate function
int          runtime_algo(int n)       { return n * n; }

constexpr int heavy_math(int n) {
    if consteval {
        return compile_time_algo(n);   // legal: we are definitely at compile time
    } else {
        return runtime_algo(n);        // taken at run time
    }
}

int main() {
    constexpr int a = heavy_math(5);   // uses compile_time_algo -> 25 at compile time
    int b = heavy_math(6);             // uses runtime_algo at run time
    std::println("{} {}", a, b);
}
```

The semantics are exactly what you want: the `consteval` branch is the constant-evaluation path and can use immediate functions; the `else` is the runtime path and can use runtime-only constructs. There is also a negated form, `if !consteval { ... }`, for when the runtime path is the one you want to write first. `if consteval` makes `std::is_constant_evaluated()` largely obsolete for new code — reach for the language construct, not the library function.

---

## 65.3 Relaxed `constexpr` Function Bodies

C++23 removes several restrictions that previously forced you to choose between writing natural code and writing `constexpr` code. Inside a `constexpr` function you may now use:

- **Non-literal variables and non-`constexpr` constructs that are never actually evaluated at compile time.** A `constexpr` function may *contain* operations that are not constant-friendly, as long as a given evaluation does not reach them — the function is no longer rejected merely for having a non-constexpr branch. This is the rule that lets a single `constexpr` function carry both a compile-time and a runtime path (as in Listing 65.1) without the compiler rejecting the runtime path outright.
- **`goto` and labels.** Previously banned in `constexpr` functions, now permitted, so machine-generated code and state machines that use `goto` can be `constexpr`.
- **`static` and `thread_local` local variables.** A `constexpr` function may now declare `static`/`thread_local` locals (they are simply not usable *during* constant evaluation, but their presence no longer disqualifies the function from being `constexpr` for its runtime uses).
- **Labels at the end of compound statements** and other small parser relaxations that remove gratuitous syntactic restrictions.

The throughline: C++23 narrows the gap between "code that can be `constexpr`" and "ordinary code," so marking a function `constexpr` is far less likely to require rewriting its body.

**Listing 65.2: A `constexpr` function using formerly-forbidden constructs.**

```cpp
#include <print>

constexpr int classify(int n) {
    if (n < 0) goto negative;     // goto now allowed in constexpr
    return n == 0 ? 0 : 1;
negative:
    return -1;
}

int main() {
    static_assert(classify(5) == 1);     // evaluated at compile time
    static_assert(classify(-3) == -1);
    std::println("{}", classify(0));     // and still usable at run time
}
```

---

## 65.4 `constexpr` `std::unique_ptr` and Compile-Time Allocation

A landmark relaxation: **`std::unique_ptr` is now `constexpr`**. Building on C++20's `constexpr` dynamic allocation (`new`/`delete` usable during constant evaluation, provided every allocation is freed before the evaluation ends), C++23 makes the owning smart pointer itself usable at compile time. This means you can build, mutate, and tear down owning pointer-based data structures — linked lists, trees, dynamically-sized buffers — entirely within a `constexpr` (or `consteval`) function.

**Listing 65.3: A `constexpr` function that allocates and owns memory.**

```cpp
#include <memory>
#include <print>

// Build a small owned array at compile time, sum it, and free it — all constexpr.
constexpr int sum_dynamic(int n) {
    auto buf = std::make_unique<int[]>(n);   // constexpr allocation + ownership
    for (int i = 0; i < n; ++i) buf[i] = i + 1;
    int total = 0;
    for (int i = 0; i < n; ++i) total += buf[i];
    return total;                            // buf freed here, before evaluation ends
}

int main() {
    static_assert(sum_dynamic(5) == 15);     // 1+2+3+4+5, computed by the compiler
    std::println("{}", sum_dynamic(10));     // also works at run time
}
```

The discipline is unchanged from C++20's constexpr allocation: every byte allocated during constant evaluation must be deallocated before that evaluation finishes — there is no "compile-time leak" into the runtime. But with `constexpr unique_ptr` you can express that ownership with RAII rather than manual `new`/`delete`, which makes compile-time data-structure construction practical.

---

## 65.5 `constexpr` Library Growth: `<cmath>`, `bitset`, `type_info`

C++23 marks a large swath of the standard library `constexpr` that previously was not:

- **Much of `<cmath>`.** Common math functions — `std::abs`, `std::fmin`/`std::fmax`, `std::ceil`, `std::floor`, `std::trunc`, `std::round`, and others — become `constexpr`, so numeric constants and lookup tables that need them can be computed at compile time. (Transcendental functions like `sin`/`exp` remain a more gradual story across implementations.)
- **`std::bitset` is largely `constexpr`.** You can construct, set, test, and manipulate a `bitset` during constant evaluation, enabling compile-time bit masks and flag tables.
- **`constexpr std::type_info::operator==`.** Comparing `type_info` objects for equality is now a constant expression, which lets type-identity checks participate in compile-time logic.
- Numerous other small pieces (`<cstdlib>` integer functions, additional `<optional>`/`<variant>` operations) join the `constexpr` world.

**Listing 65.4: Compile-time use of newly-`constexpr` library facilities.**

```cpp
#include <cmath>
#include <bitset>
#include <print>

constexpr double rounded = std::ceil(3.2);          // 4.0 at compile time

constexpr unsigned mask = [] {
    std::bitset<8> b;                                // constexpr bitset
    b.set(0); b.set(3); b.set(5);
    return static_cast<unsigned>(b.to_ulong());      // 0b00101001 = 41
}();

int main() {
    static_assert(rounded == 4.0);
    static_assert(mask == 41);
    std::println("rounded={} mask={}", rounded, mask);
}
```

> **Version-trap flag:** `if consteval` (and `if !consteval`), the relaxed `constexpr` body rules, `constexpr std::unique_ptr`, and the expanded `constexpr` library coverage are **C++23**. C++20 introduced `is_constant_evaluated()`, `consteval`, and `constexpr` allocation — but *not* `if consteval`, and *not* `constexpr unique_ptr`. Library `constexpr`-ness of specific `<cmath>` functions still varies by implementation; verify with the relevant `__cpp_lib_constexpr_*` feature-test macros for the functions you depend on.

---

## 65.6 Performance: Paying at Compile Time Instead of Run Time

The entire point of this machinery is to **move work from run time to compile time**, where it costs nothing at execution:

- **A value computed in a `constexpr`/`consteval` context is baked into the binary as a literal.** A lookup table, a parsed format descriptor, a precomputed mask — if it can be expressed as constant evaluation, the runtime sees only the finished bytes, with zero startup cost and no runtime computation.
- **`if consteval` lets one function be optimal in both worlds.** The compile-time branch can use a clean, constant-friendly algorithm while the runtime branch uses SIMD intrinsics or bit tricks that `constexpr` forbids — and each call site pays only for the path it takes, with no runtime branch (the selection happens during translation).
- **`constexpr unique_ptr` enables compile-time data structures with no runtime residue.** A tree or table built at compile time and consumed into a constant leaves no allocation in the running program.

The cost is paid in **compile time and compiler memory**. Aggressive constant evaluation — large loops, deep recursion, big allocated structures — can noticeably slow builds and, in extreme cases, hit the implementation's constexpr step limit. The engineering judgment is the usual one: push genuinely-constant work to compile time for zero runtime cost, but do not turn an expensive runtime computation into an expensive *build-time* computation unless the value is truly constant and the build-time hit is acceptable.

---

## 65.7 Professional Insights

**Prefer `if consteval` to `std::is_constant_evaluated()` in all new code.** The library function is a runtime `bool` that is dangerously easy to misuse — putting it in a `constexpr if` silently picks the compile-time branch always, and it cannot gate calls to immediate functions. `if consteval` is a real language construct with the correct semantics: it gates the `consteval`-only path properly and reads as exactly what it does. Treat `is_constant_evaluated()` as legacy.

**Mark functions `constexpr` more liberally now that the body rules are relaxed.** The historical friction — "I'd have to rewrite this to be `constexpr`-legal" — is substantially gone: `goto`, labels, `static` locals, and non-constexpr branches that go un-evaluated no longer disqualify a function. Adding `constexpr` to a function that *can* be evaluated at compile time costs nothing at run time and opens the door to `static_assert`-time checking and constant folding, so the default for pure, side-effect-free functions should lean toward `constexpr`.

**Use `constexpr unique_ptr` and compile-time allocation to precompute, not to show off.** The compelling use is turning a startup-time table build or a constant data-structure initialization into a compile-time constant, so the running program pays nothing. But every compile-time allocation must be freed within the same evaluation, and large compile-time computations tax the build. Reserve it for values that are genuinely constant and worth the build-time cost; do not move inherently-runtime work into the compiler just because you now can.

**Watch the build-time budget as you push more to compile time.** Each value you compute at compile time trades runtime cost for compiler work, and that trade is not always favorable — heavy constant evaluation can dominate build times and stress CI. Treat compile-time computation as a performance optimization with its own cost model: measure its impact on build duration, lean on the `__cpp_lib_constexpr_*` feature-test macros so the code degrades gracefully where a function is not yet `constexpr` on a given toolchain, and keep the heaviest evaluations behind clear, intentional boundaries.
