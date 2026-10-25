---
type: problem
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Cross-DSO Exception and RTTI Failure

> [!summary] Scope
> Blind problem drill presenting a mysterious std::terminate crash when an exception is thrown across a dlopen(..., RTLD_LOCAL) shared library boundary.
> Requires the engineer to diagnose typeinfo pointer inequality resulting from hidden RTTI visibility, inspect the Itanium personality routine exception match logic, and rectify symbol export maps.
> Reaches Debug, validating mastery of Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI.md and Dynamic-Linking-PLT-GOT-and-Loaders.md.

## Related

- Prerequisites: [[Cpp-Itanium-ABI-Object-Model-Vtables-and-RTTI]], [[Stack-Unwinding-CFI-and-Exception-ABI]], [[Dynamic-Linking-PLT-GOT-and-Loaders]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/00-Program-Execution/drill_cross_dso_rtti/` (requires: linux)
