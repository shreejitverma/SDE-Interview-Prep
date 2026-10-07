---
type: playbook
track: [sde]
level:
status: solid
last_reviewed:
tags: [gios, cs6200, midterm, exam-prep, operating-systems]
sources:
  - "Georgia Tech CS 6200 Midterm Practice Questions"
  - "Eykholt et al., Beyond Multiprocessing: Multithreading the SunOS Kernel (1992)"
  - "Birrell, An Introduction to Programming with Threads (1989)"
  - "Pai, Druschel, Zwaenepoel, Flash: An Efficient and Portable Web Server (USENIX 1999)"
  - "Fedorova et al., Performance of Multithreaded Chip Multiprocessors and Its Implications for Operating System Design (2007)"
  - "Stein & Shah, Implementing Lightweight Threads (USENIX 1992)"
---

# CS 6200 GIOS - Midterm Practice Questions

> Practice set for the midterm (P1L1 through P3L1 scope, plus assigned papers).
> Each question is stated exactly as given, followed by a folded answer callout.
> In Obsidian, keep answers collapsed and attempt each question first (active recall).
> On GitHub, the callouts render as plain blockquotes.

Back to [GIOS Dashboard](../_GIOS-Dashboard.md).

## Index

| # | Topic | Lecture / Source | Type |
|---|-------|------------------|------|
| [1](#1-process-creation) | Process Creation | [P2L1](../Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md#11-process-creation) | Multi-select |
| [2](#2-multi-threading-and-1-cpu) | Multithreading on 1 CPU | [P2L2](../Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md#4-why-threads-motivation-and-benefits) | Short answer |
| [3](#3-critical-section) | Producer/Consumer Bugs | [P2L2](../Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md#13-producerconsumer-problem-overview), [P2L3](../Part-2-Process-Thread-Management/P2L3-PThreads-Case-Study.md#11-spurious-wakeups-and-the-while-loop-pattern) | Code review |
| [4](#4-calendar-critical-section) | Priority Readers/Writers | [P2L2](../Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md#16-readerwriter-problem) | Pseudocode |
| [5](#5-signals) | Signals to ULTs | [P2L4](../Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md#8-signals-and-interrupts-in-multithreaded-systems) | Short answer |
| [6](#6-solaris-papers) | Solaris Thread Data Structures | [P2L4](../Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md#6-thread-management-data-structures), Eykholt paper | Recall |
| [7](#7-pipeline-model) | Pipeline Model Math | [P2L2](../Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md), [P2L5](../Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md) | Calculation |
| [8](#8-performance-observations) | Flash vs SPED vs MP | [P2L5](../Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md#10-event-driven-vs-multithreaded-architectures), Pai et al. paper | Graph analysis |
| [9](#9-cmp-and-hardware-multithreading-cpi-math) | Hardware Multithreading & CPI | [P3L1](../Part-3-Resource-Management/P3L1-Scheduling.md#7-cpi-and-cache-affinity), Fedorova et al. paper | Quantitative derivation |
| [10](#10-two-level-thread-multiplexing) | Two-Level Thread Multiplexing | [P2L4](../Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md#2-user-level-vs-kernel-level-threads), Stein & Shah paper | Architectural analysis |
| [11](#11-birrells-thread-synchronization-pitfalls) | Birrell Synchronization Pitfalls | [P2L2](../Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md#11-condition-variables), Birrell paper | Conceptual & diagnostic |
| [12](#12-cpu-scheduling-algorithms-and-quantum-selection) | CPU Scheduling & Timeslice Tuning | [P3L1](../Part-3-Resource-Management/P3L1-Scheduling.md#4-scheduling-algorithms-deep-dive) | Scheduling math |

---

## 1. Process Creation

> [!question] Question
> How is a new process created? Select all that apply.
>
> - [ ] Via fork
> - [ ] Via exec
> - [ ] Via fork followed by exec
> - [ ] Via exec followed by fork
> - [ ] Via exec or fork followed by exec
> - [ ] Via fork or fork followed by exec
> - [ ] None of the above
> - [ ] All of the above

> [!success]- Answer
> **Correct:**
>
> - Via fork
> - Via fork followed by exec
> - Via fork or fork followed by exec (the single most complete answer)
>
> **Why:**
>
> - `fork` is the only call here that creates a new process (new PID, new PCB).
>   The child is a copy of the parent and resumes at the instruction after `fork`.
> - `exec` does **not** create a process.
>   It replaces the program image (code, data, heap, stack) of the *calling* process; the PID is unchanged.
> - So a new process exists either as a pure copy (`fork` alone) or as a copy that then loads a new program (`fork` then `exec`).
> - Any option where `exec` alone creates a process ("Via exec", "Via exec followed by fork" as the creation path, "Via exec or ...") is wrong.
>
> **Trap:** "Via exec followed by fork" does produce a new process, but the creation is done by the `fork`; the `exec` merely changed the parent's image first.
> It is not *how* processes are created.

---

## 2. Multi-Threading and 1 CPU

> [!question] Question
> Is there a benefit of multithreading on 1 CPU?
>
> - Yes
> - No
>
> Give 1 reason to support your answer.

> [!success]- Answer
> **Yes.**
>
> **Reason - hiding I/O latency:** when one thread blocks (disk, network, page fault on a slow device), the CPU can context switch to another ready thread and keep doing useful work instead of idling.
>
> **The quantitative condition from lecture:**
>
> $$t_{idle} > 2 \cdot t_{ctx\_switch}$$
>
> Switching away and back costs two context switches; if the blocking time exceeds that, multithreading wins.
>
> **Why threads beat processes here:** threads share one address space, so a thread switch does not remap virtual-to-physical translations (no page table swap, TLB and caches stay warm).
> That makes $t_{ctx\_switch}$ small, so the condition holds for many more workloads.
>
> Other acceptable reasons: program structure/responsiveness (e.g. a UI thread stays responsive while a worker blocks), and lower memory footprint than the equivalent multi-process design.

---

## 3. Critical Section

> [!question] Question
> In the (pseudo) code segments for the producer code and consumer code, mark and explain all the lines where there are errors.
>
> **Global Section**
>
> ```c
> int in, out, buffer[BUFFERSIZE];
> mutex_t m;
> cond_var_t not_empty, not_full;
> ```
>
> **Producer Code**
>
> ```c
> 1.    while (more_to_produce) {
> 2.      mutex_lock(&m);
> 3.      if (out == (in + 1) % BUFFERSIZE)) // buffer full
> 4.         condition_wait(&not_full);
> 5.    add_item(buffer[in]); // add item
> 6.      in = (in + 1) % BUFFERSIZE
> 7.       cond_broadcast(&not_empty);
> 8.
> 9. } // end producer code
> ```
>
> **Consumer Code**
>
> ```c
> 1.    while (more_to_consume) {
> 2.    mutex_lock(&m);
> 3.    if (out == in) // buffer empty
> 4.      condition_wait(&not_empty);
> 5.    remove_item(out);
> 6.    out = (out + 1) % BUFFERSIZE;
> 7.    condition_signal(&not_empty);
> 8.
> 9. } // end consumer code
> ```

> [!success]- Answer
> **Producer errors**
>
> | Line | Error | Fix |
> |------|-------|-----|
> | 3 | `if` must be `while`. After waking, the thread re-acquires the mutex, but another producer may have filled the slot first (and spurious wakeups are allowed). The predicate must be re-checked. (Also an unbalanced extra `)`.) | `while (out == (in + 1) % BUFFERSIZE)` |
> | 4 | `condition_wait` must take the mutex so it can atomically release it while sleeping and re-acquire it on wakeup. Without it, the producer sleeps holding `m` and deadlocks every consumer. | `condition_wait(&not_full, &m);` |
> | 6 | Missing `;` (syntax). | `in = (in + 1) % BUFFERSIZE;` |
> | 7 | `broadcast` is wasteful: exactly one item was added, so only one consumer can make progress. Waking all of them causes a thundering herd that re-sleeps. | `condition_signal(&not_empty);` |
> | 8 | Missing `mutex_unlock(&m)`. The next loop iteration (or any consumer) blocks forever on line 2. | `mutex_unlock(&m);` |
>
> **Consumer errors**
>
> | Line | Error | Fix |
> |------|-------|-----|
> | 3 | `if` must be `while` (same reasoning as producer line 3). | `while (out == in)` |
> | 4 | `condition_wait` missing the mutex argument. | `condition_wait(&not_empty, &m);` |
> | 7 | Signals the **wrong** condition variable. The consumer just freed a slot, so it must wake a producer waiting on `not_full`. Signalling `not_empty` can leave producers asleep forever (lost wakeup to the wrong party). | `condition_signal(&not_full);` |
> | 8 | Missing `mutex_unlock(&m)`. | `mutex_unlock(&m);` |
>
> Minor: line 5 `remove_item(out)` is inconsistent with the producer's `add_item(buffer[in])`; it should be `remove_item(buffer[out])`.
>
> **Corrected code**
>
> ```c
> // Producer
> while (more_to_produce) {
>     mutex_lock(&m);
>     while (out == (in + 1) % BUFFERSIZE)      // buffer full
>         condition_wait(&not_full, &m);
>     add_item(buffer[in]);
>     in = (in + 1) % BUFFERSIZE;
>     condition_signal(&not_empty);
>     mutex_unlock(&m);
> }
>
> // Consumer
> while (more_to_consume) {
>     mutex_lock(&m);
>     while (out == in)                          // buffer empty
>         condition_wait(&not_empty, &m);
>     remove_item(buffer[out]);
>     out = (out + 1) % BUFFERSIZE;
>     condition_signal(&not_full);
>     mutex_unlock(&m);
> }
> ```
>
> **Note:** signal-then-unlock and unlock-then-signal are both correct.
> Unlock-then-signal avoids the woken thread immediately blocking on the still-held mutex (a "hurry up and wait"), at the cost of a tiny window where a third thread can sneak in, which the `while` loop already handles.
>
> **Pattern to memorize:** lock -> `while (!predicate) wait(cv, m)` -> mutate state -> signal the *other* side's CV -> unlock.

---

## 4. Calendar Critical Section

> [!question] Question
> A shared calendar supports three types of operations for reservations:
>
> - read
> - cancel
> - enter
>
> Requests for cancellations should have priority above reads, who in turn have priority over new updates.
>
> In pseudocode, write the critical section enter/exit code for the read operation.

> [!success]- Answer
> **Model:** this is readers/writers with priorities.
> `read` is a shared (reader) operation; `cancel` and `enter` both modify the calendar, so they are exclusive (writer) operations.
> Priority order: `cancel` > `read` > `enter`.
>
> **Shared state** (the standard GIOS `resource_counter` proxy-variable pattern):
>
> ```c
> mutex_t m;
> cond_var_t cancel_phase, read_phase, enter_phase;
> int resource_counter = 0;  // 0 = free, >0 = # active readers, -1 = one writer (cancel or enter) active
> int waiting_cancels  = 0;  // # cancel requests blocked
> int waiting_reads    = 0;  // # read requests blocked
> int waiting_enters   = 0;  // # enter requests blocked
> ```
>
> **Read: enter critical section**
>
> ```c
> mutex_lock(&m);
> // Block if a writer is active OR any cancel is waiting (cancels outrank reads).
> // Waiting enters do NOT block reads (reads outrank enters).
> while (resource_counter == -1 || waiting_cancels > 0) {
>     waiting_reads++;
>     condition_wait(&read_phase, &m);
>     waiting_reads--;
> }
> resource_counter++;            // one more active reader
> mutex_unlock(&m);
> ```
>
> **Read the calendar** (outside the mutex, so readers run concurrently)
>
> ```c
> // ... read calendar data ...
> ```
>
> **Read: exit critical section**
>
> ```c
> mutex_lock(&m);
> resource_counter--;
> if (resource_counter == 0) {   // last reader out hands off by priority
>     if (waiting_cancels > 0)
>         condition_signal(&cancel_phase);
>     else if (waiting_enters > 0)
>         condition_signal(&enter_phase);
> }
> mutex_unlock(&m);
> ```
>
> **Why it is correct:**
>
> - Readers proceed concurrently because `resource_counter` counts them; only the mutex around the bookkeeping is exclusive.
> - The `waiting_cancels > 0` check in the entry `while` gives cancels priority: once a cancel is queued, new readers stop entering, so the active readers drain and the last one signals `cancel_phase`.
> - The exit path checks cancels before enters, enforcing `cancel` > `enter`.
> - No reader is signalled on exit because a reader can only be waiting when a cancel is pending or a writer is active; the cancel/enter exit path handles waking readers.
> - `signal` (not `broadcast`) is used for writers because only one writer may run at a time.
>
> **Companion code (not required, but shows the protocol is consistent):**
>
> ```c
> // CANCEL enter
> mutex_lock(&m);
> while (resource_counter != 0) {
>     waiting_cancels++;
>     condition_wait(&cancel_phase, &m);
>     waiting_cancels--;
> }
> resource_counter = -1;
> mutex_unlock(&m);
> // ... cancel reservation ...
> // CANCEL / ENTER exit (shared logic)
> mutex_lock(&m);
> resource_counter = 0;
> if (waiting_cancels > 0)       condition_signal(&cancel_phase);
> else if (waiting_reads > 0)    condition_broadcast(&read_phase);   // all readers may enter together
> else if (waiting_enters > 0)   condition_signal(&enter_phase);
> mutex_unlock(&m);
>
> // ENTER enter: must also yield to any waiting cancels or reads
> mutex_lock(&m);
> while (resource_counter != 0 || waiting_cancels > 0 || waiting_reads > 0) {
>     waiting_enters++;
>     condition_wait(&enter_phase, &m);
>     waiting_enters--;
> }
> resource_counter = -1;
> mutex_unlock(&m);
> ```
>
> **Trade-off to mention:** strict priority can starve `enter` operations under a steady stream of reads/cancels.
> A production design would add aging or a bounded batch size.

---

## 5. Signals

> [!question] Question
> If the kernel cannot see user-level signal masks, then how is a signal delivered to a user-level thread (where the signal can be handled)?

> [!success]- Answer
> **Mechanism:** the user-level threading library installs its **own handler** for signals with the kernel.
> The kernel delivers the signal to a kernel-level thread (KLT) whose kernel mask has it enabled; that KLT runs the **library's** handler first.
> The library can see every user-level thread's (ULT's) mask, so it routes the signal:
>
> 1. **Currently running ULT has the signal unmasked:** the library invokes that ULT's handler directly.
> 2. **Running ULT has it masked, but another runnable ULT on the same KLT has it unmasked:** the library scheduler switches to that ULT and runs its handler there.
> 3. **The ULT with it unmasked is running on a different KLT:** the library sends a *directed signal* (e.g. `pthread_kill`/`tgkill`) to that KLT; when it arrives there, the library handler on that KLT delivers it to the target ULT (case 1).
> 4. **Every ULT has it masked:** the library makes a system call to update the **kernel-level** mask of that KLT to block the signal, then re-raises/re-enqueues the signal so the kernel tries another KLT.
>    Once all KLTs have it masked, it stays pending; when some ULT later unmasks it, the library makes a system call to re-enable it in the kernel mask, and the pending signal is delivered.
>
> **Key idea:** the library acts as a signal proxy that bridges the kernel's view (per-KLT masks) and the user-level view (per-ULT masks), lazily synchronizing kernel masks only when needed (the common case of updating ULT masks needs no system call).

---

## 6. Solaris Papers

> [!question] Question
> The implementation of Solaris threads described in the paper ["Beyond Multiprocessing: Multithreading the Sun OS Kernel"](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/references/ud923-eykholt-paper.pdf), describes four key data structures used by the OS to support threads.
> For each of these data structures, list at least two elements they must contain:
>
> - Process
> - LWP
> - Kernel-threads
> - CPU

> [!success]- Answer
> | Structure | Elements (any two) | Notes |
> |-----------|--------------------|-------|
> | **Process** | list of kernel-level threads (LWPs) in the process; pointer to the virtual address space (memory map); user credentials; signal handlers/dispositions; open file descriptors | Shared by all threads of the process. |
> | **LWP** | user-level registers; system call arguments; resource usage / accounting info; profiling info; signal mask; pointer to its kernel thread | Per-thread state needed only while the process is active, so it is **swappable** (not always in memory). |
> | **Kernel thread** | kernel-level registers; kernel stack pointer; scheduling info (priority, scheduling class); pointers to its LWP, process, and CPU; queue links (run queue / sleep queue) | Always needed by the kernel (e.g. for scheduling), so it is **not swappable**. |
> | **CPU** | pointer to the currently running kernel thread; list of kernel threads (dispatch/run queue); idle thread; interrupt handling info / interrupt stack | Per-processor; on SPARC a dedicated register points to the current thread for fast access. |
>
> **Relationships to remember:**
>
> - Process 1 : N LWPs, LWP 1 : 1 kernel thread; some kernel threads (daemons, interrupt threads) have no LWP.
> - The split between LWP (swappable) and kernel thread (non-swappable) exists to minimize pinned kernel memory: only what the kernel needs while the thread is not running stays resident.
> - Interrupts are handled as threads in this design, which lets interrupt handlers block on synchronization.

---

## 7. Pipeline Model

> [!question] Question
> An image web server has three stages with average execution times as follows:
>
> - Stage 1: read and parse request (10ms)
> - Stage 2: read and process image (30ms)
> - Stage 3: send image (20ms)
>
> You have been asked to build a multi-threaded implementation of this server using the pipeline model.
> Using a pipeline model, answer the following questions:
>
> 1. How many threads will you allocate to each pipeline stage?
> 2. What is the expected execution time for 100 requests (in sec)?
> 3. What is the average throughput of the system in Question 2 (in req/sec)? Assume there are infinite processing resources (CPU's, memory, etc.).

> [!success]- Answer
> **1. Threads per stage: 1, 3, 2**
>
> Balance the pipeline so every stage emits one result per 10 ms (the fastest stage's rate):
>
> | Stage | Time | Threads | Effective time per request |
> |-------|------|---------|----------------------------|
> | 1 - parse | 10 ms | 1 | 10 / 1 = 10 ms |
> | 2 - process image | 30 ms | 3 | 30 / 3 = 10 ms |
> | 3 - send | 20 ms | 2 | 20 / 2 = 10 ms |
>
> Threads per stage = stage time / shortest stage time.
>
> **2. Execution time for 100 requests: 1.05 s**
>
> - First request must traverse all stages: 10 + 30 + 20 = **60 ms** (pipeline fill latency).
> - After that, one request completes every **10 ms** (the balanced stage time).
>
> $$T = 60 + (100 - 1) \times 10 = 60 + 990 = 1050\ \text{ms} = 1.05\ \text{s}$$
>
> **3. Average throughput: ~95.2 req/s**
>
> $$\text{throughput} = \frac{100}{1.05\ \text{s}} \approx 95.24\ \text{req/s}$$
>
> (Steady-state throughput approaches 1 / 10 ms = 100 req/s as the number of requests grows, because the 60 ms fill cost is amortized.)
>
> **General formula** for a balanced pipeline with $N$ requests, total latency $L$, and bottleneck stage time $t_{stage}$:
>
> $$T(N) = L + (N - 1) \cdot t_{stage}$$

---

## 8. Performance Observations

> [!question] Question
> Here is a graph from the paper ["Flash: An Efficient and Portable Web Server"](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/references/ud923-pai-paper.pdf), that compares the performance of Flash with other web servers.
>
> **Graph: ECE Trace - FreeBSD** (bandwidth in Mb/s vs. data set size in MB, approximate readings)
>
> | Data set size | SPED | Flash | Zeus | MP | Apache |
> |---------------|------|-------|------|----|--------|
> | 15 MB | ~183 | ~178 | ~170 | ~140 | ~110 |
> | 60 MB | ~162 | ~148 | ~140 | ~110 | ~80 |
> | 90 MB | ~160 | ~150 | ~142 | ~110 | ~92 |
> | 105 MB | ~100 | ~125 | ~140 | ~75 | ~70 |
> | 120 MB | ~65 | ~95 | ~75 | ~52 | ~48 |
> | 150 MB | ~52 | ~82 | ~45 | ~35 | ~37 |
>
> Shape: below ~100 MB the order is SPED > Flash > Zeus > MP > Apache.
> Past ~100 MB the data set no longer fits in memory, SPED collapses, and Flash ends up highest.
>
> For data sets where the data set size is less than 100 MB why does...
>
> 1. Flash perform worse than SPED?
> 2. Flash perform better than MP?

> [!success]- Answer
> **Setting:** below ~100 MB the whole data set fits in the main-memory file cache, so (after warm-up) every request is a cache hit and **no disk I/O happens**.
> The workload is purely CPU/memory bound.
>
> **1. Why Flash is worse than SPED**
>
> - Flash is AMPED (Asymmetric Multi-Process Event-Driven): SPED plus helper processes for blocking disk I/O.
> - To decide whether to hand a request to a helper, Flash first checks if the file's pages are resident in memory, using `mincore()` on the memory-mapped file.
> - When everything is cached, that check **always** says "resident": it is pure overhead on every request and the helpers never get used.
> - SPED skips the check and just serves the file, so it does slightly less work per request and gets higher bandwidth.
> - Once the data no longer fits in memory (more than ~100 MB), SPED's single thread blocks on every disk read and its bandwidth crashes. Flash's residency check then pays off because only the helpers block. That is why the curves cross over.
>
> **2. Why Flash is better than MP**
>
> MP (Multi-Process, one request per process, Apache-style) pays several costs that Flash's single event-driven process avoids:
>
> - **Context switching:** many processes means frequent process context switches (address-space switch, TLB flush, cold caches). Flash's one event loop switches between connections with no kernel context switch.
> - **Memory footprint:** each MP process has its own address space and stack. This takes memory away from the file cache and puts more pressure on the CPU caches and TLB.
> - **No shared application caches:** Flash keeps one shared cache of pathname translations, response headers, and memory-mapped files. In MP each process has its own private copies, so hit rates are lower and the same work gets done over and over (or the processes need IPC/synchronization to share).
> - **Synchronization/coordination:** processes that share state (logging, the accept queue) need cross-process synchronization. A single event-driven process does not.
>
> **One-line summary:**
>
> - Flash < SPED: Flash does an extra memory-residency check (`mincore`) per request, and when everything is cached it buys nothing.
> - Flash > MP: Flash is a single event-driven process with shared caches, so it avoids MP's context switches, duplicated memory, and duplicated per-process caches.
>
> **Related optimizations in Flash worth knowing:** memory-mapped files plus `writev` gather writes (zero-copy-like), byte alignment to DMA boundaries, and caching of precomputed response headers and pathname lookups.
> Zeus is also SPED-based but lacks some of these optimizations, which is why it sits just below Flash in the cached range.

---

## 9. CMP and Hardware Multithreading CPI Math

> [!question] Question
> In the paper *"Performance of Multithreaded Chip Multiprocessors and Its Implications for Operating System Design"* (Fedorova et al.), the authors examine how memory stalls dominate execution time on Chip Multiprocessors (CMPs) and how hardware multithreading affects OS scheduling.
> 
> Consider a processor core with a base execution time $CPI_{base} = 1.0$ cycle per instruction (the instruction pipeline latency assuming all memory references hit in the cache).
> A software workload has the following memory characteristics:
> - 20% of all instructions executed are memory instructions (loads or stores).
> - 5% of all memory instructions miss the on-chip cache and must be fetched from main DRAM.
> - The memory access stall latency ($t_{mem\_stall}$) for an off-chip DRAM fetch is 200 cycles.
> 
> Answer the following questions:
> 1. Calculate the total Cycles Per Instruction ($CPI_{total}$) and the Instructions Per Cycle ($IPC$) for this thread executing alone on a single-threaded core.
> 2. What percentage of the processor core's total execution time is spent stalled waiting for memory?
> 3. If the core supports hardware multithreading with 4 hardware thread contexts (such as UltraSPARC T1 Niagara), why does co-scheduling 4 memory-intensive threads ("memory-bound mix") result in sub-linear speedup and memory bus contention, and what OS scheduling strategy does Fedorova et al. propose?

> [!success]- Answer
> **1. Quantitative CPI and IPC Derivation:**
> The total CPI equation from Fedorova et al. decomposes execution time into base computation cycles and memory stall cycles:
> 
> $$CPI_{total} = CPI_{base} + (\text{Memory Instructions per Instruction}) \times (\text{LLC Miss Rate}) \times (\text{Memory Stall Latency})$$
> 
> Substitute the given parameters:
> - $CPI_{base} = 1.0$
> - $\text{Memory Instructions per Instruction} = 0.20$
> - $\text{LLC Miss Rate} = 0.05$ (5%)
> - $\text{Memory Stall Latency} = 200$ cycles
> 
> Calculate memory stall penalty per instruction:
> $$\text{Stall cycles per instruction} = 0.20 \times 0.05 \times 200 = 0.01 \times 200 = 2.0\text{ cycles}$$
> 
> Calculate $CPI_{total}$:
> $$CPI_{total} = 1.0 + 2.0 = 3.0\text{ cycles per instruction}$$
> 
> Calculate $IPC$:
> $$IPC = \frac{1}{CPI_{total}} = \frac{1}{3.0} \approx 0.333\text{ instructions per cycle}$$
> 
> ---
> 
> **2. Percentage of Time Spent Stalled:**
> $$\text{Memory Stall Percentage} = \frac{\text{Memory Stall Cycles}}{CPI_{total}} = \frac{2.0}{3.0} = 66.67\%$$
> 
> The processor core spends two-thirds ($66.7\%$) of its total operational cycles completely stalled waiting for data to travel across the memory bus from DRAM, while actual instruction retirement occurs during only $33.3\%$ of cycles.
> 
> ---
> 
> **3. Multithreading Co-Scheduling and OS Strategy:**
> - **Why 4 Memory-Bound Threads Result in Sub-Linear Speedup:**
> Hardware multithreading (SMT or fine-grained multithreading) masks memory latency by interleaving instructions from other ready thread contexts when one thread stalls on a cache miss.
> However, if *all 4 threads* are memory-intensive, all 4 threads quickly miss the cache and stall on DRAM requests simultaneously.
> Once all 4 hardware contexts are stalled waiting on memory, the execution pipeline goes completely idle.
> Furthermore, co-scheduling 4 memory-bound threads saturates the shared Last-Level Cache (LLC) and memory controller bus, causing severe cache line evictions and increasing memory latency for all threads.
> - **Fedorova's Proposed OS Scheduling Solution:**
> The Operating System scheduler must be **cache-conscious** and **CPI-aware**.
> Rather than treating all threads uniformly, the scheduler inspects hardware performance counters (measuring hardware CPI or LLC miss rates).
> The OS scheduler pairs a high-CPI (memory-bound) thread with a low-CPI (compute-bound) thread on the same physical core.
> While the memory-bound thread stalls on an off-chip memory fetch, the compute-bound thread utilizes the execution pipeline without competing for memory bus bandwidth, maximizing aggregate CMP core utilization and avoiding memory saturation.

---

## 10. Two-Level Thread Multiplexing

> [!question] Question
> In the paper *"Implementing Lightweight Threads"* (Stein and Shah, USENIX 1992), the authors analyze the architecture of the two-level (many-to-many) threading model in SunOS / Solaris.
> 
> 1. What is the fundamental operational difference between an **unbound thread** and a **bound thread** in this architecture?
> 2. When an unbound user-level thread (ULT) executes a blocking system call (such as a synchronous `read()` from a disk file), what happens to the underlying Lightweight Process (LWP)?
> 3. How does the user-level thread library detect when an LWP has blocked, and how does it prevent runnable ULTs from starving?

> [!success]- Answer
> **1. Unbound vs. Bound Threads:**
> - **Unbound User-Level Thread:** The thread is multiplexed dynamically onto a pool of LWPs (kernel threads).
> The user-level thread library scheduler can switch an unbound thread from LWP 1 to LWP 2 across scheduling quanta.
> Unbound threads have negligible creation and context-switch costs because context switches happen entirely in user space without entering the kernel.
> - **Bound Thread:** The user-level thread is permanently bound to a dedicated LWP ($1:1$ mapping) for its entire lifetime.
> It never moves to another LWP.
> Bound threads are used for real-time threads, high-priority workloads, or threads that frequently execute blocking system calls where dedicated kernel execution context is required.
> 
> ---
> 
> **2. Behavior When an Unbound Thread Blocks in the Kernel:**
> When an unbound ULT invokes a blocking system call (such as `read()` on a block device or a socket with no data):
> - The calling thread transitions into kernel space via a trap.
> - The underlying **LWP blocks inside the kernel scheduler** waiting on the I/O event.
> - Because the LWP is sleeping in the kernel, the user-level thread library scheduler (which runs in user space on top of active LWPs) cannot run on that LWP.
> - The blocked LWP remains tied to the sleeping ULT until the I/O completes.
> 
> ---
> 
> **3. Preventing Starvation (LWP Pool Management):**
> If all LWPs in the pool block in the kernel on I/O, other runnable user-level threads in the application would starve even though the application has useful work to do and the physical CPU is idle.
> To prevent this, Stein & Shah implemented dynamic LWP pool adjustment:
> 1. **Signal Coordination (SIGWAITING):**
> When all LWPs belonging to a process block in the kernel, the Solaris kernel detects that the process has no active threads on CPU.
> The kernel sends a special signal, `SIGWAITING`, to the process.
> 2. **LWP Allocation:**
> The user-level thread library's `SIGWAITING` signal handler intercepts the signal.
> The library inspects its user-level runqueue.
> If there are runnable ULTs waiting to execute, the library issues a system call (`_lwp_create()`) to spawn a new LWP.
> 3. **Resuming Execution:**
> The newly created LWP enters the user-level scheduler, dequeues the waiting runnable ULTs, and executes them immediately on the CPU.
> When the original I/O completes and the blocked LWP wakes up, the system temporarily has an extra LWP; if the extra LWP remains idle past a timeout, the library reaps it to conserve kernel resources.

---

## 11. Birrell's Thread Synchronization Pitfalls

> [!question] Question
> In Andrew D. Birrell's landmark paper *"An Introduction to Programming with Threads"* (1989), several subtle concurrency hazards and design principles are highlighted.
> 
> 1. Explain the **Nested Monitor Lockout** problem.
> How does it differ from a classical circular deadlock?
> 2. Why do Mesa-style condition variables require checking the state condition in a `while` loop rather than an `if` statement?
> List the three distinct real-world causes of spurious or unexpected wakeups identified by Birrell.
> 3. What is the fundamental design difference between Birrell's thread **Alerts** (`AlertWait`) and standard UNIX asynchronous signals?

> [!success]- Answer
> **1. Nested Monitor Lockout:**
> - **The Problem:** A thread acquires outer Lock A (the outer monitor) and then calls an inner function that acquires inner Lock B.
> Inside the inner function, the thread evaluates a condition variable associated with Lock B and calls `wait(cv_B, lock_B)`.
> - **The Failure Mode:** The `wait` call atomically releases inner Lock B and puts the thread to sleep.
> However, **it does not release outer Lock A**.
> Another thread that needs to update the shared data and signal `cv_B` must first acquire Lock A to enter the outer subsystem.
> Because the sleeping thread retains Lock A, the signalling thread blocks forever on Lock A.
> Neither thread can proceed.
> - **Difference from Circular Deadlock:**
> In circular deadlock (Coffman conditions), Thread 1 holds A and waits for B, while Thread 2 holds B and waits for A.
> In nested monitor lockout, there is no circular dependency across two active lock holders; Thread 1 is sleeping on a condition variable while retaining an outer lock that prevents the signaller from ever entering to change the condition.
> 
> ---
> 
> **2. While Loop vs. If Statement (Three Causes of Spurious Wakeups):**
> In Mesa-style condition variables (used in POSIX PThreads, Java, and modern OSes), signalling a condition variable is merely a **hint** that the condition *might* be satisfied.
> The waiting thread is moved from the condition variable queue to the mutex ready queue, but it does not execute immediately with guaranteed exclusive access.
> A `while (!predicate)` loop is mandatory because of three distinct phenomena:
> 1. **Intervening Thread (Stolen Wakeup):** Between the moment Thread A is signalled and the moment Thread A actually wakes up and re-acquires the mutex, another thread (Thread C) can acquire the mutex, observe that the predicate is true, consume the resource, and release the mutex.
> When Thread A finally runs, the predicate is false again.
> 2. **Spurious Kernel Wakeup:** The operating system kernel may wake a sleeping thread due to internal signal interruptions, multiprocessor race conditions, or memory pressure without any thread having called `signal()`.
> 3. **Broadcast Imprecision (Thundering Herd):** When `broadcast()` is used, all waiting threads wake up.
> Only the first thread to acquire the lock will find the condition true; for all subsequent waking threads, the condition is false.
> 
> ---
> 
> **3. Birrell Alerts vs. UNIX Signals:**
> - **UNIX Asynchronous Signals:**
> Handled asynchronously.
> When a signal (`SIGINT`, `SIGTERM`) arrives, the OS kernel forcibly interrupts the thread at an arbitrary instruction pointer and executes the signal handler.
> This makes signal handlers notoriously dangerous (non-reentrant functions like `malloc()` or `printf()` cause deadlocks or heap corruption if interrupted).
> - **Birrell's Thread Alerts (`AlertWait`):**
> Handled **synchronously and cooperatively**.
> An alert does not interrupt a thread executing arbitrary code.
> Instead, calling `Alert(thread)` sets a boolean flag in the thread's TCB.
> The target thread only checks for alerts at explicit, well-defined cancellation points (such as `AlertWait()`, `TestAlert()`, or blocking I/O).
> If an alert is pending when the thread calls `AlertWait()`, the function returns immediately with an error/exception (`Alerted`), allowing the thread to clean up locks and invariants safely.

---

## 12. CPU Scheduling Algorithms and Quantum Selection

> [!question] Question
> Consider four processes arriving at time $t = 0$ with the following single CPU burst times:
> 
> | Process | Burst Time ($t_{burst}$) |
> | :--- | :--- |
> | $P_1$ | 8 ms |
> | $P_2$ | 4 ms |
> | $P_3$ | 9 ms |
> | $P_4$ | 5 ms |
> 
> Use the official CS 6200 performance evaluation formulas:
> - **Throughput Formula:** $\frac{\text{jobs\_completed}}{\text{time\_to\_complete\_all\_jobs}}$
> - **Avg. Completion Time Formula:** $\frac{\sum \text{times\_to\_complete\_each\_job}}{\text{jobs\_completed}}$
> - **Avg. Wait Time Formula:** $\frac{\sum t_i\text{\_wait\_time}}{\text{jobs\_completed}} = \frac{t_1\text{\_wait\_time} + t_2\text{\_wait\_time} + t_3\text{\_wait\_time} + t_4\text{\_wait\_time}}{\text{jobs\_completed}}$
> - **Makespan per Job:** $\frac{\text{time\_to\_complete\_all\_jobs}}{\text{jobs\_completed}}$
> 
> *(CS 6200 Rule: You do not have to include units in your answers. Also, for decimal answers, please round to the hundredths place).*
> 
> 1. Draw the execution timeline and calculate the turnaround time ($T_{turnaround} = T_{completion} - T_{arrival}$) and waiting time ($T_{wait} = T_{turnaround} - t_{burst}$) for each process under:
>    - First-Come, First-Served (FCFS) in process ID order ($P_1, P_2, P_3, P_4$).
>    - Shortest Job First (SJF, non-preemptive).
>    - Round Robin (RR) with a timeslice quantum $q = 3\text{ ms}$.
> 2. Calculate the Throughput, Average Completion Time, and Average Waiting Time for each algorithm.
> 3. In Round Robin scheduling, what are the architectural consequences of setting the quantum $q$ too small versus too large?
> State the general rule of thumb for quantum sizing in relation to CPU burst times.

> [!success]- Answer
> **1. Timelines and Metrics Calculation:**
> 
> #### Algorithm A: FCFS ($P_1 \rightarrow P_2 \rightarrow P_3 \rightarrow P_4$)
> - Timeline: $P_1$ runs [0, 8], $P_2$ runs [8, 12], $P_3$ runs [12, 21], $P_4$ runs [21, 26].
> - Completion times: $P_1 = 8$, $P_2 = 12$, $P_3 = 21$, $P_4 = 26$.
> - Waiting times ($T_{wait} = T_{completion} - t_{burst}$):
>   - $P_1 = 8 - 8 = 0\text{ ms}$
>   - $P_2 = 12 - 4 = 8\text{ ms}$
>   - $P_3 = 21 - 9 = 12\text{ ms}$
>   - $P_4 = 26 - 5 = 21\text{ ms}$
> - Turnaround times: $P_1 = 8$, $P_2 = 12$, $P_3 = 21$, $P_4 = 26\text{ ms}$.
> 
> #### Algorithm B: SJF (Shortest Job First: $P_2 [4] \rightarrow P_4 [5] \rightarrow P_1 [8] \rightarrow P_3 [9]$)
> - Timeline: $P_2$ runs [0, 4], $P_4$ runs [4, 9], $P_1$ runs [9, 17], $P_3$ runs [17, 26].
> - Completion times: $P_2 = 4$, $P_4 = 9$, $P_1 = 17$, $P_3 = 26$.
> - Waiting times:
>   - $P_2 = 4 - 4 = 0\text{ ms}$
>   - $P_4 = 9 - 5 = 4\text{ ms}$
>   - $P_1 = 17 - 8 = 9\text{ ms}$
>   - $P_3 = 26 - 9 = 17\text{ ms}$
> - Turnaround times: $P_2 = 4$, $P_4 = 9$, $P_1 = 17$, $P_3 = 26\text{ ms}$.
> 
> #### Algorithm C: Round Robin ($q = 3\text{ ms}$)
> Execution sequence across quanta:
> - Round 1:
>   - $P_1$ runs [0, 3] (remaining: 5)
>   - $P_2$ runs [3, 6] (remaining: 1)
>   - $P_3$ runs [6, 9] (remaining: 6)
>   - $P_4$ runs [9, 12] (remaining: 2)
> - Round 2:
>   - $P_1$ runs [12, 15] (remaining: 2)
>   - $P_2$ runs [15, 16] (remaining: 0, **$P_2$ completes at $t = 16$**)
>   - $P_3$ runs [16, 19] (remaining: 3)
>   - $P_4$ runs [19, 21] (remaining: 0, **$P_4$ completes at $t = 21$**)
> - Round 3:
>   - $P_1$ runs [21, 23] (remaining: 0, **$P_1$ completes at $t = 23$**)
>   - $P_3$ runs [23, 26] (remaining: 0, **$P_3$ completes at $t = 26$**)
> - Completion times: $P_1 = 23$, $P_2 = 16$, $P_3 = 26$, $P_4 = 21$.
> - Waiting times:
>   - $P_1 = 23 - 8 = 15\text{ ms}$
>   - $P_2 = 16 - 4 = 12\text{ ms}$
>   - $P_3 = 26 - 9 = 17\text{ ms}$
>   - $P_4 = 21 - 5 = 16\text{ ms}$
> - Turnaround times: $P_1 = 23$, $P_2 = 16$, $P_3 = 26$, $P_4 = 21\text{ ms}$.
> 
> ---
> 
> **2. Quantitative Metrics Comparison Table (Rounded to Hundredths):**
> 
> | Scheduling Algorithm | Throughput ($\frac{\text{jobs}}{\text{total\_time}}$) | Avg. Completion Time ($\frac{\sum \text{completion}}{\text{jobs}}$) | Avg. Wait Time ($\frac{\sum \text{wait}}{\text{jobs}}$) | Makespan per Job ($\frac{\text{total\_time}}{\text{jobs}}$) |
> | :--- | :--- | :--- | :--- | :--- |
> | **FCFS** | $\frac{4}{26} = \mathbf{0.15}$ | $\frac{8 + 12 + 21 + 26}{4} = \frac{67}{4} = \mathbf{16.75}$ | $\frac{0 + 8 + 12 + 21}{4} = \frac{41}{4} = \mathbf{10.25}$ | $\frac{26}{4} = \mathbf{6.50}$ |
> | **SJF** | $\frac{4}{26} = \mathbf{0.15}$ | $\frac{4 + 9 + 17 + 26}{4} = \frac{56}{4} = \mathbf{14.00}$ | $\frac{0 + 4 + 9 + 17}{4} = \frac{30}{4} = \mathbf{7.50}$ | $\frac{26}{4} = \mathbf{6.50}$ |
> | **Round Robin ($q=3$)** | $\frac{4}{26} = \mathbf{0.15}$ | $\frac{16 + 21 + 23 + 26}{4} = \frac{86}{4} = \mathbf{21.50}$ | $\frac{12 + 16 + 15 + 17}{4} = \frac{60}{4} = \mathbf{15.00}$ | $\frac{26}{4} = \mathbf{6.50}$ |
> 
> *Key Observations:*
> - **Throughput:** Identical across all non-idle algorithms ($0.15$), because total processing time ($26\text{ ms}$) without idle gaps depends only on the aggregate workload bursts.
> - **Average Completion & Wait Times:** SJF achieves the minimum completion time ($14.00$) and minimum wait time ($7.50$).
> Round Robin trades average completion latency ($21.50$) for interactive fairness and responsiveness.
> 
> ---
> 
> **3. Quantum Selection Tradeoffs and Rule of Thumb:**
> - **Quantum Too Small ($q \to 0$):**
> Context switch overhead ($t_{ctx\_switch}$) becomes a dominant fraction of CPU time.
> The CPU spends more time saving and restoring registers, invalidating TLBs, and thrashing processor caches than executing application logic.
> Processor throughput collapses.
> - **Quantum Too Large ($q \to \infty$):**
> Round Robin degenerates into FCFS.
> Short interactive processes get stuck behind long compute-bound batch jobs (the convoy effect), destroying system responsiveness.
> - **Rule of Thumb:**
> The quantum $q$ should be large relative to the context switch cost ($q \gg t_{ctx\_switch}$, typically $100\times$ larger, e.g., $10-100\text{ ms}$ vs $1-10\ \mu\text{s}$ context switch latency).
> Approximately **$80\%$ of CPU bursts** in the workload should be shorter than the timeslice $q$.
> This ensures that I/O-bound interactive processes finish their burst and yield the CPU before their quantum expires, while compute-bound processes are preempted to guarantee fairness.

---

## Revision Checklist

- [ ] Q1 - can explain why `exec` never creates a process
- [ ] Q2 - can state $t_{idle} > 2 \cdot t_{ctx\_switch}$ and why threads switch cheaper
- [ ] Q3 - can spot all 9 bugs without looking
- [ ] Q4 - can write the priority readers/writers code from a blank page
- [ ] Q5 - can list all 4 signal routing cases
- [ ] Q6 - can name 2 fields for each Solaris structure and explain swappable vs non-swappable
- [ ] Q7 - can derive 1/3/2 threads, 1.05 s, 95.2 req/s
- [ ] Q8 - can explain the `mincore` overhead (Flash < SPED) and the MP costs (Flash > MP), plus the >100 MB crossover
- [ ] Q9 - can calculate CPI and IPC with memory stall cycles ($CPI = CPI_{base} + \text{misses} \times \text{latency}$)
- [ ] Q10 - can explain Stein & Shah two-level thread multiplexing and `SIGWAITING` dynamic LWP allocation
- [ ] Q11 - can explain Birrell nested monitor lockout, the 3 causes of spurious wakeups, and AlertWait vs UNIX signals
- [ ] Q12 - can solve FCFS vs SJF vs Round Robin scheduling math and state the 80% burst rule of thumb

