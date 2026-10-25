---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Playbook: Deadlocks and Livelocks

> [!summary] Scope
> This playbook provides an emergency runbook for triaging hung multi-threaded C++ applications suffering from deadlocks or livelocks.
> It guides thread stack extraction, reconstruction of resource allocation wait-for graphs from pthread mutex internals, and lock-order cycle detection.
> It provides immediate operational mitigation steps alongside long-term architectural refactorings.

## Related

- Prerequisites: [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]], [[Cpp-GDB-and-LLDB-Expert-Guide]]
