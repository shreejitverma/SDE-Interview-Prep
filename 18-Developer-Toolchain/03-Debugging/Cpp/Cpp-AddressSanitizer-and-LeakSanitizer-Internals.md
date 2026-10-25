---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ AddressSanitizer and LeakSanitizer Internals

> [!summary] Scope
> This note owns the mechanical internals of Clang and GCC AddressSanitizer (ASan) and LeakSanitizer (LSan).
> It reaches the Use, Explain, and Rebuild rungs by deriving 1:8 shadow memory byte encoding, redzone poison checking, quarantine ring-buffers, and LSan stop-the-world pointer scanning.
> It links to existing usage guides in CPP_Zero_to_Godhood Chapter 105 while leaving hardware tagging to Cpp-HWASan-and-Hardware-Memory-Tagging.

## Related

- Prerequisites: none
- Next: [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code|Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code]]
- Lab: `code/03-Debugging/asan_shadow_mapping/`
