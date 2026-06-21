# Chapter 86: CPU Microarchitecture for Performance Engineers

The instruction set architecture (x86-64, AArch64) is a contract that says *what* instructions do; the **microarchitecture** is the wildly more complex machine that says *how fast* they do it — and the two are almost unrelated for performance purposes. A modern core decodes your instructions into micro-ops, executes dozens at once out of order across multiple execution ports, speculates past branches it has not resolved, and stalls for a hundred cycles on a single cache miss. This chapter builds the working model of that machine a performance engineer needs: pipelines, superscalar out-of-order execution, speculation, branch prediction, and the dependency chains that actually determine throughput.

## Chapter Roadmap

- 86.1 Why Microarchitecture, Not the ISA, Determines Speed
- 86.2 The Pipeline: Why CPUs Are Pipelined
- 86.3 Superscalar and Out-of-Order Execution
- 86.4 Speculative Execution and Branch Prediction
- 86.5 The Cost of a Misprediction
- 86.6 Data Dependencies and the Real Throughput Limit
- 86.7 Ports, Execution Units, and Instruction-Level Parallelism
- 86.8 Implications for the C++ Programmer

---

## 86.1 Why Microarchitecture, Not the ISA, Determines Speed

When you read assembly (Chapter 89), you see ISA instructions — `add`, `mov`, `cmp`, `jne`. But the core does not execute those directly. It **decodes** each into one or more **micro-ops (µops)**, the actual primitive operations its execution units perform, and schedules those µops dynamically. The same instruction can cost 1 cycle or 200 depending entirely on microarchitectural state — whether its operands are in registers, in L1, or in DRAM; whether a branch before it was predicted correctly; whether its inputs depend on a not-yet-finished operation.

> **Why this matters.** Instruction *count* is a poor predictor of performance; what matters is how the µops flow through the pipeline. Two code sequences with identical instruction counts can differ 10× because one has a long dependency chain (each op waits for the previous) and the other has independent ops the core runs in parallel. This is why "fewer instructions" (Chapter 80) is necessary but not sufficient, and why reading assembly without understanding the microarchitecture misleads. The unit of performance is the µop and its dependencies, not the instruction.

---

## 86.2 The Pipeline: Why CPUs Are Pipelined

Executing an instruction involves several stages — fetch, decode, execute, memory access, write-back. A non-pipelined CPU would do all stages for one instruction before starting the next. A **pipelined** CPU overlaps them like an assembly line: while instruction N is executing, N+1 is being decoded and N+2 fetched. With a *k*-stage pipeline, throughput approaches one instruction per cycle even though each instruction still takes *k* cycles end to end (its **latency**).

> **Why this matters / cost model.** Pipelining is why clock frequency could rise: deeper pipelines (more, simpler stages) allow higher clocks. But the assembly line only stays full if the stream of instructions is **predictable and independent**. Anything that breaks the flow — a branch whose direction is unknown, an operation waiting on a memory load, a dependency on a prior result — introduces a **bubble** (a stall) or, worse, a **flush** (discarding speculative work). The two fundamental enemies of a full pipeline are *control hazards* (branches, §86.4) and *data hazards* (dependencies, §86.6). Almost every microarchitectural performance technique is about keeping the pipeline fed.

---

## 86.3 Superscalar and Out-of-Order Execution

Modern cores go beyond one-instruction-per-cycle in two ways:

- **Superscalar:** the core has *multiple* execution units and can issue several µops per cycle (a modern x86 core can retire ~4–6 µops/cycle).
- **Out-of-order (OoO):** the core does not wait for a stalled instruction; it executes *later* independent instructions whose operands are ready, then **retires** results in program order to preserve the abstract machine's semantics.

The machinery: instructions are decoded into µops, **register renaming** removes false dependencies (two instructions reusing the same architectural register are given different physical registers), µops wait in a **reservation station / scheduler** until their operands are ready, execute on a free port, and their results are held in a **reorder buffer (ROB)** until they can retire in order.

> **Why this matters.** Out-of-order execution is why a cache miss does not necessarily stall the whole core: while one load waits ~100 cycles for DRAM, the core executes dozens of independent later instructions, *hiding* the miss latency behind useful work. This is **memory-level parallelism**, and it is why having *independent* work available around a miss matters enormously (Chapter 87). Register renaming is why you should not hand-minimise register usage — the compiler and hardware manage a far larger physical register file than the ~16 architectural registers, and false dependencies you "optimise away" were already removed. The core is a dataflow engine wearing a sequential-instruction costume.

---

## 86.4 Speculative Execution and Branch Prediction

The pipeline needs to know which instruction comes next, but a conditional branch's direction is not known until it executes — many cycles after it was fetched. Rather than stall, the core **predicts** the branch direction and speculatively executes down the predicted path. If the prediction was right, the work is kept and nothing was lost. If wrong, the speculative work is discarded (flushed) and the correct path restarted.

The **branch predictor** is a sophisticated piece of hardware: it tracks per-branch history (taken/not-taken patterns) and global history (correlations between branches) in tables, achieving >95% accuracy on typical code. Loop branches (taken N times, then not) and biased branches (almost always one way) are predicted nearly perfectly.

> **Why this matters / cost model.** Prediction is what lets deep pipelines work at all — without it, every branch would stall the pipeline for its full depth. A *well-predicted* branch is effectively free. The corollary, central to Chapter 91, is that the cost of a branch is not the branch instruction but the *misprediction rate*: a branch the predictor learns (sorted data, biased conditions) costs nothing; a branch on random data costs a flush half the time. This is the entire reason "processing a sorted array is faster than an unsorted one" despite identical instructions — the predictor learns the sorted pattern.

---

## 86.5 The Cost of a Misprediction

When a branch is mispredicted, every instruction speculatively executed after it must be discarded, and the pipeline must refill from the correct target — a penalty equal to roughly the **pipeline depth**, ~15–20 cycles on modern cores.

```cpp
// Min standard: C++11. Demonstrates the prediction-dependent cost.
// data[] holds random values in [0,255]. Threshold sums those >= 128.
long sum = 0;
for (int i = 0; i < N; ++i)
    if (data[i] >= 128)        // UNPREDICTABLE on random data -> ~50% mispredict
        sum += data[i];
// Sorting `data` first makes the branch predictable (FFF...TTT) -> near-zero mispredict,
// and the loop runs several times faster for the SAME instructions.
```
*Listing 86.1 — The classic "sorted array" effect: branch predictability, not work, dominates.*

> **Why this matters.** A 15–20 cycle flush is ~50–60 instructions of lost work. In a tight loop with an unpredictable branch on every iteration, mispredictions can dominate runtime. The mitigations — making branches predictable (sort, group), removing them (branchless/predication, Chapter 91), or hinting their bias (`[[likely]]`/`[[unlikely]]`) — all target this specific cost. But note the asymmetry: a *predictable* branch is cheaper than the branchless alternative (which always does both sides' work), so the fix is only worthwhile when the branch is genuinely unpredictable. Measure the misprediction rate (`perf stat` reports `branch-misses`) before "optimising" a branch.

---

## 86.6 Data Dependencies and the Real Throughput Limit

Out-of-order execution can only run instructions in parallel if they are *independent*. A chain where each operation needs the previous one's result — a **dependency chain** (or critical path) — cannot be parallelised, and its length, multiplied by each operation's latency, sets a hard floor on runtime regardless of how many execution units exist.

```cpp
// Min standard: C++11. Two loops, same instruction count, very different speed.
// (a) Serial dependency chain: each += depends on the previous sum.
float sum = 0;
for (int i = 0; i < N; ++i) sum += a[i];        // latency-bound: one add per ~4 cycles (FP add latency)

// (b) Broken into independent chains the core runs in parallel:
float s0=0, s1=0, s2=0, s3=0;
for (int i = 0; i < N; i += 4) {
    s0 += a[i]; s1 += a[i+1]; s2 += a[i+2]; s3 += a[i+3];   // 4 independent chains
}
float total = s0 + s1 + s2 + s3;                 // throughput-bound: ~4x faster
```
*Listing 86.2 — Breaking a dependency chain unlocks instruction-level parallelism.*

> **Why this matters / cost model.** This is the single most counterintuitive microarchitectural fact: version (b) does the *same arithmetic* as (a) but runs ~4× faster, because (a) is limited by the *latency* of floating-point addition (each `sum +=` must wait for the previous), while (b) has four independent accumulators that the superscalar core advances simultaneously, limited instead by *throughput*. This is why compilers unroll-and-reduce, why `-ffast-math` (which lets the compiler reassociate FP and do this automatically) can multiply performance, and why naive serial reductions leave most of the core idle. Identifying and breaking the critical dependency chain is often the highest-leverage micro-optimization there is.

---

## 86.7 Ports, Execution Units, and Instruction-Level Parallelism

The superscalar core's execution units are grouped behind **ports** — a modern x86 core has ~8–10 ports, each able to start one µop per cycle, with specific units behind them (multiple ALUs, load ports, store ports, FP/vector units). The achievable **instruction-level parallelism (ILP)** is bounded by which ports your µops need: four independent adds can issue together if there are four ALU ports, but four loads may bottleneck on two load ports.

> **Why this matters.** Port pressure is the ceiling that Listing 86.2's parallel version eventually hits — you can break dependency chains only until you saturate the relevant ports. This is why the practical unroll factor is small (2–8): beyond the port count, more independent chains do not help. Tools like LLVM-MCA and `uops.info` model exactly which ports each instruction uses, letting you compute a loop's theoretical throughput. For most engineers the takeaway is qualitative: the core can do several *different kinds* of work per cycle (loads, adds, multiplies, branches) in parallel, so mixing operation types and avoiding long single-type chains keeps more ports busy.

---

## 86.8 Implications for the C++ Programmer

| Microarchitectural fact | C++ consequence |
|---|---|
| µops, not instructions, are the unit | Read assembly *and* model dependencies (Chapter 89) |
| OoO hides miss latency behind independent work | Provide independent work around loads (Chapter 87) |
| Branch prediction makes predictable branches free | Sort/group data; branchless only for *unpredictable* branches (Chapter 91) |
| Mispredict = ~15–20 cycle flush | Measure `branch-misses` before optimising a branch |
| Dependency-chain length is the throughput floor | Break reductions into independent accumulators |
| Finite ports cap ILP | Unroll modestly; mix operation types |

> **The discipline.** You rarely program the microarchitecture directly, but you constantly program *for* it: give the out-of-order engine independent work to hide latency, keep branches predictable or remove them, and break the dependency chains that starve the execution units. These are the levers behind every chapter in the compute cost model — branchless code (91), SIMD (92), and reading the optimizer's output (89). Next, the other half of performance: the memory hierarchy that feeds this hungry engine.
