---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
course: cs6210
tags: [cs6210]
---

# Part 2 - Parallel Systems

Lesson 4: shared memory machines, scalable locks and barriers, lightweight RPC, multiprocessor scheduling, and OS structures for many cores (Tornado, Corey, Cellular Disco).

| Note | Concepts | Lab |
| --- | ---: | --- |
| [L04a Shared Memory Machines](L04a-Shared-Memory-Machines.md) | 11 | [lab-04-cache-coherence](../labs/lab-04-cache-coherence/README.md) |
| [L04b Synchronization](L04b-Synchronization.md) | 12 | [lab-05-spinlocks](../labs/lab-05-spinlocks/README.md) |
| [L04c Barrier Synchronization](L04c-Barrier-Synchronization.md) | 7 | [lab-06-barriers](../labs/lab-06-barriers/README.md) |
| [L04d Lightweight RPC](L04d-Lightweight-RPC.md) | 6 | [lab-07-rpc-costs](../labs/lab-07-rpc-costs/README.md) |
| [L04e Scheduling](L04e-Scheduling.md) | 11 | [lab-08-scheduling](../labs/lab-08-scheduling/README.md) |
| [L04f Shared Memory Multiprocessor OS](L04f-Shared-Memory-Multiprocessor-OS.md) | 15 | [lab-09-scalable-structures](../labs/lab-09-scalable-structures/README.md) |

## Papers

- [Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors](../Papers/L04-MCS-Scalable-Synchronization.md) - TOCS 1991, required
- [Lightweight Remote Procedure Call](../Papers/L04-LRPC.md) - TOCS 1990, required
- [Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling](../Papers/L04-Cache-Affinity-Scheduling.md) - IEEE TPDS 1993, partial
- [Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design](../Papers/L04-Multithreaded-Chip-Multiprocessors.md) - USENIX ATC 2005, required
- [Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System](../Papers/L04-Tornado.md) - OSDI 1999, required
- [Corey: An Operating System for Many Cores](../Papers/L04-Corey.md) - OSDI 2008, partial
- [Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors](../Papers/L04-Cellular-Disco.md) - SOSP 1999, partial

## Practice

- [Practice L04](../Practice/Practice-L04.md)
