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
> This note owns hardware-assisted memory tagging: HWASan, a software sanitizer built on ARM64 Top-Byte Ignore (TBI), and the hardware Memory Tagging Extension (MTE).
> It covers the Explain and Decide rungs, evaluating HWASan's 1:16 shadow tag memory, MTE's synchronous, asynchronous, and asymmetric fault modes, and production sampling with GWP-ASan.
> It establishes the boundary between high-overhead software shadow memory in ASan and lower but measurable MTE overhead in production fleets, with synchronous mode costing the most.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-Sanitizers-and-Memory-Debugging-on-macOS]]
