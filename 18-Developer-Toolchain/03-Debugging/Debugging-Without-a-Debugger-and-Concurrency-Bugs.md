---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Debugging Without a Debugger and Concurrency Bugs

> [!summary] Scope
> This note owns techniques for diagnosing live production concurrency bugs without attaching an interactive debugger.
> It covers in-memory ring-buffer flight recorders, dynamic assertion tripwires, lock-order acquisition graphs, and deterministic thread interleaving.
> It reaches the Use, Explain, and Debug rungs, leaving static synchronization checking to language-specific sanitizer notes and full execution replay to rr.

## Related

- Prerequisites: [[Scientific-Debugging-Delta-Debugging-and-Heisenbugs]]
- Next: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
