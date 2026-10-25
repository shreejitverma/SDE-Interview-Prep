---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Triage a Silent Memory Corruption ABI Break Across Shared Libraries

> [!summary] Scope
> Investigative exercise where adding a struct member in a dynamic library causes an unbumped SONAME binary to silently corrupt heap memory in downstream calling applications.
> The reader inspects memory dumps, traces struct padding differences via GDB and pahole, and repairs the interface using symbol versioning and PImpl idiom isolation.
> Teaches root-cause identification of memory layout mismatches across dynamic linking boundaries.
> Provides automated test cases demonstrating both broken and corrected library states.

## Related

- Prerequisites: [[Cpp-ABI-Stability-Symbol-Visibility-and-SONAME]]
- Next: [[Drill-Packaging-Cargo-Feature-Unification-Hazard]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
- Lab: `code/07-Packaging-and-Release/cpp_abi_break/` (requires: linux)
