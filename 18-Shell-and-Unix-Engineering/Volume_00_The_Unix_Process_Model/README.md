---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 00 - The Unix Process Model Under the Shell

Every shell command is a sequence of system calls: fork or posix_spawn, exec, file descriptor plumbing, signals, and wait.
This volume builds that model from the kernel interface up, so that every later volume can explain shell behavior by naming the system call that causes it.

## Prerequisites

- None inside this curriculum; comfort with a terminal and with C-level ideas such as processes and file descriptors helps.

## Chapters

1. [Processes: fork, exec, wait, and posix_spawn](Chapter_01_Fork_Exec_Wait_and_Posix_Spawn.md) - Process creation and copy-on-write, vfork and posix_spawn, the exec family and PATH search in execvp, reaping with wait, and the case against fork in "A fork() in the road" (HotOS 2019).
2. [Program Startup: argv, envp, auxv, and ARG_MAX](Chapter_02_Program_Startup_Argv_Envp_and_ARG_MAX.md) - How the kernel lays out the initial process stack, argument and environment size limits, E2BIG, and exactly what a shell passes to every child.
3. [File Descriptors and Open File Descriptions](Chapter_03_File_Descriptors_and_Open_File_Descriptions.md) - The per-process descriptor table and the system-wide open file table, offsets shared after fork and dup, `O_CLOEXEC`, and descriptor inheritance across exec.
4. [Pipes and FIFOs](Chapter_04_Pipes_and_FIFOs.md) - Pipe buffers and capacity, `PIPE_BUF` write atomicity, blocking and end-of-file semantics, SIGPIPE and EPIPE, and named pipes.
5. [Exit Status and Wait Status](Chapter_05_Exit_Status_and_Wait_Status.md) - The wait status encoding and its macros, the 8-bit exit code, the 126, 127, and 128 plus signal conventions, and `sysexits.h`.
6. [Signals](Chapter_06_Signals_for_Shell_Programmers.md) - Dispositions, masks, and pending sets, delivery and default actions, inheritance across fork and exec, and async-signal-safety.
7. [Sessions, Process Groups, and Controlling Terminals](Chapter_07_Sessions_Process_Groups_and_Controlling_Terminals.md) - setsid and setpgid, the foreground process group, orphaned process groups, SIGHUP on hangup, and daemonization.
8. [Filesystem Semantics a Scripter Relies On](Chapter_08_Filesystem_Semantics_a_Scripter_Relies_On.md) - Inodes, hard and symbolic links, permissions and umask, atomic rename, unlink while open, fsync durability, and why Linux ignores setuid on interpreted scripts.
9. [The /proc and /dev/fd Interfaces](Chapter_09_The_proc_and_dev_fd_Interfaces.md) - Introspecting processes from the shell through `/proc/PID`, the `/dev/fd` and `/dev/stdin` files, and their macOS equivalents.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v00-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
