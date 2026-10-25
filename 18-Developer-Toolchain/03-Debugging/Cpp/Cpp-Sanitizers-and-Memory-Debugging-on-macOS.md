---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Sanitizers and Memory Debugging on macOS

> [!summary] Scope
> This note owns memory and concurrency debugging specific to macOS and Apple Silicon hardware.
> It reaches the Use and Debug rungs by documenting ASan and TSan capabilities, native leaks analysis, libgmalloc guard pages, and MallocScribble instrumentation.
> It explicitly identifies platform boundaries where LSan, MSan, and HWASan cannot be used under macOS.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-Valgrind-Memcheck-Helgrind-and-DRD]]
