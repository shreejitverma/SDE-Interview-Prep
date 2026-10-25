---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Compile-Time and Binary Size Profiling

> [!summary] Scope
> This note owns compile-time bottleneck diagnosis and binary bloat analysis in Rust projects.
> It covers the Use, Explain, and Debug rungs by deploying cargo build --timings, -Z self-profile crox traces, cargo-bloat, and cargo-llvm-lines.
> It equips engineers to identify expensive generic monomorphizations and strip unnecessary binary weight from release artifacts.

## Related

- Prerequisites: none
