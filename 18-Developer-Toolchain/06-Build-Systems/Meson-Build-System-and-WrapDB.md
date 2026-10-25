---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Meson Build System and WrapDB

> [!summary] Scope
> Owns fast, user-friendly meta-build generation utilizing a non-Turing-complete, strictly typed domain-specific language.
> Reaches Use and Explain rungs by analyzing Meson target declarations, dependency resolution with declare_dependency, subproject management via the WrapDB system, built-in cross-compilation options, and unity build support.
> Compares Meson speed, determinism, and maintainability directly against CMake and autotools.

## Related

- Prerequisites: [[Make-and-Ninja-Mechanics]]
- Next: [[Cpp-Modules-in-Build-Systems]]
- See also: [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Autoconf-and-configure|Autoconf and configure]]
- Lab: `code/06-Build-Systems/meson_wrap_build/` (requires: network)
