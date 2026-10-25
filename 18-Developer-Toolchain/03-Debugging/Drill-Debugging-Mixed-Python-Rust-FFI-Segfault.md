---
type: problem
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Debugging Mixed Python-Rust FFI Segfault

> [!summary] Scope
> This problem note provides a realistic drill diagnosing an intermittent segmentation fault occurring across a PyO3 Rust extension boundary.
> It tests applying GDB with Python extensions, enabling AddressSanitizer across mixed shared libraries, and isolating illegal pointer sharing between runtimes.
> It validates end-to-end troubleshooting skills developed in Rust-FFI-Boundary-Debugging-Across-Cpp-and-Python.

## Related

- Prerequisites: [[Rust-FFI-Boundary-Debugging-Across-Cpp-and-Python]]
- Lab: `code/03-Debugging/drill_ffi_segfault/`
