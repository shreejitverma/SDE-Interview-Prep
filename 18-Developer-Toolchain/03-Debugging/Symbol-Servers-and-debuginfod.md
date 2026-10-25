---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Symbol Servers and debuginfod

> [!summary] Scope
> This note owns the indexing, federation, and retrieval of separate debug information using GNU Build IDs, Mach-O UUIDs, debuginfod HTTP servers, and macOS dSYM bundles.
> It covers the Use, Explain, and Decide rungs, explaining symbol stripping, on-demand retrieval and caching, and enterprise symbol distribution pipelines.
> It supplies symbols to debugger sessions in Cpp-GDB-and-LLDB-Expert-Guide and crash triages in Debug-a-Production-Crash-From-a-Core-Dump.

## Related

- Prerequisites: [[Core-Dumps-Minidumps-and-Crash-Reporting]]
- Next: [[Record-and-Replay-rr-UDB-and-Pernosco]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
