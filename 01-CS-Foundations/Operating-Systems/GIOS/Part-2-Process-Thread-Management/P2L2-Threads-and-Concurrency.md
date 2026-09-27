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
- [19. Quizzes and Exercises](#19-quizzes-and-exercises)
- [20. Key Takeaways](#20-key-takeaways)

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

## 19. Quizzes and Exercises

### Quiz 1: Thread Data Sharing

> *Which of these are shared between threads in the same process?*
> - (a) Heap memory - **Shared**
> - (b) Stack variables - **NOT shared** (each thread has its own stack)
> - (c) Global variables - **Shared**
> - (d) File descriptors - **Shared**
> - (e) Program counter - **NOT shared** (each thread has its own PC)

### Quiz 2: Deadlock Identification

> *Can a single-threaded program deadlock?*
>
> **Answer:** Yes, if it attempts to lock the same non-recursive mutex twice (self-deadlock). Use `PTHREAD_MUTEX_RECURSIVE` to allow re-locking by the same thread.

### Exercise: Thread-Safe Counter with Atomics

```c
// Alternative to mutex: use atomic operations (no lock needed)
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int counter = 0;

void *increment(void *arg) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&counter, 1);  // Lock-free, atomic increment
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
    // Always correct, and faster than mutex for simple operations
    return 0;
}
```

```bash
gcc -pthread -o atomic_counter atomic_counter.c && ./atomic_counter
```

---

## 20. Key Takeaways

1. A **thread** is the unit of execution; threads share the process address space but have their own stack and registers.
2. Threads are ~5-10x cheaper to create than processes and share memory directly without IPC.
3. **Concurrency** is about managing multiple tasks (possible on one core); **parallelism** is about running them simultaneously (needs multiple cores).
4. **Race conditions** occur when threads access shared data without synchronization; the outcome depends on scheduling order.
5. **Mutexes** enforce mutual exclusion: only one thread in the critical section at a time.
6. **Condition variables** let threads wait for a condition and be notified; always use `while` loops (not `if`) around `wait()`.
7. **Deadlocks** require four conditions (mutual exclusion, hold-and-wait, no preemption, circular wait); break any one to prevent them.
8. **Lock ordering** is the simplest and most reliable deadlock prevention strategy.

---

**Previous:** [P2L1: Processes and Process Management](P2L1-Processes-and-Process-Management.md)
**Next:** [P2L3: Threads Case Study - PThreads](P2L3-PThreads-Case-Study.md)
