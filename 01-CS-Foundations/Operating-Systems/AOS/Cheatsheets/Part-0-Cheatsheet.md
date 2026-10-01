---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
tags: [cs6210, cs6210/cheatsheet]
---

# Part 0 Cheat Sheet

## Virtual Memory and Caches

**Formulas and Cache Arithmetic**
- A VIPT cache avoids synonyms when the cache size divided by associativity is less than or equal to the page size.
- The number of colors in a cache equals the cache size divided by the page size.
- A multi-level page table reduces space by omitting subtrees for unmapped virtual address ranges.
- Belady's anomaly occurs when a FIFO page replacement policy faults more times on a trace after being given more physical frames.
- A stack algorithm like LRU is immune to Belady's anomaly.

**Translation and Context Switch Mechanisms**
- The CPU translates virtual addresses to physical frames through a page table that the OS manages and the TLB caches.
- A VIPT cache overlaps the set lookup with TLB translation because the index bits sit entirely inside the page offset.
- A page fault traps to the kernel, allocates or fills a frame, updates the PTE, invalidates the stale TLB entry, and restarts the instruction.
- A thread context switch costs the direct overhead of saving registers to the kernel stack and the hidden penalty of TLB invalidation and cache eviction.
- A context switch between threads of the same process avoids the TLB invalidation because the address space remains identical.

## Architecture, Storage, and Networking

**NUMA and Performance Arithmetic**
- An SMP provides uniform DRAM latency to all CPUs, whereas a NUMA machine provides lower latency to local DRAM and higher latency to remote nodes.
- A remote scan delivers lower bandwidth than a local scan and may trigger migration if automatic NUMA balancing is active.
- To compute network segments, subtract the IP header and TCP header sizes from the MTU to find the MSS, then divide the payload by the MSS.
- At high line rates, an interrupt per packet consumes significant core-seconds, making NAPI batching necessary to divide interrupt overhead by the poll budget.

**File System and Data Structures**
- A directory entry contains a name and an inode number.
- An inode holds the file metadata, link count, and the block map to the device.
- A classic inode uses 12 direct blocks, one single-indirect block, one double-indirect block, and one triple-indirect block.
- The number of pointers in an indirect block is the block size divided by the pointer size.
- An open file maintains its own offset and holds a reference to the inode, keeping it alive even if the directory link count drops to zero.
- The storage stack flows from the VFS to the page cache, down to the block layer, and finally to the device driver.

**Networking Layers**
- The socket layer manages the process boundary and the copy between user and kernel memory.
- The transport layer, using TCP or UDP, demultiplexes endpoints using a 16-bit port.
- The network layer handles IPv4 or IPv6 routing and decrements the hop limit.
- The neighbor layer resolves the next-hop IP address to a link-layer MAC address using ARP or NDP.
- The device queueing layer enforces traffic control ordering before the driver rings the NIC DMA descriptors.

## Concurrency and the Kernel

**Memory Layout and Control Transfers**
- Concurrency structures a program as independent tasks, while parallelism executes tasks simultaneously on multiple physical cores.
- The process memory layout places the text segment at the bottom, followed by initialized data and uninitialized BSS.
- The heap grows upward from the BSS segment, and the stack grows downward from a high virtual address.
- The kernel gains processor control through synchronous exceptions, asynchronous hardware interrupts, and intentional system calls.
- Every user thread has a small, fixed-size kernel stack used exclusively when executing in kernel mode.

**Lock Algorithm Comparison**
| Algorithm | Space | Fairness | Interconnect Traffic | Best For |
| --- | --- | --- | --- | --- |
| Test-and-Set Spinlock | O(1) | None | High (spins on write) | Very short waits |
| Spin on Read | O(1) | None | Medium (spins in cache, invalidation storm on release) | Short waits |
| Backoff Lock | O(1) | None | Low (delay reduces traffic) | Unknown contention |
| Ticket Lock | O(1) | FIFO | Medium (spins on read, invalidation storm on release) | Fair access on small machines |
| Anderson (Array) | O(N) | FIFO | Low (spins on private padded line) | UMA with known max threads |
| MCS Lock | O(N) | FIFO | Low (spins on local node) | ccNUMA machines |
| Linux qspinlock | O(1) lock, O(N) queue | FIFO | Low (MCS-like queueing when contended) | Modern production systems |

**Barrier Algorithm Comparison**
| Algorithm | Space | Critical Path | Messages | Bottleneck |
| --- | --- | --- | --- | --- |
| Centralized Counting | O(1) | O(N) | O(N) | Single counter |
| Sense-Reversing | O(1) | O(N) | O(N) | Single counter (avoids reset race) |
| Combining Tree | O(N) | O(log N) | O(N) | Distributed |
| MCS Tree | O(N) | O(log N) | O(N) | 4-ary arrival, binary wakeup |
| Tournament | O(N) | O(log N) | O(N) | Binary tree winners |
| Dissemination | O(N log N) | O(log N) | O(N log N) | Symmetric, no central node |
