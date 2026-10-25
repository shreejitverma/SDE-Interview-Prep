---
type: problem
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Diagnose and Remediate a manylinux GLIBC Symbol Leak

> [!summary] Scope
> Hands-on diagnostic scenario where a compiled Python wheel built on a modern Linux host fails at import time on older deployment targets with glibc version lookup errors.
> Guides the engineer through inspecting GNU version requirement sections via readelf, diagnosing unpinned build host dependencies, and configuring auditwheel and cibuildwheel containerized baselines.
> Reinforces practical binary analysis and symbol version auditing under pressure.
> Delivers a fully verified solution with automated repro scripts.

## Related

- Prerequisites: [[Python-Wheels-Platform-Tags-Manylinux-and-ABI3]], [[Cpp-Glibc-Symbol-Versioning-and-Portable-Linux-Binaries]]
- Next: [[Drill-Packaging-Cpp-Silent-ABI-Break-Across-Shared-Libraries]]
- Lab: `code/07-Packaging-and-Release/manylinux_leak/` (requires: linux, docker)
