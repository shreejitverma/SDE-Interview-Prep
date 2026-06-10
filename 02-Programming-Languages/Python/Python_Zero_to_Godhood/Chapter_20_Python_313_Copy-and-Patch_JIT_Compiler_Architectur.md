# Python 3.13: Copy-and-Patch JIT Compiler Architecture


### 21.1 Copy-and-Patch JIT Compiler Pipeline
Python 3.13 introduces an experimental **Copy-and-Patch Just-In-Time (JIT) compiler** (PEP 744). Unlike classic template JITs or heavy tracing compilers (like PyPy or V8), a copy-and-patch JIT achieves fast compilation times with very low runtime memory overhead.

#### 1. Compile-Time vs. Runtime Pipeline
```
BUILD TIME (Clang/LLVM):
[CPython C Code] -> [LLVM Clang] -> [Machine Code Stencils (ELF)] -> [Stencil Assembly Extractor] -> [Stitcher Headers]

RUNTIME EXECUTION:
[Bytecode Instructions] -> [JIT Engine] -> [Copy Stencils to Memory] -> [Patch Relocations/Variables] -> [Execute Native Code]
```

1. **Build Time**: The CPython build system uses LLVM/Clang to compile individual bytecode execution paths (helpers in `ceval.c`) into object file templates (stencils). A python script parses these ELF files and extracts the raw machine instructions, noting the offsets of "holes" (relocations) representing variables, stack offsets, or jump addresses.
2. **Runtime**: When a bytecode sequence becomes "hot," the JIT engine copies the pre-compiled stencils into an executable memory page and patches the holes with runtime addresses (e.g., target object references or jump offsets).

---

### 21.2 Stencil Relocation and Hole Patching
Consider a bytecode operation like `LOAD_FAST` inside a stencil template:
```assembly
mov rax, [r14 + HOLE_OFFSET_LOCAL_VAR]  ; Load local variable offset
```

During the patching phase, the JIT engine replaces the placeholder `HOLE_OFFSET_LOCAL_VAR` with the actual frame offset:
$$\text{Relocation Address} = \text{Frame Base} + (\text{Var Index} \times 8)$$
The engine then calls `mprotect()` to set the memory page's permissions to Read/Execute (RX), enabling direct hardware-native execution of the compiled sequence.

---

### 21.3 JIT Performance & Memory Footprint Bounds
Because the JIT engine only performs simple memory copying (`memcpy`) and value patching, it avoids the heavy parsing, graph building, register allocation, and optimization passes associated with traditional JIT compilers. 
*   **Compilation Cost**: Compiling code is extremely fast, requiring only microseconds.
*   **Memory Footprint**: Minimal overhead, as the template stencils are compiled during CPython's build time and stored in the static binary data section.
*   **Instruction Cache Friendly**: Generated machine code templates match the specialized adaptive bytecodes from Chapter 17, ensuring high hardware pipeline utilization.

---
