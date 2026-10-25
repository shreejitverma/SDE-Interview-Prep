---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Free-Threaded Python Wheels and Hardware Wheel Variants

> [!summary] Scope
> Binary wheel distribution under free-threaded CPython, ABI tags cp313t and cp314t, and multi-phase initialization with Py_mod_gil.
> Analyzes WheelNext proposals PEP 817 (draft), PEP 825 (provisional), and PEP 850 (draft) for hardware-accelerated wheel variants targeting CUDA, ROCm, and x86-64 microarchitecture levels.
> Establishes matrix build strategies for high-performance Python libraries.
> Details fallback behavior when specialized hardware wheels are requested on generic platforms.

## Related

- Prerequisites: [[Python-Wheels-Platform-Tags-Manylinux-and-ABI3]]
- Next: [[Python-Cross-Platform-Wheel-Pipelines-with-cibuildwheel]]
- See also: [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels|Chapter_57_The_Python_Packaging_Ecosystem_PEP_517_to_Wheels]]
