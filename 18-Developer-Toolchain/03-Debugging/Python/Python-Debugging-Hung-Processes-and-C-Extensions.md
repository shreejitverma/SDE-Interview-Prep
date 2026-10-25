---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Python Debugging Hung Processes and C Extensions

> [!summary] Scope
> This note owns techniques for diagnosing deadlocked Python processes and segfaulting native C extensions using py-spy and GDB.
> It covers the Use and Explain rungs, exploring py-spy dump out-of-process virtual memory reading, python-gdb.py macros like py-bt, and PyObject inspection.
> It bridges pure Python execution issues with compiled C/C++ extension debugging.

## Related

- Prerequisites: [[Python-faulthandler-and-Crash-Diagnostics]], [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- Next: [[Python-Concurrency-Asyncio-Multiprocessing-and-Free-Threading-Bugs]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]], [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS|Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS]]
