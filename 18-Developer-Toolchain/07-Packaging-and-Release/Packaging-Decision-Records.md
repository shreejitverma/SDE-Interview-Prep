---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Packaging and Release Architecture Decision Records

> [!summary] Scope
> Evaluated trade-off matrices and architectural decision records for selecting tooling and distribution architectures.
> Compares pip versus uv versus Conda and Pixi, Conan 2 versus vcpkg versus Spack, static versus dynamic library distribution, and SemVer versus CalVer.
> Guides senior engineers in aligning package topologies with build performance and ecosystem constraints.
> Establishes formal rationale documentation templates for enterprise release infrastructure changes.

## Related

- Prerequisites: [[Versioning-SemVer-CalVer-and-ABI]], [[Dependency-Resolution-SAT-PubGrub-and-MVS]]
- Next: [[Ship-a-Rust-Extension-in-a-Python-Wheel]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling|Chapter_110_Build_Systems_Dependency_Management_and_Tooling]]
