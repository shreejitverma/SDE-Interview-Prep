---
type: paper
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: ["https://www.usenix.org/legacy/event/osdi02/tech/waldspurger.html"]
course: cs6210
lesson: L03
reading: required
venue: "OSDI 2002"
authors: ["Carl A. Waldspurger"]
tags: [cs6210, cs6210/paper]
aliases: ["Memory Resource Management in VMware ESX Server"]
---

# Memory Resource Management in VMware ESX Server

OSDI 2002. Reading status: required. [Link](https://www.usenix.org/legacy/event/osdi02/tech/waldspurger.html).

> [!abstract] One-line summary
> ESX Server efficiently multiplexes overcommitted memory among unmodified commodity operating systems using ballooning, content-based page sharing, and an idle memory tax.

## Problem

Server consolidation requires efficiently multiplexing hardware resources among multiple virtual machines running unmodified commodity operating systems.
Since guest operating systems expect dedicated physical memory, the virtual machine monitor must manage overcommitted memory without knowing the internal state or memory management policies of the guest OS.
Standard paging mechanisms introduced at the hypervisor layer can cause double paging and performance anomalies because the hypervisor cannot distinguish between active and idle guest memory pages.

## Key idea

ESX Server introduces a suite of cooperative and transparent mechanisms to manage memory efficiently in an overcommitted virtualized environment without modifying the guest OS.
The primary technique is ballooning, where a pseudo-device driver within the guest allocates pinned physical pages to return memory to the hypervisor, tricking the guest native memory manager into making informed eviction decisions.
This is augmented by content-based page sharing to deduplicate identical memory pages and an idle memory tax to fairly allocate memory based on active working sets.

## Design

The balloon driver operates by inflating its memory allocation to increase pressure inside the guest OS, forcing it to page out less valuable data to its own swap disk.
To reduce overall memory footprint, a background scanner uses a hash-based approach to identify identical pages across different virtual machines and shares them using copy-on-write, avoiding the need for guest OS cooperation.
ESX Server allocates memory to VMs using a proportional-share algorithm based on shares and an estimated working set size.
An idle memory tax is applied to charge more for idle pages, preventing VMs from hoarding memory they are not actively using and reallocating it to VMs that need it.

## Evaluation

The system was evaluated using benchmarks like SPEC95 and dbench on an ESX Server running multiple Linux and Windows virtual machines.
The ballooning mechanism demonstrated overheads ranging from only 1.4% to 4.4% compared to a natively sized virtual machine.
Content-based page sharing was shown to reclaim approximately 10-33% of total memory in real-world production environments, and up to 67% in idealized homogeneous workloads.
The idle memory tax effectively reallocated memory from idle VMs to active VMs, improving overall system throughput while maintaining performance isolation.

## Limitations and critiques

Ballooning relies on the guest OS responding to memory pressure, and if the balloon driver is disabled or the guest OS is unresponsive, the hypervisor must fall back to blind hypervisor-level swapping.
Content-based page sharing incurs a continuous CPU overhead for background scanning and hashing.
The use of an idle memory tax and statistical working set estimation may sometimes penalize bursty workloads that need memory quickly after a period of idleness.

## What it led to

The memory management techniques introduced in this paper became industry standard practices for modern hypervisors.
Ballooning is widely implemented across all major virtualization platforms to manage memory overcommitment.
Transparent page sharing proved crucial for high-density server consolidation and virtual desktop infrastructure, although security concerns regarding side-channel attacks later caused some platforms to disable inter-VM sharing by default.
The proportional-share allocation model heavily influenced subsequent cloud computing resource scheduling algorithms.

## Exam angles

<details>
<summary>Explain the "double paging" problem and how VMware ESX Server avoids it.</summary>
Double paging occurs when the hypervisor pages out a guest page to the hypervisor swap, and then the guest OS decides to page out that same page to its own virtual disk, causing it to be faulted back into memory only to be written out again.
ESX Server avoids this primarily by using ballooning, which forces the guest OS to choose which pages to swap out using its own native paging mechanism, keeping the hypervisor out of the paging path in the common case.
</details>

<details>
<summary>How does ESX Server implement page sharing without modifying the guest operating system?</summary>
Instead of tracking shared files or intercepting OS calls, ESX Server uses content-based page sharing.
A background process periodically hashes the contents of guest memory pages.
If two pages have the same hash, a full byte-by-byte comparison is performed, and if they are identical, the hypervisor updates the page tables to map both virtual pages to the same physical machine page with copy-on-write protection.
</details>

<details>
<summary>What is the purpose of the idle memory tax in ESX Server's allocation policy?</summary>
The idle memory tax is designed to prevent virtual machines from hoarding memory they are not actively using.
By charging a higher price for idle pages compared to active pages within the proportional-share framework, the system incentivizes the reclamation of memory from idle VMs, which can then be reallocated to VMs with active working sets.
</details>

## Related

- Lessons: [L03a](../Part-1-OS-Structure-and-Virtualization/L03a-Introduction-to-Virtualization.md), [L03b](../Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md), [L03c](../Part-1-OS-Structure-and-Virtualization/L03c-CPU-and-Device-Virtualization.md)
