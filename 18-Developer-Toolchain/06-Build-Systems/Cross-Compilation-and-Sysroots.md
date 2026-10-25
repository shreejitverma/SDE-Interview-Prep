---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Cross-Compilation, Triples, and Hermetic Sysroots

> [!summary] Scope
> Owns multi-architecture build targets and toolchain isolation.
> Reaches Use, Explain, and Debug rungs by defining host, build, and target triples, constructing reproducible hermetic sysroots, and configuring target toolchains in CMake, Meson, and Bazel platform transitions.
> Explains runtime library resolution hazards and cross-compilation testing using QEMU user emulation.

## Related

- Prerequisites: [[Modern-CMake-Architecture-and-Targets]], [[Hermetic-and-Reproducible-Builds]]
- Next: [[Cargo-Build-Graph-and-Rustc-Pipelining]]
- See also: [[19-Shell-and-Unix-Engineering/18-Build-Packaging-and-Bootstrap/Portable-sh-for-configure|Portable sh for configure]]
- Lab: `code/06-Build-Systems/cross_sysroot_demo/` (requires: linux)
