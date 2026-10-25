---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# File Discovery and Batch Execution: Find, Fd, Xargs, Parallel

> [!summary] Scope
> Owns filesystem directory traversal and batch command execution across find, fd, xargs, and GNU parallel.
> Details NUL-delimited safe record separation (-print0, -0), ARG_MAX kernel execve limits, process pool sizing with xargs -P, and distributed load scheduling with GNU parallel.
> Reaches Use through Explain, handing Unicode collation anomalies to Locale-Collation-and-Unicode-in-CLI-Tools.md.

## Related

- Prerequisites: [[Pipelines-Subshells-Redirection-and-Process-Substitution]]
- Next: [[Locale-Collation-and-Unicode-in-CLI-Tools]]
- See also: [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/find-Expressions|find Expressions]], [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/find-print0-and-exec-Plus|find print0 and exec Plus]], [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/xargs-and-Argument-Batching|xargs and Argument Batching]], [[19-Shell-and-Unix-Engineering/15-Concurrency-and-Scheduling/GNU-parallel|GNU parallel]], [[19-Shell-and-Unix-Engineering/15-Concurrency-and-Scheduling/xargs-P|xargs P]]
- Lab: `code/01-Command-Line/batch_find_xargs/`
