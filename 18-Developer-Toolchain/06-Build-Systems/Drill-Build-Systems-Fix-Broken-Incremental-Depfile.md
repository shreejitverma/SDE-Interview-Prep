---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Fix a Broken Incremental Depfile Generator

> [!summary] Scope
> Owns a hands-on troubleshooting drill where a custom code generation rule in Ninja and CMake silently produces stale output or race conditions during multi-threaded builds.
> Reaches the Debug rung by providing a failing reproduction environment, requiring the student to inspect dependency logs and header generation flags.
> Guides fixing the faulty depfile emitted by the generator and verifying that incremental edits trigger correct downstream rebuilds without race conditions.

## Related

- Prerequisites: [[Make-and-Ninja-Mechanics]]
- Next: [[Drill-Build-Systems-Break-a-Dependency-Bottleneck]]
- Lab: `code/06-Build-Systems/drill_broken_depfile/`
