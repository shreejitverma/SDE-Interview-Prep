---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# C++ HWASan and Hardware Memory Tagging

> [!summary] Scope
> This note owns hardware-assisted memory sanitization using ARM64 Top-Byte Ignore (TBI) and Memory Tagging Extension (MTE).
> It covers the Explain and Decide rungs, evaluating 1:16 shadow tag memory, synchronous versus asymmetric fault modes, and production sampling with GWP-ASan.
> It establishes the boundary between high-overhead software shadow memory in ASan and zero-overhead hardware protection in production fleets.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-Sanitizers-and-Memory-Debugging-on-macOS]]
