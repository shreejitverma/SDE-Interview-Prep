---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++20 Modules in Build Systems

> [!summary] Scope
> Owns the compilation architecture and DAG mutation required for C++20 Named Modules and Header Units.
> Reaches Explain and Debug rungs by analyzing the architectural shift of C++ compilation from independent parallel translation units to dynamic two-phase compilation where pre-scanning discovers module dependencies and instructs Ninja via dyndep to compile Binary Module Interfaces before consumers.
> Compares CMake native cxxmodules engine against Bazel and Buck2 module roadmaps.

## Related

- Prerequisites: [[Make-and-Ninja-Mechanics]], [[Modern-CMake-Architecture-and-Targets]]
- Next: [[Bazel-Skyframe-Bzlmod-and-Execution-Model]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/10-part-compilation/39_build_systems_and_modules|39_build_systems_and_modules]]
- Lab: `code/06-Build-Systems/cpp_modules_ninja/`
