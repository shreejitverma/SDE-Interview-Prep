---
type: playbook
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Rebuild a Minimal Shell With Job Control

> [!summary] Scope
> Owns the architecture, implementation milestones, and verification tests for rebuilding a functional POSIX-style command shell in C++ or Rust.
> Guides implementation of lexical tokenization, multi-stage pipelines with pipe and dup2, file descriptor redirections, child process fork-exec execution, process group assignment, terminal control acquisition via tcsetpgrp, and SIGCHLD/SIGTSTP job control.
> Reaches Rebuild, putting process and pipeline concepts into code.

## Related

- Prerequisites: [[Processes-Fork-Exec-and-File-Descriptors]], [[Signals-Process-Groups-Sessions-and-Job-Control]], [[TTYs-PTYs-and-Line-Discipline]], [[Pipelines-Subshells-Redirection-and-Process-Substitution]]
- Next: [[Drill-Command-Line-Quoting-and-Signal-Trap-Race]]
- See also: [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/fg-bg-jobs-and-the-Foreground-Process-Group|fg bg jobs and the Foreground Process Group]], [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Job-Control-Signals|Job Control Signals]]
- Lab: `code/01-Command-Line/rebuild_mini_shell/` (requires: linux)
