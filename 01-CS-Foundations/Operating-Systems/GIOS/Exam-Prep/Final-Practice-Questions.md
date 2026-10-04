---
type: playbook
track: [sde]
level: advanced
status: complete
last_reviewed:
tags: [gios, cs6200, final-exam, exam-prep, operating-systems]
sources:
  - "Georgia Tech CS 6200 Final Exam Practice Questions"
  - "Memory Coherence in Shared Virtual Memory Systems, Li & Hudak (1989)"
  - "The Andrew File System (AFS), Morris et al. (1986)"
  - "Formal Requirements for Virtualizable Third Generation Architectures, Popek & Goldberg (1974)"
  - "NFS Version 4 Protocol, RFC 7530"
---

# CS 6200 GIOS - Final Exam Practice Questions

> Comprehensive practice set for the GIOS Final Exam (P3L2 through P4L4 scope).
> Each question covers core resource management, virtualization, and distributed systems topics.
> In Obsidian, keep answers collapsed and attempt each question first (active recall).
> On GitHub, the callouts render as clean blockquotes.

Back to [GIOS Dashboard](../_GIOS-Dashboard.md).

## Index

| # | Topic | Lecture / Source | Type |
|---|-------|------------------|------|
| [1](#1-effective-access-time-and-multi-level-paging) | EAT & Multi-Level Page Tables | [P3L2](../Part-3-Resource-Management/P3L2-Memory-Management.md#10-translation-lookaside-buffer-tlb) | Mathematical derivation |
| [2](#2-page-replacement-algorithms-and-beladys-anomaly) | Belady's Anomaly & Page Replacement | [P3L2](../Part-3-Resource-Management/P3L2-Memory-Management.md#12-page-replacement-algorithms) | Algorithm trace |
| [3](#3-ipc-mechanisms-and-copy-vs-map-threshold) | IPC: Copy vs. Memory Mapping Threshold | [P3L3](../Part-3-Resource-Management/P3L3-Inter-Process-Communication.md#8-shared-memory-vs-message-passing) | Performance analysis |
| [4](#4-hardware-synchronization-and-cache-coherence) | Hardware Lock Contention & Cache Coherence | [P3L4](../Part-3-Resource-Management/P3L4-Synchronization-Constructs.md#5-spinlocks-hardware-support) | Complexity & protocol |
| [5](#5-unix-inode-capacity-and-file-addressing) | UNIX Inode Max File Size Math | [P3L5](../Part-3-Resource-Management/P3L5-IO-Management.md#7-file-systems-vfs-and-inodes) | Storage calculation |
| [6](#6-disk-scheduling-algorithms) | Disk Arm Head Scheduling (SSTF vs SCAN vs LOOK) | [P3L5](../Part-3-Resource-Management/P3L5-IO-Management.md#11-disk-scheduling-algorithms) | Queue simulation |
| [7](#7-popek-goldberg-theorem-and-x86-virtualization) | Popek-Goldberg Virtualization & Shadow Pages | [P3L6](../Part-3-Resource-Management/P3L6-Virtualization.md#5-virtualization-requirements-popek-goldberg) | Theorem & memory walk |
| [8](#8-rpc-semantics-and-xdr-wire-serialization) | RPC Semantics & XDR Byte Alignment | [P4L1](../Part-4-Distributed-Systems/P4L1-Remote-Procedure-Calls.md#9-xdr-external-data-representation) | Protocol & wire math |
| [9](#9-distributed-file-systems-caching-and-estale) | DFS Caching: NFS vs AFS & ESTALE Root Cause | [P4L2](../Part-4-Distributed-Systems/P4L2-Distributed-File-Systems.md#4-nfs-network-file-system) | Distributed state analysis |
| [10](#10-memory-consistency-models-and-ivy-dsm) | Consistency Models & IVY Page Invalidation | [P4L3](../Part-4-Distributed-Systems/P4L3-Distributed-Shared-Memory.md#3-consistency-models) | Distributed memory trace |
| [11](#11-datacenter-technologies-and-amdahls-law) | Cluster Failure Probability & Distributed Amdahl's Law | [P4L4](../Part-4-Distributed-Systems/P4L4-Datacenter-Technologies.md#10-datacenter-networking-and-failures) | Probability & scaling math |

---

## 1. Effective Access Time and Multi-Level Paging

> [!question] Question
> Consider a 64-bit computer architecture implementing a 4-level hierarchical page table.
> - Main physical memory access latency ($t_{mem}$) is $100\text{ ns}$.
> - The on-chip hardware Translation Lookaside Buffer (TLB) has a lookup latency ($t_{tlb}$) of $1\text{ ns}$.
> - The system achieves a TLB hit ratio ($\alpha$) of $98\%$.
> - Assume that all page table levels and target data pages are resident in physical memory (no page fault to disk).
> 
> 1. Calculate the Effective Access Time (EAT) for a memory reference in this system.
> 2. If the TLB hit ratio drops from $98\%$ to $90\%$, what is the percentage increase in the Effective Access Time?
> 3. Why do 64-bit systems utilize multi-level paging rather than a single flat page table?

> [!success]- Answer
> **1. Effective Access Time (EAT) Calculation:**
> When the CPU generates a virtual memory address:
> - **TLB Hit Case:** The address translation is found in the TLB ($1\text{ ns}$).
> The CPU then issues one memory read to physical RAM to fetch the actual operand ($100\text{ ns}$).
> Total latency on TLB hit:
> $$T_{hit} = t_{tlb} + t_{mem} = 1\text{ ns} + 100\text{ ns} = 101\text{ ns}$$
> - **TLB Miss Case:** The translation is not in the TLB ($1\text{ ns}$).
> The hardware MMU must perform a page table walk across all 4 levels of the hierarchical page table.
> Each page table level requires a memory read ($4 \times 100\text{ ns} = 400\text{ ns}$).
> After determining the physical frame number, the CPU performs the final memory read for the target data ($100\text{ ns}$).
> Total latency on TLB miss:
> $$T_{miss} = t_{tlb} + (4 \times t_{mem}) + t_{mem} = 1 + 400 + 100 = 501\text{ ns}$$
> - **Formula for EAT:**
> $$EAT = \alpha \cdot T_{hit} + (1 - \alpha) \cdot T_{miss}$$
> $$EAT = 0.98 \times (101) + 0.02 \times (501) = 98.98 + 10.02 = \mathbf{109.0\text{ ns}}$$
> 
> ---
> 
> **2. EAT with $\alpha = 0.90$ (90% Hit Ratio):**
> $$EAT_{90} = 0.90 \times (101) + 0.10 \times (501) = 90.90 + 50.10 = \mathbf{141.0\text{ ns}}$$
> 
> Percentage increase in EAT:
> $$\text{Percentage Increase} = \frac{141.0 - 109.0}{109.0} \times 100 = \frac{32.0}{109.0} \times 100 \approx \mathbf{29.36\%}$$
> 
> An $8\%$ decline in TLB hit rate increases average memory latency by nearly $30\%$ because each miss penalizes execution by 4 additional round trips to RAM.
> 
> ---
> 
> **3. Rationale for Multi-Level Paging in 64-Bit Architectures:**
> In a 64-bit architecture with 4 KB pages ($2^{12}$ bytes), a flat linear page table would require $2^{52}$ page table entries (PTEs).
> At 8 bytes per PTE, a single process's page table would consume $2^{55}\text{ bytes} = 32\text{ Petabytes}$ of RAM.
> Furthermore, this flat table would have to be physically contiguous in memory.
> Multi-level paging creates a sparse tree.
> Entire subtrees corresponding to unused regions of the process's 64-bit virtual address space (the vast gap between stack, heap, and code) require no page tables at all.
> Only the root directory and the active branch tables are allocated in RAM.

---

## 2. Page Replacement Algorithms and Belady's Anomaly

> [!question] Question
> Consider a process referencing memory pages in the following sequence of virtual page numbers:
> 
> $$\sigma = \langle 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5 \rangle$$
> 
> 1. Trace the execution and determine the total number of page faults using the **First-In, First-Out (FIFO)** replacement algorithm with:
>    - Physical allocation of **3 page frames**.
>    - Physical allocation of **4 page frames**.
> 2. Does this reference string demonstrate **Belady's Anomaly** under FIFO?
> Define Belady's Anomaly.
> 3. Does the **Least Recently Used (LRU)** algorithm suffer from Belady's Anomaly on this reference string or any reference string?
> Explain the theoretical property that governs this behavior.

> [!success]- Answer
> **1. FIFO Page Fault Trace:**
> 
> #### FIFO with 3 Frames (Initial frames empty: `[-] [-] [-]`):
> 
> | Ref | Frame 1 | Frame 2 | Frame 3 | Fault? | Replaced Page |
> | :---: | :---: | :---: | :---: | :---: | :---: |
> | **1** | 1 | - | - | **Fault (1)** | - |
> | **2** | 1 | 2 | - | **Fault (2)** | - |
> | **3** | 1 | 2 | 3 | **Fault (3)** | - |
> | **4** | 4 | 2 | 3 | **Fault (4)** | 1 (oldest) |
> | **1** | 4 | 1 | 3 | **Fault (5)** | 2 (oldest) |
> | **2** | 4 | 1 | 2 | **Fault (6)** | 3 (oldest) |
> | **5** | 5 | 1 | 2 | **Fault (7)** | 4 (oldest) |
> | **1** | 5 | 1 | 2 | Hit | - |
> | **2** | 5 | 1 | 2 | Hit | - |
> | **3** | 5 | 3 | 2 | **Fault (8)** | 1 (oldest) |
> | **4** | 5 | 3 | 4 | **Fault (9)** | 2 (oldest) |
> | **5** | 5 | 3 | 4 | Hit | - |
> 
> **Total Page Faults (3 frames): 9 faults.**
> 
> ---
> 
> #### FIFO with 4 Frames (Initial frames empty: `[-] [-] [-] [-]`):
> 
> | Ref | Frame 1 | Frame 2 | Frame 3 | Frame 4 | Fault? | Replaced Page |
> | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
> | **1** | 1 | - | - | - | **Fault (1)** | - |
> | **2** | 1 | 2 | - | - | **Fault (2)** | - |
> | **3** | 1 | 2 | 3 | - | **Fault (3)** | - |
> | **4** | 1 | 2 | 3 | 4 | **Fault (4)** | - |
> | **1** | 1 | 2 | 3 | 4 | Hit | - |
> | **2** | 1 | 2 | 3 | 4 | Hit | - |
> | **5** | 5 | 2 | 3 | 4 | **Fault (5)** | 1 (oldest) |
> | **1** | 5 | 1 | 3 | 4 | **Fault (6)** | 2 (oldest) |
> | **2** | 5 | 1 | 2 | 4 | **Fault (7)** | 3 (oldest) |
> | **3** | 5 | 1 | 2 | 3 | **Fault (8)** | 4 (oldest) |
> | **4** | 4 | 1 | 2 | 3 | **Fault (9)** | 5 (oldest) |
> | **5** | 4 | 5 | 2 | 3 | **Fault (10)** | 1 (oldest) |
> 
> **Total Page Faults (4 frames): 10 faults.**
> 
> ---
> 
> **2. Belady's Anomaly Identification:**
> **Yes.** This reference string demonstrates Belady's Anomaly.
> - **Definition:** Belady's Anomaly is the counterintuitive phenomenon where increasing the number of physical page frames allocated to a process results in an **increase** in the total number of page faults for a given reference string.
> Here, allocating 3 frames caused 9 faults, while increasing physical memory to 4 frames worsened performance to 10 faults.
> 
> ---
> 
> **3. LRU and Stack Algorithms:**
> **No.** LRU will never exhibit Belady's Anomaly.
> - **Theoretical Reason:** LRU belongs to the class of **stack algorithms**.
> A replacement algorithm is a stack algorithm if the set of pages resident in an $n$-frame memory is guaranteed to be a strict subset of the pages resident in an $(n+1)$-frame memory for any reference string at every step:
> 
> $$S(n, t) \subseteq S(n+1, t)$$
> 
> Because an $(n+1)$-frame memory always retains everything that an $n$-frame memory retains, adding frames can never cause a page hit to turn into a page fault.
> FIFO is not a stack algorithm because the oldest page in memory changes regardless of recent reference recency.

---

## 3. IPC Mechanisms and Copy vs. Map Threshold

> [!question] Question
> An operating system offers two primary Inter-Process Communication (IPC) paradigms:
> - **Message Passing:** The sender invokes `write()` or `msgsnd()` to copy $N$ bytes from user space into a kernel buffer; the kernel subsequently copies the $N$ bytes into the receiver's user address space.
> - **Shared Memory:** The processes map a shared physical page into both virtual address spaces using `mmap()`.
> Data is written directly by the sender and read directly by the receiver with zero kernel data copies.
> 
> Assume the following system costs:
> - Memory copy throughput ($BW_{copy}$) is $10\text{ GB/s}$ ($10\text{ bytes/ns}$).
> - Setting up, tearing down, and managing page table mappings via `mmap()` / `munmap()` and flushing TLB entries incurs a constant overhead $T_{map\_overhead} = 8\ \mu\text{s}$ ($8,000\text{ ns}$).
> 
> 1. What is the mathematical data transfer size threshold $N^*$ (in bytes) above which shared memory becomes faster than message passing?
> 2. Explain why message passing is preferred for small messages even though it performs two memory copies.
> 3. How does Unix domain socket file descriptor passing (`SCM_RIGHTS`) bridge the gap between message passing and shared memory?

> [!success]- Answer
> **1. Threshold Calculation:**
> In message passing, data is copied twice (user $\to$ kernel, kernel $\to$ user):
> 
> $$T_{msg}(N) = 2 \times \frac{N}{BW_{copy}} = 2 \times \frac{N}{10\text{ bytes/ns}} = \frac{N}{5}\text{ ns}$$
> 
> In shared memory, the mapping overhead is paid once, after which data access incurs zero copy overhead through the kernel:
> 
> $$T_{shm}(N) = T_{map\_overhead} = 8,000\text{ ns}$$
> 
> Find the crossover point where $T_{msg}(N^*) = T_{shm}(N^*)$:
> 
> $$\frac{N^*}{5} = 8,000\text{ ns} \implies N^* = 40,000\text{ bytes} \approx 39.06\text{ KB}$$
> 
> For messages smaller than $\approx 40\text{ KB}$, double memory copying via message passing is faster than the TLB and page table manipulation cost of shared memory.
> For messages larger than $40\text{ KB}$, shared memory provides superior throughput.
> 
> ---
> 
> **2. Why Message Passing is Preferred for Small Payloads:**
> - **Lower Fixed Overhead:** Setting up shared memory requires system calls (`shm_open`, `ftruncate`, `mmap`), page table updates, and TLB invalidations.
> For small control payloads (e.g. 64-byte RPC headers or status codes), the fixed cost of manipulating hardware page tables exceeds the sub-microsecond cost of a hardware L1/L2 cache copy.
> - **Built-in Synchronization:** Message passing queues provide implicit synchronization (the receiver blocks automatically until data arrives).
> Shared memory provides raw memory bytes with no synchronization; applications must implement separate synchronization primitives (semaphores, mutexes, or atomic flags) to coordinate read/write access.
> - **Security Isolation:** Message passing preserves strict process isolation.
> In shared memory, a corrupted pointer or wild write in one process can overwrite memory in the peer process.
> 
> ---
> 
> **3. Role of `SCM_RIGHTS`:**
> `SCM_RIGHTS` enables a process to pass open file descriptors across a Unix Domain Socket (`AF_UNIX`).
> A process creates an anonymous shared memory segment (`shm_open()` or `memfd_create()`), writes arbitrary multi-gigabyte datasets into it, and sends the single file descriptor over the Unix socket using `SCM_RIGHTS`.
> The receiving process receives an actual descriptor pointing to the identical open file description in the kernel file table and calls `mmap()` on it.
> This combines the control plane advantages of message passing (authenticated descriptor handover) with the zero-copy bulk data bandwidth of shared memory.

---

## 4. Hardware Synchronization and Cache Coherence

> [!question] Question
> On a Symmetric Multiprocessing (SMP) system with 32 CPU cores connected via a shared bus using a write-invalidate cache coherence protocol (e.g. MESI):
> 
> 1. Contrast the interconnect bus traffic and algorithmic complexity of the **Test-and-Set (TAS)** spinlock versus the **Test-and-Test-and-Set (TATAS)** spinlock when all 32 cores contend for a lock.
> 2. What fundamental cache line defect causes the **Ticket Lock** to generate $O(N)$ invalidations on lock release, and how does the **MCS Lock** eliminate this issue?
> 3. What is the difference between write-invalidate and write-update protocols in terms of network message complexity under high write contention?

> [!success]- Answer
> **1. TAS vs. TATAS Interconnect Traffic:**
> - **Test-and-Set (TAS):**
> Every contending core repeatedly executes an atomic read-modify-write instruction (e.g. `XCHG` or `CMPXCHG`).
> Atomic write operations require exclusive cache line ownership.
> Therefore, every failed spin attempt issues an invalidation broadcast across the shared bus, forcing all other 31 cores to invalidate their local L1/L2 cache lines and stall on bus arbitration.
> For $N$ contending processors, TAS generates **$O(N^2)$ bus transactions**, completely saturating the memory bus.
> - **Test-and-Test-and-Set (TATAS):**
> Contending cores first read the lock variable in a standard read loop without atomic writes (`while (*lock == 1)`).
> The cache line enters the **Shared (S)** state and is cached locally across all 32 cores.
> Cores spin entirely within their private L1 caches, generating **zero bus traffic** while the lock is held.
> However, when the holder releases the lock (`*lock = 0`), the single write invalidates all 31 cached copies.
> All 31 cores wake up simultaneously and execute `test_and_set()`, creating a momentary **thundering herd storm** of $O(N^2)$ invalidations before one core succeeds and the rest return to local spinning.
> 
> ---
> 
> **2. Ticket Lock vs. MCS Lock:**
> - **Defect of Ticket Lock:**
> The Ticket Lock enforces FIFO fairness using two counters: `next_ticket` and `now_serving`.
> To acquire the lock, a thread fetches a ticket and spins reading `now_serving`.
> All contending threads spin on the **same single shared memory location** (`now_serving`).
> When the lock holder releases the lock, it executes `now_serving++`.
> This write invalidates the cache line in *every* contending core's cache ($O(N)$ invalidations), causing all $N$ cores to issue bus read requests to reload the new value, even though only one thread possesses the winning ticket.
> - **MCS Lock Solution:**
> The MCS lock (Mellor-Crummey and Scott) organizes waiting threads into a linked list where each thread allocates an `mcs_node` structure containing a private boolean flag (`waiting`).
> Each thread spins **strictly on its own private flag** located on its own distinct cache line:
> 
> ```c
> while (my_node.waiting == true) { /* spin on local cache line */ }
> ```
> 
> When the lock holder releases the lock, it inspects `my_node.next` and writes `false` exclusively into the immediate successor's `waiting` flag.
> Exactly **one cache line is invalidated**, reducing release bus traffic from $O(N)$ to $O(1)$.
> 
> ---
> 
> **3. Write-Invalidate vs. Write-Update Complexity:**
> - **Write-Invalidate:** The first write invalidates all remote copies.
> Subsequent writes by the same processor hit the local cache in Modified (M) state with zero bus transactions.
> Interconnect traffic is bounded by $O(N)$ per burst of writes.
> - **Write-Update:** Every single store instruction broadcasts the updated data word across the bus to update all remote cache lines.
> If a processor writes to a contended variable 1,000 times in a loop, it generates 1,000 bus broadcasts ($O(N \times \text{writes})$), rapidly exhausting memory interconnect bandwidth.

---

## 5. UNIX Inode Capacity and File Addressing

> [!question] Question
> A UNIX-like operating system uses an inode structure with the following disk addressing layout:
> - 12 direct block pointers
> - 1 single indirect block pointer
> - 1 double indirect block pointer
> - 1 triple indirect block pointer
> 
> Assume the file system block size is $4\text{ KB}$ ($4,096$ bytes) and each disk block address pointer requires $4\text{ bytes}$ (32-bit block addresses).
> 
> 1. Calculate how many disk block addresses fit inside a single indirect block.
> 2. Calculate the maximum file size addressable by:
>    - The direct pointers only.
>    - The single indirect pointer only.
>    - The double indirect pointer only.
>    - The triple indirect pointer only.
> 3. Calculate the total theoretical maximum file size supported by this inode structure.

> [!success]- Answer
> **1. Pointers Per Block:**
> $$\text{Pointers per Block} (P) = \frac{\text{Block Size}}{\text{Pointer Size}} = \frac{4,096\text{ bytes}}{4\text{ bytes}} = 1,024 = 2^{10}\text{ pointers}$$
> 
> ---
> 
> **2. Capacity Per Addressing Level:**
> - **Direct Pointers (12 pointers):**
> 
> $$\text{Capacity}_{direct} = 12 \times 4\text{ KB} = 48\text{ KB}$$
> 
> - **Single Indirect Pointer (1 pointer):**
> Points to 1 block containing 1,024 block pointers.
> 
> $$\text{Capacity}_{single} = 1,024 \times 4\text{ KB} = 4,096\text{ KB} = 4\text{ MB}$$
> 
> - **Double Indirect Pointer (1 pointer):**
> Points to 1 block containing 1,024 single indirect block pointers, each pointing to 1,024 data blocks.
> 
> $$\text{Capacity}_{double} = 1,024 \times 1,024 \times 4\text{ KB} = 1,048,576 \times 4\text{ KB} = 4,194,304\text{ KB} = 4\text{ GB}$$
> 
> - **Triple Indirect Pointer (1 pointer):**
> Points to 1 block containing 1,024 double indirect block pointers.
> 
> $$\text{Capacity}_{triple} = 1,024 \times 1,024 \times 1,024 \times 4\text{ KB} = 2^{30} \times 4\text{ KB} = 4\text{ TB}$$
> 
> ---
> 
> **3. Total Maximum File Size:**
> 
> $$\text{Total Size} = 48\text{ KB} + 4\text{ MB} + 4\text{ GB} + 4\text{ TB} \approx \mathbf{4.004\text{ TB}}$$
> 
> *(Note: The 32-bit block address field limits the overall file system to $2^{32} \times 4\text{ KB} = 16\text{ TB}$ of addressable storage).*

---

## 6. Disk Scheduling Algorithms

> [!question] Question
> A magnetic disk drive has 200 cylinders numbered 0 to 199.
> The disk head is currently positioned at cylinder **53** and was previously at cylinder 35 (the head is moving in the **direction of increasing cylinder numbers**).
> The queue of pending I/O cylinder requests in arrival order is:
> 
> $$\text{Queue} = \langle 98, 183, 37, 122, 14, 124, 65, 67 \rangle$$
> 
> 1. Compute the sequence of head movements and the total seek distance (in cylinders) for:
>    - **Shortest Seek Time First (SSTF)**
>    - **SCAN (Elevator Algorithm)**
>    - **C-SCAN (Circular SCAN)**
>    - **LOOK**
> 2. Why is SSTF susceptible to starvation while SCAN guarantees bounded waiting times?

> [!success]- Answer
> **1. Algorithm Traces and Total Seek Distances:**
> 
> #### A. Shortest Seek Time First (SSTF)
> At each step, select the request closest to the current head position.
> 
> - Start at 53:
>   - Closest to 53 is 65 (distance 12)
>   - From 65, closest is 67 (distance 2)
>   - From 67, closest is 37 (distance 30; candidates: 37 dist 30, 98 dist 31)
>   - From 37, closest is 14 (distance 23)
>   - From 14, closest is 98 (distance 84)
>   - From 98, closest is 122 (distance 24)
>   - From 122, closest is 124 (distance 2)
>   - From 124, closest is 183 (distance 59)
> - Sequence: $53 \to 65 \to 67 \to 37 \to 14 \to 98 \to 122 \to 124 \to 183$
> - Seek distance:
>   $$(65-53) + (67-65) + (67-37) + (37-14) + (98-14) + (122-98) + (124-122) + (183-124)$$
>   $$= 12 + 2 + 30 + 23 + 84 + 24 + 2 + 59 = \mathbf{236\text{ cylinders}}$$
> 
> ---
> 
> #### B. SCAN (Elevator Algorithm)
> Moves in the current direction (increasing) all the way to the disk edge (199), servicing requests along the path, then reverses direction toward 0.
> 
> - Upward sweep: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 199$
> - Downward sweep: $199 \to 37 \to 14$
> - Sequence: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 199 \to 37 \to 14$
> - Seek distance:
>   $$(199 - 53) + (199 - 14) = 146 + 185 = \mathbf{331\text{ cylinders}}$$
> 
> ---
> 
> #### C. C-SCAN (Circular SCAN)
> Moves upward to the disk edge (199), servicing requests, jumps back to cylinder 0 without servicing requests, and sweeps upward again.
> 
> - Upward sweep: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 199$
> - Reset: $199 \to 0$
> - Second sweep: $0 \to 14 \to 37$
> - Sequence: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 199 \to [0] \to 14 \to 37$
> - Seek distance:
>   $$(199 - 53) + (199 - 0) + (37 - 0) = 146 + 199 + 37 = \mathbf{382\text{ cylinders}}$$
> 
> ---
> 
> #### D. LOOK
> Same as SCAN, but the head reverses as soon as the last request in that direction is reached, without traveling to the physical platter edge (199).
> 
> - Upward sweep: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183$ (highest request is 183)
> - Downward sweep: $183 \to 37 \to 14$
> - Sequence: $53 \to 65 \to 67 \to 98 \to 122 \to 124 \to 183 \to 37 \to 14$
> - Seek distance:
>   $$(183 - 53) + (183 - 14) = 130 + 169 = \mathbf{299\text{ cylinders}}$$
> 
> ---
> 
> **2. Starvation in SSTF vs. Bounded Wait in SCAN:**
> - In SSTF, if new I/O requests arrive continuously near the current head position (e.g. around cylinders 50-70), the head remains in that local cluster servicing them because their seek distance is small.
> Requests far away (such as cylinder 14 or 183) will starve indefinitely.
> - SCAN sweeps the disk in a single continuous direction.
> Any request ahead of the arm is serviced during the current sweep, and requests behind it are serviced on the return sweep, guaranteeing an upper bound on waiting time of at most two full platter sweeps.

---

## 7. Popek-Goldberg Theorem and x86 Virtualization

> [!question] Question
> 1. State the **Popek-Goldberg Virtualization Theorem** regarding the relationship between sensitive instructions and privileged instructions for a processor architecture to be classically virtualizable via trap-and-emulate.
> 2. Why was the original x86-32 architecture notoriously unvirtualizable according to the Popek-Goldberg theorem?
> Provide two specific examples of sensitive x86 instructions that failed this test.
> 3. In Hardware-Assisted Virtualization (Intel VT-x / AMD-V), what is a **Two-Dimensional Page Walk** (Extended Page Tables - EPT), and what is its maximum memory access penalty in the worst case during a TLB miss?

> [!success]- Answer
> **1. Popek-Goldberg Theorem:**
> A computer architecture is **classively virtualizable** if and only if all **sensitive instructions** are a strict subset of **privileged instructions**:
> 
> $$\text{Sensitive Instructions} \subseteq \text{Privileged Instructions}$$
> 
> - **Sensitive Instructions:** Instructions that manipulate hardware configurations (e.g. MMU page tables, interrupt enable flags) or behave differently depending on execution privilege level.
> - **Privileged Instructions:** Instructions that trap when executed in user mode (unprivileged mode).
> 
> If all sensitive instructions trap in user mode, a Type-1 hypervisor can run the Guest OS in user mode; whenever the Guest OS attempts to touch hardware, the CPU traps to the hypervisor, which emulates the operation safely (Trap-and-Emulate).
> 
> ---
> 
> **2. Why x86-32 Failed the Popek-Goldberg Condition:**
> In x86-32, there were **17 sensitive unprivileged instructions** (the "virtualization virtualization holes").
> When executed in user mode (Ring 1 or Ring 3), these instructions **did not trap to the hypervisor**; instead, they failed silently or exposed real hardware state.
> Examples:
> 1. **`POPF` (Push/Pop to Interrupt Flag):** Modifies the hardware interrupt enable flag (`IF`) in `EFLAGS`.
> In kernel mode (Ring 0), it changes the interrupt mask.
> In user mode (Ring 1/3), it executes without raising an exception; the CPU simply ignores modifications to the `IF` bit.
> A Guest OS attempting to disable interrupts believes interrupts are disabled, while hardware interrupts continue firing.
> 2. **`SGDT` / `SIDT` / `SLDT` (Store Global/Interrupt/Local Descriptor Table):** Reads the hardware register containing the pointer to the base interrupt/segment table into a memory buffer.
> These instructions are sensitive because they expose the host hypervisor's base register address to the guest, but they are unprivileged and can be executed in Ring 3 without trapping.
> 
> ---
> 
> **3. Extended Page Tables (EPT) and the 2D Page Walk:**
> - In VT-x with EPT, there are two layers of address translation:
>   1. Guest Virtual Address (GVA) $\to$ Guest Physical Address (GPA) (managed by Guest OS page tables).
>   2. Guest Physical Address (GPA) $\to$ Host Physical Address (HPA) (managed by Hypervisor EPT tables).
> - On a TLB miss, the hardware MMU must perform a **2D page walk**.
> For a 4-level 64-bit architecture:
>   - Translating the root Guest CR3 requires walking the 4 levels of EPT ($4\text{ memory references}$).
>   - Reading the Level 4 Guest PTE requires translating its GPA through 4 levels of EPT ($4\text{ references}$).
>   - Reading the Level 3 Guest PTE requires 4 EPT references ($4\text{ references}$).
>   - Reading the Level 2 Guest PTE requires 4 EPT references ($4\text{ references}$).
>   - Reading the Level 1 Guest PTE requires 4 EPT references ($4\text{ references}$).
>   - Finally, fetching the actual target data requires translating the target GPA through 4 EPT levels ($4\text{ references}$) plus the final physical read ($1\text{ reference}$).
> - **Total Worst-Case Memory References:**
> 
> $$\text{Total References} = 4 \times 4 + 4 + 4 + 1 = \mathbf{24\text{ memory accesses for a single memory instruction!}}$$
> 
> This immense penalty illustrates why hardware TLBs that cache direct GVA $\to$ HPA mappings (such as Intel Virtual Processor IDs - VPID) are critical for virtualization performance.

---

## 8. RPC Semantics and XDR Wire Serialization

> [!question] Question
> 1. Compare the failure behavior and suitability of **At-Least-Once**, **At-Most-Once**, and **Exactly-Once** Remote Procedure Call (RPC) execution semantics over an unreliable network.
> 2. Under SunRPC eXternal Data Representation (XDR), data types are aligned to 4-byte boundaries on the wire.
> Calculate the exact serialized size on the network wire (in bytes) of an RPC request containing:
>    - A 32-bit unsigned integer.
>    - A string containing 9 ASCII characters: `"gatech6200"`.
>    - A dynamic array of five 16-bit integers (`short int`).

> [!success]- Answer
> **1. RPC Semantics Comparison:**
> 
> | Semantic | Mechanism | Failure Mode | Use Case |
> | :--- | :--- | :--- | :--- |
> | **At-Least-Once** | Client retries RPC continuously until an ACK/reply is received. | If replies are lost, the server executes the procedure multiple times. | **Idempotent operations** only (e.g. `read_page()`, `get_balance()`). Catastrophic for non-idempotent operations (e.g. `withdraw_money()`). |
> | **At-Most-Once** | Server maintains a duplicate request cache keyed by transaction ID. If a duplicate arrives, the server returns the cached result without re-executing. | If the server crashes before caching, the call may execute 0 times; if it crashes after, 1 time. | Non-idempotent operations where duplicated execution is dangerous. |
> | **Exactly-Once** | Requires distributed 2-phase commit (2PC) or consensus protocols (Paxos/Raft) with persistent write-ahead logging. | Heavy latency and synchronization overhead. | Financial transactions, database commits. |
> 
> ---
> 
> **2. XDR Wire Sizing Calculation:**
> In XDR, every primitive and composite data structure is encoded as a multiple of 4 bytes (32 bits).
> 
> 1. **32-Bit Unsigned Integer:**
> Encoded directly as 4 bytes in network byte order (big-endian).
> Size = **4 bytes**.
> 2. **String `"gatech6200"`:**
> Length of `"gatech6200"` is 10 characters.
> XDR encodes a variable-length string as:
> - A 4-byte integer length prefix ($L = 10$).
> - The 10 bytes of ASCII characters.
> - Zero-padding up to the next 4-byte boundary.
> The next multiple of 4 after 10 is 12 (requires $12 - 10 = 2$ bytes of zero padding).
> Total string size = $4\text{ (length prefix)} + 10\text{ (data)} + 2\text{ (padding)} = \mathbf{16\text{ bytes}}$.
> 3. **Dynamic Array of Five 16-bit Integers:**
> XDR encodes dynamic arrays as:
> - A 4-byte element count prefix ($M = 5$).
> - In XDR, integers smaller than 32 bits (such as `short` or `char`) are individually padded to 4 bytes on the wire.
> Each 16-bit short consumes 4 bytes.
> Total array size = $4\text{ (count prefix)} + (5 \times 4\text{ bytes}) = 4 + 20 = \mathbf{24\text{ bytes}}$.
> 
> **Total Serialized Payload on Wire:**
> 
> $$\text{Total Size} = 4\text{ bytes (int)} + 16\text{ bytes (string)} + 24\text{ bytes (array)} = \mathbf{44\text{ bytes}}$$

---

## 9. Distributed File Systems Caching and ESTALE

> [!question] Question
> 1. Compare the cache validation mechanism and server scalability limits of the **Network File System (NFS v3)** versus the **Andrew File System (AFS)**.
> 2. An engineer working on an NFS mount executes `cat file.txt` and receives the error `ESTALE (errno 116: Stale file handle)`.
> Explain step-by-step the server and inode lifecycle events that triggered this error.
> 3. In the Sprite distributed file system (Ousterhout et al.), how does the central server maintain consistency when two clients open the same file concurrently, where at least one client opens for write?

> [!success]- Answer
> **1. NFS v3 vs. AFS Caching Architecture:**
> - **NFS v3 (Polling & Stateless):**
> Clients cache file blocks in RAM.
> To validate freshness, clients periodically send `GETATTR` RPC requests to the server (every 3 to 60 seconds).
> Because validation is driven by client polling, server load scales linearly with the number of active clients ($O(\text{clients})$).
> A busy NFS server spends most of its CPU cycles answering attribute checks for unchanged files.
> Scales to roughly hundreds of clients.
> - **AFS (Callbacks & Stateful):**
> Clients cache entire files or large chunks on local client persistent disk partitions.
> When a client downloads a file, the server issues a **callback promise** guaranteeing that the server will notify the client if any other client commits modifications to that file.
> The client reads from its local disk cache indefinitely with zero network traffic.
> When a writer closes a modified file, the server pushes callback break messages to invalidate caches.
> Server load is proportional only to the file modification rate, scaling cleanly to thousands of clients.
> 
> ---
> 
> **2. Lifecycle Breakdown of `ESTALE`:**
> 1. Client A opens `file.txt`.
> The NFS server returns an opaque 32-byte **file handle** containing the filesystem ID (`fsid`), the inode number (`ino = 48201`), and an inode generation number (`generation = 1`).
> 2. Client A caches the file handle.
> 3. Meanwhile, Client B (or an administrative user logged directly into the file server) unlinks and deletes `file.txt`.
> 4. The server's local underlying filesystem (e.g. ext4 or XFS) deallocates inode 48201 and marks it free.
> 5. Later, a new file `another_file.log` is created on the server.
> The filesystem reallocates the recycled inode 48201 to the new file and increments the generation count to `generation = 2`.
> 6. Client A issues a `read()` RPC using its cached file handle (`ino = 48201, generation = 1`).
> 7. The server inspects inode 48201 on disk and discovers that the stored generation count (2) does not match the file handle (1).
> 8. The server recognizes that the file originally referenced by Client A has been deleted, and returns the error code `NFS3ERR_STALE` (`ESTALE`).
> 
> ---
> 
> **3. Sprite DFS Concurrent Write-Sharing Protocol:**
> 1. Sprite clients aggressively cache file blocks in memory.
> 2. The central Sprite server tracks active open sessions for all files in a central table.
> 3. When Client 2 opens a file for writing while Client 1 already has it open, the server detects **concurrent write-sharing**.
> 4. The server signals the writer to flush all modified dirty blocks back to the server.
> 5. The server issues a control message instructing both Client 1 and Client 2 to **disable local caching** for that file.
> 6. For the remainder of the shared session, all reads and writes bypass local client caches and travel synchronously across the network to the server buffer cache.
> 7. When the concurrent session terminates (files are closed), client caching is restored.

---

## 10. Memory Consistency Models and IVY DSM

> [!question] Question
> 1. Arrange the following consistency models in order from **strictest** to **most relaxed**:
>    - Release Consistency (RC)
>    - Strict Consistency
>    - Causal Consistency
>    - Sequential Consistency
> 2. In the IVY Distributed Shared Memory system (Li & Hudak), describe the exact sequence of network messages that occur when a node experiences a **Write Fault** under the **Dynamic Distributed Manager with Broadcast** scheme.

> [!success]- Answer
> **1. Ordering of Consistency Models:**
> 
> $$\text{Strict Consistency} \succ \text{Sequential Consistency} \succ \text{Causal Consistency} \succ \text{Release Consistency}$$
> 
> - **Strict:** All writes are immediately visible across all nodes in absolute global real time.
> Requires instant signal propagation ($c = \infty$), impossible in distributed systems.
> - **Sequential:** Operations appear in some global interleaved sequence respecting program order, and all processors observe the identical global order.
> - **Causal:** Only causally related writes ($A \rightarrow B$) must be observed in the same order by all nodes; concurrent writes may be observed in different orders.
> - **Release:** Memory operations are ordered only relative to explicit synchronization operations (`Acquire` and `Release`).
> Ordinary reads and writes can be reordered freely between sync points.
> 
> ---
> 
> **2. IVY Write Fault Sequence (Dynamic Distributed Manager):**
> 1. **Page Fault Trap:** Node $k$ executes a store instruction to an address on Page $P$.
> The page is marked read-only or invalid in Node $k$'s page table.
> The hardware MMU triggers a `SIGSEGV`, which is caught by IVY.
> 2. **Manager Inquiry:** Node $k$ checks its local hint table (`probOwner`).
> It sends an ownership request message to the designated manager for Page $P$.
> 3. **Invalidation Broadcast:**
> - The manager retrieves Page $P$'s record.
> - The manager sends an invalidation request to all nodes listed in Page $P$'s **copy set** (all nodes possessing read-only copies).
> - Every node in the copy set marks its local page table entry as invalid (`PROT_NONE`) and sends an acknowledgment back.
> 4. **Data Transfer:**
> - The previous owner transmits the full 4 KB page data across the network to Node $k$.
> 5. **State Update:**
> - Node $k$ becomes the new authoritative owner of Page $P$.
> - The manager updates its directory: `Owner = Node k`, `CopySet = { Node k }`.
> 6. **Protection Elevation:**
> - Node $k$ sets the page permissions to `PROT_READ | PROT_WRITE` in its local page table and resumes user instruction execution.

---

## 11. Datacenter Technologies and Amdahl's Law

> [!question] Question
> 1. A distributed datacenter analytics job has $90\%$ of its computation structured as perfectly parallelizable Map tasks, while $10\%$ is an inherently serial final aggregation stage.
>    - Using **Amdahl's Law**, calculate the theoretical maximum speedup achievable if the job is run on a cluster of 90 worker nodes.
>    - What is the absolute upper bound on speedup as the number of nodes approaches infinity ($N \to \infty$)?
> 2. A cloud datacenter contains $N = 1,000$ commodity server nodes.
> The probability of any individual server failing during a 24-hour period is $p = 0.002$ ($0.2\%$).
> Assuming server failures are independent, what is the probability that **at least one server crashes** in the datacenter during that 24-hour window?

> [!success]- Answer
> **1. Amdahl's Law Calculations:**
> Amdahl's Law defines speedup $S(N)$ for parallel fraction $P$ and serial fraction $(1 - P)$ on $N$ processors:
> 
> $$S(N) = \frac{1}{(1 - P) + \frac{P}{N}}$$
> 
> Given $P = 0.90$ and $1 - P = 0.10$:
> - **For $N = 90$ nodes:**
> 
> $$S(90) = \frac{1}{0.10 + \frac{0.90}{90}} = \frac{1}{0.10 + 0.01} = \frac{1}{0.11} \approx \mathbf{9.09\times\text{ speedup}}$$
> 
> Adding 90 server nodes achieves only a $9\times$ speedup because the $10\%$ serial fraction rapidly dominates runtime.
> 
> - **As $N \to \infty$:**
> 
> $$S_{max} = \lim_{N \to \infty} \frac{1}{(1 - P) + \frac{P}{N}} = \frac{1}{1 - P} = \frac{1}{0.10} = \mathbf{10.0\times\text{ speedup}}$$
> 
> No matter how many thousands of nodes are provisioned, the job can never run faster than $10\times$ its single-node speed.
> 
> ---
> 
> **2. Datacenter Cluster Failure Probability:**
> - Probability that a single node does *not* fail:
> 
> $$P(\text{no fail}) = 1 - p = 1 - 0.002 = 0.998$$
> 
> - Probability that *all* 1,000 nodes survive without failure:
> 
> $$P(\text{all survive}) = (1 - p)^N = (0.998)^{1,000}$$
> 
> Using the approximation $(1 - x)^N \approx e^{-Nx}$ for small $x$:
> 
> $$P(\text{all survive}) \approx e^{-(1,000 \times 0.002)} = e^{-2} \approx 0.1353\ (13.53\%)$$
> 
> - Probability that **at least one server fails**:
> 
> $$P(\ge 1\text{ failure}) = 1 - P(\text{all survive}) = 1 - 0.1353 = \mathbf{0.8647\ (86.47\%)}$$
> 
> In a cluster of 1,000 commodity nodes, there is an $86.5\%$ probability of experiencing a machine crash every single day.
> This mathematical reality dictates why modern datacenter software frameworks (MapReduce, GFS, HDFS, Kubernetes) must treat hardware failures as routine operational events rather than exceptional conditions.

---

## Final Exam Revision Checklist

- [ ] Q1 - can derive EAT for multi-level page tables with TLB hits and misses ($EAT = \alpha T_{hit} + (1-\alpha)T_{miss}$)
- [ ] Q2 - can trace Belady's anomaly on FIFO and explain why stack algorithms (LRU) are immune
- [ ] Q3 - can calculate the copy vs map threshold ($2 \times \frac{N}{BW} = T_{overhead}$) and explain `SCM_RIGHTS`
- [ ] Q4 - can explain TAS $O(N^2)$ vs TATAS thundering herd storm vs MCS lock $O(1)$ local spinning
- [ ] Q5 - can calculate UNIX inode addressing limits across direct, single, double, and triple indirect pointers
- [ ] Q6 - can trace SSTF, SCAN, C-SCAN, and LOOK and calculate total head seek distances
- [ ] Q7 - can state the Popek-Goldberg condition ($\text{Sensitive} \subseteq \text{Privileged}$) and explain x86 holes (`POPF`/`SGDT`)
- [ ] Q8 - can compare RPC semantics (at-least-once vs at-most-once) and calculate XDR 4-byte padding wire sizes
- [ ] Q9 - can explain NFS attribute polling vs AFS callbacks, Sprite write-sharing token revocation, and `ESTALE`
- [ ] Q10 - can order consistency models and trace IVY dynamic manager write fault invalidation broadcasts
- [ ] Q11 - can solve distributed Amdahl's Law speedups and calculate cluster failure probabilities ($1 - (1-p)^N$)
