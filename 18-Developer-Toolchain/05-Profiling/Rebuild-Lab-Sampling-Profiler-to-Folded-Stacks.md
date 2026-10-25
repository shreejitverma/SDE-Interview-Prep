---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rebuild Lab: Sampling Profiler to Folded Stacks

> [!summary] Scope
> This playbook guides the complete implementation of a minimal working sampling profiler in C++ or Rust.
> It fulfills the Rebuild depth rung by stepping through process stack sampling via perf_event_open or timer signals, resolving ELF/Mach-O symbol tables, and emitting folded stack output for flame graphs.
> It transforms the sampling concepts from Pillar 04 into functional software.

## Related

- Prerequisites: [[Linux-perf-in-Depth-and-perf-c2c]]
- Next: [[Playbook-Diagnose-a-Production-Tail-Latency-Spike-with-eBPF-and-perf]]
- Lab: `code/05-Profiling/mini_profiler/`
