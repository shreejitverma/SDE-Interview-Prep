---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Profiling False Sharing and Cache Contention

> [!summary] Scope
> This problem note provides a timed, guided-to-blind drill for diagnosing a severe multi-core throughput collapse caused by false sharing on adjacent cache lines.
> It challenges the engineer to measure Hit-Modified (HITM) events using perf c2c, locate the contention in source code, and apply hardware cache-line alignment to eliminate bouncing.
> It validates mastery over concurrent memory access and cache coherency protocols.

## Related

- Prerequisites: [[Linux-perf-in-Depth-and-perf-c2c]]
- Next: [[Drill-Profiling-Python-C-Extension-GIL-and-Allocation-Bottleneck]]
- Lab: `code/05-Profiling/drill_false_sharing/` (requires: linux, pmu)
