---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Reading Assembly and Optimization Remarks

> [!summary] Scope
> Owns engineering methodologies for reading x86-64 and AArch64 compiler output, using Compiler Explorer effectively, and interpreting compiler diagnostic remarks.
> Details Clang remarks (-Rpass, -Rpass-missed, -Rpass-analysis, -fsave-optimization-record) and GCC optimization reports (-fopt-info) to verify vectorization and inlining.
> Reaches Use through Explain, handing microarchitectural simulation to llvm-mca and downstream profiling to 05-Profiling.

## Related

- Prerequisites: [[Calling-Conventions-System-V-AMD64-and-AAPCS64]], [[Compiler-Pipeline-and-Debuggability]]
- Next: [[Linkers-LTO-PGO-AutoFDO-and-BOLT]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_89_Reading_the_Compilers_Output/Chapter_89_Reading_the_Compilers_Output|Chapter_89_Reading_the_Compilers_Output]]
- Lab: `code/00-Program-Execution/opt_remarks_asm/`
