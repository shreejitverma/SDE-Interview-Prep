---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Valgrind Memcheck, Helgrind, and DRD

> [!summary] Scope
> This note owns dynamic binary translation debugging using the Valgrind VEX intermediate representation.
> It reaches the Explain and Debug rungs, detailing Memcheck V-bit and A-bit tracking, Helgrind happens-before race detection with lock-order checking, and DRD happens-before thread analysis.
> It contrasts binary instrumentation with compiler sanitizers, noting Valgrind's lack of upstream arm64 macOS support and its role for uninstrumented binaries.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]], [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]]
- Next: [[Cpp-ODR-Violations-Static-Init-and-Linker-Template-Errors]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/09-part-design/36_advanced_debugging|36_advanced_debugging]]
- Lab: `code/03-Debugging/valgrind_memcheck/` (requires: linux)
