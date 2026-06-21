# Chapter 92: SIMD in Practice — Intrinsics, Auto-Vectorisation, and std::simd

A modern core can add eight `float`s, or sixteen with AVX-512, in a single instruction — a potential 8–16× speedup that is left entirely on the table by scalar code. **SIMD** (Single Instruction, Multiple Data) exploits the wide vector registers and execution units that every server and most client CPUs have carried for over a decade. But the speedup is conditional: it materialises only when the data is laid out for it, the loop has no cross-lane dependencies, and the operation is genuinely arithmetic-bound. This chapter covers the three routes to SIMD (auto-vectorisation, intrinsics, `std::simd`), the alignment and layout prerequisites, and the cost model that says when vectorisation pays.

## Chapter Roadmap

- 92.1 What SIMD Is and Why It Exists
- 92.2 The Three Routes to SIMD
- 92.3 Auto-Vectorisation: The Default First Try
- 92.4 Intrinsics: Explicit Control
- 92.5 `std::simd`: Portable Explicit SIMD
- 92.6 Alignment, Tails, and Horizontal Operations
- 92.7 When Vectorisation Does and Doesn't Pay

---

## 92.1 What SIMD Is and Why It Exists

A scalar instruction operates on one value; a **SIMD** instruction operates on a *vector* of values in a wide register — 128-bit SSE (4 floats), 256-bit AVX/AVX2 (8 floats), 512-bit AVX-512 (16 floats); on ARM, 128-bit NEON and scalable SVE. The same `add` hardware, widened, does N additions for the price of one instruction.

> **Why this matters / cost model.** SIMD is *data parallelism* exploited within a single core, distinct from the *instruction-level* parallelism of out-of-order execution (Chapter 86) and the *thread* parallelism of multiple cores. The three compose: a vectorised loop on a multi-core machine with good ILP can be hundreds of times faster than naive scalar single-threaded code. The ceiling is set by the vector width and the operation: 8-wide `float` AVX caps a compute-bound loop at ~8× — *if* you can keep the vector units fed. That qualifier is the whole story: SIMD multiplies *arithmetic* throughput, so it helps only when arithmetic, not memory or branches, is the bottleneck.

---

## 92.2 The Three Routes to SIMD

| Route | Control | Portability | Effort | When |
|---|---|---|---|---|
| **Auto-vectorisation** | Compiler decides | Fully portable | Lowest | Always try first |
| **`std::simd`** (C++26) / libraries | Explicit, portable | Portable across ISAs | Medium | Need guaranteed vectorisation, portably |
| **Intrinsics** | Per-instruction | ISA-specific (x86/ARM) | Highest | Last 10–20%, ISA-specific kernels |

> **Why this matters.** The routes form a ladder of escalating control and cost, and you climb it only as far as you must. Auto-vectorisation is free and maintainable but *fragile* — a tiny code change can silently de-vectorise a loop. `std::simd`/libraries (Highway, Vc, xsimd) give *guaranteed*, portable vectorisation with readable code. Intrinsics give maximal performance but lock you to one ISA and are write-once-read-never. The mastery move is to start at the bottom (write vectorisable scalar code, let the compiler vectorise), verify in the disassembly (Chapter 89), and only escalate when the compiler demonstrably fails and the kernel is hot enough to justify the cost.

---

## 92.3 Auto-Vectorisation: The Default First Try

The compiler will vectorise a loop automatically if it can prove the iterations are independent and the access pattern is regular. Your job is to *not block it*.

```cpp
// Min standard: C++11. Portable. Auto-vectorises at -O2/-O3 if a and b don't alias.
void scale(float* __restrict a, const float* __restrict b, float k, size_t n) {
    for (size_t i = 0; i < n; ++i)
        a[i] = b[i] * k;        // unit-stride, independent, no branches -> vectorises
}
```
*Listing 92.1 — A vectorisable loop. `__restrict` (non-portable but widely supported) promises no aliasing, which the vectoriser needs.*

What blocks auto-vectorisation:

- **Possible aliasing** — if `a` and `b` might overlap, the compiler must assume each store affects later loads; `__restrict` (or `std::span` discipline) resolves it.
- **Loop-carried dependencies** — `a[i] = a[i-1] + b[i]` is serial (Chapter 86's critical path).
- **Branches with side effects** — recast as masks (Chapter 91).
- **Non-unit stride / gather** — SoA layout (Chapter 90) gives unit stride.
- **Function calls in the loop** — inline them first (Chapter 89).

> **Why this matters.** Auto-vectorisation fails *silently* and for fixable reasons. The diagnostic flags `-Rpass=loop-vectorize` / `-Rpass-missed=loop-vectorize` (Clang) and `-fopt-info-vec[-missed]` (GCC) tell you exactly which loops vectorised and why others didn't — usually aliasing or a dependency you can remove. This is the highest-leverage SIMD work: a one-line `__restrict` or a SoA layout change can unlock an 8× speedup with zero unportable code. Always exhaust auto-vectorisation, with the remarks flags on, before writing a single intrinsic.

---

## 92.4 Intrinsics: Explicit Control

**Intrinsics** are compiler built-ins that map almost one-to-one to SIMD instructions, giving per-instruction control. They are the route when the compiler cannot vectorise (irregular algorithms, shuffles, custom reductions) and the kernel is hot enough to justify ISA-specific code.

```cpp
// Min standard: C++11 + AVX. NON-PORTABLE: x86 AVX only; needs -mavx and runtime CPU check.
#include <immintrin.h>
void add_avx(float* a, const float* b, size_t n) {   // assumes n % 8 == 0, 32-byte aligned
    for (size_t i = 0; i < n; i += 8) {
        __m256 va = _mm256_load_ps(a + i);            // aligned load of 8 floats
        __m256 vb = _mm256_load_ps(b + i);
        __m256 vs = _mm256_add_ps(va, vb);            // 8 adds in one instruction
        _mm256_store_ps(a + i, vs);
    }
}
```
*Listing 92.2 — AVX intrinsics. Non-portable (x86-only), requires alignment, and needs a scalar tail for `n % 8 ≠ 0`.*

> **Why this matters / cost model.** Intrinsics extract the last 10–20% the compiler leaves behind and enable algorithms with no scalar analogue (vectorised string search, SIMD JSON parsing, shuffle-heavy kernels). The costs are severe: the code is **non-portable** (a separate path per ISA — SSE, AVX2, AVX-512, NEON — often selected at runtime by CPU feature detection), unreadable, and easy to get wrong (alignment faults, forgotten tails). `_mm256_load_ps` *requires* 32-byte alignment and faults otherwise; `_mm256_loadu_ps` is the unaligned (slightly slower) form. Reserve intrinsics for proven-hot kernels you are willing to maintain per-architecture.

---

## 92.5 `std::simd`: Portable Explicit SIMD

C++26's **`std::simd`** (long available as `std::experimental::simd`) provides explicit, *portable* vectorisation: a `simd<float>` is a vector whose width the implementation chooses for the target ISA, with overloaded operators that map to the native SIMD instructions.

```cpp
// Min standard: C++26 (std::simd) or the experimental TS. Portable across ISAs.
#include <simd>                                   // <experimental/simd> for the TS
namespace stdx = std;                             // or std::experimental
void add_simd(float* a, const float* b, size_t n) {
    using V = stdx::simd<float>;
    size_t w = V::size(), i = 0;
    for (; i + w <= n; i += w) {
        V va(&a[i], stdx::element_aligned);
        V vb(&b[i], stdx::element_aligned);
        (va + vb).copy_to(&a[i], stdx::element_aligned);   // native width, portable source
    }
    for (; i < n; ++i) a[i] += b[i];              // scalar tail
}
```
*Listing 92.2b — `std::simd`: one source, vectorises natively on x86, ARM, etc. Min standard C++26 / experimental TS.*

> **Why this matters.** `std::simd` is the sweet spot the volume points toward: *guaranteed* vectorisation (unlike fragile auto-vectorisation) that is *portable* (unlike intrinsics) and *readable* (operators, not `_mm256_*`). The width adapts to the target — 8 lanes on AVX2, 16 on AVX-512, 4 on NEON — from one source. Its limits: it covers the common "vertical" operations cleanly but expresses exotic shuffles less directly than raw intrinsics, and (as of C++26) it is still stabilising across toolchains. For most vectorisation that auto-vectorisation can't guarantee, `std::simd` or a library like Google Highway is the right tool — far preferable to per-ISA intrinsics.

---

## 92.6 Alignment, Tails, and Horizontal Operations

Three practical concerns recur in all explicit SIMD:

- **Alignment.** Aligned loads/stores (`_mm256_load_ps`) require the data be aligned to the vector width (32 bytes for AVX) and fault otherwise; unaligned forms (`loadu`) work anywhere but were historically slower (the gap is small on modern CPUs). Allocate SIMD buffers with `alignas(32)`/`alignas(64)` or aligned `new` (Chapter 79).
- **Tail handling.** A loop over N elements with width W must handle the `N % W` leftover with a scalar tail (or a masked final iteration). Forgetting the tail is a correctness bug.
- **Horizontal vs vertical operations.** *Vertical* ops (lane *i* of A with lane *i* of B) are cheap and natural. *Horizontal* ops (reducing across lanes — summing a vector to a scalar) are expensive and break the data-parallel model; keep N independent partial sums in a vector and reduce *once* at the very end (the multiple-accumulator pattern of Chapter 86), not every iteration.

> **Why this matters / cost model.** These three account for most SIMD bugs and disappointing speedups. A horizontal reduction inside the loop serialises the vector lanes and can erase the vectorisation win — the fix (vector of partial sums, one final horizontal reduce) is the SIMD form of breaking the dependency chain. Misalignment either faults (aligned load on unaligned data) or silently costs a little; a forgotten tail corrupts the last few elements. Mastering these turns "I vectorised it but it's not faster" into a correct, fast kernel.

---

## 92.7 When Vectorisation Does and Doesn't Pay

| Workload | Vectorises well? | Why |
|---|---|---|
| Dense numeric arrays (dot product, scale, filter) | **Yes** | Arithmetic-bound, unit stride, independent |
| Image/audio/DSP, ML inference kernels | **Yes** | Data-parallel, regular |
| Pointer-chasing (lists, trees, hash lookups) | **No** | No data parallelism; memory-bound, irregular |
| Branchy control flow per element | Only via masking | Branches don't vectorise (Chapter 91) |
| Memory-bandwidth-bound loops | Marginally | Already limited by DRAM, not compute |
| Short loops (N < width) | **No** | Setup/tail overhead exceeds the gain |

> **Why this matters.** SIMD multiplies *compute* throughput, so it pays exactly when compute is the bottleneck and the data is regular and parallel. It does nothing for memory-bound code (the loop already waits on DRAM, not the ALU — fix layout first, Chapter 87), nothing for pointer-chasing (no parallel lanes to fill), and is counterproductive for tiny loops. This is why the volume's ordering matters: get the *data layout* right (SoA, Chapter 90) and the *branches* out (Chapter 91) *first* — those are often what unblock vectorisation *and* are prerequisites for the compute to be the bottleneck at all.

> **The discipline.** Climb the SIMD ladder deliberately: (1) write loops that *can* vectorise — unit stride (SoA), no aliasing (`__restrict`), no per-element branches (masks), no loop-carried dependencies; (2) let the compiler auto-vectorise and *verify* with `-Rpass`/disassembly; (3) if it can't, reach for `std::simd`/Highway for portable explicit vectorisation; (4) drop to ISA intrinsics only for proven-hot kernels worth maintaining per-architecture. And always confirm the workload is compute-bound first — SIMD applied to memory-bound or branchy code is wasted effort. This closes the compute-cost block; the volume now turns to concurrency, where the memory model and lock-free chapters await.
