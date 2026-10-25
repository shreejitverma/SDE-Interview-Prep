---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 11 - Concurrency and Parallelism in Shell

Shell makes concurrency easy to start and hard to get right.
This volume covers background jobs, bounded parallelism, IPC from the shell, races and interleaving, signal propagation, and the supervisors and schedulers that run scripts in production.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)
- [Volume 09 - Robust and Defensive Scripting](../Volume_09_Robust_and_Defensive_Scripting/README.md)

## Chapters

1. [Background Jobs and wait](Chapter_01_Background_Jobs_and_Wait.md) - Asynchronous lists, the last background PID, wait, collecting exit statuses, and avoiding zombies.
2. [Bounded Parallelism](Chapter_02_Bounded_Parallelism.md) - Job pools in pure shell, `xargs -P`, and GNU parallel.
3. [GNU parallel in Depth](Chapter_03_GNU_Parallel_in_Depth.md) - Input sources, replacement strings, `--joblog`, resumable runs, and remote execution.
4. [Named Pipes, Coprocesses, and Descriptor Semaphores](Chapter_04_Named_Pipes_Coprocesses_and_Descriptor_Semaphores.md) - FIFOs for IPC, coproc, and tokens in a pipe used as a counting semaphore.
5. [Races, Interleaving, and Atomicity](Chapter_05_Races_Interleaving_and_Atomicity.md) - Output interleaving, `PIPE_BUF`, append semantics, and time-of-check to time-of-use races.
6. [Signals in Concurrent Scripts](Chapter_06_Signals_in_Concurrent_Scripts.md) - Propagating signals to children, process groups, and graceful shutdown.
7. [Process Supervision: systemd and launchd](Chapter_07_Process_Supervision_systemd_and_launchd.md) - Units and property lists, restart policies, and logging.
8. [Scheduling: cron, systemd Timers, and launchd](Chapter_08_Scheduling_cron_systemd_Timers_and_launchd.md) - Environment differences, preventing overlapping runs, and handling missed runs.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v11-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
