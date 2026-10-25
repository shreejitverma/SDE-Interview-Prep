---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Stack Unwinding, CFI, and Exception ABI

> [!summary] Scope
> Owns call stack traversal mechanisms, contrasting frame pointer chaining (-fno-omit-frame-pointer) with DWARF Call Frame Information (CFI) in .eh_frame and .eh_frame_hdr, alongside macOS __compact_unwind.
> Details the Itanium two-phase exception handling process (_Unwind_RaiseException, personality routines, Language Specific Data Areas), asynchronous unwinding tables, and zero-cost exception costs.
> Reaches Explain through Decide at scale, handing debugger backtracing to DWARF-Debug-Information-and-Symbol-Bundles.md and 03-Debugging tools.

## Related

- Prerequisites: [[Calling-Conventions-System-V-AMD64-and-AAPCS64]], [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]]
- Next: [[DWARF-Debug-Information-and-Symbol-Bundles]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
- Lab: `code/00-Program-Execution/cfi_backtrace/` (requires: linux)
