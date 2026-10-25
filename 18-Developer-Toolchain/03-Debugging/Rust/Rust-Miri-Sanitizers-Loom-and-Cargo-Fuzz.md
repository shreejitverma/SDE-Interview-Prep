---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Miri, Sanitizers, Loom, and Cargo-Fuzz

> [!summary] Scope
> This note owns undefined behavior detection, concurrency model checking, and fuzzing in Rust.
> It reaches the Use, Explain, and Debug rungs by analyzing Miri MIR interpretation under Stacked and Tree Borrows, Tier 2 pre-instrumented sanitizer targets, Loom permutation checking, and cargo-fuzz.
> It checks that unsafe Rust blocks adhere to Rust's aliasing and memory safety invariants on the paths that tests actually execute.

## Related

- Prerequisites: [[Rust-GDB-LLDB-Panics-and-Backtraces]]
- Next: [[Rust-Async-Debugging-tokio-console-and-tracing]]
- Lab: `code/03-Debugging/miri_tree_borrows/` (requires: nightly)
