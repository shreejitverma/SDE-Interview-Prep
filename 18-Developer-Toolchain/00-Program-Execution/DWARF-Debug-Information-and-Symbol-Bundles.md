---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# DWARF Debug Information and Symbol Bundles

> [!summary] Scope
> Owns the DWARF debugging information format across DWARF 4, ratified DWARF 5, and DWARF 6 working draft proposals.
> Details Debugging Information Entries (DIEs), .debug_line state machines, location expressions, accelerated lookup tables (.debug_names), Split DWARF (.dwo/.dwp), debuginfod network servers, and macOS dSYM bundles.
> Reaches Use through Rebuild, handing symbol parsing in debuggers to 03-Debugging and crash dump symbolication to Drill-Program-Execution-Stripped-Binary-Crash-and-Split-DWARF.md.

## Related

- Prerequisites: [[ELF-Mach-O-and-PE-Object-Formats]], [[Stack-Unwinding-CFI-and-Exception-ABI]]
- Next: [[Compiler-Pipeline-and-Debuggability]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation|Linux Tracing and Instrumentation]]
- Lab: `code/00-Program-Execution/dwarf_line_parser/` (requires: linux)
