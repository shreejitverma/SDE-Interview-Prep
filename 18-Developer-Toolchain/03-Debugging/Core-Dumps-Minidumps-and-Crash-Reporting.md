---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Core Dumps, Minidumps, and Crash Reporting

> [!summary] Scope
> This note owns operating system crash capture mechanisms, including Linux core_pattern, systemd-coredump, macOS crash reports, and out-of-process Crashpad minidumps.
> It reaches the Use, Explain, and Decide depth rungs by analyzing ELF PT_NOTE segments, memory dump filtering, and fleet-wide stack deduplication.
> It establishes the input requirements for Debug-a-Production-Crash-From-a-Core-Dump and symbol lookup in Symbol-Servers-and-debuginfod.

## Related

- Prerequisites: none
- Next: [[Symbol-Servers-and-debuginfod]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
