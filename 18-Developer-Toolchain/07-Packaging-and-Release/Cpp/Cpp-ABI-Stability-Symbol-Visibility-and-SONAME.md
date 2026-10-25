---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ ABI Stability, Symbol Visibility, and SONAME Mechanics

> [!summary] Scope
> Low-level mechanisms governing C++ binary compatibility, virtual table layouts, struct alignment, and member offsets under the Itanium ABI.
> Details hidden-by-default symbol visibility via -fvisibility=hidden, SONAME tagging, dynamic linking resolution via DT_NEEDED and RUNPATH, and PImpl isolation patterns.
> Defines rules for preventing catastrophic silent memory corruption when upgrading shared objects in production.
> Contrasts header-only library distribution hazards with compiled shared object ABI boundaries.

## Related

- Prerequisites: [[Versioning-SemVer-CalVer-and-ABI]]
- Next: [[Cpp-Glibc-Symbol-Versioning-and-Portable-Linux-Binaries]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_111_Interoperability/Chapter_111_Interoperability_and_the_Stable_C_ABI|Chapter_111_Interoperability_and_the_Stable_C_ABI]]
