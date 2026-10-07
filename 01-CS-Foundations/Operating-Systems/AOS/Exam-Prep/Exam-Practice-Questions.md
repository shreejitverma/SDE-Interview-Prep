---
type: playbook
track: [sde, distinguished]
level:
status: solid
last_reviewed:
sources:
  - "Georgia Tech CS 6210 Advanced Operating Systems Exam Material"
  - "Bershad et al., SPIN (SOSP 1995)"
  - "Engler et al., Exokernel (SOSP 1995)"
  - "Liedtke, On Micro-Kernel Construction (SOSP 1995)"
  - "Waldspurger, VMware ESX Server Memory Management (OSDI 2002)"
  - "Barham et al., Xen and the Art of Virtualization (SOSP 2003)"
  - "Mellor-Crummey & Scott, Scalable Synchronization (ACM TOCS 1991)"
  - "Bershad et al., Lightweight Remote Procedure Call (ACM TOCS 1990)"
  - "Lamport, Time, Clocks, and the Ordering of Events (CACM 1978)"
  - "Keleher et al., TreadMarks: Shared Memory Computing on Networks of Workstations (USENIX 1994)"
  - "Feeley et al., Implementing Global Memory Management in a Workstation Cluster (SOSP 1995)"
  - "Satyanarayanan et al., Lightweight Recoverable Virtual Memory (ACM TOCS 1994)"
  - "Lowell & Chen, Free Transactions with Rio Vista (SOSP 1997)"
  - "Fox & Brewer, Harvest, Yield, and Scalable Tolerant Systems (HOT-OS 1999)"
  - "DeCandia et al., Dynamo: Amazon's Highly Available Key-value Store (SOSP 2007)"
tags: [cs6210, cs6210/exam, exam-prep, advanced-os]
---

# CS 6210 AOS - Master Exam Practice Questions

> Comprehensive exam practice set covering Test 1 (Part 1 & 2), Test 2 (Part 3 & L07), and Test 3 (L08 & Part 5).
> Questions are formulated at the rigorous graduate exam level, emphasizing architectural tradeoffs, quantitative calculations, and seminal paper designs.
> Each question is presented in an Obsidian callout with folded answers for active recall.

Back to [AOS Dashboard](../README.md) | [Study Plan](../00-Study-Plan.md) | [Practice Index](../Practice/README.md).

## Index

| # | Topic | Scope | Lecture / Paper | Type |
|---|-------|-------|-----------------|------|
| [1](#1-os-border-crossing-cost-models) | Border-Crossing Cost Models | Test 1 | [L02a](../Part-1-OS-Structure-and-Virtualization/L02a-OS-Structure-Overview.md), [L02b](../Part-1-OS-Structure-and-Virtualization/L02b-SPIN-Approach.md), [L02d](../Part-1-OS-Structure-and-Virtualization/L02d-L3-Microkernel-Approach.md) | Calculation |
| [2](#2-esx-idle-memory-taxation) | ESX Server Idle Memory Tax | Test 1 | [L03b](../Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md), Waldspurger paper | Calculation |
| [3](#3-ept-nested-page-walk-latency) | EPT Two-Dimensional Page Walk | Test 1 | [L03b](../Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md#hardware-nested-paging-ept-and-npt) | Calculation |
| [4](#4-scalable-synchronization-locks) | Anderson vs. MCS Queue Locks | Test 1 | [L04b](../Part-2-Parallel-Systems/L04b-Synchronization.md), Mellor-Crummey & Scott | Analytical |
| [5](#5-barrier-synchronization-critical-paths) | Dissemination vs. Tournament Barriers | Test 1 | [L04c](../Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md) | Analysis & Complexity |
| [6](#6-lightweight-remote-procedure-call) | LRPC Architectural Pillars | Test 1 | [L04d](../Part-2-Parallel-Systems/L04d-Lightweight-RPC.md), Bershad paper | Deep Dive |
| [7](#7-lamport-clocks-and-total-ordering) | Logical Clocks & Distributed Mutex | Test 2 | [L05b](../Part-3-Distributed-Systems/L05b-Lamport-Clocks.md), Lamport paper | Algorithmic |
| [8](#8-distributed-shared-memory-lrc) | TreadMarks & Lazy Release Consistency | Test 2 | [L07b](../Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md), Keleher paper | Architectural |
| [9](#9-global-memory-systems-page-replacement) | GMS Page Weight & Swapping | Test 2 | [L07a](../Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md), Feeley paper | Analytical |
| [10](#10-recoverable-virtual-memory) | LRVM vs. Rio Vista Recovery | Test 3 | [L08a](../Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md), [L08b](../Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md) | Comparison |
| [11](#11-giant-scale-services-dq-principle) | Harvest, Yield, and Graceful Degradation | Test 3 | [L09a](../Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md), Fox & Brewer | Quantitative |
| [12](#12-consistent-hashing-and-quorums) | Dynamo Ring, Sloppy Quorums & Anti-Entropy | Test 3 | [L09c](../Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md), Dynamo paper | System Design |

---

## 1. OS Border-Crossing Cost Models

> [!question] Question
> A system handles an I/O event requiring a policy decision. Consider three operating system architectures running on identical hardware:
> - **Monolithic Kernel:** Trap overhead is 100 cycles, in-kernel policy function execution takes 60 cycles, and return-from-trap takes 100 cycles.
> - **Traditional Microkernel (Mach-style):** Initial trap into kernel is 100 cycles. Switching address space to a user-level policy server takes 850 cycles (TLB flush, CR3 reload, cache penalty). The user server executes the policy in 60 cycles. The user server traps to the kernel to reply (100 cycles). The kernel switches address space back to the original caller (850 cycles) and returns to user mode (100 cycles).
> - **Extensible Kernel (SPIN-style):** The extension is written in a type-safe language and dynamically compiled into the kernel address space. Trapping to the kernel takes 100 cycles. Event dispatch via indirect procedure call takes 15 cycles. Policy execution takes 60 cycles. Return from procedure takes 10 cycles. Return-from-trap takes 100 cycles.
>
> 1. Calculate the total CPU cycle cost for each of the three approaches.
> 2. What is the speedup factor of SPIN relative to the Mach-style microkernel?
> 3. Why does SPIN achieve near-monolithic performance without sacrificing security, and how does Exokernel achieve a similar objective using a different mechanism?

> [!success]- Answer
> **1. Cycle Calculations:**
> - **Monolithic:**
>   $$C_{\text{mono}} = 100\text{ (trap)} + 60\text{ (exec)} + 100\text{ (return)} = 260\text{ cycles}$$
> - **Microkernel:**
>   $$C_{\text{micro}} = 100\text{ (trap)} + 850\text{ (switch to server)} + 60\text{ (exec)} + 100\text{ (reply trap)} + 850\text{ (switch back)} + 100\text{ (return)} = 2060\text{ cycles}$$
> - **SPIN:**
>   $$C_{\text{spin}} = 100\text{ (trap)} + 15\text{ (dispatch)} + 60\text{ (exec)} + 10\text{ (ret)} + 100\text{ (return)} = 285\text{ cycles}$$
>
> **2. Speedup Factor:**
> $$\text{Speedup} = \frac{C_{\text{micro}}}{C_{\text{spin}}} = \frac{2060}{285} \approx 7.23\times$$
>
> **3. Architectural Mechanisms:**
> - **SPIN:** Colocates user extensions directly within the privileged kernel address space to eliminate border crossings. Memory safety and fault isolation are enforced via **language-level type safety** (Modula-3), logical capability interfaces, and compile-time verification rather than hardware page tables.
> - **Exokernel:** Separates resource protection from management. The unprivileged micro-layer (Aegis/Xok) exports physical resources directly using secure bindings (capabilities, packet filters, software TLB handlers). Complex OS abstractions and policies are pushed down into unprivileged user-space **Library OSes** (e.g., ExOS), eliminating kernel intervention entirely for standard operations.

---

## 2. ESX Idle Memory Taxation

> [!question] Question
> A VMware ESX host running two virtual machines experiences severe memory contention:
> - **VM Alpha (Production DB):** Assigned $S_A = 6000$ shares, currently holding $P_A = 4000$ allocated physical pages. Its active working set fraction is $f_A = 0.25$ (25% active, 75% idle).
> - **VM Beta (Web Worker):** Assigned $S_B = 2000$ shares, currently holding $P_B = 2000$ allocated physical pages. Its active working set fraction is $f_B = 0.85$ (85% active, 15% idle).
>
> The hypervisor uses dynamic min-funding revocation with an idle memory tax rate $\tau = 0.80$ (80%).
> 1. Calculate the tax penalty multiplier $k = \frac{1}{1 - \tau}$.
> 2. Determine the adjusted page counts for both VMs.
> 3. Calculate the effective share price for both VMs. Which VM will the hypervisor target first for page reclamation?
> 4. If the tax rate were set to $\tau = 0$ (pure proportional shares), which VM would be targeted? Explain why the idle tax is vital in consolidated cloud environments.

> [!success]- Answer
> **1. Penalty Multiplier $k$:**
> $$k = \frac{1}{1 - \tau} = \frac{1}{1 - 0.80} = \frac{1}{0.20} = 5.0$$
>
> **2. Adjusted Page Counts ($P_{\text{adj}} = \text{Active} + k \cdot \text{Idle}$):**
> - For VM Alpha:
>   $$\text{Active}_A = 4000 \times 0.25 = 1000\text{ pages}$$
>   $$\text{Idle}_A = 4000 \times 0.75 = 3000\text{ pages}$$
>   $$P_{\text{adj}, A} = 1000 + (5.0 \times 3000) = 1000 + 15000 = 16,000\text{ pages}$$
> - For VM Beta:
>   $$\text{Active}_B = 2000 \times 0.85 = 1700\text{ pages}$$
>   $$\text{Idle}_B = 2000 \times 0.15 = 300\text{ pages}$$
>   $$P_{\text{adj}, B} = 1700 + (5.0 \times 300) = 1700 + 1500 = 3200\text{ pages}$$
>
> **3. Effective Share Price ($R = \frac{S}{P_{\text{adj}}}$):**
> $$R_A = \frac{6000}{16000} = 0.375$$
> $$R_B = \frac{2000}{3200} = 0.625$$
> Since $R_A = 0.375 < R_B = 0.625$, the hypervisor targets **VM Alpha** first for page reclamation.
>
> **4. Pure Proportional Shares ($\tau = 0 \implies k = 1.0$):**
> $$R_{A, \text{base}} = \frac{6000}{4000} = 1.50$$
> $$R_{B, \text{base}} = \frac{2000}{2000} = 1.00$$
> Without the idle tax, VM Beta ($R=1.00$) would be reclaimed first because its base share-to-page ratio is lower.
> This would cause VM Beta's active working set to be evicted while VM Alpha hoards 3000 completely unused idle pages.
> The idle tax inflates the effective page divisor of idle allocations, lowering their marginal price and forcing idle memory to be reclaimed before active memory.

---

## 3. EPT Nested Page Walk Latency

> [!question] Question
> A virtual machine executes on an x86-64 CPU equipped with Extended Page Tables (EPT).
> Both the guest OS and the host hypervisor utilize standard 4-level paging structures (PML4, PDPT, PD, PT) with 4 KB page frames.
>
> 1. In the worst-case scenario where all TLB and EPT walk caches miss, derive the exact number of physical memory accesses required to translate a single Guest Virtual Address (GVA) to a Host Machine Address (HMA).
> 2. If DRAM access latency is 60 ns, what is the latency penalty of this cold translation walk?
> 3. How does hardware mitigate this multi-dimensional translation penalty in practice?

> [!success]- Answer
> **1. Derivation of 2D Page Walk Memory Accesses:**
> Let $L_g = 4$ (guest page table levels) and $L_h = 4$ (host EPT levels).
> - The guest `CR3` register holds the Guest Physical Address (GPA) of the guest PML4 table. Translating this GPA to an HMA requires a full 4-level EPT walk: **4 accesses**.
> - With the guest PML4 table located, the guest PML4 entry is fetched. Its content is the GPA of the guest PDPT. Translating this GPA requires a 4-level EPT walk: **4 accesses**.
> - With the guest PDPT located, the PDPTE is fetched. Its content is the GPA of the guest PD. Translating this GPA requires a 4-level EPT walk: **4 accesses**.
> - With the guest PD located, the PDE is fetched. Its content is the GPA of the guest PT. Translating this GPA requires a 4-level EPT walk: **4 accesses**.
> - With the guest PT located, the PTE is fetched. Its content is the GPA of the target data frame. Translating this final GPA requires a 4-level EPT walk: **4 accesses**.
> - Plus, reading each of the 4 guest entries requires **4 accesses** (one at each guest level).
>
> General formula:
> $$\text{Total Accesses} = (L_g + 1) \times (L_h + 1) - 1 = (4 + 1) \times (4 + 1) - 1 = 5 \times 5 - 1 = 24\text{ memory references}$$
>
> **2. Latency Penalty:**
> $$\text{Latency} = 24 \times 60\text{ ns} = 1440\text{ ns} = 1.44\ \mu\text{s}$$
> This is over an order of magnitude slower than a native 4-level page walk ($4 \times 60\text{ ns} = 240\text{ ns}$).
>
> **3. Practical Mitigations:**
> - **EPT Page-Walk Caches:** Modern CPUs incorporate dedicated hardware caches that store intermediate translations for higher-level EPT entries (PML4/PDPT), collapsing the nested walk to 5-6 references on typical TLB misses.
> - **Large Pages (Hugepages):** Using 2 MB or 1 GB pages for guest allocations reduces the depth of both the guest tables and the host EPT, cutting walk steps significantly.

---

## 4. Scalable Synchronization Locks

> [!question] Question
> On large cache-coherent distributed shared-memory multiprocessors, compare the **Anderson Array-based Queue Lock** and the **Mellor-Crummey & Scott (MCS) Linked-List Queue Lock**:
>
> 1. How does each lock eliminate the $O(P)$ bus invalidation storm inherent in test-and-test-and-set (TATAS) spinlocks upon lock release?
> 2. What are the primary space complexity differences between Anderson and MCS?
> 3. Why does the MCS lock require an atomic Compare-And-Swap (CAS) on the release path, and what happens if only Fetch-And-Store (SWAP) is supported by hardware?

> [!success]- Answer
> **1. Eliminating Invalidation Storms:**
> - **Anderson Lock:** Requesters obtain an atomic ticket modulo array size $N$ (`fetch_and_add`) and spin exclusively on their own designated slot (`flags[my_slot]`). When the lock holder releases, it writes to `flags[(my_slot + 1) % N]`, invalidating only the single cache line monitored by the immediate successor.
> - **MCS Lock:** Requesters allocate a local queue node and atomically append it to a global tail pointer via `fetch_and_store`. The waiting thread spins locally on its own node's `locked` flag (`my_node->locked`). When releasing, the holder sets `my_node->next->locked = false`, generating exactly one cache line invalidation directly to the next waiting core.
> Both ensure **local spinning** and $O(1)$ interconnect messages per lock handoff.
>
> **2. Space Complexity:**
> - **Anderson:** Requires $O(P \times L)$ space per lock, where $P$ is the maximum number of threads and $L$ is cache line padding to prevent false sharing. Space must be statically allocated up front for every lock instance, making it prohibitively expensive for fine-grained locking.
> - **MCS:** Requires only $O(1)$ space per lock (a single tail pointer), plus $O(P)$ space total across the system (one node per thread, typically allocated on the thread's stack). It dynamically scales with active contention.
>
> **3. CAS on MCS Release:**
> When the lock holder releases, it checks if it has a successor (`my_node->next == NULL`).
> If `next` is NULL, another thread might be concurrently appending itself.
> The releaser executes `CAS(&lock->tail, my_node, NULL)`:
> - If CAS succeeds, the queue was truly empty and the lock is unlocked atomically.
> - If CAS fails, a successor is currently executing `fetch_and_store` on the tail but has not yet updated `my_node->next`. The releaser spins until `my_node->next` is populated, then hands off the lock.
> If hardware only provides `fetch_and_store` (no CAS), the releaser must perform a multi-step protocol: atomically swap the tail to a temporary sentinel or coordinate with an auxiliary lock, introducing additional complexity and potential delay.

---

## 5. Barrier Synchronization Critical Paths

> [!question] Question
> Consider a parallel computation running across $N = 16$ processors.
>
> 1. Contrast the critical path length and total network message count of a **Centralized Sense-Reversing Counting Barrier**, an **MCS 4-ary Tree Barrier**, and a **Dissemination Barrier**.
> 2. How many communication rounds are required for the Dissemination Barrier when $N = 16$? Detail the exact partner distance at each round.
> 3. Under what workload conditions would a Dissemination Barrier outperform an MCS Tree Barrier, and vice versa?

> [!success]- Answer
> **1. Comparison for $N = 16$:**
>
> | Barrier Algorithm | Critical Path Length | Total Message Traffic | Local Spinning? |
> |---|---|---|---|
> | **Sense-Reversing Counting** | $O(N) = 16$ sequential atomic updates | $O(N^2)$ invalidations broadcast on release | No (spins on shared counter) |
> | **MCS Tree (4-ary arrival, 2-ary wakeup)** | $O(\log_4 N + \log_2 N) = 2 + 4 = 6$ hops | $O(N)$ point-to-point updates | Yes (spins on dedicated node flags) |
> | **Dissemination Barrier** | $\lceil \log_2 N \rceil = 4$ parallel rounds | $O(N \log_2 N) = 16 \times 4 = 64$ messages | Yes (spins on private round flags) |
>
> **2. Dissemination Barrier with $N = 16$:**
> Number of rounds: $\lceil \log_2 16 \rceil = 4$ rounds.
> In round $r$ ($r \in \{0, 1, 2, 3\}$), processor $i$ signals processor $(i + 2^r) \pmod{16}$:
> - Round 0: Distance $2^0 = 1$ (Processor $i$ signals $i + 1$)
> - Round 1: Distance $2^1 = 2$ (Processor $i$ signals $i + 2$)
> - Round 2: Distance $2^2 = 4$ (Processor $i$ signals $i + 4$)
> - Round 3: Distance $2^3 = 8$ (Processor $i$ signals $i + 8$)
> After 4 rounds, arrival knowledge has transitively propagated to all 16 processors with no centralized wakeup phase.
>
> **3. Workload Tradeoffs:**
> - **Dissemination Barrier Wins:** On systems with uniform network latency or systems lacking atomic hardware primitives, and when threads arrive nearly simultaneously. Its critical path has fewer sequential rounds ($\log_2 N$ vs. arrival + wakeup tree traversal).
> - **MCS Tree Barrier Wins:** On large-scale NUMA machines where network bandwidth is constrained or interconnect contention is high, because its total message volume scales strictly as $O(N)$ compared to $O(N \log N)$ for dissemination.

---

## 6. Lightweight Remote Procedure Call

> [!question] Question
> In their seminal 1990 paper, Bershad et al. introduced **Lightweight Remote Procedure Call (LRPC)** for cross-domain communication on the same physical machine.
>
> 1. Identify the four key performance bottlenecks of traditional RPC when applied to cross-protection-domain calls on the same machine.
> 2. Explain how LRPC's **A-Stack (Argument Stack)** design achieves zero-copy parameter passing.
> 3. Explain how **Handoff Scheduling** reduces context-switch latency during an LRPC invocation.
> 4. How does LRPC optimize cross-domain calls on Symmetric Multiprocessors (SMP)?

> [!success]- Answer
> **1. Bottlenecks of Traditional RPC on the Same Machine:**
> - **Multiple Data Copies:** Arguments are copied from client stack to RPC stub, from stub to kernel message buffer, from kernel to server stub buffer, and finally to server stack (4 copies).
> - **Two Context Switches:** Control transfer requires waking the server thread, putting the client thread to sleep, and running the full OS scheduler loop twice per round-trip.
> - **Memory Footprint & Thread Bloat:** Every server domain must maintain a dedicated pool of worker threads waiting on IPC queues.
> - **Cache & TLB Invalidation:** Full address-space context switches flush CPU caches and TLBs.
>
> **2. A-Stack Zero-Copy Passing:**
> At bind time, the kernel pairs the client and server and maps a shared argument stack (the A-Stack) into **both** the client's and the server's virtual address spaces.
> During an invocation, the client stub marshals parameters directly onto the shared A-Stack.
> The server executes directly on that same physical A-Stack without the kernel copying any data across protection boundaries.
>
> **3. Handoff Scheduling:**
> Instead of transitioning the client to a sleep queue and invoking the general OS scheduler, LRPC borrows the client thread's own CPU scheduling quantum.
> The kernel performs a fast **domain switch**: it swaps the page table base pointer (CR3) to the server's address space, updates the instruction pointer to the server entry point, and directly resumes execution on the current CPU core.
> The server executes on the client thread's time slice, completely bypassing scheduler queues.
>
> **4. Optimization on SMP:**
> LRPC caches idle execution domains on neighboring idle CPU processors.
> When an LRPC is issued, if an idle processor already has the server domain loaded in its MMU, the call can execute there immediately, avoiding even the local page table reload and keeping processor caches warm.

---

## 7. Lamport Clocks and Total Ordering

> [!question] Question
> In a distributed system with 3 processes ($P_1, P_2, P_3$), events are ordered using Lamport Logical Clocks.
>
> 1. State the **Clock Condition** and the two implementation rules ($IR_1$ and $IR_2$) defined by Leslie Lamport (1978).
> 2. Why does Lamport clock ordering ($a \to b \implies C(a) < C(b)$) not imply the converse ($C(a) < C(b) \centernot\implies a \to b$)? What synchronization primitive solves this limitation?
> 3. How does Lamport achieve a **total order** of all events in the system, and how is this applied to distributed mutual exclusion without a central coordinator?

> [!success]- Answer
> **1. Clock Condition and Implementation Rules:**
> - **Clock Condition:** For any events $a$ and $b$, if $a \to b$ (event $a$ causally precedes $b$), then $C(a) < C(b)$.
> - **Rule 1 ($IR_1$ - Local Event):** Each process $P_i$ increments its local clock between any two successive events:
>   $$C_i = C_i + 1$$
> - **Rule 2 ($IR_2$ - Message Passing):**
>   - When process $P_i$ sends message $m$, the message carries timestamp $T_m = C_i$.
>   - Upon receiving $(m, T_m)$, process $P_j$ advances its clock before delivering the message:
>     $$C_j = \max(C_j, T_m) + 1$$
>
> **2. Non-Invertibility & Solution:**
> - If $C(a) < C(b)$, events $a$ and $b$ may simply be concurrent ($a \parallel b$) with no causal communication between them. Scalar timestamps cannot distinguish between true causal precedence and concurrent execution.
> - **Vector Clocks** (Mattern / Fidge) solve this: by maintaining a vector of size $N$ representing the latest known clock of every process, vector clocks satisfy:
>   $$V(a) < V(b) \iff a \to b$$
>
> **3. Total Ordering & Distributed Mutex:**
> - **Total Ordering:** Ties between concurrent events with identical timestamps are broken deterministically using unique process identifiers:
>   $$a \Rightarrow b \iff (C(a) < C(b)) \lor (C(a) = C(b) \land P(a) < P(b))$$
> - **Distributed Mutual Exclusion:**
>   - A process requests the critical section by broadcasting `REQUEST(T, Pi)` to all peers and inserting the request into its own local queue.
>   - Peers reply with timestamped acknowledgments `ACK`.
>   - A process enters the critical section when:
>     1. Its own request is at the head of its local queue (ordered by total order $\Rightarrow$).
>     2. It has received a message (ACK or request) with timestamp $> T$ from *every* other process.
>   This guarantees mutual exclusion and fairness without a central bottleneck.

---

## 8. Distributed Shared Memory (LRC)

> [!question] Question
> The **TreadMarks** system implements Software Distributed Shared Memory (DSM) using **Lazy Release Consistency (LRC)** over commodity workstation networks.
>
> 1. Contrast **Eager Release Consistency (ERC)** with **Lazy Release Consistency (LRC)**. When are coherence messages transmitted under each model?
> 2. Explain how TreadMarks uses **Interval Timestamps**, **Vector Timestamps**, and **Write Notices** to track causal data modifications.
> 3. How do **Memory Diffs** (using twin pages) solve the false sharing problem where multiple processors write to distinct variables residing on the same 4 KB virtual memory page?

> [!success]- Answer
> **1. Eager vs. Lazy Release Consistency:**
> - **Eager RC:** When a thread executes an unlock (`release`), it immediately broadcasts all memory updates (or invalidations) made during the critical section to *all* nodes in the cluster.
> - **Lazy RC:** Postpones sending coherence information until the *next* thread acquires the lock. Coherence data travels strictly point-to-point between the last releaser and the new acquirer. If a node never acquires the lock or accesses the modified pages, it receives zero messages.
>
> **2. Tracking Modifications in TreadMarks:**
> - **Intervals:** A new interval begins on a node every time it executes an `acquire` or `release`.
> - **Vector Timestamps:** Each node tracks interval progression using a vector timestamp.
> - **Write Notices:** Instead of transmitting raw memory pages, a node records which pages were modified during an interval into a small metadata record called a *write notice*.
> - At lock acquire, the new acquirer passes its current vector timestamp to the previous holder; the previous holder returns only the write notices for intervals the acquirer has not yet observed. The acquirer invalidates the corresponding local pages in its page table.
>
> **3. Memory Diffs and False Sharing:**
> - When a page is first written, TreadMarks write-protects the page and saves an identical pristine copy called a **twin**.
> - At the end of the interval, TreadMarks performs a byte-by-byte comparison between the modified page and the twin, creating a run-length encoded **diff** containing only the modified bytes.
> - If Process A and Process B modify different variables on the same page concurrently, each generates a diff of its own changes.
> - When Process C later accesses the page, it fetches the diffs from both A and B and applies them sequentially to its base page.
> Because updates are applied as fine-grained byte diffs rather than whole-page overwrites, false sharing does not cause data corruption or ping-pong thrashing.

---

## 9. Global Memory Systems (GMS) Page Replacement

> [!question] Question
> Feeley et al. (SOSP 1995) designed the **Global Memory System (GMS)** to utilize cluster-wide idle RAM as a fast backing store, avoiding disk access across local area networks.
>
> 1. Describe the four state classifications of memory pages in GMS: Local-Private, Local-Shared, Global-Private, and Global-Shared.
> 2. Explain the **Global Min-Age Page Replacement Algorithm**. How does a node decide whether to evict a page to a remote node's memory or drop it entirely?
> 3. Why is swapping a page over a 1 Gbps LAN faster than reading from local magnetic disk, and what modern technology (e.g., RDMA / CXL) continues this principle?

> [!success]- Answer
> **1. Four GMS Page Classifications:**
> - **Local-Private:** Page is actively mapped into a local process address space on this node and has been modified (dirty). Evicting it requires saving to disk or remote backing memory.
> - **Local-Shared:** Page is actively accessed on this node, but identical cached read-only copies may exist elsewhere in the cluster (e.g., shared code or file cache).
> - **Global-Private:** Page is not actively in use on any node's working set, but represents the *only* valid in-memory copy of modified data in the cluster. It must be preserved.
> - **Global-Shared:** Page is idle across the cluster, but duplicates exist in persistent disk storage or on other nodes.
>
> **2. Global Min-Age Algorithm:**
> Nodes periodically exchange epoch timestamps and age estimates of their oldest pages.
> When Node $P$ needs to allocate a frame:
> 1. It identifies its oldest local page $L$.
> 2. If $L$ is younger than the global minimum age page across the cluster ($L > \text{MinAge}_{\text{cluster}}$), evicting $L$ to disk would be suboptimal.
> 3. Instead, Node $P$ sends $L$ across the network to Node $Q$ (the node holding the oldest idle page in the entire cluster).
> 4. Node $Q$ discards its oldest global page (or flushes it to disk if dirty) and absorbs page $L$ into its RAM.
> This ensures that the globally oldest page across the entire cluster is discarded first, preserving active working sets cluster-wide.
>
> **3. Latency Dynamics & Modern Evolution:**
> - Traditional disk seek + rotational latency took 5-10 ms (5,000-10,000 $\mu$s).
> - In contrast, sending 4 KB over a fast LAN took $< 200\ \mu\text{s}$, making remote cluster RAM 25-50$\times$ faster than local disk paging.
> - Modern Descendants: **RDMA (Remote Direct Memory Access)** and **CXL (Compute Express Link)** memory pooling allow servers to read/write remote node memory in $< 1\ \mu\text{s}$ bypassing remote CPUs entirely, fulfilling GMS's vision at hardware scale.

---

## 10. Recoverable Virtual Memory

> [!question] Question
> Compare **Lightweight Recoverable Virtual Memory (LRVM)** (Satyanarayanan et al., 1993) with **Rio Vista** (Lowell & Chen, 1997):
>
> 1. Explain LRVM's design principle of "no-flush on commit" and why it maintains a redo-only log on raw disk partitions.
> 2. How does Rio Vista eliminate the disk write bottleneck of LRVM during transaction commit?
> 3. What role does battery-backed DRAM and the Rio file cache play in ensuring durability against OS crashes versus power failures?

> [!success]- Answer
> **1. LRVM Design:**
> - **Redo-Only Log:** LRVM separates transaction atomicity and durability from page management. To maximize commit throughput, modifications to virtual memory are appended sequentially to an on-disk, append-only redo log.
> - **No-Flush on Commit:** Dirty virtual memory pages in RAM are *not* forced to disk during `set_range` or `commit`. Only the small log records (new value diffs) are synchronously flushed to the log. Background truncation periodically checkpoints log records back into the main VM backing file.
> - LRVM avoids undo logging by keeping old values in virtual memory during the transaction, simplifying recovery.
>
> **2. Rio Vista Zero-Flush Transactions:**
> - LRVM must synchronously write its redo log to physical disk on every commit, limiting commit rates to the disk's IOPS ceiling.
> - Rio Vista eliminates all disk writes during commit by running on top of the **Rio (RAM I/O)** system: physical DRAM backed by an uninterruptible power supply (UPS) or battery.
> - In Rio Vista, committing a transaction requires only writing the modification into battery-backed memory and updating an in-memory commit flag.
> - Commit latency drops from $\sim 10\text{ ms}$ (disk write) to $\sim 10\ \mu\text{s}$ (memory copy) - a $1000\times$ speedup.
>
> **3. Durability Against Crashes vs. Power Failure:**
> - **Power Failure:** The battery/UPS keeps physical DRAM refreshed during power loss, allowing a warm boot kernel to flush memory contents to disk.
> - **Operating System Crashes:** Rio protects memory against software kernel panics and runaway writes by write-protecting the physical pages in the processor MMU. Only designated transaction routines map the pages writable immediately prior to modification, preventing kernel memory corruption.

---

## 11. Giant-Scale Services: DQ Principle

> [!question] Question
> Fox and Brewer (1999) formulated the **DQ Principle** for giant-scale distributed services:
>
> 1. Define **Harvest ($H$)**, **Yield ($Y$)**, and the overall system capacity metric **Data per unit time ($DQ$)**.
> 2. A search engine cluster processes 10,000 queries per second under normal conditions, with each query searching across 100 partition nodes (full harvest $H = 1.0$) and achieving a yield of $Y = 0.999$ (99.9% successful query completion).
>    During a peak traffic surge or network partition, 20 out of the 100 database partition nodes become unreachable.
>    - Strategy A: The system drops queries that cannot access all 100 nodes.
>    - Strategy B: The system degrades gracefully, returning partial results from the 80 reachable nodes ($H = 0.80$) while answering all queries ($Y = 1.0$).
>    Calculate the overall data processed per second ($DQ$) for both strategies, and explain why giant-scale services prefer Strategy B.

> [!success]- Answer
> **1. Definitions:**
> - **Yield ($Y$):** The fraction of submitted requests completed successfully:
>   $$Y = \frac{\text{Completed Requests}}{\text{Submitted Requests}}$$
> - **Harvest ($H$):** The fraction of total data consulted to answer a completed request:
>   $$H = \frac{\text{Data Consulted}}{\text{Total Available Data}}$$
> - **DQ Metric:** Overall system capacity is proportional to the product of Data ($D$) and Queries ($Q$):
>   $$\text{Capacity} \propto H \times Y$$
>
> **2. Quantitative Calculation:**
> Baseline: 10,000 queries/sec across 100 partitions.
> - **Strategy A (Strict Consistency / Full Harvest):**
>   Because 20% of nodes are down, any query touching all partitions fails.
>   $$H = 1.0, \quad Y = 0.0 \implies DQ = 10,000 \times 1.0 \times 0.0 = 0$$
>   (If queries only hit subsets, yield drops to $Y \approx 0.0$).
> - **Strategy B (Graceful Degradation / Reduced Harvest):**
>   $$H = \frac{80}{100} = 0.80, \quad Y = 1.0 \implies DQ = 10,000 \times 0.80 \times 1.0 = 8000\text{ data units/sec}$$
>
> **Why Giant-Scale Services Prefer Strategy B:**
> A user searching for web results would rather see 80% of indexed results instantly than an HTTP 500 error page.
> Trading harvest for yield preserves service availability and user trust during large-scale network partitions or infrastructure failures.

---

## 12. Consistent Hashing and Quorums

> [!question] Question
> Amazon's **Dynamo** system (2007) is designed for non-stop availability using decentralized coordination:
>
> 1. How does **Consistent Hashing with Virtual Nodes** achieve uniform load balancing across heterogeneous physical hardware?
> 2. Explain how Dynamo configures **Sloppy Quorums** with parameters $(N, R, W)$ to satisfy the condition $R + W > N$. What happens during a network partition if $R + W \le N$?
> 3. How do **Vector Clocks** detect write conflicts during concurrent updates, and who resolves the conflict?
> 4. What is the role of **Merkle Trees** in Dynamo's background anti-entropy protocol?

> [!success]- Answer
> **1. Consistent Hashing with Virtual Nodes:**
> - Keys and physical nodes are hashed onto a circular 128-bit ring.
> - To avoid non-uniform hot spots and handle heterogeneous hardware capacities, each physical machine is assigned multiple **virtual nodes (tokens)** spread randomly around the ring.
> - A powerful server is assigned more virtual tokens, receiving proportionally more key ranges. When a physical machine fails, its load is evenly dispersed across many surviving machines rather than overloading its immediate clockwise neighbor.
>
> **2. Sloppy Quorums ($N, R, W$):**
> - $N$: Number of healthy nodes that must replicate each key.
> - $R$: Minimum number of nodes that must respond to a read request.
> - $W$: Minimum number of nodes that must acknowledge a write request.
> - If $R + W > N$, the read quorum and write quorum overlap on at least one node, guaranteeing that a read observes the latest write (strong consistency).
> - **Sloppy Quorum:** During partitions, writes are accepted by the first $N$ *healthy* reachable nodes on the ring (even if not the primary coordinators) using **Hinted Handoff**.
> - If $R + W \le N$, reads and writes can execute concurrently against disjoint sets of replicas, leading to stale reads and conflicting updates that must be reconciled later.
>
> **3. Vector Clocks and Conflict Resolution:**
> - Every write associates a vector clock with the key: $VC = \{(S_1, v_1), (S_2, v_2), \dots\}$.
> - If two concurrent updates occur during a network split, their vector clocks become causally incomparable (neither dominates the other).
> - Dynamo preserves both versions (siblings). When a client subsequently reads the key, Dynamo returns both conflicting values.
> - **Resolution:** The client application resolves the divergence (e.g., merging items in a shopping cart) and writes back the reconciled version.
>
> **4. Merkle Trees Anti-Entropy:**
> Replicas maintain hierarchical hash trees (Merkle trees) over their key ranges.
> To detect replica divergence in the background, two nodes compare the root hashes of their Merkle trees:
> - If root hashes match, the entire key range is identical (zero further traffic).
> - If root hashes differ, they traverse down the tree branches, transmitting and comparing only the specific subtrees that diverge.
> This minimizes network bandwidth required to synchronize out-of-date replicas.

---

## Final Revision Checklist

- [ ] Q1: Can compute exact border-crossing cycle costs for Monolithic, Microkernel, and SPIN.
- [ ] Q2: Can derive the idle memory tax penalty $k = \frac{1}{1-\tau}$ and effective share price in VMware ESX.
- [ ] Q3: Can calculate the 24-access worst-case latency of two-dimensional EPT page walks.
- [ ] Q4: Can explain why MCS requires $O(1)$ space per lock vs. Anderson's $O(P \times L)$ and how local spinning works.
- [ ] Q5: Can draw the communication butterfly for a 4-round Dissemination Barrier with $N=16$.
- [ ] Q6: Can detail Bershad's 4 pillars of LRPC (A-stack, handoff scheduling, early binding, argument caching).
- [ ] Q7: Can state Lamport's $IR_1, IR_2$ and explain why $C(a) < C(b) \centernot\implies a \to b$.
- [ ] Q8: Can explain why TreadMarks Lazy Release Consistency uses diffs and vector timestamps.
- [ ] Q9: Can contrast GMS page states (Local-Private, Local-Shared, Global-Private, Global-Shared).
- [ ] Q10: Can explain how Rio Vista achieves free transactions using battery-backed DRAM and protected virtual memory.
- [ ] Q11: Can apply the DQ principle ($H \times Y$) to graceful degradation scenarios.
- [ ] Q12: Can analyze Dynamo's consistent hashing ring, sloppy quorums ($N, R, W$), and Merkle trees.
