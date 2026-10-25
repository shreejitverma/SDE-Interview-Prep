---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Hardware Security ABIs: PAC, BTI, CET, and RELRO

> [!summary] Scope
> Owns binary hardening mechanisms and architecture-specific hardware security ABIs.
> Covers Read-Only Relocations (partial and full PT_GNU_RELRO), non-executable stacks (PT_GNU_STACK), stack canaries, ARM Pointer Authentication (PAC), Branch Target Identification (BTI), Intel Control-flow Enforcement Technology (CET Shadow Stack and IBT), and Apple chained fixups.
> Reaches Use through Decide at scale, handing general relocation mechanics to Symbols-Relocations-and-Static-Linking.md.

## Related

- Prerequisites: [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- Next: [[Calling-Conventions-System-V-AMD64-and-AAPCS64]]
- See also: [[15-Technical-Whitepapers/05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng|Binary Exploitation and Reverse Eng]]
- Lab: `code/00-Program-Execution/relro_stack_canary/` (requires: linux)
