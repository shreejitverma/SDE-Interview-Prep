---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Performance Regression Detection in CI

> [!summary] Scope
> This note owns automated detection of performance regressions within continuous integration pipelines.
> It covers the Explain and Decide rungs by contrasting noisy wall-clock execution against deterministic hardware instruction counts and cache simulations.
> It evaluates non-parametric statistical hypothesis testing, A/B interleaved runners, and change-point detection algorithms.

## Related

- Prerequisites: [[Statistics-for-Benchmarks-Coordinated-Omission-and-HDR-Histograms]], [[Benchmark-Methodology-Noise-and-Environment-Control]]
- Next: [[Measurement-Methodology-Decision-Records]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Latency Regression Testing in Continuous Integration|Latency Regression Testing in Continuous Integration]]
