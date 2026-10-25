---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Remote Debugging: gdbserver, lldb-server, and RSP

> [!summary] Scope
> This note owns the architecture and packet protocol of remote debugging stubs, focusing on the GDB Remote Serial Protocol (RSP) and lldb-server.
> It covers the Use and Explain depth rungs, detailing packet checksums, run-length encoding, stop reply packets, target XML descriptions, and non-stop mode.
> It interfaces between low-level ptrace primitives in How-Debuggers-Work-ptrace-Mach-and-Breakpoints and language CLI workflows in Cpp-GDB-and-LLDB-Expert-Guide.

## Related

- Prerequisites: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- Next: [[Core-Dumps-Minidumps-and-Crash-Reporting]]
