---
type: concept
track: [sde, distinguished, low-latency]
level:
status: seed
last_reviewed:
sources: []
---

# Pipe Buffering and Stdbuf

> [!summary] Scope
> Owns I/O buffering interactions across pipeline stages, contrasting kernel pipe ring buffers (default 64 KB on Linux) with libc stdio buffer modes (unbuffered, line-buffered, and 4-8 KB block-buffered).
> Analyzes inter-process pipeline deadlocks and latency lag in interactive streaming commands.
> Details dynamic library interposition via stdbuf (libstdbuf.so) and pty wrapping to force line-buffering.
> Reaches Use through Debug, handing pty kernel mechanics to 00-Program-Execution/TTYs-PTYs-and-Line-Discipline.md.

## Related

- Prerequisites: [[Pipelines-Subshells-Redirection-and-Process-Substitution]]
- Next: [[POSIX-sh-Bash-Zsh-and-Fish-Compared]]
- See also: [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/PIPE_BUF-and-Write-Atomicity|PIPE_BUF and Write Atomicity]], [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/stdio-Buffering-and-stdbuf|stdio Buffering and stdbuf]]
- Lab: `code/01-Command-Line/pipe_buffering_stdbuf/` (requires: linux)
