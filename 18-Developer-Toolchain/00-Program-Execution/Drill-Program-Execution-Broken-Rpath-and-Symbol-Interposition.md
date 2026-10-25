---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Broken Rpath and Symbol Interposition

> [!summary] Scope
> Guided problem drill providing a broken dynamic plugin architecture exhibiting unexpected DT_RUNPATH transitive lookup failures and unintended global symbol interposition.
> Guides the engineer through reproducing the loader failure, inspecting ELF headers with readelf, identifying symbol preemption bugs, and restructuring rpath and symbol visibility.
> Reaches Debug and Use rungs, grounding Dynamic-Linking-PLT-GOT-and-Loaders.md in a runnable lab.

## Related

- Prerequisites: [[Dynamic-Linking-PLT-GOT-and-Loaders]], [[Diagnose-Dynamic-Linker-and-ABI-Failures]]
- Next: [[Drill-Program-Execution-Stripped-Binary-Crash-and-Split-DWARF]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/drill_broken_rpath/` (requires: linux)
