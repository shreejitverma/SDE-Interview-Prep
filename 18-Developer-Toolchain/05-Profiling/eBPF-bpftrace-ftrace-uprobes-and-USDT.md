---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# eBPF, bpftrace, ftrace, uprobes, and USDT

> [!summary] Scope
> This note owns dynamic and static kernel tracing using Linux ftrace, eBPF virtual machines, and bpftrace scripts.
> It covers the Use, Explain, and Debug rungs, detailing BPF verifier constraints, BTF CO-RE portability, kprobes/fentry overhead, uprobe trap penalties, and USDT static marker placement.
> It provides production one-liners and scripts for low-overhead production latency inspection.

## Related

- Prerequisites: none
- Next: [[macOS-Instruments-xctrace-and-DTrace]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation|Linux Tracing and Instrumentation]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/bpftrace-One-Liners|bpftrace One Liners]]
- Lab: `code/05-Profiling/bpftrace_usdt/` (requires: linux, docker)
