# Chapter 91: Branchless Programming and Predication

A branch the CPU predicts correctly is nearly free; a branch it predicts wrong costs a ~15–20 cycle pipeline flush (Chapter 86). **Branchless programming** replaces an unpredictable branch with straight-line arithmetic or a conditional-move instruction, trading a *possible* misprediction for the *guaranteed* small cost of computing both outcomes. The technique is powerful and frequently misapplied: on a predictable branch it is a pessimisation. This chapter develops predication, the bit-trick toolkit, and — most importantly — the cost model that decides when removing a branch actually helps.

## Chapter Roadmap

- 91.1 The Branch Cost Model, Revisited
- 91.2 Predication and Conditional Moves
- 91.3 The Branchless Toolkit
- 91.4 When Branchless Wins — and When It Loses
- 91.5 Branchless and SIMD Masking
- 91.6 Hazards and the Measurement Rule

---

## 91.1 The Branch Cost Model, Revisited

The cost of a conditional branch is not the branch instruction (which is cheap and often predicted in parallel) but the **misprediction penalty** times the **misprediction rate**:

```
expected branch cost ≈ branch_overhead + mispredict_rate × pipeline_flush_penalty
```

A branch on a *biased* or *patterned* condition (almost always taken, or a loop counter, or sorted data) is predicted >99% correctly — `mispredict_rate ≈ 0`, so the branch is essentially free. A branch on a *random* condition (a coin-flip predicate over unsorted data) mispredicts ~50% of the time — at ~15–20 cycles per flush, that is ~7–10 cycles of average penalty *per branch*, which in a tight loop dominates everything else.

> **Why this matters.** This formula is the entire decision procedure for branchless programming. You remove a branch *only* when its misprediction rate is high, because the branchless replacement pays a fixed cost (computing both sides) on *every* iteration. If the branch was already predictable, you have replaced "free" with "always does extra work" — a loss. The corollary is that you cannot decide whether to go branchless from the source code alone; you must know (or measure, via `perf stat`'s `branch-misses`) the branch's predictability on real data.

---

## 91.2 Predication and Conditional Moves

**Predication** computes a value *unconditionally* and selects the result without a control-flow branch. At the hardware level this is the **conditional move** (`cmov` on x86, `csel` on ARM): an instruction that copies a source to a destination *if* a flag is set, with no branch and thus no misprediction possible.

```cpp
// Min standard: C++11. Portable; compiler chooses cmov vs branch.
int max_branchy(int a, int b)   { if (a > b) return a; else return b; }   // may mispredict
int max_predicated(int a, int b){ return (a > b) ? a : b; }               // compiler may emit cmov
```
*Listing 91.1 — A ternary often compiles to a branchless `cmov`; an `if` may or may not. Check the assembly (Chapter 89).*

> **Why this matters / cost model.** `cmov` turns a control dependency into a *data* dependency: instead of the pipeline guessing which path to fetch, it computes both operands and selects, so there is nothing to mispredict. The cost is real but fixed: `cmov` adds the selected value to the dependency chain (it cannot retire until its flag input is ready), and both candidate values must be computed. The compiler decides between branch and `cmov` using its own heuristics (and PGO data if available) — which means writing a ternary does *not* guarantee branchless code, and writing an `if` does not guarantee a branch. The only way to know what you got is to read the disassembly.

---

## 91.3 The Branchless Toolkit

Common conditional patterns have branchless arithmetic forms:

```cpp
// Min standard: C++11. Portable. Assumes two's-complement (guaranteed in C++20).
// Boolean to value — the canonical branchless idiom:
int y = (x > 0);                       // 0 or 1, no branch

// Conditional accumulate (the "sum if" of the sorted-array example):
sum += (data[i] >= 128) * data[i];     // adds data[i] when condition holds, else +0

// Branchless absolute value (two's-complement):
int abs_b(int x) { int m = x >> (sizeof(int)*8 - 1); return (x ^ m) - m; }  // m is all-1s if x<0

// Branchless min/max without cmov (portable arithmetic):
int min_b(int a, int b) { return b ^ ((a ^ b) & -(a < b)); }

// Conditionally clear/select with a mask:
unsigned select(bool c, unsigned a, unsigned b) {
    unsigned mask = -(unsigned)c;       // 0xFFFF.. if c, else 0
    return (a & mask) | (b & ~mask);
}
```
*Listing 91.2 — Branchless idioms. The mask trick `-(cond)` (all-ones or all-zeros) is the workhorse.*

> **Why this matters.** The unifying technique is the **mask**: convert a boolean into an all-ones or all-zeros bit pattern (`-(unsigned)cond`), then use bitwise AND/OR to select. This generalises directly to SIMD (§91.5), where there is no per-lane branch at all and masking is the *only* way to do conditional work. These idioms also avoid `cmov`'s dependency on the flags register and can be cheaper when the compiler can fold them. But they trade readability for speed — `min_b` is unrecognisable next to `std::min` — so they belong only in measured hot paths, with a comment explaining the intent.

---

## 91.4 When Branchless Wins — and When It Loses

| Branch character | Misprediction rate | Branchless verdict |
|---|---|---|
| Loop counter, biased (`if (rare)`) | ~0% | **Keep the branch** — branchless adds pointless work |
| Sorted / grouped data | ~0% after the boundary | **Keep the branch** — predictor learns it |
| Random / unpredictable predicate | ~50% | **Go branchless** — eliminates the flush |
| Inside a SIMD loop | N/A (no scalar branch) | **Must use masking** — branches don't vectorise |
| Branch guarding very expensive work | any | **Keep the branch** — don't compute the expensive side unconditionally |

> **Why this matters / cost model.** The last row is a crucial nuance: branchless computes *both* sides, so if one side is expensive (a function call, a memory load that might fault, a division), unconditionally evaluating it can cost far more than an occasional mispredict — and may be *incorrect* (evaluating the side that would dereference a null pointer). Branchless is for cheap, side-effect-free alternatives. The win case is narrow but real: an unpredictable branch guarding *cheap* work in a *hot* loop. Outside that, prefer making the branch predictable (sort, partition, group) over removing it — a predictable branch beats branchless because it does the work of only one side.

---

## 91.5 Branchless and SIMD Masking

SIMD has no per-lane control flow: a single instruction processes N lanes, and you cannot branch differently per lane. **Masking** is therefore not optional in vector code — it is the only conditional mechanism. You compute a comparison mask (a vector of per-lane all-ones/all-zeros), compute results for all lanes, and blend by mask.

```cpp
// Min standard: C++17 + AVX intrinsics (non-portable: x86 AVX only). Conceptual.
// for each 8-lane chunk: mask = (data >= 128); sum += blend(0, data, mask);
// __m256i mask = _mm256_cmpgt_epi32(v, thresh);
// vsum = _mm256_add_epi32(vsum, _mm256_and_si256(v, mask));   // add data where mask set, else 0
```
*Listing 91.3 — SIMD conditional accumulation via masking. AVX intrinsics are x86-specific (Chapter 92).*

> **Why this matters.** This is why branchless thinking is a *prerequisite* for SIMD (Chapter 92), not just an alternative to branches: a loop full of unpredictable scalar branches cannot be vectorised at all, because there is no vector "if." Recasting the conditional as a mask-and-blend both removes the misprediction (scalar benefit) *and* unlocks vectorisation (the much larger benefit). The sorted-array threshold-sum that was the poster child for branch misprediction in Chapter 86 becomes, in masked SIMD form, both branchless and 8-wide — a compounding win.

---

## 91.6 Hazards and the Measurement Rule

- **Pessimising predictable branches.** The most common mistake — going branchless on a branch the predictor already handles, adding work for no gain.
- **Evaluating expensive or unsafe sides unconditionally.** Branchless computes both; never apply it when a side has cost or side effects (allocation, I/O, a load that could fault, division by a possibly-zero divisor).
- **Assuming the source dictates the assembly.** A ternary may compile to a branch; an `if` may compile to `cmov`. The compiler decides; verify with disassembly (Chapter 89).
- **Two's-complement assumptions.** Bit tricks like `x >> 31` for the sign assume two's-complement (guaranteed only since C++20) and specific widths; document them.
- **Readability debt.** Branchless arithmetic is opaque. Confine it to measured hot paths and comment the intent.

> **The discipline.** Branchless programming is a precision tool, not a default. The procedure is: (1) profile to find a hot branch with a *high* misprediction rate (`perf stat`); (2) try first to make it *predictable* (sort, partition) — usually the better fix; (3) if it is inherently unpredictable and guards cheap, safe work, go branchless via `cmov` or masking; (4) verify in the disassembly that the branch is gone and the result is correct; (5) measure that it actually got faster. Skipping step (1) — going branchless on intuition — is how engineers make code both slower and unreadable. With branches handled, the next chapter scales the same masked, data-parallel thinking up to full SIMD.
