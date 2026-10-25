---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ CPU Profiling: perf, samply, Callgrind, and llvm-mca

> [!summary] Scope
> This note owns C++ CPU performance profiling workflows across Linux and macOS.
> It reaches the Use, Explain, and Debug rungs by deploying perf, samply with Firefox Profiler UI, Callgrind cache simulation, and llvm-mca basic-block pipeline modeling.
> It enables engineers to correlate high-level source bottlenecks with assembly-level execution port stalls.

## Related

- Prerequisites: [[Linux-perf-in-Depth-and-perf-c2c]]
- Next: [[Cpp-Memory-Profiling-heaptrack-Massif-DHAT-and-Allocators]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency|Chapter_103_Microbenchmarking_and_Tail_Latency]]
- Lab: `code/05-Profiling/samply_cpu_profile/`
