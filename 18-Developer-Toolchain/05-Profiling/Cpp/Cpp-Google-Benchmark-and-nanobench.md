---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Google Benchmark and nanobench

> [!summary] Scope
> This note owns microbenchmarking practice in C++ using Google Benchmark and nanobench.
> It reaches the Use, Explain, and Rebuild rungs by implementing optimizer-proof test harnesses using DoNotOptimize and ClobberMemory, measuring hardware PMU counters, and performing asymptotic complexity curve fitting.
> It helps ensure microbenchmark results reflect genuine algorithmic performance rather than compiler dead-code elimination.

## Related

- Prerequisites: none
- Next: [[Cpp-Compile-Time-Profiling-ftime-trace-and-ClangBuildAnalyzer]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency|Chapter_103_Microbenchmarking_and_Tail_Latency]]
- Lab: `code/05-Profiling/google_benchmark_tuning/`
