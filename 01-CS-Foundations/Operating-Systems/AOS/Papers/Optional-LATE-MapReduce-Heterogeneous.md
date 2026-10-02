---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi-08/improving-mapreduce-performance-heterogeneous-environments"]
course: cs6210
lesson: optional
reading: optional
venue: "OSDI 2008"
authors: ["Matei Zaharia", "Andy Konwinski", "Anthony D. Joseph", "Randy Katz", "Ion Stoica"]
tags: [cs6210, cs6210/paper]
aliases: ["Improving MapReduce Performance in Heterogeneous Environments"]
---

# Improving MapReduce Performance in Heterogeneous Environments

OSDI 2008.
Reading status: optional.
[Link](https://www.usenix.org/conference/osdi-08/improving-mapreduce-performance-heterogeneous-environments).

> [!abstract] One-line summary
> The LATE scheduler improves Hadoop MapReduce response times in heterogeneous environments by speculatively executing tasks that will finish furthest in the future.

## Problem

Hadoop's native scheduler assumed a homogeneous cluster environment and used a simple progress score to identify straggler tasks.
In heterogeneous environments, such as virtualized cloud data centers like Amazon EC2, this assumption broke down, causing the scheduler to launch excessive and incorrect speculative tasks that severely degraded performance.

## Key idea

Introduce the LATE (Longest Approximate Time to End) scheduler, which estimates task completion times and speculatively executes the tasks that are projected to finish furthest into the future.
This prioritizes the true stragglers that actually delay the job, bounds the total number of speculative tasks, and ensures they run on fast nodes.

## Design

LATE estimates the time remaining for a task based on its progress rate, calculated as the progress score divided by execution time.
It limits resource contention by enforcing a hard cap on the number of concurrent speculative tasks.
It prevents launching speculative tasks on slow nodes by checking against a slow node threshold.
Furthermore, it only speculates tasks that are progressing slower than a specific task threshold, ignoring fast tasks.

## Evaluation

The authors evaluated LATE on Amazon EC2 using up to 243 virtual machines and on a local testbed.
LATE improved Hadoop job response times by a factor of 2 in a 200-VM EC2 cluster compared to Hadoop's native scheduler.
It also drastically reduced the number of speculative tasks, dropping an excessive 80 percent speculation rate down to a bounded and useful level.

## Limitations and critiques

The progress rate heuristic assumes that a task progresses at a constant speed throughout its lifetime.
If a task is fast in its first phase but slow in its second phase, LATE might incorrectly estimate its completion time.
Additionally, LATE does not account for data locality when launching speculative map tasks, which could impact network utilization.

## What it led to

LATE became the foundation for scheduling logic in modern Hadoop distributions to effectively handle stragglers.
It highlighted the significant performance variances present in utility computing environments and sparked widespread research into cloud-aware and heterogeneity-aware task scheduling.

## Exam angles

<details>
<summary>Why does Hadoop's native speculative execution fail in heterogeneous environments?</summary>
It assumes all nodes have equal processing capacity.
It launches backup tasks for any task slightly slower than the average, leading to too many speculative tasks that compete for shared resources, and it often mistakenly launches them on slow nodes.
</details>

<details>
<summary>How does the LATE scheduler estimate which task to speculatively execute?</summary>
It calculates the progress rate of each task and estimates the time remaining.
It then prioritizes the task with the longest approximate time to end, ensuring that speculation targets the specific tasks that delay the overall job the most.
</details>

<details>
<summary>Why does LATE place a cap on the number of speculative tasks?</summary>
It caps them to prevent system thrashing and resource contention.
Since speculative tasks consume CPU, disk, and network bandwidth, launching too many can slow down the useful tasks and degrade the overall performance of the cluster.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
