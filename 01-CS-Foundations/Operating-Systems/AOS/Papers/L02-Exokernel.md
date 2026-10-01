---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://doi.org/10.1145/224056.224076"]
course: cs6210
lesson: L02
reading: required
venue: "SOSP 1995"
authors: [Dawson R. Engler, M. Frans Kaashoek, James O'Toole Jr.]
tags: [cs6210, cs6210/paper]
aliases: ["Exokernel: An Operating System Architecture for Application-Level Resource Management"]
---

# Exokernel: An Operating System Architecture for Application-Level Resource Management

SOSP 1995. Reading status: required. [Link](https://doi.org/10.1145/224056.224076).

> [!abstract] One-line summary
> Exokernel separates protection from management by safely exporting physical resources to untrusted library operating systems that implement high-level abstractions.

## Problem

Traditional monolithic operating systems enforce generalized, high-level abstractions for virtual memory, inter-process communication, and file systems.
These fixed abstractions frequently penalize specialized applications, such as databases or garbage collectors, which have predictable access patterns and unique requirements.
Hiding the underlying hardware prevents application developers from implementing their own optimized resource management policies, severely limiting both performance and flexibility.

## Key idea

An operating system should exclusively focus on securely multiplexing hardware resources, leaving all resource management and abstraction to untrusted application-level software.
By aggressively separating protection from management, the exokernel can export a low-level interface that is as close to the bare metal as possible.
Applications link against library operating systems that implement tailored policies and high-level abstractions, allowing each application to have a customized, highly optimized view of the system.

## Design

The exokernel exposes hardware resources using physical names rather than virtual ones, reducing indirection and allowing applications to optimize for physical attributes like cache layout.
It uses secure bindings to decouple authorization from actual resource usage, enabling fast access checks.
Examples of secure bindings include hardware TLB entries and downloaded packet filters, which the kernel can verify quickly without understanding their semantics.
The kernel utilizes a visible resource revocation protocol to inform applications when resources are being reclaimed, giving them the choice of which specific resources to yield.
If an application is unresponsive or malicious, the exokernel employs an abort protocol to forcefully break the secure bindings and reclaim the resources.

## Evaluation

The prototype exokernel, Aegis, and its library operating system, ExOS, were implemented and evaluated on DECstation MIPS hardware.
Primitive kernel operations like exception dispatch and protected control transfer were measured to be ten to a hundred times faster than in the monolithic Ultrix operating system.
Aegis's exception dispatch was about five times faster than the best reported microkernel implementations of the time.
Application-level implementations of virtual memory and inter-process communication in ExOS executed five to forty times faster than their kernel-level counterparts in Ultrix.

## Limitations and critiques

Delegating resource management to library operating systems heavily increases the burden on application developers if standard libraries do not meet their needs.
Achieving global fairness and optimal utilization is difficult when scheduling and resource allocation policies are decentralized across mutually distrustful applications.
Furthermore, the pure separation of protection and management is difficult to maintain perfectly, as some modern hardware devices have complex state that the kernel must understand to safely multiplex.

## What it led to

Exokernel highly influenced the development of hypervisors and virtual machine monitors, which similarly multiplex hardware for guest operating systems.
The idea of linking OS functionality directly into the application space evolved into the modern concept of unikernels.
It also motivated the development of high-performance user-space networking and storage frameworks, such as DPDK and SPDK, which bypass the kernel entirely for maximum throughput.

## Exam angles

> [!question]- What is the principle of separating protection from management in the Exokernel architecture?
> The kernel should only handle the secure multiplexing and protection of physical hardware resources.
> It should not impose policies on how those resources are used.
> All management policies, mapping, and high-level abstractions are handled by untrusted library operating systems residing in application space.

> [!question]- How do secure bindings improve performance in an exokernel?
> Secure bindings separate the complex authorization logic from the access time checks.
> An application establishes a secure binding once, such as downloading a packet filter or requesting a physical page.
> The kernel then performs a very simple, fast check on every subsequent access using the established binding, without needing to understand the higher-level semantics.

> [!question]- Why does the exokernel use visible resource revocation?
> When the kernel needs to reclaim resources, it exposes this revocation to the application rather than implicitly stealing the resource.
> This allows the library operating system to participate in the decision and choose the optimal resource to give up.
> For example, a database could choose to flush and yield a specific buffer page that it knows will not be needed soon.

## Related

- Lessons: [L02a](../Part-1-OS-Structure-and-Virtualization/L02a-OS-Structure-Overview.md), [L02b](../Part-1-OS-Structure-and-Virtualization/L02b-SPIN-Approach.md), [L02c](../Part-1-OS-Structure-and-Virtualization/L02c-Exokernel-Approach.md), [L02d](../Part-1-OS-Structure-and-Virtualization/L02d-L3-Microkernel-Approach.md)
