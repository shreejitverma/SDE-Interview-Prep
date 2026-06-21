# Chapter 88: Virtual Memory, TLB, Huge Pages, and NUMA

Every pointer your program dereferences is a *virtual* address that hardware must translate to a physical one before any byte is read — and that translation, the page faults it can trigger, and the non-uniform cost of reaching physical memory on a multi-socket machine are invisible in your source yet decisive for tail latency. This chapter exposes the machinery beneath the pointer: page tables and the TLB, demand paging and page faults, huge pages, and NUMA topology — and the disciplines (pre-faulting, page locking, huge pages, first-touch placement) that keep this machinery from injecting jitter into a latency-critical path.

## Chapter Roadmap

- 88.1 Why Virtual Memory Exists
- 88.2 Address Translation, Page Tables, and the TLB
- 88.3 Page Faults: Minor, Major, and Why They Spike Latency
- 88.4 Demand Paging and Pre-Faulting
- 88.5 Huge Pages and TLB Reach
- 88.6 NUMA: Non-Uniform Memory Access
- 88.7 The Disciplines for Deterministic Memory

---

## 88.1 Why Virtual Memory Exists

**Virtual memory** gives each process its own private, contiguous address space, decoupled from physical RAM. The OS and the CPU's memory-management unit (MMU) translate virtual addresses to physical ones on every access. This buys isolation (one process cannot see another's memory), the illusion of more memory than exists (paging to disk), and the freedom to lay out a process's address space simply while physical memory is fragmented.

> **Why this matters.** Virtual memory is a convenience that imposes a tax: every memory access requires a translation, and the mechanisms that make the illusion work (demand paging, swapping) can stall your program for *milliseconds* at unpredictable times. For ordinary code this is a fine trade. For latency-critical code it is a source of jitter that must be actively managed — you cannot opt out of virtual memory, but you can pin, pre-fault, and use huge pages to make its cost constant and small.

---

## 88.2 Address Translation, Page Tables, and the TLB

Memory is divided into fixed-size **pages** (default 4 KB on x86-64/AArch64). The mapping from virtual page to physical **frame** lives in the **page table**, a multi-level radix tree (4 levels on x86-64, soon 5). Translating one address by walking the page table from memory requires up to 4 dependent memory accesses — far too slow to do per access.

The hardware fix is the **Translation Lookaside Buffer (TLB)**: a small, fast cache of recent virtual→physical translations. A **TLB hit** translates in ~1 cycle; a **TLB miss** triggers a **page walk** (the MMU walks the page table, ~10s–100s of cycles, itself hitting caches or DRAM).

> **Why this matters / cost model.** The TLB is a cache with the same hit/miss economics as the data cache, but for *translations* rather than data — and it is small (often a few hundred to ~1–2 K entries across levels). With 4 KB pages, a few hundred entries cover only a few megabytes of memory: the TLB's **reach** (entries × page size) is tiny relative to modern working sets. A program touching many pages randomly thrashes the TLB, paying a page walk per access *on top of* the data cache miss. This "TLB miss" cost is invisible in source and a common hidden tax on large-working-set, random-access workloads (hash tables, large graphs) — and it is exactly what huge pages (§88.5) address.

---

## 88.3 Page Faults: Minor, Major, and Why They Spike Latency

When a virtual page has no valid mapping, the access triggers a **page fault** — a trap into the kernel:

- **Minor fault:** the page is in physical memory but not mapped into this process's page table yet (e.g. first touch of freshly-allocated memory, or a shared page already resident). The kernel fixes the mapping — microseconds.
- **Major fault:** the page's contents must be fetched from disk/swap or a memory-mapped file. The kernel performs I/O — **milliseconds**, and the thread is blocked the whole time.

> **Why this matters / cost model.** A page fault is a *synchronous, involuntary syscall* you did not write — the CPU traps to the kernel mid-instruction. A minor fault (microseconds) is the reason the *first* write to freshly `malloc`'d memory is far slower than subsequent ones: `malloc` reserved virtual address space, but physical frames are allocated lazily on first touch. A major fault (milliseconds) is catastrophic for latency-critical code — a single swap-in can blow a microsecond-scale latency budget by a thousandfold. This is why the hot-path discipline forbids both: pre-fault all memory before the critical phase, and lock it resident so it can never be swapped (§88.7).

---

## 88.4 Demand Paging and Pre-Faulting

By default the OS uses **demand paging**: `malloc`/`mmap` reserve virtual address space, but physical frames are allocated only on first access (the minor fault of §88.3). This is efficient — unused memory costs nothing — but it means the *cost of allocation is deferred to first use*, scattered unpredictably through your hot path.

```cpp
// Min standard: C++11. Linux. Pre-faulting a buffer so the hot path never faults.
#include <cstring>
#include <vector>

void prefault(std::vector<char>& buf) {
    // Touch one byte per page to force the minor faults NOW, not during the hot path.
    const size_t page = 4096;
    for (size_t i = 0; i < buf.size(); i += page) buf[i] = 0;   // first-touch each page
}
// Alternatively: mmap(..., MAP_POPULATE) pre-faults at mapping time (Linux-specific).
```
*Listing 88.1 — Pre-faulting moves the minor-fault cost out of the critical path. Linux-specific behaviour.*

> **Why this matters.** Pre-faulting converts unpredictable, distributed fault latency into a single up-front cost during initialisation/warmup, where it is harmless. Combined with **warming** (running the hot path on dummy data at startup so caches, TLB, and branch predictors are primed — Chapter 106), this is how trading systems ensure the *first real* message is as fast as the millionth. The trade-off is startup time and committed physical memory; for a latency-critical service that is exactly the right trade.

---

## 88.5 Huge Pages and TLB Reach

A **huge page** maps a much larger region with a single page-table entry and TLB entry: 2 MB or 1 GB on x86-64 instead of 4 KB. One 2 MB huge page covers 512× the memory of a 4 KB page with one TLB entry, dramatically extending TLB reach.

```bash
# Linux: reserve explicit huge pages (admin), then map them.
# echo 512 > /proc/sys/vm/nr_hugepages          # reserve 512 * 2MB = 1GB
# mmap(..., MAP_HUGETLB, ...) or use Transparent Huge Pages (THP).
```
*Listing 88.2 — Reserving huge pages on Linux (non-portable, requires privilege).*

> **Why this matters / cost model.** For a large working set accessed with poor locality, TLB misses (§88.2) can rival data cache misses as a bottleneck. Huge pages cut the number of TLB entries needed for a given footprint by 512× (2 MB) or 262144× (1 GB), so a multi-gigabyte working set can fit in the TLB's reach — eliminating page walks. The costs and caveats: huge pages reduce allocation granularity (internal fragmentation), can increase minor-fault latency (zeroing 2 MB at once), and **Transparent Huge Pages (THP)**, which promote pages automatically, can *introduce* jitter via background defragmentation/`khugepaged` — many latency-sensitive shops disable THP and use *explicit* huge pages instead. Huge pages are a measured optimization for large-footprint, TLB-bound workloads, not a default.

---

## 88.6 NUMA: Non-Uniform Memory Access

On a multi-socket server, each CPU socket has its own local memory controller and DRAM. A core accessing memory attached to *its* socket (local) is fast; accessing memory on *another* socket (remote) crosses an inter-socket link (e.g. UPI/Infinity Fabric) at higher latency and lower bandwidth. This is **NUMA** — Non-Uniform Memory Access.

```cpp
// Min standard: C++11. Linux/libnuma concepts (non-portable).
// numactl --cpunodebind=0 --membind=0 ./app   # pin process to node 0's CPUs and memory
// First-touch policy: a page is placed on the NUMA node of the thread that FIRST writes it.
```
*Listing 88.3 — Binding a process to a NUMA node and the first-touch placement rule.*

The default Linux policy is **first-touch**: a physical page is allocated on the NUMA node of the thread that first *writes* it — not the thread that allocated it.

> **Why this matters / cost model.** Remote memory access can be ~1.5–2× the latency and a fraction of the bandwidth of local access. On a NUMA machine, a thread repeatedly touching remote memory is silently paying that penalty on every miss. The classic bug: a single initialiser thread first-touches a giant array (placing it all on *one* node), then worker threads on other nodes hammer it remotely. The fix follows first-touch: have *each worker* initialise the memory it will use, so pages land on its local node; and **pin threads** to cores (Chapter 96) so they stay near their memory. `numactl`/libnuma give explicit control. NUMA-obliviousness is one of the most common scalability failures on big servers — adding cores stops helping because they all contend one socket's memory controller.

---

## 88.7 The Disciplines for Deterministic Memory

| Hazard | Cost | Discipline |
|---|---|---|
| TLB miss | Page walk (10s–100s cycles) | Huge pages; locality; smaller footprint |
| Minor page fault | Microseconds, per first-touch | Pre-fault during warmup (`MAP_POPULATE`, touch each page) |
| Major page fault (swap) | **Milliseconds** | `mlock`/`mlockall` to lock pages resident; disable swap |
| Remote NUMA access | ~1.5–2× latency, less bandwidth | First-touch local; pin threads; `numactl` |
| THP background defrag | Unpredictable jitter | Disable THP; use explicit huge pages |

```cpp
// Min standard: C++11. Linux. Lock all current and future pages resident.
#include <sys/mman.h>
// mlockall(MCL_CURRENT | MCL_FUTURE);   // never swap this process's memory (needs privilege)
```
*Listing 88.4 — `mlockall` prevents major faults by pinning memory resident. Linux, privileged.*

> **The discipline.** Virtual memory's mechanisms — lazy allocation, swapping, multi-level translation, NUMA placement — are tuned for *average* throughput across many processes, which is exactly wrong for a single latency-critical process that cares about the *tail*. The remedy is to take manual control: allocate and **pre-fault** all memory up front, **`mlock`** it so it can never swap, use **huge pages** to keep translations in the TLB, and place memory **local** to the threads that use it. These four moves turn virtual memory from a source of millisecond jitter into a fixed, paid-once cost — the same determinism imperative that runs through the allocator (Chapter 79), threading (Chapter 96), and hot-path (Chapter 106) chapters. Next, we go up the stack to read exactly what the compiler emitted.
