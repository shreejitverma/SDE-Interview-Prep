---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rust FFI Boundary Debugging Across C++ and Python

> [!summary] Scope
> This note owns cross-language debugging across Foreign Function Interface (FFI) boundaries connecting Rust, C++, and Python.
> It covers the Explain and Debug rungs, analyzing extern C versus extern C-unwind panic safety, memory layout compatibility across bindgen and PyO3, and mixed-sanitizer linking.
> It isolates fault points that occur when ownership, alignment, or exception models clash across language runtimes.

## Related

- Prerequisites: [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]], [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]], [[Python-Debugging-Hung-Processes-and-C-Extensions]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]]
