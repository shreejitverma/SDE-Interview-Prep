---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Capstone: POSIX-Compatible Unix Shell with Job Control

> [!summary] Scope
> This capstone playbook guides the construction of a production-quality Unix shell in Rust or C++ supporting full pipeline orchestration, file descriptor redirection, and terminal job control.
> It covers recursive-descent command parsing, fork/execve and posix_spawn execution, pipeline pipe setup, and subshell handling.
> It details process group management, controlling terminal synchronization via tcsetpgrp, foreground/background job state machines, and asynchronous signal handling for SIGINT, SIGTSTP, and SIGCHLD.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]], [[Pipelines-Subshells-Redirection-and-Process-Substitution]], [[Signals-Process-Groups-Sessions-and-Job-Control]], [[TTYs-PTYs-and-Line-Discipline]], [[Rebuild-a-Minimal-Shell-With-Job-Control]]
- See also: [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Sessions-Process-Groups-and-Controlling-Terminals|Sessions Process Groups and Controlling Terminals]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/fg-bg-jobs-and-the-Foreground-Process-Group|fg bg jobs and the Foreground Process Group]]
- Lab: `code/11-Arena-and-Capstones/capstone_unix_shell/` (requires: linux)
