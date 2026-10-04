---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P2L2"
  - "Operating System Concepts, 10th Ed., Silberschatz"
  - "Programming with POSIX Threads, Butenhof"
  - "Windows Internals, 7th Ed., Russinovich"
---

# P2L2: Threads and Concurrency

> **Module goal:** Understand threads as the unit of execution within a process, differentiate concurrency from parallelism, master synchronization primitives (mutexes, condition variables), and recognize concurrency pitfalls including deadlocks.

## Table of Contents

- [1. Visual Metaphor](#1-visual-metaphor)
- [2. What is a Thread?](#2-what-is-a-thread)
- [3. Process vs. Thread](#3-process-vs-thread)
- [4. Why Threads? Motivation and Benefits](#4-why-threads-motivation-and-benefits)
- [5. Concurrency vs. Parallelism](#5-concurrency-vs-parallelism)
- [6. Multithreading Use Cases](#6-multithreading-use-cases)
- [7. What Does a Thread Look Like?](#7-what-does-a-thread-look-like)
- [8. Thread Mechanisms: Execution and Data Structures](#8-thread-mechanisms-execution-and-data-structures)
- [9. Thread Creation and Lifecycle](#9-thread-creation-and-lifecycle)
- [10. Race Conditions and Critical Sections](#10-race-conditions-and-critical-sections)
- [11. Mutual Exclusion and Mutexes](#11-mutual-exclusion-and-mutexes)
- [12. Mutex Operations and Semantics](#12-mutex-operations-and-semantics)
- [13. Producer/Consumer Problem Overview](#13-producerconsumer-problem-overview)
- [14. Condition Variables](#14-condition-variables)
- [15. Condition Variable Wait and Signal Semantics](#15-condition-variable-wait-and-signal-semantics)
- [16. Reader/Writer Problem](#16-readerwriter-problem)
- [17. Common Concurrency Pitfalls: Deadlocks](#17-common-concurrency-pitfalls-deadlocks)
- [18. Deadlock Conditions and Prevention](#18-deadlock-conditions-and-prevention)
- [19. Multithreading Models & Contention Scopes](#19-multithreading-models--contention-scopes)
- [20. Multithreading Patterns (Boss-Worker, Pipeline, Layered)](#20-multithreading-patterns-boss-worker-pipeline-layered)
- [21. macOS Concurrency Architecture: Mach Threads, GCD, and os_unfair_lock](#21-macos-concurrency-architecture-mach-threads-gcd-and-os_unfair_lock)
- [22. Quizzes and Exercises](#22-quizzes-and-exercises)
- [23. Key Takeaways](#23-key-takeaways)

---

## 1. Visual Metaphor

If a process is an **order of work** in the toy shop, a **thread** is a single **worker** executing that order.

| Toy Shop | Thread |
|----------|--------|
| Worker assigned to an order | Thread within a process |
| Workers share the same workbench and materials | Threads share address space, heap, global variables |
| Each worker has their own hands and notepad | Each thread has its own stack and registers |
| Workers can help each other on the same order | Threads cooperate on the same task |
| Workers might bump into each other | Race conditions |
| Workers take turns using shared tools | Mutual exclusion (mutexes) |

---

## 2. What is a Thread?

A **thread** is the basic unit of CPU utilization within a process. It has its own:
- Program counter (instruction pointer)
- Register set
- Stack

But it **shares** with other threads in the same process:
- Address space (code, data, heap)
- Open file descriptors
- Signal handlers
- PID (from the kernel's perspective, on Linux, each thread has a unique TID)

```
Single-Threaded Process          Multi-Threaded Process
+---------------------+         +---------------------+
| Code    Data   Heap |         | Code    Data   Heap |
|                     |         |    (SHARED)          |
| +------+            |         | +------+ +------+ +------+
| |Stack |            |         | |Stack | |Stack | |Stack |
| |  T0  |            |         | |  T0  | |  T1  | |  T2  |
| +------+            |         | +------+ +------+ +------+
| Registers           |         | Regs T0  Regs T1  Regs T2 |
| PC                  |         | PC T0    PC T1    PC T2    |
+---------------------+         +---------------------+
```

---

## 3. Process vs. Thread

| Aspect | Process | Thread |
|--------|---------|--------|
| Address space | Own (isolated) | Shared with other threads |
| Creation cost | High (~ms; new page tables, VMAs) | Low (~us; just stack + TCB) |
| Context switch cost | High (TLB flush, cache pollution) | Low (same address space; no TLB flush) |
| Communication | IPC (pipes, sockets, shared memory) | Direct memory access (shared heap) |
| Fault isolation | Crash in one doesn't affect others | Crash in one thread kills entire process |
| Synchronization | Implicit (separate memory) | Explicit (mutexes, condvars needed) |
| Resource overhead | High (separate PCB, page tables, FD table) | Low (shared resources, small TCB) |

**Linux - thread vs process creation cost:**
```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

void *thread_func(void *arg) { return NULL; }

int main(void) {
    const int N = 10000;
    struct timespec start, end;

    // Measure fork() cost
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < N; i++) {
        pid_t pid = fork();
        if (pid == 0) _exit(0);
        waitpid(pid, NULL, 0);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double fork_time = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec) / 1e9;

    // Measure pthread_create() cost
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < N; i++) {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_func, NULL);
        pthread_join(tid, NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    double thread_time = (end.tv_sec - start.tv_sec) +
                         (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("fork():          %.1f us per creation\n", fork_time / N * 1e6);
    printf("pthread_create(): %.1f us per creation\n", thread_time / N * 1e6);
    printf("Ratio: fork is %.1fx slower\n", fork_time / thread_time);
    return 0;
}
```

```bash
gcc -O2 -pthread -o thread_vs_fork thread_vs_fork.c && ./thread_vs_fork
# Typical: fork ~100-300 us, pthread_create ~10-30 us, ratio ~5-10x
```

---

## 4. Why Threads? Motivation and Benefits

### 1. Parallelism

On a multi-core machine, threads can run truly simultaneously:

```
4-Core CPU:
  Core 0: [Thread 0 - matrix row 0-249    ]
  Core 1: [Thread 1 - matrix row 250-499  ]
  Core 2: [Thread 2 - matrix row 500-749  ]
  Core 3: [Thread 3 - matrix row 750-999  ]

Total time: ~T/4 instead of T (ideal speedup)
```

### 2. Concurrency (Even on Single Core)

Threads allow overlapping I/O with computation:

```
Single-threaded:
  [Read file A] [Process A] [Read file B] [Process B]
  Total: 4 time units

Multi-threaded:
  Thread 1: [Read A]        [Read B]
  Thread 2:          [Process A]      [Process B]
  Total: ~3 time units (I/O overlapped with computation)
```

### 3. Resource Sharing

Threads in the same process share memory without any IPC overhead:
- No `pipe()`, `shmget()`, or serialization needed
- Just read/write to shared variables (with proper synchronization)

### 4. Responsiveness

A UI thread can remain responsive while worker threads perform heavy computation:

```
Main Thread (UI):  [Handle click] [Render frame] [Handle click] ...
Worker Thread:     [Compute FFT for 500ms                        ]

Without threads: UI freezes during 500ms FFT computation.
With threads: UI remains responsive; worker runs in background.
```

---

## 5. Concurrency vs. Parallelism

These terms are often confused but are distinct concepts:

```
Concurrency (logical):
  Managing multiple tasks that make progress.
  Possible on a single core via time-slicing.

  Core 0: [T1][T2][T1][T3][T2][T1][T3]...
  All three tasks make progress, but only one runs at any instant.

Parallelism (physical):
  Multiple tasks literally executing at the same instant.
  Requires multiple cores/CPUs.

  Core 0: [T1][T1][T1][T1]...
  Core 1: [T2][T2][T2][T2]...
  Core 2: [T3][T3][T3][T3]...
  Three tasks running simultaneously.
```

| | Concurrency | Parallelism |
|--|------------|-------------|
| Definition | Multiple tasks in progress | Multiple tasks executing simultaneously |
| Hardware | Single core sufficient | Multiple cores required |
| Analogy | Juggling (one ball in hand at a time) | Multiple people each holding a ball |
| Goal | Structure/responsiveness | Speed/throughput |

**All parallel systems are concurrent, but not all concurrent systems are parallel.**

---

## 6. Multithreading Use Cases

| Use Case | Why Threads | Example |
|----------|------------|---------|
| Web servers | Handle thousands of clients concurrently | Apache (thread-per-connection), Nginx (event + threads) |
| Database systems | Concurrent query execution | PostgreSQL (process-per-connection), MySQL (thread-per-connection) |
| GUI applications | Keep UI responsive during computation | Any desktop app with background tasks |
| Scientific computing | Parallelize matrix/vector operations | OpenMP parallel for loops |
| Game engines | Separate rendering, physics, AI, audio | Unreal Engine, Unity |
| Financial trading | Parallel strategy evaluation, market data processing | Low-latency trading systems |

---

## 7. What Does a Thread Look Like?

```
Process Memory Layout with Threads
+===========================================================+
| Code (Text) Segment        [SHARED - read only]            |
+===========================================================+
| Data Segment (globals)     [SHARED - read/write]           |
+===========================================================+
| Heap                       [SHARED - read/write]           |
| malloc/new allocations are visible to all threads          |
+===========================================================+
|                                                            |
| Thread 0 Stack  | Thread 1 Stack  | Thread 2 Stack        |
| [local vars]    | [local vars]    | [local vars]          |
| [call frames]   | [call frames]   | [call frames]         |
| [return addrs]  | [return addrs]  | [return addrs]        |
| (grows down)    | (grows down)    | (grows down)          |
|                                                            |
+===========================================================+

Each thread also has its own:
  - Program Counter (PC/RIP)
  - Stack Pointer (SP/RSP)
  - General-purpose registers
  - Thread-local storage (TLS)
```

**Linux - see threads of a process:**
```bash
# List threads of a process
ps -T -p $PID
# or
ls /proc/$PID/task/

# Thread-level info
cat /proc/$PID/task/$TID/status

# htop: press H to toggle thread view
# top:  press H to show threads
```

**Windows - see threads:**
```powershell
# List threads of a process
(Get-Process -Id $PID).Threads | Select-Object Id, ThreadState, WaitReason,
    TotalProcessorTime, StartTime

# Or using Process Explorer (Sysinternals)
```

---

## 8. Thread Mechanisms: Execution and Data Structures

### Thread Control Block (TCB)

Each thread has a small data structure (the TCB) that stores its execution context:

```
Thread Control Block (TCB):
+----------------------------------+
| Thread ID (TID)                  |
| Program Counter                  |
| Stack Pointer                    |
| Register set                     |
| Thread state (running/ready/...) |
| Priority                         |
| Pointer to owning PCB            |
| Thread-local storage pointer     |
| Signal mask                      |
+----------------------------------+

Size: Typically a few hundred bytes to a few KB
(vs. PCB which is several KB including memory mappings)
```

### Linux Thread Data Structure

In Linux, threads and processes both use `task_struct`. A thread is simply a `task_struct` that shares its `mm_struct` (memory descriptor) with other threads in the same thread group.

```bash
# A process with 4 threads has 4 task_struct entries,
# all sharing the same mm_struct.
# The TGID (Thread Group ID) equals the PID of the main thread.

# Verify: all threads share the same address space
ls -la /proc/$PID/task/*/maps | head -5
# All map files are identical (same inode)
```

---

## 9. Thread Creation and Lifecycle

### Thread Lifecycle

```
        create()
          |
          v
     +---------+
     |  READY  |<-----------+
     +---------+             |
          |                  | preempted / yield
     dispatched              |
          |                  |
          v                  |
     +---------+             |
     | RUNNING |-------------+
     +---------+
      |       |
 wait/block   | return/exit/cancel
      |       |
      v       v
 +---------+ +-----------+
 | WAITING | | TERMINATED |
 +---------+ |  (ZOMBIE   |
      |      |   until    |
      |      |   joined)  |
   event/    +-----------+
   signal
      |
      v
    READY
```

### Linux (PThreads):
```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *worker(void *arg) {
    int id = *(int*)arg;
    printf("Thread %d: started (TID=%lu)\n", id, pthread_self());
    sleep(1);  // Simulate work
    printf("Thread %d: done\n", id);
    int *result = malloc(sizeof(int));
    *result = id * 10;
    return result;
}

int main(void) {
    pthread_t threads[3];
    int ids[3] = {0, 1, 2};

    // Create threads
    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, worker, &ids[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", ret);
            return 1;
        }
    }

    // Join threads (wait for completion and get return value)
    for (int i = 0; i < 3; i++) {
        void *retval;
        pthread_join(threads[i], &retval);
        printf("Main: thread %d returned %d\n", i, *(int*)retval);
        free(retval);
    }

    printf("Main: all threads done\n");
    return 0;
}
```

```bash
gcc -pthread -o threads threads.c && ./threads
```

### Windows (Win32 Threads):
```c
#include <windows.h>
#include <stdio.h>

DWORD WINAPI worker(LPVOID arg) {
    int id = *(int*)arg;
    printf("Thread %d: started (TID=%lu)\n", id, GetCurrentThreadId());
    Sleep(1000);  // Simulate work (milliseconds)
    printf("Thread %d: done\n", id);
    return id * 10;
}

int main(void) {
    HANDLE threads[3];
    int ids[3] = {0, 1, 2};

    // Create threads
    for (int i = 0; i < 3; i++) {
        threads[i] = CreateThread(
            NULL,           // default security
            0,              // default stack size
            worker,         // thread function
            &ids[i],        // argument
            0,              // run immediately
            NULL            // don't need thread ID
        );
        if (!threads[i]) {
            printf("CreateThread failed: %lu\n", GetLastError());
            return 1;
        }
    }

    // Wait for all threads
    WaitForMultipleObjects(3, threads, TRUE, INFINITE);

    // Get exit codes
    for (int i = 0; i < 3; i++) {
        DWORD exitCode;
        GetExitCodeThread(threads[i], &exitCode);
        printf("Main: thread %d returned %lu\n", i, exitCode);
        CloseHandle(threads[i]);
    }

    printf("Main: all threads done\n");
    return 0;
}
```

```powershell
cl /Fe:threads.exe threads.c
.\threads.exe
```

---

## 10. Race Conditions and Critical Sections

A **race condition** occurs when the outcome of a program depends on the relative timing of thread execution.

### Example: Unsynchronized Counter

```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;  // Shared variable - NO PROTECTION

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  // NOT ATOMIC!
        // This compiles to:
        //   mov eax, [counter]   ; load
        //   add eax, 1           ; increment
        //   mov [counter], eax   ; store
        // Another thread can interleave between these instructions
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Expected: 2000000, Got: %d\n", counter);
    // Output will be LESS than 2000000 due to race condition
    return 0;
}
```

```bash
gcc -pthread -o race race.c && for i in $(seq 5); do ./race; done
# Each run gives a different (incorrect) result
```

### The Race Condition Explained

```
Thread 1                         Thread 2
--------                         --------
load counter (= 100)
                                 load counter (= 100)
add 1 (= 101)
                                 add 1 (= 101)
store counter (= 101)
                                 store counter (= 101)  <-- LOST UPDATE!

Both threads incremented, but counter only went from 100 to 101 (not 102).
This is a "lost update" race condition.
```

### Critical Section

A **critical section** is a region of code that accesses shared resources and must not be executed by more than one thread at a time.

```
Non-critical section (private data, local variables)
    |
    v
+---+-----------------------------------+
| CRITICAL SECTION                       |  <-- Only one thread at a time
| - Read/modify shared variable         |
| - Access shared data structure        |
| - Write to shared file                |
+---+-----------------------------------+
    |
    v
Non-critical section
```

**Requirements for a correct critical section solution:**
1. **Mutual exclusion:** At most one thread in the critical section at a time
2. **Progress:** If no thread is in the CS, a waiting thread must be allowed to enter
3. **Bounded waiting:** A thread cannot be starved (must eventually enter)

---

## 11. Mutual Exclusion and Mutexes

A **mutex** (mutual exclusion lock) enforces that only one thread can hold the lock at a time.

```
Without mutex:                    With mutex:
T1: [  CS  ]                     T1: [lock][  CS  ][unlock]
T2:    [  CS  ]  OVERLAP!        T2:       [wait][lock][  CS  ][unlock]
                 Race condition!              No overlap. Correct.
```

### Mutex Semantics

```
Mutex States:
  UNLOCKED (free)  ---> lock() by Thread A ---> LOCKED (owned by A)
  LOCKED (owned)   ---> lock() by Thread B ---> Thread B BLOCKS (sleeps)
  LOCKED (owned)   ---> unlock() by Thread A -> UNLOCKED
                                                 Thread B WAKES UP, acquires lock
```

**Critical rule:** Only the thread that locked a mutex should unlock it.
Unlocking a mutex you don't own is undefined behavior.

---

## 12. Mutex Operations and Semantics

### Linux (PThreads Mutex):
```c
#include <stdio.h>
#include <pthread.h>

int counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        pthread_mutex_lock(&lock);    // Acquire: blocks if already held
        counter++;                     // Critical section: safe
        pthread_mutex_unlock(&lock);   // Release: wakes a waiting thread
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Expected: 2000000, Got: %d\n", counter);
    // Now always outputs 2000000
    return 0;
}
```

```bash
gcc -pthread -o mutex_demo mutex_demo.c && for i in $(seq 5); do ./mutex_demo; done
# Every run: Expected: 2000000, Got: 2000000
```

### Windows (CRITICAL_SECTION and SRWLOCK):
```c
#include <windows.h>
#include <stdio.h>

int counter = 0;
CRITICAL_SECTION cs;

DWORD WINAPI increment(LPVOID arg) {
    for (int i = 0; i < 1000000; i++) {
        EnterCriticalSection(&cs);
        counter++;
        LeaveCriticalSection(&cs);
    }
    return 0;
}

int main(void) {
    InitializeCriticalSection(&cs);

    HANDLE t1 = CreateThread(NULL, 0, increment, NULL, 0, NULL);
    HANDLE t2 = CreateThread(NULL, 0, increment, NULL, 0, NULL);

    WaitForSingleObject(t1, INFINITE);
    WaitForSingleObject(t2, INFINITE);

    printf("Expected: 2000000, Got: %d\n", counter);

    CloseHandle(t1);
    CloseHandle(t2);
    DeleteCriticalSection(&cs);
    return 0;
}
```

> **Quiz: Mutex**
>
> *A mutex is locked by Thread A. Thread B calls `lock()` on the same mutex. What happens?*
>
> **Answer:** Thread B **blocks** (goes to sleep). It is placed in the mutex's wait queue. When Thread A calls `unlock()`, Thread B is woken up and acquires the lock. This is different from a **spinlock** where Thread B would busy-wait (burn CPU cycles).

---

## 13. Producer/Consumer Problem Overview

The **producer/consumer** (bounded buffer) problem is a classic concurrency problem:

```
Producer(s):                    Consumer(s):
+----------+    +----------+   +----------+
|          |--->|  Buffer  |-->|          |
| Produce  |    | [_][_][_]|   | Consume  |
| items    |    | capacity |   | items    |
+----------+    +----------+   +----------+

Constraints:
1. Producer must WAIT if buffer is FULL
2. Consumer must WAIT if buffer is EMPTY
3. Only one thread may modify the buffer at a time (mutex)
```

A mutex alone is insufficient because:
- A mutex can enforce "only one at a time" but cannot express "wait until the buffer is not full"
- We need a mechanism to **wait for a condition** and be **notified** when it changes

This leads to **condition variables**.

---

## 14. Condition Variables

A **condition variable** allows threads to wait for a specific condition to become true, and to be notified when it changes.

```
Condition Variable Operations:
  wait(cv, mutex):
    1. Atomically: release mutex AND block on cv
    2. When signaled: re-acquire mutex and return
    
  signal(cv):    Wake up ONE waiting thread
  broadcast(cv): Wake up ALL waiting threads
```

**Why wait releases the mutex:** If a thread held the mutex while sleeping, no other thread could enter the critical section to change the condition.

### Producer/Consumer with Condition Variables

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0;
int in = 0, out = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        int item = id * 100 + i;

        pthread_mutex_lock(&mutex);

        // Wait while buffer is full
        while (count == BUFFER_SIZE) {
            printf("Producer %d: buffer full, waiting...\n", id);
            pthread_cond_wait(&not_full, &mutex);
        }

        // Produce item
        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        count++;
        printf("Producer %d: produced %d (count=%d)\n", id, item, count);

        // Signal a waiting consumer
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *consumer(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        pthread_mutex_lock(&mutex);

        // Wait while buffer is empty
        while (count == 0) {
            printf("Consumer %d: buffer empty, waiting...\n", id);
            pthread_cond_wait(&not_empty, &mutex);
        }

        // Consume item
        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer %d: consumed %d (count=%d)\n", id, item, count);

        // Signal a waiting producer
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void) {
    pthread_t prod[2], cons[2];
    int ids[] = {0, 1};

    for (int i = 0; i < 2; i++) {
        pthread_create(&prod[i], NULL, producer, &ids[i]);
        pthread_create(&cons[i], NULL, consumer, &ids[i]);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(prod[i], NULL);
        pthread_join(cons[i], NULL);
    }
    printf("All done.\n");
    return 0;
}
```

```bash
gcc -pthread -o prodcons prodcons.c && ./prodcons
```

**Windows equivalent uses CONDITION_VARIABLE:**
```c
#include <windows.h>
#include <stdio.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int count = 0, in_idx = 0, out_idx = 0;

CRITICAL_SECTION cs;
CONDITION_VARIABLE cv_not_full;
CONDITION_VARIABLE cv_not_empty;

DWORD WINAPI producer(LPVOID arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        EnterCriticalSection(&cs);
        while (count == BUFFER_SIZE) {
            SleepConditionVariableCS(&cv_not_full, &cs, INFINITE);
        }
        buffer[in_idx] = id * 100 + i;
        in_idx = (in_idx + 1) % BUFFER_SIZE;
        count++;
        printf("Producer %d: produced (count=%d)\n", id, count);
        WakeConditionVariable(&cv_not_empty);
        LeaveCriticalSection(&cs);
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 10; i++) {
        EnterCriticalSection(&cs);
        while (count == 0) {
            SleepConditionVariableCS(&cv_not_empty, &cs, INFINITE);
        }
        int item = buffer[out_idx];
        out_idx = (out_idx + 1) % BUFFER_SIZE;
        count--;
        printf("Consumer %d: consumed (count=%d)\n", id, count);
        WakeConditionVariable(&cv_not_full);
        LeaveCriticalSection(&cs);
    }
    return 0;
}

int main(void) {
    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&cv_not_full);
    InitializeConditionVariable(&cv_not_empty);

    int ids[] = {0, 1};
    HANDLE threads[4];
    threads[0] = CreateThread(NULL, 0, producer, &ids[0], 0, NULL);
    threads[1] = CreateThread(NULL, 0, producer, &ids[1], 0, NULL);
    threads[2] = CreateThread(NULL, 0, consumer, &ids[0], 0, NULL);
    threads[3] = CreateThread(NULL, 0, consumer, &ids[1], 0, NULL);

    WaitForMultipleObjects(4, threads, TRUE, INFINITE);
    for (int i = 0; i < 4; i++) CloseHandle(threads[i]);
    DeleteCriticalSection(&cs);
    printf("All done.\n");
    return 0;
}
```

---

## 15. Condition Variable Wait and Signal Semantics

### Why Use `while` Instead of `if`?

```c
// WRONG - using if:
if (count == 0) {
    pthread_cond_wait(&not_empty, &mutex);  // Spurious wakeup possible!
}
// After wakeup, count might STILL be 0 (another consumer grabbed the item)

// CORRECT - using while:
while (count == 0) {
    pthread_cond_wait(&not_empty, &mutex);
}
// After wakeup, re-checks the condition. Safe against:
// 1. Spurious wakeups (POSIX allows them)
// 2. Stolen wakeups (another thread consumed the item first)
```

**Spurious wakeups** can occur because:
- The OS implementation may wake threads for internal reasons
- POSIX explicitly permits this behavior
- The `while` loop pattern is the only correct approach

### Signal vs. Broadcast

| Operation | Effect | When to Use |
|-----------|--------|-------------|
| `signal()` | Wakes ONE waiting thread | When any one waiter can make progress |
| `broadcast()` | Wakes ALL waiting threads | When the condition change might affect multiple waiters differently |

**Rule of thumb:**
- Use `signal()` when producing one item (one consumer can proceed)
- Use `broadcast()` when the state change could affect different waiters differently (e.g., changing from "exclusive lock" to "free" where multiple readers might proceed)

> **Quiz: Condition Variable**
>
> *Why must `pthread_cond_wait()` atomically release the mutex and block?*
>
> **Answer:** If these were separate operations, there would be a race condition:
> 1. Thread A releases mutex
> 2. Thread B acquires mutex, changes condition, signals CV
> 3. Thread A blocks on CV (but the signal was already sent - missed!)
>
> By making release+block atomic, no signal can be missed between the two operations.

---

## 16. Reader/Writer Problem

Multiple **readers** can access shared data simultaneously, but a **writer** needs exclusive access.

```
Read-Read:   SAFE (no data modification)
Read-Write:  UNSAFE (reader might see partial update)
Write-Write: UNSAFE (data corruption)

Access Matrix:
         | Reader | Writer |
---------+--------+--------+
Reader   |  OK    | BLOCK  |
Writer   | BLOCK  | BLOCK  |
```

### Implementation with Mutexes and Condition Variables

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t readers_ok;
    pthread_cond_t writer_ok;
    int readers;           // number of active readers
    int writer;            // 1 if a writer is active
    int waiting_writers;   // writers waiting (for priority)
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    pthread_mutex_init(&rw->mutex, NULL);
    pthread_cond_init(&rw->readers_ok, NULL);
    pthread_cond_init(&rw->writer_ok, NULL);
    rw->readers = 0;
    rw->writer = 0;
    rw->waiting_writers = 0;
}

void rwlock_read_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    // Wait if a writer is active or writers are waiting (writer preference)
    while (rw->writer || rw->waiting_writers > 0) {
        pthread_cond_wait(&rw->readers_ok, &rw->mutex);
    }
    rw->readers++;
    pthread_mutex_unlock(&rw->mutex);
}

void rwlock_read_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->readers--;
    if (rw->readers == 0) {
        pthread_cond_signal(&rw->writer_ok);  // Last reader signals writer
    }
    pthread_mutex_unlock(&rw->mutex);
}

void rwlock_write_lock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->waiting_writers++;
    while (rw->writer || rw->readers > 0) {
        pthread_cond_wait(&rw->writer_ok, &rw->mutex);
    }
    rw->waiting_writers--;
    rw->writer = 1;
    pthread_mutex_unlock(&rw->mutex);
}

void rwlock_write_unlock(rwlock_t *rw) {
    pthread_mutex_lock(&rw->mutex);
    rw->writer = 0;
    pthread_cond_broadcast(&rw->readers_ok);  // Wake all waiting readers
    pthread_cond_signal(&rw->writer_ok);      // Wake one waiting writer
    pthread_mutex_unlock(&rw->mutex);
}

// Usage example
rwlock_t rw;
int shared_data = 0;

void *reader(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 5; i++) {
        rwlock_read_lock(&rw);
        printf("Reader %d: read %d\n", id, shared_data);
        usleep(100000);  // Simulate read time
        rwlock_read_unlock(&rw);
        usleep(50000);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) {
        rwlock_write_lock(&rw);
        shared_data++;
        printf("Writer %d: wrote %d\n", id, shared_data);
        usleep(200000);  // Simulate write time
        rwlock_write_unlock(&rw);
        usleep(100000);
    }
    return NULL;
}

int main(void) {
    rwlock_init(&rw);
    pthread_t readers[3], writers[2];
    int rids[] = {0, 1, 2};
    int wids[] = {0, 1};

    for (int i = 0; i < 3; i++)
        pthread_create(&readers[i], NULL, reader, &rids[i]);
    for (int i = 0; i < 2; i++)
        pthread_create(&writers[i], NULL, writer, &wids[i]);

    for (int i = 0; i < 3; i++) pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(writers[i], NULL);

    printf("Final value: %d\n", shared_data);
    return 0;
}
```

**Windows has a native SRWLOCK (Slim Reader/Writer Lock):**
```c
#include <windows.h>
#include <stdio.h>

SRWLOCK srw = SRWLOCK_INIT;  // No cleanup needed
int shared_data = 0;

DWORD WINAPI reader(LPVOID arg) {
    AcquireSRWLockShared(&srw);
    printf("Reader: %d\n", shared_data);
    ReleaseSRWLockShared(&srw);
    return 0;
}

DWORD WINAPI writer(LPVOID arg) {
    AcquireSRWLockExclusive(&srw);
    shared_data++;
    printf("Writer: %d\n", shared_data);
    ReleaseSRWLockExclusive(&srw);
    return 0;
}
```

---

## 17. Common Concurrency Pitfalls: Deadlocks

A **deadlock** occurs when two or more threads are blocked forever, each waiting for a resource held by another.

```
Deadlock Example:
                    wants Lock B
Thread A: [holds Lock A] ----------> [waiting for Lock B]
                                           |
                                           | held by
                                           v
Thread B: [holds Lock B] ----------> [waiting for Lock A]
                    wants Lock A

Neither can proceed. System is stuck.
```

### Classic Deadlock Code

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    pthread_mutex_lock(&lock_a);
    printf("Thread 1: acquired lock_a\n");
    sleep(1);  // Increase chance of deadlock
    printf("Thread 1: waiting for lock_b...\n");
    pthread_mutex_lock(&lock_b);  // DEADLOCK: thread2 holds lock_b
    printf("Thread 1: acquired lock_b\n");
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&lock_b);
    printf("Thread 2: acquired lock_b\n");
    sleep(1);
    printf("Thread 2: waiting for lock_a...\n");
    pthread_mutex_lock(&lock_a);  // DEADLOCK: thread1 holds lock_a
    printf("Thread 2: acquired lock_a\n");
    pthread_mutex_unlock(&lock_a);
    pthread_mutex_unlock(&lock_b);
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_join(t1, NULL);  // Hangs forever
    pthread_join(t2, NULL);
    return 0;
}
```

```bash
gcc -pthread -o deadlock deadlock.c && timeout 5 ./deadlock
# Program hangs. Use Ctrl+C to kill.
```

---

## 18. Deadlock Conditions and Prevention

### Four Necessary Conditions (Coffman Conditions)

All four must hold simultaneously for a deadlock to occur:

| Condition | Description |
|-----------|-------------|
| **1. Mutual exclusion** | At least one resource is held in non-sharable mode |
| **2. Hold and wait** | A thread holds at least one resource and is waiting for additional resources |
| **3. No preemption** | Resources cannot be forcibly taken away; only the holder can release |
| **4. Circular wait** | A circular chain of threads exists, each waiting for a resource held by the next |

### Prevention Strategies

| Strategy | Breaks Which Condition | How |
|----------|----------------------|-----|
| **Lock ordering** | Circular wait | Always acquire locks in a global order |
| **Try-lock** | Hold and wait | Use `trylock()`; release all if one fails |
| **Timeout** | Hold and wait | Give up after a timeout, retry |
| **Single lock** | Hold and wait | Use one lock for all resources (reduces concurrency) |
| **Lock-free algorithms** | Mutual exclusion | Use atomic operations instead of locks |

### Fix: Lock Ordering

```c
// FIXED: Both threads acquire locks in the same order (A before B)
void *thread1(void *arg) {
    pthread_mutex_lock(&lock_a);   // Always lock A first
    pthread_mutex_lock(&lock_b);   // Then lock B
    // Critical section
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&lock_a);   // Same order: A first
    pthread_mutex_lock(&lock_b);   // Then B
    // Critical section
    pthread_mutex_unlock(&lock_b);
    pthread_mutex_unlock(&lock_a);
    return NULL;
}
```

### Fix: Try-Lock Pattern

```c
void *thread_trylock(void *arg) {
    while (1) {
        pthread_mutex_lock(&lock_a);
        if (pthread_mutex_trylock(&lock_b) == 0) {
            // Got both locks
            // Critical section
            pthread_mutex_unlock(&lock_b);
            pthread_mutex_unlock(&lock_a);
            break;
        }
        // Couldn't get lock_b; release lock_a and retry
        pthread_mutex_unlock(&lock_a);
        sched_yield();  // Let other threads run
    }
    return NULL;
}
```

**Linux - deadlock detection:**
```bash
# Detect deadlocked threads using gdb
gdb -p $PID
# (gdb) thread apply all bt   # backtrace all threads
# Look for threads stuck in __lll_lock_wait (futex)

# Using valgrind's helgrind tool (detects lock order violations)
valgrind --tool=helgrind ./my_program

# Using ThreadSanitizer (compile-time)
gcc -fsanitize=thread -g -o my_program my_program.c -lpthread
./my_program
```

**Windows - deadlock detection:**
```powershell
# WinDbg: !locks command shows held locks and their owners
# Process Explorer: Threads tab shows wait chain analysis
# Wait Chain Traversal (WCT) API in Windows SDK
```

---

---

## 19. Multithreading Models & Contention Scopes

Operating systems support multithreading across two distinct levels: **User-Level Threads (ULT)** managed entirely in user space by a runtime library, and **Kernel-Level Threads (KLT)** managed directly by the operating system kernel scheduler.

```
+-------------------------------------------------------------------------------+
| User Space          [ ULT 1 ]  [ ULT 2 ]  [ ULT 3 ]  [ ULT 4 ]  [ ULT 5 ]    |
|                     \_______/  \_______/  \_______/  \_______/  \_______/    |
| Thread Library           \         |         /           \         /          |
|                           +--------+--------+             +-------+           |
+====================================|==========================|===============+
| Kernel Space                       v                          v               |
|                              [ KLT / LWP 1 ]            [ KLT / LWP 2 ]       |
|                                    |                          |               |
| CPU Schedulers                     v                          v               |
|                              +------------+             +------------+        |
| Physical Hardware            | CPU Core 0 |             | CPU Core 1 |        |
|                              +------------+             +------------+        |
+-------------------------------------------------------------------------------+
```

### The Three Multithreading Models

| Model | Mapping (ULT:KLT) | Advantages | Disadvantages | Historical & Modern Examples |
|-------|-------------------|------------|---------------|------------------------------|
| **Many-to-One (N:1)** | Many ULTs map to 1 KLT | Ultra-fast context switches (no syscalls); minimal memory footprint; portable. | One blocking system call blocks all ULTs; cannot utilize multiple CPU cores concurrently. | GNU Portable Threads (Pth), Java Green Threads, early Ruby runtimes. |
| **One-to-One (1:1)** | 1 ULT maps to 1 KLT | True multi-core parallelism; blocking syscall on one thread does not stall other threads. | Heavyweight creation overhead; kernel resource limits; expensive context switches. | Modern Linux (NPTL), Windows Win32 Threads, macOS Darwin POSIX threads. |
| **Many-to-Many (M:N)** | M ULTs multiplexed over N KLTs | Best of both worlds: light context switching with multi-core scalability; non-blocking. | Complex two-level scheduling; scheduler activations required; high implementation complexity. | Solaris 2-9 (Lightweight Processes - LWPs), Go Runtime (Goroutines over OS threads). |

### Contention Scope: PCS versus SCS

POSIX threads define two scheduling contention scopes:

1. **Process Contention Scope (PCS - `PTHREAD_SCOPE_PROCESS`):**
   - User-level threads compete for execution time slices strictly against other user-level threads belonging to the *same process*.
   - Scheduling decisions are made entirely in user space by the threading library runtime without kernel intervention.
2. **System Contention Scope (SCS - `PTHREAD_SCOPE_SYSTEM`):**
   - Each thread competes directly against all other threads across the *entire operating system*.
   - Scheduling decisions are made by the kernel scheduler.
   - On Linux, Windows, and macOS, all threads default strictly to `PTHREAD_SCOPE_SYSTEM`.

---

## 20. Multithreading Patterns (Boss-Worker, Pipeline, Layered)

Structuring concurrent applications requires architectural design patterns that balance throughput, latency, and synchronization overhead.

### 1. Boss-Worker Pattern

In the Boss-Worker pattern, a single **Boss thread** is responsible for receiving inbound work requests (e.g., listening on network sockets) and distributing them to a pool of **Worker threads**.

```
                           +--------------+
                           |  Inbound     |
                           |  Requests    |
                           +------+-------+
                                  |
                                  v
                           +--------------+
                           |  Boss Thread |
                           +------+-------+
                                  |
              +-------------------+-------------------+
              |                   |                   |
              v                   v                   v
       +--------------+    +--------------+    +--------------+
       | Worker 1     |    | Worker 2     |    | Worker 3     |
       +--------------+    +--------------+    +--------------+
```

Key Trade-Offs and Variants:
- **Throughput Bottleneck:** Overall system throughput is strictly bounded by the rate at which the Boss can accept and assign tasks:
  $$\text{Throughput}_{\text{max}} = \frac{1}{T_{\text{boss}}}$$
  If the Boss spends significant time parsing requests, the Boss becomes saturated while Workers sit idle.
- **Worker Assignment Schemes:**
  - *Direct Assignment:* Boss tracks idle workers and signals a specific worker directly. Incurs high tracking overhead in the Boss.
  - *Shared Work Queue:* Boss places requests into a synchronized thread-safe queue. Workers dequeue tasks as they become free.
- **Worker Pool Sizing:**
  To optimize hardware utilization, the number of worker threads must account for the ratio of I/O wait time to CPU processing time:
  $$N_{\text{threads}} = N_{\text{cores}} \times \left(1 + \frac{\text{Wait Time}}{\text{Compute Time}}\right)$$
  If tasks are purely CPU-bound ($\text{Wait Time} = 0$), $N_{\text{threads}} = N_{\text{cores}}$. If tasks spend 80% of their time waiting on disk or network, $N_{\text{threads}} = N_{\text{cores}} \times 5$.

### 2. Pipeline Pattern

In the Pipeline pattern, task execution is divided into a sequence of discrete processing stages, analogous to an industrial assembly line:

```
[ Request In ] ---> [ Stage 1: Decode ] ---> [ Stage 2: Process ] ---> [ Stage 3: Output ] ---> [ Response Out ]
                          |                          |                         |
                    Thread Pool A              Thread Pool B             Thread Pool C
```

- Each stage is executed by one or more dedicated threads connected by bounded FIFO queues.
- **Throughput:** Governed entirely by the slowest stage in the pipeline (the pipeline bottleneck):
  $$\text{Throughput} = \min_{i} \left(\frac{1}{T_i}\right)$$
- **Benefits:** Keeps instruction and data caches hot because each thread repeatedly executes the same localized stage logic.

### 3. Layered Pattern

In the Layered pattern, related subtasks are grouped into hierarchical functional layers (e.g., Application Layer $\to$ Transaction Layer $\to$ Storage Layer $\to$ Hardware Driver Layer).
- Higher layers issue requests to lower layers; lower layers return results or fire asynchronous callbacks.
- Facilitates modular software architecture and clear synchronization boundaries.

---

## 21. macOS Concurrency Architecture: Mach Threads, GCD, and os_unfair_lock

macOS implements high-performance concurrency via a layered architecture spanning Mach threads, POSIX threads, and Grand Central Dispatch (GCD).

### Mach Threads and Grand Central Dispatch

1. **Mach Threads (`thread_act_t`):** The fundamental unit of execution scheduled by the XNU kernel. Mach threads have native support for thread affinity policy tags and Quality-of-Service (QoS) classes.
2. **Grand Central Dispatch (libdispatch):** Apple's preferred task-based concurrency engine. Instead of creating and managing raw threads manually, applications submit closures/blocks to FIFO dispatch queues:

```c
// Compiling on macOS: clang -Wall -O2 gcd_demo.c -o gcd_demo
#include <stdio.h>
#include <dispatch/dispatch.h>
#include <unistd.h>

int main(void) {
    // Create concurrent queue with user-initiated QoS
    dispatch_queue_t queue = dispatch_queue_create("com.gios.concurrent", DISPATCH_QUEUE_CONCURRENT);
    dispatch_group_t group = dispatch_group_create();

    for (int i = 0; i < 4; i++) {
        dispatch_group_async(group, queue, ^{
            printf("Worker task %d executing on thread %p\n", i, (void *)pthread_self());
            usleep(100000);
        });
    }

    // Wait for all group tasks to complete
    dispatch_group_wait(group, DISPATCH_TIME_FOREVER);
    printf("All concurrent tasks completed.\n");
    return 0;
}
```

### Low-Level Synchronization: `os_unfair_lock`

Historically, Darwin provided `OSSpinLock`.
However, Apple deprecated `OSSpinLock` because it causes catastrophic **priority inversion** under QoS-aware scheduling: if a low-priority thread holds the spinlock and a high-priority thread attempts to acquire it, the high-priority thread spins at 100% CPU, starving the low-priority thread and preventing it from ever releasing the lock.
Apple replaced it with `os_unfair_lock`, which tracks the owner thread and enforces kernel priority inheritance:

```c
#include <os/lock.h>

os_unfair_lock lock = OS_UNFAIR_LOCK_INIT;

void critical_section(void) {
    os_unfair_lock_lock(&lock);
    // Protected shared state access
    os_unfair_lock_unlock(&lock);
}
```

### macOS Thread Inspection Tools

```bash
# macOS: Inspect thread backtraces in real time using lldb
lldb -p <PID> -o "thread list" -o "thread apply all bt" -o "detach" -o "quit"

# macOS: Profile thread scheduling and CPU time using Instruments CLI
xcrun xctrace record --template 'Thread States' --launch -- ./my_program

# macOS: Trace thread synchronization blocks using dtrace
sudo dtrace -n 'lockstat:::adaptive-block { @[execname, probename] = count(); }'
```

---

## 22. Quizzes and Exercises

> [!question] Quiz 1: Multi-Threading on a Single CPU Core
> Is there any performance benefit to running a multi-threaded application on a computer with only 1 physical CPU core?
> Provide a concrete technical explanation to support your answer.

> [!success]- Answer
> **Yes.**
> While a single CPU core cannot provide **parallelism** (simultaneous execution of instructions), multithreading provides substantial **concurrency** by overlapping CPU computation with high-latency I/O operations.
> When Thread 1 initiates a blocking I/O operation (e.g., reading a disk file or awaiting an incoming network socket packet), the OS transitions Thread 1 to the `WAITING/BLOCKED` state and context-switches the CPU core to execute Thread 2. This hides I/O latency and maintains high CPU utilization rather than allowing the CPU to sit idle.

> [!question] Quiz 2: Critical Section Errors in Producer-Consumer (Clips 105-106)
> Examine the following producer and consumer pseudo-code implementations utilizing shared buffer, mutex, and condition variables:
> 
> ```c
> // Global Shared State
> int in = 0, out = 0, buffer[BUFFERSIZE];
> mutex_t m;
> cond_var_t not_empty, not_full;
> 
> // Producer Code
> while (more_to_produce) {
>     mutex_lock(&m);
>     if (out == (in + 1) % BUFFERSIZE) {   // Line P1
>         cond_wait(&not_full, &m);          // Line P2
>     }
>     buffer[in] = item;
>     in = (in + 1) % BUFFERSIZE;
>     cond_signal(&not_empty);               // Line P3
>     mutex_unlock(&m);                      // Line P4
> }
> 
> // Consumer Code
> while (more_to_consume) {
>     mutex_lock(&m);
>     if (in == out) {                       // Line C1
>         cond_wait(&not_empty, &m);         // Line C2
>     }
>     item = buffer[out];
>     out = (out + 1) % BUFFERSIZE;
>     cond_signal(&not_full);                // Line C3
>     mutex_unlock(&m);                      // Line C4
> }
> ```
> Identify the critical bugs in Lines P1 and C1, and explain what system failure occurs under Mesa condition variable semantics.

> [!success]- Answer
> **Critical Bug:** Lines P1 and C1 use `if` statements instead of `while` loops to evaluate the buffer condition before waiting.
> 
> **Failure Mechanics under Mesa Semantics:**
> 1. In Mesa semantics (standard in POSIX, Linux, Windows, macOS), `cond_signal()` moves a waiting thread from the condition variable wait queue to the mutex ready queue; it does **not** immediately yield the CPU or guarantee execution.
> 2. Suppose Consumer 1 finds the buffer empty (`in == out`) and waits in Line C2.
> 3. Producer creates an item, signals `not_empty` in Line P3, and releases the mutex.
> 4. Consumer 1 is awakened, but before Consumer 1 re-acquires the mutex, another thread (Consumer 2) executes, acquires the mutex, and consumes the newly produced item.
> 5. When Consumer 1 finally re-acquires the mutex, the buffer is empty again. Because Line C1 used an `if` statement, Consumer 1 proceeds to read from `buffer[out]`, causing an underflow error and reading garbage memory.
> 
> **Remedy:** Always wrap condition variable waits inside a predicate verification loop:
> ```c
> while (out == (in + 1) % BUFFERSIZE) { cond_wait(&not_full, &m); }
> while (in == out) { cond_wait(&not_empty, &m); }
> ```

> [!question] Quiz 3: Multithreading Models & Contention Scope (Clips 108-109)
> An operating system implements an M:N multithreading model where 100 user-level threads are multiplexed across 4 kernel-level threads on a 4-core machine.
> 1. What happens if one user-level thread executes an infinite CPU-bound loop?
> 2. What happens if one user-level thread executes a blocking `read()` system call?

> [!success]- Answer
> 1. **CPU-bound loop:** The user-level thread occupies one of the 4 kernel threads. The remaining 99 user-level threads can still be scheduled across the remaining 3 kernel-level threads on the other 3 CPU cores.
> 2. **Blocking `read()`:** The kernel thread executing the `read()` enters the kernel `WAITING` state. If the runtime library does not use **scheduler activations**, that underlying kernel thread is blocked, temporarily reducing the active execution capacity to 3 kernel threads for the remaining 99 user threads.

> [!question] Quiz 4: Multithreading Patterns Comparison (Clips 116-117)
> Consider a web service where requests involve three sequential steps: (1) TLS decryption and request validation, (2) database query execution, and (3) JSON serialization and network response.
> Contrast how this service would be architected under the **Boss-Worker pattern** versus the **Pipeline pattern**, highlighting cache behavior and throughput bottlenecks.

> [!success]- Answer
> 1. **Boss-Worker Pattern:**
>    - *Architecture:* The Boss thread accepts connections and assigns the entire request (Steps 1, 2, and 3) to an available worker thread. Each worker executes all three steps from beginning to end.
>    - *Cache Behavior:* Workers experience poor instruction cache locality because each thread alternates between cryptography code, database driver logic, and JSON serialization libraries.
>    - *Bottleneck:* Throughput scales with worker count until CPU or database contention is reached, bounded by Boss dispatch overhead.
> 2. **Pipeline Pattern:**
>    - *Architecture:* Request processing is split into 3 distinct stages connected by bounded queues. Thread Pool 1 executes TLS, Thread Pool 2 executes Database queries, and Thread Pool 3 executes JSON serialization.
>    - *Cache Behavior:* Outstanding cache locality. Threads in Pool 1 only execute crypto instructions; their CPU core instruction caches remain hot with crypto routines.
>    - *Bottleneck:* Throughput is strictly determined by the slowest stage (e.g., database query execution). If Stage 2 takes 50 ms while Stages 1 and 3 take 5 ms, Stages 1 and 3 will continuously block on full/empty queues unless Stage 2 is allocated more worker threads.

---

### Exercise: Lock-Free Counter with C11 Atomics

```c
// Lock-free atomic increment across threads without mutex locks
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("Expected: 2000000, Got: %d\n", counter);
    return 0;
}
```

```bash
# Linux / macOS compilation
gcc -pthread -O2 -o atomic_counter atomic_counter.c && ./atomic_counter
```

---

## 23. Key Takeaways

1. A **thread** is the unit of execution; threads share the process address space but have their own stack, registers, and program counter.
2. Threads are ~5-10x cheaper to create than processes and share memory directly without IPC.
3. **Concurrency** is about managing multiple tasks (possible on one core); **parallelism** is about running them simultaneously (needs multiple cores).
4. **Race conditions** occur when threads access shared data without synchronization; the outcome depends on scheduling order.
5. **Mutexes** enforce mutual exclusion: only one thread in the critical section at a time.
6. **Condition variables** let threads wait for a condition and be notified; always use `while` loops (not `if`) around `wait()`.
7. **Deadlocks** require four conditions (mutual exclusion, hold-and-wait, no preemption, circular wait); break any one to prevent them.
8. **Lock ordering** is the simplest and most reliable deadlock prevention strategy.
9. In **Mesa condition variable semantics**, signal is only an advisory hint; predicate verification in a while loop is mandatory.
10. The three classical multithreading patterns are **Boss-Worker**, **Pipeline**, and **Layered**, each optimizing different throughput, latency, and cache profiles.

---

**Previous:** [P2L1: Processes and Process Management](P2L1-Processes-and-Process-Management.md)
**Next:** [P2L3: Threads Case Study - PThreads](P2L3-PThreads-Case-Study.md)

