---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rebuild Lab: ptrace Debugger with DWARF Backtrace

> [!summary] Scope
> This playbook guides the complete implementation of a minimal working debugger in C++ or Rust using the Linux ptrace API.
> It fulfills the Rebuild rung by walking through process attachment, software breakpoint injection via INT3, single-stepping, register inspection, and DWARF call frame unwinding.
> It implements the theoretical mechanisms taught in How-Debuggers-Work-ptrace-Mach-and-Breakpoints.

## Related

- Prerequisites: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- Next: [[Drill-Debugging-Use-After-Free-Core-Dump]]
- See also: [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management|P2L1 Processes and Process Management]]
- Lab: `code/03-Debugging/ptrace_debugger/` (requires: linux, docker)
