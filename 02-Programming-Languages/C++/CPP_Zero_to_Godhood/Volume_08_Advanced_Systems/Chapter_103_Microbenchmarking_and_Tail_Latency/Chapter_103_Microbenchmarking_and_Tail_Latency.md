# Chapter 103: Microbenchmarking and Tail-Latency Measurement

A benchmark is an experiment, and like any experiment it can be designed so badly that its result is worse than no data — a confidently wrong number that sends a team optimising the wrong thing. The optimizer deletes work you meant to time, the mean hides the tail that actually matters, and the measurement loop itself perturbs what it measures. This chapter is the rigorous practice of microbenchmarking and latency measurement: defeating the optimizer's elision, measuring steady state, and — most importantly for the systems this volume targets — characterising the *full latency distribution* and its tail rather than a meaningless average.

## Chapter Roadmap

- 103.1 Why Benchmarks Lie
- 103.2 Defeating Dead-Code Elimination
- 103.3 Warmup and Steady State
- 103.4 Statistical Rigour: Noise, Runs, and Significance
- 103.5 Tail Latency: Why the Mean Is Useless
- 103.6 Coordinated Omission
- 103.7 The Profiling Workflow
- 103.8 The Discipline

---

## 103.1 Why Benchmarks Lie

The most dangerous output of a benchmark is a plausible-but-wrong number. The ways it happens:

- **Dead-code elimination:** the optimizer proves your benchmark's result is unused and *deletes the work* (the as-if rule, Chapter 89) — you measure an empty loop.
- **Constant folding:** if the input is a compile-time constant, the optimizer computes the answer at build time and the loop does nothing.
- **Cold start:** the first iterations pay cache, TLB, and branch-predictor warmup costs that do not reflect steady state.
- **Measurement perturbation:** the timing code's overhead, the loop's own branches, and memory effects contaminate the result (Chapter 101).
- **Unrepresentative conditions:** measuring on an idle machine, with a tiny working set that fits in L1, or without contention, then extrapolating to production.

> **Why this matters.** Each of these has caused real teams to "optimise" something that was already free, or to ship a change that helped the benchmark and hurt production. A benchmark that the optimizer hollowed out reports nanoseconds and proves nothing; a benchmark on an L1-resident dataset says nothing about the cache-missing production workload. Microbenchmarking is a discipline precisely because the naive version is so reliably misleading — and the fixes are specific, learnable techniques.

---

## 103.2 Defeating Dead-Code Elimination

The optimizer removes computations whose results are never observed. To get a true measurement you must *force the result to be observed* and *prevent the input from being treated as constant*.

```cpp
// Min standard: C++11. Google Benchmark provides DoNotOptimize / ClobberMemory.
#include <benchmark/benchmark.h>
static void BM_hash(benchmark::State& state) {
    std::string input = make_input(state.range(0));   // runtime value: not constant-foldable
    for (auto _ : state) {
        auto h = compute_hash(input);
        benchmark::DoNotOptimize(h);                  // "this result is used" — blocks DCE
        benchmark::ClobberMemory();                   // "memory changed" — blocks store elision
    }
}
BENCHMARK(BM_hash)->Arg(64)->Arg(4096);
```
*Listing 103.1 — `DoNotOptimize` forces the result to be considered observed; `ClobberMemory` defeats store elision. (Google Benchmark.)*

> **Why this matters.** `benchmark::DoNotOptimize(x)` is a portable-ish way to tell the compiler "this value escapes," so the optimizer must actually compute it; `ClobberMemory` simulates an external memory read so stores aren't elided. Without them, `compute_hash(input)` with the result discarded compiles to *nothing*. The companion rule is to make inputs *runtime* values (read from `state.range`, a file, or a non-`constexpr` source) so the compiler cannot constant-fold the whole computation. The verification, as always, is to read the disassembly (Chapter 89) and confirm the work is present in the loop body — a benchmark you have not disassembled is a benchmark you should not trust.

---

## 103.3 Warmup and Steady State

The first iterations of any benchmark are unrepresentative: caches are cold, the TLB is empty, branch predictors are untrained, the CPU may be in a low-frequency state, and code may not yet be in the instruction cache. **Warmup** runs the code enough times to reach steady state *before* measurement begins.

> **Why this matters / cost model.** A benchmark that includes cold-start costs measures a transient that production (running continuously) never experiences — or, conversely, a latency-critical system whose *real* concern is exactly that cold first request needs the cold number measured separately, not averaged in. Frameworks like Google Benchmark auto-detect steady state (running until timings stabilise) and discard warmup. CPU frequency scaling is a particular trap: a benchmark that starts at a low clock and ramps up reports a misleadingly slow average; pin the frequency (`cpupower frequency-set`) or warm until the clock saturates. Steady-state measurement answers "how fast is the hot path once warm," which is the right question for throughput; the cold path is a *separate* measurement (and the subject of warmup discipline in Chapter 106).

---

## 103.4 Statistical Rigour: Noise, Runs, and Significance

A single run is a sample, not a measurement. Real systems have noise — interrupts, scheduling, other processes, thermal effects — so a benchmark must be run *many times* and characterised statistically: report a robust central estimate (median, not mean — the mean is skewed by outliers), the spread (standard deviation or IQR), and whether two results differ by more than the noise.

> **Why this matters.** Declaring "version B is 3% faster" from one run each is noise, not a finding, when run-to-run variance is 5%. The discipline: multiple repetitions (`--benchmark_repetitions=N` in Google Benchmark, which reports mean/median/stddev), a quiet machine (no background load, fixed frequency, pinned thread), and a difference large enough to exceed the measured variance. For A/B comparisons, run them *interleaved* (not all-A-then-all-B) so slow drift (thermal throttling) doesn't bias one. Tools like `perf stat -r N` give run-to-run variance directly. A performance claim without a variance is not yet a claim.

---

## 103.5 Tail Latency: Why the Mean Is Useless

For the systems this volume targets, the **mean** latency is not just insufficient — it is actively misleading. Latency distributions are **right-skewed and multi-modal**: most requests are fast, but a tail of requests are far slower (a cache miss, a page fault, a lock convoy, a GC pause). The mean is dragged around by that tail and conceals it; what matters is the **percentiles**.

```cpp
// Min standard: C++11. Record EVERY latency, then report percentiles (use HdrHistogram in production).
#include <vector>
#include <algorithm>
std::vector<uint64_t> samples;                  // one entry per operation (pre-reserved!)
// ... record samples[i] = measured_ns for each op ...
std::sort(samples.begin(), samples.end());
auto pct = [&](double p){ return samples[size_t(p/100.0 * (samples.size()-1))]; };
// Report: p50=pct(50)  p99=pct(99)  p99.9=pct(99.9)  max=samples.back()
```
*Listing 103.2 — Report the distribution, not the mean. Production uses HdrHistogram for memory-efficient full-range percentiles.*

> **Why this matters.** A trading system, a real-time pipeline, or an SLA-bound service is judged by its **p99 / p99.9 / max**, because those are the requests that lose money, miss deadlines, or breach the contract. A system with a 1 μs mean and a 10 ms p99.9 is, for these domains, *broken* — and the mean hides exactly that. Every villain in this volume — cache miss (87), page fault (88), lock convoy (95), allocation slow path (79), thread migration (96), syscall (98) — is a *tail* event: rare, so invisible in the mean, but precisely what determines the p99.9. This is why the volume's determinism theme (Chapter 85) is measured in tail percentiles: optimising the tail *is* optimising for these systems. Use **HdrHistogram** (constant memory, full dynamic range, no precision loss) rather than naive sorted vectors at production scale, and always report p50/p99/p99.9/max, never just the average.

---

## 103.6 Coordinated Omission

A subtle and pervasive measurement error: **coordinated omission**. A load generator that sends a request, *waits for the response*, then sends the next, **omits** the latency of requests that *would have been sent* during a stall. If the system freezes for 100 ms, a closed-loop tester records *one* slow request — but in production, at a fixed arrival rate, *thousands* of requests would have queued behind that stall, each suffering escalating latency.

> **Why this matters.** Coordinated omission makes tail latency look *far* better than reality — often by orders of magnitude at the high percentiles — because the measurement tool stops the clock during exactly the stalls that matter. The fix is to measure against a *fixed schedule* (the request *should* have been sent at time T regardless of whether the previous one finished) and record latency from the *intended* send time, not the actual one. Tools that do this correctly (wrk2, properly-configured HdrHistogram-based harnesses) report the brutal truth; naive closed-loop benchmarks (and many built-in ones) systematically under-report the tail. A p99.9 measured with coordinated omission is one of the most common ways production latency surprises a team that "measured" it.

---

## 103.7 The Profiling Workflow

Microbenchmarks measure a *known* hot spot; **profiling** finds *which* code is hot in a whole program. The workflow:

- **Sampling profilers** (`perf record`/`perf report`, VTune, Instruments) interrupt periodically to sample the call stack — low overhead, finds hot functions and lines, the right first tool.
- **Hardware counters** (`perf stat`) report cache misses, branch mispredicts, IPC, and stalls — telling you *why* a hot spot is slow (the cost model of Chapter 85: compute-, memory-, or boundary-bound).
- **`perf c2c`** finds false sharing (Chapter 87); **`strace -c`** counts syscalls (Chapter 98); **heaptrack** counts allocations (Chapter 97).

> **Why this matters.** Profiling tells you *where* and *why* before you write a benchmark for the *what*. The hardware-counter step is what assigns a hot loop to a cost model: high cache-miss rate → memory-bound (fix layout, Chapter 87/90); high branch-miss rate → fix branches (Chapter 91); high IPC but slow → compute-bound (vectorise, Chapter 92); low IPC with few misses → likely dependency-chain-bound (Chapter 86) or syscall-bound (Chapter 98). Profiling *first*, then microbenchmarking the identified hot spot, then verifying in the disassembly, is the loop that keeps optimization effort aimed at what actually matters — the antidote to the premature optimization of Chapter 80.

---

## 103.8 The Discipline

| Pitfall | Symptom | Defence |
|---|---|---|
| Dead-code elimination | Implausibly fast; empty loop in asm | `DoNotOptimize`/`ClobberMemory`; read disasm |
| Constant folding | Loop optimised away | Runtime (non-`constexpr`) inputs |
| Cold start | High variance, slow early iters | Warmup to steady state |
| Single run | "3% faster" that's noise | Repetitions + variance; interleave A/B |
| Reporting the mean | Tail hidden | p50/p99/p99.9/max; HdrHistogram |
| Coordinated omission | Tail too good to be true | Fixed schedule; intended send time |
| Wrong target | Optimising cold code | Profile first; hardware counters |

> **The discipline.** A benchmark is only as good as its defences against self-deception: force the work to be observed, feed it runtime inputs, warm to steady state, run it enough times to beat the noise, and — above all for these systems — report the *full distribution and its tail*, measured without coordinated omission. Pair microbenchmarks with whole-program profiling so you optimise the code that is actually hot, and verify every result in the disassembly. This is the measurement foundation the entire volume rests on: every "this is faster" claim in the preceding chapters is only meaningful if measured this way. The final two chapters address the correctness counterpart — undefined behaviour and the tools to catch it — and then the synthesis: engineering for determinism.
