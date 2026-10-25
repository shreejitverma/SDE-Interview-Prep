---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Coordinated Omission in Load Generators

> [!summary] Scope
> This problem note presents a scenario where a naive closed-loop load generator fails to detect a 100x p99.9 latency spike during service pauses.
> It tasks the engineer with exposing the coordinated omission distortion, re-implementing open-loop arrival schedules, and computing corrected percentile distributions using HdrHistogram.
> It provides practical mastery over latency measurement pitfalls in distributed and real-time systems.

## Related

- Prerequisites: [[Statistics-for-Benchmarks-Coordinated-Omission-and-HDR-Histograms]]
- See also: [[14-Low-Latency-Systems/07 - Time & Measurement/Coordinated Omission in Low Latency Systems|Coordinated Omission in Low Latency Systems]]
- Lab: `code/04-Measurement-Methodology/drill_coord_omission/`
