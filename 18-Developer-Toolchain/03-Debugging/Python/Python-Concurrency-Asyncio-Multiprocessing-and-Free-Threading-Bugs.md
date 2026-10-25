---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Python Concurrency: Asyncio, Multiprocessing, and Free-Threading Bugs

> [!summary] Scope
> This note owns the diagnosis of concurrency bugs across Python asyncio, multiprocessing, and free-threaded (PEP 703) interpreter builds.
> It reaches the Use, Explain, and Debug rungs, explaining PYTHONASYNCIODEBUG event loop stalls, multiprocessing fork deadlocks, and multi-core data races in GIL-disabled builds.
> It equips engineers to use ThreadSanitizer effectively on modern concurrent Python codebases.

## Related

- Prerequisites: [[Python-Debugging-Hung-Processes-and-C-Extensions]]
- Next: [[Python-Hypothesis-and-Type-Checkers-as-Bug-Prevention]]
- Lab: `code/03-Debugging/free_threaded_race/`
