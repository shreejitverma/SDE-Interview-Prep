---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust Async Debugging: tokio-console and tracing

> [!summary] Scope
> This note owns runtime diagnostics and state inspection for asynchronous Rust applications built on Tokio.
> It covers the Use and Explain rungs, exploring tokio-console instrumentation, task poll and wake histograms, lost waker detection, and tracing span propagation across await points.
> It resolves async hangs and task starvation issues that remain invisible to conventional thread-based debuggers.

## Related

- Prerequisites: [[Rust-GDB-LLDB-Panics-and-Backtraces]]
- Next: [[Rust-Borrow-Checker-Trait-Errors-Cargo-Expand-and-Clippy]]
