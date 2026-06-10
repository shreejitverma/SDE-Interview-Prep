# Python 3.13: Copy-and-Patch JIT Compiler Architecture


### 21.1 The Copy-and-Patch JIT Paradigm (PEP 744)
Python 3.13 introduces an experimental **Copy-and-Patch Just-In-Time (JIT) compiler** (PEP 744). Unlike classic template JITs (which compile bytecode to slow sequences of function calls) or heavy optimizing tracing/method JITs like PyPy or V8 (which compile code via complex graph building and register allocation at runtime), CPython's copy-and-patch JIT achieves fast compilation times with near-zero runtime compilation overhead.

#### 1. Optimization and Compilation Tradeoffs
Traditional JIT compilers (e.g., LLVM-based JITs) generate highly optimized assembly code at runtime, but the compilation process itself requires substantial CPU cycles and memory. This makes them unsuitable for interactive runtimes or workloads with short execution times, where the compilation cost outweighs the speedup.

Copy-and-patch JIT solves this by splitting the compilation workload:
*   **Compile-Time (Static Build Phase)**: High-performance compilers (Clang/LLVM) compile small C functions (micro-operations) into native object files (ELF, Mach-O, or COFF). These object files are parsed to extract the raw machine code bytes (stencils) and metadata describing the location of symbols and offsets (holes).
*   **Runtime (JIT Compilation Phase)**: The runtime JIT engine traverses a hot execution trace, copies the stencils into a memory buffer using simple memory copies (`memcpy`), and fills the holes (patches the relocations) with runtime values like frame offsets, constants, or jump destinations.

This architecture delivers machine-native code execution speeds with a compiler that executes in microseconds.

---

### 21.2 Stencil Relocation and C-Level Internals

#### 1. Machine Code Stencils and Relocations
At the C level, the JIT engine represents bytecode instructions as stencils. A stencil is a sequence of native instructions containing placeholders (holes) that must be resolved with runtime context:

```c
/* Python/jit.c (conceptual layout) */
typedef struct {
    uint32_t offset;         /* Offset in bytes from start of stencil where hole begins */
    uint8_t type;            /* Relocation type (e.g., absolute 64-bit, PC-relative 32-bit) */
    uintptr_t value;         /* Raw value or identifier of symbol to patch into hole */
} JITRelocation;

typedef struct {
    const unsigned char *code;     /* Array of raw machine code bytes */
    size_t code_size;              /* Size of machine code array */
    const JITRelocation *relocs;   /* Pointer to array of relocation entries */
    size_t reloc_count;            /* Count of relocations for this stencil */
} JITStencil;
```

#### 2. Visual Representation of Copy-and-Patch Stitching
```
    Stencil 1: LOAD_FAST                    Stencil 2: LOAD_CONST
+--------------------------------+      +--------------------------------+
| mov rax, [r14 + [   HOLE 1  ]] |      | mov rbx, [rip + [   HOLE 2  ]] |
+--------------------------------+      +--------------------------------+
                  |                                      |
                  | Copy stencils to RX memory           |
                  v                                      v
+------------------------------------------------------------------------+
| mov rax, [r14 + 0x18]                 | mov rbx, 0x103fc9208           |
+------------------------------------------------------------------------+
                  ^                                      ^
                  | Patch with local offset 0x18         | Patch with constant address
```

During the build phase, CPython uses Clang to compile each bytecode case. A Python script (`Tools/cases_generator/jit.py`) parses the resulting object files, extracts the machine code, and outputs a C header file (`jit_cases.c.h`) containing arrays of static stencils.

---

### 21.3 Tier 2 Micro-Ops (uops) and the Optimization Pipeline
CPython 3.13 splits execution into a multi-tiered pipeline:
1.  **Tier 1 (Specializing Interpreter)**: The standard interpreter loop (from Chapter 17) executes bytecode. If a block of bytecode is executed frequently, it triggers dynamic tracing.
2.  **Tier 2 (Tracer / Optimizer)**: A tracing engine compiles the bytecode block into a flat linear sequence of simplified instructions called **Micro-Ops (uops)**.
3.  **Tier 2 Optimization**: The uop trace undergoes basic optimizations:
    *   **Dead Code Elimination**: Removing operations whose outputs are unused.
    *   **Type Propagation**: Propagating specialized types through the trace to eliminate redundant type-checks.
    *   **Abstract Evaluation**: Simulating stack heights to eliminate redundant push and pop operations.
4.  **Tier 2 JIT Execution**: The optimized uop trace is compiled by the Copy-and-Patch JIT engine, producing native machine code.

#### 1. Bytecode to Native Compilation Flow
```
+---------------+      +----------------------+      +-----------------------+
|  Python Code  | ---> |  AST / Tier 1 Byte  | ---> | Specializing ceval.c  |
+---------------+      +----------------------+      +-----------------------+
                                                                 | (Hot block)
                                                                 v
+---------------+      +----------------------+      +-----------------------+
|  JIT Native  | <--- |  Copy & Patch Engine | <--- |  Tier 2 Micro-Ops     |
|  Machine Code |      |  (Stencils + Holes)  |      |  (uop linear trace)   |
+---------------+      +----------------------+      +-----------------------+
```

#### 2. Tier 2 uop Compilation Loop
The JIT compiler processes optimized uops using a loop that matches uop IDs to static stencils:

```c
/* Python/jit.c */
void* _PyJIT_CompileTrace(PyExecutorObject *executor, _PyUOpInstruction *trace, size_t trace_len) {
    // 1. Allocate writable memory page
    unsigned char *code_buffer = allocate_jit_memory();
    unsigned char *cursor = code_buffer;
    
    for (size_t i = 0; i < trace_len; i++) {
        _PyUOpInstruction uop = trace[i];
        // Retrieve static stencil generated at build time
        JITStencil stencil = get_stencil_for_uop(uop.opcode);
        
        // Copy machine code template
        memcpy(cursor, stencil.code, stencil.code_size);
        
        // Patch relocations inside the copied code
        for (size_t j = 0; j < stencil.reloc_count; j++) {
            JITRelocation reloc = stencil.relocs[j];
            uintptr_t patch_value = resolve_uop_symbol(uop, reloc.value);
            patch_hole(cursor + reloc.offset, reloc.type, patch_value);
        }
        
        cursor += stencil.code_size;
    }
    
    // 2. Transition memory permissions to Read/Execute (RX)
    make_jit_memory_executable(code_buffer, cursor - code_buffer);
    return code_buffer;
}
```

---

### 21.4 Relocation Hole Patching and Memory Security
Patching relocation holes requires writing machine-specific offsets directly into the copied stencils. The JIT engine implements several relocation type handlers matching the host architecture:

#### 1. Relocation Implementations
*   **x86_64 Relocations**:
    *   `R_X86_64_64`: Direct absolute 64-bit address patch.
    *   `R_X86_64_PC32`: 32-bit PC-relative address patch (used for relative jumps between stencils).
*   **AArch64 (ARM64) Relocations**:
    *   `R_AARCH64_MOVW_UABS_G0` & `R_AARCH64_MOVW_UABS_G1`: Patches lower/upper bits of immediate load operations (`movz`/`movk`).
    *   `R_AARCH64_CONDBR19`: Patches conditional branch instruction target offsets.

#### 2. Memory Security: W^X Enforcement
Modern operating systems enforce **Write-Once-Read-Execute (W^X)** permissions to prevent security vulnerabilities. A memory page cannot be simultaneously writable and executable.

To adhere to this, CPython manages execution memory in two steps:
1.  **Allocation Phase**: The JIT allocator requests pages from the OS with Read-Write (RW) permissions (using `mmap` with `PROT_READ | PROT_WRITE` on POSIX, or `VirtualAlloc` with `PAGE_READWRITE` on Windows).
2.  **Compilation & Patching**: The compiler copies stencils and writes relocation values into the RW pages.
3.  **Permissions Transition**: Once patching is complete, the JIT locks down the memory, requesting the OS to transition permissions to Read-Execute (RX) (using `mprotect` with `PROT_READ | PROT_EXEC`, or `VirtualProtect` with `PAGE_EXECUTE_READ`).
4.  **Execution**: The executor executes the native machine code block by calling it via a C function pointer.

---
