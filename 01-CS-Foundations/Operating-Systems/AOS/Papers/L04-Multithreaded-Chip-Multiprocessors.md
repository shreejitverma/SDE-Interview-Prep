---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/2005-usenix-annual-technical-conference/performance-multithreaded-chip-multiprocessors-and"]
course: cs6210
lesson: L04
reading: required
venue: "USENIX ATC 2005"
authors: ["Alexandra Fedorova", "Margo Seltzer", "Christopher Small", "Daniel Nussbaum"]
tags: [cs6210, cs6210/paper]
aliases: ["Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design"]
---

# Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design

USENIX ATC 2005. Reading status: required. [Link](https://www.usenix.org/conference/2005-usenix-annual-technical-conference/performance-multithreaded-chip-multiprocessors-and).

> [!abstract] One-line summary
> Balance-set scheduling mitigates L2 cache contention on chip multiprocessors by co-scheduling threads that share the cache amiably.

## Problem

Multiple hardware threads share on-chip caches on multithreaded chip multiprocessors.
If threads sharing a cache have conflicting working sets or high cache miss ratios, they will thrash the cache and degrade overall performance.
The performance penalty of L2 cache misses is particularly severe, making contention for the shared L2 cache the primary bottleneck for multithreaded performance.

## Key idea

The operating system scheduler should be L2-cache-conscious by managing how threads share the cache on a chip multiprocessor.
Using a technique called balance-set scheduling, the operating system groups runnable threads into subsets such that the combined cache miss ratio of each group fits well within the shared L2 cache.
By co-scheduling threads that share the cache amiably, the overall instruction throughput increases and cache miss ratios decrease.
The scheduler selects a threshold and ensures that every runnable thread is part of at least one group whose combined miss ratio falls below the threshold, preventing starvation.

## Design

The scheduler uses the Berg-Hagersten model to estimate the cache miss ratio of a group of threads by monitoring their memory re-use patterns.
The scheduler computes an estimated cache miss ratio for all possible thread groups and selects a cache miss ratio threshold.
Thread groups whose estimated cache miss ratio falls below the threshold are scheduled together for a time slice.
The threshold is chosen as the lowest value that still ensures every runnable thread is included in at least one scheduled group.

## Evaluation

The system was simulated on a multithreaded architecture using multi-process SPEC CPU2000 workloads.
Balance-set scheduling reduced L2 cache miss ratios by 25 to 37 percent compared to the default Solaris scheduler.
Overall instructions per cycle improved by 27 percent with a 384KB L2 cache to 45 percent with a 48KB L2 cache.

## Limitations and critiques

Estimating cache miss ratios dynamically via memory monitoring is very expensive in a real operating system.
Evaluating all possible combinations of threads does not scale well as the number of runnable threads increases.
The scheme assumes a diverse workload, but if all threads are highly memory-intensive, no good grouping may exist.

## What it led to

It raised early awareness that operating system schedulers must be topology-aware and shared-resource-aware on multicore processors.
This paved the way for cache-aware and NUMA-aware scheduling in modern operating systems like Linux.

## Exam angles

<details>
<summary>What hardware trend motivates balance-set scheduling for chip multiprocessors?</summary>
Multiple hardware threads share an L2 cache that cannot grow infinitely due to silicon constraints.
The performance penalty of L2 misses dominates, making contention for the shared L2 cache the primary bottleneck for multithreaded performance.
</details>

<details>
<summary>How does the balance-set scheduler prevent starvation while optimizing for cache miss ratios?</summary>
It selects a cache miss ratio threshold such that every runnable thread is part of at least one group whose combined miss ratio falls below the threshold.
Among all such valid thresholds, it picks the lowest one.
</details>

<details>
<summary>Why is the Berg-Hagersten model used instead of a simple working-set size metric?</summary>
Working-set size alone is a poor indicator of cache behavior because the reuse pattern of memory locations matters more.
The Berg-Hagersten model accounts for memory re-use distances to better estimate actual cache miss ratios.
</details>

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
