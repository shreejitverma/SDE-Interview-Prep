---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Hardware Performance Counters, Top-Down TMA, and Roofline

> [!summary] Scope
> This note owns microarchitectural performance evaluation using hardware performance monitoring counters (PMCs).
> It reaches the Use, Explain, and Decide rungs by deriving Ahmad Yasin's Top-Down Microarchitecture Analysis (TMA) hierarchy (Retiring, Bad Speculation, Front-End Bound, Back-End Bound) and the Roofline model.
> It analyzes counter multiplexing error and hybrid P-core/E-core PMU partitioning.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]]
- Next: [[USE-RED-and-Systems-Performance-Methodologies]]
- See also: [[14-Low-Latency-Systems/07 - Time & Measurement/CPU Timestamp Counter RDTSC Mechanics|CPU Timestamp Counter RDTSC Mechanics]]
- Lab: `code/04-Measurement-Methodology/top_down_tma/` (requires: linux, pmu)
