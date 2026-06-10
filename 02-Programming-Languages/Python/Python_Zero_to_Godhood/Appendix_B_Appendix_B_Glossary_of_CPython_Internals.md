# Appendix B: Glossary of CPython Internals


This glossary provides precise definitions for the terms used by core developers and "Godhood" level practitioners.

*   **Arena**: A large block of memory (typically 256KB) allocated from the OS by `PyMalloc`. Arenas are divided into **Pools**.
*   **BATS (Basic Abstract Type System)**: The internal categorization of types in the C-API.
*   **Borrowed Reference**: A pointer to a `PyObject` where the caller does not own the reference. You must not call `Py_DECREF` on it unless you first call `Py_INCREF`.
*   **Check Interval**: The frequency at which the interpreter checks for signals and thread switches.
*   **Compact Dictionary**: A memory-optimized dict implementation (introduced in Python 3.6) that uses a dense array for values and a sparse index for keys.
*   **Descriptor**: Any object that defines `__get__`, `__set__`, or `__delete__`. These power properties, class methods, and the entire `bound method` system.
*   **Free-Threaded**: A build of Python (3.13+) where the Global Interpreter Lock has been removed, replaced by fine-grained locking and biased reference counting.
*   **Interning**: The process of storing only one copy of an immutable object (like short strings or small integers) in a global pool to save memory and allow identity comparison (`is`) instead of equality (`==`).
*   **MRO (Method Resolution Order)**: The linearized order in which Python searches for attributes in a class hierarchy, calculated using the C3 linearization algorithm.
*   **Obmalloc**: The CPython custom memory allocator specialized for small objects (less than 512 bytes).
*   **Opcodes**: The numerical identifiers for virtual machine instructions (e.g., `LOAD_CONST`, `CALL_FUNCTION`).
*   **Peephole Optimizer**: A compiler stage that looks at small sequences of bytecode and replaces them with more efficient versions (e.g., `1 + 2` $\rightarrow$ `3`).
*   **PyObject**: The base C-struct for all Python objects. It contains the reference count (`ob_refcnt`) and a pointer to the type object (`ob_type`).
*   **Slot**: A function pointer field in the `PyTypeObject` struct that corresponds to a dunder method (e.g., `tp_call` for `__call__`).
*   **Tiers of Execution**: In Python 3.13+, the VM moves from Tier 1 (standard bytecode) to Tier 2 (specialized/optimized micro-ops) based on execution frequency.

---
