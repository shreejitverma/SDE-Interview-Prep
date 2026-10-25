---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Capstone: Linux ptrace Debugger with DWARF Line Lookup

> [!summary] Scope
> This capstone playbook guides the implementation of a native Linux x86-64 and AArch64 process debugger using the ptrace system call and libelf/gimli DWARF parsing.
> It implements process attachment, software breakpoints via int3 opcode patching, register inspection via PTRACE_GETREGSET, single-stepping, and memory peeking/poking.
> It details DWARF .debug_line state machine evaluation and .debug_info DIE navigation to map instruction pointers to source code lines and symbol names.

## Related

- Prerequisites: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]], [[DWARF-Debug-Information-and-Symbol-Bundles]], [[Calling-Conventions-System-V-AMD64-and-AAPCS64]], [[Rebuild-Lab-ptrace-Debugger-with-DWARF-Backtrace]]
- Next: [[Capstone-Sampling-Profiler-Folded-Stacks]]
- See also: [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management|P2L1 Processes and Process Management]]
- Lab: `code/11-Arena-and-Capstones/capstone_ptrace_debugger/` (requires: linux, docker)
