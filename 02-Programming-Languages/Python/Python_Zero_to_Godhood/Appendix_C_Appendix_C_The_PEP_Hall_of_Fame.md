# Appendix C: The PEP Hall of Fame


The history of Python is the history of its **Python Enhancement Proposals (PEPs)**.

| PEP # | Title | Impact |
| :--- | :--- | :--- |
| **PEP 8** | Style Guide for Python Code | The standard for readable, idiomatic Python. |
| **PEP 20** | The Zen of Python | The guiding philosophy of the language. |
| **PEP 257** | Docstring Conventions | Formalized internal documentation. |
| **PEP 343** | The "with" Statement | Introduced context managers and resource safety. |
| **PEP 380** | Syntax for Delegating to a Subgenerator | Introduced `yield from`. |
| **PEP 443** | Single-dispatch generic functions | Functional-style polymorphism. |
| **PEP 484** | Type Hints | The foundation of modern Python static typing. |
| **PEP 498** | Literal String Interpolation | Introduced F-Strings. |
| **PEP 525** | Asynchronous Generators | Bridged the gap between `asyncio` and `yield`. |
| **PEP 572** | Assignment Expressions | The Walrus Operator (`:=`). |
| **PEP 594** | Removing dead batteries | Cleaned up the Standard Library for Python 3.13. |
| **PEP 634** | Structural Pattern Matching | Introduced `match` and `case`. |
| **PEP 703** | Making the GIL Optional | The roadmap for Free-Threaded Python. |

---

**END OF APPENDICES**

---




To achieve the ultimate level of "Godhood," one must look beyond the virtual machine and understand how Python interacts with physical hardware.

# Chapter 70: CPU Cache Locality and Data Alignment

Modern CPUs are significantly faster than system memory. Performance is often bottlenecked by the "Memory Wall."

### 70.1 The Cache Hierarchy (L1, L2, L3)
When the CPU needs data, it checks the caches first. A cache hit takes ~1-10 cycles, while a main memory access (cache miss) takes ~200-300 cycles.

#### 1. Why Python is Cache-Unfriendly
Standard Python objects are scattered across the heap. A `list` of `float` objects is actually an array of pointers to `PyObject` structs.
*   **Pointer Chasing**: To read the value of `mylist[0]`, the CPU must load the pointer, then jump to another memory location to load the actual float value. This jump often causes a cache miss.

#### 2. The Solution: `array.array` and NumPy
As seen in Chapter 28, contiguous memory is the secret. By storing raw C-types in a block, the CPU can pre-fetch the next values into the cache, leading to 10x-100x speedups for numerical processing.

### 70.2 Memory Alignment and Padding
C-structs (like those in Chapter 24) are padded by the compiler to ensure that fields start at memory addresses divisible by their size (e.g., an 8-byte double should start at an 8-byte boundary).
*   **Performance**: Misaligned access can require two memory fetches instead of one, or even trigger hardware exceptions on some architectures.

---
