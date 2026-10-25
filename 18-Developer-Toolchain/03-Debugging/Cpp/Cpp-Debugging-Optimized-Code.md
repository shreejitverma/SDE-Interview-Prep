---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Debugging Optimized Code

> [!summary] Scope
> This note owns the mechanics of debugging release binaries compiled with -O2, -O3, and Link-Time Optimization.
> It reaches the Explain and Debug rungs by analyzing instruction reordering, register spilling, inlined call chains, and DWARF location expressions like DW_OP_entry_value.
> It equips engineers to reconstruct optimized-out parameters and variables directly from disassembly and register state.

## Related

- Prerequisites: [[Cpp-GDB-and-LLDB-Expert-Guide]]
- Next: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO|Chapter_102_Linking_Loading_ABI_and_WPO]], [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_104_Undefined_Behaviour_and_the_Optimiser/Chapter_104_Undefined_Behaviour_and_the_Optimiser|Chapter_104_Undefined_Behaviour_and_the_Optimiser]]
