---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Stripped Binary Crash and Split DWARF

> [!summary] Scope
> Timed problem drill presenting a stripped production binary crash with only a raw instruction pointer and a detached Split DWARF (.dwp) package file.
> Challenges the engineer to locate the crash point, reconstruct the stack frame using .eh_frame unwinding, resolve inlined frames via DWARF debug line tables, and configure a local debuginfod daemon.
> Reaches Debug, testing mastery of Stack-Unwinding-CFI-and-Exception-ABI.md and DWARF-Debug-Information-and-Symbol-Bundles.md.

## Related

- Prerequisites: [[Stack-Unwinding-CFI-and-Exception-ABI]], [[DWARF-Debug-Information-and-Symbol-Bundles]]
- Next: [[Drill-Program-Execution-Cross-DSO-Exception-and-RTTI-Failure]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
- Lab: `code/00-Program-Execution/drill_split_dwarf/` (requires: linux)
