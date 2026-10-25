---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Capstone: Sampling Profiler Emitting Folded Stacks

> [!summary] Scope
> This capstone playbook walks through constructing a high-performance sampling profiler that captures thread call stacks at high frequency with minimal target perturbation and formats them as folded stacks for FlameGraph visualization.
> It covers timer signal delivery (SIGPROF/timer_create) and perf_event_open sampling, frame pointer stack traversal, and DWARF .eh_frame unwinding fallback.
> It implements an ELF symbolication engine resolving dynamic and static symbol tables (.symtab and .dynsym) with caching.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]], [[Stack-Unwinding-CFI-and-Exception-ABI]], [[Flame-Graphs-Differential-Flame-Graphs-and-Visualizations]], [[Rebuild-Lab-Sampling-Profiler-to-Folded-Stacks]]
- Next: [[Capstone-Polyglot-Packaging-and-Release-Pipeline]]
- See also: [[12-Performance-Engineering/02-Profiling/profiling_guide|profiling_guide]], [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon|Brendan Gregg Performance Canon]]
- Lab: `code/11-Arena-and-Capstones/capstone_sampling_profiler/` (requires: linux)
