---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Execute a Zero-Downtime Shared Library SONAME Migration

> [!summary] Scope
> Operational runbook for safely migrating an ABI-breaking C++ shared library across an enterprise fleet without breaking downstream services.
> Details dual-SONAME packaging, inline namespace versioning, GNU ld version script symbol mapping, and side-by-side deployment staging.
> Includes pre-flight validation scripts, symlink management, and emergency rollback procedures.
> Provides concrete verification commands using readelf and ldd to check binary compatibility.

## Related

- Prerequisites: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]], [[Cpp-Glibc-Symbol-Versioning-and-Portable-Linux-Binaries]], [[Diamond-Dependency-and-Incompatible-ABI-Mitigation]]
- Next: [[Contain-and-Roll-Back-a-Bad-Production-Release]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/07-Packaging-and-Release/soname_migration/` (requires: linux)
