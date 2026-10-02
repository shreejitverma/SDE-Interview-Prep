---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/1629575.1629579"]
course: cs6210
lesson: optional
reading: optional
venue: "SOSP 2009"
authors: ["Andrew Baumann", "Paul Barham", "Pierre-Evariste Dagand", "Tim Harris", "Rebecca Isaacs", "Simon Peter", "Timothy Roscoe", "Adrian Schüpbach", "Akhilesh Singhania"]
tags: [cs6210, cs6210/paper]
aliases: ["The Multikernel: A New OS Architecture for Scalable Multicore Systems"]
---

# The Multikernel: A New OS Architecture for Scalable Multicore Systems

SOSP 2009. Reading status: optional. [Link](https://doi.org/10.1145/1629575.1629579).

> [!abstract] One-line summary
> The multikernel OS architecture treats a multicore machine as a network of independent cores communicating via explicit messages, avoiding shared memory in the OS to improve scalability and support heterogeneous hardware.

## Problem

Modern multicore systems feature rapidly increasing core counts and highly diverse architectural tradeoffs, including varied cache hierarchies, complex interconnects, varying memory consistency models, and heterogeneous cores.
Traditional monolithic operating systems, which rely heavily on shared memory data structures protected by locks, struggle to scale and adapt to this hardware diversity.
OS optimizations for shared memory are deeply intertwined with specific hardware characteristics, making them difficult to evolve and quick to become obsolete as hardware architectures change.

## Key idea

The multikernel model completely rethinks operating system structure by adopting a distributed systems approach within a single machine.
The design is guided by three core principles: make all inter-core communication explicit using message passing, make OS structure hardware-neutral to adapt to different interconnect topologies, and view global OS state as replicated rather than shared.
This approach naturally accommodates heterogeneous hardware, minimizes interconnect contention, and allows the OS to leverage well-understood distributed systems algorithms.

## Design

Barrelfish is the prototype operating system built on the multikernel model.
The OS on each core is factored into a privileged-mode CPU driver and a distinguished user-mode monitor process.
CPU drivers are purely local to a core and share absolutely no state with other cores; they handle low-level operations like trapping, scheduling, and local event delivery.
Monitors run in user space and perform inter-core communication using explicit remote procedure calls (RPCs) to manage global state through replication and agreement protocols.
The system employs split-phase, asynchronous message passing to hide interconnect latency and allow pipelining of requests.
Memory management is redesigned as a distributed capability system, utilizing a two-phase commit protocol for capability revocation.
A central System Knowledge Base (SKB), containing hardware discovery data, is queried using constraint logic programming to dynamically determine the most efficient messaging mechanisms and routing topologies.

## Evaluation

Barrelfish was evaluated on several multicore systems, including a 4x4-core AMD Opteron system and an 8x4-core AMD system, comparing its performance against commodity operating systems like Windows and Linux.
Compute performance on standard benchmarks (such as SPLASH-2) is comparable to commodity OSes.
However, Barrelfish demonstrates vastly superior scalability for OS-intensive tasks.
For example, TLB shootdown implemented via topology-aware multicast messaging scales extremely well, completing operations in roughly 2000 cycles on 16 cores, while the shared-memory OSes degraded significantly due to cache contention.
Unmap latency using the distributed two-phase commit protocol scales linearly but remains competitive or faster than Windows up to 16 cores.

## Limitations and critiques

Implementing an operating system as an event-driven, message-passing distributed system introduces significant software engineering complexities, such as "stack ripping" and making control flow much harder for developers to follow.
Maintaining replica consistency for global OS state across all cores adds communication overhead, and certain operations will inevitably experience longer latencies depending on the data volumes and the specific consistency model required.
Furthermore, strict adherence to the shared-nothing model sacrifices some platform-specific optimizations, such as exploiting a shared L2 cache between tightly coupled hardware threads.

## What it led to

Barrelfish successfully demonstrated the viability of the multikernel model, challenging the long-held assumption that general-purpose operating systems must be structured around shared memory.
It sparked significant research interest into applying distributed systems techniques to single-node operating systems, particularly concerning scalability, fault tolerance, and the management of heterogeneous system-on-chip architectures.

## Exam angles

<details>
<summary>What are the three core design principles of the multikernel architecture?</summary>
The three core principles are: (1) Make all inter-core communication explicit by using message passing instead of shared memory.
(2) Make the OS structure hardware-neutral to adapt easily to different and evolving interconnect topologies.
(3) View global OS state as replicated across cores rather than shared, using agreement protocols to maintain consistency.
</details>

<details>
<summary>How does Barrelfish manage global system state without relying on shared memory?</summary>
Barrelfish manages global state through replication and distributed agreement protocols.
User-space monitor processes on each core maintain local replicas of global OS state.
When a change is required, the monitors communicate via explicit messages to update the replicas, using techniques like two-phase commit for operations that require strong consistency across the system.
</details>

<details>
<summary>Why does a multikernel architecture argue that explicit message passing is superior to shared memory for future multicore scalability?</summary>
As core counts increase, the overhead of hardware cache coherence for shared memory becomes a massive bottleneck.
Message passing avoids this contention by localizing data access.
Furthermore, explicit messages can be pipelined and batched, hiding interconnect latency and utilizing bandwidth more efficiently than the implicit, fine-grained messages generated by cache coherence protocols on every shared memory access.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
