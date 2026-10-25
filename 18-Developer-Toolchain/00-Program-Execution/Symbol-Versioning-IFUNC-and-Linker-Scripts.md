---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Symbol Versioning, IFUNC, and Linker Scripts

> [!summary] Scope
> Owns GNU symbol versioning (.gnu.version, .gnu.version_r, .gnu.version_d) and runtime dynamic dispatch via STT_GNU_IFUNC resolvers.
> Details IFUNC resolver constraints (invoked prior to full dynamic relocation), linker script architecture (SECTIONS, PHDRS, MEMORY, VERSION commands), --gc-sections dead code elimination, and Identical Code Folding (ICF).
> Reaches Use through Decide at scale, handing dynamic loader failure resolution to Diagnose-Dynamic-Linker-and-ABI-Failures.md.

## Related

- Prerequisites: [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- Next: [[Thread-Local-Storage-Models-and-ELF-Relocations]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/ifunc_resolver/` (requires: linux)
