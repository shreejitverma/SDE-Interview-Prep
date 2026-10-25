---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Linkers, LTO, PGO, AutoFDO, and BOLT

> [!summary] Scope
> Owns modern linkers (GNU ld/bfd, gold, lld, mold, Apple ld-prime) and post-compilation code layout optimization.
> Contrasts monolithic full LTO with distributed ThinLTO, and compares compiler instrumentation PGO with sampling-based AutoFDO and post-link binary optimization via BOLT.
> Reaches Use through Decide at scale, handing build system orchestration of ThinLTO and PGO to 06-Build-Systems and basic relocation to Symbols-Relocations-and-Static-Linking.md.

## Related

- Prerequisites: [[Symbols-Relocations-and-Static-Linking]], [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- Next: [[macOS-vs-Linux-Toolchain-Mechanics]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/thinlto_bolt_pgo/` (requires: linux)
