---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Diagnose Dynamic Linker and ABI Failures

> [!summary] Scope
> Owns the operational runbook for diagnosing dynamic loading errors, missing symbol faults, GLIBC/GLIBCXX version mismatches, and broken rpath or runpath configurations.
> Details step-by-step diagnostic workflows utilizing LD_DEBUG, DYLD_PRINT_TO_STDERR, readelf, objdump, and nm to isolate symbol resolution and ABI versioning conflicts.
> Reaches Debug, serving as the practical troubleshooting companion to Dynamic-Linking-PLT-GOT-and-Loaders.md.

## Related

- Prerequisites: [[Dynamic-Linking-PLT-GOT-and-Loaders]], [[Symbol-Versioning-IFUNC-and-Linker-Scripts]]
- Next: [[Drill-Program-Execution-Broken-Rpath-and-Symbol-Interposition]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/ld_debug_diagnosis/` (requires: linux)
