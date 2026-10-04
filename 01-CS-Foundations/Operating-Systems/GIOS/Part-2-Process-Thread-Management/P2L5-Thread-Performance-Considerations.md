---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P2L5"
  - "The Art of Multiprocessor Programming, Herlihy & Shavit"
  - "Systems Performance, 2nd Ed., Brendan Gregg"
---

# P2L5: Thread Performance Considerations

> **Module goal:** Quantify multithreaded performance via Amdahl's Law, understand hardware effects (cache coherence, false sharing), compare synchronization mechanisms (spinlocks vs. sleep locks), and evaluate architectural patterns (thread pools, event-driven, pipeline).

## Table of Contents

- [1. Performance Metrics](#1-performance-metrics)
- [2. Measuring Multithreaded Performance](#2-measuring-multithreaded-performance)
- [3. Amdahl's Law](#3-amdahls-law)
- [4. Multiprocessor Architectures and Hardware Caching](#4-multiprocessor-architectures-and-hardware-caching)
- [5. Cache Coherence and Snooping Protocols](#5-cache-coherence-and-snooping-protocols)
- [6. False Sharing and Memory Alignment](#6-false-sharing-and-memory-alignment)
- [7. Synchronization Overhead: Spinlocks vs. Sleep Locks](#7-synchronization-overhead-spinlocks-vs-sleep-locks)
- [8. Context Switch Overhead](#8-context-switch-overhead)
- [9. Thread Pool Design Pattern](#9-thread-pool-design-pattern)
- [10. Web Server Architectures & The Flash Paper Case Study (SPED, MP, MT, AMPED)](#10-web-server-architectures--the-flash-paper-case-study-sped-mp-mt-amped)
- [11. Pipeline and Leader-Follower Patterns](#11-pipeline-and-leader-follower-patterns)
- [12. Quizzes and Exercises](#12-quizzes-and-exercises)
- [13. Key Takeaways](#13-key-takeaways)
- [14. Perf Flamegraph Analysis for Multithreaded Programs](#14-perf-flamegraph-analysis-for-multithreaded-programs)
- [15. The C10K Problem and Modern Solutions](#15-the-c10k-problem-and-modern-solutions)
- [16. NUMA Effects on Thread Performance](#16-numa-effects-on-thread-performance)
- [17. Thread Sanitizer (TSan): Catching Race Conditions](#17-thread-sanitizer-tsan-catching-race-conditions)
- [18. Cross-Platform Performance Profiling: Linux (perf/wrk), macOS (xctrace/powermetrics), Windows (xperf/WPR)](#18-cross-platform-performance-profiling-linux-perfwrk-macos-xctracepowermetrics-windows-xperfwpr)

---

## 1. Performance Metrics

| Metric | Definition | Formula |
|--------|-----------|---------|
| **Latency** | Time to complete a single operation | end_time - start_time |
| **Throughput** | Operations completed per unit time | ops / time |
| **Speedup** | Performance gain from parallelism | T_sequential / T_parallel |
| **Efficiency** | How well parallelism is utilized | Speedup / num_processors |
| **Utilization** | Fraction of time a resource is busy | busy_time / total_time |
| **Scalability** | How performance changes with resources | Speedup(N) / N |

```
Ideal scaling:        Real scaling:
Speedup               Speedup
  |     /                |    ___-------
  |    /                 |  /
  |   /                  | /
  |  /                   |/
  | /                    |
  +--------->            +--------->
    Processors             Processors
```

---

## 2. Measuring Multithreaded Performance

**Linux tools:**
```bash
# Wall clock time vs CPU time
time ./my_program
# real = wall clock (what the user experiences)
# user = CPU time in user mode
# sys  = CPU time in kernel mode
# Ideal: user + sys >> real means good parallelism
# Bad:   real >> user + sys means waiting on I/O

# Detailed perf analysis
perf stat -d ./my_program
# Shows: task-clock, context-switches, cpu-migrations,
#        cache-references, cache-misses, instructions, cycles

# Thread-level profiling
perf record -g --per-thread ./my_program
perf report

# See CPU utilization per core
mpstat -P ALL 1 5
```

**Windows tools:**
```powershell
# Measure execution time
Measure-Command { .\my_program.exe }

# CPU utilization per core
Get-Counter '\Processor(*)\% Processor Time' -SampleInterval 1 -MaxSamples 5
```

---

## 3. Amdahl's Law

**Amdahl's Law** gives the theoretical maximum speedup of a program with both serial and parallel portions:

```
                    1
Speedup(N) = ─────────────────
              S + (1 - S) / N

Where:
  S = fraction of work that is serial (cannot be parallelized)
  N = number of processors
  (1-S) = fraction of work that is parallelizable
```

### Visual Representation

```
Single processor:
[===== Serial (S=20%) =====][====== Parallel (80%) ======]
Total time: T = 1.0

4 processors:
[===== Serial (20%) =====][== P ==]
                          [== P ==]
                          [== P ==]
                          [== P ==]
Total time = 0.2 + 0.8/4 = 0.4
Speedup = 1.0 / 0.4 = 2.5x  (not 4x!)

Infinite processors:
[===== Serial (20%) =====] (parallel part -> 0)
Total time = 0.2
Speedup = 1.0 / 0.2 = 5x   (limit, even with infinite CPUs)
```

### Computed Speedups

| Serial fraction (S) | 2 CPUs | 4 CPUs | 8 CPUs | 16 CPUs | inf CPUs |
|---------------------|--------|--------|--------|---------|---------|
| 0% | 2.0x | 4.0x | 8.0x | 16.0x | inf |
| 5% | 1.9x | 3.5x | 5.9x | 9.1x | 20.0x |
| 10% | 1.8x | 3.1x | 4.7x | 6.4x | 10.0x |
| 25% | 1.6x | 2.3x | 2.9x | 3.4x | 4.0x |
| 50% | 1.3x | 1.6x | 1.8x | 1.9x | 2.0x |

**Key insight:** Even a small serial fraction severely limits scalability. With 10% serial code, you can never exceed 10x speedup no matter how many CPUs you add.

> **Quiz: Amdahl's Law**
>
> *A program is 90% parallelizable. What is the maximum speedup with 8 processors?*
>
> Speedup = 1 / (0.1 + 0.9/8) = 1 / (0.1 + 0.1125) = 1 / 0.2125 = **4.71x**

### Gustafson's Law (Complementary View)

Amdahl's Law assumes a fixed problem size. **Gustafson's Law** considers that with more processors, we often solve larger problems:

```
Scaled Speedup = N - S * (N - 1)

Where:
  N = number of processors
  S = serial fraction (measured on the parallel system)
```

With 8 processors and 10% serial: Scaled Speedup = 8 - 0.1 * 7 = **7.3x**

---

## 4. Multiprocessor Architectures and Hardware Caching

### Cache Hierarchy

```
CPU Core 0          CPU Core 1          CPU Core 2          CPU Core 3
+----------+        +----------+        +----------+        +----------+
| Registers|        | Registers|        | Registers|        | Registers|
| ~0.3 ns  |        | ~0.3 ns  |        | ~0.3 ns  |        | ~0.3 ns  |
+----------+        +----------+        +----------+        +----------+
| L1 Cache |        | L1 Cache |        | L1 Cache |        | L1 Cache |
| 32-64 KB |        | 32-64 KB |        | 32-64 KB |        | 32-64 KB |
| ~1 ns    |        | ~1 ns    |        | ~1 ns    |        | ~1 ns    |
+----------+        +----------+        +----------+        +----------+
| L2 Cache |        | L2 Cache |        | L2 Cache |        | L2 Cache |
| 256 KB-  |        | 256 KB-  |        | 256 KB-  |        | 256 KB-  |
| 1 MB     |        | 1 MB     |        | 1 MB     |        | 1 MB     |
| ~4-7 ns  |        | ~4-7 ns  |        | ~4-7 ns  |        | ~4-7 ns  |
+----+-----+        +----+-----+        +----+-----+        +----+-----+
     |                    |                   |                    |
+----+--------------------+-------------------+--------------------+----+
|                    L3 Cache (Shared)                                   |
|                    8-64 MB  ~10-20 ns                                  |
+-----------------------------------------------------------------------+
                              |
                    +---------+---------+
                    |    Main Memory    |
                    |   (DRAM, DDR5)   |
                    |   ~50-100 ns     |
                    +------------------+
```

---

## 5. Cache Coherence and Snooping Protocols

When multiple cores cache the same memory location, they must agree on the value. This is the **cache coherence** problem.

### MESI Protocol

Each cache line is in one of four states:

| State | Meaning | Can Read? | Can Write? |
|-------|---------|-----------|------------|
| **M**odified | Only copy, dirty (changed) | Yes | Yes |
| **E**xclusive | Only copy, clean | Yes | Yes (transitions to M) |
| **S**hared | Multiple copies, clean | Yes | No (must invalidate others first) |
| **I**nvalid | Not in cache | No | No |

```
Core 0 reads X:     Core 0: X=E       Core 1: X=I
Core 1 reads X:     Core 0: X=S       Core 1: X=S
Core 0 writes X:    Core 0: X=M       Core 1: X=I  (invalidated!)
Core 1 reads X:     Core 0: X=S(flush) Core 1: X=S  (fetched from Core 0)
```

**Cost of coherence:** When Core 0 writes to a cache line that Core 1 also has, Core 1's copy must be invalidated. If Core 1 then reads it, the data must be fetched from Core 0's cache (or main memory). This takes ~30-100 ns instead of ~1 ns.

---

## 6. False Sharing and Memory Alignment

**False sharing** occurs when two threads modify **different** variables that happen to reside on the **same cache line** (typically 64 bytes).

```
Cache Line (64 bytes):
+------+------+------+------+------+------+------+------+
| var_a| var_b| .... | .... | .... | .... | .... | .... |
+------+------+------+------+------+------+------+------+
   ^                                   
   |       Thread 0 writes var_a        
   |       Thread 1 writes var_b        
   |
   Both variables are on the SAME cache line!
   
Result: Every write by Thread 0 invalidates Thread 1's cache line,
and vice versa. Even though they access DIFFERENT variables.
This is pure overhead with no actual data sharing.
```

### Demonstrating False Sharing

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ITERATIONS 100000000

// BAD: counters on the same cache line
struct bad_counters {
    long counter0;  // offset 0
    long counter1;  // offset 8 -- SAME CACHE LINE as counter0
};

// GOOD: counters on different cache lines
struct good_counters {
    long counter0;
    char padding[56];  // pad to 64-byte cache line boundary
    long counter1;     // now on a DIFFERENT cache line
} __attribute__((aligned(64)));

struct bad_counters bad = {0, 0};
struct good_counters good = {0, {0}, 0};

void *increment_bad_0(void *arg) {
    for (long i = 0; i < NUM_ITERATIONS; i++)
        bad.counter0++;
    return NULL;
}
void *increment_bad_1(void *arg) {
    for (long i = 0; i < NUM_ITERATIONS; i++)
        bad.counter1++;
    return NULL;
}
void *increment_good_0(void *arg) {
    for (long i = 0; i < NUM_ITERATIONS; i++)
        good.counter0++;
    return NULL;
}
void *increment_good_1(void *arg) {
    for (long i = 0; i < NUM_ITERATIONS; i++)
        good.counter1++;
    return NULL;
}

int main(void) {
    pthread_t t0, t1;
    struct timespec start, end;

    // Test: false sharing (bad)
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&t0, NULL, increment_bad_0, NULL);
    pthread_create(&t1, NULL, increment_bad_1, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double bad_time = (end.tv_sec - start.tv_sec) +
                      (end.tv_nsec - start.tv_nsec) / 1e9;

    // Test: no false sharing (good)
    clock_gettime(CLOCK_MONOTONIC, &start);
    pthread_create(&t0, NULL, increment_good_0, NULL);
    pthread_create(&t1, NULL, increment_good_1, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    clock_gettime(CLOCK_MONOTONIC, &end);
    double good_time = (end.tv_sec - start.tv_sec) +
                       (end.tv_nsec - start.tv_nsec) / 1e9;

    printf("False sharing (bad):  %.3f sec\n", bad_time);
    printf("No false sharing:     %.3f sec\n", good_time);
    printf("Speedup from fixing:  %.2fx\n", bad_time / good_time);

    return 0;
}
```

```bash
gcc -O2 -pthread -o false_sharing false_sharing.c && ./false_sharing
# Typical output:
# False sharing (bad):  1.200 sec
# No false sharing:     0.300 sec
# Speedup from fixing:  4.00x
```

**Linux - detect false sharing with perf:**
```bash
perf c2c record ./false_sharing
perf c2c report
# Shows cache lines with high HITM (Hit Modified) counts
# indicating cross-core cache line bouncing
```

> **Quiz: False Sharing**
>
> *Two threads each increment their own counter in a struct. The struct has no mutex. Why is performance poor?*
>
> **Answer:** **False sharing**. Both counters share a cache line. Each write invalidates the other core's cached copy, forcing expensive cache-to-cache transfers on every increment. Fix: pad or align counters to separate cache lines.

---

## 7. Synchronization Overhead: Spinlocks vs. Sleep Locks

### Spinlocks

A **spinlock** busy-waits (spins) in a loop until the lock becomes available:

```c
// Naive spinlock (test-and-set)
void spin_lock(volatile int *lock) {
    while (__sync_lock_test_and_set(lock, 1)) {
        // Spin (busy wait)
    }
}

void spin_unlock(volatile int *lock) {
    __sync_lock_release(lock);
}
```

### Sleep Locks (Mutexes)

A **sleep lock** puts the thread to sleep (deschedules it) if the lock is held:

```c
// pthread_mutex_lock() is a sleep lock (via futex on Linux):
// 1. Try atomic CAS (fast path, no syscall)
// 2. If contended, futex(FUTEX_WAIT) - kernel deschedules thread
```

### Comparison

| Factor | Spinlock | Sleep Lock (Mutex) |
|--------|----------|-------------------|
| Wait mechanism | Busy-wait (burns CPU) | Sleep (frees CPU) |
| Overhead when uncontended | Very low (~5-10 ns) | Low (~25 ns, atomic CAS) |
| Overhead when contended | High (CPU spinning) | Moderate (context switch ~1-5 us) |
| Best for | Short critical sections (<1 us) | Long critical sections (>1 us) |
| Multiprocessor only? | Practically yes | Works on single-core too |
| In kernel or user space? | Both (kernel spinlocks are common) | Usually user space |

**Rule of thumb:** Use spinlocks when the expected wait time is less than the cost of two context switches (~5-10 us). Use sleep locks otherwise.

### Adaptive Locks

Some implementations combine both: spin for a short time, then sleep if the lock is still held (e.g., Linux kernel mutexes, Java synchronized blocks on HotSpot).

---

## 8. Context Switch Overhead

Context switch cost has two components:

```
Total context switch cost = Direct cost + Indirect cost

Direct cost (~1-5 us):
  - Save/restore registers
  - Switch kernel stacks
  - Switch page tables (if process switch)
  - Mode transitions (user -> kernel -> user)

Indirect cost (~10-1000+ us):
  - TLB misses (cold TLB for new process)
  - Cache misses (cold cache)
  - Branch predictor pollution
  - Prefetch queue flush

The indirect cost DOMINATES.
```

**Linux - measure context switch overhead:**
```bash
# Using perf
perf stat -e context-switches,cpu-migrations,cache-misses ./my_program

# Using lmbench
# lat_ctx -s 0 2  # switch latency between 2 processes

# Using /proc for per-process stats
cat /proc/$$/status | grep ctxt
# voluntary_ctxt_switches: 150
# nonvoluntary_ctxt_switches: 5
```

---

## 9. Thread Pool Design Pattern

Instead of creating/destroying threads per task, reuse a pool of pre-created threads:

```
Thread Pool Architecture:
                                  +--------+
                                  | Worker |<--+
                                  | Thread |   |
                                  +--------+   |
+--------+    +-----------+       +--------+   | Process
| Client |    |   Task    |       | Worker |   | tasks
| Submit |--->|   Queue   |------>| Thread |   | from
| Tasks  |    | [T1][T2]  |       +--------+   | queue
+--------+    | [T3][T4]  |       +--------+   |
              +-----------+       | Worker |   |
                                  | Thread |<--+
                                  +--------+
```

### Sizing Thread Pools

```
For CPU-bound tasks:
  pool_size = num_cores          (or num_cores + 1)

For I/O-bound tasks:
  pool_size = num_cores * (1 + wait_time / compute_time)

Example: 4 cores, tasks spend 80% time waiting on I/O:
  pool_size = 4 * (1 + 0.8/0.2) = 4 * 5 = 20 threads

For mixed workloads:
  Use separate pools for CPU-bound and I/O-bound tasks
```

### Linux Thread Pool Implementation

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct task {
    void (*function)(void*);
    void *arg;
    struct task *next;
} task_t;

typedef struct {
    task_t *head, *tail;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    pthread_t *threads;
    int num_threads;
    bool shutdown;
} threadpool_t;

void *pool_worker(void *arg) {
    threadpool_t *pool = (threadpool_t*)arg;
    while (1) {
        pthread_mutex_lock(&pool->mutex);
        while (!pool->head && !pool->shutdown) {
            pthread_cond_wait(&pool->not_empty, &pool->mutex);
        }
        if (pool->shutdown && !pool->head) {
            pthread_mutex_unlock(&pool->mutex);
            break;
        }
        task_t *task = pool->head;
        pool->head = task->next;
        if (!pool->head) pool->tail = NULL;
        pthread_mutex_unlock(&pool->mutex);

        task->function(task->arg);
        free(task);
    }
    return NULL;
}

threadpool_t *threadpool_create(int num_threads) {
    threadpool_t *pool = calloc(1, sizeof(threadpool_t));
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(pthread_t));
    pthread_mutex_init(&pool->mutex, NULL);
    pthread_cond_init(&pool->not_empty, NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&pool->threads[i], NULL, pool_worker, pool);
    }
    return pool;
}

void threadpool_submit(threadpool_t *pool, void (*fn)(void*), void *arg) {
    task_t *task = malloc(sizeof(task_t));
    task->function = fn;
    task->arg = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->mutex);
    if (pool->tail) pool->tail->next = task;
    else pool->head = task;
    pool->tail = task;
    pthread_cond_signal(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex);
}

void threadpool_destroy(threadpool_t *pool) {
    pthread_mutex_lock(&pool->mutex);
    pool->shutdown = true;
    pthread_cond_broadcast(&pool->not_empty);
    pthread_mutex_unlock(&pool->mutex);

    for (int i = 0; i < pool->num_threads; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    free(pool->threads);
    pthread_mutex_destroy(&pool->mutex);
    pthread_cond_destroy(&pool->not_empty);
    free(pool);
}

// Usage
void task_function(void *arg) {
    int id = (intptr_t)arg;
    printf("Task %d executed by thread %lu\n", id, pthread_self());
    usleep(100000);
}

int main(void) {
    threadpool_t *pool = threadpool_create(4);

    for (int i = 0; i < 20; i++) {
        threadpool_submit(pool, task_function, (void*)(intptr_t)i);
    }

    sleep(3);
    threadpool_destroy(pool);
    printf("All tasks completed.\n");
    return 0;
}
```

```bash
gcc -pthread -o threadpool threadpool.c && ./threadpool
```

---

## 10. Web Server Architectures & The Flash Paper Case Study (SPED, MP, MT, AMPED)

The landmark paper *"Flash: An Efficient and Portable Web Server"* (Vivek S. Pai, Peter Druschel, and Willy Zwaenepoel, USENIX 1999) provides the foundational systems framework for analyzing concurrent server architectures.

### The Four Server Architectures

```
+-----------------------------------------------------------------------------------------+
| Architecture           | Concurrency Mechanism        | Disk I/O Handling               |
+------------------------+------------------------------+---------------------------------+
| SPED (Single Process   | Single event-driven process  | Synchronous blocking disk reads |
| Event Driven)          | (select / poll / epoll)      | (Blocks entire server on miss!) |
+------------------------+------------------------------+---------------------------------+
| MP (Multi-Process)     | Multiple independent OS      | Blocking read in worker process |
|                        | processes (e.g., Apache 1.3) | (Only calling process blocks)   |
+------------------------+------------------------------+---------------------------------+
| MT (Multi-Threaded)    | Single process, multiple     | Blocking read in worker thread  |
|                        | kernel threads (thread pool) | (Only calling thread blocks)    |
+------------------------+------------------------------+---------------------------------+
| AMPED (Asymmetric      | Main event loop process +    | Main loop checks cache via      |
| Multi-Process Event    | auxiliary helper processes   | mincore(); helper reads disk    |
| Driven - Flash)        | for disk I/O                 | asynchronously                  |
+------------------------+------------------------------+---------------------------------+
```

### The Blocking Disk I/O Dilemma in SPED

In an event-driven server, non-blocking network socket I/O is achieved cleanly using `select()`, `poll()`, or `epoll()`.
However, standard UNIX file system operations (`read()`, `write()`) do **not** support non-blocking execution on local disk files:
- When a requested file is already resident in the operating system's buffer cache (page cache), calling `read()` copies data in sub-microsecond time without blocking.
- When a requested file is **not** in the buffer cache (cold cache miss), the calling thread is put to sleep in the kernel while the disk controller seeks and reads physical sectors (taking 5 to 10 ms).
- In a pure **SPED** server, this blocking read freezes the single execution thread, halting the entire server and leaving thousands of ready network clients unserviced.

### AMPED Mechanics: The Flash Solution

Flash resolves the disk blocking dilemma by combining the zero-overhead event-driven paradigm with an asymmetric pool of lightweight helper processes:

```
[ Incoming Network Clients ]
            |
            v
+========================================================================+
| Flash Main Process (Single-Threaded Event Loop)                        |
| - Handles non-blocking network I/O (epoll / select)                    |
| - Inspects OS Buffer Cache via mincore() system call                   |
+========================================================================+
            |                                           |
    [ Cache Hit: In RAM ]                      [ Cache Miss: On Disk ]
            |                                           |
            v                                           v
    Stream directly to socket              Delegate to Helper Process via IPC
    (Zero context switch)                               |
                                                        v
                                           +=============================+
                                           | Auxiliary Helper Process    |
                                           | - Executes blocking read()  |
                                           | - Pulls file into OS Cache  |
                                           +=============================+
                                                        |
                                            [ Page now in OS Cache ]
                                                        |
                                                        v
                                           Notify Main Event Loop (pipe/socket)
```

1. **`mincore()` Cache Residency Check:** When a client requests a file, the main event loop uses `mmap()` and queries `mincore()` to verify whether the target memory pages are currently present in physical RAM.
2. **Fast Path (RAM Hit):** If `mincore()` confirms the pages are in memory, the main process writes the data directly to the client socket without blocking.
3. **Slow Path (Disk Miss):** If `mincore()` indicates a cache miss, the main process delegates the file read to an auxiliary helper process over an IPC socket and immediately resumes servicing other network clients. The helper process executes the blocking disk `read()`, bringing the pages into the kernel buffer cache. When the read completes, the helper sends a short completion notification to the main process, which can now stream the cached pages with zero disk delay.

### Performance Observations: Analysis of the Flash Benchmark

```
Throughput (req/s)
   ^
   |        /---- SPED (Highest when in-memory)
   |       /--- Flash (AMPED: 2-5% lower than SPED due to mincore overhead)
   |      /-- MT (Context switches & lock contention degrade in-memory throughput)
   |     /- MP (Lowest in-memory throughput due to heavy process context switches)
   |
   +---------------------------------------------------------------> Dataset Size
                |                                      |
         < 100 MB (In-Memory)                   > 100 MB (Disk-Bound)
         - SPED > Flash > MT > MP               - Flash >= MT > MP >> SPED (SPED collapses!)
```

#### Why Flash Performs Slightly Worse than SPED for Small Datasets (< 100 MB)
When the working set fits entirely in physical RAM (< 100 MB), every request is a buffer cache hit:
- **SPED** achieves peak throughput because it has zero disk faults, zero context switches, zero inter-process communication, and zero lock synchronization.
- **Flash (AMPED)** performs approximately 2% to 5% worse than SPED because Flash must execute the `mincore()` system call to verify cache residency before every transfer, incurring minor syscall overhead that SPED avoids.

#### Why Flash Performs Significantly Better than MP for Small Datasets (< 100 MB)
Even when data fits entirely in memory:
- **Multi-Process (MP)** servers (e.g., Apache prefork) assign each connection to a separate OS process. Context-switching between hundreds of processes pollutes CPU L1/L2 caches and the TLB, consumes massive process memory tables, and incurs scheduling latency.
- **Flash** services all connections inside a single thread with hot instruction and data caches, completely eliminating process context switching and inter-thread contention.

#### Why SPED Collapses for Large Datasets (> 100 MB)
When the dataset exceeds available RAM:
- **SPED** throughput plummets because every cache miss causes the entire server to block on synchronous disk I/O.
- **Flash** maintains high throughput because its helper processes absorb the blocking disk I/O while the main event loop continues delivering network data.

---

## 11. Pipeline and Leader-Follower Patterns

### Pipeline Pattern

```
Stage 1          Stage 2          Stage 3
(Thread 1)       (Thread 2)       (Thread 3)
+---------+      +---------+      +---------+
| Parse   |----->| Process |----->| Respond |
| request |queue | data    |queue | to      |
|         |      |         |      | client  |
+---------+      +---------+      +---------+

Each stage runs in its own thread(s).
Stages communicate via queues.
Throughput = rate of slowest stage.
```

### Leader-Follower Pattern

```
State machine for each thread:

  +-----------+    accept()     +-----------+
  | Follower  |  ----------->  |  Leader   |
  | (waiting  |                | (listening|
  |  in pool) |                |  on socket)|
  +-----------+                +-----------+
       ^                            |
       |                            | connection arrives
       |                            v
       |                      +-----------+
       |    done processing   | Processing|
       +<-------------------  | (handling |
                              |  request) |
                              +-----------+

Only one thread (the "leader") waits for new connections.
When a connection arrives, the leader promotes a follower
to be the new leader, then handles the request itself.

Advantage over thread pool: no queue contention
Disadvantage: more complex state management
```

---

## 12. Quizzes and Exercises

> [!question] Quiz 1: Flash Web Server Performance Observations (Pai et al. Paper - Midterm Question 8)
> In the landmark paper *"Flash: An Efficient and Portable Web Server"* (Pai et al., 1999), the authors evaluate web server throughput across varying dataset sizes.
> For datasets where the total dataset size is **less than 100 MB** (fits entirely in the OS buffer cache in RAM):
> 1. Why does Flash perform slightly worse than SPED?
> 2. Why does Flash perform significantly better than MP (Multi-Process)?

> [!success]- Answer
> 1. **Why Flash performs worse than SPED (< 100 MB):**
>    - When the dataset is < 100 MB, the entire working set resides in RAM; every file access is a buffer cache hit.
>    - **SPED** operates as a single-threaded event loop and performs direct reads without disk blocking. It incurs zero context switches, zero IPC overhead, and zero synchronization overhead.
>    - **Flash (AMPED)** attempts to detect cold disk misses by querying the kernel via the `mincore()` system call before serving every file. For in-memory datasets where no disk misses actually occur, this `mincore()` check represents redundant system call overhead (~2-5% latency penalty) that SPED completely avoids.
> 2. **Why Flash performs better than MP (< 100 MB):**
>    - **Multi-Process (MP)** servers (e.g., Apache prefork) assign each connection to an independent operating system process.
>    - Even when data is in RAM, MP incurs substantial context-switching overhead across hundreds of processes, severe cache and TLB pollution (evicting cache lines on each switch), high memory footprint for per-process page tables, and inter-process synchronization contention.
>    - **Flash** handles all connections in a single address space with zero context switching, maintaining hot instruction and data caches across all active client requests.

> [!question] Quiz 2: Amdahl's Law and Scaling Limits
> A computational pipeline executes in 100 seconds on a single CPU core.
> Profiling indicates that 10% of the execution time is strictly sequential (data loading and lock synchronization), while 90% can be parallelized.
> 1. What is the theoretical maximum speedup achievable with an infinite number of CPU cores?
> 2. What is the speedup achieved with 16 CPU cores?
> 3. What is the parallel efficiency achieved with 16 CPU cores?

> [!success]- Answer
> 1. **Maximum Speedup ($N \to \infty$):**
>    $$S_{\text{max}} = \frac{1}{S} = \frac{1}{0.10} = 10\times$$
>    No matter how many CPU cores are added, execution time cannot fall below 10 seconds.
> 2. **Speedup with $N = 16$ cores:**
>    $$S(16) = \frac{1}{S + \frac{1 - S}{N}} = \frac{1}{0.10 + \frac{0.90}{16}} = \frac{1}{0.10 + 0.05625} = \frac{1}{0.15625} = 6.4\times$$
>    Execution time on 16 cores: $\frac{100\text{ s}}{6.4} = 15.625\text{ seconds}$.
> 3. **Parallel Efficiency:**
>    $$E(16) = \frac{S(16)}{N} = \frac{6.4}{16} = 0.40 = 40\%$$
>    Only 40% of the available 16-core compute capacity is converted into productive speedup; the remaining 60% is lost to sequential bottlenecks.

> [!question] Quiz 3: Models and Memory Footprint (Clips 203-204)
> Compare the memory consumption and kernel overhead of servicing 10,000 concurrent client connections using:
> 1. Multi-Process Architecture (1 connection per process).
> 2. Multi-Threaded Architecture (1 connection per kernel thread).
> 3. Event-Driven Architecture (Single process event loop with non-blocking sockets).

> [!success]- Answer
> 1. **Multi-Process:** Highest memory overhead. Each process requires private page tables, file descriptor tables, memory-mapped shared libraries, and PCB entries in kernel space. At ~1-4 MB per process, 10,000 processes consume 10-40 GB of RAM, causing memory exhaustion and thrashing.
> 2. **Multi-Threaded:** Moderate memory overhead. Threads share page tables and global heap, but each thread requires a private stack (typically 2-8 MB virtual stack, with ~64 KB resident memory) and kernel `task_struct` / thread control block (~8 KB). 10,000 threads consume ~1 GB of resident RAM plus high scheduler queue overhead.
> 3. **Event-Driven:** Lowest memory overhead. A single process handles all 10,000 connections using non-blocking socket file descriptors and an `epoll`/`kqueue` state machine. Each connection requires only a socket buffer and a small user-space connection state struct (~1-4 KB), consuming less than 50 MB total RAM for 10,000 clients.

> [!question] Quiz 4: Systems Experimental Design & Benchmarking Invariants (Clips 214-215)
> When measuring the throughput and latency of a concurrent network server, what three experimental precautions must an engineer take to ensure scientifically valid and reproducible results?

> [!success]- Answer
> 1. **Buffer Cache Warmup Runs:** Systems benchmarks must discard initial cold runs to ensure that OS file buffer caches and CPU instruction/data caches are adequately warmed up before timing commences, preventing disk seek latency from contaminating in-memory compute benchmarks.
> 2. **Client-Server Isolation:** The benchmarking client tool (e.g., `wrk`, `ab`) must execute on a physically separate machine from the server under test. Running load generators on the same machine causes client threads to compete with server worker threads for CPU time slices, L1/L2 caches, and memory bus bandwidth.
> 3. **Statistical Significance and Confidence Intervals:** Experiments must be repeated multiple times (typically 10-30 iterations). Engineers must report mean, standard deviation, and 99th percentile tail latency rather than isolated minimum or maximum outliers.

---

### Exercise: Benchmark False Sharing

Compile and run the false sharing benchmark from section 6. Vary the padding size (0, 8, 16, 32, 64 bytes) and plot the results.

### Exercise: Thread Pool vs. Thread-per-Task

Write a benchmark that creates 1000 short tasks (each takes 1ms). Compare:
1. Creating a new thread per task (1000 thread creates + joins)
2. Using a thread pool with 4 workers

Measure total wall-clock time and context switches.

---

## 13. Key Takeaways

1. **Amdahl's Law:** Speedup is limited by the serial fraction; even 5% serial limits you to 20x max speedup.
2. **False sharing** can degrade multi-threaded performance by 2-10x; pad shared data to cache line boundaries (64 bytes).
3. **Spinlocks** are better for very short critical sections (<1 us); **sleep locks** (mutexes) are better for longer ones.
4. **Thread pools** amortize thread creation cost; size them based on workload type (CPU-bound: N cores; I/O-bound: higher).
5. **Event-driven** architectures scale to thousands of connections with minimal overhead; combine with thread pools for CPU work.
6. The **indirect cost** of context switches (cache/TLB pollution) dominates the direct cost.
7. Profile before optimizing: use `perf`, `perf c2c`, and `valgrind --tool=cachegrind` to find the real bottleneck.

---

## 14. Perf Flamegraph Analysis for Multithreaded Programs

Flamegraphs visualize where CPU time is spent across all threads simultaneously.

```bash
# Install flamegraph tools
git clone https://github.com/brendangregg/FlameGraph
export PATH=$PATH:$(pwd)/FlameGraph

# Record all threads for 30 seconds (60Hz sampling)
perf record -F 60 -a -g --call-graph dwarf -p <pid> -- sleep 30

# OR: record a specific command
perf record -F 99 -g --call-graph dwarf -- ./multithreaded_app

# Generate flamegraph
perf script | stackcollapse-perf.pl | flamegraph.pl > flamegraph.svg
# Open in browser: firefox flamegraph.svg

# Per-thread flamegraph (separate svg per thread)
perf script | grep "thread_name" | stackcollapse-perf.pl | flamegraph.pl > per_thread.svg

# Off-CPU analysis (time blocked, not running)
# Useful to find threads waiting on locks, I/O, sleep
sudo offcputime-bpfcc -p <pid> 30 | stackcollapse.pl | flamegraph.pl --color=io > offcpu.svg

# Lock contention flamegraph
sudo offwaketime-bpfcc -p <pid> 10 | stackcollapse.pl | flamegraph.pl > wakeup.svg

# Differential flamegraph (before vs. after an optimization)
stackcollapse-perf.pl before.perf > before.folded
stackcollapse-perf.pl after.perf  > after.folded
difffolded.pl before.folded after.folded | flamegraph.pl > diff.svg
# Red = slower after; Blue = faster after
```

### perf c2c: Cache-to-Cache Bounce Detection

```bash
# Detect false sharing and true sharing in multi-core programs
perf c2c record -g -a -- ./multithreaded_program
perf c2c report --stdio

# Output interpretation:
# =======================================================
# Shared Data Cache Line Table     (Legend: hitm - True/False Sharing)
#             ----------- Cacheline ----------
#  Address  Node  PA cnt     LL       RmtHitm       LclHitm
# 0x7f...   0    1239     0.00%      45.23%         54.77%
# =======================================================
# High "Hitm" % = cache line bouncing between cores = false sharing!

# Alternative: valgrind cachegrind
valgrind --tool=cachegrind --branch-sim=yes ./program
cg_annotate cachegrind.out.<pid>

# Hardware PMU counters for cache misses
perf stat -e cache-misses,cache-references,L1-dcache-load-misses \
          -e LLC-load-misses ./program
```

---

## 15. The C10K Problem and Modern Solutions

The "C10K problem" (Dan Kegel, 1999): handle 10,000 concurrent connections on a single server.

```
Evolution of Concurrency Models:

1990s: Thread-per-connection
  10,000 connections = 10,000 OS threads
  Memory: 10,000 × 8MB stack = 80 GB  (IMPOSSIBLE)
  Context switches: millions/sec overhead

2000s: select/poll + event loop (C10K solved)
  Single thread, epoll for I/O readiness
  Node.js, nginx, Redis model
  Limitation: CPU-bound tasks block the event loop

2010s: Thread pool + async I/O hybrid
  Event loop dispatches to thread pool for CPU work
  Go goroutines, Python asyncio + executors
  Handles C100K (100,000 connections)

2020s: io_uring + lockless queues
  Near-zero-syscall async I/O
  Handles C1M+ connections on modern hardware
```

```c
/* Minimal event loop handling 10,000 connections */
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_EVENTS 64
#define THREAD_POOL_SIZE 4    /* CPU cores for CPU work */

/* Thread pool for CPU-intensive tasks */
typedef struct {
    int (*fn)(void *);
    void *arg;
} Task;

/* Hybrid: epoll event loop + thread pool */
void server_loop(int listen_fd) {
    int epfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];

    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    while (1) {
        int n = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == listen_fd) {
                /* Accept: O(1) with SO_REUSEPORT across threads */
                int client = accept4(listen_fd, NULL, NULL,
                                     SOCK_NONBLOCK | SOCK_CLOEXEC);
                ev.events = EPOLLIN | EPOLLET;  /* Edge-triggered */
                ev.data.fd = client;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client, &ev);
            } else {
                /* I/O ready: handle inline if fast, dispatch if slow */
                handle_client(events[i].data.fd);
            }
        }
    }
}
```

```bash
# Benchmark concurrency models
# Install: wrk (HTTP benchmarking tool)
wrk -t12 -c400 -d30s http://localhost:8080/

# ab (Apache Bench): simple connection count test
ab -n 100000 -c 1000 http://localhost:8080/

# vegeta: rate-limited load testing
echo "GET http://localhost:8080/" | vegeta attack -rate=10000 -duration=30s | vegeta report

# TCP connection count
ss -s                              # Socket statistics
ss -tn state established | wc -l   # Current established connections
cat /proc/sys/net/ipv4/tcp_max_syn_backlog  # SYN queue limit
cat /proc/sys/net/core/somaxconn           # Accept queue limit

# Increase limits for high concurrency
echo 65536 > /proc/sys/net/core/somaxconn
echo 65536 > /proc/sys/net/ipv4/tcp_max_syn_backlog
sysctl -w net.ipv4.ip_local_port_range="1024 65535"  # More ephemeral ports
ulimit -n 1048576   # File descriptor limit per process
```

---

## 16. NUMA Effects on Thread Performance

Non-Uniform Memory Access: memory latency depends on which NUMA node it lives on.

```
NUMA Topology (2-socket server):

Socket 0                    Socket 1
+------------------+        +------------------+
| Core 0-15        |        | Core 16-31       |
| L3 Cache (30MB)  |        | L3 Cache (30MB)  |
| Memory Node 0    |        | Memory Node 1    |
| (128 GB local)   |        | (128 GB local)   |
+------------------+        +------------------+
        |                           |
        +--------- QPI/UPI ---------+  (remote access: ~2x slower)
```

```bash
# Inspect NUMA topology
numactl --hardware
# node 0 cpus: 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
# node 0 size: 128982 MB
# node 0 free: 112234 MB
# node distances: node 0 = 10, node 1 = 21  (21/10 = 2.1x penalty!)

lstopo          # Graphical NUMA topology (hwloc)
lscpu | grep NUMA

# Run program on specific NUMA node (memory + CPU)
numactl --cpunodebind=0 --membind=0 ./program     # All on node 0
numactl --interleave=all ./program                # Interleave memory (databases)

# Check NUMA memory access statistics
numastat                        # Per-process NUMA hit/miss
numastat -p <pid>               # Specific process

# Monitor NUMA remote accesses (bad for performance!)
perf stat -e \
    node0/mem_load_l3_miss_retired.remote_dram/ \
    node0/mem_load_retired.local_pmm/ \
    ./program

# Auto NUMA balancing (kernel moves pages to preferred node)
cat /proc/sys/kernel/numa_balancing     # 0=disabled, 1=enabled
echo 1 > /proc/sys/kernel/numa_balancing

# Pin a process's threads to one NUMA node
taskset -c 0-15 ./program              # CPU mask
numactl --physcpubind=0-15 ./program   # NUMA-aware version

# Thread affinity within NUMA node (in code)
#include <numa.h>    /* Link with -lnuma */
struct bitmask *mask = numa_allocate_cpumask();
numa_node_to_cpus(0, mask);   /* Get CPUs on node 0 */
numa_run_on_node(0);           /* Pin current thread to node 0 */
void *mem = numa_alloc_onnode(size, 0);  /* Allocate on node 0 */
```

### NUMA-Aware Data Structure Design

```c
/* Bad: shared counter accessed by all NUMA nodes */
atomic_int global_counter;   /* Cache line bounces across nodes! */

/* Good: per-NUMA-node counters, summed when needed */
#define NUMA_NODES 2
#define CACHELINE 64
struct numa_counter {
    atomic_int value;
    char pad[CACHELINE - sizeof(atomic_int)];  /* Avoid false sharing */
} counters[NUMA_NODES];

void increment(int numa_node) {
    atomic_fetch_add(&counters[numa_node].value, 1);
}

long total(void) {
    long sum = 0;
    for (int i = 0; i < NUMA_NODES; i++)
        sum += atomic_load(&counters[i].value);
    return sum;
}
```

---

## 17. Thread Sanitizer (TSan): Catching Race Conditions

```bash
# Compile with TSan (Clang or GCC, ~5-15x slower)
gcc -fsanitize=thread -g -O1 -o program_tsan program.c -lpthread
# or
clang -fsanitize=thread -g -O1 -o program_tsan program.c -lpthread

# Run: TSan reports data races at runtime
./program_tsan

# Example TSan output:
# WARNING: ThreadSanitizer: data race (pid=12345)
#   Write of size 4 at 0x7f... by thread T2:
#     #0 increment() counter.c:15
#   Previous read of size 4 at 0x7f... by thread T1:
#     #0 print_count() counter.c:22
#   Thread T2 created by main thread at:
#     #0 pthread_create()
#     #1 main() counter.c:35

# TSan suppressions file (for known false positives)
cat tsan.supp
# race:some_known_benign_function

TSAN_OPTIONS="suppressions=tsan.supp" ./program_tsan

# AddressSanitizer for memory bugs in thread code
gcc -fsanitize=address,undefined -g -o program_asan program.c -lpthread

# Helgrind: Valgrind's race detector (more thorough, ~100x slower)
valgrind --tool=helgrind ./program

# DRD: Alternative Valgrind race detector
valgrind --tool=drd --read-var-info=yes ./program
```

---

## 18. Cross-Platform Performance Profiling: Linux (perf/wrk), macOS (xctrace/powermetrics), Windows (xperf/WPR)

Profiling concurrent multithreaded systems requires tracking hardware performance counters (cache misses, branch mispredictions, context switches) across operating systems.

### 1. Linux Performance Profiling

```bash
# High-concurrency HTTP benchmarking tool (wrk)
wrk -t4 -c100 -d30s --latency http://127.0.0.1:8080/index.html

# Hardware performance counter monitoring with perf
perf stat -e cycles,instructions,cache-references,cache-misses,context-switches,cpu-migrations ./my_program

# Pin threads/processes to specific CPU cores to evaluate cache affinity
taskset -c 0,2,4,6 ./my_server

# Analyze CPU cache-to-cache false sharing with perf c2c
sudo perf c2c record -F 60000 -- ./my_program
sudo perf c2c report --stdio
```

### 2. macOS Performance Profiling

```bash
# Profile CPU execution stacks using Apple Instruments CLI
xcrun xctrace record --template 'Time Profiler' --launch -- ./my_program

# Monitor CPU core energy, frequency residency, and memory bandwidth on Apple Silicon
sudo powermetrics --samplers cpu_power,gpu_power,thermal -i 1000 -n 5

# Trace mutex lock contention in real time using DTrace on macOS
sudo dtrace -n 'lockstat:::adaptive-block { @[execname, probename] = count(); }'

# Set macOS thread Quality-of-Service (QoS) classes in C:
# pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
```

### 3. Windows Performance Profiling

```powershell
# Record kernel execution trace with Windows Performance Recorder (WPR)
wpr.exe -start GeneralProfile -start CPU

# ... execute concurrent workload ...

# Save and analyze trace in Windows Performance Analyzer (WPA)
wpr.exe -stop C:\temp\perf_trace.etl

# Benchmark execution wall time via PowerShell
Measure-Command { .\my_program.exe }

# Monitor thread context switch rates and processor queue length in real time
Get-Counter -Counter "\System\Context Switches/sec", "\Processor(_Total)\% Processor Time" -Continuous
```

---

**Previous:** [P2L4: Thread Design Considerations](P2L4-Thread-Design-Considerations.md)
**Next:** [P3L1: Scheduling](../Part-3-Resource-Management/P3L1-Scheduling.md)


