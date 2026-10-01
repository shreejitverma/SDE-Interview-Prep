---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/945445.945462"]
course: cs6210
lesson: L03
reading: required
venue: "SOSP 2003"
authors: ["Paul Barham", "Boris Dragovic", "Keir Fraser", "Steven Hand", "Tim Harris", "Alex Ho", "Rolf Neugebauer", "Ian Pratt", "Andrew Warfield"]
tags: [cs6210, cs6210/paper]
aliases: ["Xen and the Art of Virtualization"]
---

# Xen and the Art of Virtualization

SOSP 2003. Reading status: required. [Link](https://doi.org/10.1145/945445.945462).

> [!abstract] One-line summary
> Paravirtualization provides high-performance isolation for multiple commodity operating systems by exposing an idealized virtual machine abstraction that requires minor guest OS modifications.

## Problem

Full virtualization on the x86 architecture is difficult due to architecture limitations, leading to high overheads or requiring complex binary rewriting.
Existing operating systems lack strong resource isolation for multiplexing multiple services on a single machine.
Running unmodified guest operating systems incurs significant performance penalties on standard hardware.

## Key idea

Paravirtualization offers a compromise between full virtualization and operating system-level multiplexing by presenting a virtual machine abstraction similar but not identical to the underlying hardware.
By modifying the guest operating system to use a specialized hypervisor interface, the system avoids the need to virtualize difficult architectural features or use expensive binary rewriting.
This approach achieves near-native performance while maintaining strong isolation and backward compatibility with unmodified application binaries.

## Design

Xen multiplexes physical resources at the granularity of an entire operating system.
The guest OS runs at a lower privilege level (ring 1) than the hypervisor (ring 0) to ensure protection.
Privileged instructions are replaced with hypercalls into the Xen hypervisor.
Memory management is paravirtualized by allowing guest operating systems direct read access to hardware page tables while requiring hypervisor validation for updates.
Device I/O is handled via asynchronous shared memory descriptor rings to minimize context switching overhead.
A separate control domain called Domain0 manages policy, admission control, and device configuration.

## Evaluation

The evaluation used a suite of microbenchmarks and macrobenchmarks comparing XenoLinux against native Linux, VMware Workstation, and User-Mode Linux.
XenoLinux achieved performance within 2-8% of native Linux on most macrobenchmarks.
Xen demonstrated the ability to host up to 100 concurrent virtual machines with minimal performance degradation and effectively enforced resource isolation between competing domains.

## Limitations and critiques

Paravirtualization requires modifying the source code of the guest operating system, which is not feasible for closed-source legacy operating systems.
The initial design was heavily tied to the x86 architecture and its specific privilege ring structure.
Exposing some physical machine details to the guest OS can complicate live migration and dynamic resource resizing if not carefully managed.
The reliance on a single privileged Domain0 introduces a potential single point of failure and a bottleneck for device I/O.

## What it led to

Xen popularized the concept of paravirtualization and demonstrated that high-performance virtualization was achievable on commodity hardware without hardware virtualization extensions.
It became a foundational technology for early cloud computing platforms, notably powering Amazon Web Services EC2 for many years.
The project spurred CPU vendors like Intel and AMD to introduce hardware virtualization support to eliminate the need for paravirtualization and support unmodified guest operating systems.

## Exam angles

<details>
<summary>Why does Xen use ring 1 for the guest operating system instead of ring 3?</summary>
Xen uses ring 1 for the guest OS to isolate it from user applications running in ring 3, while preserving ring 0 for the hypervisor.
This allows the guest OS to be protected from applications without needing a separate address space or causing excessive TLB flushes on context switches.
</details>

<details>
<summary>How does Xen handle page faults efficiently compared to full virtualization?</summary>
Unlike full virtualization which might require shadow page tables and trapping every page table update, Xen allows the guest OS to have direct read access to hardware page tables.
Updates are batched and validated by the hypervisor through hypercalls, significantly reducing the overhead of memory management operations.
</details>

<details>
<summary>What is the purpose of Domain0 in the Xen architecture?</summary>
Domain0 is a privileged virtual machine created at boot time that hosts application-level management software.
It separates policy from mechanism by handling complex tasks like admission control, scheduling parameter configuration, and device setup, leaving the hypervisor small and focused on basic control operations.
</details>

## Related

- Lessons: [L03a](../Part-1-OS-Structure-and-Virtualization/L03a-Introduction-to-Virtualization.md), [L03b](../Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md), [L03c](../Part-1-OS-Structure-and-Virtualization/L03c-CPU-and-Device-Virtualization.md)
