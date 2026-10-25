---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ PGO, AutoFDO, and BOLT Workflow

> [!summary] Scope
> This note owns feedback-directed binary optimization pipelines, including instrumentation PGO, sampling-based AutoFDO via hardware LBR, and LLVM BOLT post-link layout optimization.
> It covers the Use, Explain, and Decide rungs, evaluating instruction cache miss reductions, basic block reordering, and function splitting.
> It provides a complete end-to-end production workflow and shows how to measure the resulting speedups.

## Related

- Prerequisites: [[Linux-perf-in-Depth-and-perf-c2c]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/05-Profiling/pgo_bolt_workflow/` (requires: linux)
