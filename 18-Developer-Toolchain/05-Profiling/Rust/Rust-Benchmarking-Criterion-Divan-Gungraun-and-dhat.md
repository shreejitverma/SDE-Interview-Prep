---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Benchmarking: Criterion, Divan, Gungraun, and dhat

> [!summary] Scope
> This note owns statistical and deterministic benchmarking suites in the Rust ecosystem.
> It reaches the Use, Explain, and Rebuild rungs by implementing Criterion.rs linear regression tests, Divan allocation benchmarks, Gungraun (formerly iai-callgrind) instruction/cache-miss CI counters, and dhat-rs heap profiling.
> It helps ensure that Rust performance gains are statistically robust and reproducible across test runners.

## Related

- Prerequisites: [[Rust-CPU-Profiling-cargo-flamegraph-samply-and-Frame-Pointers]]
- Next: [[Rust-Compile-Time-and-Binary-Size-Profiling]]
- Lab: `code/05-Profiling/criterion_gungraun/` (requires: linux)
