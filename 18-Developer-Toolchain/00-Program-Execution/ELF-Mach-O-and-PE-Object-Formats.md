---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# ELF, Mach-O, and PE Object Formats

> [!summary] Scope
> Owns binary container architectures across operating systems: System V ELF (headers, sections, program segments, PT_LOAD, .note), Apple Mach-O (fat universal headers, load commands, __TEXT, __DATA, __LINKEDIT), and PE/COFF in brief.
> Details header invariants, section-to-segment mapping, and alignment requirements.
> Reaches Use through Rebuild, handing symbol table relocation parsing to Symbols-Relocations-and-Static-Linking.md and dynamic loader runtime execution to Dynamic-Linking-PLT-GOT-and-Loaders.md.

## Related

- Prerequisites: [[Program-Startup-C-Runtime-Init-and-Auxiliary-Vectors]]
- Next: [[Symbols-Relocations-and-Static-Linking]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]], [[15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng|Binary Exploitation and Reverse Eng]]
- Lab: `code/00-Program-Execution/elf_section_parser/` (requires: linux)
