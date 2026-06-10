# PyPy: The JIT and Meta-Tracing


PyPy is an alternative implementation of Python written in **RPython** (a restricted subset of Python).

### 74.1 The Meta-Tracing JIT
Unlike the CPython JIT (Chapter 21), which compiles bytecode to machine code, PyPy's JIT "traces" the execution of the interpreter itself.
*   **Warming Up**: The first time a loop runs, it is interpreted.
*   **Recording**: If the loop is "hot," the JIT records every instruction.
*   **Optimizing**: It removes redundant operations (like repeated type checks for the same object) and generates highly optimized machine code.

### 74.2 Memory Management: The IncrGC
PyPy uses a moving, generational garbage collector. This is more efficient than reference counting for long-running processes but can lead to higher "pause times" during major collections.

---


# Appendices
