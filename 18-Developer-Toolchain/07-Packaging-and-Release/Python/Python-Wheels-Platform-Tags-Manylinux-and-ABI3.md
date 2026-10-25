---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python Wheels: Platform Tags, manylinux Baselines, and the abi3 Stable API

> [!summary] Scope
> Structure and archive invariants of Python wheels, platform compatibility tags, and the PEP 600 perennial manylinux_2_28 and PEP 656 musllinux standards.
> Details external shared library grafting and symbol auditing via auditwheel, delocate, and delvewheel.
> Covers the CPython Limited API and abi3 stable ABI contract across minor Python versions.
> Examines common pitfalls where extension modules inadvertently leak unversioned CPython symbols.

## Related

- Prerequisites: [[Python-Packaging-History-and-pyproject-toml]]
- Next: [[Python-Free-Threaded-Wheels-and-Wheel-Variants]]
- See also: [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels|Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels]], [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_24_C_Extensions__Python_C-API_Interoperability|Chapter_24_C_Extensions__Python_C API_Interoperability]]
