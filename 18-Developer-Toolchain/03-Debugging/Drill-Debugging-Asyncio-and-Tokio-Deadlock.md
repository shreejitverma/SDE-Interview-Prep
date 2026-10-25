---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Debugging Asyncio and Tokio Deadlock

> [!summary] Scope
> This problem note presents a guided-to-blind scenario diagnosing a live task deadlock across Python asyncio and Rust Tokio services without restarting the process.
> It tests inspection of pending tasks, detection of unawaited futures, and identification of lost wakers using live diagnostic tools.
> It validates mastery of concepts taught in Python-Concurrency-Asyncio-Multiprocessing-and-Free-Threading-Bugs and Rust-Async-Debugging-tokio-console-and-tracing.

## Related

- Prerequisites: [[Python-Concurrency-Asyncio-Multiprocessing-and-Free-Threading-Bugs]], [[Rust-Async-Debugging-tokio-console-and-tracing]]
- Next: [[Drill-Debugging-Mixed-Python-Rust-FFI-Segfault]]
- Lab: `code/03-Debugging/drill_async_deadlock/`
