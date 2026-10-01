---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L03
tags: [cs6210, cs6210/practice]
---

# Practice L03

Original exam-style questions for [L03a](../Part-1-OS-Structure-and-Virtualization/L03a-Introduction-to-Virtualization.md), [L03b](../Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md), [L03c](../Part-1-OS-Structure-and-Virtualization/L03c-CPU-and-Device-Virtualization.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

## L03a Introduction to Virtualization

> [!question]- Q1. Compare and contrast native bare-metal hypervisors and hosted hypervisors. What are the key architectural differences in how they provide platform virtualization? (concepts: L03a-01, L03a-02, L03a-06)
> Native hypervisors run directly on the hardware to virtualize memory, CPU, and devices for guest operating systems.
> Hosted hypervisors run as an application within a conventional host operating system, relying on the host system for low-level hardware management.

> [!question]- Q2. How did full virtualization historically handle privileged instructions compared to para-virtualization, and what role did binary translation play? (concepts: L03a-03, L03a-04, L03a-05)
> Full virtualization historically relied on trap-and-emulate for privileged instructions.
> However, on architectures where some privileged instructions failed silently instead of trapping, binary translation was used to inspect and rewrite guest OS code dynamically.
> Para-virtualization instead required the guest OS to be modified to use explicit hypercalls, avoiding the need for binary translation and improving performance.

> [!question]- Q3. How have modern hardware capabilities changed the landscape of platform virtualization? (concepts: L03a-07)
> Modern hardware-assisted virtualization extensions, such as VT-x, AMD-V, and ARM EL2, added new CPU execution modes specifically for the hypervisor.
> This eliminated the need for complex software techniques like binary translation or extensive para-virtualization for CPU instructions, allowing unmodified guest operating systems to be efficiently virtualized.

## L03b Memory Virtualization

> [!question]- Q4. Distinguish between virtual, physical, and machine addresses in a virtualized system. How do shadow page tables map these addresses, and how does this contrast with efficient mapping in full virtualization versus para-virtualization? (concepts: L03b-01, L03b-02, L03b-03, L03b-04)
> Virtual addresses are used by guest applications, physical addresses represent the guest OS view of continuous memory, and machine addresses correspond to the actual hardware RAM.
> Shadow page tables directly map guest virtual addresses to machine addresses to speed up translation in full virtualization, though they require the hypervisor to intercept and synchronize guest page table updates.
> Para-virtualization allows the guest OS to manage the machine-level page tables directly but requires the hypervisor to validate all page table updates to ensure strict isolation.

> [!question]- Q5. How does hardware nested paging simplify memory mapping compared to software shadow page tables? What are some modern descendants in memory virtualization? (concepts: L03b-05, L03b-12)
> Hardware nested paging, like EPT and NPT, offloads the translation from physical to machine addresses directly to the MMU hardware.
> This completely eliminates the need for the hypervisor to maintain software shadow page tables and handle related page fault intercepts.
> Modern descendants include KSM, which provides content-based page sharing for KVM, and virtio-balloon, which provides a standardized interface for memory reclamation.

> [!question]- Q6. Explain the mechanism of ballooning for dynamically increasing and reclaiming memory. Why is it preferred over hypervisor-level paging? (concepts: L03b-06, L03b-07, P-VMware-ESX-Memory)
> Ballooning introduces a pseudo-device driver in the guest OS that allocates or frees memory pages.
> This enables the hypervisor to reclaim machine memory when the balloon expands and return it when the balloon shrinks.
> The VMware ESX paper highlights that this is preferred over hypervisor-level paging because the guest OS knows exactly which pages are least valuable and should be swapped out to disk, preventing the double-paging problem.

> [!question]- Q7. Compare pure share-based memory allocation policies to working-set based ones. How does the idle memory tax interact with these policies when sharing memory across VMs via content-based page sharing? (concepts: L03b-08, L03b-09, L03b-10, L03b-11, P-VMware-ESX-Memory)
> Pure share-based policies distribute memory strictly according to assigned weights, potentially wasting memory on idle VMs.
> Working-set based policies allocate memory based on active usage, ensuring that active VMs get the resources they need.
> Content-based page sharing reclaims identical pages across VMs to free up memory globally.
> The VMware ESX system introduces an idle memory tax to charge more for idle pages, incentivizing VMs to release unused memory and dynamically adjusting shares to favor active working sets.

## L03c CPU and Device Virtualization

> [!question]- Q8. What are the primary CPU virtualization goals, and how do proportional-share and fair-share CPU schedulers achieve them? (concepts: L03c-01, L03c-02)
> The goals are to provide the illusion of complete CPU ownership while ensuring fair sharing among multiple guest VMs.
> Proportional-share and fair-share schedulers achieve this by allocating CPU time slices in proportion to assigned weights.
> This guarantees that each VM receives its designated fraction of processor resources regardless of the behavior of other VMs.

> [!question]- Q9. How does handling program discontinuities like exceptions, syscalls, page faults, and interrupts differ from a native environment? (concepts: L03c-03)
> In a native environment, these discontinuities trap directly to the OS kernel for handling.
> In a virtualized environment, they typically trap to the hypervisor first.
> The hypervisor must then decide whether to handle the event itself or forward it to the appropriate guest OS by injecting a virtual interrupt or exception.

> [!question]- Q10. Compare device virtualization in full virtualization versus para-virtualization. How does Xen implement control transfer and data transfer for efficient I/O? (concepts: L03c-04, L03c-05, L03c-06, L03c-07, P-Xen)
> Full virtualization often relies on device emulation, which can be slow due to the overhead of trapping individual register accesses.
> Para-virtualization uses split drivers with a front-end in the guest and a back-end in the hypervisor for better performance.
> The Xen paper describes using software interrupts called event channels for asynchronous control transfer.
> Xen also utilizes asynchronous I/O rings for zero-copy data transfer between the guest and hypervisor domains.

> [!question]- Q11. How did the Xen architecture handle network and disk virtualization, and what modern descendants have standardized these approaches? (concepts: L03c-08, L03c-09, P-Xen)
> Xen uses a privileged driver domain with direct hardware access to manage physical disks and network interfaces.
> This domain routes traffic to and from unprivileged VMs via shared memory I/O rings and event channels.
> Modern descendants like virtio have standardized these para-virtualized device interfaces across different hypervisors.
> SR-IOV takes device virtualization further by allowing physical PCIe devices to present multiple virtual interfaces directly to VMs, bypassing the hypervisor entirely for the data path.
