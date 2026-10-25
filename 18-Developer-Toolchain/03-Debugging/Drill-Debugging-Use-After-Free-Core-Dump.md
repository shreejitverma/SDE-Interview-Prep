---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Debugging Use-After-Free From a Core Dump

> [!summary] Scope
> This problem note provides a timed, guided-to-blind drill for isolating a use-after-free crash from a stripped C++ core dump and debug symbol archive.
> It tests the engineer's ability to extract registers, reconstruct damaged stack frames, and pinpoint the corrupt allocation using GDB or LLDB within 30 minutes.
> It serves as the primary practical validation for Debug-a-Production-Crash-From-a-Core-Dump.

## Related

- Prerequisites: [[Debug-a-Production-Crash-From-a-Core-Dump]]
- Next: [[Drill-Debugging-Asyncio-and-Tokio-Deadlock]]
- Lab: `code/03-Debugging/drill_uaf_core/` (requires: linux)
