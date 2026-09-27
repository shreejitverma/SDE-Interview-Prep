---
type: concept
track: [sde]
level: advanced
status: active
last_reviewed: 2026-09-27
sources:
  - "Georgia Tech CS 6200: Introduction to Operating Systems (GIOS)"
  - "College of Computing, Georgia Institute of Technology"
---

# P1L1: Course Overview & Logistics

> In-depth course overview for Georgia Tech CS 6200 (Introduction to Operating Systems), detailing curriculum structure, instructional staff, technical prerequisites, project architecture, grading schemes, and development environments.

---

## Table of Contents

1. [Course Preview](#1-course-preview)
2. [Welcome to CS 6200](#2-welcome-to-cs-6200)
3. [Instructional Team and Course Staff](#3-instructional-team-and-course-staff)
   - [Instructor](#instructor)
   - [Udacity Course Developers](#udacity-course-developers)
   - [Teaching Assistants (TAs)](#teaching-assistants-tas)
   - [Communication and Office Hours](#communication-and-office-hours)
4. [Prerequisites and Technical Expectations](#4-prerequisites-and-technical-expectations)
   - [Core Systems Competencies](#core-systems-competencies)
   - [Required C Programming Skills](#required-c-programming-skills)
   - [Debugging and Profiling Tooling](#debugging-and-profiling-tooling)
5. [Course Structure and 16-Module Curriculum](#5-course-structure-and-16-module-curriculum)
   - [Part 1: Architecture and OS Basics](#part-1-architecture-and-os-basics)
   - [Part 2: Process and Thread Management](#part-2-process-and-thread-management)
   - [Part 3: Resource Management and Communication](#part-3-resource-management-and-communication)
   - [Part 4: Distributed Systems](#part-4-distributed-systems)
6. [Projects and Assessments](#6-projects-and-assessments)
   - [Project 1: Multithreaded Web Server (Getfile Protocol)](#project-1-multithreaded-web-server-getfile-protocol)
   - [Project 2: Inter-Process Communication (Shared Memory Proxy)](#project-2-inter-process-communication-shared-memory-proxy)
   - [Project 3: Shared Memory Cache Proxy](#project-3-shared-memory-cache-proxy)
   - [Project 4: Distributed File System (gRPC / Sun RPC)](#project-4-distributed-file-system-grpc--sun-rpc)
   - [Exams and Quizzes](#exams-and-quizzes)
7. [Development Environments and Build Infrastructure](#7-development-environments-and-build-infrastructure)
   - [Linux Development (Canonical Environment)](#linux-development-canonical-environment)
   - [Windows Subsystem for Linux (WSL2) Setup](#windows-subsystem-for-linux-wsl2-setup)
   - [Sanitizers and Memory Safety Checks](#sanitizers-and-memory-safety-checks)
8. [Course Summary and Success Strategies](#8-course-summary-and-success-strategies)

---

## 1. Course Preview

Georgia Tech CS 6200 (Introduction to Operating Systems, or GIOS) serves as the foundational graduate systems course in the College of Computing.
The course bridges hardware mechanisms and high-level software abstractions.
Modern operating systems are among the most intricate software artifacts ever engineered.
They balance hardware multiplexing, absolute resource protection, concurrency control, and low-latency abstractions.

Operating systems are fundamentally about arbitration and abstraction.
Arbitration resolves resource contention across competing workloads fairly, securely, and efficiently.
Abstraction hides hardware idiosyncrasies behind uniform, predictable programming interfaces like files, processes, and virtual memory.
Understanding how modern kernels implement these principles is essential for designing scalable distributed backends, low-latency trading engines, database engines, and virtualization stacks.

```
+-------------------------------------------------------------------------+
|                          Application Workloads                          |
|         (Web Servers, Databases, Distributed Compute, Runtimes)         |
+-------------------------------------------------------------------------+
                                    |
                            [ POSIX / Win32 ]  System Calls (traps/syscall)
                                    v
+-------------------------------------------------------------------------+
|                        Kernel Subsystems (GIOS)                         |
|  +--------------------+  +--------------------+  +--------------------+ |
|  | Process & Threads  |  | Virtual Memory &   |  | I/O & Filesystems  | |
|  | (P2L1-L5, P3L1)    |  | Allocators (P3L2)  |  | (P3L5, P4L2)       | |
|  +--------------------+  +--------------------+  +--------------------+ |
|  +--------------------+  +--------------------+  +--------------------+ |
|  | Synchronization    |  | IPC Subsystems     |  | Virtualization &   | |
|  | & Atomics (P3L4)   |  | (P3L3)             |  | Hypervisors (P3L6) | |
|  +--------------------+  +--------------------+  +--------------------+ |
|  +--------------------------------------------------------------------+ |
|  | Distributed Systems: RPC & Distributed Shared Memory (P4L1, P4L3)  | |
|  +--------------------------------------------------------------------+ |
+-------------------------------------------------------------------------+
                                    |
                         [ Architectural Interface ]
                                    v
+-------------------------------------------------------------------------+
|                       Hardware Architecture (x86_64)                    |
|   CPUs, Caches (L1/L2/L3), MMU, TLB, Interrupt Controllers, Bus, PCIe   |
+-------------------------------------------------------------------------+
```

---

## 2. Welcome to CS 6200

CS 6200 covers classical principles and modern implementations of operating systems.
Students analyze design trade-offs between monolithic kernels, microkernels, and hybrid platforms.
Every conceptual module reinforces abstract theory with direct systems programming in C on POSIX Linux and Windows platforms.

The course emphasizes rigorous mechanical understanding over surface-level intuition.
Students write multi-threaded servers, concurrent synchronization primitives, shared-memory IPC protocols, and distributed RPC services.
Grading and feedback rely on automated autograders running inside isolated environments that subject student solutions to heavy stress, race condition fuzzing, and memory leak analysis.

---

## 3. Instructional Team and Course Staff

The instructional team consists of faculty leadership, course developers, and an extensive staff of graduate teaching assistants.

### Instructor

| Role | Name | Email | Institution / Dept |
|------|------|-------|--------------------|
| Professor | **Ada Gavrilovska** | [ada@cc.gatech.edu](mailto:ada@cc.gatech.edu) | Georgia Tech, School of Computer Science |

Ada Gavrilovska is an Associate Professor in the School of Computer Science at Georgia Tech.
Her research interests span operating systems, virtualization, high-performance computing, and cloud systems infrastructure.
She leads systems research at the Center for Experimental Research in Computer Systems (CERCS).

### Udacity Course Developers

The course content was developed in collaboration with Udacity:

| Name | Email | Role |
|------|-------|------|
| **Jarrod Parkes** | [jarrod@udacity.com](mailto:jarrod@udacity.com) | Course Developer |
| **Charles Brubaker** | [charles.brubaker@knowlabs.com](mailto:charles.brubaker@knowlabs.com) | Course Developer |
| **Ben Gardner** | [benjamin@udacity.com](mailto:benjamin@udacity.com) | Course Developer |

### Teaching Assistants (TAs)

The TA staff guides students through projects, facilitates discussion forums, and conducts interactive office hours.

#### Head Teaching Assistants

| Name | Role | Email |
|------|------|-------|
| **Tony Mason** | Head TA | [fsgeek@gatech.edu](mailto:fsgeek@gatech.edu) |
| **Vijayalakshmi (VJ) Parthiban** | Head TA | [vijayalakshmi.parthiban@gatech.edu](mailto:vijayalakshmi.parthiban@gatech.edu) |
| **Tho Ha** | Head TA | [Tho.Ha@gatech.edu](mailto:Tho.Ha@gatech.edu) |
| **Ioan G. Istrate** | Head TA | [iistrate3@gatech.edu](mailto:iistrate3@gatech.edu) |

#### Graduate Teaching Assistants

| Name | Role | Email |
|------|------|-------|
| **Alex (Poly) Kim** | TA | [akim618@gatech.edu](mailto:akim618@gatech.edu) |
| **Cherry Wang** | TA | [bwang647@gatech.edu](mailto:bwang647@gatech.edu) |
| **Cody Luu** | TA | [cluu30@gatech.edu](mailto:cluu30@gatech.edu) |
| **Dax Alessandra Delvira** | TA | [dax@gatech.edu](mailto:dax@gatech.edu) |
| **Daniel Sullivan** | TA | [danielsullivan@gatech.edu](mailto:danielsullivan@gatech.edu) |
| **Debajyoti Banerjee** | TA | [debajyoti.banerjee@gatech.edu](mailto:debajyoti.banerjee@gatech.edu) |
| **Dhruv Garg** | TA | [dgarg39@gatech.edu](mailto:dgarg39@gatech.edu) |
| **Eric Michael Gregori** | TA | [egregori3@gatech.edu](mailto:egregori3@gatech.edu) |
| **Eric O'Brien** | TA | [eobrien31@gatech.edu](mailto:eobrien31@gatech.edu) |
| **Evie Vanderveer** | TA | [ederveer3@gatech.edu](mailto:ederveer3@gatech.edu) |
| **Jesse Hall** | TA | [jhall376@gatech.edu](mailto:jhall376@gatech.edu) |
| **Jiaqi Deng** | TA | [jiaqi.deng@gatech.edu](mailto:jiaqi.deng@gatech.edu) |
| **Mitchell Flax** | TA | [mflax3@gatech.edu](mailto:mflax3@gatech.edu) |
| **Nikola Maruszewski** | TA | [nikola@gatech.edu](mailto:nikola@gatech.edu) |
| **Phillip Wilson Moore** | TA | [pwm@gatech.edu](mailto:pwm@gatech.edu) |
| **Richard William Suhr** | TA | [rsuhr3@gatech.edu](mailto:rsuhr3@gatech.edu) |
| **Robert Bischoff** | TA | [rbischoff3@gatech.edu](mailto:rbischoff3@gatech.edu) |
| **Seyed Nouraie** | TA | [snouraie3@gatech.edu](mailto:snouraie3@gatech.edu) |
| **Shauna Hyppolite** | TA | [shyppolite3@gatech.edu](mailto:shyppolite3@gatech.edu) |
| **Suraj Mallenahalli Shivu** | TA | [sshivu3@gatech.edu](mailto:sshivu3@gatech.edu) |
| **Vijay Thurimella** | TA | [vthurimella@gatech.edu](mailto:vthurimella@gatech.edu) |
| **Peter Jaglom** | TA | [pjaglom3@gatech.edu](mailto:pjaglom3@gatech.edu) |
| **Jonathan Ramirez** | TA | [jramirez311@gatech.edu](mailto:jramirez311@gatech.edu) |

### Communication and Office Hours

- **Ed Discussion / Canvas:** Primary forum for all technical questions, project clarifications, and errata.
- **Office Hours:** Conducted virtually throughout each week by the Head TAs and TAs covering conceptual reviews and debugging methodologies.
- **Direct TA Email:** Reserved for private administrative matters, student accommodations, and sensitive grading questions.

---

## 4. Prerequisites and Technical Expectations

CS 6200 is an intensive systems programming course with strict prerequisite expectations.
Students are expected to enter the course with fluency in the C programming language and familiarity with basic computer organization.

### Core Systems Competencies

1. **Computer Organization:** Memory hierarchy (registers, L1/L2/L3 caches, main memory, disk storage), CPU execution cycles, instruction sets, stack frames, and hardware interrupts.
2. **Data Structures:** Linked lists, circular ring buffers, hash tables, balanced search trees, and bitmasks.
3. **Discrete Math & Logic:** Boolean algebra, bitwise operations (`&`, `|`, `^`, `~`, `<<`, `>>`), and algorithmic complexity analysis.

### Required C Programming Skills

C is the implementation language for all course assignments.
Deficiencies in C syntax and semantics directly hinder progress on systems projects.
Students must master the following idioms:

- **Explicit Memory Allocation:** Managing lifetimes with `malloc()`, `calloc()`, `realloc()`, and `free()`.
- **Pointer Arithmetic and Indirection:** Pointer casting, void pointers (`void *`), double pointers (`void **`), and function pointers for callbacks.
- **Struct Alignment and Padding:** Memory layout, padding bytes inserted by the compiler, `sizeof`, and `offsetof`.
- **POSIX API Semantics:** Error checking with `errno`, return code validation for system calls (`fork`, `execvp`, `open`, `read`, `write`, `mmap`).
- **String and Buffer Safety:** Avoiding buffer overflows by using `snprintf`, `strncat`, and bounds-checked pointer indexing rather than legacy unsafe functions like `strcpy` or `gets`.

```c
// Example: Safe dynamic buffer allocation and pointer manipulation in C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    size_t capacity;
    size_t length;
    char *data;
} dynamic_buffer_t;

dynamic_buffer_t *buffer_create(size_t initial_cap) {
    dynamic_buffer_t *buf = malloc(sizeof(dynamic_buffer_t));
    if (!buf) return NULL;
    
    buf->data = malloc(initial_cap);
    if (!buf->data) {
        free(buf);
        return NULL;
    }
    buf->capacity = initial_cap;
    buf->length = 0;
    return buf;
}

void buffer_destroy(dynamic_buffer_t *buf) {
    if (!buf) return;
    free(buf->data);
    free(buf);
}
```

### Debugging and Profiling Tooling

Modern systems development requires mastering non-interactive and interactive diagnostic tools:

| Tool | Purpose | Primary Commands / Invocations |
|------|---------|--------------------------------|
| **GDB** | Interactive breakpoint debugging and backtraces | `gdb ./program`, `run`, `bt`, `info threads`, `thread apply all bt` |
| **Valgrind** | Heap memory corruption and memory leak detection | `valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./program` |
| **AddressSanitizer (ASan)** | Fast compiler instrumentation for buffer overflows and use-after-free | `gcc -fsanitize=address -g -O1 source.c -o program` |
| **ThreadSanitizer (TSan)** | Data race and dead-lock detection in concurrent code | `gcc -fsanitize=thread -g -O1 -pthread source.c -o program` |
| **strace** | Tracing OS system calls and signal dispatches | `strace -f -e trace=read,write,mmap,futex -o trace.log ./program` |

---

## 5. Course Structure and 16-Module Curriculum

The curriculum spans four main areas totaling 16 comprehensive modules:

```
CS 6200 GIOS Architecture
├── Part 1: Architecture & OS Basics
│   ├── P1L1: Course Overview & Logistics (this document)
│   └── P1L2: Introduction to Operating Systems
├── Part 2: Process & Thread Management
│   ├── P2L1: Processes and Process Management
│   ├── P2L2: Threads and Concurrency
│   ├── P2L3: Threads Case Study: PThreads
│   ├── P2L4: Thread Design Considerations
│   └── P2L5: Thread Performance Considerations
├── Part 3: Resource Management & Communication
│   ├── P3L1: Scheduling
│   ├── P3L2: Memory Management
│   ├── P3L3: Inter-Process Communication (IPC)
│   ├── P3L4: Synchronization Constructs
│   ├── P3L5: I/O Management
│   └── P3L6: Virtualization
└── Part 4: Distributed Systems
    ├── P4L1: Remote Procedure Calls (RPC)
    ├── P4L2: Distributed File Systems (DFS)
    └── P4L3: Distributed Shared Memory (DSM)
```

### Part 1: Architecture and OS Basics
- **P1L1: Course Overview:** Course logistics, prerequisites, staff, and environment setup.
- **P1L2: Introduction to Operating Systems:** Direct execution, dual-mode hardware enforcement (Ring 0 vs. Ring 3), system call boundary crossings, trap handlers, and kernel structural paradigms (monolithic, microkernel, exokernel, hybrid).

### Part 2: Process and Thread Management
- **P2L1: Processes & Process Management:** Address space layout, Process Control Blocks (PCBs), context switching mechanics, and process life cycle transitions.
- **P2L2: Threads & Concurrency:** Execution contexts, shared versus private state, mutual exclusion, condition variables, and Mesa versus Hoare semantics.
- **P2L3: Threads Case Study - PThreads:** POSIX thread management API, mutexes, condition variables, thread-safe producer-consumer patterns, and error handling.
- **P2L4: Thread Design Considerations:** User-Level Threads (ULT) versus Kernel-Level Threads (KLT), threading models (1:1, N:1, M:N), scheduler activations, and Linux `clone()` mechanics.
- **P2L5: Thread Performance Considerations:** Amdahl's Law, synchronization overhead, memory cache coherence, false sharing, CPU pinning, and worker pool architectures.

### Part 3: Resource Management and Communication
- **P3L1: Scheduling:** Preemptive versus non-preemptive scheduling, FCFS, SJF, SRTF, Round Robin, priority inversion, priority inheritance, MLFQ, and Linux CFS.
- **P3L2: Memory Management:** Virtual memory, multi-level page tables, TLBs, inverted page tables, page replacement policies (FIFO, LRU, Clock), and Linux buddy/slab allocators.
- **P3L3: Inter-Process Communication:** Pipe buffers, message queues, shared memory with `shmget`/`mmap`, POSIX signals, and Unix domain sockets.
- **P3L4: Synchronization Constructs:** Hardware atomic instructions (Test-and-Set, Compare-and-Swap, LL/SC), spinlocks, ticket locks, queue-based MCS locks, semaphores, and Read-Copy-Update (RCU).
- **P3L5: I/O Management:** Hardware device registers, Direct Memory Access (DMA), interrupt vs. polling dispatch, disk scheduling algorithms (SSTF, SCAN, C-SCAN), SSD flash translation layers (FTL), and POSIX VFS.
- **P3L6: Virtualization:** Popek-Goldberg virtualization requirements, trap-and-emulate, x86 hardware assistance (Intel VT-x, AMD-V), Extended Page Tables (EPT), shadow page tables, and containerization via Linux namespaces and cgroups.

### Part 4: Distributed Systems
- **P4L1: Remote Procedure Calls (RPC):** Client/server stubs, Interface Definition Languages (IDL), serialization, XDR, Protocol Buffers, and gRPC streaming.
- **P4L2: Distributed File Systems (DFS):** Stateless versus stateful file servers, NFS caching and consistency semantics, AFS tokenized callbacks, and scale-out cluster architectures (GFS, HDFS, Ceph).
- **P4L3: Distributed Shared Memory (DSM):** Page-based DSM, strict and sequential consistency, release consistency, lazy release consistency (TreadMarks), directory coherence protocols, and false sharing mitigation.

---

## 6. Projects and Assessments

The course requires completing four major systems programming projects.
Each project implements a real-world systems architecture from scratch in C or C++.

```
Project Pipeline Timeline
+--------------------------------------------------------------------------+
| Project 1: Multi-threaded Web Server (Getfile Protocol)                  |
| Focus: PThreads, Mutexes, Condition Variables, Sockets, Thread Pools     |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| Project 2: Inter-Process Communication (Shared Memory Proxy)             |
| Focus: POSIX Shared Memory, Ring Buffers, Semaphore Sync, POSIX IPC     |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| Project 3: Shared Memory Web Cache Proxy                                 |
| Focus: Cache Replacement (LRU), Reader-Writer Locks, IPC Benchmarking    |
+--------------------------------------------------------------------------+
                                     |
                                     v
+--------------------------------------------------------------------------+
| Project 4: Distributed File System (gRPC / Sun RPC)                      |
| Focus: RPC Stubs, Protobuf Serializer, File Chunking, Network Resilience |
+--------------------------------------------------------------------------+
```

### Project 1: Multithreaded Web Server (Getfile Protocol)
- **Objective:** Build a concurrent HTTP-like server and client implementing a custom transfer protocol (`GETFILE`).
- **Core Mechanics:** BSD socket programming (`socket`, `bind`, `listen`, `accept`), POSIX thread pool architecture, bounded task queues, mutexes, and condition variables.
- **Key Challenges:** Eliminating thread synchronization bottlenecks, avoiding deadlocks, preventing memory leaks, and managing graceful shutdown.

### Project 2: Inter-Process Communication (Shared Memory Proxy)
- **Objective:** Decouple request fetching and file processing across independent OS processes using shared memory channels.
- **Core Mechanics:** POSIX shared memory (`shm_open`, `ftruncate`, `mmap`), POSIX semaphores (`sem_open`, `sem_wait`, `sem_post`), and lock-free circular ring buffers.
- **Key Challenges:** Memory fence enforcement, synchronized shared buffer state, clean cleanup of IPC resources on abnormal termination (`SIGINT`/`SIGTERM`).

### Project 3: Shared Memory Cache Proxy
- **Objective:** Augment the IPC proxy architecture with an in-memory cache shared across worker processes.
- **Core Mechanics:** Dynamic shared memory allocation, cache eviction policies (Least Recently Used - LRU), and reader-writer synchronization.
- **Key Challenges:** Multi-process concurrency control on metadata structures without corrupting the global cache state.

### Project 4: Distributed File System (gRPC / Sun RPC)
- **Objective:** Build a network-transparent distributed file service supporting remote read, write, and directory lookup operations.
- **Core Mechanics:** Google Protocol Buffers, gRPC asynchronous service handlers, remote error code translation, and file block streaming.
- **Key Challenges:** Network failure handling, timeout management, chunked transfers for large binaries, and server-side concurrency.

### Exams and Quizzes

- **Midterm Examination:** Covers Part 1 and Part 2 (Architecture, Process Model, Threads, Concurrency, PThreads API, Thread Scheduling).
- **Final Examination:** Comprehensive exam with heavy emphasis on Part 3 and Part 4 (Memory Management, IPC, Hardware Atomics, Synchronization, I/O, Virtualization, RPC, DFS, and DSM).
- **Video Quizzes:** Embedded conceptual checks throughout each lesson testing invariant understanding and edge-case handling.

---

## 7. Development Environments and Build Infrastructure

Reproducibility is essential in systems programming.
Code that appears to function on one platform may fail under high load due to memory layout differences, cache line sizes, or scheduling jitter.

### Linux Development (Canonical Environment)

The official grading platform is Ubuntu Linux (x86_64).
Projects should be compiled with strict compiler warnings:

```bash
# Recommended build flags for systems development
gcc -Wall -Wextra -Werror -pedantic -std=gnu11 -g -O2 -pthread source.c -o program
```

Explanation of required flags:
- `-Wall -Wextra`: Enables comprehensive compiler warnings for suspicious constructs.
- `-Werror`: Treats all warnings as fatal errors to prevent subtle defects.
- `-pedantic`: Demands strict ISO C compliance and rejects non-standard extensions.
- `-std=gnu11`: Uses the 2011 C standard with GNU extensions (necessary for POSIX APIs).
- `-g`: Includes DWARF debugging symbols for GDB stack inspection.
- `-pthread`: Configures preprocessor macros and links the POSIX threads library.

### Windows Subsystem for Linux (WSL2) Setup

Students developing on Windows machines should use WSL2 running Ubuntu.
WSL2 runs a genuine Linux kernel inside a lightweight virtual machine, ensuring complete POSIX system call compatibility:

```powershell
# Windows PowerShell (Admin) - Install WSL2 with Ubuntu
wsl --install -d Ubuntu

# Verify running kernel version
wsl --status
```

Within the WSL2 environment, install the standard GNU compilation toolchain:

```bash
# Ubuntu package installation
sudo apt-get update
sudo apt-get install -y build-essential gdb valgrind cmake clang clang-format git
```

### Sanitizers and Memory Safety Checks

Always test solutions under LLVM sanitizers before running benchmarks or submitting code:

```bash
# AddressSanitizer (ASan) for memory safety and leak checking
gcc -fsanitize=address -fno-omit-frame-pointer -g -O1 source.c -o program_asan

# ThreadSanitizer (TSan) for detecting data races
gcc -fsanitize=thread -g -O1 -pthread source.c -o program_tsan
```

---

## 8. Course Summary and Success Strategies

Success in CS 6200 requires disciplined systems development practices:

1. **Start Projects Early:** Systems projects cannot be completed in an overnight sprint. Memory bugs, race conditions, and synchronization deadlocks require iterative debugging.
2. **Design Before Writing Code:** Sketch process layouts, thread interactions, shared memory layouts, and state machines on paper before writing C code.
3. **Check Every Return Value:** Every system call (`malloc`, `pthread_create`, `sem_wait`, `read`, `write`, `socket`) can fail. Never ignore return codes.
4. **Use Sanitizers Continuously:** Run AddressSanitizer and ThreadSanitizer from day one. Do not defer memory leak and race detection until the final submission.
5. **Master GDB:** Learn to inspect core dumps, set conditional breakpoints, and navigate multi-threaded call stacks rather than relying solely on `printf` logging.
6. **Participate in Discussion Forums:** Actively engage with peers and the instructional staff on Ed Discussion.

---

**Navigation:**
- Back to Dashboard: [GIOS Dashboard](../_GIOS-Dashboard.md)
- Next Module: [P1L2: Introduction to Operating Systems](P1L2-Introduction-to-Operating-Systems.md)

