---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python cProfile, Tachyon, and Sampling Profilers

> [!summary] Scope
> This note owns CPU profiling methodologies in Python, contrasting deterministic call tracing with statistical sampling.
> It reaches the Use, Explain, and Debug rungs by analyzing cProfile overhead, Python 3.15's standard profiling.sampling (Tachyon) module, py-spy, pyinstrument, Scalene, and Austin.
> It diagnoses async task execution delays and GIL contention without distorting interpreter bytecode execution.

## Related

- Prerequisites: none
- Next: [[Python-CPython-perf-Trampoline-and-Tachyon-Internals]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]], [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS|Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS]]
- Lab: `code/05-Profiling/tachyon_sampling/`
