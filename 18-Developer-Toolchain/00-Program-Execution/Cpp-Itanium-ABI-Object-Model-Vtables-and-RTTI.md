---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Itanium ABI: Object Model, Vtables, and RTTI

> [!summary] Scope
> Owns the Itanium C++ ABI specification covering object layout, single and multiple inheritance offsets, empty base optimization (EBO), virtual function tables (vtables), virtual table tables (VTTs), adjustor thunks, and runtime type information (std::type_info and the __cxxabiv1::__class_type_info family).
> Details C++ name mangling grammar, guard variables for thread-safe static initialization, and COMDAT vague linkage rules.
> Reaches Explain through Debug, handing call frame unwinding to Stack-Unwinding-CFI-and-Exception-ABI.md and cross-DSO visibility issues to Drill-Program-Execution-Cross-DSO-Exception-and-RTTI-Failure.md.

## Related

- Prerequisites: [[Calling-Conventions-System-V-AMD64-and-AAPCS64]], [[Symbols-Relocations-and-Static-Linking]]
- Next: [[Stack-Unwinding-CFI-and-Exception-ABI]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]]
- Lab: `code/00-Program-Execution/vtable_layout_thunk/`
