---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L02
reading: self-study
venue: "GMD TR 933, 1995"
authors: [Jochen Liedtke]
tags: [cs6210, cs6210/paper]
aliases: ["Improved Address-Space Switching on Pentium Processors by Transparently Multiplexing User Address Spaces"]
---

# Improved Address-Space Switching on Pentium Processors by Transparently Multiplexing User Address Spaces

GMD TR 933, 1995. Reading status: self-study.

> [!abstract] One-line summary
> By mapping small tasks into distinct segments of a single shared page table, L4 avoids costly TLB flushes on address space switches for Pentium processors.

## Problem

Processors with untagged TLBs, such as the Intel 486 and Pentium, require a complete TLB flush during an address space switch (i.e., changing the page directory root). As TLBs increase in size, the secondary costs of address space switching grow substantially because the returning process experiences many expensive TLB misses while re-establishing its working set.

## Key idea

Instead of switching the page table root on every context switch, the operating system can use x86 segment registers to multiplex multiple small tasks within a single, shared linear address space. By allocating each small process to a dedicated sub-region of the virtual address space and updating segment base and limit bounds during a switch, the OS enforces memory protection without modifying the page table root. This completely avoids the TLB flush penalty and dramatically reduces the overhead of inter-process communication for typical microkernel workloads.

## Design

The 4 GB virtual address space is partitioned into three regions: a 3 GB large user space, a 0.5 GB area for small user spaces, and a 0.5 GB kernel space. Small tasks are each assigned a distinct segment within the small user space region, which is shared across all page tables. When switching to a small task, the kernel simply modifies the base and limit of the user segment descriptor to restrict the task's access to its designated region. This requires no `cr3` register reload, preserving the TLB. Furthermore, since all small tasks reside in the same global region, the kernel can copy messages directly between them without any additional temporary memory mapping. Tasks are dynamically promoted to a large user space if they attempt to map memory outside their small allocated region.

## Evaluation

Segment-based context switching significantly improves performance on a 90 MHz Pentium. For an application communicating with a server via RPC, the segment switch eliminates the TLB miss penalty incurred when re-establishing the application's working set. When the application's data working set is 64 pages, a traditional page-table switch takes 12.7 microseconds, whereas the segment-based switch remains roughly constant at 3.6 microseconds. The overhead of an address space switch drops from an average of 50-302 cycles down to just 23-51 cycles when small spaces are involved.

## Limitations and critiques

The approach is highly hardware-specific, relying on the segmented memory architecture of x86 processors which has largely been deprecated in 64-bit long mode. The technique also depends on the workload consisting predominantly of tasks with very small memory footprints that fit into the multiplexed 0.5 GB region.

## What it led to

This optimization became a core technique in the L4 microkernel, proving that microkernel IPC overheads were largely an artifact of poor OS implementation rather than an inherent architectural flaw. It demonstrated that hardware-specific optimizations could yield order-of-magnitude performance improvements.

## Exam angles

- **How does the L4 segment-based switch bypass the TLB flush penalty?**
  It places small tasks in disjoint segments within the same page directory, updating segment bounds instead of the page table root (`cr3`) during a context switch.
- **Why does switching between two large address spaces still require a TLB flush?**
  Large tasks each require the full 3 GB user region, meaning their addresses overlap. They must use separate page directories, requiring a `cr3` change and a TLB flush.
- **What happens if a small task grows beyond its assigned segment?**
  The kernel transparently converts the task into a large task by assigning it a new page directory and moving its memory mapping to the large user space region.

## Related

- Lessons: [L02a](../Part-1-OS-Structure-and-Virtualization/L02a-OS-Structure-Overview.md), [L02b](../Part-1-OS-Structure-and-Virtualization/L02b-SPIN-Approach.md), [L02c](../Part-1-OS-Structure-and-Virtualization/L02c-Exokernel-Approach.md), [L02d](../Part-1-OS-Structure-and-Virtualization/L02d-L3-Microkernel-Approach.md)
