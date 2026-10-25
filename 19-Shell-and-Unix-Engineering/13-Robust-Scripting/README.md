---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Robust Scripting

Most scripts work on the happy path and fail silently on every other one.
These notes are the pattern catalog for scripts that fail loudly, clean up, rerun safely, and never leave a half-written result.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Unofficial Strict Mode Critique](Unofficial-Strict-Mode-Critique.md)
2. [Error Handling Patterns](Error-Handling-Patterns.md)
3. [Cleanup Traps and Re Raising Signals](Cleanup-Traps-and-Re-Raising-Signals.md)
4. [Idempotent Scripts](Idempotent-Scripts.md)
5. [Atomic Writes](Atomic-Writes.md)
6. [Locking and Stale Locks](Locking-and-Stale-Locks.md)
7. [Retries Backoff and Jitter](Retries-Backoff-and-Jitter.md)
8. [Timeouts in Scripts](Timeouts-in-Scripts.md)
9. [Argument Parsing and CLI Design](Argument-Parsing-and-CLI-Design.md)
10. [Exit Code Contracts](Exit-Code-Contracts.md)
11. [Configuration Precedence](Configuration-Precedence.md)
12. [Logging from Scripts](Logging-from-Scripts.md)
13. [Dry Run and Apply](Dry-Run-and-Apply.md)
14. [Safe rm and Destructive Operations](Safe-rm-and-Destructive-Operations.md)
15. [Shell Style Guides](Shell-Style-Guides.md)
