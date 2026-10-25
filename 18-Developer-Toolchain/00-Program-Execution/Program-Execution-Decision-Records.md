---
type: playbook
track: [distinguished, low-latency, sde]
level:
status: seed
last_reviewed:
sources: []
---

# Program Execution Decision Records

> [!summary] Scope
> Owns architectural decision records governing program execution choices across large-scale engineering organizations.
> Evaluates trade-offs between static versus dynamic linking, frame pointer retention versus CFI/eBPF unwinding overhead, ThinLTO versus monolithic LTO, and PGO versus AutoFDO versus post-link BOLT.
> Reaches Decide at scale, synthesizing mechanisms established across the foundational concept notes.

## Related

- Prerequisites: [[Linkers-LTO-PGO-AutoFDO-and-BOLT]], [[Stack-Unwinding-CFI-and-Exception-ABI]], [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- Next: [[Diagnose-Dynamic-Linker-and-ABI-Failures]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
