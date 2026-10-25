---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python Memory Profiling: Memray and Line-Profiler

> [!summary] Scope
> This note owns line-level CPU profiling and native memory allocation tracking in Python applications.
> It reaches the Use and Explain rungs by examining Bloomberg's memray profiler, native C allocator hook interception, temporary allocation churn, and line_profiler statement timing.
> It equips engineers to isolate memory leaks and micro-bottlenecks inside tight Python loops.

## Related

- Prerequisites: [[Python-cProfile-Tachyon-and-Sampling-Profilers]]
- Next: [[Python-Benchmarking-pyperf-asv-and-NumPy-Pandas-Profiling]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]]
- Lab: `code/05-Profiling/memray_tracking/`
