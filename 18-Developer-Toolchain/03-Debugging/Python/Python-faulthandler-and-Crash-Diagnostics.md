---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python faulthandler and Crash Diagnostics

> [!summary] Scope
> This note owns the diagnosis of fatal CPython crashes, segmentation faults, and interpreter hangs using the standard faulthandler module.
> It covers the Use and Explain rungs, detailing async-signal-safe Python traceback dumping, C-level stack extraction in Python 3.14+, and watchdog timeouts.
> It enables engineers to capture diagnostic state before the operating system terminates a corrupt interpreter process.

## Related

- Prerequisites: none
- Next: [[Python-tracemalloc-gc-and-Memory-Leaks]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]]
