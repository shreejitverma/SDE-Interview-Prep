---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources: []
course: cs6210
lesson: L01
tags: [cs6210, cs6210/practice]
---

# Practice L01

Original exam-style questions for [L01](../Part-1-OS-Structure-and-Virtualization/L01-Introduction-to-AOS.md).
Each question names the coverage ids it exercises; answers are folded so this page works as a self-test.

> [!question]- Q1. True or false: the OS hides the complexity of underlying hardware from applications through abstractions, which means the OS itself no longer needs to be aware of the specific hardware organization. (concepts: L01-01, L01-02)
> False.
> While the OS provides simplified abstractions like files, sockets, and processes to user-level applications, the OS kernel itself must intimately understand the hardware organization to function effectively.
> It relies on specific hardware hooks, such as interrupts, memory management units, page table structures, memory controllers, cache line sizes, and bus interconnect architectures, to physically manage data movement and enforce security boundaries.
> Furthermore, OS performance optimizations (e.g., NUMA-aware page allocation, hugepage promotion, cache line bouncing prevention) require deep, continuous awareness of physical processor topology.

> [!question]- Q2. A system performs a context switch that flushes the Translation Lookaside Buffer (TLB). If the system immediately executes 5000 memory accesses with a cold TLB hit rate of 40%, and a TLB miss costs 120 cycles, what is the total penalty in cycles incurred by the context switch due to TLB misses? (concepts: L01-03, L01-05)
> The context switch causes 5000 memory accesses to experience a 60% miss rate due to the cold TLB.
> The number of missed accesses is 5000 multiplied by 0.60, which equals 3000 misses.
> Multiplying 3000 misses by the 120-cycle penalty per miss results in a total penalty of 360,000 cycles.
> This demonstrates the indirect cache and memory hierarchy costs of CPU management through context switching, which often dwarf direct register save/restore time.

> [!question]- Q3. Consider a scenario where a monolithic kernel is being redesigned to improve fault isolation. How would moving to a microkernel architecture change the boundary between policy and mechanism, and what is the primary performance tradeoff? (concepts: L01-04, L01-06)
> In a microkernel, only minimal mechanisms like basic inter-process communication (IPC), address space manipulation, and low-level thread scheduling remain in the privileged protection domain (kernel mode).
> High-level OS functionalities and policies, such as file system hierarchies, device drivers, network protocols, and complex access control policies, are moved into unprivileged user-space servers.
> The primary tradeoff is reduced performance due to the overhead of frequent context switches, address space switches, and IPC message-passing operations required to coordinate between these isolated user-space servers for what was previously a single in-kernel function call.
> However, this provides much stronger fault isolation, as a crashed user-space driver or service can be restarted independently without corrupting kernel state or crashing the machine.

> [!question]- Q4. The operating system multiplexes execution units to manage the CPU across multiple ready threads. Why is a timer interrupt an essential hardware mechanism for this CPU management? (concepts: L01-05, L01-06)
> A timer interrupt guarantees that CPU control is periodically returned from a user-level application back to the operating system kernel.
> Without this mechanism, a long-running, CPU-bound, or malicious process operating in its unprivileged protection domain could enter an infinite loop and permanently monopolize the processor in a non-preemptive environment.
> The periodic timer interrupt enforces preemptive scheduling policies, allowing the OS scheduler to update thread accounting metrics, evaluate runqueue priorities, pause the running process, save its architectural state, and switch execution to another thread to ensure fairness and bounded latency.

> [!question]- Q5. Outline the sequence of hardware and OS events that occur when a user application needs to read a file, transitioning across protection domains. (concepts: L01-01, L01-04, L01-06)
> 1. The user application invokes the file `read()` API, passing file descriptors, buffer addresses, and byte counts.
> 2. The standard library places syscall arguments into designated architecture registers (e.g., `%rax`, `%rdi`, `%rsi`, `%rdx` on x86-64) and executes a hardware trap or dedicated instruction (`syscall`/`sysenter`).
> 3. The CPU hardware switches execution privilege from Ring 3 (user mode) to Ring 0 (kernel mode), swaps the stack pointer `%rsp` from the user stack to the process's kernel stack (via the Task State Segment / MSR), and vectors execution to the kernel's system call entry handler.
> 4. The kernel saves non-volatile user registers onto the kernel stack, validates pointer arguments against user-space memory limits, and checks access permissions for the target file descriptor.
> 5. The VFS layer routes the request to the specific file system implementation (e.g., ext4), which inspects the in-memory page cache. On a miss, it formats block I/O requests (`struct bio`), issues DMA commands to the storage device controller, and puts the thread into a sleep state (`TASK_UNINTERRUPTIBLE`).
> 6. When DMA transfer completes, the disk controller fires a hardware interrupt, waking the thread and placing it back on the scheduler runqueue.
> 7. Once rescheduled, the kernel copies retrieved data to the user buffer (or maps the page directly), loads the return value into `%rax`, restores saved user registers, and executes `sysretq`/`sysexit`.
> 8. The CPU switches back to Ring 3 unprivileged mode, restores the user `%rsp`, and execution resumes at the instruction immediately following the system call.

> [!question]- Q6. A multicore CPU runs at 3.0 GHz. A thread switch incurs two distinct categories of overhead: direct overhead and indirect overhead. Direct overhead requires saving and restoring integer registers, control registers, and switching kernel stacks, taking 450 clock cycles. Indirect overhead arises because the new thread pollutes the 32 KB L1 data cache: during the first 10 microseconds after the switch, the thread experiences 200 additional L1 cache misses that must be served by L3 cache (latency 40 cycles each) and 50 misses that miss L3 entirely and must be fetched from DRAM (latency 180 cycles each). Calculate the total context switch cost in cycles and the percentage of that cost attributable to indirect memory hierarchy effects. (concepts: L01-02, L01-03, L01-05)
> Direct context switch cost:
> $$C_{\text{direct}} = 450\text{ cycles}$$
> Indirect memory hierarchy cost:
> $$C_{\text{indirect}} = (200 \times 40\text{ cycles}) + (50 \times 180\text{ cycles}) = 8000 + 9000 = 17,000\text{ cycles}$$
> Total context switch cost:
> $$C_{\text{total}} = C_{\text{direct}} + C_{\text{indirect}} = 450 + 17,000 = 17,450\text{ cycles}$$
> Percentage due to indirect memory hierarchy effects:
> $$\frac{17,000}{17,450} \times 100\% \approx 97.42\%$$
> This calculation highlights why modern OS schedulers prioritize CPU and cache affinity: the pure architectural cost of saving registers is negligible compared to the indirect cost of cache and TLB perturbation.

> [!question]- Q7. Contrast hardware Address Space Identifiers (ASID / PCID on modern x86/ARM) with older TLB architectures during a process context switch. How does hardware support for tagged TLBs alter the cost model of address space switching? (concepts: L01-02, L01-03, L01-05)
> On older CPU architectures without TLB tagging, every process switch required the operating system to reload the base page table register (e.g., `CR3` on x86), which automatically invalidated (flushed) all non-global TLB entries to prevent the incoming process from accessing the previous process's address translations.
> Consequently, the incoming process began execution with an entirely cold TLB, incurring hundreds of high-latency multi-level page table walks during its initial memory accesses.
> With hardware support for Address Space Identifiers (ASID on ARM, Process-Context Identifiers / PCID on x86-64), the hardware tags each TLB entry with the owning process's ID.
> When switching processes, the OS loads `CR3` with a bitmask indicating the target PCID without setting the flush bit.
> Translations for both the old and new processes remain co-resident in the TLB.
> If the CPU returns to a recently executed process whose translations have not been evicted by capacity conflicts, memory operations achieve immediate TLB hits, dramatically reducing the indirect cost of address space border crossings.

> [!question]- Q8. Explain why OS abstractions are described as "leaky" when applied to high-performance and hard real-time systems. Provide two concrete examples where hiding hardware realities causes pathological system behavior. (concepts: L01-01, L01-02, L01-04)
> An abstraction is "leaky" when the implementation details of the underlying hardware cannot be completely concealed without imposing severe performance, predictability, or correctness penalties.
> While general-purpose applications benefit from abstractions like flat uniform memory and file streams, specialized high-performance systems frequently suffer:
> 1. Flat Virtual Address Space vs. NUMA / Paging: The virtual memory abstraction provides the illusion of uniform, boundless memory access. However, on Non-Uniform Memory Access (NUMA) architectures, accessing memory attached to a remote CPU socket incurs substantially higher latency and lower bandwidth than local memory. Furthermore, an unexpected demand-paging fault or page reclamation trigger (e.g., Linux kswapd) will block a thread for milliseconds to perform disk I/O, completely violating hard real-time deadlines.
> 2. Synchronous File Append vs. Storage Erase Blocks: The POSIX file abstraction presents files as linear streams of bytes where `write()` appends data sequentially. In solid-state drives (SSDs), physical flash memory can only be overwritten after erasing an entire block (typically several megabytes). Uncoordinated small random file writes trigger severe write amplification, background garbage collection, and device-level latency spikes that break application throughput assumptions.

> [!question]- Q9. In the context of hardware resources and OS protection domains, explain the distinction between synchronous exceptions, asynchronous interrupts, and software-initiated traps. How does the CPU hardware handle interrupt masking to prevent race conditions during OS state transitions? (concepts: L01-02, L01-05, L01-06)
> - Synchronous Exceptions: Conditions generated directly by the execution of an instruction within the current CPU thread (e.g., divide-by-zero, page fault, alignment check, invalid opcode). They occur at reproducible instruction boundaries and must be resolved before the faulting instruction can complete or abort.
> - Asynchronous Interrupts: External electrical signals delivered to the CPU by hardware devices (e.g., timer chip, NIC, disk controller) via an interrupt controller (e.g., APIC). They arrive independently of the instruction stream currently executing on the processor.
> - Software Traps: Instructions executed deliberately by software to invoke OS services (e.g., `INT 0x80`, `syscall`, `trap`), transitioning execution synchronously from unprivileged to privileged mode.
>
> To prevent race conditions during critical OS transitions (such as updating process control blocks or switching kernel stacks), the CPU provides hardware interrupt masking (e.g., the Interrupt Flag `IF` in x86 `EFLAGS`, cleared via `cli` and set via `sti`).
> When the CPU traps into kernel mode through an interrupt gate, the hardware automatically clears `IF`, preventing nested external interrupts until the kernel establishes a valid stack frame and explicitly re-enables interrupts.

> [!question]- Q10. What is the fundamental difference between "policy" and "mechanism" in operating system design? Illustrate this difference using the Unix file system permission model and CPU priority scheduling. (concepts: L01-04, L01-06)
> - Mechanism: The fundamental hardware or low-level software capability that defines *how* a capability is implemented without dictating *what* decisions should be made or *who* receives access.
> - Policy: The high-level rules, algorithms, or administrative parameters that determine *what* choices are made, when operations are permitted, and how resources are allocated.
>
> Illustrations:
> 1. Unix File Permissions: The mechanism is the permission bitmask stored in the file inode (read, write, execute bits for user, group, and other) and the hardware/kernel validation logic that checks these bits against the calling process's effective UID/GID during `open()`. The policy is the specific configuration set by the administrator or user (e.g., setting a file mode to `0600` for private credentials vs. `0644` for public reading).
> 2. CPU Scheduling: The mechanism is the timer interrupt, context-switch logic, register saving/restoring routines, and runqueue data structures that can preempt one thread and dispatch another. The policy is the decision algorithm (e.g., Linux CFS vs. FIFO vs. Round Robin with 10ms quantum) that decides *which* thread on the runqueue runs next and for how long.
> Separating policy from mechanism enables operating systems to adapt to vastly different application requirements (e.g., servers vs. mobile devices) without rewriting core hardware-interfacing code.

> [!question]- Q11. A cloud server hosts two competing workloads: a latency-critical web service and a batch scientific computing simulation. Both run in separate unprivileged protection domains on the same physical CPU. Describe how the operating system manages memory and CPU resources to provide mutual isolation while arbitrating shared hardware. What failure modes occur if the OS enforces mechanisms without proper policy throttling? (concepts: L01-01, L01-04, L01-05, L01-06)
> To provide mutual isolation:
> 1. CPU Arbitration: The OS utilizes preemptive priority scheduling (e.g., Linux SCHED_OTHER with dynamic nice values or cgroup CPU shares/bandwidth limits). The latency-critical service is granted higher scheduling priority or dedicated CPU affinity, allowing it to preempt the batch simulation immediately upon packet arrival.
> 2. Memory Arbitration: The OS allocates distinct page table hierarchies, isolating virtual address spaces. Physical memory is partitioned using cgroup memory limits to prevent the batch simulation from consuming all physical RAM and forcing the web service's active working set to swap.
>
> Failure modes without proper policy throttling:
> - Priority Inversion and Starvation: If the batch simulation monopolizes memory bandwidth or disk I/O channels without I/O scheduling throttling (e.g., blkio cgroups), the latency-critical service experiences severe tail-latency spikes due to bus contention and shared cache eviction.
> - Memory Thrashing: If the OS allows the batch simulation to overcommit memory without cgroup limits, the kernel's page reclaim daemon (`kswapd`) consumes excessive CPU cycles evicting pages across all processes, driving the system into thrashing where execution time is dominated by disk paging rather than useful computation.
