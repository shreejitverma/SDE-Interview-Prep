---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Make and Ninja Mechanics

> [!summary] Scope
> Owns low-level execution engines and file-level dependency graph execution.
> Reaches Use, Explain, and Rebuild rungs by examining GNU Make pitfalls, the GNU Make jobserver protocol, which POSIX does not define and Ninja 1.13 joins as a client, and high-performance Ninja execution internals.
> Establishes the boundary with high-level meta-build generators like CMake and Meson.

## Related

- Prerequisites: [[Build-System-Theory-Schedulers-and-Rebuilders]]
- Next: [[Modern-CMake-Architecture-and-Targets]]
- See also: [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Make-Recipe-Execution|Make Recipe Execution]], [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/ONESHELL-and-SHELLFLAGS|ONESHELL and SHELLFLAGS]]
- Lab: `code/06-Build-Systems/ninja_dyndep/`
