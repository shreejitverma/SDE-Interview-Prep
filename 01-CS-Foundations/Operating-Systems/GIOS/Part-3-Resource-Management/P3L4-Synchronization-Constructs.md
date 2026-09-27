---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P3L4"
  - "The Art of Multiprocessor Programming, Herlihy & Shavit"
  - "Linux Kernel Development, 3rd Ed., Robert Love"
---

# P3L4: Synchronization Constructs

> **Module goal:** Master synchronization beyond mutexes - semaphores, monitors, reader-writer locks, barriers, spinlocks, RCU, and lock-free/wait-free data structures.

## Table of Contents

- [1. Synchronization Overview](#1-synchronization-overview)
- [2. Semaphores](#2-semaphores)
- [3. Monitors](#3-monitors)
- [4. Reader-Writer Locks](#4-reader-writer-locks)
- [5. Barriers](#5-barriers)
- [6. Spinlocks and Atomic Operations](#6-spinlocks-and-atomic-operations)
- [7. Read-Copy-Update (RCU)](#7-read-copy-update-rcu)
- [8. Lock-Free and Wait-Free Data Structures](#8-lock-free-and-wait-free-data-structures)
- [9. Deadlock Prevention and Detection](#9-deadlock-prevention-and-detection)
- [10. Windows Synchronization Primitives](#10-windows-synchronization-primitives)
- [11. Quizzes and Exercises](#11-quizzes-and-exercises)
- [12. Key Takeaways](#12-key-takeaways)

---

## 1. Synchronization Overview

```
Synchronization constructs spectrum:

Low-level                                          High-level
(hardware)                                         (OS/language)

Atomic ops   Spinlocks   Mutexes   Semaphores   Monitors   Channels
  CAS         spin_lock   futex    sem_wait     Java sync  Go chan
  FAA                    pthread   sem_post     C# lock
                         _mutex                 Python Lock
```

| Construct | Blocking? | Count? | Ownership? | Best For |
|-----------|-----------|--------|------------|----------|
| Mutex | Yes | Binary | Yes (only owner unlocks) | Protecting shared data |
| Semaphore | Yes | Counting | No (any thread can post) | Resource counting, signaling |
| Monitor | Yes | Binary + condvars | Yes | High-level mutual exclusion |
| RW Lock | Yes | Multiple readers OR one writer | Yes | Read-heavy workloads |
| Barrier | Yes | N threads | N/A | Phase synchronization |
| Spinlock | No (busy-wait) | Binary | Yes | Short critical sections, kernel |
| RCU | No (readers) | N/A | N/A | Read-mostly kernel data |

---

## 2. Semaphores

A semaphore is a **counter** with two atomic operations:
- **wait (P / down / acquire):** Decrement counter. If counter < 0, block.
- **post (V / up / release):** Increment counter. If threads are waiting, wake one.

```
Binary Semaphore (value = 0 or 1):
  Equivalent to a mutex (but no ownership - any thread can post)

Counting Semaphore (value = N):
  Controls access to a pool of N identical resources
```

### POSIX Semaphores

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAX_CONNECTIONS 3

sem_t conn_pool;

void *client(void *arg) {
    int id = (intptr_t)arg;

    printf("Client %d: waiting for connection...\n", id);
    sem_wait(&conn_pool);  // Decrement; block if 0

    printf("Client %d: GOT connection\n", id);
    sleep(2);  // Simulate work

    printf("Client %d: releasing connection\n", id);
    sem_post(&conn_pool);  // Increment; wake a waiter

    return NULL;
}

int main(void) {
    sem_init(&conn_pool, 0, MAX_CONNECTIONS);  // Init to 3

    pthread_t threads[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&threads[i], NULL, client, (void*)(intptr_t)i);
        usleep(100000);  // Stagger arrivals
    }

    for (int i = 0; i < 10; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&conn_pool);
    return 0;
}
```

### Named Semaphores (Inter-Process)

```c
// Process 1: create and wait
sem_t *sem = sem_open("/my_sem", O_CREAT, 0666, 0);
printf("Waiting for signal...\n");
sem_wait(sem);  // Blocks until process 2 posts
printf("Got signal!\n");
sem_close(sem);

// Process 2: post
sem_t *sem = sem_open("/my_sem", 0);
printf("Signaling...\n");
sem_post(sem);
sem_close(sem);
sem_unlink("/my_sem");
```

### Classic Semaphore Problem: Dining Philosophers (with Semaphores)

```c
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5
sem_t forks[N];

void *philosopher(void *arg) {
    int id = (intptr_t)arg;
    int left = id;
    int right = (id + 1) % N;

    // Resource ordering: always pick lower-numbered fork first
    int first = left < right ? left : right;
    int second = left < right ? right : left;

    for (int i = 0; i < 3; i++) {
        printf("Philosopher %d: thinking\n", id);
        usleep(100000);

        sem_wait(&forks[first]);
        sem_wait(&forks[second]);

        printf("Philosopher %d: eating\n", id);
        usleep(100000);

        sem_post(&forks[second]);
        sem_post(&forks[first]);
    }
    return NULL;
}

int main(void) {
    for (int i = 0; i < N; i++) sem_init(&forks[i], 0, 1);

    pthread_t threads[N];
    for (int i = 0; i < N; i++)
        pthread_create(&threads[i], NULL, philosopher, (void*)(intptr_t)i);
    for (int i = 0; i < N; i++)
        pthread_join(threads[i], NULL);
    for (int i = 0; i < N; i++)
        sem_destroy(&forks[i]);
    return 0;
}
```

---

## 3. Monitors

A **monitor** encapsulates shared data, operations, and synchronization into a single construct. Only one thread can be "inside" the monitor at a time.

```
Monitor = Mutex + Condition Variables + Shared Data

+------------------------------------------+
| Monitor: BoundedBuffer                   |
|                                          |
| Private data:                            |
|   buffer[MAX], count, head, tail         |
|                                          |
| Condition variables:                     |
|   not_full, not_empty                    |
|                                          |
| Public methods (auto-locked):            |
|   put(item):                             |
|     while full: wait(not_full)           |
|     buffer[tail] = item; count++         |
|     signal(not_empty)                    |
|                                          |
|   get():                                 |
|     while empty: wait(not_empty)         |
|     item = buffer[head]; count--         |
|     signal(not_full)                     |
|     return item                          |
+------------------------------------------+
```

### Monitor Semantics: Mesa vs. Hoare

| Aspect | Hoare Monitor | Mesa Monitor |
|--------|-------------|-------------|
| After `signal()` | Signaled thread runs **immediately** | Signaled thread is moved to ready queue |
| Signaler | Blocks until signaled thread releases monitor | Continues running |
| Condition check | `if` is sufficient | Must use `while` loop |
| Used in | Original theory, Java (partially) | PThreads, most modern systems |

**Java synchronized as a monitor:**
```java
class BoundedBuffer {
    private final Object[] buffer;
    private int count, head, tail;

    public synchronized void put(Object item) throws InterruptedException {
        while (count == buffer.length) {
            wait();  // releases monitor lock, waits on condition
        }
        buffer[tail] = item;
        tail = (tail + 1) % buffer.length;
        count++;
        notifyAll();  // wake all waiters
    }

    public synchronized Object get() throws InterruptedException {
        while (count == 0) {
            wait();
        }
        Object item = buffer[head];
        head = (head + 1) % buffer.length;
        count--;
        notifyAll();
        return item;
    }
}
```

---

## 4. Reader-Writer Locks

Allow **multiple simultaneous readers** OR **one exclusive writer**.

```
State machine:
                  +----------------+
                  |  No locks held |
                  +-------+--------+
                  /       |        \
       reader_lock  writer_lock  reader_lock
              /           |           \
  +-----------+   +-------+-------+   +-----------+
  | N readers |   | 1 writer      |   | N readers |
  | (shared)  |   | (exclusive)   |   | (shared)  |
  +-----------+   +---------------+   +-----------+
  Can add more    No other readers    Can add more
  readers         or writers allowed  readers
```

### PThreads Reader-Writer Lock

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;
int shared_data = 0;

void *reader(void *arg) {
    int id = (intptr_t)arg;
    for (int i = 0; i < 5; i++) {
        pthread_rwlock_rdlock(&rwlock);  // Shared lock
        printf("Reader %d: data = %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(100000);
    }
    return NULL;
}

void *writer(void *arg) {
    int id = (intptr_t)arg;
    for (int i = 0; i < 3; i++) {
        pthread_rwlock_wrlock(&rwlock);  // Exclusive lock
        shared_data++;
        printf("Writer %d: data updated to %d\n", id, shared_data);
        pthread_rwlock_unlock(&rwlock);
        usleep(200000);
    }
    return NULL;
}

int main(void) {
    pthread_t readers[5], writers[2];
    for (int i = 0; i < 5; i++)
        pthread_create(&readers[i], NULL, reader, (void*)(intptr_t)i);
    for (int i = 0; i < 2; i++)
        pthread_create(&writers[i], NULL, writer, (void*)(intptr_t)i);

    for (int i = 0; i < 5; i++) pthread_join(readers[i], NULL);
    for (int i = 0; i < 2; i++) pthread_join(writers[i], NULL);

    pthread_rwlock_destroy(&rwlock);
    return 0;
}
```

### Reader vs. Writer Priority

| Policy | Behavior | Risk |
|--------|----------|------|
| Reader-preference | New readers can acquire even if writers waiting | Writer starvation |
| Writer-preference | No new readers if a writer is waiting | Reader starvation |
| Fair (alternating) | FIFO ordering of requests | Lower throughput |

```c
// PThreads: Set writer preference
pthread_rwlockattr_t attr;
pthread_rwlockattr_init(&attr);
pthread_rwlockattr_setkind_np(&attr,
    PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
pthread_rwlock_init(&rwlock, &attr);
```

---

## 5. Barriers

A **barrier** blocks all threads until N threads have arrived. Then all are released simultaneously.

```
Thread 0: work... | barrier_wait() | ... continue
Thread 1: work... | barrier_wait() | ... continue
Thread 2: work... | barrier_wait() | ... continue
Thread 3: work... | barrier_wait() | ... continue
                     ^
                     All 4 must arrive before ANY continues
```

### PThreads Barrier

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 4
pthread_barrier_t barrier;

void *phase_work(void *arg) {
    int id = (intptr_t)arg;

    // Phase 1
    printf("Thread %d: phase 1 work\n", id);
    usleep(100000 * (id + 1));  // Different amounts of work

    printf("Thread %d: waiting at barrier\n", id);
    int ret = pthread_barrier_wait(&barrier);
    // Exactly one thread gets PTHREAD_BARRIER_SERIAL_THREAD
    if (ret == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("Thread %d: I'm the serial thread (do cleanup)\n", id);
    }

    // Phase 2 (all threads proceed together)
    printf("Thread %d: phase 2 work\n", id);

    return NULL;
}

int main(void) {
    pthread_barrier_init(&barrier, NULL, NUM_THREADS);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_create(&threads[i], NULL, phase_work, (void*)(intptr_t)i);
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(&barrier);
    return 0;
}
```

---

## 6. Spinlocks and Atomic Operations

### Hardware Atomic Instructions

| Instruction | Description | x86 Mnemonic |
|------------|-------------|-------------|
| Test-and-Set (TAS) | Atomically set to 1, return old value | `LOCK XCHG` |
| Compare-and-Swap (CAS) | If *addr == expected, set to new | `LOCK CMPXCHG` |
| Fetch-and-Add (FAA) | Atomically add, return old value | `LOCK XADD` |
| Load-Linked/Store-Conditional | LL/SC pair for lock-free algorithms | ARM `LDXR`/`STXR` |

### Spinlock Variants

```c
// Test-and-Set spinlock (simplest, but high bus traffic)
void tas_lock(volatile int *lock) {
    while (__sync_lock_test_and_set(lock, 1))
        ;  // spin
}
void tas_unlock(volatile int *lock) {
    __sync_lock_release(lock);
}

// Test-and-Test-and-Set (TTAS): reduces bus traffic
void ttas_lock(volatile int *lock) {
    while (1) {
        while (*lock)     // spin on cached copy (no bus traffic)
            ;
        if (!__sync_lock_test_and_set(lock, 1))
            return;       // got the lock
        // Else: someone else got it, retry
    }
}

// TTAS with exponential backoff
void ttas_backoff_lock(volatile int *lock) {
    int delay = 1;
    while (1) {
        while (*lock)
            ;
        if (!__sync_lock_test_and_set(lock, 1))
            return;
        for (volatile int i = 0; i < delay; i++)
            ;  // backoff
        delay = delay < 1024 ? delay * 2 : 1024;
    }
}
```

### Ticket Lock (Fair Spinlock)

```c
#include <stdatomic.h>

typedef struct {
    atomic_int next_ticket;
    atomic_int now_serving;
} ticket_lock_t;

void ticket_lock(ticket_lock_t *lock) {
    int my_ticket = atomic_fetch_add(&lock->next_ticket, 1);
    while (atomic_load(&lock->now_serving) != my_ticket)
        ;  // spin until my turn (FIFO ordering)
}

void ticket_unlock(ticket_lock_t *lock) {
    atomic_fetch_add(&lock->now_serving, 1);
}
```

---

## 7. Read-Copy-Update (RCU)

RCU is a Linux kernel synchronization mechanism optimized for **read-mostly** data structures (routing tables, module lists, etc.).

```
RCU Principle:
  Readers: NO locking at all. Just read.
  Writers:
    1. COPY the data structure
    2. MODIFY the copy
    3. PUBLISH the new pointer (atomic swap)
    4. WAIT for all existing readers to finish (grace period)
    5. FREE the old data

Timeline:
  [------- old data valid -------]
                    [-- new data valid --]
  Reader 1: [read old]
  Reader 2:      [read old]
  Writer:   [copy][modify][publish]
  Reader 3:                        [read new]
                         [grace period]
                                   [free old]
```

```c
// Kernel RCU API (simplified)
// Reader side:
rcu_read_lock();
struct foo *p = rcu_dereference(global_ptr);  // Read-side: no lock!
// Use p...
rcu_read_unlock();

// Writer side:
struct foo *new_data = kmalloc(sizeof(*new_data), GFP_KERNEL);
*new_data = *old_data;        // Copy
new_data->field = new_value;  // Modify
rcu_assign_pointer(global_ptr, new_data);  // Publish
synchronize_rcu();            // Wait for all readers of old data
kfree(old_data);              // Free old
```

**When to use RCU vs. RW locks:**
| Factor | RCU | RW Lock |
|--------|-----|---------|
| Read overhead | Zero (no atomic ops) | Atomic increment/decrement |
| Write overhead | Copy + grace period | Exclusive lock acquisition |
| Best for | 99%+ reads | ~80-90% reads |
| Complexity | High | Low |
| Availability | Linux kernel, userspace-rcu library | PThreads, everywhere |

---

## 8. Lock-Free and Wait-Free Data Structures

### Lock-Free Stack (Treiber Stack)

```c
#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct node {
    int data;
    struct node *next;
} node_t;

typedef struct {
    _Atomic(node_t*) top;
} lf_stack_t;

void lf_push(lf_stack_t *stack, int value) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->data = value;

    node_t *old_top;
    do {
        old_top = atomic_load(&stack->top);
        new_node->next = old_top;
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &old_top, new_node));
    // CAS: if top == old_top, set top = new_node
    // Retry if another thread changed top between load and CAS
}

int lf_pop(lf_stack_t *stack, int *value) {
    node_t *old_top;
    do {
        old_top = atomic_load(&stack->top);
        if (!old_top) return -1;  // Empty
    } while (!atomic_compare_exchange_weak(&stack->top,
                                           &old_top, old_top->next));
    *value = old_top->data;
    free(old_top);  // Caution: ABA problem in real implementations
    return 0;
}
```

**ABA Problem:** Between a thread's load and CAS, another thread pops A, pushes B, pushes A back. The CAS succeeds but the data structure is corrupted. Solutions: hazard pointers, epoch-based reclamation, double-width CAS with counter.

---

## 9. Deadlock Prevention and Detection

### Four Necessary Conditions (Coffman)

All four must hold simultaneously for deadlock:

| Condition | Meaning | Prevention Strategy |
|-----------|---------|-------------------|
| **Mutual exclusion** | Resource can't be shared | Use sharable resources (read locks) |
| **Hold and wait** | Hold one, request another | Request all at once |
| **No preemption** | Can't force release | Allow preemption (rollback) |
| **Circular wait** | Cycle in resource graph | Lock ordering |

### Lock Ordering

```c
// Prevent deadlock: always acquire locks in a consistent global order
// Rule: always lock the mutex with the lower address first

void safe_transfer(account_t *a, account_t *b, int amount) {
    account_t *first  = a < b ? a : b;
    account_t *second = a < b ? b : a;

    pthread_mutex_lock(&first->lock);
    pthread_mutex_lock(&second->lock);

    a->balance -= amount;
    b->balance += amount;

    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}
```

### Deadlock Detection: Resource Allocation Graph

```
Process P1 holds R1, requests R2
Process P2 holds R2, requests R1

    P1 --requests--> R2 --held-by--> P2
    ^                                |
    |                                |
    +--held-by-- R1 <--requests------+

Cycle detected -> DEADLOCK
```

```bash
# Linux: detect potential deadlocks with lockdep (kernel)
# Enable CONFIG_PROVE_LOCKING in kernel config
# Lockdep detects lock ordering violations at runtime

# Userspace: ThreadSanitizer
gcc -fsanitize=thread -g -pthread -o my_prog my_prog.c
./my_prog
# WARNING: ThreadSanitizer: lock-order-inversion (potential deadlock)
```

---

## 10. Windows Synchronization Primitives

| Primitive | API | Notes |
|-----------|-----|-------|
| Critical Section | `InitializeCriticalSection` | In-process mutex, fastest |
| Mutex | `CreateMutex` | Cross-process, named |
| Semaphore | `CreateSemaphore` | Cross-process, named |
| Event | `CreateEvent` | Manual/auto reset, cross-process |
| SRWLOCK | `InitializeSRWLock` | Slim reader/writer lock |
| Condition Variable | `InitializeConditionVariable` | Works with CS or SRWLOCK |
| Interlocked functions | `InterlockedIncrement` | Atomic operations |

### Windows Event Objects

```c
#include <windows.h>
#include <stdio.h>

HANDLE hEvent;

DWORD WINAPI waiter(LPVOID arg) {
    printf("Waiter: waiting for event...\n");
    WaitForSingleObject(hEvent, INFINITE);
    printf("Waiter: event signaled!\n");
    return 0;
}

int main(void) {
    // Manual reset: stays signaled until ResetEvent()
    // Auto reset: resets after one waiter is released
    hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  // Auto-reset, initially unsignaled

    HANDLE hThread = CreateThread(NULL, 0, waiter, NULL, 0, NULL);

    Sleep(2000);
    printf("Main: signaling event\n");
    SetEvent(hEvent);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hEvent);
    return 0;
}
```

---

## 11. Quizzes and Exercises

> **Quiz: Semaphore vs. Mutex**
>
> *A mutex is initialized and locked by Thread A. Thread B calls unlock. What happens?*
>
> **Answer:** **Undefined behavior** - mutexes have ownership semantics; only the locking thread should unlock. With `PTHREAD_MUTEX_ERRORCHECK`, it returns `EPERM`. A semaphore does not have this restriction - any thread can call `sem_post()`.

> **Quiz: RCU**
>
> *Why is RCU faster than rwlocks for readers?*
>
> **Answer:** RCU readers perform **zero atomic operations** - no cache line bouncing. rwlock readers must atomically increment a shared counter (`rdlock`) and decrement it (`unlock`), causing cache coherence traffic between cores.

---

## 12. Key Takeaways

1. **Semaphores** generalize mutexes with counting; use for resource pools and signaling between threads.
2. **Monitors** (mutex + condvar + data) are the standard high-level synchronization pattern; Java's `synchronized` is a monitor.
3. **Reader-writer locks** allow concurrent reads but exclusive writes; set writer-preference to avoid writer starvation.
4. **Barriers** synchronize threads at a common point; essential for iterative parallel algorithms.
5. **RCU** provides zero-overhead reads by deferring cleanup; used extensively in the Linux kernel for read-mostly data.
6. **Lock-free** structures use CAS loops instead of locks; harder to implement correctly (ABA problem) but provide progress guarantees.
7. **Deadlock prevention** via lock ordering is the most practical strategy; lockdep and ThreadSanitizer catch violations at runtime.

---

## 13. Seqlock: High-Performance Read-Write Without Read Blocking

Seqlock is ideal for infrequently-written, frequently-read data (e.g., `jiffies`, `timespec`). Readers never block writers.

```c
/* Seqlock implementation */
#include <stdatomic.h>

typedef struct {
    atomic_uint seq;   /* Sequence number: odd = write in progress */
    int         data;  /* Protected data */
} seqlock_t;

/* Writer: increment sequence before/after write */
void seqlock_write(seqlock_t *sl, int new_val) {
    unsigned s = atomic_load(&sl->seq);
    atomic_store(&sl->seq, s + 1);   /* Odd: write in progress */
    atomic_thread_fence(memory_order_release);

    sl->data = new_val;              /* Write protected data */

    atomic_thread_fence(memory_order_release);
    atomic_store(&sl->seq, s + 2);   /* Even: write complete */
}

/* Reader: retry if sequence changed during read */
int seqlock_read(seqlock_t *sl) {
    unsigned start, end;
    int val;
    do {
        /* Spin until sequence is even (no write in progress) */
        do {
            start = atomic_load(&sl->seq);
        } while (start & 1);  /* Odd = write in progress */

        atomic_thread_fence(memory_order_acquire);
        val = sl->data;       /* Read data */
        atomic_thread_fence(memory_order_acquire);

        end = atomic_load(&sl->seq);
        /* Retry if sequence changed (writer wrote during our read) */
    } while (start != end);

    return val;
}
```

```bash
# Seqlock usage in the Linux kernel:
# kernel/time/timekeeping.c - reading jiffies
# arch/x86/include/asm/vsyscall.h - gettimeofday fast path

# Observe seqlock retries (BPF)
sudo bpftrace -e '
    uprobe:/lib/libc.so.6:clock_gettime { @calls = count(); }
    interval:s:5 {
        printf("clock_gettime calls/s: %d\n", @calls/5);
        clear(@calls);
    }'
```

---

## 14. Hazard Pointers: Safe Memory Reclamation in Lock-Free Code

Lock-free data structures need safe memory reclamation - you can't `free()` a node while another thread holds a pointer to it.

```c
/*
 * Hazard Pointer scheme:
 * Each thread announces the pointer it's reading ("hazard pointer")
 * Freeing threads check all hazard pointers before actually freeing
 */

#define MAX_THREADS 64
#define MAX_HAZARDS 2   /* Max pointers per thread */

/* Global hazard pointer table */
atomic_uintptr_t hp[MAX_THREADS][MAX_HAZARDS];

/* Retired nodes pending free */
typedef struct retired_node {
    void *ptr;
    void (*deleter)(void *);
    struct retired_node *next;
} RetiredNode;

__thread RetiredNode *retired_list = NULL;
__thread int retired_count = 0;
__thread int thread_id;  /* Assigned at thread start */

/* Protect a pointer: announce "I am reading this" */
void *hazard_acquire(int idx, atomic_uintptr_t *src) {
    void *p;
    do {
        p = (void *)atomic_load(src);
        atomic_store(&hp[thread_id][idx], (uintptr_t)p);
        /* Verify src still points to p (no race with deletion) */
    } while ((void *)atomic_load(src) != p);
    return p;
}

/* Release hazard pointer: "I'm done reading this" */
void hazard_release(int idx) {
    atomic_store(&hp[thread_id][idx], 0);
}

/* Retire a pointer for deferred deletion */
void hazard_retire(void *ptr, void (*deleter)(void *)) {
    RetiredNode *node = malloc(sizeof(*node));
    node->ptr = ptr;
    node->deleter = deleter;
    node->next = retired_list;
    retired_list = node;
    retired_count++;

    /* Scan and reclaim when we have enough retired nodes */
    if (retired_count >= MAX_THREADS * MAX_HAZARDS) {
        /* Collect all active hazard pointers */
        uintptr_t active[MAX_THREADS * MAX_HAZARDS];
        int nactive = 0;
        for (int t = 0; t < MAX_THREADS; t++)
            for (int i = 0; i < MAX_HAZARDS; i++) {
                uintptr_t p = atomic_load(&hp[t][i]);
                if (p) active[nactive++] = p;
            }

        /* Free retired nodes not in any hazard pointer */
        RetiredNode **prev = &retired_list;
        RetiredNode *cur = retired_list;
        while (cur) {
            int in_use = 0;
            for (int i = 0; i < nactive; i++)
                if (active[i] == (uintptr_t)cur->ptr) { in_use = 1; break; }

            if (!in_use) {
                cur->deleter(cur->ptr);  /* Safe to free */
                *prev = cur->next;
                free(cur);
                retired_count--;
                cur = *prev;
            } else {
                prev = &cur->next;
                cur = cur->next;
            }
        }
    }
}
```

---

## 15. Lock-Free Queue (Michael & Scott, 1996)

The canonical lock-free MPMC (multi-producer, multi-consumer) queue.

```c
#include <stdatomic.h>
#include <stdlib.h>

typedef struct Node {
    int value;
    _Atomic(struct Node *) next;
} Node;

typedef struct {
    _Atomic(Node *) head;  /* Dequeue from head */
    _Atomic(Node *) tail;  /* Enqueue to tail */
} LFQueue;

void lfq_init(LFQueue *q) {
    Node *dummy = calloc(1, sizeof(Node));  /* Sentinel node */
    atomic_store(&q->head, dummy);
    atomic_store(&q->tail, dummy);
}

void lfq_enqueue(LFQueue *q, int val) {
    Node *node = malloc(sizeof(Node));
    node->value = val;
    atomic_store(&node->next, NULL);

    Node *tail, *next;
    while (1) {
        tail = atomic_load(&q->tail);
        next = atomic_load(&tail->next);

        if (tail == atomic_load(&q->tail)) {  /* Tail still valid? */
            if (next == NULL) {
                /* Tail is pointing to last node: try to link new node */
                Node *expected = NULL;
                if (atomic_compare_exchange_weak(&tail->next, &expected, node)) {
                    break;  /* Enqueue succeeded */
                }
            } else {
                /* Tail not pointing to last node: advance tail */
                atomic_compare_exchange_weak(&q->tail, &tail, next);
            }
        }
    }
    /* Advance tail to new node (best-effort) */
    atomic_compare_exchange_weak(&q->tail, &tail, node);
}

int lfq_dequeue(LFQueue *q, int *out_val) {
    Node *head, *tail, *next;
    while (1) {
        head = atomic_load(&q->head);
        tail = atomic_load(&q->tail);
        next = atomic_load(&head->next);

        if (head == atomic_load(&q->head)) {
            if (head == tail) {
                if (next == NULL) return 0;  /* Empty queue */
                atomic_compare_exchange_weak(&q->tail, &tail, next);  /* Advance tail */
            } else {
                *out_val = next->value;
                if (atomic_compare_exchange_weak(&q->head, &head, next)) {
                    /* Retire 'head' - use hazard pointers or epoch-based reclamation */
                    free(head);  /* Simplified: unsafe with concurrent dequeue! */
                    return 1;
                }
            }
        }
    }
}
```

```bash
# Test lock-free queue with TSan
gcc -fsanitize=thread -g -O1 lockfree_queue.c -o lfq_test
./lfq_test

# Benchmark lock-free vs mutex queue
# Expected: lock-free 2-5x faster under high contention, similar uncontended
perf stat ./lfq_bench --threads=16 --iters=1000000
perf stat ./mutex_bench --threads=16 --iters=1000000
```

---

## 16. Windows Synchronization Objects

Windows provides a rich set of kernel synchronization objects distinct from POSIX.

```c
#include <windows.h>

/* ── CRITICAL SECTION (userspace mutex with kernel fallback) ── */
CRITICAL_SECTION cs;
InitializeCriticalSectionAndSpinCount(&cs, 4000);  /* Spin 4000 times before blocking */
EnterCriticalSection(&cs);
/* ... critical section ... */
LeaveCriticalSection(&cs);
DeleteCriticalSection(&cs);

/* ── SLIM READER/WRITER LOCK (Vista+, like pthread_rwlock) ── */
SRWLOCK srw = SRWLOCK_INIT;
AcquireSRWLockShared(&srw);     /* Read lock */
ReleaseSRWLockShared(&srw);
AcquireSRWLockExclusive(&srw);  /* Write lock */
ReleaseSRWLockExclusive(&srw);

/* ── CONDITION VARIABLE (Vista+) ── */
CONDITION_VARIABLE cv = CONDITION_VARIABLE_INIT;
SleepConditionVariableCS(&cv, &cs, INFINITE);  /* Wait */
WakeConditionVariable(&cv);                     /* Signal one */
WakeAllConditionVariable(&cv);                  /* Broadcast */

/* ── MUTEX (cross-process, named) ── */
HANDLE mutex = CreateMutex(NULL, FALSE, L"Global\\MyMutex");
WaitForSingleObject(mutex, INFINITE);  /* Lock */
ReleaseMutex(mutex);                   /* Unlock */
CloseHandle(mutex);

/* ── SEMAPHORE ── */
HANDLE sem = CreateSemaphore(NULL,
    0,     /* Initial count */
    10,    /* Maximum count */
    NULL); /* Unnamed */
WaitForSingleObject(sem, 1000);  /* Decrement (timeout 1s) */
ReleaseSemaphore(sem, 1, NULL);  /* Increment by 1 */

/* ── EVENT (binary signal) ── */
HANDLE event = CreateEvent(NULL, FALSE, FALSE, NULL);  /* Auto-reset */
WaitForSingleObject(event, INFINITE);  /* Wait for signal */
SetEvent(event);                        /* Signal */
ResetEvent(event);                      /* Manual reset */

/* ── WAIT FOR MULTIPLE ── */
HANDLE handles[2] = { mutex, event };
DWORD result = WaitForMultipleObjects(2, handles, FALSE /* any */, INFINITE);
if (result == WAIT_OBJECT_0) { /* mutex signaled */ }
if (result == WAIT_OBJECT_0 + 1) { /* event signaled */ }
```

```powershell
# Windows: inspect synchronization objects
# Use SysInternals Handle.exe or WinObj
handle.exe -p <pid>              # List all handles (mutexes, events, semaphores)
# Or WinObj.exe for global namespace

# Deadlock detection: WinDbg
# !locks           - Show all CRITICAL_SECTIONs (locked/unlocked)
# !handle 0 f      - Dump all handles
# !analyze -hang   - Auto-detect hang/deadlock

# ProcMon / Process Monitor for synchronization events
# Filter: "Thread" event category, "Lock Acquired"/"Lock Released"

# ETW: Low-overhead lock contention tracing
logman start mysession -p "Microsoft-Windows-Kernel-Thread" -o mytrace.etl -ets
# ... run your program ...
logman stop mysession -ets
tracerpt mytrace.etl -o myreport.xml
```

---

**Previous:** [P3L3: Inter-Process Communication](P3L3-Inter-Process-Communication.md)
**Next:** [P3L5: I/O Management](P3L5-IO-Management.md)

