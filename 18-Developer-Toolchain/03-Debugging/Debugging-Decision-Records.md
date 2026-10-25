---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Debugging Decision Records

> [!summary] Scope
> This playbook owns architectural decision records for debugging infrastructure at engineering organizational scale.
> It evaluates sanitizers versus Valgrind, full core dumps versus minidumps, debuginfod versus centralized symbol storage, and deterministic record-replay in CI.
> It provides default selections, flipping criteria, cost trade-offs, and migration paths across the entire debugging toolchain.

## Related

- Prerequisites: [[Core-Dumps-Minidumps-and-Crash-Reporting]], [[Symbol-Servers-and-debuginfod]], [[Record-and-Replay-rr-UDB-and-Pernosco]]
- Next: [[Debugging-Expert-Defaults-gdbinit-and-lldbinit]]
