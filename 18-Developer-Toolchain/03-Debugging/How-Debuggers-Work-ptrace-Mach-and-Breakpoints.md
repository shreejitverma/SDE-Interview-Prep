---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# How Debuggers Work: ptrace, Mach Exception Ports, and Breakpoints

> [!summary] Scope
> This note owns the operating system primitives behind modern debuggers, including Linux ptrace, Mach exception ports on macOS, software INT3 trap patching, and hardware debug registers.
> It covers the Use, Explain, and Rebuild rungs by showing how single-stepping and memory patching operate at the kernel-user boundary and how the debugger evaluates DWARF expressions in user space on top of them.
> It provides the architectural foundation for Rebuild-Lab-ptrace-Debugger-with-DWARF-Backtrace while leaving remote stubs to Remote-Debugging-gdbserver-lldb-server-and-RSP.

## Related

- Prerequisites: none
- Next: [[Remote-Debugging-gdbserver-lldb-server-and-RSP]]
- See also: [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management|P2L1 Processes and Process Management]], [[19-Shell-and-Unix-Engineering/12-Linux-System-Interfaces/System-Calls-and-strace|System Calls and strace]]
