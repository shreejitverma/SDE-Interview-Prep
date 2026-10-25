---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Compiler Pipeline and Debuggability

> [!summary] Scope
> Owns the compiler transformation pipeline from lexing and parsing through intermediate representation (LLVM IR, GCC GIMPLE), optimization passes, and machine code generation.
> Analyzes how inlining, Scalar Replacement of Aggregates (SROA), tail call elimination, and loop vectorization degrade or destroy DWARF variable locations and line mappings.
> Reaches Explain through Debug, handing assembly reading to Reading-Assembly-and-Optimization-Remarks.md and interactive debugging to 03-Debugging.

## Related

- Prerequisites: [[Calling-Conventions-System-V-AMD64-and-AAPCS64]], [[DWARF-Debug-Information-and-Symbol-Bundles]]
- Next: [[Reading-Assembly-and-Optimization-Remarks]]
- See also: [[01-CS-Foundations/Computer-Science-Canon/Compilers-and-SSA|Compilers and SSA]]
- Lab: `code/00-Program-Execution/compiler_opt_debug/`
