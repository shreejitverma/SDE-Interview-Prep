---
type: playbook
track: [sde]
level:
status: draft
last_reviewed:
tags: [gios, cs6200, midterm, exam-prep, operating-systems]
sources:
  - "Georgia Tech CS 6200 Midterm Practice Questions"
  - "Eykholt et al., Beyond Multiprocessing: Multithreading the SunOS Kernel (1992)"
  - "Birrell, An Introduction to Programming with Threads (1989)"
  - "Pai, Druschel, Zwaenepoel, Flash: An Efficient and Portable Web Server (USENIX 1999)"
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

## Revision Checklist

- [ ] Q1 - can explain why `exec` never creates a process
- [ ] Q2 - can state $t_{idle} > 2 \cdot t_{ctx\_switch}$ and why threads switch cheaper
- [ ] Q3 - can spot all 9 bugs without looking
- [ ] Q4 - can write the priority readers/writers code from a blank page
- [ ] Q5 - can list all 4 signal routing cases
- [ ] Q6 - can name 2 fields for each Solaris structure and explain swappable vs non-swappable
- [ ] Q7 - can derive 1/3/2 threads, 1.05 s, 95.2 req/s
- [ ] Q8 - can explain the `mincore` overhead (Flash < SPED) and the MP costs (Flash > MP), plus the >100 MB crossover
