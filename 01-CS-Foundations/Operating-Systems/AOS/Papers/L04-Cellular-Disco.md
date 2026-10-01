---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/319151.319162"]
course: cs6210
lesson: L04
reading: partial
venue: "SOSP 1999"
authors: ["Kinshuk Govil", "Dan Teodosiu", "Yongqiang Huang", "Mendel Rosenblum"]
tags: [cs6210, cs6210/paper]
aliases: ["Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors"]
---

# Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors

SOSP 1999. Reading status: partial. The syllabus requires sections 1 (Introduction), 2 (The Cellular Disco architecture), 4 (CPU management), and 5 (Memory management). [Link](https://doi.org/10.1145/319151.319162).

> [!abstract] One-line summary
> Cellular Disco extends the Disco virtual machine monitor to provide hardware fault containment and global resource management, effectively turning a large-scale shared-memory multiprocessor into a flexible virtual cluster.

## Problem

Commercial operating systems struggle to scale to the large number of processors available in shared-memory multiprocessors, and adapting them requires significant development cost and complexity.
Existing alternatives like hardware partitioning avoid scalability bottlenecks but severely restrict resource sharing, as resources are statically divided and applications cannot burst beyond their partition's limits.
Furthermore, large-scale systems have higher failure rates, and most operating systems lack hardware fault containment, meaning a single hardware fault crashes the entire machine.

## Key idea

Rather than modifying the operating system, insert a virtual machine monitor (Cellular Disco) between the hardware and commodity operating systems to provide scalability, resource load balancing, and hardware fault containment.
By structuring the monitor internally as a set of semi-independent cells, it can contain hardware faults similar to a physical cluster.
Simultaneously, it preserves the benefits of shared-memory systems by dynamically managing CPU and memory resources globally, allowing virtual machines to overcommit resources and share memory pages without being constrained by static hardware partitions.

## Design

Cellular Disco is designed as a virtual machine monitor that multiplexes hardware resources across multiple instances of unmodified operating systems.
To achieve hardware fault containment, the monitor is internally divided into semi-independent cells, ensuring that a hardware fault in one cell only affects the virtual machines utilizing resources from that specific cell.
The monitor manages CPU scheduling using a distributed gang scheduler and implements idle and periodic load balancing to migrate virtual CPUs across the system.
Memory is managed dynamically, allowing cells running low on memory to borrow pages from other cells, with the monitor carefully tracking these allocations to balance performance with fault vulnerability.
To avoid writing complex device drivers, the prototype piggybacks on a host IRIX operating system for I/O operations, intercepting virtual machine I/O requests and forwarding them to the host kernel.
The monitor also supports a virtual paging disk to eliminate redundant paging overheads caused by the virtualized operating system and the monitor both attempting to page out the same memory.

## Evaluation

The Cellular Disco prototype was implemented in about 50,000 lines of C and assembly and evaluated on a 32-processor SGI Origin 2000 system.
The system ran four commercial workloads (Database, Pmake, Raytrace, Web server) using IRIX 6.2 on top of the monitor.
The virtualization overhead was shown to be very low, with a worst-case uniprocessor execution time penalty of only 9% and a maximum of 20% on multiprocessor configurations.
In a simulated hardware fault containment setup, virtual machines spanning cell boundaries experienced practically the same running time as those in a single cell, demonstrating that fault containment adds negligible overhead.
Compared to static hardware partitioning, Cellular Disco significantly improved resource utilization, achieving 58% CPU utilization compared to 31% for partitioning under a mixed workload.

## Limitations and critiques

The prototype implementation piggybacks on a host IRIX operating system for device drivers, meaning that the I/O subsystem remains a single point of failure and does not fully benefit from hardware fault containment.
Actual hardware fault recovery relies on the underlying machine hardware having fault containment support (like the FLASH multiprocessor), which the SGI Origin 2000 used in the primary experiments lacked.
Memory borrowing across cells increases a virtual machine's vulnerability to faults, forcing a trade-off between optimal resource load balancing and fault isolation that users must carefully configure.

## What it led to

Cellular Disco demonstrated that virtual machine monitors could provide enterprise-grade features like fault containment, dynamic resource allocation, and scalability at a fraction of the cost of rewriting an operating system.
This approach validated the commercial viability of virtualization on modern architectures.
The work heavily influenced the foundation of VMware and the development of VMware ESX Server, which brought bare-metal virtualization and dynamic resource management to industry-standard x86 servers.

## Exam angles

<details>
<summary>How does Cellular Disco provide hardware fault containment without requiring a specialized operating system?</summary>
Cellular Disco is internally structured into a set of semi-independent cells, each managing its own resources and containing a complete copy of the monitor code.
When a hardware fault occurs in a specific cell, only the virtual machines that are currently using CPU or memory resources from that cell are affected.
The monitor isolates these failures transparently, allowing unmodified commodity operating systems running in virtual machines on other cells to continue executing without interruption.
</details>

<details>
<summary>What is the trade-off Cellular Disco must manage when balancing memory across different cells?</summary>
The monitor must balance efficient resource utilization with fault containment constraints.
When a cell borrows memory from another cell to satisfy a virtual machine's peak demand, that virtual machine becomes vulnerable to hardware faults occurring in the loaner cell.
Cellular Disco must weigh the performance benefit of avoiding disk paging against the increased risk of failure from spanning multiple fault containment units.
</details>

<details>
<summary>How does Cellular Disco resolve the redundant paging problem inherent in running an operating system over a virtual machine monitor?</summary>
Redundant paging occurs when the guest operating system and the virtual machine monitor independently try to page out the same data to disk.
Cellular Disco avoids this by providing the guest operating system with a virtual paging disk.
When the guest attempts to write to this disk, the monitor recognizes if it has already paged out that specific page and simply updates an internal indirection table instead of performing an actual disk write.
</details>

## Related

- Lessons: [L04a](../Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md), [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md), [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), [L04e](../Part-2-Parallel-Systems/L04e-Scheduling.md), [L04f](../Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md)
