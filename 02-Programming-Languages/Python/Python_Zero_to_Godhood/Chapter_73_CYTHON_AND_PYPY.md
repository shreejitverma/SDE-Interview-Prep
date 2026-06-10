# Phase XIV: Performance Engineering: Cython and PyPy

For applications that hit the limits of CPython, developers often turn to alternative interpreters or ahead-of-time (AOT) compilers.

# Chapter 73: Cython: The C-Python Hybrid

Cython is a superset of Python that allows you to write C-types directly in your Python code.

### 73.1 Type Annotations for Speed
By adding `cdef` declarations, you can tell the compiler to bypass Python's dynamic lookup system entirely.
```cython
def python_sum(n):
    s = 0
    for i in range(n):
        s += i
    return s

cdef long c_sum(long n):
    cdef long s = 0
    cdef long i
    for i in range(n):
        s += i
    return s
```
The `c_sum` version compiles to pure C, running at the speed of a native executable while maintaining Python's syntax.

### 73.2 Wrapping C Libraries
Cython is the preferred tool for wrapping large C/C++ libraries (like SciPy or SpaCy). It handles the marshaling between Python and C types with minimal manual reference counting.

---

# Chapter 74: PyPy: The JIT and Meta-Tracing

PyPy is an alternative implementation of Python written in **RPython** (a restricted subset of Python).

### 74.1 The Meta-Tracing JIT
Unlike the CPython JIT (Chapter 21), which compiles bytecode to machine code, PyPy's JIT "traces" the execution of the interpreter itself.
*   **Warming Up**: The first time a loop runs, it is interpreted.
*   **Recording**: If the loop is "hot," the JIT records every instruction.
*   **Optimizing**: It removes redundant operations (like repeated type checks for the same object) and generates highly optimized machine code.

### 74.2 Memory Management: The IncrGC
PyPy uses a moving, generational garbage collector. This is more efficient than reference counting for long-running processes but can lead to higher "pause times" during major collections.

---
