---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Playbook: Heap Corruption

> [!summary] Scope
> This playbook provides a decision tree and step-by-step procedure for isolating wild-pointer overwrites, heap metadata corruption, and double-frees.
> It guides engineers through triage with ASan redzones, hardware watchpoints, MallocScribble, and core dump memory inspection.
> It turns non-deterministic crashes into isolated, reproducible bug reports.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]], [[Debug-a-Production-Crash-From-a-Core-Dump]]
- Next: [[Cpp-Playbook-Deadlocks-and-Livelocks]]
