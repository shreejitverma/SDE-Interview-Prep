---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Dynamic Linking, PLT, GOT, and Loaders

> [!summary] Scope
> Owns position-independent code (PIC) execution, Global Offset Table (GOT) data access, Procedure Linkage Table (PLT) jump stubs, and lazy versus immediate binding (LD_BIND_NOW).
> Covers dynamic loader runtime mechanics (glibc ld.so and Apple dyld4), search paths (DT_RPATH, DT_RUNPATH, @rpath, @loader_path), and library interposition via LD_PRELOAD and DYLD_INSERT_LIBRARIES.
> Reaches Use through Debug, handing linker symbol versioning to Symbol-Versioning-IFUNC-and-Linker-Scripts.md and runbook triage to Diagnose-Dynamic-Linker-and-ABI-Failures.md.

## Related

- Prerequisites: [[Symbols-Relocations-and-Static-Linking]]
- Next: [[Symbol-Versioning-IFUNC-and-Linker-Scripts]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/plt_got_interpose/` (requires: linux)
