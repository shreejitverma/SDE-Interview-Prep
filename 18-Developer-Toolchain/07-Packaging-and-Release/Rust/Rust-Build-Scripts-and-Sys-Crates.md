---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Build Scripts and -sys Crate Conventions

> [!summary] Scope
> Execution model of build.rs compile-time scripts, cargo rustc-link instruction emission, and the links manifest key.
> Details sys crate architectural patterns for finding, binding, or vendoring native C and C++ libraries, including metadata communication between downstream crates.
> Explores cross-compilation hazards, rerunning triggers, and host versus target compiler confusion in build scripts.
> Defines best practices for packaging C library wrappers with robust fallback mechanisms.

## Related

- Prerequisites: [[Rust-Cargo-Model-Packages-Targets-Features-and-Workspaces]]
- Next: [[Rust-SemVer-Checks-MSRV-and-Editions]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]]
