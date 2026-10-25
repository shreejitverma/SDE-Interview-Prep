---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ ThreadSanitizer and MemorySanitizer Internals

> [!summary] Scope
> This note owns the compiler instrumentation and runtime state machines for ThreadSanitizer (TSan) and MemorySanitizer (MSan).
> It covers the Explain and Debug rungs, detailing TSan shadow words, vector clocks, and happens-before synchronization alongside MSan bit-exact shadow propagation and origin tracking.
> It explains why MSan requires an instrumented standard library and remains unsupported on macOS.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-UndefinedBehaviorSanitizer-and-UB-Field-Guide]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code|Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code]]
- Lab: `code/03-Debugging/tsan_race_vector/` (requires: linux, docker)
