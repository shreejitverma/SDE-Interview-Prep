---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Signals, Process Groups, Sessions, and Job Control

> [!summary] Scope
> Owns asynchronous notification mechanics, POSIX standard and real-time signal generation, disposition tables, signal masks, sigaltstack, and async-signal-safety guarantees.
> Explains process groups, session leadership, controlling terminals, and kernel job control signal generation (SIGTSTP, SIGTTIN, SIGTTOU, SIGHUP).
> Reaches Use through Rebuild, leaving line discipline specifics to TTYs-PTYs-and-Line-Discipline.md and shell trap scripting to Robust-Shell-Scripting-and-ShellCheck.md.

## Related

- Prerequisites: [[Processes-Fork-Exec-and-File-Descriptors]]
- Next: [[Proc-Filesystem-and-Process-Credentials]]
- See also: [[01-CS-Foundations/Operating-Systems/IPC-Complete-Guide|IPC Complete Guide]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Signals-Dispositions-Masks-and-Delivery|Signals Dispositions Masks and Delivery]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Async-Signal-Safety|Async Signal Safety]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Sessions-Process-Groups-and-Controlling-Terminals|Sessions Process Groups and Controlling Terminals]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Job-Control-Signals|Job Control Signals]]
- Lab: `code/00-Program-Execution/signal_job_control/` (requires: linux)
