---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: optional
reading: optional
venue: "USENIX Summer 1986"
authors: ["Mike Accetta", "Robert Baron", "William Bolosky", "David Golub", "Richard Rashid", "Avadis Tevanian", "Michael Young"]
tags: [cs6210, cs6210/paper]
aliases: ["Mach: A New Kernel Foundation for UNIX Development"]
---

# Mach: A New Kernel Foundation for UNIX Development

USENIX Summer 1986. Reading status: optional.

> [!abstract] One-line summary
> Mach is a multiprocessor operating system kernel that separates the process abstraction into tasks and threads, integrates virtual memory with capability-based interprocess communication, and acts as an extensible foundation for UNIX development.

## Problem

The traditional UNIX process abstraction was insufficient for modern applications and tightly coupled multiprocessors.
It led to high overhead for server applications and limited parallelism.

## Key idea

The key idea is to separate the UNIX process abstraction into two orthogonal concepts: a task (an execution environment and unit of resource allocation) and a thread (the basic unit of CPU utilization).
This division, combined with a capability-based interprocess communication facility integrated with virtual memory management, allows the kernel to be an extensible foundation for building UNIX functionality in user-space rather than a monolithic entity.

## Design

Mach supports four basic abstractions: tasks, threads, ports, and messages.
A task provides a paged virtual address space and protected access to system resources.
A thread executes within a task and shares its resources.
A port is a protected communication channel representing objects or services.
A message is a typed collection of data objects transferred between threads via ports.
Mach features a virtual memory system supporting large sparse address spaces, copy-on-write sharing, and memory-mapped files with user-provided pagers.
Network communication is handled transparently by user-level network servers that map local ports to network ports.

## Evaluation

Early benchmarks on the MicroVAX II showed that allocating and touching new memory cost less than 0.7 milliseconds per 1024 bytes, compared to approximately 1.2 milliseconds for 4.3BSD.
Operations like fork were substantially faster due to the new virtual memory support using copy-on-write mechanisms.

## Limitations and critiques

The paper reports early results from an untuned system, lacking extensive performance comparisons with other systems.
Moving UNIX compatibility into user-space tasks could introduce IPC overhead that offsets the benefits of the microkernel design.

## What it led to

Mach profoundly influenced modern operating systems by establishing the microkernel architecture model.
Its concepts heavily shaped the design of macOS, iOS, and Windows NT.

## Exam angles

<details>
<summary>Describe how Mach separates the traditional UNIX process abstraction and explain the benefits of this separation for multiprocessor environments.</summary>
Mach splits the process into a task, which is a collection of resources like a virtual address space and port rights, and a thread, which is the basic unit of CPU execution.
This allows an application to use full parallelism on a multiprocessor with minimal kernel overhead by running multiple threads within a single task's shared address space.
</details>

<details>
<summary>How does Mach handle large message transfers efficiently between tasks on the same node?</summary>
Mach integrates its interprocess communication with its virtual memory management.
When a large message is sent, the memory containing the message is marked as copy-on-write.
The receiving task maps this data into its own address space, avoiding the overhead of physically copying the memory unless either task attempts to modify it.
</details>

<details>
<summary>Explain the role of ports in Mach and how they relate to the concept of object-oriented design.</summary>
Ports are protected communication channels that act as reference objects.
Accessing a service or operating on a resource is done by sending a message to the port representing it, similar to invoking a method on an object reference in an object-oriented system.
</details>

## Related

- Lessons: not covered in lectures (optional reading)
