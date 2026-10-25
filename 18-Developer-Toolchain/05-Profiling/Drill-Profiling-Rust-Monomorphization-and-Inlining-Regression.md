---
type: problem
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Profiling Rust Monomorphization and Inlining Regression

> [!summary] Scope
> This problem note provides a drill investigating an unexpected 3x build slowdown and runtime instruction-cache regression in a generic Rust library.
> It guides the engineer to use cargo-llvm-lines, cargo build --timings, and samply to isolate excessive monomorphization bloat, refactoring hot functions to dynamic dispatch or non-generic helpers.
> It serves as practical validation for compile-time and runtime optimization techniques.

## Related

- Prerequisites: [[Rust-CPU-Profiling-cargo-flamegraph-samply-and-Frame-Pointers]], [[Rust-Compile-Time-and-Binary-Size-Profiling]]
- Lab: `code/05-Profiling/drill_rust_bloat/`
