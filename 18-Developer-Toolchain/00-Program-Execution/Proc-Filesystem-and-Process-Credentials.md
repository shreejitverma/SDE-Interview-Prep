---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Proc Filesystem and Process Credentials

> [!summary] Scope
> Owns live process introspection via /proc/[pid]/* pseudofiles (maps, smaps_rollup, pagemap, fd, status, auxv, oom_score_adj) and POSIX process credentials.
> Covers real, effective, saved, and filesystem UID/GID transitions alongside Linux capability sets.
> Reaches Use through Debug, bounding its scope against shell-level inspection tools covered in System-Inspection-and-Syscall-Tracing-From-the-Shell.md.

## Related

- Prerequisites: [[Processes-Fork-Exec-and-File-Descriptors]]
- Next: [[TTYs-PTYs-and-Line-Discipline]]
- See also: [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/The-proc-Filesystem|The proc Filesystem]], [[19-Shell-and-Unix-Engineering/12-Linux-System-Interfaces/Linux-Capabilities|Linux Capabilities]]
- Lab: `code/00-Program-Execution/proc_credentials/` (requires: linux)
