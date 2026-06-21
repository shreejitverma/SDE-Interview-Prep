# Chapter 89: Reading the Compiler's Output — Assembly, Inlining, and the Optimiser

The only ground truth about what your code costs is the machine code the compiler emitted — not the source, not your intuition, not a microbenchmark that may have been optimised away. Learning to read disassembly converts performance work from speculation into observation: you can *see* whether a function inlined, whether a loop vectorised, whether a bounds check was elided, whether your "optimisation" changed a single instruction. This chapter teaches the Godbolt workflow, enough x86-64 to read a hot loop, and the optimiser behaviours — inlining, vectorisation, the as-if rule — that determine whether your abstractions are free.

## Chapter Roadmap

- 89.1 Why Read Assembly at All
- 89.2 The Godbolt Workflow
- 89.3 Enough x86-64 to Read a Hot Loop
- 89.4 Mapping C++ Constructs to Assembly
- 89.5 Inlining: The Optimiser's Master Lever
- 89.6 Recognising Vectorisation and Other Optimisations
- 89.7 The As-If Rule and What It Lets the Optimiser Do
- 89.8 A Practical Reading Discipline

---

## 89.1 Why Read Assembly at All

High-level C++ is many transformations removed from what runs (Chapter 81). The optimiser may inline a function to nothing, vectorise a loop, fold a `constexpr` to a constant, hoist a load out of a loop, or delete code it proves has no effect. Conversely it may *fail* to do any of these because of a hidden dependency, an aliasing concern, or a missing `noexcept`. The source tells you none of this; the assembly tells you all of it.

> **Why this matters.** Two questions recur in performance work and only assembly answers them definitively: *Did the optimisation I expected actually happen?* and *Why is this slower than it should be?* A "zero-cost abstraction" is only zero-cost if it compiled to the same instructions as the hand-written version — and the way you *know* is to compare the disassembly. Reading assembly is also the only reliable defence against the benchmark trap (Chapter 103): if the compiler deleted the work you meant to measure, the assembly shows an empty loop.

---

## 89.2 The Godbolt Workflow

**Compiler Explorer** (godbolt.org) is the standard tool: paste C++, pick a compiler and flags, and see the generated assembly side by side, with colour-coding that maps source lines to instructions. Locally, the equivalent is:

```bash
# Emit assembly with source interleaved (GCC/Clang). Min: any modern GCC/Clang.
g++ -O2 -S -masm=intel -fverbose-asm hot.cpp -o hot.s   # human-readable Intel syntax
objdump -d -M intel --no-show-raw-insn a.out            # disassemble a built binary
clang++ -O2 -S -emit-llvm hot.cpp -o hot.ll             # see the LLVM IR before codegen
```
*Listing 89.1 — Emitting assembly locally. `-O2` is essential — `-O0` output is unreadable and unrepresentative.*

> **Why this matters.** The single most important habit is to **always read optimised output** (`-O2`/`-O3`), never `-O0`. Unoptimised assembly spills every variable to the stack and inlines nothing — it bears no resemblance to release performance and will mislead you completely. Compiler Explorer's source-to-asm colour mapping is the fastest way to find which instructions a given line produced. Comparing two implementations side by side (same flags) is how you settle "is A faster than B" at the instruction level before you even run anything.

---

## 89.3 Enough x86-64 to Read a Hot Loop

You do not need to *write* assembly to *read* it. The essentials of x86-64 (Intel syntax, `dest, src`):

| Element | Meaning |
|---|---|
| `rax, rbx, ...` | 64-bit general registers; `eax`=low 32, `al`=low 8 |
| `xmm0, ymm0, zmm0` | 128/256/512-bit SIMD vector registers |
| `mov rax, [rbx]` | load from memory at address in `rbx` into `rax` |
| `lea rax, [rbx+rcx*4]` | address computation (no memory access) |
| `add/sub/imul` | integer arithmetic |
| `cmp` + `jne/jl/jge` | compare then conditional branch |
| `call/ret` | function call / return |
| `addps/vaddps`, `mulps` | packed (SIMD) float arithmetic — a sign of vectorisation |

```asm
; A simple sum loop at -O2 (Intel syntax). Reading it:
.L3:
    add    eax, [rdi+rcx*4]   ; sum += a[i]      (load a[i], add to eax)
    add    rcx, 1             ; ++i
    cmp    rcx, rdx           ; i < n ?
    jne    .L3                ; loop
```
*Listing 89.2 — A scalar reduction loop. `[rdi+rcx*4]` is `a[i]` (base + index*sizeof(int)).*

> **Why this matters.** Recognising a handful of patterns is enough for 90% of performance reading: a `call` that you expected to be inlined (it wasn't); `xmm`/`ymm` registers and `vaddps`/`vmovups` (it vectorised); a tight loop body with no `call` (it inlined); extra `cmp`/`jae`/`call __stack_chk` (a bounds check or stack protector). You read for *shape*, not every instruction: how many memory accesses per iteration, is there a branch, is it scalar or vector, did the call disappear.

---

## 89.4 Mapping C++ Constructs to Assembly

- **A function call** is `call sym`; if you see it, the callee was *not* inlined (§89.5).
- **A virtual call** is a vtable load then an *indirect* `call [rax]` — visibly more expensive than a direct `call`, and an inlining barrier (Chapter 74's CRTP eliminates it).
- **A `std::vector` element access** is base+index addressing (`[rdi+rcx*8]`); `operator[]` adds no code at `-O2`, but `.at()` adds a compare and a throw path.
- **A bounds check** appears as a `cmp`/`jae` guarding the access with a branch to a throw/abort.
- **A `constexpr` result** appears as an immediate constant — the computation is gone entirely.

> **Why this matters.** This mapping is how you verify the abstractions you rely on are free. Did `operator[]` cost the same as raw pointer indexing? The addressing modes are identical — yes. Did the `unique_ptr` add overhead? At `-O2` it compiles to the same load as a raw pointer — yes, free. Did the `std::sort` comparator inline? If you see the comparison inline in the sort body rather than a `call`, yes. Each verification is a side-by-side disassembly comparison, and each one either confirms zero-cost or reveals a tax you can then remove.

---

## 89.5 Inlining: The Optimiser's Master Lever

**Inlining** replaces a call with the callee's body. It is the single most important optimisation because it is an *enabler*: once a function is inlined, the optimiser can propagate constants into it, eliminate dead branches for the specific call site, keep values in registers across the former call boundary, and vectorise loops that span it. A call that is *not* inlined is an opaque barrier — the optimiser must assume the callee clobbers memory and cannot optimise across it.

> **Why this matters / cost model.** Inlining's direct saving (the call/return overhead, ~a few cycles) is the *least* of its value; the indirect enabling of every other optimisation is the real win, and it is why small hot functions should be inlinable. The compiler inlines based on heuristics (callee size, call-site count, `-O` level); you influence it with `inline`/`__attribute__((always_inline))` (a hint/force, non-portable for the latter) and you *defeat* it with virtual calls, function pointers, calls across un-LTO'd translation units (Chapter 102), and very large function bodies. The first thing to check when a hot path is slow: did the key function inline? If the assembly shows a `call`, that is your barrier.

---

## 89.6 Recognising Vectorisation and Other Optimisations

The optimiser performs many transformations you can spot in the output:

- **Auto-vectorisation:** `xmm`/`ymm`/`zmm` registers and packed ops (`vaddps`, `vfmadd...`, `vmovups`) instead of scalar ones; the loop processes multiple elements per iteration (Chapter 92).
- **Loop unrolling:** the loop body appears 2–8× with a smaller iteration count.
- **Constant folding / propagation:** computed values appear as immediates.
- **Dead-code elimination:** code with no observable effect is simply absent.
- **Strength reduction:** an expensive op (multiply, divide) replaced by cheaper ones (shift, `lea`, multiply-by-reciprocal).
- **Hoisting (LICM):** a loop-invariant load/computation moved before the loop.

> **Why this matters.** Each presence/absence is actionable. If a loop did *not* vectorise, `-fopt-info-vec-missed` (GCC) / `-Rpass-missed=loop-vectorize` (Clang) tells you *why* — usually a dependency, aliasing, or non-unit stride you can fix (Chapter 92). If a redundant computation was *not* hoisted, you may have a hidden aliasing or volatility issue. The disassembly plus the compiler's optimisation-remark flags turn "I hope it optimised" into a diagnosable conversation with the compiler.

---

## 89.7 The As-If Rule and What It Lets the Optimiser Do

The optimiser operates under the **as-if rule**: it may perform *any* transformation as long as the program's observable behaviour (I/O, volatile accesses, atomic operations, program termination) is unchanged. It is free to reorder, combine, eliminate, and invent operations within that constraint.

> **Why this matters.** The as-if rule is the legal basis for everything in §89.6 — and for the surprises. It is why a benchmark loop with no observable effect can be **deleted entirely** (Chapter 103): the optimiser proves the result is unused and removes the work. It is why **undefined behaviour is exploitable** (Chapter 104): once UB occurs, the program has *no* defined observable behaviour to preserve, so the optimiser's transformations are unconstrained — a signed-overflow assumption can delete a safety check. And it is why `volatile` exists: it marks accesses as observable so the optimiser *cannot* elide them (correct for MMIO, but no help for threading — Chapter 76). Reading assembly is, ultimately, reading the as-if rule's decisions for your specific code.

---

## 89.8 A Practical Reading Discipline

| Question | What to look for in `-O2` assembly |
|---|---|
| Did my function inline? | Absence of `call` to it in the caller |
| Did my loop vectorise? | `ymm`/`zmm` registers, `vaddps`/`vfmadd` |
| Is my abstraction free? | Identical instructions to the hand-written version |
| Is there a hidden cost? | Unexpected `call`, bounds-check `cmp`/`jae`, virtual `call [reg]` |
| Did the compiler delete my benchmark? | An empty or trivial loop body |
| Why didn't it optimise? | `-Rpass-missed=` / `-fopt-info-missed` remarks |

> **The discipline.** Reading assembly is not about writing it; it is about *verification*. Before claiming an optimisation worked, look at the output. Before believing a benchmark, confirm the work survived. Before trusting an abstraction is zero-cost, diff its disassembly against the manual version. This habit — ground every performance claim in the emitted instructions — is what makes the rest of the compute-cost chapters (branchless, SIMD) trustworthy, and it is the prerequisite for the measurement rigour of Chapter 103. The next chapter turns from reading the code to shaping the *data* it operates on.
