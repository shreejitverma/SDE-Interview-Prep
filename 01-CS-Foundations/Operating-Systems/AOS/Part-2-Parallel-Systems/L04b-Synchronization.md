---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: ["slides L04b; Mellor-Crummey and Scott"]
course: cs6210
part: 2
sub_lesson: L04b
lab: "[[labs/lab-05-spinlocks/README|lab-05-spinlocks]]"
papers: ["[[L04-MCS-Scalable-Synchronization]]", "[[L04-LRPC]]", "[[L04-Cache-Affinity-Scheduling]]", "[[L04-Multithreaded-Chip-Multiprocessors]]", "[[L04-Tornado]]", "[[L04-Corey]]", "[[L04-Cellular-Disco]]"]
tags: [cs6210, cs6210/lesson]
aliases: ["Synchronization"]
---

# L04b Synchronization

> [!summary] TL;DR
> To be written.

## Learning outcomes

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Motivation and the problem

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Core concepts

### Lock and barrier primitives

<!-- coverage: L04b-01 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Atomic read-modify-write instructions

<!-- coverage: L04b-02 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Scalability issues: latency, waiting time, contention

<!-- coverage: L04b-03 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Naive spinlock (spin on test-and-set)

<!-- coverage: L04b-04 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Caching spinlock (spin on read)

<!-- coverage: L04b-05 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Spinlock with delay and exponential backoff

<!-- coverage: L04b-06 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Ticket lock

<!-- coverage: L04b-07 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Array-based queuing lock (Anderson)

<!-- coverage: L04b-08 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Linked-list queuing lock (MCS)

<!-- coverage: L04b-09 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### MCS lock with only fetch-and-store

<!-- coverage: L04b-10 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Comparison of lock algorithms

<!-- coverage: L04b-11 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

### Modern descendants: Linux qspinlock

<!-- coverage: L04b-12 -->
> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Mechanisms step by step

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Worked examples

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Comparison

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Paper deep dives

- [Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors](../Papers/L04-MCS-Scalable-Synchronization.md)
- [Lightweight Remote Procedure Call](../Papers/L04-LRPC.md)
- [Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling](../Papers/L04-Cache-Affinity-Scheduling.md)
- [Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design](../Papers/L04-Multithreaded-Chip-Multiprocessors.md)
- [Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System](../Papers/L04-Tornado.md)
- [Corey: An Operating System for Many Cores](../Papers/L04-Corey.md)
- [Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors](../Papers/L04-Cellular-Disco.md)

## Modern descendants

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Pitfalls and exam traps

> [!todo] Seed
> To be written; see the coverage matrix row for sources.

## Practice

- [Practice L04](../Practice/Practice-L04.md)

## Lab

- [lab-05-spinlocks](../labs/lab-05-spinlocks/README.md): Spinlock zoo: TAS, TTAS, backoff, ticket, Anderson, and MCS in C11

## Further reading

> [!todo] Seed
> To be written; see the coverage matrix row for sources.
