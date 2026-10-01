---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: R
tags: [cs6210, cs6210/practice]
---

# Practice R

Original exam-style questions for [R01](../Part-0-Refresher/R01-Virtual-Memory-and-Caches.md), [R02](../Part-0-Refresher/R02-Architecture-Storage-and-Networking.md), [R03](../Part-0-Refresher/R03-Concurrency-and-the-Kernel.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

> [!question]- Q1. How does a virtual address get translated to a physical address in a system with multi-level page tables and a virtually indexed physically tagged (VIPT) cache, and what role does the TLB play? (concepts: R01-01, R01-02, R01-03, R01-04)
> The CPU divides the address space into virtual pages which map to physical frames.
> Multi-level page tables hold these mappings efficiently by using a hierarchical tree.
> During translation, the hardware simultaneously uses the virtual address to index the VIPT cache and queries the Translation Lookaside Buffer (TLB).
> If the TLB misses, a hardware page table walker traverses the multi-level page table to find the physical frame.
> The physical address tag is then compared with the tag from the cache to determine a hit.
> When a page mapping is modified or invalidated, the OS must issue a TLB shootdown to ensure all other cores clear their cached, stale translations.

> [!question]- Q2. Trace the end-to-end handling of a page fault for a memory page that has been swapped to disk, and explain how thrashing relates to the working set. (concepts: R01-05, R01-07, R01-08)
> When a process accesses a page not resident in physical memory, the hardware traps into the kernel, initiating a page fault.
> The kernel identifies the faulting address and consults its internal structures to find the corresponding block on disk.
> The loader and linker originally placed the program executable on disk, but dynamically allocated pages are managed by the paging daemon and swapper.
> The swapper reads the required page from the swap device into a free physical frame, updating the page table and returning control to the process.
> A process's working set is the active collection of pages it needs to run efficiently.
> Thrashing occurs when the total working sets of active processes exceed the available physical memory.
> Under thrashing, the system spends more time swapping pages in and out than executing useful instructions.

> [!question]- Q3. Compare and contrast page replacement policies with cache replacement policies, and explain how page coloring can optimize performance. (concepts: R01-06, R01-09, R01-10, R01-12)
> Page replacement policies determine which memory page to evict to disk when physical memory is full.
> These algorithms can afford to be relatively complex, often approximating Least Recently Used (LRU) using clock bits, because page faults are expensive.
> Cache replacement policies operate in hardware and must be extremely fast.
> They use simple approximations to evict cache lines when a cache set is full.
> Paging provides fixed-size memory blocks and completely isolates virtual address spaces, whereas early systems used segmentation to manage variable-sized logical boundaries.
> Page coloring is a software technique where the OS assigns physical frames to virtual pages such that consecutive pages map to different cache sets.
> This optimization minimizes cache conflicts by ensuring pages that are contiguous in virtual memory do not heavily overlap in a physically indexed cache.

> [!question]- Q4. How does a multicore chip's cache hierarchy maintain cache coherence, and what is the impact of cache associativity? (concepts: R01-11, R01-13, R01-14)
> Multicore chips typically have private L1 caches for each core and a shared L2 or L3 cache.
> Cache organization relies on associativity to determine how many distinct cache lines can reside in the same index set.
> Higher associativity reduces conflict misses but increases hardware complexity and access latency.
> Cache coherence protocols ensure that all cores see a consistent view of shared memory.
> An update protocol broadcasts written values to all other caches holding that line.
> An invalidate protocol instead sends a message to mark copies in other caches as invalid when one core writes to the data.
> Most modern architectures use invalidate protocols to conserve bus bandwidth.

> [!question]- Q5. How does the memory access latency difference between SMP and NUMA architectures impact operating system design? (concepts: R02-01)
> Symmetric Multiprocessing (SMP) architectures provide uniform memory access times to all processors.
> Non-Uniform Memory Access (NUMA) architectures divide memory into nodes local to specific processors.
> In a NUMA system, a processor can access its local memory much faster than remote memory attached to another processor.
> The OS scheduler must be NUMA-aware to keep threads on the same node as their allocated memory.
> The memory allocator must also attempt to place new pages in the memory bank local to the requesting thread.

> [!question]- Q6. Describe the relationship between block devices, the file system, and data structures like inodes. (concepts: R02-02, R02-03)
> A block device provides a storage interface that reads and writes data in fixed-size blocks.
> The file system builds a logical hierarchy of files and directories on top of this block device.
> An inode is a core file system data structure that represents a single file or directory.
> It contains metadata such as permissions, ownership, file size, and timestamps.
> Most importantly, the inode stores pointers that map logical file offsets to the physical blocks on the underlying device.

> [!question]- Q7. Trace the data path of a network packet from a user-space application to the Network Interface Card (NIC), identifying the major stack layers. (concepts: R02-04, R02-05)
> The user-space application issues a system call, such as send, writing data into a kernel socket buffer.
> The Linux network protocol stack processes the data by passing it through the transport layer, which adds a TCP or UDP header.
> The packet then descends to the network layer where an IP header is attached to enable routing.
> Next, the data link layer constructs a frame by adding MAC addresses and other physical network details.
> Finally, the kernel queues the frame in a ring buffer and notifies the NIC device driver.
> The NIC reads the frame from host memory using Direct Memory Access (DMA) and transmits it over the physical medium.

> [!question]- Q8. What is the fundamental difference between concurrency and parallelism, and how do thread context switches facilitate these concepts? (concepts: R03-01, R03-02, R03-05)
> Concurrency is the execution of multiple tasks making progress over overlapping time periods, often by interleaving them on a single core.
> Parallelism requires true simultaneous execution of tasks on multiple physical cores.
> An operating system implements scheduling basics to assign CPU time slices to runnable threads.
> To swap between threads, the OS performs a context switch which involves saving the current thread's registers and loading the next thread's state.
> A thread context switch is generally cheaper than a process context switch because threads within the same process share the same virtual address space.
> A process context switch incurs additional costs such as flushing the TLB and loading a new page table base pointer.

> [!question]- Q9. How does the operating system kernel gain control of the processor during execution, and why does each thread require a kernel stack? (concepts: R03-03, R03-04)
> The kernel gains control through three primary hardware mechanisms: traps, interrupts, and system calls.
> Traps occur synchronously due to exceptional conditions like divide-by-zero or page faults.
> Interrupts occur asynchronously, driven by external hardware devices signaling for attention.
> System calls are intentional requests made by user applications to invoke privileged OS services.
> When crossing from user space to kernel space, the processor switches to a privileged execution mode.
> Each thread maintains its own kernel stack to safely store local variables, return addresses, and register state during these kernel executions.

> [!question]- Q10. Describe the memory layout of a process and explain how compiling, linking, and pointer errors can lead to a segfault. (concepts: R03-08, R03-10, R03-11)
> A standard process memory layout consists of text, data, bss, heap, and stack segments.
> Compiling translates high-level code into object files, and linking resolves external symbols to build a final executable structured around this memory layout.
> The text segment holds read-only executable instructions, while the stack grows downward to manage function calls.
> A segmentation fault, or segfault, occurs when a process attempts to access a memory location it does not have permission to read or write.
> This often happens when a program dereferences a null or invalid pointer, or tries to write to the read-only text segment.
> Developers debug segfaults by analyzing core dumps in tools like GDB to inspect the crashing instruction and call stack.

> [!question]- Q11. In a Pthreads producer-consumer program, how are function calls managed at the machine level, and why is mutual exclusion critical? (concepts: R03-06, R03-07, R03-09, R03-12)
> A Pthreads producer-consumer architecture uses multiple concurrent threads that communicate through a shared buffer.
> To pass complex data between threads, C programs rely on pointers, and often use function pointers and casts for generic thread execution routines.
> At the machine level, function calls push arguments and a return address onto the stack before jumping to the new routine.
> A return instruction pops the address off the stack to resume execution at the caller.
> Because both producer and consumer threads access the same shared buffer, basic mutual exclusion using locks is required.
> Without mutual exclusion, overlapping read and write operations would corrupt the shared data structure.
