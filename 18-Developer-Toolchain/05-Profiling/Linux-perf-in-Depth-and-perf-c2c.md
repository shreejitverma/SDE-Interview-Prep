---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Linux perf in Depth and perf c2c

> [!summary] Scope
> This note owns the comprehensive architecture and operational mastery of the Linux perf subsystem.
> It reaches the Use, Explain, and Debug rungs by examining perf_event_open syscall mechanics, ring buffer sampling, DWARF versus FP versus LBR callchains, and false-sharing diagnosis with perf c2c.
> It connects kernel performance events with the low-latency cache contention analysis demonstrated in false_sharing.cpp.

## Related

- Prerequisites: none
- Next: [[eBPF-bpftrace-ftrace-uprobes-and-USDT]]
- See also: [[12-Performance-Engineering/02-Profiling/profiling_guide|profiling_guide]], [[14-Low-Latency-Systems/11 - Participant-Side Systems/Drill - 11 Tick-to-Trade Pipeline Bottleneck Hunting|Drill   11 Tick to Trade Pipeline Bottleneck Hunting]]
- Lab: `code/05-Profiling/perf_c2c_false_sharing/` (requires: linux, pmu)
