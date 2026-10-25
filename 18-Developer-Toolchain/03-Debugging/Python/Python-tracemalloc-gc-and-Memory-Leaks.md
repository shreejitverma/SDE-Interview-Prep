---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python tracemalloc, gc, and Memory Leaks

> [!summary] Scope
> This note owns memory profiling and leak isolation in Python using the tracemalloc module and the cyclic garbage collector.
> It reaches the Use and Explain rungs by analyzing PyTraceMalloc allocator domain hooks, snapshot diffing, generational garbage collection cycles, and reference cycle graphs.
> It separates pure Python object accumulation from unmanaged native memory leaks.

## Related

- Prerequisites: none
- Next: [[Python-Debugging-Hung-Processes-and-C-Extensions]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]]
- Lab: `code/03-Debugging/tracemalloc_leak/`
