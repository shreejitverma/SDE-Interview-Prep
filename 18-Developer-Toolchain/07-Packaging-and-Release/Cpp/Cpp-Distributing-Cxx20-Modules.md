---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Distributing C++20 Modules: BMI Portability and Package Metadata

> [!summary] Scope
> Packaging challenges of C++20 Named Modules arising from compiler-flag and version fragility of Binary Module Interfaces.
> Details CMake FILE_SET CXX_MODULES installation semantics, P1689R5 dependency scanning integration, and CPS metadata models for source-plus-metadata module distribution.
> Establishes architectural rules for publishing portable C++20 module libraries across distinct compiler vendors.
> Explores why pre-compiled BMIs cannot be distributed like traditional pre-compiled shared libraries.

## Related

- Prerequisites: [[Cpp-CMake-Package-Configs-pkg-config-and-CPS]]
- Next: [[Cpp-Conan-2-Architecture-and-Binary-Model]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling|Chapter_110_Build_Systems_Dependency_Management_and_Tooling]]
