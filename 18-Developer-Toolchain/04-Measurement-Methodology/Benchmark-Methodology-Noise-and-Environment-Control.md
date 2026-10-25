---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Benchmark Methodology: Noise and Environment Control

> [!summary] Scope
> This note owns experimental rigor and environmental noise elimination in software benchmarking.
> It reaches the Use, Explain, and Debug rungs by showing how CPU frequency governors, Turbo Boost, SMT/hyperthreading, NUMA topology, ASLR, and memory layout bias corrupt benchmarks.
> It grounds the empirical findings of Mytkowicz et al. and establishes host preparation protocols for low-noise measurement.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]]
- Next: [[Statistics-for-Benchmarks-Coordinated-Omission-and-HDR-Histograms]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency|Chapter_103_Microbenchmarking_and_Tail_Latency]]
- Lab: `code/04-Measurement-Methodology/env_alignment_bias/` (requires: linux)
