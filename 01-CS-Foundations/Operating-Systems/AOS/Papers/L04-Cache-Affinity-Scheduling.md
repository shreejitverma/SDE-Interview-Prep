---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1109/71.207589"]
course: cs6210
lesson: L04
reading: partial
venue: "IEEE TPDS 1993"
authors: ["Mark S. Squillante", "Edward D. Lazowska"]
tags: [cs6210, cs6210/paper]
aliases: ["Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling"]
---

# Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling

IEEE TPDS 1993. Reading status: partial. See the syllabus for the required sections. [Link](https://doi.org/10.1109/71.207589).

> [!abstract] One-line summary
> Analyzes and proposes processor scheduling policies that utilize cache affinity to improve performance on shared-memory multiprocessors.

## Problem

On shared-memory multiprocessors with per-processor caches, executing a thread on a processor where it recently ran can be beneficial because its data may still reside in the cache.
Traditional multiprocessor schedulers often ignore this processor-cache affinity, leading to high cache miss rates and degraded performance when threads are migrated arbitrarily across processors.

## Key idea

The scheduler should incorporate cache affinity by preferentially dispatching a thread to the processor on which it previously executed.
By tracking the footprint of a thread and the time elapsed since it last ran on a processor, the operating system can balance the benefits of cache reuse against the potential idle time caused by waiting for a specific processor.
The provided source text for this paper was completely empty and unreadable, so this note relies on existing knowledge of the topic.

## Design

The authors use detailed queuing network models and trace-driven simulations to evaluate different scheduling policies.
They compare a baseline policy with no affinity to policies that delay a thread's execution to wait for its preferred processor.
The design tracks thread footprints and models cache invalidations caused by intervening threads to estimate the residual cache state.

## Evaluation

Evaluated using simulation models driven by workload traces.
The results demonstrated that affinity-based scheduling significantly reduces cache miss rates and improves overall system throughput.
The performance gains are highly dependent on the cache size, the workload's cache reload transient time, and the multiprocessor's memory latency.

## Limitations and critiques

The strict enforcement of cache affinity can lead to load imbalance if many threads favor a single processor while other processors sit idle.
The optimal delay threshold before migrating a thread is difficult to determine dynamically and depends heavily on specific architectural parameters and workload characteristics.

## What it led to

Cache affinity scheduling became a standard feature in almost all modern multiprocessor and multicore operating systems.
Techniques for soft processor affinity and hierarchical scheduling domains evolved directly from these early analyses of cache reuse.

## Exam angles

> [!question]- What is processor-cache affinity and why does it matter in shared-memory multiprocessors?
> It is the tendency of a thread to run more efficiently on a processor where it previously executed, because its data and instructions are already resident in that processor's cache.
> Ignoring this affinity causes unnecessary cache misses and increases memory bus traffic.

> [!question]- What is the primary trade-off when deciding whether to schedule a thread on its preferred processor versus an idle processor?
> The scheduler must balance the time saved by reusing cached data against the time wasted if the thread sits idle waiting for its preferred processor to become available.

> [!question]- How do cache size and memory latency influence the effectiveness of cache affinity scheduling?
> Larger caches increase the likelihood that a thread's footprint remains intact after an interruption, making affinity more valuable.
> Higher memory latency increases the penalty for cache misses, further amplifying the performance benefits of affinity scheduling.

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
