---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/74850.74859"]
course: cs6210
lesson: L05
reading: partial
venue: "SOSP 1989"
authors: ["Michael D. Schroeder", "Michael Burrows"]
tags: [cs6210, cs6210/paper]
aliases: ["Performance of the Firefly RPC"]
---

# Performance of the Firefly RPC

SOSP 1989. Reading status: partial. [Link](https://doi.org/10.1145/74850.74859).
The syllabus requires Sections 1 through 4.

> [!abstract] One-line summary
> The Firefly RPC system demonstrates that remote procedure calls can achieve near-hardware performance limits by meticulously optimizing the fast path and tightly integrating the OS and network interface.

## Problem

Early remote procedure call implementations were notoriously slow compared to local procedure calls.
This high latency led distributed system developers to avoid RPC for performance-critical inter-machine communication.
Many researchers assumed that this high latency was an unavoidable consequence of network communication and rigid protocol layering.
There was a need to prove that software overhead could be minimized enough to make RPC practical for fine-grained distributed computing.

## Key idea

The Firefly RPC system demonstrates that RPC can be made extremely fast by meticulously optimizing the fast path, which is the common case of a successful, single-packet remote call and return.
By carefully co-designing the RPC software, the operating system, and the network interface, developers can avoid unnecessary context switches and data copies.
This deep integration reduces software latency to near the theoretical minimum allowed by the hardware.
The work proves that poor performance is not an inherent flaw of the RPC paradigm but rather a result of sub-optimal implementation.

## Design

The design relies on custom user-level threads that are tightly integrated with the RPC mechanism.
The fast path for communication is hand-optimized in assembly language to shave off unnecessary cycles.
Shared memory between the network interface and the CPU minimizes expensive data copying operations.
When a network packet arrives, the interrupt handler directly invokes the waiting server thread, bypassing the standard OS scheduler completely.
Data marshaling is streamlined by generating highly optimized stub code tailored to specific argument types.

## Evaluation

The system was evaluated on DEC Firefly multiprocessor workstations connected by a standard 10 Mbps Ethernet network.
A null RPC took only 2.66 milliseconds round-trip, which was exceptionally fast for contemporary hardware.
The system achieved over 1 megabyte per second in throughput for bulk data transfers.
The actual transmission time on the wire accounted for a significant fraction of the total delay, proving that the software overhead was minimal.

## Limitations and critiques

The design was highly customized to the specific Firefly hardware architecture and the Taos operating system.
This deep integration made the optimizations very difficult to port to more generalized or standard operating systems.
The optimizations focused almost exclusively on the fast path.
Exception handling and multi-packet complex calls did not benefit from the same aggressive optimizations and could still suffer performance penalties.

## What it led to

Firefly set a new benchmark for RPC performance and proved that software overhead does not have to be the bottleneck.
The techniques influenced the design of future high-performance communication frameworks and modern microkernels.
The principles of zero-copy networking and scheduler bypass are now standard concepts in high-performance networking stacks.

## Exam angles

> [!question]- What is the fast path in Firefly RPC and why is it important?
> The fast path is the execution sequence for the most common case, such as a simple RPC that fits in a single packet and encounters no errors.
> Optimizing this path yields the most significant performance improvements for typical application workloads.

> [!question]- How did Firefly RPC reduce context switching overhead?
> When a network packet for an RPC arrives, the interrupt handler directly identifies the waiting server thread.
> It then hands the packet directly to that thread, bypassing the heavier OS scheduler queue entirely.

## Related

- Lessons: [L05a](../Part-3-Distributed-Systems/L05a-Distributed-Systems-Definitions.md), [L05b](../Part-3-Distributed-Systems/L05b-Lamport-Clocks.md), [L05c](../Part-3-Distributed-Systems/L05c-Latency-Limits.md), [L05d](../Part-3-Distributed-Systems/L05d-Active-Networks.md), [L05e](../Part-3-Distributed-Systems/L05e-Systems-from-Components.md)
