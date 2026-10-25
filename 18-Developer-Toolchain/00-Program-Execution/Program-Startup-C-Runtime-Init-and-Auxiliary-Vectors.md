---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Program Startup, C Runtime Init, and Auxiliary Vectors

> [!summary] Scope
> Owns the execution sequence from kernel execve return to main invocation.
> Details the initial stack layout containing argc, argv, envp, and auxiliary vector (auxv) key-value pairs (AT_ENTRY, AT_PHDR, AT_SYSINFO_EHDR for vDSO).
> Traces _start through __libc_start_main, static TLS reservation, dynamic linker bootstrap, and .init_array versus .fini_array constructor and destructor execution order.
> Reaches Explain through Debug, handing linker symbol layout to Symbols-Relocations-and-Static-Linking.md.

## Related

- Prerequisites: [[Virtual-Memory-Mmap-and-Process-Address-Space]]
- Next: [[ELF-Mach-O-and-PE-Object-Formats]]
- See also: [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Program-Startup-argv-envp-and-ARG_MAX|Program Startup argv envp and ARG_MAX]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Environment-Variables-and-Inheritance|Environment Variables and Inheritance]]
- Lab: `code/00-Program-Execution/auxv_init_array/` (requires: linux)
