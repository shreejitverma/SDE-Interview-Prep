---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Concurrency and Scheduling

Shell makes concurrency easy to start and hard to get right.
These notes cover background jobs, bounded parallelism, IPC, races, signal propagation, and the schedulers that run scripts.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Background Jobs and wait](Background-Jobs-and-wait.md)
2. [Bounded Job Pools](Bounded-Job-Pools.md)
3. [xargs -P](xargs-P.md)
4. [GNU parallel](GNU-parallel.md)
5. [Named Pipes for IPC](Named-Pipes-for-IPC.md)
6. [Descriptor Semaphores](Descriptor-Semaphores.md)
7. [Output Interleaving and Append Atomicity](Output-Interleaving-and-Append-Atomicity.md)
8. [Signal Propagation to Children](Signal-Propagation-to-Children.md)
9. [Graceful Shutdown](Graceful-Shutdown.md)
10. [cron](cron.md)
11. [systemd Timers](systemd-Timers.md)
12. [launchd](launchd.md)
13. [Overlap Prevention and Missed Runs](Overlap-Prevention-and-Missed-Runs.md)
