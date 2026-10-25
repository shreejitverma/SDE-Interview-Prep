---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Rust FFI Packaging: cdylib, staticlib, and Header Generation with cbindgen

> [!summary] Scope
> Exporting stable C-compatible application binary interfaces from Rust libraries using cdylib and staticlib crate types.
> Covers automated C and C++ header generation via cbindgen, repr C data layouts, panic boundaries with C-unwind, symbol visibility, and linker version scripts.
> Prepares native Rust components for seamless consumption in foreign host runtimes without undefined behavior.
> Details memory allocation and ownership handoff across FFI boundaries.

## Related

- Prerequisites: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
- Next: [[Rust-PyO3-and-Maturin-Internals]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]]
