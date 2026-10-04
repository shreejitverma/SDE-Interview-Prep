---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P2L1"
  - "Operating System Concepts, 10th Ed., Silberschatz"
  - "Linux Kernel Development, 3rd Ed., Robert Love"
  - "Windows Internals, 7th Ed., Russinovich"
---

# P2L1: Processes and Process Management

> **Module goal:** Understand the process abstraction, its address space layout, the process control block, context switching mechanics, process lifecycle, CPU scheduling basics, and inter-process communication mechanisms.

## Table of Contents

- [1. Visual Metaphor](#1-visual-metaphor)
- [2. What is a Process?](#2-what-is-a-process)
- [3. What Does a Process Look Like?](#3-what-does-a-process-look-like)
- [4. Process Address Space](#4-process-address-space)
- [5. Address Space and Memory Management](#5-address-space-and-memory-management)
- [6. Process Execution State](#6-process-execution-state)
- [7. Process Control Block (PCB)](#7-process-control-block-pcb)
- [8. How is a PCB Used?](#8-how-is-a-pcb-used)
- [9. Context Switch](#9-context-switch)
- [10. Process Life Cycle: States and Transitions](#10-process-life-cycle-states-and-transitions)
- [11. Process Creation](#11-process-creation)
- [12. Role of the CPU Scheduler](#12-role-of-the-cpu-scheduler)
- [13. CPU and I/O Bursts](#13-cpu-and-io-bursts)
- [14. Inter-Process Communication (IPC)](#14-inter-process-communication-ipc)
- [15. Quizzes and Exercises](#15-quizzes-and-exercises)
- [16. Key Takeaways](#16-key-takeaways)
- [17. The /proc Filesystem: Process Introspection Deep Dive](#17-the-proc-filesystem-process-introspection-deep-dive)
- [18. Process Tracing with strace, ltrace, and perf](#18-process-tracing-with-strace-ltrace-and-perf)
- [19. Copy-on-Write (COW) Mechanics](#19-copy-on-write-cow-mechanics)
- [20. Process Namespaces: Isolation Without VMs](#20-process-namespaces-isolation-without-vms)
- [21. Windows Process Internals: CreateProcess and EPROCESS](#21-windows-process-internals-createprocess-and-eprocess)
- [22. cgroup Process Resource Control](#22-cgroup-process-resource-control)
- [23. Process Accounting and Audit](#23-process-accounting-and-audit)
- [24. macOS Process Architecture: Mach Tasks, BSD Processes, and posix_spawn](#24-macos-process-architecture-mach-tasks-bsd-processes-and-posix_spawn)

---

## 1. Visual Metaphor

A **process** is like an **order of work** in the toy shop:

| Toy Shop | Process |
|----------|---------|
| Order form (item, quantity, deadline) | Program + input data + state |
| Work area assigned to the order | Address space |
| Worker(s) executing the order | Thread(s) of execution |
| Materials allocated (paint, wood, screws) | Resources (CPU time, memory, file descriptors) |
| Progress tracking slip | Process Control Block (PCB) |

The shop manager (OS) tracks every active order, allocates workbenches, and switches workers between orders.

---

## 2. What is a Process?

A **process** is an instance of a program in execution.

```
Program (static)                    Process (dynamic)
+-------------------+               +-------------------+
| Binary on disk    |  --- exec --> | Running instance  |
| /usr/bin/ls       |               | PID: 12345        |
|                   |               | State: running    |
| - Code (.text)    |               | - Registers       |
| - Data (.data)    |               | - Stack pointer   |
| - Symbols         |               | - Open files      |
+-------------------+               | - Memory mappings |
                                    | - Credentials     |
                                    +-------------------+
```

**Critical distinction:**
- A **program** is a passive entity (a file on disk).
- A **process** is an active entity (a program being executed, with its own state, resources, and address space).
- Multiple processes can run the same program (e.g., two terminal windows each running `bash`).

**Linux - inspecting processes:**
```bash
# List all processes
ps aux
ps -ef

# Detailed info for a specific process
cat /proc/$$/status          # $$ = current shell's PID
cat /proc/$$/maps            # memory mappings
cat /proc/$$/fd              # open file descriptors (use ls -la)
ls -la /proc/$$/fd

# Tree view of process hierarchy
pstree -p

# Real-time process monitor
top
htop                         # if installed; much better UI
```

**Windows - inspecting processes:**
```powershell
# List all processes
Get-Process | Format-Table Id, ProcessName, CPU, WorkingSet64 -AutoSize

# Detailed info for a specific process
Get-Process -Id $PID | Format-List *

# Process tree (requires Sysinternals)
# pslist -t       (from Sysinternals)

# Task Manager programmatic equivalent
Get-CimInstance Win32_Process | Select-Object ProcessId, Name, CommandLine |
  Format-Table -AutoSize
```

---

## 3. What Does a Process Look Like?

A process consists of:

1. **Address space** - the memory allocated to the process
2. **Execution state** - register values, program counter, stack pointer
3. **OS metadata** - PID, credentials, open files, signal handlers (stored in the PCB)

```
A Process in Memory
+---------------------------------------------------+ 0xFFFF...
|                                                   |
|              Kernel Space                          |
|        (mapped into every process but              |
|         only accessible in kernel mode)            |
|                                                   |
+---------------------------------------------------+ (kernel/user split)
|              Stack                                 |
|  (grows downward)                                 |
|  - local variables                                |
|  - function call frames                           |
|  - return addresses                               |
|              |                                     |
|              v                                     |
|                                                   |
|         (unmapped gap)                             |
|                                                   |
|              ^                                     |
|              |                                     |
|              Heap                                  |
|  (grows upward)                                   |
|  - malloc/new allocations                         |
|  - dynamic data structures                        |
+---------------------------------------------------+
|              BSS Segment                           |
|  (uninitialized global/static variables)           |
|  (zero-initialized by the OS at load time)         |
+---------------------------------------------------+
|              Data Segment                          |
|  (initialized global/static variables)             |
+---------------------------------------------------+
|              Text Segment (Code)                   |
|  (read-only, executable)                           |
|  (shared across processes running same program)    |
+---------------------------------------------------+ 0x0000...
```

---

## 4. Process Address Space

The **address space** is the range of virtual addresses a process can reference.
On a 64-bit system, the theoretical address space is 2^64 bytes = 16 EB (exabytes), but in practice:

| Architecture | User space range | Kernel space range |
|-------------|-----------------|-------------------|
| x86-64 Linux (48-bit) | `0x0000000000000000` - `0x00007FFFFFFFFFFF` (128 TB) | `0xFFFF800000000000` - `0xFFFFFFFFFFFFFFFF` |
| x86-64 Linux (57-bit, 5-level) | `0x0000000000000000` - `0x00FFFFFFFFFFFFFF` (64 PB) | Upper half |
| ARM64 Linux | `0x0000000000000000` - `0x0000FFFFFFFFFFFF` (256 TB) | `0xFFFF000000000000` - `0xFFFFFFFFFFFFFFFF` |
| x86-64 Windows | `0x0000000000000000` - `0x00007FFFFFFEFFFF` (128 TB) | `0xFFFF800000000000` - `0xFFFFFFFFFFFFFFFF` |

### Examining the Address Space

**Linux:**
```bash
# See a process's virtual memory layout
cat /proc/$$/maps
# Output format: start-end perms offset dev inode pathname
# Example:
# 55a4c8200000-55a4c8228000 r--p 00000000 08:01 1234  /usr/bin/bash
# 55a4c8228000-55a4c82f1000 r-xp 00028000 08:01 1234  /usr/bin/bash  (code)
# 55a4c82f1000-55a4c82fb000 r--p 000f1000 08:01 1234  /usr/bin/bash  (rodata)
# 55a4c82fc000-55a4c8300000 rw-p 000fb000 08:01 1234  /usr/bin/bash  (data)
# 55a4c8300000-55a4c8312000 rw-p 00000000 00:00 0      [heap]
# 7fff8a200000-7fff8a221000 rw-p 00000000 00:00 0      [stack]

# Summarized view
pmap $$
pmap -x $$   # extended info with RSS

# See ASLR status (Address Space Layout Randomization)
cat /proc/sys/kernel/randomize_va_space
# 0 = off, 1 = stack/mmap randomized, 2 = full ASLR (default)
```

**Windows:**
```powershell
# Using VMMap from Sysinternals for detailed virtual address space
# Or programmatically:

# See working set size
Get-Process -Id $PID | Select-Object WorkingSet64, VirtualMemorySize64,
    PagedMemorySize64, NonpagedSystemMemorySize64

# Using Process Explorer (Sysinternals) for detailed per-region view
```

**C program to explore its own address space:**
```c
#include <stdio.h>
#include <stdlib.h>

int global_initialized = 42;        // .data segment
int global_uninitialized;           // .bss segment

void function(void) {               // .text segment
    int local = 10;                 // stack
    printf("  Stack variable:            %p\n", (void*)&local);
}

int main(void) {
    int stack_var = 1;
    int *heap_var = malloc(sizeof(int));
    *heap_var = 2;
    static int static_var = 99;

    printf("Process Address Space Layout:\n");
    printf("  Code (main):               %p\n", (void*)main);
    printf("  Code (function):           %p\n", (void*)function);
    printf("  Initialized global:        %p\n", (void*)&global_initialized);
    printf("  Uninitialized global (BSS): %p\n", (void*)&global_uninitialized);
    printf("  Static local:              %p\n", (void*)&static_var);
    printf("  Heap allocation:           %p\n", (void*)heap_var);
    printf("  Stack variable:            %p\n", (void*)&stack_var);
    function();

    free(heap_var);
    return 0;
}
```

```bash
gcc -o addr_space addr_space.c && ./addr_space
# Observe: code < globals < heap <<< stack
# Each run may give different addresses (ASLR)
```

> **Quiz: Virtual Addresses**
>
> *If two processes are running the same program, will their global variable `x` have the same virtual address?*
>
> **Answer:** Possibly the same virtual address (due to identical binary layout), but they map to **different physical addresses**. Each process has its own page table. With ASLR enabled, even the virtual addresses may differ between runs.

---

## 5. Address Space and Memory Management

The OS uses **virtual memory** to give each process the illusion of having its own private, contiguous address space.

```
Process A's view          Physical RAM           Process B's view
+---------------+        +---------------+       +---------------+
| Page 0 (code) | -----> | Frame 7       |       | Page 0 (code) | --+
+---------------+        +---------------+       +---------------+   |
| Page 1 (data) | -----> | Frame 12      |       | Page 1 (data) | --+--> Frame 23
+---------------+        +---------------+       +---------------+   |
| Page 2 (heap) | -----> | Frame 3       |       | Page 2 (heap) | --+--> Frame 45
+---------------+        +---------------+       +---------------+
| Page 3 (stack)| -----> | Frame 50      |       | Page 3 (stack)| -----> Frame 51
+---------------+        +---------------+       +---------------+

Key insight: Same virtual addresses, different physical frames.
The page table (per-process) performs the translation.
```

**Key mechanisms** (detailed in P3L2: Memory Management):
- **Page table:** translates virtual page numbers to physical frame numbers
- **TLB (Translation Lookaside Buffer):** hardware cache for page table entries
- **Page faults:** when a virtual page isn't in physical memory, the OS loads it from disk
- **Demand paging:** pages are loaded only when first accessed

---

## 6. Process Execution State

At any instant, a process's execution state is defined by:

```
Execution State:
+-----------------------------------+
| Program Counter (PC / RIP)        |  <-- which instruction to execute next
| Stack Pointer (SP / RSP)          |  <-- top of the current stack frame
| General-Purpose Registers         |  <-- rax, rbx, rcx, rdx, rsi, rdi...
| Floating-Point Registers          |  <-- xmm0-xmm15, ymm0-ymm15
| Status/Flags Register (RFLAGS)    |  <-- carry, zero, overflow flags
| Segment Registers                 |  <-- CS, DS, SS (mostly legacy on x86-64)
+-----------------------------------+
```

**Linux - inspecting register state of a running process:**
```bash
# Using gdb to examine registers
gdb -p $PID
# Inside gdb:
# (gdb) info registers
# (gdb) print $rip          # instruction pointer
# (gdb) print $rsp          # stack pointer
# (gdb) print $rax          # return value register

# Without gdb, from /proc:
cat /proc/$PID/stat
# Field 28 = start code, field 29 = end code
cat /proc/$PID/syscall
# Shows current syscall number and arguments
```

**Windows - inspecting register state:**
```powershell
# Using WinDbg:
# !process 0 0          List all processes
# .attach <PID>         Attach to process
# r                     Display registers
# r rip                 Display instruction pointer

# Using PowerShell + Debug Diagnostics:
# procdump -ma <PID>    (Sysinternals - creates a crash dump)
```

---

## 7. Process Control Block (PCB)

The **Process Control Block (PCB)** is the kernel data structure that stores all information about a process.

```
Process Control Block (PCB)
+--------------------------------------------------+
| Process Identification                            |
|   - PID (Process ID)                             |
|   - PPID (Parent Process ID)                     |
|   - UID/GID (User/Group ID)                      |
+--------------------------------------------------+
| Process State                                     |
|   - new / ready / running / waiting / terminated  |
+--------------------------------------------------+
| CPU State (saved on context switch)               |
|   - Program counter (RIP)                        |
|   - Stack pointer (RSP)                          |
|   - General-purpose registers                    |
|   - FPU/SSE/AVX state                           |
|   - Flags register                               |
+--------------------------------------------------+
| Memory Management Info                            |
|   - Page table base register (CR3 on x86)        |
|   - Virtual memory areas (VMAs)                  |
|   - Memory limits                                |
+--------------------------------------------------+
| Scheduling Info                                   |
|   - Priority                                     |
|   - Scheduling policy (SCHED_OTHER, SCHED_FIFO)  |
|   - CPU time consumed                            |
|   - Nice value                                   |
+--------------------------------------------------+
| I/O and File Info                                 |
|   - Open file descriptor table                   |
|   - Current working directory                    |
|   - Root directory                               |
+--------------------------------------------------+
| Signal Info                                       |
|   - Pending signals                              |
|   - Signal mask                                  |
|   - Signal handlers                              |
+--------------------------------------------------+
| IPC Info                                          |
|   - Shared memory segments                       |
|   - Message queues                               |
|   - Semaphores                                   |
+--------------------------------------------------+
| Accounting                                        |
|   - CPU time used (user + kernel)                |
|   - Wall clock time                              |
|   - Memory high watermark                        |
+--------------------------------------------------+
```

### Linux: `task_struct`

In Linux, the PCB is the `task_struct` structure, defined in `include/linux/sched.h`.
It is one of the largest structures in the kernel (~6KB+).

```bash
# See task_struct size
sudo cat /proc/kallsyms | grep task_struct

# Key fields in task_struct:
#   pid                  - process ID
#   tgid                 - thread group ID (== PID for main thread)
#   state                - TASK_RUNNING, TASK_INTERRUPTIBLE, etc.
#   mm                   - pointer to memory descriptor (mm_struct)
#   fs                   - filesystem info
#   files                - open file descriptors (files_struct)
#   signal               - signal handlers
#   parent               - pointer to parent's task_struct
#   children             - list of child task_structs
#   se                   - scheduling entity (CFS)
#   policy               - scheduling policy
#   prio, static_prio    - priority values

# Inspect key PCB fields via /proc:
echo "PID: $$"
echo "State: $(cat /proc/$$/status | grep State)"
echo "Parent PID: $(cat /proc/$$/status | grep PPid)"
echo "Threads: $(cat /proc/$$/status | grep Threads)"
echo "Memory: $(cat /proc/$$/status | grep VmSize)"
echo "Open FDs: $(ls /proc/$$/fd | wc -l)"
echo "Scheduling policy: $(chrt -p $$)"
```

### Windows: `EPROCESS` and `KPROCESS`

In Windows, the PCB equivalent is the `EPROCESS` structure (Executive Process Block).

```
EPROCESS (Executive Process Block)
+---------------------------+
| KPROCESS (Kernel Block)   |  <-- scheduling, thread list, affinity
|   - DirectoryTableBase    |  <-- page table (like CR3)
|   - ThreadListHead        |
|   - BasePriority          |
|   - Affinity              |
+---------------------------+
| PEB (Process Env Block)   |  <-- user-space accessible
|   - ImageBaseAddress      |
|   - ProcessHeap           |
|   - ProcessParameters     |
+---------------------------+
| UniqueProcessId           |  <-- PID
| InheritedFromUniqueProcessId | <-- PPID
| Token                     |  <-- security token
| ObjectTable               |  <-- handle table
| VadRoot                   |  <-- virtual address descriptors (memory map)
| SectionObject             |  <-- executable image section
+---------------------------+
```

```powershell
# Inspect process info (user-accessible subset of EPROCESS)
Get-Process -Id $PID | Select-Object Id, ProcessName, StartTime,
    PriorityClass, HandleCount, WorkingSet64,
    UserProcessorTime, PrivilegedProcessorTime

# WinDbg commands for EPROCESS inspection:
# dt nt!_EPROCESS
# !process 0 0 notepad.exe
# !process <EPROCESS_addr> 7
```

---

## 8. How is a PCB Used?

The PCB is the kernel's "bookmark" for a process.

**During a context switch:**

```
1. Process A is running
   CPU registers contain A's state
   
2. Timer interrupt fires (or A makes a blocking syscall)
   
3. Kernel saves A's register state INTO A's PCB
   +---PCB_A---+
   | RIP = 0x4567  (where A was executing) |
   | RSP = 0x7FFF  (A's stack pointer)     |
   | RAX = 42      (A's computation)       |
   +----------------------------------------+
   
4. Kernel selects Process B (scheduler decision)
   
5. Kernel loads B's register state FROM B's PCB
   +---PCB_B---+
   | RIP = 0x1234  (where B left off)      |
   | RSP = 0x3FFF  (B's stack pointer)     |
   | RAX = 99      (B's computation)       |
   +----------------------------------------+
   
6. Kernel switches page tables (CR3 = B's page table base)
   
7. Kernel returns to user mode
   CPU now executes B's code from where B left off
```

**Key point:** The PCB enables the illusion that each process has its own dedicated CPU.
In reality, the CPU rapidly switches between processes, saving/restoring state via PCBs.

---

## 9. Context Switch

A **context switch** is the mechanism of saving the state of one process and loading the state of another.

### What Happens During a Context Switch

```
Time
  |
  |  Process A running (user mode)
  |  +-----------------------+
  |  | User code executing   |
  |  +-----------------------+
  |     | <-- interrupt/syscall
  |     v
  |  +-----------------------+
  |  | 1. Enter kernel mode  |  ~10-30 ns  (hardware mode switch)
  |  +-----------------------+
  |  | 2. Save A's registers |  ~50-100 ns (to A's kernel stack / PCB)
  |  |    to PCB_A           |
  |  +-----------------------+
  |  | 3. Update A's state   |  ~10 ns     (RUNNING -> READY or WAITING)
  |  |    in PCB_A           |
  |  +-----------------------+
  |  | 4. Scheduler runs:    |  ~100-500 ns (pick next process)
  |  |    select Process B   |
  |  +-----------------------+
  |  | 5. Update B's state   |  ~10 ns     (READY -> RUNNING)
  |  |    in PCB_B           |
  |  +-----------------------+
  |  | 6. Restore B's regs   |  ~50-100 ns (from PCB_B)
  |  |    from PCB_B         |
  |  +-----------------------+
  |  | 7. Switch page table  |  ~20-50 ns  (load B's CR3 / TTBR)
  |  |    (CR3 = B's PT)     |
  |  +-----------------------+
  |  | 8. Return to user mode|  ~10-30 ns
  |  +-----------------------+
  |     |
  |     v
  |  Process B running (user mode)
  |  +-----------------------+
  |  | B's code executing    |
  |  +-----------------------+
  |
  v
```

### Direct Cost vs. Indirect Cost

| Cost Type | Description | Magnitude |
|-----------|-------------|-----------|
| **Direct** | Register save/restore, page table switch, mode transitions | ~1-5 us |
| **Indirect (cold cache)** | TLB misses, cache misses after switch | ~10-1000 us |

The **indirect cost** dominates.
After switching to Process B, the CPU caches and TLB are full of Process A's data ("hot" for A, "cold" for B).
B will experience many cache misses as it refills the caches with its own data.

> **Quiz: Hot Cache**
>
> *When we say a cache is "hot" for a process, we mean:*
> - (a) The cache contains data the process will likely access soon
> - (b) The cache is physically warm from heavy computation
> - (c) The cache has been recently flushed
>
> **Answer:** (a). A "hot cache" means the cache lines are populated with data relevant to the currently running process, leading to high hit rates. After a context switch, the cache is "cold" for the incoming process.

### Measuring Context Switch Cost

**Linux:**
```bash
# Rough measurement using lmbench
# lmbench is the gold standard for this measurement
# lat_ctx -s 0 2   # context switch latency between 2 processes, 0 bytes

# Quick and dirty: use perf
perf stat -e context-switches,cpu-migrations sleep 10

# See context switches per process
cat /proc/$PID/status | grep voluntary_ctxt_switches
cat /proc/$PID/status | grep nonvoluntary_ctxt_switches
# voluntary = process called a blocking syscall
# nonvoluntary = preempted by scheduler (timeslice expired)
```

**Windows:**
```powershell
# Context switches per second (system-wide)
Get-Counter '\System\Context Switches/sec'

# Per-process context switches
Get-Counter '\Thread(*)\Context Switches/sec'

# Using Performance Monitor (perfmon.msc):
# Add counter: Process -> Context Switches/sec
```

---

## 10. Process Life Cycle: States and Transitions

### Process States

```
                         +--------------------+
              fork()     |                    |
         +-------------->|       NEW          |
         |               |  (being created)   |
         |               +--------+-----------+
         |                        |
         |                        | admitted (OS loads PCB, allocates resources)
         |                        v
         |               +--------+-----------+
         |               |                    |<---------+
         |               |      READY         |          |
         |               |  (in ready queue,  |          |
         |               |   waiting for CPU) |          |
         |               +--------+-----------+          |
         |                        |                      |
         |         dispatched     |                      |  preempted
         |      (scheduler picks) |                      | (timeslice expired
         |                        v                      |  or higher priority
         |               +--------+-----------+          |  process arrives)
         |               |                    |----------+
         |               |     RUNNING        |
         |               |  (executing on CPU)|
         |               +--+-----+-----+-----+
         |                  |     |     |
         |     I/O request  |     |     | exit()
         |     or wait()    |     |     |
         |                  v     |     v
         |   +----------+-----+  | +---+-----------+
         |   |                 |  | |               |
         |   |    WAITING      |  | |  TERMINATED   |
         |   | (blocked on I/O |  | | (zombie until |
         |   |  or event)      |  | |  parent waits)|
         |   +--------+--------+  | +---------------+
         |            |           |
         |            | I/O done  |
         |            | or event  |
         |            v           |
         |   READY <--+           |
         |                        |
         +------------------------+
```

### State Transitions

| Transition | Trigger | Example |
|-----------|---------|---------|
| NEW -> READY | OS admits process | `fork()` + `exec()` completes |
| READY -> RUNNING | Scheduler dispatches | Timeslice allocated |
| RUNNING -> READY | Preempted | Timer interrupt, higher-priority process |
| RUNNING -> WAITING | Blocking I/O or wait | `read()`, `wait()`, `sleep()`, lock acquisition |
| WAITING -> READY | I/O complete or event | Disk read completes, child exits |
| RUNNING -> TERMINATED | Process exits | `exit()`, `return` from `main()`, signal (SIGKILL) |

**Linux - observe process states:**
```bash
# In ps output, the STAT column shows process state:
ps aux | head -5
# STAT codes:
# R = Running/Runnable
# S = Sleeping (interruptible - waiting for event)
# D = Disk sleep (uninterruptible - waiting for I/O)
# T = Stopped (by signal, e.g., SIGSTOP or Ctrl+Z)
# Z = Zombie (terminated but parent hasn't waited)
# I = Idle (kernel thread)

# Find zombie processes
ps aux | awk '$8 ~ /Z/'

# Find processes in D state (uninterruptible sleep)
ps aux | awk '$8 ~ /D/'
```

**Windows - process states:**
```powershell
# Windows doesn't directly expose the 5-state model, but threads have:
# Ready, Running, Waiting, Standby, Terminated, Transition, etc.

# See thread states
Get-Process notepad | Select-Object -ExpandProperty Threads |
  Select-Object Id, ThreadState, WaitReason

# Thread states: Running, Ready, Standby, Wait, Transition, Terminated, Unknown
```

> **Quiz: Process State**
>
> *A process makes a read() system call to read from disk. What state transition occurs?*
>
> **Answer:** RUNNING -> WAITING. The process blocks because disk I/O takes millions of CPU cycles. When the disk read completes (interrupt), the process transitions WAITING -> READY.

> **Quiz: Parent Process**
>
> *In Linux, what happens when a child process terminates but its parent never calls wait()?*
>
> **Answer:** The child becomes a **zombie** (state Z). Its PCB remains in the kernel (consuming a PID and a small amount of kernel memory) until the parent calls `wait()` or `waitpid()`. If the parent itself exits without waiting, the zombie is reparented to PID 1 (`init`/`systemd`), which periodically reaps zombies.

---

## 11. Process Creation

### Linux: `fork()` + `exec()`

Linux uses a two-step process creation model:

```
Parent Process (PID 100)
    |
    | fork()
    |----->  Child Process (PID 101)
    |            |
    |            | exec("/usr/bin/ls")
    |            |----->  Now running 'ls'
    |            |        (new code, data, stack)
    |            |        (PID still 101)
    |            |
    | wait()     | ls runs...
    | (blocked)  |
    |            | exit(0)
    |<-----------+
    | (resumed, gets exit status)
```

**`fork()`:**
- Creates a near-exact copy of the calling process
- Child gets a new PID but inherits parent's address space (via copy-on-write), open files, signal handlers, etc.
- Returns child's PID to parent, 0 to child

**`exec()`:**
- Replaces the calling process's code, data, and stack with a new program
- PID does not change
- Open file descriptors are inherited (unless `FD_CLOEXEC` is set)

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    printf("Parent PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        exit(1);
    }

    if (pid == 0) {
        // Child process
        printf("Child PID: %d, Parent PID: %d\n", getpid(), getppid());

        // Replace child with 'ls -la /tmp'
        execlp("ls", "ls", "-la", "/tmp", NULL);

        // If exec returns, it failed
        perror("exec failed");
        exit(1);
    }

    // Parent process
    int status;
    waitpid(pid, &status, 0);  // Wait for child to finish

    if (WIFEXITED(status)) {
        printf("Child exited with status %d\n", WEXITSTATUS(status));
    }

    return 0;
}
```

```bash
gcc -o fork_exec fork_exec.c && ./fork_exec
```

**Copy-on-Write (COW):**
After `fork()`, parent and child share the same physical pages (marked read-only).
Only when one of them **writes** to a page does the kernel copy that page.
This makes `fork()` fast even for processes with large address spaces.

```
Before fork():
  Parent page table:  VP0 -> PF5 (rw)

After fork() (COW):
  Parent page table:  VP0 -> PF5 (r-)  <-- marked read-only
  Child page table:   VP0 -> PF5 (r-)  <-- shares same physical frame

When parent writes to VP0:
  1. Page fault (write to read-only page)
  2. Kernel copies PF5 to new frame PF9
  3. Parent page table: VP0 -> PF9 (rw)  <-- parent's private copy
  4. Child page table:  VP0 -> PF5 (rw)  <-- child keeps original
```

**Linux `clone()` (advanced):**
`fork()` is actually implemented via `clone()`, which allows fine-grained control over what is shared:

```c
// clone() flags:
// CLONE_VM     - share address space (creates a thread)
// CLONE_FS     - share filesystem info
// CLONE_FILES  - share file descriptor table
// CLONE_SIGHAND - share signal handlers
// CLONE_THREAD - same thread group (same PID from userspace view)
// CLONE_NEWNS  - new mount namespace (containers)
// CLONE_NEWPID - new PID namespace (containers)
```

### Windows: `CreateProcess()`

Windows uses a single-step process creation model:

```c
#include <windows.h>
#include <stdio.h>

int main(void) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create a new process running "notepad.exe"
    BOOL success = CreateProcessA(
        NULL,                   // application name (use command line)
        "notepad.exe",          // command line
        NULL,                   // process security attributes
        NULL,                   // thread security attributes
        FALSE,                  // inherit handles
        0,                      // creation flags
        NULL,                   // environment (inherit parent's)
        NULL,                   // current directory (inherit parent's)
        &si,                    // startup info
        &pi                     // process info (output)
    );

    if (!success) {
        printf("CreateProcess failed: %lu\n", GetLastError());
        return 1;
    }

    printf("Created process with PID: %lu\n", pi.dwProcessId);
    printf("Created main thread with TID: %lu\n", pi.dwThreadId);

    // Wait for the child process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    printf("Child exited with code: %lu\n", exitCode);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}
```

```powershell
# Compile with MSVC
cl /Fe:create_proc.exe create_proc.c

# PowerShell equivalent
$proc = Start-Process notepad -PassThru
$proc.WaitForExit()
Write-Host "Exit code: $($proc.ExitCode)"
```

**Linux `fork()+exec()` vs. Windows `CreateProcess()` comparison:**

| Aspect | Linux `fork()+exec()` | Windows `CreateProcess()` |
|--------|----------------------|--------------------------|
| Steps | Two separate calls | Single call |
| Address space | Initially shared (COW) | New address space |
| Inheritance | Fine-grained (clone flags) | Handle inheritance flag |
| Flexibility | Can do work between fork/exec | Must specify everything upfront |
| Overhead | fork is cheap (COW) | Must create new page tables |
| Thread creation | Also via clone() | Separate: `CreateThread()` |

---

## 12. Role of the CPU Scheduler

The **CPU scheduler** decides which ready process gets the CPU next.

```
                     CPU Scheduler
                    +-------------+
Ready Queue:        |             |
[P1] [P2] [P3] --> | Algorithm:  | --> CPU: [Pn running]
                    | FCFS, RR,   |
                    | CFS, etc.   |
                    +-------------+

The scheduler runs when:
1. A process terminates or blocks (non-preemptive trigger)
2. A timer interrupt fires (preemptive trigger)
3. A higher-priority process becomes ready (preemptive trigger)
```

**Key scheduler responsibilities:**
- **Select** the next process to run (from the ready queue)
- **Allocate** CPU time (how long the process runs - the timeslice/quantum)
- **Preempt** if necessary (take the CPU away from a running process)
- **Balance** across CPUs (on multiprocessor systems)

Detailed scheduling algorithms are covered in [P3L1: Scheduling](../Part-3-Resource-Management/P3L1-Scheduling.md).

> **Quiz: Scheduler Responsibilities**
>
> *Which is NOT a scheduler responsibility?*
> - (a) Deciding which process runs next
> - (b) Allocating memory pages to a process
> - (c) Setting the timer interrupt for preemption
> - (d) Balancing load across CPUs
>
> **Answer:** (b) - Memory allocation is the memory manager's job, not the scheduler's.

---

## 13. CPU and I/O Bursts

Processes alternate between **CPU bursts** (computation) and **I/O bursts** (waiting for I/O).

```
Process Execution Timeline:
+--------+      +--------+      +--------+
|  CPU   |      |  CPU   |      |  CPU   |
| burst  |      | burst  |      | burst  |
+--------+      +--------+      +--------+
    |    +------+    |    +------+    |
    |    | I/O  |    |    | I/O  |    |
    |    | burst|    |    | burst|    |
    |    +------+    |    +------+    |
    |                |                |
    v                v                v
Time ------>
```

**Process classification by burst behavior:**

| Type | CPU Burst | I/O Burst | Example |
|------|-----------|-----------|---------|
| **CPU-bound** | Long | Short/rare | Scientific computing, compilation, encryption |
| **I/O-bound** | Short | Long/frequent | Web server, database, text editor |
| **Mixed** | Moderate | Moderate | Video encoding, data analysis |

**Why this matters for scheduling:**
- I/O-bound processes should get higher priority (they give up the CPU quickly anyway)
- CPU-bound processes should get longer timeslices (to amortize context switch overhead)
- A good scheduler recognizes the burst pattern and adapts

**Linux - observe CPU vs I/O behavior:**
```bash
# See CPU vs I/O wait
vmstat 1 5
#  r  = processes in run queue (CPU-bound demand)
#  b  = processes blocked on I/O
# us  = user CPU time
# sy  = system CPU time
# wa  = I/O wait time
# id  = idle time

# Per-process I/O stats
cat /proc/$PID/io
# rchar: bytes read
# wchar: bytes written
# syscr: read syscalls
# syscw: write syscalls
# read_bytes: actual disk reads
# write_bytes: actual disk writes

# iotop - like top but for I/O
sudo iotop -o   # show only processes doing I/O
```

---

## 14. Inter-Process Communication (IPC)

Processes are isolated by default. IPC mechanisms allow them to exchange data.

### Two Fundamental Approaches

```
Message-Based IPC                    Shared Memory IPC
+----------+    +----------+         +----------+    +----------+
|          |    |          |         |          |    |          |
| Process  |--->| Process  |         | Process  |    | Process  |
|    A     |msg |    B     |         |    A     |    |    B     |
|          |<---|          |         |    |     |    |     |    |
+----------+    +----------+         +----+-----+    +-----+----+
                                          |                |
Via kernel (pipes, sockets,               +---+--------+---+
message queues, signals)                      |  Shared |
                                              | Memory  |
                                              | Region  |
                                              +---------+
                                    Mapped into both address spaces
                                    (kernel sets up mapping, then
                                     gets out of the way)
```

### IPC Mechanisms Overview

| Mechanism | Type | Direction | Speed | Use Case |
|-----------|------|-----------|-------|----------|
| **Pipes** | Message | Unidirectional | Moderate | Parent-child communication |
| **Named pipes (FIFOs)** | Message | Unidirectional | Moderate | Unrelated processes |
| **Signals** | Message | Unidirectional | Fast (notification only) | Async event notification |
| **Message queues** | Message | Bidirectional | Moderate | Structured messages |
| **Unix domain sockets** | Message | Bidirectional | Fast | Local client-server |
| **Shared memory** | Shared memory | Bidirectional | Fastest | High-throughput data sharing |
| **Memory-mapped files** | Shared memory | Bidirectional | Fast | File-backed shared data |

### Shared Memory IPC Example

**Linux (POSIX shared memory):**
```c
// writer.c - creates shared memory and writes data
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *name = "/cs6200_shm";
    const int SIZE = 4096;

    // Create shared memory object
    int fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (fd < 0) { perror("shm_open"); exit(1); }

    // Set size
    ftruncate(fd, SIZE);

    // Map into address space
    char *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }

    // Write data
    sprintf(ptr, "Hello from writer process (PID %d)", getpid());
    printf("Writer: wrote to shared memory\n");

    // Keep running so reader can access
    printf("Writer: press Enter to cleanup...\n");
    getchar();

    munmap(ptr, SIZE);
    shm_unlink(name);
    return 0;
}
```

```c
// reader.c - reads from shared memory
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    const char *name = "/cs6200_shm";
    const int SIZE = 4096;

    int fd = shm_open(name, O_RDONLY, 0666);
    if (fd < 0) { perror("shm_open"); exit(1); }

    char *ptr = mmap(NULL, SIZE, PROT_READ, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) { perror("mmap"); exit(1); }

    printf("Reader: '%s'\n", ptr);

    munmap(ptr, SIZE);
    return 0;
}
```

```bash
# Compile
gcc -o writer writer.c -lrt
gcc -o reader reader.c -lrt

# Terminal 1:
./writer

# Terminal 2 (while writer is running):
./reader
# Output: Reader: 'Hello from writer process (PID 12345)'
```

**Windows (shared memory via file mapping):**
```c
#include <windows.h>
#include <stdio.h>

int main(void) {
    const char *name = "Local\\CS6200SharedMem";
    const int SIZE = 4096;

    // Create file mapping
    HANDLE hMap = CreateFileMappingA(
        INVALID_HANDLE_VALUE,   // use system page file
        NULL,                   // default security
        PAGE_READWRITE,
        0, SIZE,                // size
        name                    // name
    );
    if (!hMap) {
        printf("CreateFileMapping failed: %lu\n", GetLastError());
        return 1;
    }

    // Map view
    char *ptr = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, SIZE);
    if (!ptr) {
        printf("MapViewOfFile failed: %lu\n", GetLastError());
        return 1;
    }

    sprintf(ptr, "Hello from writer (PID %lu)", GetCurrentProcessId());
    printf("Writer: wrote to shared memory\n");
    printf("Press Enter to cleanup...\n");
    getchar();

    UnmapViewOfFile(ptr);
    CloseHandle(hMap);
    return 0;
}
```

> **Quiz: IPC Comparison**
>
> *Which IPC mechanism would you choose for each scenario?*
> 1. A web server sending a log message to a logger process: **Named pipe or Unix domain socket**
> 2. A trading system sharing real-time market data between matching engine and risk manager: **Shared memory** (lowest latency)
> 3. A parent process notifying a child to terminate: **Signal (SIGTERM)**
> 4. Two unrelated processes on different machines exchanging data: **TCP sockets** (not local IPC)

---

## 15. Quizzes and Exercises

> [!question] Quiz 1: Virtual Addresses and Physical Mapping (Clips 48-49)
> Consider two distinct processes, Process A and Process B, running concurrently on the same physical machine.
> Both processes examine the address of their local variable `int x` and observe that `&x == 0x7ffd5a00`.
> 1. Does variable `x` in Process A point to the exact same physical memory cell as `x` in Process B?
> 2. How does the operating system and hardware MMU prevent Process A from reading or modifying Process B's variable?

> [!success]- Answer
> 1. **No.** Both processes share the same numerical **virtual address**, but each process has its own private, isolated **page table**. The CPU's Memory Management Unit (MMU) uses Process A's page table to translate `0x7ffd5a00` to physical frame $F_A$, and uses Process B's page table to translate `0x7ffd5a00` to physical frame $F_B$ ($F_A \ne F_B$).
> 2. **Memory Protection Enforcement:** The kernel configures the CPU page table base register (CR3 on x86_64, TTBR0 on ARM64) during each context switch. When Process A executes, the CPU can only reference physical frames mapped in Process A's page table. Accessing an unmapped frame or a frame marked read-only triggers a hardware page fault exception (Ring 0 trap), causing the OS to terminate the offending process with a Segmentation Fault (`SIGSEGV`).

> [!question] Quiz 2: Hot Cache versus Cold Cache Context Switches (Clips 54-55)
> During a context switch from Process $P_1$ to Process $P_2$, the kernel saves $P_1$'s CPU registers into its PCB and restores $P_2$'s registers.
> 1. Distinguish between the **direct cost** and the **indirect cost** of a context switch.
> 2. What distinguishes a **hot cache** from a **cold cache**, and why does context switching frequency degrade computational throughput?

> [!success]- Answer
> 1. **Direct vs. Indirect Cost:**
>    - **Direct Cost:** The deterministic CPU cycles required to execute the kernel context switch routine (saving CPU registers, switching kernel stack pointers, swapping the page table base pointer in CR3, and updating the task state segment). Typically $1 \text{ to } 5 \ \mu\text{s}$ (~1,000-5,000 cycles).
>    - **Indirect Cost (Cache Pollution):** When $P_2$ begins executing, the L1/L2/L3 data and instruction caches still contain lines belonging to $P_1$. $P_2$ suffers a barrage of cache misses and TLB misses, forcing memory accesses to fetch from high-latency main RAM (~50-100 ns per miss) until $P_2$'s working set warms up the cache.
> 2. **Hot vs. Cold Cache:**
>    - A **hot cache** contains the actively referenced instructions and data of the currently running process, resulting in near-zero cache miss penalties.
>    - A **cold cache** has had its lines evicted or invalidated (due to intervening workloads or TLB flushes).
>    - Frequent context switches prevent processes from maintaining a hot cache, drastically reducing CPU IPC (Instructions Per Cycle) and wasting memory bus bandwidth.

> [!question] Quiz 3: Process State Transitions (Clips 57-58)
> In the 5-state process lifecycle (NEW, READY, RUNNING, WAITING/BLOCKED, TERMINATED):
> 1. Can a process transition directly from WAITING/BLOCKED to RUNNING? Explain why or why not.
> 2. What event triggers the transition from RUNNING to READY?

> [!success]- Answer
> 1. **No.** A waiting process that completes its I/O or receives a signal transitions from **WAITING to READY**, never directly to RUNNING. The CPU may already be occupied executing another process. The newly unblocked process must enter the scheduler's ready queue (or runqueue) and wait for the CPU scheduler to dispatch it.
> 2. **RUNNING to READY** is triggered by an **interrupt**:
>    - An architectural timer interrupt indicating the process has exhausted its time slice (quantum).
>    - A preemption event because a higher-priority process has entered the READY queue.

> [!question] Quiz 4: Process Creation Mechanics (Clips 60-61)
> In UNIX-like operating systems, process creation is decoupled into `fork()` and `exec()`.
> 1. Why does `fork()` return different values to the parent and child processes?
> 2. If a parent process terminates before its child process exits, what happens to the child process? Who reaps its exit status?

> [!success]- Answer
> 1. `fork()` returns **0 to the newly created child process** and returns the **child's PID (positive integer) to the parent process** (or -1 on failure). This allows the exact same code image to distinguish its role via conditional branching:
>    ```c
>    pid_t pid = fork();
>    if (pid == 0) { /* child executes here */ }
>    else if (pid > 0) { /* parent executes here */ }
>    ```
> 2. An orphaned process is immediately **reparented to the init process (PID 1)** on Linux (or `launchd` on macOS). Init periodically executes `wait()` / `waitpid()` in a background loop to reap terminating orphan processes and prevent them from remaining permanent zombies in the system process table.

> [!question] Quiz 5: CPU Bursts and Scheduling Responsibilities (Clips 65-66)
> Processes alternate between CPU execution bursts and I/O wait bursts.
> 1. How does the CPU scheduler distinguish between a CPU-bound process and an I/O-bound process?
> 2. Why do general-purpose interactive operating systems favor I/O-bound processes over CPU-bound processes?

> [!success]- Answer
> 1. **Observation of Quantum Expiration:**
>    - An **I/O-bound process** frequently yields the CPU voluntarily (via `read`, `write`, `poll`, or `sleep`) before its assigned scheduling time slice expires.
>    - A **CPU-bound process** consistently consumes its entire time slice until preempted by the hardware timer interrupt.
> 2. **Interactive Responsiveness:** I/O-bound tasks typically drive user interfaces, audio playback, or network communication. Scheduling them immediately when their I/O completes keeps interactive latency minimal. Because their CPU bursts are short, they quickly yield the CPU back to long-running throughput-oriented CPU-bound tasks with negligible degradation to overall compute throughput.

> [!question] Quiz 6: Shared Memory versus Message-Passing IPC (Clips 68-69)
> Compare Shared Memory IPC with Message-Passing IPC across:
> 1. Operating system involvement during ongoing data transfer.
> 2. Data copy overhead.
> 3. Synchronization requirements.

> [!success]- Answer
> 1. **OS Involvement:**
>    - **Message Passing (Pipes, Sockets, MQ):** Every message transfer requires kernel boundary crossings via system calls (`write()`, `send()`, `read()`, `recv()`). The kernel arbitrates every byte.
>    - **Shared Memory:** The OS is involved only during channel setup (`shm_open()`, `mmap()`). Subsequent data transfers occur via direct memory load and store CPU instructions without kernel intervention.
> 2. **Copy Overhead:**
>    - **Message Passing:** Incurs at least two data copies (User space of sender $\to$ Kernel buffer $\to$ User space of receiver).
>    - **Shared Memory:** Zero copy overhead between sender and receiver address spaces.
> 3. **Synchronization Requirements:**
>    - **Message Passing:** Implicitly synchronized by the kernel. If a reader attempts to read from an empty pipe, the kernel automatically blocks the reader.
>    - **Shared Memory:** Explicit application-level synchronization is mandatory. Cooperating processes must use shared mutexes, POSIX semaphores, or atomic flags to prevent race conditions and data corruption.

---

### Exercise 1: Process Tree

```bash
# Linux: Create a process tree and observe it
# Create a script that forks multiple levels deep
cat << 'EOF' > process_tree.c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    printf("Root: PID=%d\n", getpid());

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            printf("  Child %d: PID=%d, PPID=%d\n", i, getpid(), getppid());
            sleep(10);  // Keep alive for observation
            return 0;
        }
    }

    // In another terminal: pstree -p <root_pid>
    printf("Parent waiting... (run 'pstree -p %d' in another terminal)\n", getpid());
    for (int i = 0; i < 3; i++) wait(NULL);
    return 0;
}
EOF
gcc -o process_tree process_tree.c && ./process_tree
```

### Exercise 2: Zombie Process

```bash
# Create a zombie: child exits but parent doesn't wait
cat << 'EOF' > zombie.c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    pid_t pid = fork();
    if (pid == 0) {
        printf("Child (PID %d) exiting immediately\n", getpid());
        exit(0);  // Child exits
    }
    // Parent sleeps without calling wait()
    printf("Parent (PID %d) sleeping... child %d should be zombie\n",
           getpid(), pid);
    printf("Check with: ps aux | grep %d\n", pid);
    sleep(30);
    // Zombie exists for 30 seconds
    return 0;
}
EOF
gcc -o zombie zombie.c && ./zombie &
# In another terminal:
ps aux | grep Z
```

---

## 16. Key Takeaways

1. A **process** is a program in execution with its own address space, execution state, and OS metadata.
2. The **PCB** (Linux: `task_struct`, Windows: `EPROCESS`) stores everything the OS needs to manage a process.
3. **Context switches** save/restore PCB state; the indirect cost (cache/TLB pollution) dominates the direct cost.
4. Processes cycle through states: NEW -> READY -> RUNNING -> WAITING -> TERMINATED.
5. Linux creates processes via `fork()` (COW) + `exec()`; Windows uses `CreateProcess()`.
6. **CPU-bound** processes have long CPU bursts; **I/O-bound** processes have short CPU bursts and frequent I/O.
7. IPC comes in two flavors: **message-based** (kernel mediates every transfer) and **shared memory** (kernel sets up mapping, then steps aside).

---

## 17. The /proc Filesystem: Process Introspection Deep Dive

The `/proc` pseudo-filesystem is the primary interface for inspecting process internals on Linux.

```bash
# Every running process has a directory: /proc/<pid>/

# ── Identity and State ──
cat /proc/self/status             # Self = current shell process
# Name:   bash
# Umask:  0022
# State:  S (sleeping)
# Tgid:   1234                   (Thread Group ID = PID of main thread)
# Pid:    1234                   (Thread ID, same as Tgid for main thread)
# PPid:   1200                   (Parent PID)
# Uid:    1000  1000  1000  1000 (real, effective, saved, filesystem UID)
# Gid:    1000  1000  1000  1000
# VmPeak:  123456 kB             (Peak virtual memory size)
# VmSize:  120000 kB             (Current VM size)
# VmRSS:    45000 kB             (Resident set: pages in physical RAM)
# VmSwap:       0 kB             (Swapped out pages)
# Threads:       4               (Number of threads)
# voluntary_ctxt_switches:  1234 (yielded CPU intentionally, e.g., I/O)
# nonvoluntary_ctxt_switches: 56 (preempted by scheduler)

# ── Memory Map ──
cat /proc/self/maps
# address                      perms offset  dev   inode  pathname
# 55f4a8000000-55f4a8003000    r--p  00000000 08:01 123456 /usr/bin/bash
# 55f4a8003000-55f4a80c5000    r-xp  00003000 08:01 123456 /usr/bin/bash  ← text
# 55f4a80c5000-55f4a80f5000    r--p  000c5000 08:01 123456 /usr/bin/bash  ← rodata
# 55f4a80f7000-55f4a80fc000    rw-p  000f5000 08:01 123456 /usr/bin/bash  ← data+bss
# 55f4a8100000-55f4a8200000    rw-p  00000000 00:00 0      [heap]
# 7f1234000000-7f1234200000    rw-p  00000000 00:00 0      (anonymous: thread stack / mmap)
# 7f1234500000-7f1234680000    r-xp  00000000 08:01 789012 /usr/lib/libc.so.6
# 7ffce4000000-7ffce4021000    rw-p  00000000 00:00 0      [stack]
# 7ffce4ffe000-7ffce5000000    r--p  00000000 00:00 0      [vvar]  ← vDSO data
# 7ffce5000000-7ffce5002000    r-xp  00000000 00:00 0      [vdso]  ← virtual syscall page

# Decode perms: r=read, w=write, x=execute, p=private (COW), s=shared

# ── Detailed Memory Accounting ──
cat /proc/self/smaps_rollup
# Rss:               45000 kB    (pages in RAM)
# Pss:               35000 kB    (Proportional Share: shared pages / #sharers)
# Pss_Anon:          20000 kB    (PSS of anonymous/heap pages)
# Pss_File:          15000 kB    (PSS of file-backed pages)
# Shared_Clean:      12000 kB    (shared pages not dirtied)
# Shared_Dirty:          0 kB
# Private_Clean:      8000 kB    (private, file-backed)
# Private_Dirty:     25000 kB    (private, modified = unique to this process)
# Referenced:        44000 kB    (accessed recently)
# Swap:                  0 kB

# ── File Descriptors ──
ls -la /proc/self/fd/
# 0 -> /dev/pts/0     (stdin)
# 1 -> /dev/pts/0     (stdout)
# 2 -> /dev/pts/0     (stderr)
# 3 -> /path/to/open/file
# 4 -> socket:[12345]
# 5 -> pipe:[67890]

# Count open file descriptors
ls /proc/self/fd/ | wc -l

# File descriptor limits
cat /proc/self/limits
# Max open files            1024                 1048576              files

# ── Detailed per-FD info ──
cat /proc/self/fdinfo/3
# pos:    0                 (file offset)
# flags:  0100002           (O_RDWR | O_CLOEXEC)
# mnt_id: 25

# ── CPU and Scheduling ──
cat /proc/self/stat | awk '{print "utime="$14, "stime="$15, "priority="$18, "nice="$19, "threads="$20}'

# ── Task (thread) listing ──
ls /proc/self/task/
# Lists TIDs (Thread IDs) for all threads in this process

# ── cgroups membership ──
cat /proc/self/cgroup
# 0::/user.slice/user-1000.slice/session-2.scope

# ── Namespace IDs ──
ls -la /proc/self/ns/
# cgroup -> cgroup:[4026531835]
# ipc    -> ipc:[4026531839]
# mnt    -> mnt:[4026531840]
# net    -> net:[4026531992]
# pid    -> pid:[4026531836]
# user   -> user:[4026531837]
# uts    -> uts:[4026531838]

# ── IO Statistics ──
cat /proc/self/io
# rchar:  123456789    (bytes read from syscalls, incl. page cache hits)
# wchar:   87654321    (bytes written to syscalls)
# syscr:      12345    (read syscall count)
# syscw:       8765    (write syscall count)
# read_bytes:  45000000 (bytes actually fetched from storage)
# write_bytes: 30000000 (bytes actually sent to storage)
# cancelled_write_bytes: 0

# ── Kernel Stack Trace ──
cat /proc/self/stack
# Shows where the kernel is on behalf of this process (requires CONFIG_STACKTRACE)

# ── Environment Variables ──
cat /proc/self/environ | tr '\0' '\n' | head -10

# ── Executable Path ──
readlink /proc/self/exe
# /usr/bin/bash

# ── Command Line ──
cat /proc/self/cmdline | tr '\0' ' '
# bash
```

```bash
# ── Useful /proc one-liners for process forensics ──

# Find all processes sorted by RSS (memory usage)
ps aux --sort=-%mem | head -20

# Same using /proc directly (without ps)
for pid in /proc/[0-9]*/; do
    pid=$(basename "$pid")
    rss=$(awk '/VmRSS/{print $2}' "/proc/$pid/status" 2>/dev/null)
    name=$(awk '/Name/{print $2}' "/proc/$pid/status" 2>/dev/null)
    [ -n "$rss" ] && echo "$rss $pid $name"
done | sort -rn | head -20

# Find processes with most open files
for pid in /proc/[0-9]*/; do
    pid=$(basename "$pid")
    count=$(ls "/proc/$pid/fd" 2>/dev/null | wc -l)
    name=$(awk '/Name/{print $2}' "/proc/$pid/status" 2>/dev/null)
    [ "$count" -gt 50 ] && echo "$count $pid $name"
done | sort -rn | head -20

# Find all TCP connections for a process
cat /proc/<pid>/net/tcp
# Local address:port  Remote address:port  State
# Addresses are in hex, little-endian

# Human-readable via ss
ss -tnp | grep "pid=<pid>"

# Memory breakdown: RSS vs. VSZ vs. PSS
smem -t -k -p -s pss | head -20
# PSS is the fairest measure: shared libs counted proportionally
```

---

## 18. Process Tracing with strace, ltrace, and perf

```bash
# ── strace: Trace system calls ──

# Basic: trace all syscalls
strace ls /tmp

# Follow forks (critical for multi-process programs)
strace -f bash -c "ls | grep txt"

# Summary: count syscalls and time
strace -c -S time ls /tmp
# % time     seconds  usecs/call     calls    errors syscall
# ------ ----------- ----------- --------- --------- ----------------
#  45.00    0.000045           4        10           read
#  30.00    0.000030           5         6           write
#  15.00    0.000015           5         3           openat
# ...

# Filter specific syscalls
strace -e trace=open,read,write,close cat /etc/hostname
strace -e trace=network curl -s https://example.com >/dev/null
strace -e trace=memory ./my_program          # mmap, brk, mprotect, etc.
strace -e trace=process ./my_program         # fork, exec, wait, exit
strace -e trace=signal ./my_program          # kill, sigaction, rt_sigprocmask

# Timestamp each syscall
strace -T ls /tmp           # -T: show time spent in each syscall
strace -tt ls /tmp          # -tt: wall-clock timestamp (microseconds)
strace -r ls /tmp           # -r: relative timestamp (time since last syscall)

# Output to file (essential for large traces)
strace -o /tmp/trace.log -f my_server &

# Decode file descriptor paths
strace -y cat /etc/hostname
# read(3</etc/hostname>, "myhost\n", 131072) = 7

# Trace a running process
strace -p <pid>

# Inject faults (for testing error handling!)
strace -e inject=openat:error=ENOENT:when=3 ls /tmp
# Third openat() call will return "No such file or directory"

strace -e inject=read:delay_enter=100000 cat /etc/hostname
# Add 100ms delay before every read() - simulates slow disk

# ── ltrace: Trace library calls ──
ltrace ls /tmp
# malloc(128)      = 0x55a...
# opendir(".")     = 0x55a...
# readdir(0x55a..) = { "file1.txt", ... }

ltrace -c ls /tmp              # Summary with counts and time
ltrace -e malloc+free ./my_program  # Track only allocation calls

# ── perf trace: Low-overhead tracing ──
# perf trace is like strace but 5-10x lower overhead (uses eBPF)
sudo perf trace ls /tmp
sudo perf trace -p <pid> --duration 5  # Trace for 5 seconds
sudo perf trace -s ls /tmp             # Summary mode (like strace -c)

# Trace specific events
sudo perf trace -e 'syscalls:sys_enter_openat' -a -- sleep 5
# System-wide: trace every openat() for 5 seconds

# Compare overhead
time strace -c ls /tmp >/dev/null 2>&1
time perf trace -s ls /tmp >/dev/null 2>&1
# perf trace is typically 5-10x faster
```

---

## 19. Copy-on-Write (COW) Mechanics

```
fork() with COW:

BEFORE fork():
  Parent: Page Table → Physical Page A [ref=1] (writable)

AFTER fork():
  Parent: Page Table → Physical Page A [ref=2] (read-only!)
  Child:  Page Table → Physical Page A [ref=2] (read-only!)
  (Both PTEs now point to the SAME physical page, marked read-only)

Child writes to the page:
  1. CPU raises page fault (write to read-only page)
  2. Kernel checks: ref count > 1 → COW!
  3. Kernel allocates new Physical Page B
  4. Copies content: Page A → Page B
  5. Updates child PTE to point to Page B (writable)
  6. Decrements Page A ref count: [ref=1]
  7. Re-marks parent PTE as writable (only one ref now)
  8. Returns from fault handler; child retries the write → succeeds

Cost: ONE page fault + ONE page copy (4KB) per modified page.
If child never writes (or calls exec() immediately): ZERO copies!
```

```c
/* Demonstrate COW: observe physical memory sharing */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

/* Read RSS from /proc/self/status */
long get_rss_kb(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (!f) return -1;
    char line[256];
    long rss = 0;
    while (fgets(line, sizeof(line), f)) {
        if (sscanf(line, "VmRSS: %ld kB", &rss) == 1) break;
    }
    fclose(f);
    return rss;
}

int main(void) {
    /* Allocate and touch 100MB */
    size_t size = 100 * 1024 * 1024;
    char *data = malloc(size);
    memset(data, 'A', size);  /* Force physical allocation */
    printf("Parent RSS before fork: %ld KB\n", get_rss_kb());

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: check RSS immediately (shared via COW) */
        printf("  Child RSS after fork (COW, shared): %ld KB\n", get_rss_kb());

        /* Touch half the pages → triggers COW for 50MB */
        memset(data, 'B', size / 2);
        printf("  Child RSS after writing 50MB: %ld KB\n", get_rss_kb());

        /* Touch all pages → COW for remaining 50MB */
        memset(data + size / 2, 'C', size / 2);
        printf("  Child RSS after writing 100MB: %ld KB\n", get_rss_kb());

        free(data);
        _exit(0);
    }

    wait(NULL);
    printf("Parent RSS after child exits: %ld KB\n", get_rss_kb());
    free(data);
    return 0;
}
```

```bash
gcc -o cow_demo cow_demo.c && ./cow_demo
# Parent RSS before fork: ~102400 KB
# Child RSS after fork (COW, shared): ~102400 KB   ← NO new memory allocated!
# Child RSS after writing 50MB: ~153600 KB         ← 50MB new pages
# Child RSS after writing 100MB: ~204800 KB        ← All 100MB duplicated

# Observe COW faults
perf stat -e 'page-faults,minor-faults,major-faults' ./cow_demo
# minor-faults: ~25600 per 100MB (100MB / 4KB per page = 25600 faults)
```

---

## 20. Process Namespaces: Isolation Without VMs

Linux namespaces provide per-process resource isolation - the foundation of containers.

```bash
# ── Available namespace types ──
# pid:    Process ID isolation (child sees its own PID 1)
# net:    Network stack isolation (own interfaces, IPs, routes)
# mnt:    Mount point isolation (own filesystem view)
# uts:    Hostname isolation
# ipc:    IPC isolation (own message queues, semaphores)
# user:   UID/GID mapping (root inside container = unprivileged outside)
# cgroup: cgroup root isolation (own cgroup hierarchy)
# time:   (Linux 5.6+) clock_gettime isolation

# ── Create a new PID namespace ──
# unshare creates a new namespace and runs a command in it
sudo unshare --pid --fork --mount-proc bash
# Inside: ps aux shows only this shell and its children
ps aux
# PID 1 is the shell itself!
echo $$  # 1
exit

# ── Network namespace ──
sudo ip netns add testns                     # Create
sudo ip netns exec testns ip addr            # Only loopback, no eth0
sudo ip netns exec testns ping 8.8.8.8       # Fails: no route

# Create a veth pair to connect namespaces
sudo ip link add veth0 type veth peer name veth1
sudo ip link set veth1 netns testns
sudo ip addr add 10.0.0.1/24 dev veth0
sudo ip netns exec testns ip addr add 10.0.0.2/24 dev veth1
sudo ip link set veth0 up
sudo ip netns exec testns ip link set veth1 up
sudo ip netns exec testns ping 10.0.0.1      # Works!

sudo ip netns delete testns                  # Cleanup

# ── Mount namespace ──
sudo unshare --mount bash
mount -t tmpfs none /tmp      # This mount is only visible in this namespace
ls /tmp                       # Empty tmpfs
exit
ls /tmp                       # Original /tmp is intact

# ── User namespace (unprivileged containers!) ──
unshare --user --map-root-user bash
id   # uid=0(root) gid=0(root) - but ONLY inside this namespace!
# Cannot actually modify host files despite appearing as root

# ── Combine namespaces (mini-container) ──
sudo unshare --pid --fork --mount-proc --net --uts bash
hostname isolated-container
hostname    # "isolated-container"
ps aux      # Only this shell
ip addr     # Only loopback
exit
hostname    # Original hostname restored
```

```c
/* Create a namespace programmatically with clone() */
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)  /* 1MB stack for child */

static int child_func(void *arg) {
    printf("Child PID (inside new PID namespace): %d\n", getpid());
    printf("Hostname: ");
    sethostname("container", 9);
    char hostname[64];
    gethostname(hostname, sizeof(hostname));
    printf("%s\n", hostname);

    /* Mount proc for the new PID namespace */
    mount("proc", "/proc", "proc", 0, NULL);
    execlp("ps", "ps", "aux", NULL);
    return 1;
}

int main(void) {
    char *stack = malloc(STACK_SIZE);
    if (!stack) { perror("malloc"); return 1; }

    /* clone with new PID, UTS, and mount namespaces */
    pid_t child = clone(child_func,
                        stack + STACK_SIZE,  /* Stack grows down */
                        CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD,
                        NULL);
    if (child == -1) { perror("clone"); return 1; }

    printf("Parent: child PID in parent namespace: %d\n", child);
    waitpid(child, NULL, 0);
    free(stack);
    return 0;
}
```

---

## 21. Windows Process Internals: CreateProcess and EPROCESS

```c
/* Windows: CreateProcess deep dive */
#include <windows.h>
#include <stdio.h>

int main(void) {
    STARTUPINFOW si = { .cb = sizeof(si) };
    PROCESS_INFORMATION pi;

    /* CreateProcess stages:
     * 1. Open the executable file
     * 2. Create the Windows executive process object (EPROCESS)
     * 3. Create the initial thread (ETHREAD)
     * 4. Notify the Windows Subsystem (csrss.exe)
     * 5. Start execution of the initial thread
     * 6. In the context of the new process: initialize the address space
     *    (ntdll.dll → kernel32.dll → user entry point)
     */
    BOOL ok = CreateProcessW(
        L"C:\\Windows\\System32\\notepad.exe",  /* lpApplicationName */
        NULL,           /* lpCommandLine (can override) */
        NULL,           /* lpProcessAttributes (security) */
        NULL,           /* lpThreadAttributes (security) */
        FALSE,          /* bInheritHandles */
        CREATE_SUSPENDED,  /* dwCreationFlags: don't start yet */
        NULL,           /* lpEnvironment (inherit parent's) */
        NULL,           /* lpCurrentDirectory (inherit) */
        &si,            /* lpStartupInfo */
        &pi             /* lpProcessInformation (output) */
    );

    if (!ok) {
        printf("CreateProcess failed: %lu\n", GetLastError());
        return 1;
    }

    printf("Process ID:   %lu\n", pi.dwProcessId);
    printf("Thread ID:    %lu\n", pi.dwThreadId);
    printf("Process Handle: %p\n", pi.hProcess);
    printf("Thread Handle:  %p\n", pi.hThread);

    /* Process is suspended. Inspect it before starting. */

    /* Query process information */
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(pi.hProcess, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    printf("Working Set:  %zu KB\n", pmc.WorkingSetSize / 1024);
    printf("Private Bytes: %zu KB\n", pmc.PrivateUsage / 1024);

    /* Set CPU affinity: restrict to CPUs 0 and 1 */
    SetProcessAffinityMask(pi.hProcess, 0x3);

    /* Set priority */
    SetPriorityClass(pi.hProcess, ABOVE_NORMAL_PRIORITY_CLASS);

    /* Now resume the main thread */
    ResumeThread(pi.hThread);

    /* Wait for process to exit (with timeout) */
    DWORD result = WaitForSingleObject(pi.hProcess, 5000);  /* 5 second timeout */
    if (result == WAIT_TIMEOUT) {
        printf("Process still running after 5 seconds\n");
        TerminateProcess(pi.hProcess, 1);
    }

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    printf("Exit code: %lu\n", exit_code);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
```

```powershell
# PowerShell: Process inspection and management

# Detailed process information
Get-Process -Id <pid> | Format-List *

# Process tree (parent-child relationships)
Get-CimInstance Win32_Process | Select-Object ProcessId, ParentProcessId, Name |
    Sort-Object ParentProcessId | Format-Table -AutoSize

# Process creation with Start-Process
$proc = Start-Process -FilePath "notepad.exe" -PassThru -WindowStyle Minimized
$proc.Id          # PID
$proc.HasExited   # $false
$proc.Kill()

# Process command line (like /proc/<pid>/cmdline)
Get-CimInstance Win32_Process -Filter "ProcessId = $pid" |
    Select-Object CommandLine

# Process modules (loaded DLLs)
Get-Process -Id $pid | Select-Object -ExpandProperty Modules

# Process handles (file descriptors equivalent)
# Requires SysInternals handle.exe
handle.exe -p $pid

# Job Objects: Windows equivalent of cgroups
# Limit a group of processes together
$job = [System.Diagnostics.Process]::Start("cmd.exe")
# PowerShell doesn't natively support Job Objects; use P/Invoke or Windows API

# WMI: Watch for new process creation
Register-WmiEvent -Class Win32_ProcessStartTrace -Action {
    $e = $Event.SourceEventArgs.NewEvent
    Write-Host "New process: $($e.ProcessName) PID=$($e.ProcessID)"
}
```

```
Windows EPROCESS Structure (simplified):

EPROCESS {
    KPROCESS            Pcb;            // Kernel process block
    EX_PUSH_LOCK        ProcessLock;
    LARGE_INTEGER       CreateTime;
    LARGE_INTEGER       ExitTime;
    EX_RUNDOWN_REF      RundownProtect;
    HANDLE              UniqueProcessId;
    LIST_ENTRY          ActiveProcessLinks;  // Linked list of all processes
    SIZE_T              PeakVirtualSize;
    SIZE_T              VirtualSize;
    MMSUPPORT_FULL      Vm;             // Virtual memory manager state
    LIST_ENTRY          ThreadListHead; // All threads in this process
    ULONG               ActiveThreads;
    ACCESS_MASK         GrantedAccess;
    PEB                 *Peb;           // Process Environment Block (user-mode accessible)
    SECURITY_PORT       *SecurityPort;
    ULONG               ExitStatus;
    PPEB_LDR_DATA       Ldr;            // Loader data (DLL list)
    // ... 800+ fields total in Windows 11
}
```

---

## 22. cgroup Process Resource Control

```bash
# cgroup v2: unified hierarchy for resource control

# Show current cgroup hierarchy
mount | grep cgroup2
# cgroup2 on /sys/fs/cgroup type cgroup2 (rw,nosuid,nodev,noexec,relatime)

# Available controllers
cat /sys/fs/cgroup/cgroup.controllers
# cpu io memory pids

# Create a resource-limited group
sudo mkdir /sys/fs/cgroup/myapp

# Enable controllers for the group
echo "+cpu +memory +pids +io" | sudo tee /sys/fs/cgroup/myapp/cgroup.subtree_control

# ── CPU Limits ──
# cpu.max: quota period (microseconds)
echo "100000 1000000" | sudo tee /sys/fs/cgroup/myapp/cpu.max
# = 100ms per 1000ms = 10% of one CPU

# cpu.weight: proportional share (1-10000, default 100)
echo "50" | sudo tee /sys/fs/cgroup/myapp/cpu.weight
# Half the default share

# ── Memory Limits ──
echo "256M" | sudo tee /sys/fs/cgroup/myapp/memory.max   # Hard limit (OOM kill)
echo "128M" | sudo tee /sys/fs/cgroup/myapp/memory.high  # Soft limit (throttle)

# Memory usage
cat /sys/fs/cgroup/myapp/memory.current
cat /sys/fs/cgroup/myapp/memory.stat
# anon:    134217728   (anonymous pages: heap, stack)
# file:     67108864   (file-backed pages: mmap'd files)
# slab:      1048576   (kernel slab allocator)
# pgfault:  12345      (total page faults)
# pgmajfault:   23     (major faults = disk reads)

# ── PID Limits (prevent fork bombs) ──
echo "100" | sudo tee /sys/fs/cgroup/myapp/pids.max
cat /sys/fs/cgroup/myapp/pids.current   # Current process count

# ── I/O Limits ──
# Get device major:minor
lsblk -o NAME,MAJ:MIN
# Limit to 50MB/s read, 20MB/s write for device 8:0
echo "8:0 rbps=52428800 wbps=20971520" | sudo tee /sys/fs/cgroup/myapp/io.max

# ── Move a process into the cgroup ──
echo $$ | sudo tee /sys/fs/cgroup/myapp/cgroup.procs

# Verify
cat /proc/$$/cgroup
# 0::/myapp

# ── Monitor resource usage ──
cat /sys/fs/cgroup/myapp/cpu.stat
# usage_usec:    1234567    (total CPU time)
# user_usec:     1000000    (user-mode CPU)
# system_usec:    234567    (kernel-mode CPU)
# nr_periods:       1234    (scheduling periods)
# nr_throttled:       56    (times throttled)
# throttled_usec:  78900    (total time throttled)

# ── Pressure Stall Information (PSI) ──
cat /sys/fs/cgroup/myapp/cpu.pressure
# some avg10=2.50 avg60=1.20 avg300=0.80 total=456789
# "some" = at least one task waiting for CPU

cat /sys/fs/cgroup/myapp/memory.pressure
cat /sys/fs/cgroup/myapp/io.pressure

# ── Freeze/Thaw a cgroup (pause all processes) ──
echo 1 | sudo tee /sys/fs/cgroup/myapp/cgroup.freeze
cat /sys/fs/cgroup/myapp/cgroup.events
# frozen 1

echo 0 | sudo tee /sys/fs/cgroup/myapp/cgroup.freeze  # Resume

# ── Cleanup ──
# Move all processes out first
cat /sys/fs/cgroup/myapp/cgroup.procs | while read pid; do
    echo $pid | sudo tee /sys/fs/cgroup/cgroup.procs
done
sudo rmdir /sys/fs/cgroup/myapp
```

---

## 23. Process Accounting and Audit

```bash
# ── Process accounting: track every process start/stop ──
# Enable BSD-style process accounting
sudo accton /var/log/pacct

# Run some commands...
ls /tmp
cat /etc/hostname

# View accounting records
lastcomm --file /var/log/pacct
# ls          shreejit  pts/0    0.00 secs Wed Sep 26 16:00
# cat         shreejit  pts/0    0.00 secs Wed Sep 26 16:00

# Summary by command or user
sa -m --file /var/log/pacct   # By user
sa -c --file /var/log/pacct   # By command

# Disable accounting
sudo accton off

# ── auditd: Security-focused process tracking ──
# Watch all process executions
sudo auditctl -a always,exit -F arch=b64 -S execve -k process_exec

# Watch specific binary
sudo auditctl -w /usr/bin/sudo -p x -k sudo_usage

# Search audit log
sudo ausearch -k process_exec --start recent | head -40
# type=SYSCALL ... comm="ls" exe="/usr/bin/ls" key="process_exec"
# type=EXECVE ... a0="ls" a1="/tmp"

# Report: summarize audit events
sudo aureport --executable --summary

# ── loginctl: Session tracking (systemd) ──
loginctl list-sessions
loginctl session-status 2
loginctl show-session 2 -p State,Leader,Service

# ── systemd-cgls: Show cgroup tree ──
systemd-cgls
# Shows the full process-cgroup hierarchy as a tree
# ├─user.slice
# │ └─user-1000.slice
# │   ├─session-2.scope
# │   │ ├─1234 bash
# │   │ └─5678 vim file.txt

# ── systemd-cgtop: Live cgroup resource usage ──
systemd-cgtop
# Control Group          Tasks  %CPU  Memory  Input/s  Output/s
# /                        345  12.3    4.2G    1.5M     512K
# /user.slice              120   8.5    2.1G    1.2M     256K
```

---

## 24. macOS Process Architecture: Mach Tasks, BSD Processes, and posix_spawn

### Mach Task versus BSD Process

macOS (built on the XNU hybrid kernel) implements a dual-layer abstraction for executing programs:

```
+=============================================================================+
| macOS USER PROCESS                                                          |
+=============================================================================+
                                      |
              +-----------------------+-----------------------+
              |                                               |
              v                                               v
+-------------------------------+             +-------------------------------+
| BSD Layer (proc_t)            |             | Mach Layer (task_t)           |
| - POSIX Process ID (PID)      |             | - Mach Task Port (task_self)  |
| - Parent PID (PPID)           |             | - Virtual Address Space (VM)  |
| - User / Group Credentials    |             | - Mach Port Rights & IPC      |
| - File Descriptor Table       |             | - Thread Containers (threads) |
| - POSIX Signal Handlers       |             | - Exception Ports             |
+-------------------------------+             +-------------------------------+
              |                                               |
              +-----------------------+-----------------------+
                                      v
+=============================================================================+
| XNU Kernel Core (Address Space, Scheduling, Memory, Hardware Access)        |
+=============================================================================+
```

1. **Mach Task (`task_t`):** The low-level resource allocation unit. A task possesses no execution state of its own; it is purely a passive container holding a virtual memory map, a set of Mach port capabilities (rights to send/receive messages), and an array of Mach threads.
2. **BSD Process (`proc_t`):** Wraps the Mach task to provide standard POSIX compliance. The BSD layer manages the integer PID, parent-child process tree, process group, user credentials (UID/GID), file descriptor table, and POSIX signal masks.

### Why `posix_spawn()` is Preferred Over `fork()` on macOS

On macOS, calling `fork()` in multi-threaded processes that link against system frameworks (CoreFoundation, Cocoa, GCD) is dangerous:
- `fork()` duplicates only the calling thread into the child process. Any lock, mutex, or runtime queue held by another thread in the parent remains permanently locked in the child, causing immediate deadlock.
- In macOS 12+ (Monterey and later), calling `fork()` without an immediate `exec()` in GUI applications or applications initializing Objective-C runtime classes throws a fatal warning or aborts execution.
- `posix_spawn()` resolves this by combining process creation and binary execution into a single atomic kernel operation. The kernel initializes a clean Mach task and loads the target Mach-O binary directly, avoiding unnecessary page table duplication.

```c
// Compiling on macOS: clang -Wall -O2 spawn_demo.c -o spawn_demo
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>

extern char **environ;

int main(void) {
    pid_t pid;
    char *argv[] = {"/bin/ls", "-lh", "/tmp", NULL};
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attr;

    posix_spawn_file_actions_init(&actions);
    posix_spawnattr_init(&attr);

    // Spawn child process atomically
    int status = posix_spawn(&pid, "/bin/ls", &actions, &attr, argv, environ);
    if (status != 0) {
        perror("posix_spawn failed");
        return 1;
    }

    printf("Spawned child PID: %d on macOS\n", pid);

    // Wait for child termination
    waitpid(pid, &status, 0);
    printf("Child process %d exited with status %d\n", pid, WEXITSTATUS(status));

    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    return 0;
}
```

### macOS Process Introspection Commands

```bash
# macOS: Inspect virtual memory map of a process (resident, dirty, purgeable pages)
vmmap -summary <PID>

# macOS: Sample call stacks across all threads in a running process for 5 seconds
sample <PID> 5 -file /tmp/process_sample.txt

# macOS: Detect dynamic memory leaks in a running process
leaks <PID>

# macOS: Inspect process launch supervision and services managed by launchd (PID 1)
launchctl list | head -n 20

# macOS: Inspect Mach task port and thread counts via proc_pidinfo
sudo dtrace -n 'syscall::proc_info:entry { printf("PID %d queried proc_info", pid); }'
```

---

**Previous:** [P1L2: Introduction to Operating Systems](../Part-1-Architecture/P1L2-Introduction-to-Operating-Systems.md)
**Next:** [P2L2: Threads and Concurrency](P2L2-Threads-and-Concurrency.md)


