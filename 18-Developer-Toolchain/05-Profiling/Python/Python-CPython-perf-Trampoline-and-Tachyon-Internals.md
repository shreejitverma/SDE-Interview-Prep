---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Python CPython perf Trampoline and Tachyon Internals

> [!summary] Scope
> This note owns the runtime mechanisms that expose CPython interpreter execution to external native profilers.
> It covers the Explain and Rebuild rungs by examining Python 3.12 -X perf trampoline maps, Python 3.13+ -X perf_jit DWARF JIT registration, and Tachyon's out-of-process _PyInterpreterFrame walking.
> It solves the symbol invisibility problem when profiling mixed Python and native C/C++ runtimes.

## Related

- Prerequisites: [[Python-cProfile-Tachyon-and-Sampling-Profilers]]
- Next: [[Python-Memory-Profiling-Memray-and-Line-Profiler]]
