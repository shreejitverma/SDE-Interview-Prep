---
type: problem
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Profiling Python-C Extension GIL and Allocation Bottleneck

> [!summary] Scope
> This problem note presents a scenario diagnosing an unexpected 5x throughput slowdown in a high-concurrency Python service using a compiled C extension.
> It tasks the engineer with applying Tachyon sampling and memray tracking to isolate hidden memory allocations and unintended GIL acquisition during hot loops.
> It tests practical proficiency in profiling mixed-language applications under realistic production conditions.

## Related

- Prerequisites: [[Python-cProfile-Tachyon-and-Sampling-Profilers]], [[Python-Memory-Profiling-Memray-and-Line-Profiler]]
- Next: [[Drill-Profiling-Rust-Monomorphization-and-Inlining-Regression]]
- Lab: `code/05-Profiling/drill_python_gil_alloc/`
