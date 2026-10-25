---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Statistics for Benchmarks: Coordinated Omission and HDR Histograms

> [!summary] Scope
> This note owns statistical data analysis for benchmark latency and throughput measurements.
> It reaches the Use, Explain, and Rebuild rungs by exploring non-normal distributions, median and median absolute deviation, bootstrap confidence intervals, and Gil Tene's coordinated omission trap.
> It derives HdrHistogram bucket compression internals and contrasts open versus closed workload generators.

## Related

- Prerequisites: [[Benchmark-Methodology-Noise-and-Environment-Control]]
- Next: [[Hardware-Performance-Counters-Top-Down-TMA-and-Roofline]]
- See also: [[14-Low-Latency-Systems/07 - Time & Measurement/Coordinated Omission in Low Latency Systems|Coordinated Omission in Low Latency Systems]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency|Chapter_103_Microbenchmarking_and_Tail_Latency]]
- Lab: `code/04-Measurement-Methodology/coordinated_omission/`
