---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Memory Profiling: heaptrack, Massif, DHAT, and Allocators

> [!summary] Scope
> This note owns heap memory profiling and allocation optimization in C++ applications.
> It covers the Use, Explain, and Debug rungs by examining heaptrack, Valgrind Massif, DHAT block lifetimes, and built-in telemetry in jemalloc, tcmalloc, and mimalloc.
> It diagnoses temporary allocation churn, heap fragmentation, and memory bloat that degrade runtime throughput.

## Related

- Prerequisites: none
- Next: [[Cpp-Google-Benchmark-and-nanobench]]
- See also: [[12-Performance-Engineering/02-Profiling/profiling_guide|profiling_guide]]
- Lab: `code/05-Profiling/heaptrack_profiling/` (requires: linux, docker)
