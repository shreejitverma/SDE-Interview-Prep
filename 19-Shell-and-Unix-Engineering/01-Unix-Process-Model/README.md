---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# The Unix Process Model

Every shell command is a sequence of system calls: fork or posix_spawn, exec, descriptor plumbing, signals, and wait.
These notes build that model from the kernel interface up, so every other note can explain shell behavior by naming the system call that causes it.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Processes: fork, exec, and wait](Processes-fork-exec-and-wait.md)
2. [posix_spawn and vfork](Posix-Spawn-and-Vfork.md)
3. [Program Startup: argv, envp, and ARG_MAX](Program-Startup-argv-envp-and-ARG_MAX.md)
4. [File Descriptors and Open File Descriptions](File-Descriptors-and-Open-File-Descriptions.md)
5. [Close-on-Exec and Descriptor Inheritance](Close-on-Exec-and-Descriptor-Inheritance.md)
6. [Pipes and FIFOs](Pipes-and-FIFOs.md)
7. [PIPE_BUF and Write Atomicity](PIPE_BUF-and-Write-Atomicity.md)
8. [SIGPIPE and EPIPE](SIGPIPE-and-EPIPE.md)
9. [Exit Status and Wait Status](Exit-Status-and-Wait-Status.md)
10. [Signals: Dispositions, Masks, and Delivery](Signals-Dispositions-Masks-and-Delivery.md)
11. [Async Signal Safety](Async-Signal-Safety.md)
12. [Sessions Process Groups and Controlling Terminals](Sessions-Process-Groups-and-Controlling-Terminals.md)
13. [Daemonization](Daemonization.md)
14. [Zombies and Orphans](Zombies-and-Orphans.md)
15. [The /proc Filesystem](The-proc-Filesystem.md)
16. [/dev/fd and /dev/stdin](dev-fd-and-dev-stdin.md)
17. [Environment Variables and Inheritance](Environment-Variables-and-Inheritance.md)
18. [Resource Limits and ulimit](Resource-Limits-and-ulimit.md)
