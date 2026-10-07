---
type: concept
track: [sde]
level:
status: solid
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L1"
  - "Operating System Concepts, 10th Ed., Silberschatz"
  - "Linux Kernel Development, 3rd Ed., Robert Love"
  - "Understanding the Linux Kernel, Bovet & Cesati"
---

# P3L1: Scheduling

> **Module goal:** Master CPU scheduling algorithms from FCFS through CFS, understand scheduling metrics, multi-level queues, Linux scheduling evolution, multiprocessor scheduling, and NUMA-aware scheduling.

## Table of Contents

- [1. What is CPU Scheduling?](#1-what-is-cpu-scheduling)
- [2. Scheduling Metrics](#2-scheduling-metrics)
- [3. First-Come, First-Served (FCFS)](#3-first-come-first-served-fcfs)
- [4. Shortest Job First (SJF)](#4-shortest-job-first-sjf)
- [5. Shortest Remaining Time First (SRTF)](#5-shortest-remaining-time-first-srtf)
- [6. Round Robin (RR) Scheduling](#6-round-robin-rr-scheduling)
- [7. Priority Scheduling and Starvation](#7-priority-scheduling-and-starvation)
- [8. Multi-Level Queue (MLQ)](#8-multi-level-queue-mlq)
- [9. Multi-Level Feedback Queue (MLFQ)](#9-multi-level-feedback-queue-mlfq)
- [10. Linux Scheduling Evolution](#10-linux-scheduling-evolution)
- [11. Completely Fair Scheduler (CFS)](#11-completely-fair-scheduler-cfs)
- [12. Multiprocessor Scheduling](#12-multiprocessor-scheduling)
- [13. NUMA-Aware Scheduling and Hyperthreading](#13-numa-aware-scheduling-and-hyperthreading)
- [14. Quizzes and Exercises](#14-quizzes-and-exercises)
- [15. Key Takeaways](#15-key-takeaways)
- [16. Linux CFS Internals: vruntime and the Red-Black Tree](#16-linux-cfs-internals-vruntime-and-the-red-black-tree)
- [17. CPU Throttling and cgroup CPU Bandwidth Control](#17-cpu-throttling-and-cgroup-cpu-bandwidth-control)
- [18. Windows Scheduling Internals](#18-windows-scheduling-internals)
- [19. macOS Scheduling Architecture & Darwin Quality-of-Service (QoS)](#19-macos-scheduling-architecture--darwin-quality-of-service-qos)

---

## 1. What is CPU Scheduling?

The CPU scheduler decides **which ready process/thread gets the CPU next** and **for how long**.

```
Ready Queue:            CPU Scheduler              CPU
[P3][P1][P5][P2] -----> [Algorithm] --------> [P1 running]
                         select + dispatch

Scheduling decisions occur when:
1. Process blocks (voluntary: I/O, wait)     - Non-preemptive
2. Process terminates                         - Non-preemptive
3. Timer interrupt fires (quantum expires)   - Preemptive
4. Higher-priority process becomes ready     - Preemptive
```

**Preemptive vs. Non-preemptive:**
- **Non-preemptive:** process runs until it voluntarily gives up CPU (blocks or exits)
- **Preemptive:** OS can forcibly take CPU away (via timer interrupt)

---

## 2. Scheduling Metrics

| Metric | Formula | Goal |
|--------|---------|------|
| **Turnaround time** | Completion time - Arrival time | Minimize |
| **Waiting time** | Time spent in ready queue | Minimize |
| **Response time** | First run time - Arrival time | Minimize (interactive) |
| **Throughput** | Processes completed / Time | Maximize |
| **CPU utilization** | Busy time / Total time | Maximize |
| **Fairness** | Equal CPU share per process | Maximize |

```
Process timeline:
Arrival                                      Completion
  |        Response         |                    |
  |<----->|                 |                    |
  |  wait | First execution |    more execution  |
  |       |                 |                    |
  |<------------- Turnaround time -------------->|
  |<- Waiting time ->|      (total time in ready queue)
```

### Official CS 6200 Scheduling Metric Formulas

The Georgia Tech CS 6200 curriculum formally defines scheduling performance evaluation using the following formulas:

- **Throughput Formula:**
  $$\text{Throughput} = \frac{\text{jobs\_completed}}{\text{time\_to\_complete\_all\_jobs}}$$

- **Average Completion Time Formula:**
  $$\text{Avg. Completion Time} = \frac{\sum \text{times\_to\_complete\_each\_job}}{\text{jobs\_completed}}$$

- **Average Wait Time Formula:**
  $$\text{Avg. Wait Time} = \frac{\sum_{i=1}^{n} t_i\text{\_wait\_time}}{\text{jobs\_completed}} = \frac{t_1\text{\_wait\_time} + t_2\text{\_wait\_time} + \dots + t_n\text{\_wait\_time}}{\text{jobs\_completed}}$$

- **Time to Complete All Jobs per Job (Average Makespan):**
  $$\text{Makespan per Job} = \frac{\text{time\_to\_complete\_all\_jobs}}{\text{jobs\_completed}}$$

> [!NOTE]
> **CS 6200 Exam & Quiz Conventions:**
> - You do not have to include units in your numerical answers on quizzes and exams.
> - For decimal answers, please round to the hundredths place (two decimal digits, e.g., `0.25`, `1.67`, `5.00`).


---

## 3. First-Come, First-Served (FCFS)

The simplest scheduling algorithm: processes are served in order of arrival.

```
Non-preemptive. Implemented as a FIFO queue.

Example:
Process  Arrival  Burst
P1       0        24
P2       1        3
P3       2        3

Gantt chart:
|------------ P1 ------------|-- P2 --|-- P3 --|
0                            24      27      30

Turnaround:  P1=24  P2=26  P3=28   Avg=26.0
Waiting:     P1=0   P2=23  P3=25   Avg=16.0
```

**Problem: Convoy effect** - short processes stuck behind a long process.

If P2 and P3 arrived first:
```
|-- P2 --|-- P3 --|------------ P1 ------------|
0        3        6                            30

Turnaround:  P2=3   P3=4   P1=30   Avg=12.3  (much better!)
Waiting:     P2=0   P3=1   P1=6    Avg=2.3
```

---

## 4. Shortest Job First (SJF)

Selects the process with the **shortest CPU burst** next.

```
Non-preemptive SJF.

Process  Arrival  Burst
P1       0        7
P2       2        4
P3       4        1
P4       5        4

Gantt chart (non-preemptive: P1 runs to completion first):
|------- P1 -------|-- P3 --|------ P4 ------|- P2 -|
0                  7        8              12      16

Wait: P1=0  P3=3  P4=3  P2=6   Avg=3.0
```

**SJF is provably optimal** for minimizing average waiting time among non-preemptive algorithms.

**Problem:** How do you know the burst length in advance? You don't. It must be **estimated**, typically using exponential averaging of past bursts:

```
tau_{n+1} = alpha * t_n + (1 - alpha) * tau_n

Where:
  t_n     = actual length of the nth burst
  tau_n   = predicted length of the nth burst
  alpha   = weighting factor (0 < alpha < 1, typically 0.5)
```

---

## 5. Shortest Remaining Time First (SRTF)

**Preemptive version of SJF**: if a new process arrives with a shorter remaining burst than the currently running process, preempt.

```
Process  Arrival  Burst
P1       0        7
P2       2        4
P3       4        1
P4       5        4

Timeline:
t=0: P1 arrives, runs (remaining=7)
t=2: P2 arrives (burst=4 < P1 remaining=5), preempt P1
     P2 runs (remaining=4)
t=4: P3 arrives (burst=1 < P2 remaining=2), preempt P2
     P3 runs (remaining=1)
t=5: P3 finishes. P4 arrives (burst=4). P2 remaining=2 < P4 burst=4
     P2 runs (remaining=2)
t=7: P2 finishes. P4 burst=4 vs P1 remaining=5
     P4 runs (remaining=4)
t=11: P4 finishes. P1 runs (remaining=5)
t=16: P1 finishes.

Gantt:
|- P1 -|- P2 -|P3|- P2 -|---- P4 ----|------ P1 ------|
0      2      4  5      7           11               16

Turnaround: P1=16  P2=5  P3=1  P4=6   Avg=7.0
Waiting:    P1=9   P2=1  P3=0  P4=2   Avg=3.0
```

> **Quiz: SJF/SRTF**
>
> *SJF is optimal for average waiting time. Why isn't it always used?*
>
> **Answer:** Because burst lengths are unknown in advance. You can only estimate them. Also, SRTF (preemptive SJF) can **starve** long processes if short ones keep arriving.

---

## 6. Round Robin (RR) Scheduling

Each process gets a fixed **time quantum** (timeslice). After the quantum expires, the process is preempted and moved to the back of the ready queue.

```
Quantum = 4

Process  Arrival  Burst
P1       0        24
P2       0        3
P3       0        3

Gantt:
|- P1 -|- P2 -|- P3 -|- P1 -|- P1 -|- P1 -|- P1 -|- P1 -|
0      4      7     10     14     18     22     26     30

P2 finishes at 7 (burst=3 < quantum=4)
P3 finishes at 10

Turnaround: P1=30  P2=7  P3=10   Avg=15.7
Waiting:    P1=6   P2=4  P3=7    Avg=5.7
```

### Timeslice Selection Tradeoffs

| Quantum | Effect |
|---------|--------|
| Very small (1 ms) | More responsive, but high context switch overhead |
| Very large (100 ms) | Low overhead, but poor response time (degenerates to FCFS) |
| Sweet spot (~10-100 ms) | Balance between responsiveness and overhead |

**Rule of thumb:** Quantum should be large enough that 80%+ of CPU bursts complete within one quantum.

```
Context switch cost = C
Quantum = Q
Overhead fraction = C / (Q + C)

If C = 0.5 ms, Q = 10 ms: overhead = 0.5/10.5 = 4.8%
If C = 0.5 ms, Q = 1 ms:  overhead = 0.5/1.5 = 33%  (too high!)
If C = 0.5 ms, Q = 100 ms: overhead = 0.5/100.5 = 0.5% (but poor response)
```

**Linux - see and set timeslice:**
```bash
# CFS doesn't use a fixed quantum but a "scheduling latency"
cat /proc/sys/kernel/sched_latency_ns
# Default: 6000000 ns (6 ms) for desktop
# This is the target period in which all runnable tasks should run once

cat /proc/sys/kernel/sched_min_granularity_ns
# Default: 750000 ns (0.75 ms) - minimum timeslice per task

# With N tasks, each gets: max(latency/N, min_granularity)
```

---

## 7. Priority Scheduling and Starvation

Each process has a **priority**. The highest-priority ready process runs next.

```
Priority levels (lower number = higher priority, typical convention):

Priority 0 (highest): [P_realtime]
Priority 1:           [P_system]
  ...
Priority 19:          [P_interactive]
Priority 20-39:       [P_batch, P_idle]
```

**Problem: Starvation** - a low-priority process may never run if higher-priority processes keep arriving.

**Solution: Aging** - gradually increase the priority of waiting processes:

```
Process P_low starts at priority 30.
Every 1 second it waits, its priority improves by 1.
After 20 seconds: priority = 10 (now runs)
```

**Linux - priorities:**
```bash
# Nice values: -20 (highest priority) to +19 (lowest)
nice -n 10 ./cpu_bound_task    # lower priority
nice -n -10 ./important_task   # higher priority (needs root)

# Renice a running process
renice -n 5 -p $PID

# Real-time priorities (1-99, above all normal tasks)
chrt -f 50 ./realtime_task     # SCHED_FIFO, priority 50
chrt -r 50 ./realtime_task     # SCHED_RR, priority 50
```

**Windows - priorities:**
```powershell
# Priority classes: Idle, BelowNormal, Normal, AboveNormal, High, Realtime
Start-Process notepad -Priority High

# Change running process priority
$p = Get-Process -Id $PID
$p.PriorityClass = 'AboveNormal'
```

---

## 8. Multi-Level Queue (MLQ)

Processes are permanently assigned to different queues based on their type:

```
Queue 0 (highest): Real-time processes    [RR or FIFO]
Queue 1:           System processes       [RR]
Queue 2:           Interactive processes  [RR, short quantum]
Queue 3:           Batch processes        [FCFS or RR, long quantum]
Queue 4 (lowest):  Idle processes         [FCFS]

Scheduling between queues:
  - Fixed priority: always run highest non-empty queue
  - Time-slice: Queue 0 gets 40%, Queue 1 gets 30%, etc.
```

**Problem:** A process in Queue 3 (batch) can never move to Queue 2 (interactive) even if its behavior changes. This is too rigid.

---

## 9. Multi-Level Feedback Queue (MLFQ)

MLFQ allows processes to **move between queues** based on observed behavior.

```
Queue 0 (highest priority, shortest quantum = 8ms)
  New processes start here
  +---------+
  | [P_new] |  If uses full quantum -> demote to Queue 1
  +---------+  If yields early (I/O) -> stay in Queue 0

Queue 1 (medium priority, quantum = 16ms)
  +---------+
  | [P_mid] |  If uses full quantum -> demote to Queue 2
  +---------+  If yields early -> promote to Queue 0

Queue 2 (lowest priority, quantum = 32ms or FCFS)
  +---------+
  | [P_low] |  Runs only when Queue 0 and 1 are empty
  +---------+
```

### MLFQ Rules

| Rule | Description |
|------|-------------|
| **Rule 1** | If Priority(A) > Priority(B), A runs |
| **Rule 2** | If Priority(A) = Priority(B), run in RR |
| **Rule 3** | New jobs start at the highest priority |
| **Rule 4a** | If a job uses its entire quantum, it is demoted |
| **Rule 4b** | If a job voluntarily relinquishes CPU (I/O), it stays |
| **Rule 5** | After time period S, boost all jobs to top queue (anti-starvation) |

**Rule 5 (Priority Boost)** solves starvation: periodically, all processes are moved to the highest queue. This also handles processes that change behavior (e.g., a CPU-bound process becomes interactive).

**Gaming prevention:** Without Rule 4's refinement, a process could game MLFQ by issuing a tiny I/O request just before its quantum expires, staying at high priority forever. Modern MLFQ tracks total CPU time at each level and demotes based on accumulated time, not per-quantum behavior.

> **Quiz: MLFQ**
>
> *A new I/O-bound process and a long-running CPU-bound process both start. Describe their MLFQ trajectories.*
>
> **Answer:**
> - **I/O-bound:** Starts in Queue 0, frequently yields for I/O before quantum expires, stays in Queue 0 (high priority). Gets excellent response time.
> - **CPU-bound:** Starts in Queue 0, uses full quantum, demoted to Queue 1. Uses full quantum again, demoted to Queue 2. Gets lower priority but longer quanta.
> - This is **exactly the desired behavior**: interactive processes get fast response, batch processes get throughput.

---

## 10. Linux Scheduling Evolution

### The O(n) Scheduler (Linux 2.4)

```
Every scheduling decision: scan ALL processes in the run queue.
  - O(n) per scheduling event
  - For 1000 processes: 1000 comparisons per context switch
  - Terrible scalability
```

### The O(1) Scheduler (Linux 2.6.0 - 2.6.22)

```
Two arrays of 140 priority queues:
  Active array:  [prio 0] [prio 1] ... [prio 139]
  Expired array: [prio 0] [prio 1] ... [prio 139]

Scheduling: O(1)
  1. Find highest-priority non-empty queue in active array
     (use a bitmap; find-first-bit is O(1) on x86)
  2. Dequeue from that queue
  3. When quantum expires, insert into expired array
  4. When active array is empty, swap active <-> expired

Problem: Heuristic-based interactivity detection was unreliable.
Desktop responsiveness was inconsistent.
```

### Completely Fair Scheduler (CFS) - Linux 2.6.23+ (Current Default)

CFS replaces the fixed-priority approach with a **fair share** model based on **virtual runtime**.

---

## 11. Completely Fair Scheduler (CFS)

### Concept: Virtual Runtime (vruntime)

```
vruntime = actual runtime * (nice_0_weight / task_weight)

Each task accumulates vruntime as it runs.
Higher-priority tasks accumulate vruntime SLOWER.
Lower-priority tasks accumulate vruntime FASTER.

The task with the SMALLEST vruntime runs next.
This ensures all tasks get a "fair" share of CPU time,
weighted by their priority.
```

**Nice value to weight mapping (selected):**
| Nice | Weight | Ratio to nice 0 |
|------|--------|-----------------|
| -20 | 88761 | ~68x faster share |
| -10 | 9548 | ~7.3x |
| 0 | 1024 | 1.0x (baseline) |
| 10 | 110 | ~0.1x |
| 19 | 15 | ~0.015x |

### CFS Red-Black Tree

CFS organizes runnable tasks in a **red-black tree** (self-balancing BST), keyed by vruntime:

```
                        Red-Black Tree
                       (sorted by vruntime)
                            
                           [vr=50]
                          /        \
                     [vr=30]      [vr=80]
                    /      \          \
               [vr=20]  [vr=40]    [vr=100]
                  ^
                  |
            leftmost node = smallest vruntime
            = NEXT to run (O(1) access via cached pointer)
```

| Operation | Complexity |
|-----------|-----------|
| Pick next task | O(1) - cached leftmost pointer |
| Enqueue task | O(log n) - RB-tree insert |
| Dequeue task | O(log n) - RB-tree delete |

### CFS Parameters

```bash
# Scheduling latency (target period for all tasks to run once)
cat /proc/sys/kernel/sched_latency_ns          # 6000000 (6ms)

# Minimum granularity (smallest possible timeslice)
cat /proc/sys/kernel/sched_min_granularity_ns  # 750000 (0.75ms)

# Wakeup granularity (min vruntime difference to preempt)
cat /proc/sys/kernel/sched_wakeup_granularity_ns  # 1000000 (1ms)

# Timeslice for task = latency * (task_weight / total_weight)
# With 8 equal-weight tasks: each gets 6ms / 8 = 0.75ms

# Example with nice values:
# Task A (nice 0, weight 1024) and Task B (nice 5, weight 335)
# Total weight = 1359
# A's timeslice = 6ms * 1024/1359 = 4.5ms
# B's timeslice = 6ms * 335/1359  = 1.5ms
```

**Linux - inspect CFS state:**
```bash
# See scheduling info for a process
cat /proc/$PID/sched
# Key fields:
# se.vruntime            : current virtual runtime
# se.sum_exec_runtime    : total CPU time consumed
# nr_switches            : total context switches
# nr_voluntary_switches  : voluntary (blocked on I/O)
# nr_involuntary_switches: involuntary (preempted)
# policy                 : SCHED_NORMAL (=CFS), SCHED_FIFO, SCHED_RR

# Scheduling statistics system-wide
cat /proc/schedstat
```

---

## 12. Multiprocessor Scheduling

### Processor Affinity

**Soft affinity:** OS tries to keep a process on the same CPU (for cache warmth) but can migrate it for load balancing.

**Hard affinity:** Process is pinned to specific CPUs.

```bash
# Linux: set CPU affinity
taskset -c 0,1 ./my_program        # Run on CPUs 0 and 1 only
taskset -p -c 2,3 $PID             # Change running process

# Or using cgroups:
echo "0-3" > /sys/fs/cgroup/cpuset/my_group/cpuset.cpus

# See current affinity
taskset -p $PID

# In code:
# sched_setaffinity(pid, sizeof(cpuset), &cpuset);
```

```powershell
# Windows: set processor affinity
$proc = Get-Process -Id $PID
$proc.ProcessorAffinity = 0x0F  # CPUs 0-3 (bitmask)

# Or via Start-Process
Start-Process -FilePath "program.exe" -PassThru |
  ForEach-Object { $_.ProcessorAffinity = 3 }  # CPUs 0,1
```

### Load Balancing and Work Stealing

```
CPU 0: [T1][T2][T3]       CPU 1: [T4]
CPU 2: [T5][T6][T7][T8]   CPU 3: []    <-- idle!

Load balancing: migrate T8 from CPU 2 to CPU 3
Work stealing: CPU 3 "steals" T8 from CPU 2's queue

Tradeoff:
  + Better utilization (no idle CPUs)
  - Cache pollution (migrated task loses cache warmth)
  - Lock contention on shared run queues
```

**Linux load balancing:**
```bash
# See per-CPU load
cat /proc/stat | grep ^cpu
# cpu0  user nice system idle iowait irq softirq
# cpu1  user nice system idle iowait irq softirq

# Disable load balancing for a set of CPUs (via isolcpus boot parameter)
# In /etc/default/grub:
# GRUB_CMDLINE_LINUX="isolcpus=2,3"
# CPUs 2,3 will not participate in load balancing
# Useful for latency-sensitive tasks
```

---

## 13. NUMA-Aware Scheduling and Hyperthreading

### NUMA (Non-Uniform Memory Access)

```
NUMA Topology:
+------------------+           +------------------+
| NUMA Node 0      |           | NUMA Node 1      |
|                  |           |                  |
| CPU 0  CPU 1     |           | CPU 2  CPU 3     |
| CPU 4  CPU 5     |           | CPU 6  CPU 7     |
|                  |           |                  |
| Local Memory     |   QPI/    | Local Memory     |
| (fast: ~80 ns)   |<--UPI--->| (fast: ~80 ns)   |
|                  | interconn |                  |
+------------------+ (~150 ns) +------------------+

Accessing local memory: ~80 ns
Accessing remote memory: ~150 ns (2x slower!)

NUMA-aware scheduling: keep a process on the same NUMA node as its memory.
```

```bash
# See NUMA topology
numactl --hardware
# available: 2 nodes (0-1)
# node 0 cpus: 0 1 4 5
# node 0 size: 32768 MB
# node 1 cpus: 2 3 6 7
# node 1 size: 32768 MB
# node distances:
# node   0   1
#   0:  10  20
#   1:  20  10

# Run on specific NUMA node
numactl --cpunodebind=0 --membind=0 ./my_program

# See per-NUMA-node memory stats
numastat
numastat -p $PID
```

### Hyperthreading (SMT)

```
Physical Core with Hyperthreading:
+---------------------------------+
| Physical Core 0                 |
|  +------------+ +------------+  |
|  | Logical    | | Logical    |  |
|  | CPU 0      | | CPU 4      |  |
|  | (thread 0) | | (thread 1) |  |
|  +------------+ +------------+  |
|                                 |
|  Shared: Execution units, cache |
|  Private: Registers, IP, state  |
+---------------------------------+

Two "logical CPUs" share one physical core.
Benefit: ~20-30% throughput improvement (not 2x).
The OS scheduler should prefer spreading threads across
physical cores before using both hyperthreads of one core.
```

```bash
# See hyperthrading topology
lscpu | grep -E "Thread|Core|Socket"
# Thread(s) per core: 2
# Core(s) per socket: 4
# Socket(s): 1
# = 4 physical cores, 8 logical CPUs

# See which logical CPUs share a physical core
cat /sys/devices/system/cpu/cpu0/topology/thread_siblings_list
# 0,4  (CPU 0 and CPU 4 share a physical core)
```

---

## 14. Quizzes and Exercises

> [!question] Quiz 1: Shortest Job First (SJF) and Turnaround Time Optimality (Clips 233-234)
> Consider a single-CPU system running non-preemptive run-to-completion scheduling.
> Three tasks arrive simultaneously at time $T_0$:
> - Task $T_1$: execution time = $1\text{ second}$
> - Task $T_2$: execution time = $10\text{ seconds}$
> - Task $T_3$: execution time = $1\text{ second}$
> 
> Using the official CS 6200 formulas:
> - $\text{Throughput} = \frac{\text{jobs\_completed}}{\text{time\_to\_complete\_all\_jobs}}$
> - $\text{Avg. Completion Time} = \frac{\sum \text{times\_to\_complete\_each\_job}}{\text{jobs\_completed}}$
> - $\text{Avg. Wait Time} = \frac{t_1\text{\_wait\_time} + t_2\text{\_wait\_time} + t_3\text{\_wait\_time}}{\text{jobs\_completed}}$
> 
> *(Note: Do not include units in answers; round decimals to the hundredths place).*
> 
> 1. What is the execution order under Shortest Job First (SJF)?
> 2. Calculate the Throughput, Average Completion Time, and Average Wait Time under SJF.
> 3. Compare these metrics against First-Come First-Served (FCFS) in arrival order $T_1 \to T_2 \to T_3$.
> 4. State the mathematical proof of why SJF minimizes total and average waiting time.

> [!success]- Answer
> **1. Execution Order under SJF:**
> Since $T_1 = 1\text{s}$, $T_3 = 1\text{s}$, and $T_2 = 10\text{s}$, the shortest jobs execute first:
> $$\text{Order: } T_1 \longrightarrow T_3 \longrightarrow T_2 \quad (\text{or } T_3 \longrightarrow T_1 \longrightarrow T_2)$$
> 
> ---
> 
> **2. Quantitative Metrics under SJF:**
> - **Timeline:**
>   - $T_1$ runs from $t = 0$ to $t = 1$ (completes at $1\text{s}$, waited $0\text{s}$)
>   - $T_3$ runs from $t = 1$ to $t = 2$ (completes at $2\text{s}$, waited $1\text{s}$)
>   - $T_2$ runs from $t = 2$ to $t = 12$ (completes at $12\text{s}$, waited $2\text{s}$)
> - **Throughput:**
>   $$\text{Throughput} = \frac{\text{jobs\_completed}}{\text{time\_to\_complete\_all\_jobs}} = \frac{3}{12} = \mathbf{0.25}$$
> - **Average Completion Time:**
>   $$\text{Avg. Completion Time} = \frac{1 + 2 + 12}{3} = \frac{15}{3} = \mathbf{5.00}$$
> - **Average Wait Time:**
>   $$\text{Avg. Wait Time} = \frac{0 + 1 + 2}{3} = \frac{3}{3} = \mathbf{1.00}$$
> - **Time to Complete All Jobs per Job:**
>   $$\text{Makespan per Job} = \frac{12}{3} = \mathbf{4.00}$$
> 
> ---
> 
> **3. Comparison Against FCFS ($T_1 \to T_2 \to T_3$):**
> - **FCFS Timeline:**
>   - $T_1$ runs $[0, 1]$ (completes at $1\text{s}$, waited $0\text{s}$)
>   - $T_2$ runs $[1, 11]$ (completes at $11\text{s}$, waited $1\text{s}$)
>   - $T_3$ runs $[11, 12]$ (completes at $12\text{s}$, waited $11\text{s}$)
> - **FCFS Metrics:**
>   - $\text{Throughput} = \frac{3}{12} = \mathbf{0.25}$ (identical to SJF)
>   - $\text{Avg. Completion Time} = \frac{1 + 11 + 12}{3} = \frac{24}{3} = \mathbf{8.00}$ (vs $5.00$ in SJF)
>   - $\text{Avg. Wait Time} = \frac{0 + 1 + 11}{3} = \frac{12}{3} = \mathbf{4.00}$ (vs $1.00$ in SJF)
> 
> *Key takeaway:* While throughput is identical ($0.25$), SJF slashes average wait time from $4.00\text{s}$ to $1.00\text{s}$ ($4\times$ improvement) and reduces average completion time from $8.00\text{s}$ to $5.00\text{s}$.
> 
> ---
> 
> **4. Mathematical Proof of Optimality:**
> - Consider $n$ jobs arriving simultaneously with durations $t_1, t_2, \dots, t_n$.
> - Under schedule $\pi$, the waiting time of the $k$-th job is $W_k = \sum_{j=1}^{k-1} t_{\pi(j)}$.
> - Total waiting time is:
>   $$W_{\text{total}} = \sum_{k=1}^{n} (n-k)t_{\pi(k)} = (n-1)t_{\pi(1)} + (n-2)t_{\pi(2)} + \dots + 1 \cdot t_{\pi(n-1)} + 0 \cdot t_{\pi(n)}$$
> - By the rearrangement inequality, this sum of products is strictly minimized when larger multipliers $(n-1, n-2, \dots)$ are paired with smaller job lengths $t$.
> Hence, sorting jobs such that $t_1 \le t_2 \le \dots \le t_n$ produces the minimum possible average wait time.
> - *Practical Limit:* Future CPU burst lengths cannot be known with certainty; general-purpose kernels must approximate them using exponential smoothing ($\tau_{n+1} = \alpha t_n + (1-\alpha)\tau_n$).


> [!question] Quiz 2: Preemptive Scheduling and Priority Inversion (Clips 237-238)
> 1. Differentiate between **preemptive** and **non-preemptive** CPU scheduling.
> 2. What is **Priority Inversion**, and how did the Mars Pathfinder spacecraft recover from priority inversion in its real-time scheduler?

> [!success]- Answer
> 1. **Preemptive vs. Non-preemptive:**
>    - **Non-preemptive:** A running process retains the CPU until it voluntarily terminates or blocks on I/O. A newly arrived higher-priority task must wait.
>    - **Preemptive:** The kernel can interrupt and suspend a running process when its time slice expires or when a higher-priority task enters the READY state, immediately switching the CPU.
> 2. **Priority Inversion & Mars Pathfinder:**
>    - *Mechanism:* Occurs when a high-priority task ($T_{\text{high}}$) is blocked waiting for a shared mutex held by a low-priority task ($T_{\text{low}}$). If a medium-priority task ($T_{\text{med}}$) arrives that does not need the mutex, it preempts $T_{\text{low}}$. Consequently, $T_{\text{med}}$ indirectly delays $T_{\text{high}}$ indefinitely!
>    - *Solution (Priority Inheritance Protocol):* When $T_{\text{high}}$ blocks on the mutex held by $T_{\text{low}}$, the kernel temporarily elevates $T_{\text{low}}$'s scheduling priority to match $T_{\text{high}}$. $T_{\text{low}}$ cannot be preempted by $T_{\text{med}}$, rapidly finishes its critical section, releases the mutex, drops back to its low priority, and enables $T_{\text{high}}$ to execute immediately.

> [!question] Quiz 3: Timeslice (Quantum) Selection Trade-Offs (Clips 246-247)
> In Round Robin (RR) and time-sharing scheduling:
> 1. What happens if the timeslice quantum is configured too small ($\to 0$)?
> 2. What happens if the timeslice quantum is configured too large ($\to \infty$)?
> 3. What rule of thumb governs timeslice sizing in production kernels?

> [!success]- Answer
> 1. **Quantum Too Small:** Context-switch overhead dominates CPU cycles. If quantum $q = 1\text{ ms}$ and context switch cost $c = 0.1\text{ ms}$, then $\frac{0.1}{1.1} \approx 9\%$ of total CPU capacity is wasted purely on switching overhead and cache pollution.
> 2. **Quantum Too Large:** Round Robin degenerates into FCFS. Short interactive jobs suffer long queuing delays behind long-running CPU-bound tasks, degrading interactive responsiveness.
> 3. **Rule of Thumb:** Configure the quantum such that context-switch overhead accounts for less than 1% of the timeslice ($q \gg c$), while keeping $q$ within the human perception limit for interactive responsiveness (typically 10 ms to 100 ms).

> [!question] Quiz 4: Linux Scheduler Evolution (Clips 251-252)
> Contrast the three historical generations of Linux CPU schedulers:
> 1. Linux 2.4 $O(n)$ Scheduler
> 2. Linux 2.6 $O(1)$ Scheduler
> 3. Linux 2.6.23+ Completely Fair Scheduler (CFS)

> [!success]- Answer
> 1. **$O(n)$ Scheduler:** Used a single global runqueue protected by a single lock. Every scheduling decision required scanning all ready processes to calculate `goodness()`. Recomputed quantum epochs when all tasks expired. Bottlenecked on multiprocessors ($O(n)$ complexity).
> 2. **$O(1)$ Scheduler:** Introduced per-CPU runqueues with two priority arrays: `active` and `expired`. Bitmaps tracked non-empty priority queues. Constant time $O(1)$ lookup via hardware bit-scan instructions (`bsfl`). Swapped array pointers when active became empty.
>    *Flaw:* Complex heuristics to determine interactivity caused fairness anomalies and jitter.
> 3. **Completely Fair Scheduler (CFS):** Replaced priority arrays with a time-ordered **red-black tree** keyed by `vruntime` (virtual runtime). Tasks with smallest `vruntime` occupy the leftmost tree node ($O(1)$ dispatch). When a task runs, its `vruntime` increases proportionally to its nice weight ($vruntime += \Delta t \times \frac{w_0}{w_i}$). Eliminates complex heuristic hacks with mathematical fairness.

> [!question] Quiz 5: Multiprocessor Scheduling & The CPI Experiment (Clips 258-260)
> In multi-processor scheduling experiments measuring **CPI (Cycles Per Instruction)**:
> 1. Why does a thread's CPI increase significantly when it is migrated from CPU Core 0 to CPU Core 1 across scheduling quantums?
> 2. What mechanism do modern operating systems deploy to prevent cache thrashing across cores?

> [!success]- Answer
> 1. **Cache Coldness & Memory Bus Contention:** When running on Core 0, the thread keeps Core 0's private L1/L2 caches hot. When migrated to Core 1, Core 1's private caches are cold; every instruction fetch and memory read incurs high-latency L3 or main memory access, drastically increasing Cycles Per Instruction (CPI).
> 2. **Processor Affinity (Warm Cache Affinity):** Schedulers maintain soft affinity by attempting to re-schedule a task on the exact same CPU core where it previously ran. Schedulers only migrate tasks to another core during severe load imbalance (work stealing).

---

### Exercise: Scheduling Algorithms Simulator

```python
#!/usr/bin/env python3
"""Scheduling algorithm simulator: FCFS, SJF, SRTF, RR."""

from collections import deque

def fcfs(processes):
    """First-Come, First-Served."""
    procs = sorted(processes, key=lambda p: p['arrival'])
    time = 0
    results = []
    for p in procs:
        if time < p['arrival']:
            time = p['arrival']
        wait = time - p['arrival']
        time += p['burst']
        turnaround = time - p['arrival']
        results.append({**p, 'wait': wait, 'turnaround': turnaround})
    return results

def round_robin(processes, quantum):
    """Round Robin with given quantum."""
    procs = [{'name': p['name'], 'arrival': p['arrival'],
              'remaining': p['burst'], 'burst': p['burst']}
             for p in sorted(processes, key=lambda p: p['arrival'])]
    time = 0
    queue = deque()
    results = {p['name']: {'first_run': -1, 'completion': 0} for p in procs}
    idx = 0

    while queue or idx < len(procs):
        # Add newly arrived processes
        while idx < len(procs) and procs[idx]['arrival'] <= time:
            queue.append(procs[idx])
            idx += 1

        if not queue:
            time = procs[idx]['arrival']
            continue

        p = queue.popleft()
        if results[p['name']]['first_run'] < 0:
            results[p['name']]['first_run'] = time

        run_time = min(quantum, p['remaining'])
        time += run_time
        p['remaining'] -= run_time

        # Add processes that arrived during this quantum
        while idx < len(procs) and procs[idx]['arrival'] <= time:
            queue.append(procs[idx])
            idx += 1

        if p['remaining'] > 0:
            queue.append(p)
        else:
            results[p['name']]['completion'] = time

    # Calculate metrics
    for p in processes:
        r = results[p['name']]
        r['turnaround'] = r['completion'] - p['arrival']
        r['wait'] = r['turnaround'] - p['burst']
        r['response'] = r['first_run'] - p['arrival']

    return results

# Test
processes = [
    {'name': 'P1', 'arrival': 0, 'burst': 24},
    {'name': 'P2', 'arrival': 0, 'burst': 3},
    {'name': 'P3', 'arrival': 0, 'burst': 3},
]

print("=== FCFS ===")
for r in fcfs(processes):
    print(f"  {r['name']}: wait={r['wait']}, turnaround={r['turnaround']}")

print("\n=== Round Robin (quantum=4) ===")
rr = round_robin(processes, 4)
for name, r in rr.items():
    print(f"  {name}: wait={r['wait']}, turnaround={r['turnaround']}, "
          f"response={r['response']}")
```

```bash
python3 scheduler_sim.py
```

---

## 15. Key Takeaways

1. **FCFS** is simple but suffers from the convoy effect; not suitable for interactive workloads.
2. **SJF/SRTF** minimizes average waiting time but requires burst prediction and can starve long processes.
3. **Round Robin** provides fairness via time quanta; quantum size trades off response time vs. context switch overhead.
4. **MLFQ** adapts to process behavior: I/O-bound processes get high priority, CPU-bound get long quanta. Priority boost prevents starvation.
5. **Linux CFS** uses virtual runtime and a red-black tree for O(log n) scheduling with weighted fair sharing based on nice values.
6. **Multiprocessor scheduling** must balance load while respecting cache affinity; NUMA-aware scheduling keeps processes near their memory for 2x latency improvement.
7. **Hyperthreading** gives ~20-30% throughput gain; spread threads across physical cores before using sibling logical CPUs.

---

## 16. Linux CFS Internals: vruntime and the Red-Black Tree

```
CFS Run Queue (per CPU):

          vruntime
  min →  [task_A: 1000 ns]  ← rb_leftmost (next to run)
          [task_B: 1200 ns]
          [task_C: 1450 ns]
          [task_D: 2000 ns]  ← max_vruntime

Each task accumulates vruntime at rate: delta_exec * (NICE0_WEIGHT / task_weight)
Lower weight tasks accumulate vruntime faster → get less CPU time proportionally.

Weight table (from kernel sched/sched.h):
  nice -20: weight = 88761   (most CPU time)
  nice   0: weight =  1024   (default)
  nice +19: weight =    15   (least CPU time)

With nice 0 and nice -5 sharing CPU:
  weight_0  = 1024, weight_-5 = 3121
  CPU share: 1024/(1024+3121) ≈ 25%  vs  3121/(1024+3121) ≈ 75%
```

```bash
# Inspect CFS scheduling internals
# /proc/sched_debug shows per-CPU run queues
cat /proc/sched_debug | head -80

# Per-process scheduling stats
cat /proc/<pid>/sched
# se.vruntime         = 1234567890.123456
# se.sum_exec_runtime = 5678.901234
# nr_voluntary_switches: 1234  (explicit yields, e.g., sleep/I/O)
# nr_involuntary_switches: 567  (preempted: used full timeslice)

# Scheduling statistics (CONFIG_SCHEDSTATS required)
cat /proc/<pid>/schedstat
# time running on CPU (ns), time waiting in run queue (ns), # time slices

# /proc/schedstat: system-wide scheduler stats
cat /proc/schedstat

# Show CFS group scheduling
cat /sys/fs/cgroup/cpu.stat
# usage_usec: total CPU time in microseconds
# nr_periods: number of periods (100ms by default)
# nr_throttled: how many times throttled
# throttled_usec: total time throttled

# CPU bandwidth control (cgroup v2)
cat /sys/fs/cgroup/myapp/cpu.max
# 500000 1000000  (= 500ms per 1000ms = 50% CPU limit)

# View CPU shares (proportional)
cat /sys/fs/cgroup/myapp/cpu.weight  # 100 = default (1-10000 range)

# Latency tracking (scheduling latency watcher)
sudo bpftrace -e '
    tracepoint:sched:sched_stat_wait {
        @lat_us[comm] = hist(args->delay / 1000);
    }
    interval:s:10 { print(@lat_us); exit(); }'
```

### Preemption Model Selection

```bash
# Linux kernel preemption models (configured at build time)
grep CONFIG_PREEMPT /boot/config-$(uname -r)
# CONFIG_PREEMPT_NONE=y          Server: throughput-optimized (no voluntary preemption)
# CONFIG_PREEMPT_VOLUNTARY=y     Desktop: balance (only explicit preempt points)
# CONFIG_PREEMPT=y               Real-time: preemptible kernel
# CONFIG_PREEMPT_RT=y            Full real-time (PREEMPT_RT patch)

# Check current preemption model
cat /sys/kernel/debug/sched/preempt_model  # kernel 5.15+

# Voluntary preemption points in kernel code
# might_sleep() - checks if preemption is needed
# cond_resched() - explicit cooperative preemption check

# Real-time scheduling (SCHED_FIFO / SCHED_RR)
# - Not subject to CFS; runs at fixed priority
# - Preempts all SCHED_OTHER tasks
# - Priority range: 1-99 (99=highest)

# Set SCHED_FIFO (run-to-completion at priority 50):
chrt -f 50 ./realtime_task
# Or in code:
struct sched_param sp = { .sched_priority = 50 };
pthread_setschedparam(pthread_self(), SCHED_FIFO, &sp);

# SCHED_DEADLINE: EDF-based (Earliest Deadline First)
# Guarantees: task gets runtime_ns CPU time every period_ns
struct sched_attr attr = {
    .sched_policy   = SCHED_DEADLINE,
    .sched_runtime  =  5000000,   /* 5ms runtime */
    .sched_deadline = 10000000,   /* 10ms deadline */
    .sched_period   = 10000000,   /* 10ms period */
};
syscall(SYS_sched_setattr, 0, &attr, 0);

# View SCHED_DEADLINE tasks
cat /proc/<pid>/sched | grep -E "policy|dl_"
```

---

## 17. CPU Throttling and cgroup CPU Bandwidth Control

```bash
# CFS Bandwidth Control: limit CPU to N% across a cgroup
# Period = 100ms (default), Quota = CPU time allowed per period

# Give a container exactly 1.5 CPUs worth of CPU
echo "150000 100000" > /sys/fs/cgroup/myapp/cpu.max
#     ^^^^^^ quota (150ms)
#            ^^^^^^ period (100ms)
# = 150ms per 100ms = 1.5 CPUs

# Monitor throttling in real time
watch -n1 'cat /sys/fs/cgroup/myapp/cpu.stat | grep -E "nr_throttled|throttled_usec"'

# Kubernetes CPU limits → cgroup cpu.max
# resources:
#   limits:
#     cpu: "500m"   → 50000 100000 (50ms per 100ms = 0.5 CPU)

# THROTTLING ANTI-PATTERN: CPU limit set too low
# Symptom: high p99 latency even with low average CPU usage
# Cause: bursts throttled even though average is below limit
# Fix: raise limit OR switch to cpu.weight (proportional, no ceiling)

# Disable CPU limit (let it use proportional shares only)
echo "max 100000" > /sys/fs/cgroup/myapp/cpu.max  # "max" = unlimited

# CPU pinning: restrict container to specific cores
echo "0-3" > /sys/fs/cgroup/myapp/cpuset.cpus
echo "0"   > /sys/fs/cgroup/myapp/cpuset.mems   # NUMA node 0

# Observe scheduling events with ftrace
echo "sched:sched_switch" > /sys/kernel/debug/tracing/set_event
cat /sys/kernel/debug/tracing/trace_pipe | head -50
# task_name-PID [CPU] TIMESTAMP: sched_switch: prev=task1 next=task2 prio=120

# Identify scheduling bottlenecks
sudo runqlat-bpfcc        # Run queue latency histogram
sudo runqlen-bpfcc        # Run queue length over time
sudo cpudist-bpfcc        # On-CPU time distribution per process
```

---

## 18. Windows Scheduling Internals

Windows uses a **priority-based preemptive scheduler** with 32 priority levels.

```
Windows Priority Levels:
 0-15: Dynamic priorities (user-mode threads, adjusted by kernel)
16-31: Real-time priorities (require SeIncreaseBasePriority privilege)

Priority Classes (Process level):    Base Priority:
  IDLE_PRIORITY_CLASS               4
  BELOW_NORMAL_PRIORITY_CLASS       6
  NORMAL_PRIORITY_CLASS             8  (default)
  ABOVE_NORMAL_PRIORITY_CLASS      10
  HIGH_PRIORITY_CLASS              13
  REALTIME_PRIORITY_CLASS          24

Thread priority offset (-2 to +2 from base):
  THREAD_PRIORITY_LOWEST           -2
  THREAD_PRIORITY_BELOW_NORMAL     -1
  THREAD_PRIORITY_NORMAL            0  (default)
  THREAD_PRIORITY_ABOVE_NORMAL     +1
  THREAD_PRIORITY_HIGHEST          +2
  THREAD_PRIORITY_TIME_CRITICAL   +15 (special)
```

```c
/* Windows: Set process and thread priorities */
#include <windows.h>

/* Set process priority class */
SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

/* Set thread priority (relative to process class) */
HANDLE hThread = GetCurrentThread();
SetThreadPriority(hThread, THREAD_PRIORITY_ABOVE_NORMAL);

/* Query current priority */
int prio = GetThreadPriority(hThread);

/* SetThreadAffinityMask: pin thread to specific CPUs */
DWORD_PTR mask = 0x3;  /* CPUs 0 and 1 */
SetThreadAffinityMask(hThread, mask);

/* NUMA affinity */
SetThreadIdealProcessorEx(hThread, &proc_info, NULL);
```

```powershell
# PowerShell: inspect and modify process priority
Get-Process -Name "myapp" | Select-Object PriorityClass, BasePriority, Id
(Get-Process -Id 1234).PriorityClass = "High"

# WMI: process CPU affinity
$proc = Get-WmiObject Win32_Process -Filter "ProcessId = 1234"
$proc.SetPriority(128)  # 128 = High

# Process Monitor / Process Explorer
# GUI: show per-thread CPU, priority, ideal CPU
# CLI equivalent with PowerShell
Get-Process -Name "myapp" | ForEach-Object {
    $_.Threads | Select-Object Id, CurrentPriority, TotalProcessorTime
}

# Perfmon counters for scheduling
Get-Counter '\System\Context Switches/sec'
Get-Counter '\Processor(_Total)\% Processor Time'
Get-Counter '\Thread(*)\Context Switches/sec'

# ETW tracing for scheduler events
xperf -on PROC_THREAD+LOADER -stackwalk Profile -f trace.etl
xperf -stop
# Analyze with Windows Performance Analyzer (WPA)
# Or: wpaexporter -i trace.etl -profile CpuUsage
```

---

## 19. macOS Scheduling Architecture & Darwin Quality-of-Service (QoS)

macOS (built on the XNU hybrid kernel) implements a priority-based preemptive scheduler that bridges low-level Mach thread priorities with high-level Quality-of-Service (QoS) classes.
On modern Apple Silicon (M-series architectures), the Darwin scheduler is asymmetric multiprocessing (AMP) aware, steering threads between high-performance **P-cores** and energy-efficient **E-cores**.

### Darwin Quality-of-Service (QoS) Classes

Instead of exposing raw integer priority values to application developers, macOS groups threads into semantic Quality-of-Service tiers:

```
Priority Tier                        Core Affinity (Apple Silicon)     Target Workload
+------------------------------------+--------------------------------+--------------------------------+
| QOS_CLASS_USER_INTERACTIVE (33)    | High-frequency P-cores only    | Main UI event loop, animations |
+------------------------------------+--------------------------------+--------------------------------+
| QOS_CLASS_USER_INITIATED   (25)    | Scheduled on P-cores           | User waiting on button click   |
+------------------------------------+--------------------------------+--------------------------------+
| QOS_CLASS_DEFAULT          (21)    | P-cores / E-cores balance      | Standard POSIX execution       |
+------------------------------------+--------------------------------+--------------------------------+
| QOS_CLASS_UTILITY          (17)    | Steered toward E-cores         | Long computation with progress |
+------------------------------------+--------------------------------+--------------------------------+
| QOS_CLASS_BACKGROUND        (9)    | Strictly E-cores (low power)   | Indexing, backup, sync tasks   |
+------------------------------------+--------------------------------+--------------------------------+
```

### Dynamic QoS Propagation and Priority Inversion Avoidance

When a high-priority `USER_INTERACTIVE` thread awaits the result of an asynchronous task running at `UTILITY` QoS (or blocks on a mutex held by a background thread), the Darwin kernel automatically elevates the background thread's QoS class to `USER_INTERACTIVE` (QoS donation).
This prevents priority inversion and ensures that background workers holding critical resources do not starve on low-frequency E-cores.

### Setting QoS in C / Objective-C

```c
// Configuring macOS thread QoS classes
#include <pthread.h>
#include <stdio.h>

void *background_worker(void *arg) {
    // Explicitly set calling thread to Background QoS (energy efficient)
    pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);

    printf("Executing background compute task on efficiency cores...\n");
    // Long running work
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, background_worker, NULL);
    pthread_join(tid, NULL);
    return 0;
}
```

### macOS Scheduling Inspection Commands

```bash
# macOS: Throttle an entire running process to background efficiency QoS (limits to E-cores)
taskpolicy -b -p <PID>

# macOS: Restore a process to standard interactive execution
taskpolicy -B -p <PID>

# macOS: Inspect Darwin scheduler sysctl tunables
sysctl kern.sched

# macOS: Profile core residency (P-core vs E-core usage) in real time
sudo powermetrics --samplers cpu_power -i 1000 -n 3
```

---

**Previous:** [P2L5: Thread Performance Considerations](../Part-2-Process-Thread-Management/P2L5-Thread-Performance-Considerations.md)
**Next:** [P3L2: Memory Management](P3L2-Memory-Management.md)


