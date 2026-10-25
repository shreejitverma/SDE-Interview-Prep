---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Quoting and Signal Trap Race

> [!summary] Scope
> Guided problem drill presenting a brittle deployment shell script plagued by quoting mistakes, IFS word splitting errors, command substitution exit code masking (SC2155), and leaked background child processes on SIGTERM.
> Guides the engineer through isolating the failures using ShellCheck and execution traces, fixing subshell scopes, and implementing idempotent signal cleanup traps.
> Reaches Debug and Use, reinforcing Robust-Shell-Scripting-and-ShellCheck.md.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]], [[Robust-Shell-Scripting-and-ShellCheck]]
- Next: [[Drill-Command-Line-Catastrophic-Backtracking-and-Stream-Bottleneck]]
- See also: [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/errexit-Semantics-and-Exceptions|errexit Semantics and Exceptions]]
- Lab: `code/01-Command-Line/drill_quoting_traps/`
