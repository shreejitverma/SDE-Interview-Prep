---
type: paper
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Paper: Engineering Record and Replay for Deployability (O'Callahan et al. 2017)

> [!summary] Scope
> Owns the USENIX ATC 2017 systems paper on deterministic record and replay debugging by Robert O'Callahan et al.
> Details how rr achieves low recording overhead on commodity Linux hardware through single-core thread scheduling, hardware performance counters for retired conditional branches, and seccomp-bpf syscall buffering.
> Analyzes copy-on-write memory checkpointing and reverse execution mechanics that make deterministic debugging practical on stock Linux.

## Related

- Prerequisites: none
- Next: [[Paper-Lessons-From-Static-Analysis-at-Google]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing|Deterministic Replay and Packet Injection Testing]]
