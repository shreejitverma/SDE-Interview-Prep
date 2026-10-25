---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Diagnose a Production Tail Latency Spike With eBPF and perf

> [!summary] Scope
> This playbook provides an end-to-end incident runbook for diagnosing a live production p99 tail latency regression without restarting services.
> It outlines a structured sequence: checking resource saturation via USE, running bpftrace off-CPU and runqlat probes, capturing differential flame graphs, and confirming root-cause contention.
> It translates theoretical tracing tools into actionable production mitigation.

## Related

- Prerequisites: [[Linux-perf-in-Depth-and-perf-c2c]], [[eBPF-bpftrace-ftrace-uprobes-and-USDT]]
- Next: [[Drill-Profiling-False-Sharing-Cache-Contention]]
- See also: [[14-Low-Latency-Systems/11 - Participant-Side Systems/Drill - 11 Tick-to-Trade Pipeline Bottleneck Hunting|Drill   11 Tick to Trade Pipeline Bottleneck Hunting]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/bpftrace-One-Liners|bpftrace One Liners]]
- Lab: `code/05-Profiling/latency_spike_triage/` (requires: linux)
