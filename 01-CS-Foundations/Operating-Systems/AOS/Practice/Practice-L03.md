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

> [!question]- Q1. Compare and contrast native bare-metal (Type-1) hypervisors and hosted (Type-2) hypervisors. What are the key architectural differences in how they provide platform virtualization, and what are their respective performance bottlenecks? (concepts: L03a-01, L03a-02, L03a-06)
> Native (Type-1) hypervisors (e.g., VMware ESXi, Xen, modern KVM on Linux) execute directly on bare physical hardware at the highest privilege level (Ring 0 / VMX root mode).
> They own physical memory management, CPU scheduling, and hardware device drivers directly.
> Because there is no intermediary host OS, Type-1 hypervisors achieve near-native CPU and I/O performance with minimal virtualization tax.
> Hosted (Type-2) hypervisors (e.g., VirtualBox, VMware Workstation) run as user-space processes within a conventional host operating system (e.g., macOS, Windows, Linux).
> The Type-2 hypervisor relies on the host OS kernel for physical memory allocation, hardware device access, and scheduling.
> The primary bottleneck of Type-2 hypervisors is the double scheduling and context-switching overhead: any guest I/O or privileged instruction trap must traverse the host OS kernel, transition to the hypervisor process, handle emulation, and transition back through the host OS, introducing substantial latency.

> [!question]- Q2. Explain the Popek-Goldberg virtualization theorem and how the x86-32 architecture historically violated its core condition. How did VMware full virtualization solve this using dynamic binary translation, and how did Xen paravirtualization bypass it? (concepts: L03a-03, L03a-04, L03a-05)
> The Popek-Goldberg virtualization theorem states that an architecture is fully virtualizable if and only if all sensitive instructions (instructions that expose or modify physical hardware state, such as `CLI`, `STI`, `POPF`, or reading base register addresses) are a strict subset of privileged instructions (instructions that trap when executed in unprivileged user mode).
> In x86-32, this condition was violated by 17 sensitive but unprivileged instructions.
> For example, `POPF` alters the hardware interrupt flag `IF` in Ring 0, but when executed in Ring 1 or Ring 3, it silently ignored the flag modification without generating a trap.
> A classic trap-and-emulate hypervisor could not detect or intercept this state change.
> Solutions:
> - VMware Full Virtualization (Binary Translation): VMware ran the guest OS at Ring 1 and parsed the guest instruction stream in basic blocks at runtime. Sensitive, non-trapping instructions were dynamically replaced with safe trap instructions or direct calls into VMM emulation routines before execution on the physical CPU.
> - Xen Paravirtualization: Instead of dynamically translating binary code, Xen modified the source code of the guest OS. Sensitive instructions were explicitly replaced with hypercalls (software traps to Ring 0 / hypervisor), eliminating the virtualization hole without requiring binary translation or hardware support.

> [!question]- Q3. How did modern hardware-assisted virtualization extensions (Intel VT-x, AMD-V, ARM EL2) transform CPU and platform virtualization? Describe the architectural execution modes introduced and how they eliminated the "Ring Compression" problem. (concepts: L03a-06, L03a-07)
> Prior to hardware assistance, hypervisors suffered from "Ring Compression": the x86 architecture provided 4 rings (0 to 3).
> Because the hypervisor occupied Ring 0, the guest kernel was forced to run in Ring 1 or Ring 3 alongside or just above user applications, requiring complex segment manipulation and binary translation.
> Intel VT-x and AMD-V solved this by introducing dual execution modes orthogonal to the existing Ring 0-3 privilege levels:
> - VMX Root Operation: The fully privileged mode where the hypervisor executes.
> - VMX Non-Root Operation: The execution mode where guest software executes. Both the guest kernel (running at non-root Ring 0) and guest user applications (running at non-root Ring 3) execute unmodified.
>
> Hardware enforces virtualization boundaries using a hardware-managed structure (the Virtual Machine Control Structure / VMCS on Intel, VMCB on AMD).
> When the guest executes a sensitive instruction, accesses privileged control registers, or receives an external interrupt, the hardware automatically saves the guest CPU state into the VMCS, switches from non-root to root mode (a VM-Exit), and vectors control directly to the hypervisor's exit handler.
> After handling the event, the hypervisor executes `VMLAUNCH`/`VMRESUME`, which hardware-loads the state and performs a VM-Entry back to non-root mode.

## L03b Memory Virtualization

> [!question]- Q4. Distinguish between virtual, physical, and machine addresses in a virtualized system. Explain how software shadow page tables (SPT) bridge this gap in full virtualization, and why synchronizing shadow page tables introduces significant overhead. (concepts: L03b-01, L03b-02, L03b-03, L03b-04)
> - Guest Virtual Address (GVA): The address space generated by guest applications.
> - Guest Physical Address (GPA): The contiguous address space the guest OS believes is physical RAM.
> - Host Machine Address (HMA): The actual physical RAM installed on the motherboard.
>
> In software virtualization, the hardware MMU can only point to a single page table (e.g., `CR3` register) that maps directly to Host Machine Addresses (HMA).
> Because the guest OS maintains its own page tables mapping GVA $\rightarrow$ GPA, the hypervisor must construct a Shadow Page Table (SPT) that maps GVA directly to HMA.
> The primary overhead is synchronization:
> 1. To detect when the guest OS creates or modifies its page tables, the hypervisor write-protects all guest physical memory pages containing guest page tables.
> 2. Whenever the guest kernel updates a page table entry (e.g., allocating a heap page for an application), the CPU triggers a write-protection page fault.
> 3. This fault traps into the hypervisor (VM-Exit), requiring the VMM to decode the faulting instruction, emulate the guest write to the guest page table, and update the corresponding entry in the shadow page table before resuming the guest.
> A single process startup or memory allocation burst can trigger thousands of costly VM-Exits.

> [!question]- Q5. On a system using two-dimensional hardware nested paging (Intel EPT or AMD NPT), assume both the guest OS and the host hypervisor utilize 4-level page tables (PML4, PDPT, PD, PT) with 4 KB pages. Calculate the worst-case number of memory accesses required to translate a single Guest Virtual Address (GVA) to a Host Machine Address (HMA) when every access misses the TLB and EPT TLB caches. Contrast this with the memory access cost of a populated Shadow Page Table. (concepts: L03b-05, L03b-12)
> In 4-level nested paging, every access to a guest page table pointer (which is a GPA) must itself be translated by the 4-level EPT to an HMA!
> Step-by-step breakdown:
> 1. The guest base register (Guest `CR3`) contains a GPA. Translating this GPA to an HMA requires walking the 4-level EPT: 4 memory accesses.
> 2. With the base of Guest PML4 in hand, the CPU looks up the PML4 entry (PML4E). Translating this GPA requires walking the 4-level EPT: 4 memory accesses.
> 3. With the base of Guest PDPT in hand, looking up the PDPTE requires walking the 4-level EPT: 4 memory accesses.
> 4. With the base of Guest PD in hand, looking up the PDE requires walking the 4-level EPT: 4 memory accesses.
> 5. With the base of Guest PT in hand, looking up the PTE requires walking the 4-level EPT: 4 memory accesses.
> 6. Finally, the target GPA is resolved, and translating the target page's GPA to an HMA requires walking the 4-level EPT: 4 memory accesses.
>
> General formula for $L_{\text{guest}}$ levels of guest tables and $L_{\text{host}}$ levels of host EPT:
> $$\text{Worst-Case Accesses} = (L_{\text{guest}} + 1) \times L_{\text{host}} + L_{\text{guest}} = (4 + 1) \times 4 + 4 = 20 + 4 = 24\text{ memory references!}$$
> (Or $(L_{\text{guest}} + 1) \times (L_{\text{host}} + 1) - 1 = 5 \times 5 - 1 = 24$ accesses).
> In contrast, once a Shadow Page Table is populated, translating a GVA takes exactly 4 memory accesses (one 4-level walk directly to HMA).
> Tradeoff: Nested paging avoids all VM-Exit trapping on guest page table writes, but incurs severe TLB-miss walk latency (up to 24 DRAM references), which hardware mitigates using dedicated EPT page-walk caches.

> [!question]- Q6. Explain why hypervisor-level paging causes the "double paging" pathological anomaly. Describe how VMware ESX ballooning solves this, and identify under what specific condition ballooning fails. (concepts: L03b-06, L03b-07, P-VMware-ESX-Memory)
> Double paging occurs when the hypervisor faces memory pressure and unilaterally swaps out a guest machine page (HMA) to the hypervisor swap disk.
> Because the hypervisor cannot inspect the guest OS's internal page replacement queues (e.g., active vs. inactive lists), it may inadvertently swap out an active page or a page belonging to the guest's idle free list.
> Later, when the guest OS decides to evict that same page to its own virtual disk, the guest kernel attempts to read the page, forcing the hypervisor to read it back from the hypervisor swap disk into memory, only for the guest OS to immediately write it out to the guest swap disk!
>
> Ballooning Solution:
> A balloon pseudo-device driver is installed inside the guest OS.
> When the hypervisor needs memory, it commands the balloon driver to inflate by $X$ MB.
> The balloon driver invokes the guest OS's native memory allocation routines (`malloc` / `alloc_pages`) to allocate pinned physical memory.
> If the guest is under memory pressure, the guest OS's own intelligent page replacement algorithm selects its own least valuable, cold application pages and writes them to the guest swap disk.
> The balloon driver then hands the allocated Guest Physical Page Numbers (PPNs) to the hypervisor, which unmaps their machine frames (MPNs) and reallocates them to other VMs.
> No double paging occurs because the guest OS made the eviction decision.
>
> Failure condition:
> Ballooning fails if the guest OS kernel is unresponsive, the guest balloon driver is uninstalled or disabled, or the guest OS cannot page out memory fast enough to relieve immediate, critical host memory exhaustion.
> In that scenario, the hypervisor must fall back to emergency hypervisor-level swapping.

> [!question]- Q7. A virtualization host runs two virtual machines, VM 1 and VM 2.
> - VM 1: Allocated 4000 shares, currently assigned $P_1 = 2000$ pages. Active working set fraction $f_1 = 0.30$ (30% active, 70% idle).
> - VM 2: Allocated 1000 shares, currently assigned $P_2 = 1000$ pages. Active working set fraction $f_2 = 0.90$ (90% active, 10% idle).
> The hypervisor enforces an idle memory tax with tax rate $\tau = 0.75$ (75 percent).
> (a) Calculate the penalty multiplier $k = \frac{1}{1 - \tau}$.
> (b) Calculate the adjusted pages for both VMs.
> (c) Calculate the effective share price for both VMs, and state which VM will have pages revoked first under memory pressure.
> (d) Show how this outcome contrasts with a pure share-based allocation without tax. (concepts: L03b-08, L03b-09, L03b-10, L03b-11, P-VMware-ESX-Memory)
> (a) Penalty multiplier $k$:
> $$k = \frac{1}{1 - \tau} = \frac{1}{1 - 0.75} = \frac{1}{0.25} = 4.0$$
>
> (b) Adjusted pages:
> For VM 1:
> $$\text{Active}_1 = 2000 \times 0.30 = 600\text{ pages}$$
> $$\text{Idle}_1 = 2000 \times 0.70 = 1400\text{ pages}$$
> $$\text{Adjusted Pages}_1 = \text{Active}_1 + k \cdot \text{Idle}_1 = 600 + (4.0 \times 1400) = 600 + 5600 = 6200\text{ pages}$$
>
> For VM 2:
> $$\text{Active}_2 = 1000 \times 0.90 = 900\text{ pages}$$
> $$\text{Idle}_2 = 1000 \times 0.10 = 100\text{ pages}$$
> $$\text{Adjusted Pages}_2 = \text{Active}_2 + k \cdot \text{Idle}_2 = 900 + (4.0 \times 100) = 900 + 400 = 1300\text{ pages}$$
>
> (c) Effective share price ($R = \frac{S}{\text{Adjusted Pages}}$):
> $$R_1 = \frac{4000}{6200} \approx 0.645$$
> $$R_2 = \frac{1000}{1300} \approx 0.769$$
> The hypervisor revokes pages from the VM with the lowest effective share price.
> Because $0.645 < 0.769$, VM 1 will be targeted for page reclamation first!
>
> (d) Contrast with pure share-based allocation (tax rate $\tau = 0$):
> Without tax ($k = 1.0$):
> $$\text{Price}_1 = \frac{4000}{2000} = 2.00$$
> $$\text{Price}_2 = \frac{1000}{1000} = 1.00$$
> Under a pure share-based policy, VM 2 would be aggressively revoked first because $1.00 < 2.00$, allowing VM 1 to hoard 1400 completely idle pages while starving VM 2's actively working workload.
> The idle tax effectively forces the system to behave like a working-set allocator when idle memory exists, while preserving proportional shares among active pages.

> [!question]- Q8. Describe how Content-Based Page Sharing (CBPS) operates in VMware ESX Server. How does it prevent false hash collisions, and how does copy-on-write (COW) protect page integrity when a guest attempts to modify a shared page? (concepts: L03b-08, L03b-12, P-VMware-ESX-Memory)
> Content-Based Page Sharing scans machine memory to deduplicate identical pages across different virtual machines without guest OS cooperation.
> Algorithm:
> 1. A background scanner thread traverses physical machine pages.
> 2. For each candidate page $P$, it computes a 64-bit hashing signature of the page contents.
> 3. It looks up the hash in a global hash table:
>    - Hash Miss: The hash is inserted into the table as a "hint" along with the page's machine frame address (MPN). No sharing occurs yet.
>    - Hash Hit: A matching entry already exists in the table, pointing to page $P'$. Because hash functions can produce collisions on different data, the hypervisor performs a full, byte-for-byte comparison between $P$ and $P'$.
> 4. If the byte comparison succeeds (true duplicate):
>    - The hypervisor updates the guest page tables pointing to $P$ to instead point to $P'$.
>    - Both page table entries are marked as Read-Only and Copy-On-Write (COW).
>    - Page $P$'s original machine frame is released back to the hypervisor's free list.
> 5. When any guest attempts to write to the shared page, the CPU hardware generates a write-protection fault (VM-Exit).
>    The hypervisor intercepts the fault, allocates a fresh, private machine frame, copies the contents of $P'$ to the new frame, updates the faulting guest's page table with Read-Write permissions, and resumes execution.

## L03c CPU and Device Virtualization

> [!question]- Q9. How does a hypervisor intercept, virtualize, and inject program discontinuities (exceptions, system calls, page faults, and external hardware interrupts) into guest operating systems? How do modern hardware VM-Execution controls optimize guest system call performance by avoiding hypervisor intervention? (concepts: L03c-01, L03c-02, L03c-03)
> In a native environment, program discontinuities (traps, faults, interrupts) transfer control directly from user mode to the OS kernel via the hardware Interrupt Descriptor Table (IDT).
> In a virtualized system:
> 1. Trapping: When a discontinuity occurs in guest non-root mode, the hardware consults the VMCS exception and pin-based execution bitmaps configured by the hypervisor. If the event is configured to trap (e.g., page faults under shadow paging, external device interrupts, or hypercalls), the hardware triggers a VM-Exit, saving guest state and transferring execution to the hypervisor in VMX root mode.
> 2. Virtual Discontinuity Injection: The hypervisor evaluates whether the event belongs to the guest OS (e.g., a guest application page fault or guest timer interrupt). If so, the hypervisor encodes the vector number, exception error code, and type into the VMCS `VM-Entry Interruption-Information` field. Upon the next `VMRESUME`, the hardware injects the interrupt/exception directly into the guest OS kernel as if delivered by real physical hardware.
> 3. Optimization (Bypassing the Hypervisor): Modern hardware allows guest OS system calls (`syscall`/`sysret`) and certain benign exceptions (e.g., divide-by-zero, unshadowed page faults under EPT) to execute entirely within non-root mode directly between guest user space (non-root Ring 3) and guest kernel space (non-root Ring 0), incurring zero VM-Exits and achieving native syscall performance.

> [!question]- Q10. In Xen paravirtualization, explain how control transfer and data transfer are architecturally decoupled for I/O devices. Describe the role of hypercalls, event channels, and asynchronous I/O rings. (concepts: L03c-04, L03c-05, L03c-06, L03c-07, P-Xen)
> Xen separates control signaling from bulk data movement to achieve near-bare-metal I/O throughput:
> 1. Control Transfer (Signaling):
>    - Hypercalls: Synchronous software traps executed by guest OS kernels to invoke privileged hypervisor operations (equivalent to system calls in traditional OSes). Used for establishing memory mappings and registering event handlers.
>    - Event Channels: Asynchronous software interrupts used by Xen to notify guest domains of asynchronous events (e.g., packet arrival, disk request completion). Event channels replace physical hardware interrupts and avoid per-event trapping overhead by using shared bitmasks.
> 2. Data Transfer (Bulk Movement):
>    - Asynchronous I/O Rings: Circular buffers allocated in memory pages shared between the unprivileged guest domain (frontend driver) and the privileged driver domain / Dom0 (backend driver).
>    - The ring buffer uses producer-consumer pointers (`req_prod`, `req_cons`, `rsp_prod`, `rsp_cons`).
>    - The frontend places I/O descriptor requests onto the ring and updates `req_prod`.
>    - The backend processes requests out-of-order using descriptor tags and writes completion responses, updating `rsp_prod`.
>    - Event channel notifications are only triggered when crossing threshold flags (event notification suppression), allowing high-load streaming to operate with zero interrupt overhead via batching.

> [!question]- Q11. Compare device emulation, para-virtualized split drivers (virtio), and hardware direct assignment (SR-IOV). What are the tradeoffs between performance, guest portability, and live migration capabilities? (concepts: L03c-04, L03c-05, L03c-08, L03c-09, P-Xen)
> | Virtualization Mode | Performance | Guest Modification | Live Migration | Complexity |
> | :--- | :--- | :--- | :--- | :--- |
> | **Full Device Emulation** (e.g., QEMU IDE / e1000) | Poor (high trap-and-emulate overhead per register access) | None (unmodified legacy OS) | Excellent (hypervisor captures entire device state) | High in software |
> | **Paravirtualized Split Driver** (virtio) | Very High (shared memory rings, batched notifications) | Requires virtio guest drivers | Excellent (standardized state capture) | Moderate |
> | **Direct Assignment / SR-IOV** | Near Bare-Metal (bypasses hypervisor completely via PCIe VFs) | Requires physical NIC vendor driver | Difficult (hardware state pinned to physical PCIe card) | High in hardware |
>
> Architectural Tradeoff:
> - Device emulation traps on every I/O port instruction (`IN`/`OUT`) or MMIO access, incurring hundreds of VM-Exits per packet or sector.
> - virtio replaces register traps with shared memory rings and event channels, dramatically improving throughput while preserving live migration because the hypervisor controls the virtualization protocol.
> - Single Root I/O Virtualization (SR-IOV) partitions a physical PCIe device into multiple Virtual Functions (VFs). Using the hardware IOMMU (Intel VT-d), guest physical pages are mapped directly to PCIe DMA channels. Guest reads and writes talk directly to hardware with zero hypervisor intervention, but live migration requires bonding the SR-IOV VF to a virtio failover interface.

> [!question]- Q12. Explain how proportional-share CPU schedulers in hypervisors (such as the Xen Credit Scheduler) allocate processor time among virtual CPUs (vCPUs). What is the difference between work-conserving and non-work-conserving modes, and how does the Credit Scheduler prevent vCPU starvation? (concepts: L03c-01, L03c-02, P-Xen)
> In Xen's Credit Scheduler, each guest domain is assigned a `weight` (proportional share) and an optional `cap` (maximum CPU percentage).
> Operation:
> 1. Accounting: Every 30 ms accounting epoch, the scheduler computes credits for each domain proportional to its weight:
>    $$\text{Credits} = \text{Epoch Time} \times \frac{\text{Weight}_i}{\sum \text{Weight}_j}$$
> 2. Execution: As a vCPU executes, its credits are continuously deducted by a timer tick (typically every 10 ms).
> 3. Priority States:
>    - `UNDER`: The vCPU has positive remaining credits ($> 0$).
>    - `OVER`: The vCPU has exhausted its credits ($\le 0$).
>    - `BOOST`: A temporary high priority granted to I/O-blocked vCPUs waking up on event channel notifications to ensure low packet processing latency.
>
> Work-Conserving vs. Non-Work-Conserving:
> - Work-Conserving Mode: If some vCPUs are idle, active vCPUs are allowed to consume excess CPU capacity beyond their share. A vCPU in the `OVER` state can still run if no other vCPU in `UNDER` or `BOOST` state is runnable, maximizing overall hardware utilization.
> - Non-Work-Conserving Mode: Enforced when a domain has a set `cap`. Even if physical CPUs are completely idle, a capped vCPU that exhausts its credits is forcibly descheduled until the next credit allocation epoch.
>
> Starvation Prevention:
> Because credits are replenished periodically to all active domains, even heavily penalized CPU-hog domains are reset to positive credits at the start of every epoch, guaranteeing fair-share progress and preventing starvation.
