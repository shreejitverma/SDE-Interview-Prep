---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/224056.224075"]
course: cs6210
lesson: L02
reading: required
venue: "SOSP 1995"
authors: [Jochen Liedtke]
tags: [cs6210, cs6210/paper]
aliases: ["On Micro-Kernel Construction"]
---

# On Micro-Kernel Construction

SOSP 1995. Reading status: required. [Link](https://doi.org/10.1145/224056.224075).

> [!abstract] One-line summary
> L4 demonstrates that microkernels are not inherently slow by aggressively minimizing kernel abstractions and heavily optimizing inter-process communication.

## Problem

First-generation microkernels, most notably Mach, suffered from significant performance overhead that made them unsuitable for many production environments.
This poor performance led to a widespread community consensus that microkernels were inherently slow due to the fundamental costs of cross-address space communication and context switching.
The performance penalty was largely caused by bloated kernel designs that retained too many traditional OS abstractions and failed to optimize the critical paths for inter-process communication.

## Key idea

A microkernel must be strictly minimal, including only the concepts absolutely required to establish protection boundaries.
If a feature can be implemented outside the kernel without compromising security, it must be moved to user space.
By reducing the kernel to just three core abstractions - address spaces, threads, and inter-process communication - the microkernel can be highly optimized for the underlying hardware architecture to achieve near bare-metal performance.

## Design

The L4 microkernel defines address spaces recursively, where a root address space grants or maps pages to user-level pagers that construct additional address spaces.
Inter-process communication is the most critical operation and is designed to be as fast as possible.
IPC messages are passed entirely in CPU registers whenever feasible, avoiding memory copies and reducing cache footprint.
The kernel utilizes direct process switching, transferring control directly from the sender thread to the receiver thread without invoking a complex central scheduler.
Hardware interrupts are cleanly integrated into the system by converting them into standard IPC messages delivered to associated user-level handler threads.

## Evaluation

The L4 microkernel was evaluated on Intel 486 and Pentium architectures.
L4 achieved an IPC round-trip time of 22 cycles on a Pentium processor and 114 cycles on a 486.
This was orders of magnitude faster than the IPC performance of Mach, which took well over a thousand cycles for similar operations.
The performance measurements proved that hardware limitations, such as TLB flushes on context switches, could be mitigated through architecture-specific optimizations like x86 segment registers.

## Limitations and critiques

The extreme performance of L4 was achieved through heavy optimization tailored to specific hardware architectures, raising concerns about its portability.
While individual IPC operations were incredibly fast, constructing full OS services in user space still required many IPCs, potentially causing high overall system overhead for certain workloads.
The recursive address space model, while elegant, can become complex to manage and slow to tear down when deep mapping trees are involved.

## What it led to

Liedtke's work successfully revived the field of microkernel research by proving that the performance issues of early microkernels were implementation artifacts, not architectural mandates.
The L4 microkernel family became the foundation for numerous highly successful commercial and research operating systems.
It directly paved the way for seL4, the world's first operating system kernel to achieve a formal machine-checked proof of functional correctness, which is now widely used in secure and embedded systems.

## Exam angles

> [!question]- What is Liedtke's strict rule for including a feature inside the microkernel?
> A concept is tolerated inside the microkernel only if moving it outside the kernel would prevent the implementation of the system's required functionality or compromise security.
> If a service can be safely run in user space, it strictly belongs in user space.

> [!question]- How did L4 achieve such dramatically faster IPC than early microkernels like Mach?
> L4 optimized IPC by passing short messages entirely within CPU registers, completely avoiding memory access.
> It utilized direct process switching to yield the CPU immediately to the receiving thread, bypassing the general scheduler.
> It was also carefully designed to minimize cache footprint so that IPC operations did not evict the working set of the communicating applications.

> [!question]- How does L4 handle hardware interrupts?
> L4 treats hardware interrupts as standard IPC messages.
> When an interrupt occurs, the microkernel translates it into an IPC message and sends it to the specific user-level thread registered to handle that interrupt.
> This unified the communication model and kept device driver logic entirely out of the kernel.

## Related

- Lessons: [L02a](../Part-1-OS-Structure-and-Virtualization/L02a-OS-Structure-Overview.md), [L02b](../Part-1-OS-Structure-and-Virtualization/L02b-SPIN-Approach.md), [L02c](../Part-1-OS-Structure-and-Virtualization/L02c-Exokernel-Approach.md), [L02d](../Part-1-OS-Structure-and-Virtualization/L02d-L3-Microkernel-Approach.md)
