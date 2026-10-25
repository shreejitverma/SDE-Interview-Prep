---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# C++ ODR Violations, Static Init, and Linker Template Errors

> [!summary] Scope
> This note owns the diagnosis of One Definition Rule (ODR) violations, static initialization order fiascos, and complex linker or template errors.
> It reaches the Explain and Debug rungs by using ASan ODR indicators, gold/mold linker detection flags, and C++20 concept diagnostics.
> It solves build and link failure modes that evade standard source-level debuggers.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-Clang-Tidy-Static-Analyzer-and-Fuzzing-libFuzzer-AFLPlusPlus]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_104_Undefined_Behaviour_and_the_Optimiser/Chapter_104_Undefined_Behaviour_and_the_Optimiser|Chapter_104_Undefined_Behaviour_and_the_Optimiser]]
