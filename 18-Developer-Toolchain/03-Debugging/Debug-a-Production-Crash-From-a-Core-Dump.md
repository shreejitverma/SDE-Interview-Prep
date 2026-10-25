---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Debug a Production Crash From a Core Dump

> [!summary] Scope
> This playbook provides a systematic runbook for triaging a stripped release production crash using a Linux core dump or macOS crash report.
> It covers step-by-step procedures: extracting Build IDs, resolving symbols with debuginfod or dSYM bundles, examining registers and stack frames, and identifying corrupt memory.
> It serves as an exemplar playbook bridging Core-Dumps-Minidumps-and-Crash-Reporting to practical problem remediation.

## Related

- Prerequisites: [[Core-Dumps-Minidumps-and-Crash-Reporting]], [[Symbol-Servers-and-debuginfod]]
- Next: [[Rebuild-Lab-ptrace-Debugger-with-DWARF-Backtrace]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
- Lab: `code/03-Debugging/core_dump_triage/` (requires: linux)
