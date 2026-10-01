---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/77648.77650"]
course: cs6210
lesson: L04
reading: required
venue: "TOCS 1990"
authors: ["Brian N. Bershad", "Thomas E. Anderson", "Edward D. Lazowska", "Henry M. Levy"]
tags: [cs6210, cs6210/paper]
aliases: ["Lightweight Remote Procedure Call"]
---

# Lightweight Remote Procedure Call

TOCS 1990. Reading status: required. [Link](https://doi.org/10.1145/77648.77650).

> [!abstract] One-line summary
> Introduces a lightweight remote procedure call facility optimized for rapid cross-domain communication on the same machine.

## Problem

Traditional Remote Procedure Call systems impose a high overhead when used for communication between protection domains on the same machine.
This overhead stems from redundant data copying, heavy context switches, and scheduling inefficiencies.
This overhead forces developers to bundle subsystems into monolithic kernels, trading safety for performance.

## Key idea

Since the vast majority of Remote Procedure Call traffic occurs between domains on the same machine and transfers small amounts of data, the system should optimize this common path by combining the control transfer of capability systems with the programming semantics of Remote Procedure Call.
Lightweight Remote Procedure Call uses shared argument stacks and direct thread handoff to bypass the kernel message-passing layers, drastically reducing the cost of cross-domain calls while preserving the safety of separate protection domains.

## Design

Lightweight Remote Procedure Call utilizes four key optimizations.
First, simple control transfer allows the client's thread to execute the requested procedure directly in the server's domain via kernel mediation.
Second, simple data transfer relies on a shared argument stack mapped read-write in both domains to eliminate redundant data copying.
Third, it employs simple, highly optimized assembly stubs for the common case.
Fourth, for concurrency, it uses idle processor caching to avoid full context switches on multiprocessors and minimizes shared data locking on the critical path.

## Evaluation

Evaluated in the Taos operating system on the DEC SRC Firefly multiprocessor.
A simple cross-domain call using Lightweight Remote Procedure Call took 157 microseconds on a single C-VAX processor.
In contrast, the native SRC Remote Procedure Call took 464 microseconds for the same operation.
Lightweight Remote Procedure Call added only 48 microseconds of overhead to the theoretical hardware minimum for a cross-domain call.

## Limitations and critiques

The system requires a shared memory environment to operate efficiently, making it unsuitable for actual network communication without falling back to a heavier Remote Procedure Call layer.
Managing the shared argument stacks dynamically introduces some complexity.
Large arguments might still incur high costs unless passed by reference.

## What it led to

Lightweight Remote Procedure Call fundamentally shaped how local inter-process communication is handled in microkernels and modern operating systems.
The design heavily influenced systems like Mach and L4, proving that local message passing can approach the speed of a raw system call, thereby validating the microkernel architecture.

## Exam angles

> [!question]- What percentage of Remote Procedure Call traffic typically occurs on the same machine versus across a network, according to the study?
> The study observed that cross-machine operations accounted for only a small percentage (around 5 percent in Taos) of total Remote Procedure Call activity, meaning the vast majority of calls were local.

> [!question]- How does Lightweight Remote Procedure Call eliminate redundant data copying compared to traditional Remote Procedure Call?
> It uses an argument stack that is shared and mapped read-write into both the client and server domains.
> This allows the client to push arguments once, and the server can read them directly without the kernel needing to copy messages between separate address spaces.

> [!question]- Describe the mechanism by which Lightweight Remote Procedure Call reduces context-switch overhead on a multiprocessor.
> The kernel caches domains on idle processors.
> When a call is made, the kernel can exchange the processor of the calling thread with an idle processor that already has the server's context loaded, avoiding the need to flush and reload virtual memory mappings.

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
