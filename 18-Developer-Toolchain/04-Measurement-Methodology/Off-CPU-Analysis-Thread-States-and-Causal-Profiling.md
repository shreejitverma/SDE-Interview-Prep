---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Off-CPU Analysis, Thread States, and Causal Profiling

> [!summary] Scope
> This note owns the diagnosis of non-running thread bottlenecks, including lock contention, I/O blocking, and scheduler sleep.
> It covers the Use, Explain, and Decide rungs by tracing kernel sched_switch events and calculating potential speedups via Curtsinger and Berger's Coz causal profiling.
> It explains why standard on-CPU sampling profilers miss blocked time in latency-critical concurrent software.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]]
- Next: [[Flame-Graphs-Differential-Flame-Graphs-and-Visualizations]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon|Brendan Gregg Performance Canon]]
