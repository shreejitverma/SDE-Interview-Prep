---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Symbols, Relocations, and Static Linking

> [!summary] Scope
> Owns static symbol resolution, archive (.a) member extraction algorithms, symbol binding rules (local, global, weak) and common symbols, and visibility attributes (STV_DEFAULT, STV_HIDDEN, STV_PROTECTED).
> Covers relocation mechanics, symbol table sections (.symtab vs .dynsym), and relocation calculations for x86-64 (R_X86_64_64, R_X86_64_PC32, R_X86_64_PLT32) and AArch64.
> Reaches Explain through Rebuild, handing dynamic relocation to Dynamic-Linking-PLT-GOT-and-Loaders.md.

## Related

- Prerequisites: [[ELF-Mach-O-and-PE-Object-Formats]]
- Next: [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/symbol_binding_reloc/` (requires: linux)
