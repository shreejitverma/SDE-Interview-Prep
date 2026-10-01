---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi-99/tornado-maximizing-locality-and-concurrency-shared-memory-multiprocessor-operating"]
course: cs6210
lesson: L04
reading: required
venue: "OSDI 1999"
authors: ["Ben Gamsa", "Orran Krieger", "Jonathan Appavoo", "Michael Stumm"]
tags: [cs6210, cs6210/paper]
aliases: ["Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System"]
---

# Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System

OSDI 1999. Reading status: required. [Link](https://www.usenix.org/conference/osdi-99/tornado-maximizing-locality-and-concurrency-shared-memory-multiprocessor-operating).

> [!abstract] One-line summary
> Tornado uses an object-oriented structure and clustered objects to maximize locality and eliminate shared data contention on multiprocessors.

## Problem

Traditional symmetric multiprocessor operating systems evolved from uniprocessor designs and use global locks and shared data structures.
On modern shared-memory multiprocessors, this causes severe performance degradation due to cache coherence overhead, false sharing, and high memory latency when handling concurrent requests for independent resources.
Even when concurrency is high, poor locality destroys performance.

## Key idea

To achieve high performance on shared-memory multiprocessors, the operating system must map the locality and independence of application requests directly onto the servicing of those requests.
This is achieved by using an object-oriented structure where every resource is an independent object, minimizing global state and shared locks.
The system introduces clustered objects that partition a single logical object into multiple representatives across processors, allowing local requests to be processed independently without cache coherence overhead.

## Design

Every resource, such as a thread or memory region, is a distinct object, minimizing global state.
A clustered object is accessed via a single reference but is implemented as a collection of localized representatives partitioned across processors.
Local per-processor translation tables route method calls to the local representative, and miss handlers instantiate representatives on demand.
Locks are completely internal to object representatives.
A semi-automatic garbage collection scheme manages object lifetimes, eliminating the need for external existence locks to protect object references.
The protected procedure call facility ensures that requests to a server object are serviced on the same processor as the client.

## Evaluation

The system was implemented on the 16-processor NUMAchine hardware and the SimOS simulator.
Microbenchmarks of in-core page faults and thread creations showed Tornado scaling linearly with constant cost up to 16 processors.
In contrast, commercial operating systems like AIX, Solaris, and IRIX suffered an order-of-magnitude slowdown.

## Limitations and critiques

The programming model is highly complex because developers must explicitly manage clustered objects and miss handlers.
Operations that require global consensus or modifications across all representatives become more expensive due to the need to coordinate dispersed state.
The architecture assumes a cache-coherent shared-memory hardware model, though its principles map somewhat to message-passing systems.

## What it led to

It strongly influenced the design of subsequent multikernel operating systems like Barrelfish and inspired K42 at IBM.
It directly demonstrated that avoiding shared state is the only path to scalability on many-core systems.

## Exam angles

<details>
<summary>Explain the purpose and implementation of a clustered object in Tornado.</summary>
A clustered object presents the illusion of a single object but is internally composed of multiple representatives partitioned across processors.
A per-processor translation table directs method invocations to the local representative, allowing independent local requests to be processed without global locks or cache misses.
</details>

<details>
<summary>How does Tornado's locking strategy avoid the need for global existence locks?</summary>
It uses a semi-automatic garbage collection scheme based on temporary and persistent references, combined with epochs for quiescent states.
Objects are only destroyed when no processors hold temporary references to them, so threads do not need to lock an object merely to guarantee it will not disappear.
</details>

<details>
<summary>In Tornado, why does creating a thread or taking an in-core page fault scale perfectly compared to commercial multiprocessor operating systems?</summary>
Because all data structures are localized and partitioned using clustered objects, and the protected procedure call mechanism ensures that the kernel request executes on the same processor that issued it, avoiding shared global queues and locks.
</details>

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
