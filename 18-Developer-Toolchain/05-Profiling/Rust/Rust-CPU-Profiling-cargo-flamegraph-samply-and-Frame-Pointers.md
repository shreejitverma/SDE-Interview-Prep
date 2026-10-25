---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust CPU Profiling: cargo-flamegraph, samply, and Frame Pointers

> [!summary] Scope
> This note owns CPU profiling for compiled Rust binaries on Linux and macOS.
> It reaches the Use, Explain, and Debug rungs by deploying cargo flamegraph and samply, evaluating rustc frame pointer defaults and the frame pointers std has been built with since Rust 1.79, and resolving inlined generic stack frames.
> It teaches engineers how to extract actionable CPU profiles without sacrificing release build optimizations.

## Related

- Prerequisites: none
- Next: [[Rust-Benchmarking-Criterion-Divan-Gungraun-and-dhat]]
- Lab: `code/05-Profiling/rust_flamegraph/`
