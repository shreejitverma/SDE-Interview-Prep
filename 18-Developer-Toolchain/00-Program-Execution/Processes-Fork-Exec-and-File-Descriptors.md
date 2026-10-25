---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Processes, Fork, Exec, and File Descriptors

> [!summary] Scope
> Owns process creation mechanisms including fork, vfork, clone3, execve, and posix_spawn.
> Covers kernel copy-on-write page table semantics, multithreaded fork async-signal safety deadlocks, interpreter shebang execution, and file descriptor table inheritance under FD_CLOEXEC.
> Reaches Use through Decide at scale, handing signal delivery to Signals-Process-Groups-Sessions-and-Job-Control.md and address space layout to Virtual-Memory-Mmap-and-Process-Address-Space.md.

## Related

- Prerequisites: none
- Next: [[Signals-Process-Groups-Sessions-and-Job-Control]]
- See also: [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management|P2L1 Processes and Process Management]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Processes-fork-exec-and-wait|Processes fork exec and wait]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Posix-Spawn-and-Vfork|Posix Spawn and Vfork]], [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Close-on-Exec-and-Descriptor-Inheritance|Close on Exec and Descriptor Inheritance]]
- Lab: `code/00-Program-Execution/fork_exec_fd/` (requires: linux)
