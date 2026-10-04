---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P2L4"
  - "Linux Kernel Development, 3rd Ed., Robert Love"
  - "Windows Internals, 7th Ed., Russinovich"
---

# P2L4: Thread Design Considerations

> **Module goal:** Understand user-level vs. kernel-level threading, the three multithreading models (many-to-one, one-to-one, many-to-many), thread management data structures, signals/interrupts in multithreaded systems, and how Linux implements threads via `clone()` and NPTL.

## Table of Contents

- [1. Thread Management Models Overview](#1-thread-management-models-overview)
- [2. User-Level Threads (ULT)](#2-user-level-threads-ult)
- [3. Kernel-Level Threads (KLT)](#3-kernel-level-threads-klt)
- [4. ULT vs. KLT Tradeoffs](#4-ult-vs-klt-tradeoffs)
- [5. Multithreading Models](#5-multithreading-models)
- [6. Thread Management Data Structures](#6-thread-management-data-structures)
- [7. Thread-Local Storage (TLS)](#7-thread-local-storage-tls)
- [8. Signals and Interrupts in Multithreaded Systems](#8-signals-and-interrupts-in-multithreaded-systems)
- [9. Linux Threads: clone() and task_struct](#9-linux-threads-clone-and-task_struct)
- [10. Native POSIX Thread Library (NPTL)](#10-native-posix-thread-library-nptl)
- [11. Quizzes and Exercises](#11-quizzes-and-exercises)
- [12. Key Takeaways](#12-key-takeaways)
- [13. Futex: The Kernel Primitive Behind All Modern Synchronization](#13-futex-the-kernel-primitive-behind-all-modern-synchronization)
- [14. Modern Threading Models: Go, Java Virtual Threads, Rust](#14-modern-threading-models-go-java-virtual-threads-rust)
- [15. Kernel Thread Internals: task_struct Deep Dive](#15-kernel-thread-internals-task_struct-deep-dive)
- [16. Cross-Platform Event Multiplexing: Linux (epoll), macOS (kqueue), Windows (IOCP)](#16-cross-platform-event-multiplexing-linux-epoll-macos-kqueue-windows-iocp)

---

## 1. Thread Management Models Overview

The fundamental design question is: **who manages threads - the user-level library or the kernel?**

```
                  User-Level Threads (ULT)
Application  +----+----+----+
             | T1 | T2 | T3 |  <-- thread library manages these
             +----+----+----+
                    |
             [Single kernel thread]
                    |
             +------+------+
             |   Kernel    |
             +-------------+

                  Kernel-Level Threads (KLT)
Application  +----+----+----+
             | T1 | T2 | T3 |
             +----+----+----+
               |    |    |
               |    |    |       <-- kernel manages each thread
             +------+------+
             |   Kernel    |
             | KT1 KT2 KT3|
             +-------------+
```

---

## 2. User-Level Threads (ULT)

In the ULT model, threads are managed entirely by a **user-space thread library** (e.g., GNU Pth, green threads, goroutines).

The kernel sees only a single process (or a small number of kernel threads). The user-level library:
- Maintains its own TCBs
- Implements its own scheduler
- Performs context switching in user space (just saving/restoring registers)
- Handles thread creation and synchronization without syscalls

```
User-Level Thread Library
+--------------------------------------------------+
| Thread Table:                                     |
| +------+------+------+------+                     |
| | TCB0 | TCB1 | TCB2 | TCB3 |                    |
| |  PC  |  PC  |  PC  |  PC  |                    |
| |  SP  |  SP  |  SP  |  SP  |                    |
| | regs | regs | regs | regs |                    |
| +------+------+------+------+                     |
|                                                   |
| Scheduler: picks next ULT to run                  |
| Context switch: saves/restores registers          |
|   (setjmp/longjmp or manual assembly)             |
+--------------------------------------------------+
         |
    [One kernel thread]
         |
+--------------------------------------------------+
|                    Kernel                          |
|  Sees this as a single-threaded process           |
+--------------------------------------------------+
```

### Advantages

| Advantage | Why |
|-----------|-----|
| Fast context switch | No kernel trap; just user-space register swap (~100 ns vs ~1-5 us) |
| Portable | No kernel support needed; runs on any OS |
| Customizable scheduler | Application can tune scheduling policy |
| Lightweight | Minimal per-thread overhead (no kernel stack needed) |

### Disadvantages

| Disadvantage | Why |
|-------------|-----|
| No true parallelism | All ULTs map to one kernel thread; only one CPU core used |
| Blocking syscall blocks all | If one ULT calls `read()`, the kernel blocks the entire process |
| No kernel preemption | If one ULT enters an infinite loop, others are starved |
| Signal handling is complex | Kernel delivers signals to the process, not to individual ULTs |

### Examples

- Java green threads (pre-1.2)
- GNU Pth (Portable Threads)
- Go goroutines (hybrid; goroutines are ULTs scheduled onto kernel threads)
- Python asyncio (cooperative, single-threaded)
- Ruby fibers

---

## 3. Kernel-Level Threads (KLT)

In the KLT model, the kernel manages all threads directly. Each thread is a schedulable entity.

```
Kernel Thread Table (inside the kernel)
+--------------------------------------------------+
| task_struct[0]  task_struct[1]  task_struct[2]     |
|    PID=100         PID=100         PID=100         |
|    TID=100         TID=101         TID=102         |
|    mm=0xABC        mm=0xABC        mm=0xABC        |
|    (shared)        (shared)        (shared)        |
|    state=RUNNING   state=READY     state=WAITING   |
+--------------------------------------------------+
```

### Advantages

| Advantage | Why |
|-----------|-----|
| True parallelism | Kernel schedules threads on different cores |
| Blocking is fine | One thread blocking doesn't affect others |
| Preemptive | Kernel timer interrupts ensure fairness |
| Signal delivery | Kernel can target specific threads |

### Disadvantages

| Disadvantage | Why |
|-------------|-----|
| Slower context switch | Requires kernel trap + kernel stack switch |
| Higher creation cost | Kernel data structures (task_struct) must be allocated |
| Less portable | Depends on kernel thread support |
| Heavier per-thread overhead | Each thread needs a kernel stack (~8-16 KB) |

---

## 4. ULT vs. KLT Tradeoffs

| Factor | ULT | KLT |
|--------|-----|-----|
| Context switch time | ~100 ns | ~1-5 us |
| Creation time | ~1 us | ~10-30 us |
| Parallelism | No (one core) | Yes (multi-core) |
| Blocking I/O | Blocks all threads | Only blocking thread |
| Scalability | Millions of threads possible | Thousands (limited by kernel memory) |
| Scheduling flexibility | Full control | Kernel policy (CFS, etc.) |
| Best for | Cooperative I/O-heavy workloads | CPU-bound parallel workloads |

**Modern approach:** Hybrid (M:N) - user-level scheduler on top of a pool of kernel threads (goroutines, Erlang processes, Java virtual threads).

---

## 5. Multithreading Models

### Many-to-One

```
User Threads:  [T1] [T2] [T3] [T4]
                  \    |    |   /
                   \   |   |  /
               [Single Kernel Thread]
                        |
                   [One CPU Core]
```

- All ULTs map to one KLT
- No parallelism; blocking one blocks all
- Fast context switch
- Example: GNU Pth, Solaris green threads

### One-to-One

```
User Threads:  [T1] [T2] [T3] [T4]
                |    |    |    |
Kernel Threads: [K1] [K2] [K3] [K4]
                |    |    |    |
CPU Cores:     [C0] [C1] [C2] [C3]
```

- Each ULT maps to one KLT
- Full parallelism; blocking is fine
- Higher overhead per thread
- **Linux (NPTL)**, Windows, macOS all use this model

### Many-to-Many (Two-Level)

```
User Threads:  [T1] [T2] [T3] [T4] [T5] [T6]
                  \   |   /       \   |   /
                   \  |  /         \  |  /
Kernel Threads:    [K1] [K2]      [K3] [K4]
                    |    |          |    |
CPU Cores:         [C0] [C1]      [C2] [C3]

Many user threads are multiplexed onto fewer kernel threads.
The ULT scheduler decides which ULTs run on which KLTs.
```

- N user threads map to M kernel threads (N >= M)
- Parallelism (up to M cores)
- A blocking ULT causes the scheduler to run another ULT on that KLT
- Most flexible but most complex to implement
- Examples: Solaris LWP (historical), Go goroutines (GMP scheduler), Java Virtual Threads (Project Loom)

> **Quiz: Multithreading Models**
>
> *A user application has 100 threads on a system with 4 CPU cores. Which model allows the most parallelism?*
>
> **Answer:** **Many-to-many** or **one-to-one** with at least 4 kernel threads. One-to-one would create 100 kernel threads (wasteful; only 4 can run simultaneously). Many-to-many maps 100 ULTs onto ~4-8 KLTs, achieving the same parallelism with less overhead.

---

## 6. Thread Management Data Structures

### PCB vs. TCB Relationship

```
Process Control Block (PCB / task_struct)
+--------------------------------------------------+
| PID, PPID, UID                                    |
| Address space (mm_struct)        SHARED            |
| Open file descriptors (files)    SHARED            |
| Signal handlers                  SHARED            |
| Virtual memory areas (VMAs)      SHARED            |
+--------------------------------------------------+
         |
         |-- Thread Control Block (TCB / task_struct per thread)
         |   +--------------------------------------+
         |   | TID (unique per thread)              |
         |   | Program Counter (RIP)     PER-THREAD  |
         |   | Stack Pointer (RSP)       PER-THREAD  |
         |   | Registers                 PER-THREAD  |
         |   | Kernel stack              PER-THREAD  |
         |   | Scheduling info           PER-THREAD  |
         |   | Signal mask               PER-THREAD  |
         |   | Thread-local storage      PER-THREAD  |
         |   +--------------------------------------+
         |
         |-- Thread Control Block (TCB)
         |   +--------------------------------------+
         |   | TID, PC, SP, regs, ...               |
         |   +--------------------------------------+
         ...
```

### Linux: Unified task_struct

In Linux, there is **no separate PCB and TCB**. Both processes and threads use `task_struct`. What makes a thread different from a process is the **degree of sharing**:

```c
// Process creation (fork): creates new mm_struct, files, etc.
clone(SIGCHLD, 0);

// Thread creation: shares mm_struct, files, signal handlers, etc.
clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND |
      CLONE_THREAD | CLONE_SYSVSEM | CLONE_SETTLS |
      CLONE_PARENT_SETTID | CLONE_CHILD_CLEARTID,
      child_stack, ...);
```

**Inspect thread relationships:**
```bash
# See all threads of a process
ls /proc/$PID/task/
# Each directory is a TID

# Compare PID vs TID
ps -eLf | grep $PID
# UID  PID  PPID  LWP  ... CMD
# LWP (Light Weight Process) = TID
# For the main thread, LWP == PID

# Thread group info
cat /proc/$PID/status | grep -E "^(Pid|Tgid|Threads)"
# Tgid = Thread Group ID = PID of the process
# Pid  = TID of this specific thread
# Threads = number of threads in the group
```

---

## 7. Thread-Local Storage (TLS)

TLS provides per-thread global variables - each thread gets its own copy.

### PThreads TLS API

```c
#include <stdio.h>
#include <pthread.h>

pthread_key_t key;

void destructor(void *value) {
    printf("Destroying TLS value: %d\n", *(int*)value);
    free(value);
}

void *worker(void *arg) {
    int *my_data = malloc(sizeof(int));
    *my_data = (intptr_t)arg * 100;
    pthread_setspecific(key, my_data);

    // Later, in any function called by this thread:
    int *data = pthread_getspecific(key);
    printf("Thread %ld: TLS value = %d\n", (intptr_t)arg, *data);

    return NULL;  // destructor called automatically
}

int main(void) {
    pthread_key_create(&key, destructor);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, worker, (void*)1);
    pthread_create(&t2, NULL, worker, (void*)2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pthread_key_delete(key);
    return 0;
}
// Output:
// Thread 1: TLS value = 100
// Thread 2: TLS value = 200
// Each thread has its own copy of the data
```

### GCC/Clang `__thread` (Simpler)

```c
__thread int tls_counter = 0;  // Each thread gets its own copy

void *worker(void *arg) {
    tls_counter = (intptr_t)arg;  // modifies only this thread's copy
    printf("Thread %ld: counter = %d\n", (intptr_t)arg, tls_counter);
    return NULL;
}
```

### C11 `_Thread_local`

```c
_Thread_local int tls_counter = 0;  // Standard C11
// or with the macro:
#include <threads.h>
thread_local int tls_counter = 0;
```

### Windows TLS

```c
// Dynamic TLS
DWORD tls_index = TlsAlloc();
TlsSetValue(tls_index, my_data);
void *data = TlsGetValue(tls_index);
TlsFree(tls_index);

// Static TLS (simpler)
__declspec(thread) int tls_counter = 0;
```

---

## 8. Signals and Interrupts in Multithreaded Systems

### Signals vs. Interrupts

| | Interrupts | Signals |
|--|-----------|---------|
| Source | Hardware (timer, disk, NIC) | Software (kill, SIGALRM, SIGSEGV) |
| Destination | CPU core | Process or specific thread |
| Handler runs in | Kernel mode | User mode (signal handler) |
| Timing | Asynchronous (any time) | Sync (SIGSEGV) or async (SIGTERM) |

### Signal Delivery in Multithreaded Programs

```
Kernel sends signal to process (thread group):
  1. Synchronous signals (SIGSEGV, SIGFPE, SIGBUS):
     Delivered to the thread that caused the fault

  2. Asynchronous signals (SIGTERM, SIGINT, SIGUSR1):
     Delivered to ANY thread that has not blocked the signal
     (kernel picks one - typically the main thread or
      the first thread with an unblocked mask)

  3. Directed signals (pthread_kill, tgkill):
     Delivered to a specific thread
```

### Signal Masks

Each thread has its own **signal mask** - a bitmask of blocked signals:

```c
#include <signal.h>
#include <pthread.h>

void *worker(void *arg) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    // Block SIGUSR1 in this thread only
    pthread_sigmask(SIG_BLOCK, &set, NULL);

    // This thread will NOT receive SIGUSR1
    // Other threads in the process still can
    // ...
    return NULL;
}
```

### Dedicated Signal-Handling Thread Pattern

```c
// Best practice: block all signals in worker threads;
// have one dedicated thread handle signals

sigset_t set;
sigfillset(&set);
pthread_sigmask(SIG_BLOCK, &set, NULL);  // Block all in main thread
// (inherited by all threads created after this)

// Create worker threads (they inherit the blocked mask)
// ...

// Create signal-handling thread
void *signal_handler_thread(void *arg) {
    sigset_t wait_set;
    sigemptyset(&wait_set);
    sigaddset(&wait_set, SIGINT);
    sigaddset(&wait_set, SIGTERM);

    while (1) {
        int sig;
        sigwait(&wait_set, &sig);  // Synchronously wait for signal
        printf("Received signal %d\n", sig);
        if (sig == SIGTERM) break;
    }
    return NULL;
}
```

> **Quiz: Signals**
>
> *In a process with 5 threads, thread 3 executes an illegal memory access. Which thread receives SIGSEGV?*
>
> **Answer:** **Thread 3** - SIGSEGV is a synchronous signal caused by a specific instruction in a specific thread. The kernel delivers it to the faulting thread.

---

## 9. Linux Threads: clone() and task_struct

### The clone() System Call

`clone()` is Linux's unified system call for creating both processes and threads:

```c
int clone(
    int (*fn)(void *),        // function to execute
    void *child_stack,        // stack for new task
    int flags,                // what to share
    void *arg,                // argument to fn
    /* pid_t *ptid,           // parent TID storage */
    /* struct user_desc *tls, // TLS descriptor */
    /* pid_t *ctid */         // child TID storage
);
```

### clone() Flags - What Gets Shared

| Flag | Effect |
|------|--------|
| `CLONE_VM` | Share virtual memory (address space) |
| `CLONE_FS` | Share filesystem info (cwd, root, umask) |
| `CLONE_FILES` | Share open file descriptors |
| `CLONE_SIGHAND` | Share signal handlers |
| `CLONE_THREAD` | Same thread group (same PID from userspace; different TID) |
| `CLONE_SYSVSEM` | Share System V semaphore undo values |
| `CLONE_SETTLS` | Set Thread-Local Storage for the new thread |
| `CLONE_PARENT_SETTID` | Write new TID to parent's memory |
| `CLONE_CHILD_CLEARTID` | Clear TID in child's memory on exit (futex wake) |
| `CLONE_NEWNS` | New mount namespace (containers) |
| `CLONE_NEWPID` | New PID namespace (containers) |
| `CLONE_NEWNET` | New network namespace (containers) |

**Process vs. Thread creation:**
```
fork()  = clone(SIGCHLD)
          -> New mm_struct, files_struct, signal_struct
          -> Different PID, different TGID

thread  = clone(CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD | ...)
          -> Shares mm_struct, files_struct, signal_struct
          -> Different TID, same TGID (=PID)
```

### Linux task_struct (Simplified Key Fields)

```c
struct task_struct {
    volatile long state;            // TASK_RUNNING, TASK_INTERRUPTIBLE, ...
    pid_t pid;                      // unique task ID (TID for threads)
    pid_t tgid;                     // thread group ID (= PID of main thread)

    struct task_struct *parent;     // parent process/thread
    struct list_head children;      // child list
    struct list_head thread_group;  // other threads in this group

    struct mm_struct *mm;           // memory descriptor (shared for threads)
    struct files_struct *files;     // open file descriptors (shared for threads)
    struct signal_struct *signal;   // signal handlers (shared for threads)
    struct sighand_struct *sighand; // signal handler table

    struct thread_struct thread;    // CPU-specific state (registers, FPU)
    void *stack;                    // kernel stack

    // Scheduling
    int prio, static_prio, normal_prio;
    unsigned int policy;            // SCHED_NORMAL, SCHED_FIFO, etc.
    struct sched_entity se;         // CFS scheduling entity
    cpumask_t cpus_allowed;         // CPU affinity mask

    // Credentials
    const struct cred *cred;        // UID, GID, capabilities
};
```

```bash
# See task_struct fields for current process
cat /proc/self/status

# Key fields visible via /proc:
# Name, State, Tgid, Pid, PPid, TracerPid, Uid, Gid
# VmPeak, VmSize, VmRSS, Threads
# Cpus_allowed, voluntary_ctxt_switches, nonvoluntary_ctxt_switches
```

---

## 10. Native POSIX Thread Library (NPTL)

NPTL is the modern Linux PThreads implementation (since kernel 2.6 / glibc 2.3.2).

### Key Design Decisions

| Aspect | NPTL Choice | Rationale |
|--------|-------------|-----------|
| Threading model | **1:1** (one ULT = one KLT) | Leverages kernel scheduling, true parallelism |
| System call | `clone()` with thread flags | Shares address space, files, signals |
| Synchronization | **Futex** (Fast Userspace Mutex) | Uncontended case is pure user-space; kernel involved only on contention |
| Signal handling | Per-thread signal masks via `rt_sigprocmask` | POSIX-compliant signal delivery |
| TLS | Segment register (`%fs` on x86-64) points to TCB | Fast access without syscall |

### Futex - The Key Innovation

```
Futex (Fast Userspace Mutex) operation:

Uncontended lock (fast path - user space only):
  1. Atomic CAS: 0 -> 1 (unlocked -> locked)
  2. Success! No syscall needed.
  Cost: ~25 ns

Contended lock (slow path - involves kernel):
  1. Atomic CAS: 0 -> 1 fails (already locked)
  2. Set futex value to 2 (locked + waiters)
  3. futex(FUTEX_WAIT, ...) syscall - kernel puts thread to sleep
  Cost: ~1-5 us

Unlock:
  1. Atomic exchange: value -> 0
  2. If old value was 2 (had waiters):
     futex(FUTEX_WAKE, ...) syscall - kernel wakes one waiter
  Cost: ~25 ns (no waiters) or ~1 us (with wake)
```

```bash
# Check NPTL version
getconf GNU_LIBPTHREAD_VERSION
# Output: NPTL 2.35

# Trace futex calls
strace -e futex ./my_threaded_program 2>&1 | head -20
# futex(0x7f..., FUTEX_WAIT_PRIVATE, 2, NULL) = 0
# futex(0x7f..., FUTEX_WAKE_PRIVATE, 1) = 1
```

### Before NPTL: LinuxThreads (Historical)

| Aspect | LinuxThreads (old) | NPTL (current) |
|--------|-------------------|----------------|
| Thread = process | Each thread had different PID | Threads share TGID |
| Signal delivery | Broken (signals to wrong thread) | POSIX-compliant |
| Manager thread | Required a hidden manager thread | No manager thread |
| Performance | High overhead | Low overhead (futex) |
| Max threads | ~thousands | ~millions (limited by memory) |

---

## 11. Quizzes and Exercises

> [!question] Quiz 1: Thread Structures and State Separation (Clips 153-154)
> In a multithreaded process, identify whether each of the following components belongs to the **shared process-wide state** or the **private per-thread state**:
> 1. Virtual address space (page tables, code segment, data segment, heap).
> 2. CPU register set (instruction pointer, general-purpose registers, flags).
> 3. Open file descriptor table and network sockets.
> 4. Execution call stack (local function variables and activation frames).
> 5. Signal handlers (actions associated with `SIGINT`, `SIGTERM`, etc.).
> 6. Signal mask (which signals are currently blocked from delivery).

> [!success]- Answer
> - **Shared Process-Wide State:**
>   - **1 (Virtual Address Space):** All threads share the identical memory map and heap.
>   - **3 (File Descriptor Table):** File descriptors opened by one thread are accessible to all threads.
>   - **5 (Signal Handlers):** Installed signal dispositions (`sigaction`) apply globally across the entire process.
> - **Private Per-Thread State:**
>   - **2 (CPU Registers):** Each thread represents an independent execution stream with its own `%rip`, `%rsp`, and general-purpose register state.
>   - **4 (Execution Call Stack):** Each thread allocates a distinct stack region in virtual memory to track function call frames.
>   - **6 (Signal Mask):** Each thread maintains an individual signal mask (`pthread_sigmask`), allowing individual threads to block or unblock specific signals independently.

> [!question] Quiz 2: Thread Concurrency & Kernel Visibility (Clips 158-159)
> In Solaris and early UNIX systems implementing an M:N threading model, the programmer could call `pthread_setconcurrency(int level)`.
> 1. What was the purpose of informing the runtime of the desired concurrency level?
> 2. Why does `pthread_setconcurrency()` have no effect on modern Linux (NPTL) and macOS?

> [!success]- Answer
> 1. **Purpose in M:N Models:** In an M:N model, user-level threads (ULTs) are multiplexed onto a pool of kernel-level threads (Lightweight Processes - LWPs). If the user application has 50 ULTs but only 1 LWP allocated by the kernel, all 50 ULTs serialize on a single core. Calling `pthread_setconcurrency(k)` provided an advisory hint to the user-level thread library to request $k$ underlying LWPs from the kernel so that $k$ threads could execute simultaneously on multiple physical cores.
> 2. **Irrelevance in Modern 1:1 Models:** Both Linux (NPTL) and modern macOS implement a strict **1:1 threading model** where every user-level thread is created with a corresponding kernel-level thread (`task_struct` on Linux, Mach thread on Darwin). The kernel scheduler is already aware of every individual thread, rendering user-level concurrency hints completely redundant.

> [!question] Quiz 3: Optimal Thread Count Calculation (Clips 163-164)
> An engineer is designing an image-processing microservice deployed on an 8-core server.
> Profiling reveals that each request takes 100 ms total: 20 ms of CPU compute (decoding and filtering) and 80 ms of I/O wait (reading from disk and network transfer).
> What is the mathematically optimal number of worker threads to maximize server throughput without causing excessive thrashing?

> [!success]- Answer
> Using Little's Law and the CPU wait-to-compute ratio:
> $$N_{\text{threads}} = N_{\text{cores}} \times \left(1 + \frac{\text{Wait Time}}{\text{Compute Time}}\right)$$
> Substituting the measured values:
> $$N_{\text{threads}} = 8 \times \left(1 + \frac{80\text{ ms}}{20\text{ ms}}\right) = 8 \times (1 + 4) = 40\text{ threads}$$
> Sizing the worker pool to 40 threads ensures that while 32 threads are waiting on disk/network I/O, exactly 8 threads are ready to saturate the 8 physical CPU cores, achieving near 100% compute hardware utilization.

> [!question] Quiz 4: Signal Delivery in Multithreaded Programs (Clips 173-174)
> Consider a process with 4 active threads. A `SIGSEGV` signal is generated, and a `SIGINT` signal arrives from the terminal.
> 1. How is a **synchronous signal** (e.g., `SIGSEGV`, `SIGFPE`) handled across threads?
> 2. How is an **asynchronous signal** (e.g., `SIGINT`, `SIGTERM`) delivered across threads?
> 3. What is the recommended production pattern for handling asynchronous signals in multithreaded servers?

> [!success]- Answer
> 1. **Synchronous Signals:** Generated by an illegal CPU instruction executed by a specific thread (e.g., invalid memory dereference, division by zero). The signal is directed **strictly to the specific thread that caused the hardware fault**.
> 2. **Asynchronous Signals:** Sent externally from another process (`kill()`) or terminal interrupt. The kernel delivers the signal to **any arbitrary thread** in the process that does not have the signal blocked in its `pthread_sigmask`.
> 3. **Production Dedicated Signal Thread Pattern:**
>    - At process startup, block all asynchronous signals in the main thread using `pthread_sigmask(SIG_BLOCK, &set, NULL)`. Because child threads inherit the parent's signal mask, all worker threads start with signals blocked.
>    - Spawn a dedicated signal-handling thread that synchronously waits for signals using `sigwait(&set, &sig)`. This thread cleanly executes shutdown logic without interrupting worker thread execution or corrupting mutex states.

> [!question] Quiz 5: Interrupt Handling: Top-Half versus Bottom-Half (Clip 176)
> Operating system kernels divide hardware interrupt handling into a Top Half and a Bottom Half.
> Explain the distinct roles of each half, why the division is necessary, and what mechanisms Linux and Windows provide for bottom-half processing.

> [!success]- Answer
> 1. **Top Half (Hard IRQ):**
>    - *Role:* Executes immediately when the hardware interrupt fires, running with interrupts disabled (or masked). Performs only the minimal critical work: acknowledges the hardware device, resets interrupt lines, and copies raw packet/data pointers to a kernel queue.
>    - *Constraint:* Must complete in sub-microsecond time to avoid dropping subsequent hardware interrupts.
> 2. **Bottom Half (Deferred Execution):**
>    - *Role:* Performs the heavy, time-consuming processing (e.g., TCP/IP protocol checksumming, packet routing, file buffer cache updates) with interrupts re-enabled.
> 3. **Operating System Implementations:**
>    - **Linux:** Softirqs (for high-throughput networking), Tasklets (dynamically allocated deferred functions), and Workqueues / Threaded IRQs (executed by dedicated kernel threads with sleep capability).
>    - **Windows:** Deferred Procedure Calls (DPCs) queued at `DISPATCH_LEVEL`, running before returning to user space.

---

### Exercise: Inspecting Threads on Linux

```bash
# Run a multi-threaded program in the background
python3 -c "
import threading, time
def work():
    time.sleep(60)
for i in range(4):
    t = threading.Thread(target=work)
    t.start()
time.sleep(60)
" &

PID=$!

# Inspect thread info
echo "=== Process Status ==="
cat /proc/$PID/status | grep -E "^(Name|State|Tgid|Pid|Threads)"

echo "=== Individual Threads ==="
for tid in $(ls /proc/$PID/task/); do
    echo "TID=$tid State=$(cat /proc/$PID/task/$tid/status | grep State)"
done

echo "=== Thread View (ps) ==="
ps -T -p $PID

kill $PID
```

---

## 12. Key Takeaways

1. **User-level threads** are fast to create/switch but cannot achieve true parallelism and suffer from blocking syscalls.
2. **Kernel-level threads** provide true parallelism and proper blocking but are heavier.
3. **One-to-one** is the most common model today (Linux NPTL, Windows, macOS); **many-to-many** is used by Go and Java Virtual Threads for massive concurrency.
4. Linux uses `clone()` with different flag combinations to create either processes or threads - both are `task_struct` internally.
5. **NPTL** is the modern Linux threading library; it uses **futexes** for fast synchronization (no syscall in the uncontended case).
6. **TLS** gives each thread its own copy of a global variable without explicit passing.
7. In multithreaded programs, use a **dedicated signal-handling thread** pattern for reliable signal handling.

---

## 13. Futex: The Kernel Primitive Behind All Modern Synchronization

Futex (Fast Userspace muTEX) is the Linux mechanism that makes pthreads, Go channels, and Java monitors fast.

```
Futex Design: Avoid syscall in the common (uncontended) case

FAST PATH (uncontended - no kernel involvement):
  Thread A: atomic CAS on userspace int → succeeds → locked
  Thread B: CAS → fails → calls futex(FUTEX_WAIT) → kernel blocks B

SLOW PATH (contended - kernel involved):
  Thread A: calls futex(FUTEX_WAKE) → kernel wakes B
  Thread B: retries CAS → succeeds

Cost:
  Uncontended lock:    ~5 ns (just an atomic CAS)
  Contended lock:    ~500 ns (2 syscalls: FUTEX_WAIT + FUTEX_WAKE)
  pthread_mutex:       ~8 ns uncontended (futex + overhead)
```

```c
/* futex syscall directly (glibc wraps this internally) */
#include <linux/futex.h>
#include <sys/syscall.h>
#include <stdatomic.h>

static atomic_int futex_var = 0;  /* 0=unlocked, 1=locked, 2=locked+waiters */

static long futex(atomic_int *uaddr, int futex_op, int val) {
    return syscall(SYS_futex, uaddr, futex_op, val, NULL, NULL, 0);
}

void futex_lock(atomic_int *f) {
    int c;
    /* Fast path: CAS 0 → 1 (uncontended) */
    c = 0;
    if (atomic_compare_exchange_strong(f, &c, 1))
        return;

    /* Slow path: set to 2 (locked + waiters), then sleep */
    do {
        if (c == 2 || atomic_compare_exchange_strong(f, &c, 2)) {
            futex(f, FUTEX_WAIT, 2);  /* Sleep until woken */
        }
        c = 0;
    } while (!atomic_compare_exchange_strong(f, &c, 2));
}

void futex_unlock(atomic_int *f) {
    if (atomic_fetch_sub(f, 1) != 1) {
        /* Was 2 (waiters present): wake one */
        atomic_store(f, 0);
        futex(f, FUTEX_WAKE, 1);  /* Wake 1 waiter */
    }
}
```

```bash
# Watch futex usage with perf
perf stat -e syscalls:sys_enter_futex ./your_threaded_program

# Detailed futex tracing
strace -e futex ./your_program 2>&1 | grep -E "FUTEX_(WAIT|WAKE)" | head -20

# futex contention heatmap (BPF)
sudo bpftrace -e '
    tracepoint:syscalls:sys_enter_futex
    /args->op == 0/ {          /* FUTEX_WAIT */
        @waiting[comm] = count();
    }
    interval:s:5 {
        print(@waiting);
        clear(@waiting);
    }'

# pstack / gdb to see all thread stacks
gdb -batch -ex "thread apply all bt" ./program core

# See futex queue waiters per address
cat /proc/<pid>/fdinfo/<fd>
```

---

## 14. Modern Threading Models: Go, Java Virtual Threads, Rust

### Go Goroutines (M:N Many-to-Many)

Go implements M:N threading at language level, with its own scheduler (not OS):

```
Go Runtime Scheduler (GMP model):

G (Goroutine)  = lightweight coroutine (~2KB stack, grows as needed)
M (Machine)    = OS thread (typically GOMAXPROCS count)
P (Processor)  = scheduling context (run queue + local state)

     G  G  G                 G  G  G
     |  |  |                 |  |  |
  +-------+               +-------+
  |   P   | ←→ M(OS thread)|   P   | ←→ M(OS thread)
  +-------+               +-------+
       \                      /
        \                    /
         +------------------+
         |   Global Run Q   |
         +------------------+
```

```go
package main

import (
    "fmt"
    "runtime"
    "sync"
    "time"
)

func main() {
    // Set number of OS threads = number of CPU cores
    runtime.GOMAXPROCS(runtime.NumCPU())

    var wg sync.WaitGroup

    // Spawn 100,000 goroutines - each only ~2KB stack
    for i := 0; i < 100_000; i++ {
        wg.Add(1)
        go func(id int) {
            defer wg.Done()
            time.Sleep(time.Millisecond)  // I/O wait: goroutine parked, M reused
            fmt.Printf("goroutine %d done\n", id)
        }(i)
    }
    wg.Wait()
}
```

```bash
# Profile goroutine scheduling
go tool trace trace.out       # Visual trace of goroutine lifecycle
GOMAXPROCS=1 go test -bench=. # Force single-threaded for comparison
go test -race ./...           # Race detector (uses TSan)

# Runtime statistics
import "runtime"
var stats runtime.MemStats
runtime.ReadMemStats(&stats)
fmt.Printf("Goroutines: %d\n", runtime.NumGoroutine())

# pprof: goroutine dump
import _ "net/http/pprof"
go http.ListenAndServe(":6060", nil)
# Then: curl http://localhost:6060/debug/pprof/goroutine?debug=1
```

**Go scheduler features:**
- **Work stealing**: idle P steals goroutines from busy P's run queues
- **Preemption**: goroutines preempted at function call sites (Go 1.14+: signals)
- **Syscall handling**: M parks itself, P attaches to new M while old M blocks in syscall
- **Stack growth**: stacks start at 2KB, grow to 1GB max via copying

### Java Virtual Threads (Project Loom, Java 21+)

Virtual threads are Java's M:N threading model, replacing the old thread-per-request model:

```java
import java.util.concurrent.*;

// Old model: platform thread = OS thread (expensive, ~1MB stack)
Thread platform = new Thread(() -> System.out.println("platform thread"));

// New model: virtual thread = JVM-managed coroutine (~200 bytes per thread)
Thread virtual = Thread.ofVirtual().start(() -> System.out.println("virtual thread"));

// Spawn 1,000,000 virtual threads (impossible with platform threads!)
try (var executor = Executors.newVirtualThreadPerTaskExecutor()) {
    IntStream.range(0, 1_000_000).forEach(i ->
        executor.submit(() -> {
            Thread.sleep(Duration.ofSeconds(1));  // Parking, not blocking OS thread
            return i;
        })
    );
}
```

```bash
# JVM flags for virtual thread debugging
java -Djdk.virtualThreadScheduler.parallelism=4 \
     -Djdk.virtualThreadScheduler.maxPoolSize=256 \
     MyApp

# Thread dump (shows virtual threads separately)
jcmd <pid> Thread.dump_to_file -format=json /tmp/threads.json

# JFR: Java Flight Recorder for virtual thread events
java -XX:StartFlightRecording=filename=vthreads.jfr,duration=60s MyApp
jfr print --events VirtualThreadStart,VirtualThreadEnd,VirtualThreadPinned vthreads.jfr
```

**Key virtual thread concepts:**
- **Mounting/unmounting**: virtual thread mounts onto a carrier (platform) thread to run; unmounts on blocking I/O
- **Pinning**: virtual thread pinned to carrier during `synchronized` blocks (avoid with `ReentrantLock`)
- **Carrier thread pool**: `ForkJoinPool` with `GOMAXPROCS`-equivalent parallelism

### Rust Async/Await (Cooperative M:N)

Rust's async model is zero-cost: no runtime overhead for non-async code.

```rust
use tokio::time::{sleep, Duration};

#[tokio::main]  // tokio = M:N async runtime (like Go's scheduler)
async fn main() {
    // Spawn 100,000 tasks - each is a state machine, not an OS thread
    let handles: Vec<_> = (0..100_000)
        .map(|i| {
            tokio::spawn(async move {  // lightweight task (~bytes overhead)
                sleep(Duration::from_millis(1)).await;  // Cooperative yield point
                println!("task {i} done");
            })
        })
        .collect();

    // Join all tasks
    for h in handles {
        h.await.unwrap();
    }
}
```

```bash
# Tokio console: runtime introspection
# Cargo.toml: console-subscriber = "0.1"
# Run: TOKIO_CONSOLE_BIND=127.0.0.1:6669 cargo run
tokio-console  # TUI for live task/resource view

# Async call graph with tokio-tracing
RUST_LOG=debug cargo run

# Compile-time thread safety (no runtime overhead!)
# The borrow checker enforces: Send + Sync
# Send: can be moved to another thread
# Sync: can be shared between threads (&T is Send if T is Sync)
```

---

## 15. Kernel Thread Internals: task_struct Deep Dive

```bash
# Every thread on Linux IS a task_struct
# See count of tasks (includes kernel threads)
cat /proc/sys/kernel/threads-max    # System limit
cat /proc/loadavg                   # Shows runnable + blocked task counts

# Per-task memory usage
cat /proc/<pid>/status | grep -E "VmRSS|VmSize|Threads|voluntary_ctxt"

# Stack size per thread
cat /proc/<pid>/limits | grep "stack"

# See kernel stacks (requires CONFIG_KALLSYMS)
sudo cat /proc/<tid>/wchan          # Where kernel thread is sleeping

# All kernel threads
ps aux | grep '\[' | head -20       # Kernel threads shown in []
# [kworker/0:0]  = workqueue worker on CPU 0
# [ksoftirqd/0]  = softirq handler on CPU 0
# [migration/0]  = scheduler migration thread CPU 0
# [kswapd0]      = page reclaim thread

# Thread group leader vs. thread
# TGID (Thread Group ID) = PID of thread group leader (what getpid() returns)
# PID (in kernel) = unique per-thread ID (what gettid() returns)
# ls /proc/<pid>/task/  shows all TIDs in the thread group

# context switch rates
vmstat 1 | awk '{print $12}'      # cs column = context switches/sec
pidstat -w 1 -p <pid>             # Per-process context switch rate
```

### clone() Flags Reference

```c
/* clone() is the true low-level call; fork/pthread_create are wrappers */
#include <sched.h>

/* fork() = clone with nothing shared */
pid_t fork_pid = clone(child_fn, stack,
    SIGCHLD,                        /* Signal parent on death */
    arg);

/* pthread_create() = clone with everything shared */
pid_t thread_pid = clone(thread_fn, stack,
    CLONE_VM        |   /* Share virtual memory (address space) */
    CLONE_FS        |   /* Share cwd, root, umask */
    CLONE_FILES     |   /* Share file descriptor table */
    CLONE_SIGHAND   |   /* Share signal handlers */
    CLONE_THREAD    |   /* Place in same thread group (same TGID) */
    CLONE_SETTLS    |   /* Set thread-local storage descriptor */
    CLONE_PARENT_SETTID |   /* Write TID to parent's memory */
    CLONE_CHILD_CLEARTID,   /* Clear TID in child's memory on exit (futex wake) */
    arg);

/* Container = clone with new namespaces */
pid_t container_pid = clone(init_fn, stack,
    CLONE_NEWPID    |   /* New PID namespace */
    CLONE_NEWNET    |   /* New network namespace */
    CLONE_NEWNS     |   /* New mount namespace */
    CLONE_NEWUTS    |   /* New UTS (hostname) namespace */
    SIGCHLD,
    arg);
```

```bash
# Observe clone() flags with strace
strace -e clone pthread_example 2>&1 | grep clone
# clone(child_stack=0x7f..., flags=CLONE_VM|CLONE_FS|CLONE_FILES|..., ...)

# See thread scheduling policy and priority
chrt -p <tid>                    # SCHED_OTHER / SCHED_FIFO / SCHED_RR + priority
schedtool -p <tid>               # Alternative tool

# Set real-time priority for a thread (root required)
chrt -f -p 50 <tid>             # SCHED_FIFO, priority 50
chrt -r -p 50 <tid>             # SCHED_RR, priority 50 (preemptible)

# CPU time per thread
pidstat -t -p <pid> 1           # Per-thread CPU usage
top -H -p <pid>                  # Thread view in top
```

---

## 16. Cross-Platform Event Multiplexing: Linux (epoll), macOS (kqueue), Windows (IOCP)

Modern high-performance concurrent servers combine multi-threading with asynchronous I/O multiplexing to handle tens of thousands of simultaneous connections (the C10K/C10M problem).
Each major operating system provides an architectural event demultiplexer:

```
+-------------------------------------------------------------------------------+
| Feature              | Linux epoll            | macOS kqueue         | Windows IOCP         |
+----------------------+------------------------+----------------------+----------------------+
| Core Paradigm        | Readiness Notification | Readiness Notification| Completion Model     |
| Creation Syscall     | epoll_create1()        | kqueue()             | CreateIoCompletionPort()|
| Registration         | epoll_ctl()            | kevent() EV_SET      | CreateIoCompletionPort()|
| Event Wait           | epoll_wait()           | kevent()             | GetQueuedCompletionStatus()|
| Edge vs Level        | Both (EPOLLET default) | Both (EV_CLEAR)      | Completion only      |
| Signal Multiplexing  | signalfd()             | EVFILT_SIGNAL        | MsgWaitForMultipleObjects|
| File I/O Support     | Poor (need io_uring)   | EVFILT_VNODE         | First-class support  |
+-------------------------------------------------------------------------------+
```

### 1. Linux: `epoll` Edge-Triggered Architecture

```c
// Linux epoll readiness loop
int epoll_fd = epoll_create1(0);
struct epoll_event ev, events[64];

ev.events = EPOLLIN | EPOLLET; // Edge-triggered
ev.data.fd = server_socket;
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &ev);

while (running) {
    int nfds = epoll_wait(epoll_fd, events, 64, -1);
    for (int i = 0; i < nfds; i++) {
        // Handle ready file descriptors
    }
}
```

### 2. macOS: `kqueue` and `kevent` Filter Architecture

`kqueue` is Darwin's native event multiplexing engine.
Unlike `epoll` which is limited to file descriptors, `kqueue` provides a unified filter mechanism (`struct kevent`) capable of monitoring sockets, files (`EVFILT_VNODE`), POSIX signals (`EVFILT_SIGNAL`), process termination (`EVFILT_PROC`), and hardware timers (`EVFILT_TIMER`):

```c
// macOS kqueue event loop
#include <sys/event.h>
#include <sys/time.h>

int kq = kqueue();
struct kevent change_event, event_list[64];

// Monitor socket readiness and process signals through the exact same queue
EV_SET(&change_event, socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
kevent(kq, &change_event, 1, NULL, 0, NULL);

// Register signal monitoring without signal handlers
signal(SIGINT, SIG_IGN); // Ignore default signal disposition
EV_SET(&change_event, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
kevent(kq, &change_event, 1, NULL, 0, NULL);

while (1) {
    int nevents = kevent(kq, NULL, 0, event_list, 64, NULL);
    for (int i = 0; i < nevents; i++) {
        if (event_list[i].filter == EVFILT_SIGNAL) {
            printf("Caught signal %ld safely inside kqueue event loop\n", event_list[i].ident);
            return 0;
        }
        // Handle socket I/O
    }
}
```

### 3. Windows: I/O Completion Ports (IOCP)

Windows implements a **proactive completion model** rather than a reactive readiness model.
The application initiates an asynchronous I/O operation (e.g., `ReadFile`, `WSARecv`) and provides an `OVERLAPPED` structure.
The Windows kernel performs the I/O transfer via DMA into the application's user buffer and delivers a completion packet to the thread-pool completion port queue:

```c
#include <windows.h>

HANDLE iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
CreateIoCompletionPort((HANDLE)socket, iocp, (ULONG_PTR)context, 0);

// Worker threads dequeue completed I/O packets:
DWORD bytes_transferred;
ULONG_PTR completion_key;
LPOVERLAPPED overlapped;

while (GetQueuedCompletionStatus(iocp, &bytes_transferred, &completion_key, &overlapped, INFINITE)) {
    // Process completed buffer without needing to call read() again
}
```

---

**Previous:** [P2L3: Threads Case Study - PThreads](P2L3-PThreads-Case-Study.md)
**Next:** [P2L5: Thread Performance Considerations](P2L5-Thread-Performance-Considerations.md)


