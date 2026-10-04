---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L2"
  - "Operating System Concepts, 10th Ed., Silberschatz"
  - "Understanding the Linux Kernel, Bovet & Cesati"
  - "Windows Internals, 7th Ed., Russinovich"
---

# P3L2: Memory Management

> **Module goal:** Master virtual memory, page tables (single-level, hierarchical, inverted), TLB, page fault handling, page replacement algorithms (FIFO, Optimal, LRU, Clock), Belady's anomaly, and memory allocators (buddy, slab).

## Table of Contents

- [1. Physical vs. Virtual Memory](#1-physical-vs-virtual-memory)
- [2. Memory Management Responsibilities](#2-memory-management-responsibilities)
- [3. Address Translation and Memory Protection](#3-address-translation-and-memory-protection)
- [4. Fixed and Dynamic Partitioning](#4-fixed-and-dynamic-partitioning)
- [5. Paging Architecture](#5-paging-architecture)
- [6. The Page Table Structure](#6-the-page-table-structure)
- [7. Page Table Entries and Flags](#7-page-table-entries-and-flags)
- [8. Hierarchical / Multi-Level Page Tables](#8-hierarchical--multi-level-page-tables)
- [9. Inverted Page Tables](#9-inverted-page-tables)
- [10. Translation Lookaside Buffer (TLB)](#10-translation-lookaside-buffer-tlb)
- [11. Page Fault Handling](#11-page-fault-handling)
- [12. Page Replacement Algorithms](#12-page-replacement-algorithms)
- [13. Belady's Anomaly](#13-beladys-anomaly)
- [14. Memory Allocation: Buddy Allocator](#14-memory-allocation-buddy-allocator)
- [15. Memory Allocation: Slab Allocator](#15-memory-allocation-slab-allocator)
- [16. Quizzes and Exercises](#16-quizzes-and-exercises)
- [17. Key Takeaways](#17-key-takeaways)
- [18. Huge Pages: Reducing TLB Pressure](#18-huge-pages-reducing-tlb-pressure)
- [19. Memory Compaction, Swapping, and ZRAM](#19-memory-compaction-swapping-and-zram)
- [20. eBPF Memory Tracing](#20-ebpf-memory-tracing)
- [21. Windows Virtual Memory Internals](#21-windows-virtual-memory-internals)
- [22. macOS Memory Architecture and Darwin Internals](#22-macos-memory-architecture-and-darwin-internals)

---

## 1. Physical vs. Virtual Memory

```
Physical Memory (DRAM):                Virtual Memory (per process):
+---------------------------+          +---------------------------+
| Frame 0 | Frame 1 | ...  |          | Page 0 (code)             |
| Frame 2 | Frame 3 | ...  |          | Page 1 (data)             |
| ...                       |          | Page 2 (heap)             |
| Frame N-1                 |          | ...                       |
+---------------------------+          | Page M (stack)            |
                                       +---------------------------+
Finite (4 GB, 16 GB, 64 GB)           Much larger (128 TB on x86-64)
Shared by ALL processes                Private to each process
Contiguous physical addresses           Contiguous virtual addresses
                                        (may map to non-contiguous physical)
```

Virtual memory provides:
- **Isolation:** Each process has its own address space
- **Illusion of large memory:** Process can use more memory than physical RAM (via swapping)
- **Simplification:** Compiler/linker can use fixed addresses without worrying about other processes
- **Sharing:** Multiple processes can map the same physical frame (shared libraries, COW after fork)

---

## 2. Memory Management Responsibilities

| Responsibility | Description |
|---------------|-------------|
| **Address translation** | Convert virtual addresses to physical addresses |
| **Memory protection** | Prevent processes from accessing each other's memory |
| **Demand paging** | Load pages only when accessed (lazy allocation) |
| **Page replacement** | Decide which pages to evict when physical memory is full |
| **Memory allocation** | Manage free physical frames and virtual address ranges |
| **Shared memory** | Allow controlled sharing of physical frames between processes |
| **Swap management** | Move pages between RAM and disk |

---

## 3. Address Translation and Memory Protection

```
Virtual Address (from CPU):
+--------------------+------------------+
| Virtual Page Number| Page Offset      |
| (VPN)              | (unchanged)      |
+--------------------+------------------+
         |
         v
    Page Table Lookup
    (per-process)
         |
         v
+--------------------+------------------+
| Physical Frame Num | Page Offset      |
| (PFN)              | (same offset)    |
+--------------------+------------------+
= Physical Address (sent to memory bus)
```

**Example with 32-bit addresses, 4 KB pages:**
```
Virtual address: 0x00403A7C

Page size = 4 KB = 2^12 = 4096 bytes
Offset bits = 12
VPN bits = 32 - 12 = 20

VPN = 0x00403A7C >> 12 = 0x00403
Offset = 0x00403A7C & 0xFFF = 0xA7C

Page table lookup: VPN 0x00403 -> PFN 0x5B2F1 (example)

Physical address = (0x5B2F1 << 12) | 0xA7C = 0x5B2F1A7C
```

**Memory protection** is enforced via permission bits in each page table entry:
- Read/Write/Execute permissions
- User/Supervisor bit (kernel pages not accessible from user mode)
- If a process accesses a page it doesn't have permission for: **segfault** (SIGSEGV on Linux, Access Violation on Windows)

---

## 4. Fixed and Dynamic Partitioning

**Historical approaches** before paging:

### Fixed Partitioning
```
Physical Memory:
|  Partition 0  |  Partition 1  |  Partition 2  |  Partition 3  |
|    128 KB     |    256 KB     |    512 KB     |    1 MB       |
|  [Process A]  |  [Process B]  |   [empty]     |  [Process C]  |

Problem: Internal fragmentation (process doesn't fill partition)
```

### Dynamic Partitioning
```
Physical Memory (after some allocations and deallocations):
| P1 (200KB) | free (50KB) | P2 (300KB) | free (100KB) | P3 (150KB) | free (200KB) |

Problem: External fragmentation (enough total free space but not contiguous)
Needs compaction (expensive: copy all processes to make one large free block)
```

**Paging solves both** by dividing memory into fixed-size pages/frames and allowing non-contiguous allocation.

---

## 5. Paging Architecture

```
Virtual Address Space          Physical Memory (RAM)
+-----------+                  +-----------+
| Page 0    |---+         +--->| Frame 0   |
+-----------+   |         |   +-----------+
| Page 1    |---+--+      |   | Frame 1   |<--- Page 3
+-----------+   |  |      |   +-----------+
| Page 2    |---+  |      |   | Frame 2   |<--- Page 0
+-----------+   |  |      |   +-----------+
| Page 3    |---+  +------|-->| Frame 3   |<--- Page 1
+-----------+             |   +-----------+
                          +---| Frame 4   |<--- (another process's page)
                              +-----------+
                              | Frame 5   |<--- Page 2
                              +-----------+

Pages and frames are the SAME SIZE (typically 4 KB).
The page table maps VPN -> PFN.
No external fragmentation. Internal fragmentation is at most 1 page.
```

**Page sizes on modern systems:**
| OS | Default Page Size | Huge Pages |
|----|-------------------|------------|
| Linux x86-64 | 4 KB | 2 MB, 1 GB |
| Windows x86-64 | 4 KB | 2 MB |
| ARM64 | 4 KB, 16 KB, 64 KB | 2 MB, 512 MB, 1 GB |

```bash
# Linux: check page size
getconf PAGESIZE     # 4096

# Check hugepage availability
cat /proc/meminfo | grep -i huge

# Transparent Huge Pages (THP) status
cat /sys/kernel/mm/transparent_hugepage/enabled
```

---

## 6. The Page Table Structure

A simple **single-level page table** is an array indexed by VPN:

```
Page Table (one per process):
+-------+-------+-----------+---------+
| Index | Valid  | Frame Num | Flags   |
+-------+-------+-----------+---------+
|   0   |   1   |  0x5B2F1  | RW-U    |
|   1   |   1   |  0x3A001  | R-XU    |
|   2   |   0   |  ------   | ----    |  <- not in memory (page fault if accessed)
|   3   |   1   |  0x72100  | RW-U    |
|  ...  |  ...  |   ...     | ...     |
+-------+-------+-----------+---------+

CR3 register (x86) / TTBR (ARM) points to the base of this table.
Changed on context switch to switch address spaces.
```

**Problem with single-level page table:** Size.

For 32-bit address space with 4 KB pages:
- Number of pages = 2^32 / 2^12 = 2^20 = 1,048,576 entries
- Each PTE = 4 bytes
- Page table size = 4 MB **per process**

For 64-bit address space (48-bit used) with 4 KB pages:
- Number of pages = 2^48 / 2^12 = 2^36 = 68 billion entries
- Page table size = 512 GB per process! Completely impractical.

---

## 7. Page Table Entries and Flags

Each PTE contains the physical frame number and control flags:

```
x86-64 Page Table Entry (64 bits):
+---+---+---+---+---+---+---+---+---+---------+-----------+
|NX | . | . | G | . | D | A | . |U/S|  R/W    |  Present  |
+---+---+---+---+---+---+---+---+---+---------+-----------+
 63   62  ...  8   7   6   5       2      1         0

Bits 12-51: Physical frame number (40 bits = 1 TB addressable)
```

| Bit | Name | Meaning |
|-----|------|---------|
| 0 | **Present/Valid** | Page is in physical memory (1) or not (0 -> page fault) |
| 1 | **Read/Write** | 0 = read-only, 1 = read-write |
| 2 | **User/Supervisor** | 0 = kernel only, 1 = user accessible |
| 5 | **Accessed** | Set by hardware when page is read |
| 6 | **Dirty** | Set by hardware when page is written |
| 63 | **NX (No Execute)** | 1 = page cannot be executed (security: W^X) |

> **Quiz: Page Table Mapping**
>
> *A system has 32-bit virtual addresses, 4 KB pages, and 32-bit physical addresses. Each PTE is 4 bytes. How large is the page table?*
>
> VPN bits = 32 - 12 = 20. Entries = 2^20 = 1,048,576. Size = 2^20 * 4 = **4 MB**.

---

## 8. Hierarchical / Multi-Level Page Tables

Solve the size problem by **only allocating page table pages for regions actually in use**.

### Two-Level Page Table (32-bit)

```
Virtual Address (32-bit, 4 KB pages):
+----------+----------+------------+
| L1 Index | L2 Index | Offset     |
| (10 bits)| (10 bits)| (12 bits)  |
+----------+----------+------------+

L1 Table (Page Directory): 2^10 = 1024 entries, each 4 bytes = 4 KB
  Points to L2 tables (or NULL if no pages mapped in that range)

L2 Table (Page Table): 2^10 = 1024 entries, each 4 bytes = 4 KB
  Points to physical frames

Lookup:
  CR3 -> L1 Table -> L1[L1_index] -> L2 Table -> L2[L2_index] -> PFN

Space savings: If only 3 regions are used (code, heap, stack),
only 3 L2 tables are allocated = 4 KB + 3 * 4 KB = 16 KB
instead of 4 MB for a flat table.
```

### Four-Level Page Table (x86-64)

```
Virtual Address (48-bit, 4 KB pages):
+-------+-------+-------+-------+----------+
| PML4  | PDPT  |  PD   |  PT   | Offset   |
| 9 bits| 9 bits| 9 bits| 9 bits| 12 bits  |
+-------+-------+-------+-------+----------+

CR3 -> PML4 (512 entries) -> PDPT (512 entries) -> PD (512 entries)
    -> PT (512 entries) -> Physical Frame

Each table = 512 entries * 8 bytes = 4 KB (fits in one page)

Total addressable: 2^48 = 256 TB
```

```
                    CR3
                     |
            +--------v--------+
            |  PML4 (512)     |  Level 4
            +--------+--------+
                     |
            +--------v--------+
            |  PDPT (512)     |  Level 3
            +--------+--------+
                     |
            +--------v--------+
            |  PD (512)       |  Level 2
            +--------+--------+
                     |
            +--------v--------+
            |  PT (512)       |  Level 1
            +--------+--------+
                     |
            +--------v--------+
            | Physical Frame  |
            +-----------------+
```

### Five-Level Page Tables (x86-64 with LA57)

Linux 4.14+ supports 5-level page tables for 57-bit virtual addresses (128 PB).

```bash
# Check if 5-level paging is enabled
grep la57 /proc/cpuinfo
# Or check kernel config
cat /boot/config-$(uname -r) | grep CONFIG_X86_5LEVEL
```

### Multi-Level Page Table Size Calculation

> **Example:** 64-bit system, 48-bit virtual address, 4 KB pages, 8-byte PTEs.
>
> - Offset bits: 12 (4 KB = 2^12)
> - VPN bits: 48 - 12 = 36
> - Entries per table: 4096 / 8 = 512 = 2^9
> - Levels needed: 36 / 9 = 4 levels
>
> For a process using only 1 MB of memory (256 pages):
> - 1 PML4 table: 4 KB
> - 1 PDPT: 4 KB
> - 1 PD: 4 KB
> - 1 PT: 4 KB
> - Total page table overhead: **16 KB** (vs. 512 GB for flat table)

---

## 9. Inverted Page Tables

Instead of one page table per process, use a single **global** table with one entry per physical frame:

```
Inverted Page Table (one for entire system):
+-------+-------+--------+-------+
| Frame | PID   | VPN    | Flags |
+-------+-------+--------+-------+
|   0   |  42   | 0x1A3  | RW-U  |
|   1   |  99   | 0x500  | R-XU  |
|   2   |  42   | 0x1A4  | RW-U  |
|  ...  |  ...  |  ...   |  ...  |
|  N-1  |  77   | 0x000  | R-XU  |
+-------+-------+--------+-------+

Size = number of physical frames * entry size
(Bounded by physical memory, not virtual address space)
```

**Lookup:** Search for (PID, VPN) in the table -> return frame number.
Linear search is O(n) - too slow. Use a **hash table** for O(1) average lookup.

**Used by:** PowerPC, IA-64 (Itanium), some MIPS. Not used by x86-64.

---

## 10. Translation Lookaside Buffer (TLB)

The TLB is a **hardware cache** for page table entries, stored in the CPU's MMU.

```
CPU generates virtual address
         |
         v
    +----------+
    |   TLB    |   <-- Check here first (fast: ~1-2 ns)
    |  lookup  |
    +----+-----+
    HIT  |  MISS
    |    |    |
    |    |    v
    |    | Page Table Walk (slow: ~10-100 ns for multi-level)
    |    |    |
    v    v    v
  Physical Address
```

### TLB Structure

```
TLB (typically 64-1536 entries, fully associative):
+-----+------+------+-------+
| VPN | PFN  | PID  | Flags |
+-----+------+------+-------+
| 0x3 | 0xAB | 42   | RWX   |  <-- HIT: VPN 0x3, PID 42 -> PFN 0xAB
| 0x7 | 0xCD | 99   | R-X   |
| ... | ...  | ...  | ...   |
+-----+------+------+-------+
```

### TLB Performance

```
TLB hit rate: typically 95-99%+ for well-behaved programs

Effective Memory Access Time (EMAT):
  EMAT = TLB_hit_rate * (TLB_time + mem_time)
       + TLB_miss_rate * (TLB_time + page_table_walk_time + mem_time)

Example:
  TLB_time = 1 ns
  mem_time = 100 ns
  page_table_walk = 400 ns (4-level, 100 ns per level)
  TLB hit rate = 98%

  EMAT = 0.98 * (1 + 100) + 0.02 * (1 + 400 + 100)
       = 0.98 * 101 + 0.02 * 501
       = 98.98 + 10.02
       = 109 ns  (close to single memory access!)
```

### TLB Flush on Context Switch

On a context switch, the TLB entries belong to the old process. Options:
1. **Flush entire TLB** - simple but expensive (all entries cold)
2. **Tagged TLB (PCID/ASID)** - each entry tagged with process ID; no flush needed

```bash
# Linux: check PCID support
grep pcid /proc/cpuinfo
# pcid = Process Context Identifiers (Intel)
# Avoids TLB flush on context switch

# Check KPTI status (Meltdown mitigation - adds TLB overhead)
cat /sys/devices/system/cpu/vulnerabilities/meltdown
```

---

## 11. Page Fault Handling

A **page fault** occurs when a process accesses a virtual page that is not currently in physical memory.

```
Page Fault Handling Flow:
  1. CPU generates virtual address
  2. TLB miss -> page table walk
  3. PTE's Present bit = 0 -> PAGE FAULT (trap to kernel)
  4. Kernel's page fault handler runs:
     a. Is the address valid? (in a mapped VMA?)
        NO  -> SIGSEGV (segmentation fault)
        YES -> continue
     b. Is the access permitted? (read-only page written?)
        NO  -> SIGSEGV or COW (copy-on-write) handling
        YES -> continue
     c. Is the page on disk (swapped out)?
        YES -> read from swap, allocate frame, update PTE
        NO  -> demand-zero page (first access), allocate frame
     d. If no free frame -> run page replacement algorithm
     e. Update PTE: set Present=1, PFN=new_frame
     f. Restart the faulting instruction

Types of page faults:
  Minor fault: page is in memory (e.g., page cache) but not mapped
  Major fault: page must be read from disk (swap or file)
```

**Linux - monitor page faults:**
```bash
# Per-process page fault counts
cat /proc/$PID/stat | awk '{print "minor:", $10, "major:", $12}'

# System-wide page fault rate
vmstat 1
# si = swap in, so = swap out
# pgfault column in sar

# Detailed page fault tracing
perf stat -e page-faults,minor-faults,major-faults ./my_program
```

**Windows:**
```powershell
# Page fault count for a process
Get-Process -Id $PID | Select-Object PageFaults

# Performance counter
Get-Counter '\Memory\Page Faults/sec'
Get-Counter '\Memory\Pages/sec'  # major faults (disk I/O)
```

---

## 12. Page Replacement Algorithms

When physical memory is full and a new page must be loaded, the OS must choose a **victim page** to evict.

### FIFO (First-In, First-Out)

Evict the **oldest** page (the one that has been in memory the longest).

```
Reference string: 7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2
Frames = 3

Step  Ref  Frame0  Frame1  Frame2  Fault?
 1     7     7       -       -      Yes
 2     0     7       0       -      Yes
 3     1     7       0       1      Yes
 4     2     2       0       1      Yes (evict 7)
 5     0     2       0       1      No  (0 already present)
 6     3     2       3       1      Yes (evict 0)
 7     0     2       3       0      Yes (evict 1)
 8     4     4       3       0      Yes (evict 2)
 9     2     4       2       0      Yes (evict 3)
10     3     4       2       3      Yes (evict 0)
11     0     0       2       3      Yes (evict 4)
12     3     0       2       3      No
13     2     0       2       3      No

Total faults: 10
```

### Optimal (MIN/OPT)

Evict the page that will not be used for the **longest time in the future**.

```
Same reference string: 7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2
Frames = 3

Step  Ref  Frame0  Frame1  Frame2  Fault?  Evicted
 1     7     7       -       -      Yes
 2     0     7       0       -      Yes
 3     1     7       0       1      Yes
 4     2     2       0       1      Yes     7 (never used again)
 5     0     2       0       1      No
 6     3     2       0       3      Yes     1 (used furthest: never)
 7     0     2       0       3      No
 8     4     2       4       3      Yes     0 (used at step 11, furthest)
 9     2     2       4       3      No
10     3     2       4       3      No
11     0     2       0       3      Yes     4 (never used again)
12     3     2       0       3      No
13     2     2       0       3      No

Total faults: 7 (optimal - can't do better)
```

**Problem:** Requires future knowledge - cannot be implemented in practice. Used as a **benchmark** to evaluate other algorithms.

### LRU (Least Recently Used)

Evict the page that has not been used for the **longest time in the past**.

```
Same reference string, 3 frames:

Step  Ref  Frames (most recent last)  Fault?
 1     7   [7]                         Yes
 2     0   [7, 0]                      Yes
 3     1   [7, 0, 1]                   Yes
 4     2   [0, 1, 2]                   Yes (evict 7)
 5     0   [1, 2, 0]                   No  (move 0 to most recent)
 6     3   [2, 0, 3]                   Yes (evict 1)
 7     0   [2, 3, 0]                   No
 8     4   [3, 0, 4]                   Yes (evict 2)
 9     2   [0, 4, 2]                   Yes (evict 3)
10     3   [4, 2, 3]                   Yes (evict 0)
11     0   [2, 3, 0]                   Yes (evict 4)
12     3   [2, 0, 3]                   No
13     2   [0, 3, 2]                   No

Total faults: 8 (close to optimal!)
```

**Implementation cost:** True LRU requires updating a timestamp or linked list on every memory access - expensive. Approximations are used instead.

### Clock Algorithm (Second Chance)

An approximation of LRU using the hardware **Accessed (reference) bit**:

```
Physical frames arranged in a circle with a "clock hand":

        clock hand
            |
            v
    +---+   +---+   +---+   +---+   +---+
    | A |-->| B |-->| C |-->| D |-->| E |--+
    |r=1|   |r=0|   |r=1|   |r=0|   |r=1|  |
    +---+   +---+   +---+   +---+   +---+  |
      ^                                      |
      +--------------------------------------+

Algorithm (on page fault):
  1. Check page at clock hand
  2. If reference bit = 0: EVICT this page (victim found)
  3. If reference bit = 1: Clear bit to 0, advance hand, goto 1

In the example:
  Hand at A: r=1 -> clear to 0, advance
  Hand at B: r=0 -> EVICT B (victim!)
```

**Linux uses a variant** called the **two-list** (active/inactive) approximation:
```bash
# See active vs inactive page counts
cat /proc/meminfo | grep -E "Active|Inactive"
# Active(anon):     memory recently used, harder to evict
# Inactive(anon):   not recently used, candidates for eviction
# Active(file):     file-backed pages recently used
# Inactive(file):   file-backed pages, easy to evict
```

---

## 13. Belady's Anomaly

**Belady's anomaly:** With FIFO replacement, increasing the number of frames can sometimes **increase** the number of page faults.

```
Reference string: 1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5

3 frames: 9 page faults
4 frames: 10 page faults  <-- MORE faults with MORE memory!

This is counterintuitive and is unique to FIFO.
LRU and Optimal do NOT exhibit Belady's anomaly (they are "stack algorithms").
```

**Stack algorithms** have the property that the set of pages in memory with N frames is always a subset of the pages in memory with N+1 frames. FIFO does not have this property.

---

## 14. Memory Allocation: Buddy Allocator

The **buddy allocator** manages physical page frames by splitting and merging power-of-2 sized blocks.

```
Initial state: one block of 256 pages

Request: 32 pages
  256 -> split into two 128s
  128 -> split into two 64s
  64  -> split into two 32s
  Allocate one 32-page block

Memory state:
[32 alloc][32 free][64 free][128 free]
              ^        ^        ^
           "buddy"  "buddy"  "buddy"

On free: if buddy is also free, MERGE upward
  [32 free][32 free] -> merge into [64 free]
  [64 free][64 free] -> merge into [128 free]
  And so on...
```

**Advantages:** Fast allocation/deallocation (O(log n)), eliminates external fragmentation within power-of-2 blocks, simple merging.

**Disadvantage:** Internal fragmentation (requesting 33 pages gets 64).

```bash
# Linux: see buddy allocator state
cat /proc/buddyinfo
# Node 0, zone DMA     0  0  1  0  2  1  1  0  1  1  3
# Node 0, zone Normal  1  2  3  5  3  2  4  3  2  1  180
# Numbers represent free blocks of size 2^n pages (4KB, 8KB, 16KB, ...)
```

---

## 15. Memory Allocation: Slab Allocator

The **slab allocator** sits on top of the buddy allocator and provides efficient allocation for **fixed-size kernel objects** (task_struct, inode, dentry, etc.).

```
Slab Allocator Architecture:
                         Cache (one per object type)
                        +-------------------------------+
                        | Cache: "task_struct"           |
                        | Object size: 6144 bytes        |
                        |                               |
                        | Slab 0 (full):                |
                        | [obj][obj][obj][obj]          |
                        |                               |
                        | Slab 1 (partial):             |
                        | [obj][obj][  ][  ]            |
                        |                               |
                        | Slab 2 (empty):               |
                        | [  ][  ][  ][  ]              |
                        +-------------------------------+

Allocation: take object from partial slab (O(1))
Free: return object to its slab (O(1))
If slab becomes empty AND memory is needed: return slab to buddy allocator
```

**Advantages:**
- No fragmentation for fixed-size objects
- O(1) allocation and deallocation
- Cache coloring: objects placed at different offsets within a cache line for better hardware cache utilization
- Constructor/destructor support: objects can be pre-initialized

```bash
# Linux: see slab cache statistics
cat /proc/slabinfo | head -20
# name            active_objs num_objs objsize
# task_struct         342      390     6144
# inode_cache        5432     5460      592
# dentry             8234     8316      192

# Or with more detail:
slabtop
```

**Linux SLUB allocator** (default since 2.6.23) is the modern replacement for the original SLAB allocator. It's simpler, has less metadata overhead, and better NUMA awareness.

---

## 16. Quizzes and Exercises

### Quiz 1: Multi-Level Page Table Allocation Savings (Clips 272-273)

> [!question]
> Consider a process on a system with 12-bit virtual addresses.
> The address space is configured such that only the first 2 KB and the last 1 KB are allocated and in active use.
> 
> 1. Address Format 1 (Single-Level): Virtual address has 6 bits for Virtual Page Number (VPN) and 6 bits for offset.
> How many total page table entries (PTEs) are required in a single-level page table?
> 
> 2. Address Format 2 (Two-Level): Virtual address has 2 bits for Outer Page Table (L1 directory index), 4 bits for Inner Page Table (L2 index), and 6 bits for offset.
> How many total entries are needed across all active inner page tables?

> [!success]- Answer
> 1. **Single-level page table:**
> With 6 bits for the VPN, the address space has $2^6 = 64$ virtual pages.
> A single-level page table must allocate an entry for every virtual page regardless of whether it is mapped.
> Therefore, it requires **64 entries**.
> 
> 2. **Two-level page table:**
> The offset is 6 bits, so each page is $2^6 = 64$ bytes.
> The outer page table index is 2 bits, so the outer directory has $2^2 = 4$ entries.
> Each outer entry points to an inner page table covering 4 bits of index plus 6 bits of offset ($2^{10} = 1024$ bytes = 1 KB of address space).
> The process allocates only the first 2 KB (outer entries 0 and 1) and the last 1 KB (outer entry 3).
> Outer entry 2 is unallocated, so its inner page table is never allocated.
> Each inner page table indexed by 4 bits contains $2^4 = 16$ entries.
> For the 3 active inner page tables, the total number of inner entries required is $3 \times 16 =$ **48 entries**.
> This yields a **25% reduction** in page table memory overhead compared to the flat 64-entry table.

---

### Quiz 2: Page Table Size vs Page Size (Clips 278-279)

> [!question]
> On a 12-bit architecture with a single-level page table:
> 
> 1. What is the number of page table entries required if the page size is 32 bytes?
> 2. What is the number of page table entries required if the page size is 512 bytes?

> [!success]- Answer
> 1. **Page size = 32 bytes:**
> An offset into a 32-byte page requires $\log_2(32) = 5$ bits.
> The virtual page number takes the remaining $12 - 5 = 7$ bits.
> The single-level page table must hold $2^7 =$ **128 entries**.
> 
> 2. **Page size = 512 bytes:**
> An offset into a 512-byte page requires $\log_2(512) = 9$ bits.
> The virtual page number takes the remaining $12 - 9 = 3$ bits.
> The single-level page table must hold $2^3 =$ **8 entries**.
> 
> Increasing page size directly shrinks the number of page table entries, reducing memory management overhead at the cost of potential internal fragmentation.

---

### Quiz 3: LRU Pathological Loop Thrashing (Clips 285-286)

> [!question]
> Suppose you have an array with 11 page-sized entries.
> All 11 entries are accessed continuously in a sequential loop (pages 1, 2, 3, ..., 11, 1, 2, ...).
> The physical system has exactly 10 frames of physical memory.
> What is the percentage of page accesses that will cause a page fault (demand paged) under the Least Recently Used (LRU) policy?
> Round your answer to the nearest integer percent.

> [!success]- Answer
> The correct answer is **100%**.
> 
> **Step-by-step trace:**
> 1. Pages 1 through 10 are loaded into the 10 physical frames on initial access.
> 2. Page 11 is accessed: physical memory is full.
> 3. The LRU policy evicts the page accessed least recently, which is Page 1.
> Frame slots now contain pages 2 through 11.
> 4. In the next iteration of the loop, the program accesses Page 1.
> Page 1 is missing, causing a page fault.
> 5. The LRU page in memory is now Page 2, so Page 2 is evicted to load Page 1.
> 6. Next, the program accesses Page 2, which was just evicted, causing another fault.
> Every single memory reference in the loop misses and triggers demand paging.
> 
> This pathological loop highlights that LRU can degrade to 0% hit rate when the working set exceeds physical capacity by even a single page.

---

### Quiz 4: Checkpointing Trade-offs (Clips 289-290)

> [!question]
> Which of the following statements correctly complete the sentence?
> "The more frequently you checkpoint..."
> 
> 1. The faster you will be able to recover from a fault
> 2. The higher the execution overhead of the checkpointing process
> 3. The more cumulative state you will end up transmitting/saving
> 4. All of the above

> [!success]- Answer
> The correct answer is **4. All of the above**.
> 
> **Rationale:**
> - **Faster recovery:** A more recent checkpoint means less computation must be replayed or re-executed from the last saved state when a failure occurs.
> - **Higher runtime overhead:** Checkpointing consumes CPU, memory bandwidth, and I/O.
> Frequent checkpoints interrupt execution more often.
> - **More total state saved:** If checkpoints are spaced out, multiple writes to the same page collapse into a single dirty page write (amortization).
> With frequent checkpoints, intermediate updates to dirty pages are captured and saved repeatedly, increasing total bytes written.

---

### Quiz 5: Address Translation Bit Arithmetic

> [!question]
> Given a 32-bit virtual address on a system with 4 KB pages and a 2-level page table:
> - Outer table (L1 directory): 10 bits
> - Inner table (L2): 10 bits
> - Offset: 12 bits
> 
> For virtual address `0x007A3C18`, calculate the L1 index, L2 index, and byte offset in decimal.

> [!success]- Answer
> - **L1 index:** bits 31-22 = `0x007A3C18 >> 22` = `0x1E` = **30**
> - **L2 index:** bits 21-12 = `(0x007A3C18 >> 12) & 0x3FF` = `0x3C` = **60**
> - **Offset:** bits 11-0 = `0x007A3C18 & 0xFFF` = `0xC18` = **3096**

---

### Exercise: Page Replacement Simulator

```python
#!/usr/bin/env python3
"""Page replacement algorithm simulator."""

def fifo(pages, num_frames):
    frames = []
    faults = 0
    for page in pages:
        if page not in frames:
            faults += 1
            if len(frames) >= num_frames:
                frames.pop(0)  # Remove oldest
            frames.append(page)
    return faults

def lru(pages, num_frames):
    frames = []
    faults = 0
    for page in pages:
        if page in frames:
            frames.remove(page)
            frames.append(page)  # Move to most recent
        else:
            faults += 1
            if len(frames) >= num_frames:
                frames.pop(0)  # Remove least recently used
            frames.append(page)
    return faults

def optimal(pages, num_frames):
    frames = []
    faults = 0
    for i, page in enumerate(pages):
        if page not in frames:
            faults += 1
            if len(frames) >= num_frames:
                # Find page used furthest in the future
                furthest = -1
                victim = -1
                for f in frames:
                    try:
                        next_use = pages[i+1:].index(f)
                    except ValueError:
                        next_use = float('inf')
                    if next_use > furthest:
                        furthest = next_use
                        victim = f
                frames.remove(victim)
            frames.append(page)
    return faults

# Test
ref_string = [7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2]
for n in [3, 4]:
    print(f"\nFrames = {n}:")
    print(f"  FIFO:    {fifo(ref_string, n)} faults")
    print(f"  LRU:     {lru(ref_string, n)} faults")
    print(f"  Optimal: {optimal(ref_string, n)} faults")
```

---

## 17. Key Takeaways

1. **Virtual memory** provides isolation, simplification, and the illusion of abundant memory via address translation.
2. **Multi-level page tables** (4-level on x86-64) avoid wasting space on unmapped regions.
3. The **TLB** caches recent translations; a 98% hit rate makes EMAT close to single memory access time.
4. **Page faults** are either minor (page in memory but not mapped) or major (must read from disk - ~10 ms).
5. **Optimal (MIN)** page replacement is the benchmark; **LRU** approximates it well; **Clock** is a practical hardware-assisted LRU approximation.
6. **Belady's anomaly** affects FIFO but not LRU or Optimal.
7. The **buddy allocator** handles physical frame allocation with O(log n) time; the **slab allocator** provides O(1) allocation for fixed-size kernel objects.

---

## 18. Huge Pages: Reducing TLB Pressure

Standard pages are 4KB. For large working sets, TLB misses dominate. Huge pages (2MB or 1GB) reduce TLB misses by 512x per 2MB page.

```bash
# Check huge page support
cat /proc/cpuinfo | grep -i pse    # PSE: 2MB pages
cat /proc/cpuinfo | grep -i pdpe   # PDPE: 1GB pages

# Static huge pages (reserved at boot)
cat /proc/meminfo | grep -i huge
# HugePages_Total: 128     (pre-allocated huge pages)
# HugePages_Free:  100
# Hugepagesize:    2048 kB

# Allocate 128 static 2MB huge pages
echo 128 > /proc/sys/vm/nr_hugepages

# Persistent via sysctl
echo "vm.nr_hugepages = 128" >> /etc/sysctl.conf
sysctl -p

# Use huge pages in application (explicit mmap)
#include <sys/mman.h>
void *mem = mmap(NULL, 2 * 1024 * 1024,
                 PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
                 -1, 0);

# Transparent Huge Pages (THP) - kernel auto-promotes 4KB→2MB
cat /sys/kernel/mm/transparent_hugepage/enabled
# [always] madvise never
echo "madvise" > /sys/kernel/mm/transparent_hugepage/enabled  # Opt-in only

# Opt specific memory region into THP
madvise(addr, size, MADV_HUGEPAGE);    /* Request THP */
madvise(addr, size, MADV_NOHUGEPAGE); /* Opt out (fragment-sensitive code) */

# Monitor THP usage
cat /proc/meminfo | grep -i anon_huge
cat /proc/<pid>/smaps | grep -A 5 "AnonHugePages"

# THP defrag: control when kernel defragments for THP
cat /sys/kernel/mm/transparent_hugepage/defrag
echo "defer+madvise" > /sys/kernel/mm/transparent_hugepage/defrag

# 1GB static huge pages (for databases like Oracle, PostgreSQL with huge_pages=on)
echo "vm.nr_hugepages = 4" >> /etc/sysctl.conf          # 4 × 2MB pages
echo "vm.nr_hugepages_1gb = 2" >> /etc/sysctl.conf      # 2 × 1GB pages
# 1GB pages MUST be allocated at boot via kernel parameters:
# GRUB: hugepagesz=1G hugepages=4

# PostgreSQL huge pages
# postgresql.conf: huge_pages = on
# Requires: SHMMAX >= shared_buffers, and huge pages allocated
```

---

## 19. Memory Compaction, Swapping, and ZRAM

```bash
# Memory zones and reclaim
cat /proc/zoneinfo                      # Physical memory zones
cat /proc/buddyinfo                     # Free pages per order (buddy allocator)
# Normal zone: 10 5 3 2 1 0 0 0 0 1 1  (free 4KB, 8KB, 16KB ... 4MB blocks)

# Memory reclaim tuning
cat /proc/sys/vm/swappiness             # 0-200 (default 60): prefer swapping
echo 10 > /proc/sys/vm/swappiness      # Prefer reclaiming file-backed pages

cat /proc/sys/vm/vfs_cache_pressure     # Pressure to reclaim dentries/inodes
echo 50 > /proc/sys/vm/vfs_cache_pressure  # Less pressure on dentry cache

# Manual compaction trigger (for huge page allocation)
echo 1 > /proc/sys/vm/compact_memory

# OOM Killer: when memory is exhausted
# oom_score: 0-1000 (higher = more likely to be killed)
cat /proc/<pid>/oom_score
cat /proc/<pid>/oom_score_adj           # -1000 to +1000 (override)

# Protect a process from OOM killer
echo -1000 > /proc/<pid>/oom_score_adj  # Never kill (root only)
echo  500  > /proc/<pid>/oom_score_adj  # Make it a preferred kill target

# cgroup memory limits
echo "512M" > /sys/fs/cgroup/myapp/memory.max   # Hard limit (OOM kill if exceeded)
echo "256M" > /sys/fs/cgroup/myapp/memory.high  # Soft limit (throttle/reclaim)
cat /sys/fs/cgroup/myapp/memory.stat            # Detailed memory stats

# PSI (Pressure Stall Information): memory pressure
cat /proc/pressure/memory
# some avg10=0.00 avg60=0.12 avg300=0.08 total=1234567
# full avg10=0.00 avg60=0.00 avg300=0.00 total=0
# "some": ≥1 task stalled on memory; "full": ALL tasks stalled

# ZRAM: compressed RAM swap (great for systems with little RAM)
modprobe zram
echo lz4 > /sys/block/zram0/comp_algorithm   # LZ4: fast compression
echo 4G   > /sys/block/zram0/disksize        # 4GB logical swap
mkswap /dev/zram0
swapon /dev/zram0 -p 100                     # Priority 100 (prefer over disk swap)

zramctl                                       # Show ZRAM stats
# NAME       ALGORITHM DISKSIZE DATA COMPR TOTAL STREAMS MOUNTPOINT
# /dev/zram0 lz4            4G  1.2G 312M  316M       4 [SWAP]
# Compression ratio: 1200M / 312M ≈ 3.8:1

# smem: per-process memory accounting (PSS = proportional share)
pip install smem
smem -t -p                     # All processes, as percentage
smem -s rss -r | head -20      # Sort by RSS descending
```

---

## 20. eBPF Memory Tracing

```bash
# Trace page faults in real time
sudo bpftrace -e '
    tracepoint:exceptions:page_fault_user {
        @faults[comm] = count();
    }
    interval:s:5 {
        print(@faults);
        clear(@faults);
    }'

# Major vs minor fault breakdown
sudo bpftrace -e '
    tracepoint:exceptions:page_fault_user {
        if (args->error_code & 0x4) @minor[comm]++;
        else @major[comm]++;
    }
    interval:s:10 { print(@major); print(@minor); exit(); }'

# Trace mmap calls
sudo bpftrace -e '
    tracepoint:syscalls:sys_enter_mmap {
        printf("%s mmap: len=%d prot=%d flags=%d\n",
               comm, args->len, args->prot, args->flags);
    }'

# Memory allocation profiling (malloc → sbrk → mmap chain)
sudo bpftrace -e '
    uprobe:/lib/x86_64-linux-gnu/libc.so.6:malloc {
        @alloc_sizes = hist(arg0);
    }
    interval:s:10 { print(@alloc_sizes); exit(); }'

# memleak: find memory leaks in running process
sudo memleak-bpfcc -p <pid> 10     # Sample 10 seconds
# Shows allocations not freed, with stack traces

# perf mem: record memory access patterns
perf mem record -a -- sleep 10     # System-wide
perf mem report                    # Show hot memory accesses
```

---

## 21. Windows Virtual Memory Internals

```c
/* Windows VirtualAlloc: primary memory allocation API */
#include <windows.h>

/* Reserve + Commit: allocate a 1MB read-write region */
LPVOID mem = VirtualAlloc(
    NULL,              /* Let OS choose address */
    1024 * 1024,       /* Size: 1 MB */
    MEM_RESERVE | MEM_COMMIT,  /* Reserve VA + commit physical pages */
    PAGE_READWRITE     /* Access rights */
);

/* Reserve only (no physical memory until accessed) */
LPVOID reserved = VirtualAlloc(NULL, 1024 * 1024 * 1024,
                                MEM_RESERVE, PAGE_NOACCESS);

/* Commit a subset of reserved region (demand paging) */
VirtualAlloc(reserved, 64 * 1024,  /* Commit first 64KB */
             MEM_COMMIT, PAGE_READWRITE);

/* Change page protection */
DWORD old_prot;
VirtualProtect(mem, 4096, PAGE_READONLY, &old_prot);

/* Query region information */
MEMORY_BASIC_INFORMATION mbi;
VirtualQuery(mem, &mbi, sizeof(mbi));
/* mbi.State: MEM_FREE / MEM_RESERVE / MEM_COMMIT */
/* mbi.Protect: PAGE_READWRITE / PAGE_READONLY / etc. */
/* mbi.Type: MEM_PRIVATE / MEM_MAPPED / MEM_IMAGE */

/* Free memory */
VirtualFree(mem, 0, MEM_RELEASE);  /* Release entire region */
VirtualFree(mem, 64*1024, MEM_DECOMMIT);  /* Decommit only */

/* Memory-mapped file */
HANDLE hFile = CreateFile(L"data.bin", GENERIC_READ, 0, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
HANDLE hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
LPVOID view = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);  /* Map entire file */
UnmapViewOfFile(view);
CloseHandle(hMap);
CloseHandle(hFile);
```

```powershell
# Windows: memory diagnostics
# Working set = pages currently in physical RAM for this process
Get-Process myapp | Select-Object WorkingSet64, PagedMemorySize64, VirtualMemorySize64

# Physical memory availability
Get-CimInstance Win32_OperatingSystem | Select-Object FreePhysicalMemory, TotalVisibleMemorySize

# Page file (swap) usage
Get-CimInstance Win32_PageFileUsage | Select-Object Name, CurrentUsage, PeakUsage

# RAMMap (SysInternals): physical memory breakdown by use
# Shows: ProcessPrivate, Mapped, SystemCache, Paged Pool, NonPaged Pool, etc.

# VMMap (SysInternals): per-process virtual memory map
# Shows each region: Image, Heap, Stack, Mapped, Private Data

# Perfmon: Memory counters
Get-Counter '\Memory\Available MBytes'
Get-Counter '\Memory\Page Faults/sec'
Get-Counter '\Memory\Pages/sec'          # Disk paging rate
Get-Counter '\Memory\Pool Nonpaged Bytes'  # Kernel non-paged pool

# WinDbg: kernel memory analysis
# !vm           - Virtual memory summary
# !memusage     - Physical memory usage by type
# !pool         - Kernel pool allocations
# !address      - VA space summary for process
```

---

## 22. macOS Memory Architecture and Darwin Internals

### 22.1 The Mach Virtual Memory Subsystem

The virtual memory architecture of macOS (Darwin / XNU) is rooted directly in the Mach 3.0 virtual memory design created at Carnegie Mellon University.
While user applications frequently invoke standard POSIX functions such as `mmap()`, `mprotect()`, and `brk()`, these routines act as BSD compatibility wrappers around low-level Mach VM primitives.

The XNU kernel structures virtual memory around three core abstractions:
1. **VM Map (`vm_map_t`):** Represents the complete address space of a Mach task. It contains an ordered doubly-linked list or red-black tree of `vm_map_entry` structures describing contiguous virtual address ranges.
2. **VM Object (`vm_object_t`):** Represents the memory backing for a region. It manages an array of resident physical pages (`vm_page_t`) and points to a pager (such as the default pager, vnode pager for memory-mapped files, or compressor pager).
3. **VM Map Entry (`vm_map_entry_t`):** Maps a virtual address sub-range in a `vm_map_t` to an offset inside a `vm_object_t`. This allows multiple tasks to map distinct virtual ranges to overlapping offsets within the same underlying object, enabling zero-copy shared memory and Copy-on-Write (COW) semantics.

Direct interaction with the Mach virtual memory subsystem is accomplished using Mach system traps:

```c
/* mach_vm_demo.c - Low-level Mach Virtual Memory allocation on Darwin */
#include <stdio.h>
#include <stdlib.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>

int main(void) {
    mach_port_t task = mach_task_self();
    mach_vm_address_t address = 0; /* Let kernel pick base virtual address */
    mach_vm_size_t size = 16 * 1024; /* 16 KB (one Apple Silicon page) */

    /* 1. Allocate virtual pages inside the task address space */
    kern_return_t kr = mach_vm_allocate(task, &address, size, VM_FLAGS_ANYWHERE);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "mach_vm_allocate failed: %d\n", kr);
        return 1;
    }
    printf("Allocated 16 KB virtual range at address: 0x%llx\n", (unsigned long long)address);

    /* 2. Write data to the allocated page (triggers demand fault) */
    int *data = (int *)address;
    data[0] = 0xDEADBEEF;
    printf("Wrote value: 0x%X\n", data[0]);

    /* 3. Protect memory: downgrade permissions to read-only */
    kr = mach_vm_protect(task, address, size, FALSE, VM_PROT_READ);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "mach_vm_protect failed: %d\n", kr);
    } else {
        printf("Permissions successfully set to VM_PROT_READ\n");
    }

    /* 4. Deallocate virtual address range */
    kr = mach_vm_deallocate(task, address, size);
    if (kr != KERN_SUCCESS) {
        fprintf(stderr, "mach_vm_deallocate failed: %d\n", kr);
        return 1;
    }
    printf("Successfully deallocated memory\n");

    return 0;
}
```

```bash
# Compile and run the Mach VM demo on macOS
clang -Wall -Wextra mach_vm_demo.c -o mach_vm_demo
./mach_vm_demo
```

### 22.2 The Mach Zone Allocator

Within the XNU kernel, dynamic memory allocation cannot rely on user-space `malloc()`.
Instead, Darwin employs the **Mach Zone Allocator** (`zalloc`), which serves an architectural role equivalent to the Linux SLAB/SLUB allocator.

Key properties of the zone allocator include:
- **Dedicated caches for fixed-size objects:** Zones exist for structures like `tasks`, `threads`, `vnodes`, `vm_maps`, and IPC ports.
- **Zero internal fragmentation:** Because each zone contains elements of identical size, allocations require no header tags per element.
- **Garbage collection and page stealing:** When physical RAM is low, zones can be scanned and empty pages reclaimed.

You can inspect the live status of all kernel memory zones using the macOS diagnostic utility `zprint`:

```bash
# Display the top 20 Mach zones sorted by size (requires root)
sudo zprint | head -25

# Output columns:
# ZONE NAME         ELEM SIZE  ALLOC  MAX ELEM  CUR ELEM  ALLOC SIZE  MAX SIZE
# ipc ports               168   2.1M     4.0M      2.2M      365.2M    672.0M
# threads                2048   4120     8192      4120        8.0M     16.0M
# vm.objects              240   340K     1.0M      350K       81.6M    240.0M
```

### 22.3 Compressed Memory Architecture (WKdm)

Traditional Unix systems swap unreferenced dirty pages directly to an external block device (disk partition or swap file).
Because disk operations incur millisecond-level latencies, macOS utilizes an in-memory **Compressed Memory Subsystem**.

Instead of writing cold pages to disk, the Darwin kernel runs an asynchronous compressor thread:
1. When memory pressure reaches a threshold, the kernel identifies inactive, dirty anonymous pages.
2. The page is compressed using the **WKdm algorithm** (Wilson-Kaplan-Direct-Match).
WKdm is a specialized integer-dictionary compression algorithm tuned specifically for 4 KB and 16 KB memory pages, running in tens of nanoseconds.
3. The compressed data is packed into a compact chunk in RAM, achieving typical compression ratios between 2.5:1 and 4:1.
4. The original physical frame is immediately returned to the free list.
5. If the application subsequently reads the page, a soft page fault occurs; the compressor uncompresses the chunk back into a physical frame in microseconds.
6. Only when physical RAM is critically exhausted and the in-memory compressor pool itself cannot shrink further does the system page compressed segments out to `/private/var/vm/swapfile*`.

```bash
# Inspect XNU memory compressor mode
sysctl vm.compressor_mode
# vm.compressor_mode: 4
# Mode 1: No compression (direct disk paging)
# Mode 2: Compression without disk paging
# Mode 4: Compression with disk paging fallback (default macOS production setting)

# Inspect total compressed bytes and compressor page metrics
vm_stat | grep -i "compress"
```

### 22.4 Purgeable and Volatile Memory

Darwin provides a dedicated memory optimization called **Purgeable Memory** (`VM_FLAGS_PURGEABLE`).
Purgeable memory allows an application to allocate buffers (such as rendered image caches, audio decodes, or parsed ASTs) that the kernel is permitted to reclaim silently under severe memory pressure without terminating the application.

- **Non-volatile state:** The buffer is pinned and protected against eviction.
- **Volatile state:** The application informs the OS that the buffer can be discarded if memory is needed elsewhere.
- **Empty state:** The kernel has reclaimed the physical pages backing the buffer.
Before reading the cache, the application checks whether the region remains valid using `vm_purgable_control()`.
If the kernel reclaimed the buffer, the application recalculates or re-reads the underlying asset.

```c
/* Using purgeable memory on Darwin */
#include <mach/mach.h>
#include <mach/mach_vm.h>

void purgeable_example(void) {
    mach_vm_address_t addr = 0;
    mach_vm_size_t size = 64 * 1024 * 1024; /* 64 MB cache */

    /* Allocate as purgeable */
    mach_vm_allocate(mach_task_self(), &addr, size, VM_FLAGS_ANYWHERE | VM_FLAGS_PURGEABLE);

    /* Mark as volatile (kernel may discard under memory pressure) */
    int state = VM_PURGABLE_VOLATILE;
    vm_purgable_control(mach_task_self(), (vm_address_t)addr, VM_PURGABLE_SET_STATE, &state);

    /* Later: re-access cache and verify whether data was preserved */
    state = VM_PURGABLE_NONVOLATILE;
    vm_purgable_control(mach_task_self(), (vm_address_t)addr, VM_PURGABLE_SET_STATE, &state);

    if (state == VM_PURGABLE_EMPTY) {
        /* Pages were purged by kernel; regenerate cached data */
    } else {
        /* Data is intact; read directly */
    }
}
```

### 22.5 macOS Memory Diagnostics and Tooling

Darwin includes deep command-line tools to monitor physical frame states, virtual mappings, and process memory footprints:

```bash
# 1. vm_stat: Display system-wide physical memory page counts
vm_stat

# Note: On Apple Silicon, standard hardware page size is 16384 bytes (16 KB)
# Calculate actual RAM usage by multiplying page counts by page size (vm_stat 1 prints per second)
pagesize
# 16384

# 2. footprint: The gold-standard Darwin memory accounting tool
# Measures real physical footprint (dirty anonymous pages + compressed pages + wired memory)
# Excludes clean mapped file pages and shared read-only system dylibs
footprint <pid>

# 3. vmmap: Output complete virtual memory map of a process
# Displays Mach zones, mapped frameworks, stack frames, and protection bits
vmmap -summary <pid>
vmmap -wide <pid>

# 4. heap: Inspect all live objects allocated in libmalloc zones
heap <pid>
heap <pid> -addresses all

# 5. memory_pressure: Monitor or simulate system-wide memory pressure levels
# Normal, Warn, or Critical pressure events
memory_pressure
```

---

**Previous:** [P3L1: Scheduling](P3L1-Scheduling.md)
**Next:** [P3L3: Inter-Process Communication](P3L3-Inter-Process-Communication.md)


