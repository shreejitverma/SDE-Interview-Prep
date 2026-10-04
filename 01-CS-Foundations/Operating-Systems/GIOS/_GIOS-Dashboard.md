---
type: moc
track: [sde]
level: advanced
status: active
last_reviewed:
sources:
  - "Georgia Tech CS 6200: Introduction to Operating Systems (GIOS)"
  - "Operating System Concepts, Silberschatz et al."
  - "Linux Kernel Development, Robert Love"
  - "Windows Internals, Russinovich et al."
---

# Georgia Tech CS 6200 - Introduction to Operating Systems

> Complete course notes covering all 16 modules with Linux and Windows examples, code walkthroughs, and quiz-style exercises.

---

## Course Staff & Instructional Team

### Instructor

| Role | Name | Email | Institution / Dept |
|------|------|-------|--------------------|
| Professor | **Ada Gavrilovska** | [ada@cc.gatech.edu](mailto:ada@cc.gatech.edu) | Georgia Tech, School of Computer Science |

### Udacity Course Developers

| Name | Role | Email |
|------|------|-------|
| **Jarrod Parkes** | Course Developer | [jarrod@udacity.com](mailto:jarrod@udacity.com) |
| **Charles Brubaker** | Course Developer | [charles.brubaker@knowlabs.com](mailto:charles.brubaker@knowlabs.com) |
| **Ben Gardner** | Course Developer | [benjamin@udacity.com](mailto:benjamin@udacity.com) |

### Teaching Assistants (Fall 2026 - TBA)

#### Head Teaching Assistants

| Name | Role | Email |
|------|------|-------|
| **Tony Mason** | Head TA | [fsgeek@gatech.edu](mailto:fsgeek@gatech.edu) |
| **Vijayalakshmi (VJ) Parthiban** | Head TA | [vijayalakshmi.parthiban@gatech.edu](mailto:vijayalakshmi.parthiban@gatech.edu) |
| **Tho Ha** | Head TA | [Tho.Ha@gatech.edu](mailto:Tho.Ha@gatech.edu) |
| **Ioan G. Istrate** | Head TA | [iistrate3@gatech.edu](mailto:iistrate3@gatech.edu) |

#### Graduate Teaching Assistants

| Name | Email | Name | Email |
|------|-------|------|-------|
| **Alex (Poly) Kim** | [akim618@gatech.edu](mailto:akim618@gatech.edu) | **Cherry Wang** | [bwang647@gatech.edu](mailto:bwang647@gatech.edu) |
| **Cody Luu** | [cluu30@gatech.edu](mailto:cluu30@gatech.edu) | **Dax Alessandra Delvira** | [dax@gatech.edu](mailto:dax@gatech.edu) |
| **Daniel Sullivan** | [danielsullivan@gatech.edu](mailto:danielsullivan@gatech.edu) | **Debajyoti Banerjee** | [debajyoti.banerjee@gatech.edu](mailto:debajyoti.banerjee@gatech.edu) |
| **Dhruv Garg** | [dgarg39@gatech.edu](mailto:dgarg39@gatech.edu) | **Eric Michael Gregori** | [egregori3@gatech.edu](mailto:egregori3@gatech.edu) |
| **Eric O'Brien** | [eobrien31@gatech.edu](mailto:eobrien31@gatech.edu) | **Evie Vanderveer** | [ederveer3@gatech.edu](mailto:ederveer3@gatech.edu) |
| **Jesse Hall** | [jhall376@gatech.edu](mailto:jhall376@gatech.edu) | **Jiaqi Deng** | [jiaqi.deng@gatech.edu](mailto:jiaqi.deng@gatech.edu) |
| **Mitchell Flax** | [mflax3@gatech.edu](mailto:mflax3@gatech.edu) | **Nikola Maruszewski** | [nikola@gatech.edu](mailto:nikola@gatech.edu) |
| **Phillip Wilson Moore** | [pwm@gatech.edu](mailto:pwm@gatech.edu) | **Richard William Suhr** | [rsuhr3@gatech.edu](mailto:rsuhr3@gatech.edu) |
| **Robert Bischoff** | [rbischoff3@gatech.edu](mailto:rbischoff3@gatech.edu) | **Seyed Nouraie** | [snouraie3@gatech.edu](mailto:snouraie3@gatech.edu) |
| **Shauna Hyppolite** | [shyppolite3@gatech.edu](mailto:shyppolite3@gatech.edu) | **Suraj Mallenahalli Shivu** | [sshivu3@gatech.edu](mailto:sshivu3@gatech.edu) |
| **Vijay Thurimella** | [vthurimella@gatech.edu](mailto:vthurimella@gatech.edu) | **Peter Jaglom** | [pjaglom3@gatech.edu](mailto:pjaglom3@gatech.edu) |
| **Jonathan Ramirez** | [jramirez311@gatech.edu](mailto:jramirez311@gatech.edu) | | |

---

## Course Structure

```
CS 6200 GIOS
  |
  +-- Part 1: Architecture & OS Basics
  |     +-- P1L1: Course Overview & Logistics
  |     +-- P1L2: Introduction to Operating Systems
  |
  +-- Part 2: Process & Thread Management
  |     +-- P2L1: Processes and Process Management
  |     +-- P2L2: Threads and Concurrency
  |     +-- P2L3: Threads Case Study - PThreads
  |     +-- P2L4: Thread Design Considerations
  |     +-- P2L5: Thread Performance Considerations
  |
  +-- Part 3: Resource Management & Communication
  |     +-- P3L1: Scheduling
  |     +-- P3L2: Memory Management
  |     +-- P3L3: Inter-Process Communication (IPC)
  |     +-- P3L4: Synchronization Constructs
  |     +-- P3L5: I/O Management
  |     +-- P3L6: Virtualization
  |
  +-- Part 4: Distributed Systems
        +-- P4L1: Remote Procedure Calls (RPC)
        +-- P4L2: Distributed File Systems (DFS)
        +-- P4L3: Distributed Shared Memory (DSM)
        +-- P4L4: Datacenter Technologies
```

---

## Part 1: Architecture & OS Basics

| Module | Topic | Status | Size |
|--------|-------|--------|------|
| [P1L1](Part-1-Architecture/P1L1-Course-Overview.md) | Course Overview & Logistics | `complete` | ~16 KB |
| [P1L2](Part-1-Architecture/P1L2-Introduction-to-Operating-Systems.md) | Introduction to Operating Systems | `complete` | ~46 KB |

**Key themes:** Course logistics and staff directory, OS roles (abstraction, arbitration), user/kernel mode, system calls, monolithic vs. microkernel, hybrid architectures, Linux/macOS/Windows architecture, `/proc` inspection, `strace`, `ltrace`, Win32 subsystem.

---

## Part 2: Process & Thread Management

| Module | Topic | Status | Size |
|--------|-------|--------|------|
| [P2L1](Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md) | Processes and Process Management | `complete` | ~43 KB |
| [P2L2](Part-2-Process-Thread-Management/P2L2-Threads-and-Concurrency.md) | Threads and Concurrency | `complete` | ~39 KB |
| [P2L3](Part-2-Process-Thread-Management/P2L3-PThreads-Case-Study.md) | Threads Case Study: PThreads | `complete` | ~30 KB |
| [P2L4](Part-2-Process-Thread-Management/P2L4-Thread-Design-Considerations.md) | Thread Design Considerations | `complete` | ~23 KB |
| [P2L5](Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md) | Thread Performance Considerations | `complete` | ~24 KB |

**Key themes:** Process model, PCB, context switching, threads vs processes, mutexes, condition variables, PThreads API, ULT/KLT, many-to-many models, Amdahl's law, false sharing, thread pools, Windows CreateProcess/CreateThread, Win32 synchronization primitives.

---

## Part 3: Resource Management & Communication

| Module | Topic | Status | Size |
|--------|-------|--------|------|
| [P3L1](Part-3-Resource-Management/P3L1-Scheduling.md) | Scheduling | `complete` | ~23 KB |
| [P3L2](Part-3-Resource-Management/P3L2-Memory-Management.md) | Memory Management | `complete` | ~27 KB |
| [P3L3](Part-3-Resource-Management/P3L3-Inter-Process-Communication.md) | Inter-Process Communication | `complete` | ~16 KB |
| [P3L4](Part-3-Resource-Management/P3L4-Synchronization-Constructs.md) | Synchronization Constructs | `complete` | ~21 KB |
| [P3L5](Part-3-Resource-Management/P3L5-IO-Management.md) | I/O Management | `complete` | ~18 KB |
| [P3L6](Part-3-Resource-Management/P3L6-Virtualization.md) | Virtualization | `complete` | ~17 KB |

**Key themes:** FCFS through CFS, multi-level feedback queues, paging and TLBs, page replacement, buddy/slab allocators, pipes, shared memory, semaphores, hardware atomics, spinlocks, MCS locks, RCU, DMA, disk scheduling, SSDs, VFS, hypervisors, VT-x, EPT/NPT, SR-IOV, containers (namespaces/cgroups).

---

## Part 4: Distributed Systems

| Module | Topic | Status | Size |
|--------|-------|--------|------|
| [P4L1](Part-4-Distributed-Systems/P4L1-Remote-Procedure-Calls.md) | Remote Procedure Calls (RPC) | `complete` | ~47 KB |
| [P4L2](Part-4-Distributed-Systems/P4L2-Distributed-File-Systems.md) | Distributed File Systems (DFS) | `complete` | ~50 KB |
| [P4L3](Part-4-Distributed-Systems/P4L3-Distributed-Shared-Memory.md) | Distributed Shared Memory (DSM) | `complete` | ~42 KB |
| [P4L4](Part-4-Distributed-Systems/P4L4-Datacenter-Technologies.md) | Datacenter Technologies | `complete` | ~26 KB |

**Key themes:** RPC stubs, IDL (XDR/protobuf/MIDL), XDR/protobuf marshaling, Sun RPC, gRPC (unary/streaming/bidirectional), MSRPC/Windows RPC, NFS v3/v4/v4.1, AFS callbacks, GFS, HDFS, Ceph, Windows DFS Namespace/Replication, DSM consistency models (SC/RC/LRC), IVY protocol, TreadMarks, RDMA, MPI, OpenSHMEM, false sharing, multi-tier homogeneous vs. heterogeneous architectures, cloud computing elasticity (Animoto case study), Law of Large Numbers, utility computing, NIST cloud definitions (IaaS/PaaS/SaaS), cluster failure probability $1-(1-p)^N$, big data stacks (Hadoop, MapReduce, Spark).

---

## Exam Prep

| Set | Scope | Questions | Status |
|-----|-------|-----------|--------|
| [Midterm Practice Questions](Exam-Prep/Midterm-Practice-Questions.md) | P1-P2, Solaris + Flash papers | 8 | `active` |

Answers are in collapsed Obsidian callouts - attempt each question before expanding.

---

## Code Examples

All runnable code is in the [code/](code/) directory:

| File | Module | What It Demonstrates |
|------|--------|---------------------|
| `fork_exec_demo.c` | P2L1 | Process creation with fork/exec on Linux |
| `win_createprocess.c` | P2L1 | Process creation with CreateProcess on Windows |
| `pthreads_lifecycle.c` | P2L3 | Full PThreads lifecycle: create, join, detach |
| `mutex_condvar.c` | P2L3 | Mutex + condition variable pattern |
| `producer_consumer_posix.c` | P2L2-P2L3 | Bounded buffer with PThreads |
| `reader_writer.c` | P2L2 | Reader-writer lock implementation |
| `spinlock_implementations.c` | P3L4 | TAS, TATAS, ticket lock, MCS lock |
| `mmap_shared_memory.c` | P3L3 | POSIX shared memory with mmap |
| `page_replacement_sim.py` | P3L2 | FIFO, LRU, Clock page replacement simulation |
| `disk_scheduler_sim.py` | P3L5 | FCFS, SSTF, SCAN, C-SCAN disk scheduling |
| `rpc_grpc_demo/` | P4L1 | gRPC client-server in C++ |

---

## Cross-References

- **Concurrency C++ examples:** [../../Concurrency-Cpp/](../Concurrency-Cpp/)
- **IPC Complete Guide:** [../IPC-Complete-Guide.md](../IPC-Complete-Guide.md)
- **System Design - OS Concepts:** [../../../04-System-Design/00-Concepts/](../../../04-System-Design/00-Concepts/)
- **Performance Engineering:** [../../../12-Performance-Engineering/](../../../12-Performance-Engineering/)
- **Low-Latency Systems:** [../../../14-Low-Latency-Systems/](../../../14-Low-Latency-Systems/)

---

## How to Use These Notes

1. **Sequential study:** Follow P1L2 through P4L3 in order; each module builds on prior ones.
2. **Reference lookup:** Use the TOC within each file to jump to specific topics.
3. **Hands-on practice:** Compile and run the code examples; each includes build commands for both Linux and Windows.
4. **Quiz prep:** Look for `> **Quiz**` callout blocks throughout - they mirror the course's embedded quizzes.
5. **Interview prep:** Focus on P2L2 (concurrency), P3L1 (scheduling), P3L2 (memory), and P3L4 (synchronization) - these are the most commonly tested.
