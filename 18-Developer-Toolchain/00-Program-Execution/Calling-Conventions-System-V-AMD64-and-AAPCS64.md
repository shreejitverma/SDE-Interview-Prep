---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Calling Conventions: System V AMD64 and AAPCS64

> [!summary] Scope
> Owns procedure call standards and ABI contracts for x86-64 (System V AMD64) and AArch64 (AAPCS64).
> Details argument passing register assignments, return value classification, 16-byte stack alignment invariants, the 128-byte x86-64 red zone, variadic function handling, and vector ABI extensions (AVX-512, SVE).
> Reaches Use through Rebuild, handing unwinding frame records to Stack-Unwinding-CFI-and-Exception-ABI.md and C++ name mangling to Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI.md.

## Related

- Prerequisites: [[ELF-Mach-O-and-PE-Object-Formats]]
- Next: [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_89_Reading_the_Compilers_Output/Chapter_89_Reading_the_Compilers_Output|Chapter_89_Reading_the_Compilers_Output]]
- Lab: `code/00-Program-Execution/calling_convention_abi/`
