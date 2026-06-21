# Chapter 101: Clocks, Timekeeping, and Measuring Latency Correctly

You cannot optimise what you cannot measure, and measuring nanosecond-scale latency correctly is itself a hard systems problem: the clock you pick has its own cost and resolution, the timestamp instruction can be reordered by the CPU, and the measurement overhead can exceed the thing measured. This chapter covers the clock sources C++ exposes, the `rdtsc`/TSC hardware counter that low-latency code times with, and the methodology for measuring latency without lying to yourself — the foundation for the benchmarking and jitter chapters that close the volume.

## Chapter Roadmap

- 101.1 Why Timekeeping Is Hard
- 101.2 The C++ `<chrono>` Clocks
- 101.3 The TSC and `rdtsc`
- 101.4 Measurement Overhead and Reordering
- 101.5 Measuring Latency Correctly
- 101.6 The Discipline

---

## 101.1 Why Timekeeping Is Hard

A "clock" seems simple until you need sub-microsecond accuracy. The complications: there are *multiple* clock sources with different costs and guarantees (the TSC, the HPET, the kernel's software clock); wall-clock time can jump *backwards* (NTP adjustments, leap seconds, manual changes); reading a clock has a *cost* that may dwarf what you are timing; and the CPU may *reorder* the timestamp read relative to the code you meant to bracket.

> **Why this matters.** Choosing the wrong clock or measuring carelessly produces numbers that are confidently wrong — a negative duration (wall clock stepped back), a measurement dominated by the clock's own ~hundreds-of-ns cost, or an interval that does not actually bracket the code because the CPU moved the timestamp. For ordinary logging none of this matters; for latency measurement at the nanosecond scale, every one of these is a real failure mode that has produced wrong conclusions in real systems. Correct timekeeping is a prerequisite for the entire measurement discipline of the volume.

---

## 101.2 The C++ `<chrono>` Clocks

C++ provides three standard clocks with distinct contracts:

| Clock | Monotonic? | Use for |
|---|---|---|
| `std::chrono::system_clock` | **No** — can jump (NTP, manual) | Wall-clock timestamps, dates |
| `std::chrono::steady_clock` | **Yes** — never goes backward | Measuring *intervals*/durations |
| `std::chrono::high_resolution_clock` | Implementation-defined (often an alias) | Avoid — ambiguous guarantees |

```cpp
// Min standard: C++11. Portable. ALWAYS use steady_clock for interval measurement.
#include <chrono>
auto start = std::chrono::steady_clock::now();
do_work();
auto elapsed = std::chrono::steady_clock::now() - start;   // never negative; immune to clock steps
auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
```
*Listing 101.1 — `steady_clock` is the correct choice for measuring elapsed time.*

> **Why this matters.** The single most common timing bug is measuring an interval with `system_clock`: if NTP adjusts the wall clock during the measurement, the duration can be *negative* or wildly wrong. `steady_clock` is *guaranteed monotonic* — it only moves forward at a steady rate — which is exactly what interval measurement requires; use it for every "how long did this take" question and reserve `system_clock` for "what time is it" (timestamps you display or correlate across machines). On Linux, `steady_clock::now()` is backed by `CLOCK_MONOTONIC` via the vDSO (Chapter 98), so it is cheap (~15–30 ns) — but that is still not free at the nanosecond scale.

---

## 101.3 The TSC and `rdtsc`

For the lowest-overhead timing, low-latency code reads the CPU's **Time Stamp Counter (TSC)** directly via the `rdtsc` instruction — a cycle counter that increments at a fixed rate, readable in a few cycles without any kernel involvement.

```cpp
// Min standard: C++11 + x86 (non-portable). Reading the TSC with ordering.
#include <x86intrin.h>
static inline uint64_t read_tsc() {
    unsigned aux;
    return __rdtscp(&aux);   // rdtscp: serializing enough to not float past prior instructions
}
// Or _mm_lfence(); __rdtsc(); _mm_lfence();  to fence an rdtsc explicitly.
// Convert cycles to ns by dividing by the (calibrated) TSC frequency.
```
*Listing 101.2 — Reading the TSC. `rdtsc`/`rdtscp` are x86-specific; ARM has `cntvct_el0`.*

> **Why this matters / cost model.** `rdtsc` is ~the fastest timestamp available (a handful of cycles vs ~15–30 ns for vDSO `clock_gettime`), which matters when you timestamp millions of events. But it carries sharp caveats. **Invariance:** on modern CPUs the TSC is *invariant* — it ticks at a constant rate regardless of CPU frequency scaling or sleep states — but on older CPUs it tracked the (variable) core clock, making it useless for time. **Per-core skew:** TSCs on different cores may not be perfectly synchronised, so a start on one core and end on another can yield garbage — hence the need to pin (Chapter 96). **Calibration:** the TSC counts *cycles*, not nanoseconds, so you must calibrate its frequency once at startup to convert. **Reordering:** plain `rdtsc` can be reordered by the out-of-order core (§101.4). Used carefully on a pinned thread with an invariant TSC, it is the timing primitive of choice for HFT.

---

## 101.4 Measurement Overhead and Reordering

Two effects corrupt naive timing:

- **Overhead:** the timestamp read itself costs time. If you bracket a 5 ns operation with two `clock_gettime` calls at ~20 ns each, you measure ~45 ns of mostly *measurement*. The fix: measure a *large batch* (time N iterations, divide) so the fixed overhead amortises, or subtract a measured baseline (time two back-to-back reads).
- **Reordering:** the out-of-order CPU (Chapter 86) may execute the timestamp read *before* or *after* the code you meant to bracket, because there is no data dependency forcing the order. A plain `rdtsc` can float outside the region. The fix: a *serializing* form (`rdtscp` plus `lfence`, or a compiler/CPU barrier) that prevents the timestamp from being reordered past the measured code.

```cpp
// Min standard: C++11. Compiler barrier to prevent the optimizer reordering/eliding timed code.
static inline void compiler_barrier() { asm volatile("" ::: "memory"); }  // GCC/Clang, non-portable
// Pattern: barrier(); t0 = read_clock(); barrier(); work(); barrier(); t1 = read_clock(); barrier();
```
*Listing 101.3 — A compiler barrier stops the optimizer from moving or deleting the timed region. Non-portable inline asm.*

> **Why this matters.** These are the two ways a microbenchmark silently lies. Un-amortised overhead makes a fast operation look slow (you measured the clock, not the code). Reordering makes the interval not correspond to the code (the CPU timestamped the wrong boundary). And a third, covered fully in Chapter 103: the optimizer may *delete* the timed code entirely if its result is unused (the as-if rule, Chapter 89). The defences — batch-and-amortise, fence the timestamps, and force the result to be observed (`benchmark::DoNotOptimize`) — are what separate a real measurement from a fictional one.

---

## 101.5 Measuring Latency Correctly

The methodology for trustworthy latency numbers:

1. **Use `steady_clock` (or a calibrated, pinned TSC)** — never `system_clock` — for intervals.
2. **Pin the thread** (Chapter 96) so the TSC is consistent and there are no migration artefacts.
3. **Warm up** (Chapter 106) — run the code first to prime caches, TLB, and branch predictors, then measure; the first iteration is unrepresentative.
4. **Amortise overhead** — time many iterations and divide, or subtract a baseline.
5. **Fence** the timestamps so the region is correctly bracketed.
6. **Record the full distribution, not the mean** — latency is not normally distributed; report percentiles (p50, p99, p99.9, max), because the *tail* is what the volume's systems care about (Chapter 103 develops this).

> **Why this matters.** Each step removes a specific lie. Skipping warmup measures cold-cache penalties as if they were steady-state. Reporting the mean hides the tail — a system with a great average and a terrible p99.9 is, for trading or real-time, a bad system, and the mean conceals exactly that. Pinning and fencing ensure the number corresponds to the code on a stable core. The output of correct measurement is a *distribution* — the shape that reveals whether your hot path is deterministic (tight distribution) or jittery (long tail), which is the entire question the volume's determinism discipline tries to answer.

---

## 101.6 The Discipline

| Concern | Wrong way | Right way |
|---|---|---|
| Interval clock | `system_clock` (can step back) | `steady_clock` / calibrated TSC |
| Lowest overhead | repeated `clock_gettime` | pinned, invariant `rdtscp` |
| Tiny operation | bracket each call | batch N, divide; subtract baseline |
| Reordering | plain `rdtsc` | `rdtscp`/`lfence` + compiler barrier |
| Reporting | the mean | full distribution, p50/p99/p99.9/max |
| Cross-core timing | start/end on different cores | pin the thread |

> **The discipline.** Timekeeping is the instrument of the whole volume, and a miscalibrated instrument produces confidently wrong conclusions. Use `steady_clock` for intervals and a pinned, calibrated, invariant TSC when you need the lowest overhead; warm up, amortise, and fence so the number reflects the code and not the measurement; and always report the *distribution* and its tail, never the mean. With time measured correctly, the next chapter ensures the *build* doesn't undermine your code's performance, and Chapter 103 turns these clock fundamentals into a rigorous benchmarking practice.
