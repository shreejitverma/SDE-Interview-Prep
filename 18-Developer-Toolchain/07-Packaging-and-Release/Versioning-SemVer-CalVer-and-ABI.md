---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Versioning: SemVer 2.0.0, CalVer, and ABI Compatibility

> [!summary] Scope
> Mathematical and operational semantics of SemVer 2.0.0, Calendar Versioning schemes, and the divergence between API and ABI compatibility.
> Details precedence rules, pre-release tagging hazards, build metadata invariants, and binary breaking changes.
> Establishes criteria for selecting versioning schemes based on distribution cadence and downstream consumption constraints.
> Covers the boundary where source-compatible changes break binary layout, symbol resolution, or calling conventions.

## Related

- Prerequisites: none
- Next: [[Dependency-Resolution-SAT-PubGrub-and-MVS]]
- See also: [[17-Cloud-AI-Engineering/Volume_06_Infrastructure_as_Code_and_Delivery/Chapter_02_Enterprise_Module_Design_and_Abstraction_Layers|Chapter_02_Enterprise_Module_Design_and_Abstraction_Layers]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]]
