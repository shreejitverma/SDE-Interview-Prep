---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/168619.168631"]
course: cs6210
lesson: L08
reading: required
venue: "SOSP 1993"
authors: [M. Satyanarayanan, Henry H. Mashburn, Puneet Kumar, David C. Steere, James J. Kistler]
tags: [cs6210, cs6210/paper]
aliases: ["Lightweight Recoverable Virtual Memory"]
---

# Lightweight Recoverable Virtual Memory

SOSP 1993. Reading status: required. [Link](https://doi.org/10.1145/168619.168631).

> [!abstract] One-line summary
> A user-level library that provides lightweight transactional properties for regions of virtual memory by decoupling atomicity and permanence from serializability and operating system specifics.

## Problem

Traditional transactional facilities like Camelot impose high overheads, strict programming constraints, and complex OS dependencies (such as Mach's external pager). This makes them unsuitable for applications that need simple, efficient fault-tolerance for metadata without the burden of nested, distributed, or serializable transactions.

## Key idea

Provide a minimalist, user-level library called RVM that supports only essential transactional properties (atomicity and permanence) for declared regions of virtual memory. By factoring out concurrency control, nesting, and distributed commit, RVM achieves high portability and low overhead, allowing applications to build higher-level abstractions only if needed.

## Design

RVM operates entirely at user level and assigns a separate write-ahead redo log to each application. Applications explicitly declare regions of memory to modify using a `set_range` operation. RVM copies the before-image to an in-memory undo log to handle aborts without requiring disk reads. Upon commit, modifications are appended synchronously to the on-disk redo log. The external data segment on disk is updated asynchronously during log truncation. RVM does not integrate tightly with the OS virtual memory subsystem, avoiding double-paging complexities by keeping backing store management simple.

## Evaluation

RVM was evaluated on IBM RT and DECstation hardware. It achieved sub-millisecond transaction overheads for small transactions, a dramatic improvement over Camelot's multi-millisecond latency. By replacing Camelot with RVM in the Coda file system, the authors observed significant reductions in CPU utilization, paging, and context switching, validating the minimalist design.

## Limitations and critiques

The primary limitation is that the working set of recoverable memory must fit entirely within main memory to maintain performance. Because RVM does not tightly integrate with the OS virtual memory pager, heavily swapping applications will suffer severe performance degradation. Furthermore, leaving concurrency control to the application increases the burden on developers who require strict serializability.

## What it led to

RVM demonstrated the viability and performance benefits of decoupling transactional semantics in systems software. It influenced subsequent research in persistent memory and operating systems, directly paving the way for even lighter-weight transaction systems like Rio Vista.

## Exam angles

<details>
<summary>Why did the authors of RVM abandon Camelot's tight integration with the OS VM subsystem?</summary>
Tight integration relying on Mach's external pager introduced significant complexity, poor portability, and high overhead from interprocess communication and context switching. By decoupling from the VM, RVM achieved simplicity and performance at the cost of assuming the working set fits in RAM.
</details>

<details>
<summary>How does RVM handle transaction aborts without an on-disk undo log?</summary>
RVM maintains an in-memory undo log containing the before-images of modified memory regions. Because aborts are handled in-memory and undo records are discarded upon commit, there is no need for synchronous disk I/O to write undo records.
</details>

<details>
<summary>Why does RVM leave concurrency control up to the application?</summary>
Factoring out concurrency control keeps the RVM library simple and allows applications to implement synchronization at a granularity and policy appropriate for their specific abstractions, rather than paying the cost of a general-purpose serializability mechanism.
</details>

## Related

- Lessons: [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md), [L08c](../Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md)
