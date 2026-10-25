---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Package Discovery: CMake Configs, pkg-config, and Common Package Specification

> [!summary] Scope
> Authoring relocatable C++ library packages using CMake export sets, config-file package generators, and pc files via pkg-config.
> Analyzes Common Package Specification v0.15.0 JSON schemas and CMake 4.3+ native CPS integration via install PACKAGE_INFO.
> Contrasts modern target-based usage requirements with legacy transitive link flags and non-relocatable hardcoded file paths.
> Covers best practices for publishing multi-component libraries with optional dependencies.

## Related

- Prerequisites: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]]
- Next: [[Cpp-Distributing-Cxx20-Modules]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling|Chapter_110_Build_Systems_Dependency_Management_and_Tooling]]
