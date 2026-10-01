---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/conference/osdi-08/corey-operating-system-many-cores"]
course: cs6210
lesson: L04
reading: partial
venue: "OSDI 2008"
authors: ["Silas Boyd-Wickizer", "Haibo Chen", "Rong Chen", "Yandong Mao", "Frans Kaashoek", "Robert Morris", "Aleksey Pesterev", "Lex Stein", "Ming Wu", "Yuehua Dai", "Yang Zhang", "Zheng Zhang"]
tags: [cs6210, cs6210/paper]
aliases: ["Corey: An Operating System for Many Cores"]
---

# Corey: An Operating System for Many Cores

OSDI 2008. Reading status: partial (assumed required sections 1-4 and 8). [Link](https://www.usenix.org/conference/osdi-08/corey-operating-system-many-cores).

> [!abstract] One-line summary
> Corey scales on many-core processors by providing abstractions that allow applications to explicitly control the sharing of operating system data structures.

## Problem

Multiprocessor application performance is limited when the operating system uses shared data structures for operations, causing cache contention and lock overhead even when the application access patterns do not require sharing.
Typical operating systems force all-or-nothing sharing semantics, such as a single shared address space for threads, which scales poorly as core counts increase.

## Key idea

Applications should have explicit control over the sharing of operating system data structures to avoid unnecessary cache contention and lock overhead.
The kernel should arrange its data structures so that only a single processor updates them by default, unless the application dictates otherwise.
The system provides novel abstractions including address ranges, kernel cores, and shares, which allow applications to limit sharing to only the cores that actually need access, thereby scaling efficiently on many-core hardware.

## Design

Address ranges allow applications to decouple address space sharing.
An application can map private memory to a core-private address range to avoid lock contention and TLB shootdowns, while mapping shared memory to a shared address range.
Kernel cores allow applications to dedicate specific cores to run specific kernel functions, such as network device polling.
Other cores communicate with the kernel core via shared-memory IPC, eliminating lock contention on device driver state.
Shares act as lookup tables for kernel objects whose scope is application-defined.
A core can use a private root share for private objects to avoid locking, or create a shared share for objects that must be visible to other cores.

## Evaluation

The system was prototyped as an exokernel-like operating system on 16-core AMD Opteron and Intel Xeon machines.
On a 16-core MapReduce application, Corey performed 25 percent faster than Linux because address ranges eliminated soft page faults and contention during the map and reduce phases.
A Web server benchmark saturated the network device with only 5 cores by dedicating one kernel core to the NIC, whereas Linux-like shared driver approaches required 11 cores due to lock contention.

## Limitations and critiques

The design places a significant burden on application developers or runtime library developers to explicitly manage and tune resource sharing to achieve performance.
Dedicating a kernel core wastes CPU resources if the dedicated kernel task does not fully saturate the cycles of the core.
The exokernel prototype lacked a full POSIX environment, meaning porting legacy applications could be difficult or would re-introduce the very sharing bottlenecks Corey aims to eliminate.

## What it led to

It highlighted that POSIX semantics inherently limit multicore scalability due to mandated sharing.
It influenced the design of scalable operating system APIs and runtimes that give data-plane control directly to applications or bypass the kernel entirely.

## Exam angles

<details>
<summary>How do address ranges in Corey resolve the conflict between the map phase and reduce phase of a MapReduce application?</summary>
In the map phase, threads use private address ranges, allowing them to map newly allocated memory without contention.
In the reduce phase, they access intermediate results mapped in shared address ranges, avoiding the soft page faults that would occur if entirely separate address spaces were used.
</details>

<details>
<summary>What is the advantage of using a kernel core in Corey?</summary>
By dedicating a single core to execute specific operating system functions, other cores can interact with it via lock-free shared memory IPC.
This avoids cache line bouncing and lock contention over shared driver data structures.
</details>

<details>
<summary>How do shares in Corey contrast with traditional Unix file descriptor tables?</summary>
In Unix, the file descriptor table is process-wide, so any creation involves locking a shared table.
In Corey, a share is a namespace whose visibility is defined by the application.
An application can place an object in a private share with no locks or a shared share only when other cores actually need access.
</details>

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
