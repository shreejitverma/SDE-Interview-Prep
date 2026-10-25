---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Thread-Local Storage Models and ELF Relocations

> [!summary] Scope
> Owns compiler and linker Thread-Local Storage (TLS) mechanics across General Dynamic, Local Dynamic, Initial Exec, and Local Exec access models, as well as TLSDESC optimizations.
> Explains thread pointer registers (FS/GS on x86-64, TPIDR_EL0 on AArch64), .tdata and .tbss initialization, and dlopen failures caused by static TLS surplus exhaustion in glibc.
> Reaches Explain through Debug, handing general relocation mechanics to Symbols-Relocations-and-Static-Linking.md.

## Related

- Prerequisites: [[Symbols-Relocations-and-Static-Linking]], [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- Next: [[Hardware-Security-ABIs-PAC-BTI-CET-and-RELRO]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/tls_models/` (requires: linux)
