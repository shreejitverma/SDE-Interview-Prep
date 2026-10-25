---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ UndefinedBehaviorSanitizer and UB Field Guide

> [!summary] Scope
> This note owns the catalog of C++ undefined behavior forms detected by UndefinedBehaviorSanitizer (UBSan).
> It reaches the Use, Explain, and Debug rungs by contrasting trap modes versus runtime handlers, vptr dynamic type verification, alignment checks, and integer overflows.
> It provides a taxonomy of UB failure modes, grounding compiler optimization assumptions explored in Chapter 104.

## Related

- Prerequisites: none
- Next: [[Cpp-HWASan-and-Hardware-Memory-Tagging]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_104_Undefined_Behaviour_and_the_Optimiser/Chapter_104_Undefined_Behaviour_and_the_Optimiser|Chapter_104_Undefined_Behaviour_and_the_Optimiser]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code|Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code]]
- Lab: `code/03-Debugging/ubsan_checks/`
