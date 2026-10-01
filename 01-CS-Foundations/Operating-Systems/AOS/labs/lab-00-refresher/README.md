---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lessons: [R01, R02, R03]
environment: "Lima VM aos (Ubuntu 24.04 arm64)"
tags: [cs6210, cs6210/lab]
---

# lab-00-refresher: Refresher: page faults, TLB, caches, and a pthreads producer-consumer

> [!info] Goal
> Make R01, R02, and R03 concepts concrete with real commands and measurements involving page faults, TLB and cache timing curves, and a POSIX threads producer-consumer model.

> [!warning] Honor Code Guard
> This lab is a refresher of prerequisite concepts and does not implement a graded project (such as a vCPU scheduler or a distributed barrier). No project solutions or autograder details are contained here.

## Prerequisites
- The `aos` Lima VM (Ubuntu 24.04 arm64, 8 vCPUs, nested KVM). See [setup](../setup/README.md) for details on capabilities and verification.

## Run Commands
To run the lab and verify all components inside the VM:
```bash
../setup/run-in-vm.sh lab-00-refresher test
```

## What You Should See
When running the tests, the output will resemble the captured results in `expected-output.txt`.

For **page faults**, an anonymous 10 MB mapping (2560 pages) yields exactly 2560 minor faults upon touch. Conversely, the file-backed mapping produces only 160 minor faults.
```text
After touching anonymous pages:
  Minor page faults: 2560
  Major page faults: 0

--- Major Faults Demonstration ---
  Dropping caches via sudo...
After reading dropped file pages:
  Minor page faults: 160
  Major page faults: 0
```

For **pagemap**, the present bit flips to 1 after the page is written:
```text
Before touch:
  Present: 0
  PFN: 0x0
After touch:
  Present: 1
  PFN: 0x100000000000000
```

For **cache and TLB** (showing 4096-byte stride TLB misses):
```text
Size(KB)	Stride(B)	Time(ns/access)
...
1024		4096		9.90
2048		4096		9.93
4096		4096		9.89
8192		4096		10.20
16384		4096		36.75
32768		4096		41.02
```

## How It Works

1. **Page Faults (`page_faults.c`)**: 
   The kernel delays physical memory allocation until a process actually touches a virtual page (demand paging). For an anonymous `mmap`, every 4 KB page touched causes a minor page fault, resulting in exactly 2560 faults for 10 MB. For the file-backed `mmap`, the kernel employs "fault-around", mapping 16 pages (64 KB) at a time, resulting in exactly `2560 / 16 = 160` minor faults. Zero major faults occur because the VM filesystem (virtiofs) or host page cache masks the block IO.

2. **Page Map (`pagemap.c`)**:
   `/proc/self/pagemap` provides the mapping from virtual pages to physical page frames (PFN). Before a page is touched, its present bit is 0. Upon writing, the kernel allocates physical memory and sets the present bit to 1. Note that since Linux 4.0, the actual PFN is zeroed out for unprivileged processes for security reasons (mitigating Rowhammer attacks).

3. **Cache and TLB Profiling (`cache_tlb.c`)**:
   A pointer-chasing array is used to defeat hardware spatial prefetchers. The program measures average access latency as working set size increases. A stride of 64 bytes isolates data cache latency (L1, L2, L3), while a stride of 4096 bytes isolates TLB latency. As the working set exceeds the TLB reach (e.g., jumping to ~10 ns at 1 MB, and ~36 ns at 16 MB), page table walks become the primary bottleneck. *Page coloring* is an OS technique that strategically allocates physical pages so they map to diverse cache sets, minimizing cache conflict misses for contiguous virtual pages.

4. **Producer-Consumer (`prod_cons.c`)**:
   A bounded buffer of size 5 is managed by two pthreads. A mutex protects shared state (the buffer, head, tail, and count). Two condition variables (`cond_not_full` and `cond_not_empty`) enable threads to efficiently sleep while waiting for the buffer state to change, rather than spin-waiting.

## Experiments to Try

- **Experiment 1**: Run the `cache_tlb` benchmark and pipe it to a file. Plot the output. *Prediction*: You will see clear "steps" corresponding to the capacities of the L1, L2, and L3 caches for the 64-byte stride, and TLB size limits for the 4096-byte stride.
- **Experiment 2**: Modify `page_faults.c` to use `madvise(MADV_HUGEPAGE)`. *Prediction*: Transparent Huge Pages (2 MB) will drastically reduce the number of minor page faults from 2560 to 5.
- **Experiment 3**: Increase the producer-consumer buffer size to equal the number of items (20). *Prediction*: The producer will not have to block at all and will fill the buffer rapidly before the consumer empties it.

## Questions

<details>
<summary>Why did the file-backed mapping only generate 160 minor page faults for a 10 MB file?</summary>

The Linux kernel implements a feature called "fault-around" for file-backed mappings. Instead of faulting in one 4 KB page at a time, it faults in multiple pages (typically 64 KB, or 16 pages) surrounding the faulting address to amortize the overhead of the page fault handler and page table locking. `2560 pages / 16 = 160 faults`.
</details>

<details>
<summary>Why is the actual Physical Frame Number (PFN) in `/proc/self/pagemap` shown as 0 for an unprivileged process?</summary>

Since Linux 4.0, access to the raw PFN is restricted to `CAP_SYS_ADMIN` (root) to prevent security vulnerabilities like the Rowhammer attack, where an attacker could use PFN knowledge to flip bits in adjacent physical memory.
</details>

<details>
<summary>How does the 4096-byte stride array traversal defeat the data cache and stress the TLB?</summary>

A 4096-byte stride exactly matches the page size. Every access lands on a new virtual page. Since we are pointer-chasing, the hardware prefetcher cannot predict the next address. This forces a TLB translation for every single memory access, making the latency purely dependent on TLB hits and page table walks, bypassing the benefits of data cache locality.
</details>
