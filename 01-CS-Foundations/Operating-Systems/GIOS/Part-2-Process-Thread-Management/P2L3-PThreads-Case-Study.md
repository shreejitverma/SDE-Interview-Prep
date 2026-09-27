---
type: concept
track: [sde]
level: advanced
status: complete
last_reviewed:
sources:
  - "Georgia Tech CS 6200 P2L3"
  - "Programming with POSIX Threads, David Butenhof"
  - "The Open Group POSIX.1-2017 specification"
---

# P2L3: Threads Case Study - PThreads

> **Module goal:** Master the POSIX Threads (PThreads) API - thread creation, joining, detaching, mutexes (init/lock/trylock/unlock/destroy), mutex attributes, condition variables (wait/signal/broadcast), spurious wakeups, and build a complete producer-consumer implementation.

## Table of Contents

- [1. The POSIX Threads Standard](#1-the-posix-threads-standard)
- [2. PThreads API Overview](#2-pthreads-api-overview)
- [3. Thread Creation: pthread_create](#3-thread-creation-pthread_create)
- [4. Thread Completion: pthread_join and pthread_detach](#4-thread-completion-pthread_join-and-pthread_detach)
- [5. Compiling PThreads Code](#5-compiling-pthreads-code)
- [6. Mutexes in PThreads](#6-mutexes-in-pthreads)
- [7. Mutex Operations: init, lock, trylock, unlock, destroy](#7-mutex-operations-init-lock-trylock-unlock-destroy)
- [8. Mutex Attributes and Safety](#8-mutex-attributes-and-safety)
- [9. Condition Variables in PThreads](#9-condition-variables-in-pthreads)
- [10. Condition Variable Operations: wait, signal, broadcast](#10-condition-variable-operations-wait-signal-broadcast)
- [11. Spurious Wakeups and the While Loop Pattern](#11-spurious-wakeups-and-the-while-loop-pattern)
- [12. Producer-Consumer Implementation](#12-producer-consumer-implementation)
- [13. PThreads Pitfalls](#13-pthreads-pitfalls)
- [14. Windows Threading API Comparison](#14-windows-threading-api-comparison)
- [15. Quizzes and Exercises](#15-quizzes-and-exercises)
- [16. Key Takeaways](#16-key-takeaways)

---

## 1. The POSIX Threads Standard

**POSIX** (Portable Operating System Interface) defines a standard API for thread management, defined in `<pthread.h>`.

Key properties:
- Standardized by IEEE (POSIX.1c, IEEE 1003.1c-1995)
- Available on all Unix-like systems (Linux, macOS, FreeBSD, Solaris)
- Not natively available on Windows (but Pthreads-win32 exists)
- Specifies thread creation, synchronization, scheduling, and cancellation
- The C11 standard added `<threads.h>` as an alternative, but PThreads remains the de facto standard

```bash
# Check PThreads availability
man pthreads    # or man 7 pthreads

# List available PThread functions
man -k pthread | head -30

# Check which PThread implementation your system uses
getconf GNU_LIBPTHREAD_VERSION
# Example output: NPTL 2.35 (Native POSIX Thread Library)
```

---

## 2. PThreads API Overview

```
PThreads API Categories:
+----------------------------------------------------------+
| Thread Management                                         |
|   pthread_create, pthread_join, pthread_detach,           |
|   pthread_exit, pthread_self, pthread_equal                |
+----------------------------------------------------------+
| Mutexes                                                   |
|   pthread_mutex_init, pthread_mutex_destroy,              |
|   pthread_mutex_lock, pthread_mutex_trylock,              |
|   pthread_mutex_unlock, pthread_mutex_timedlock            |
+----------------------------------------------------------+
| Condition Variables                                       |
|   pthread_cond_init, pthread_cond_destroy,                |
|   pthread_cond_wait, pthread_cond_timedwait,              |
|   pthread_cond_signal, pthread_cond_broadcast              |
+----------------------------------------------------------+
| Attributes                                                |
|   pthread_attr_init, pthread_attr_destroy,                |
|   pthread_attr_setdetachstate, pthread_attr_setstacksize, |
|   pthread_mutexattr_init, pthread_mutexattr_settype       |
+----------------------------------------------------------+
| Thread-Specific Data (TLS)                                |
|   pthread_key_create, pthread_key_delete,                 |
|   pthread_setspecific, pthread_getspecific                  |
+----------------------------------------------------------+
| Barriers, Spinlocks, Read-Write Locks                     |
|   pthread_barrier_*, pthread_spin_*, pthread_rwlock_*      |
+----------------------------------------------------------+
```

### Data Types

| Type | Purpose |
|------|---------|
| `pthread_t` | Thread identifier |
| `pthread_mutex_t` | Mutex |
| `pthread_cond_t` | Condition variable |
| `pthread_attr_t` | Thread attributes |
| `pthread_mutexattr_t` | Mutex attributes |
| `pthread_condattr_t` | Condition variable attributes |
| `pthread_key_t` | Thread-local storage key |
| `pthread_rwlock_t` | Read-write lock |
| `pthread_barrier_t` | Barrier |
| `pthread_spinlock_t` | Spinlock |

---

## 3. Thread Creation: pthread_create

```c
int pthread_create(
    pthread_t *thread,            // [out] thread ID
    const pthread_attr_t *attr,   // [in]  thread attributes (NULL for defaults)
    void *(*start_routine)(void*),// [in]  function pointer
    void *arg                     // [in]  argument to start_routine
);
// Returns: 0 on success, error number on failure (NOT -1)
```

### Basic Example

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>

typedef struct {
    int id;
    const char *name;
    int iterations;
} thread_args_t;

void *worker(void *arg) {
    thread_args_t *args = (thread_args_t*)arg;
    printf("Thread %d (%s): starting %d iterations\n",
           args->id, args->name, args->iterations);

    int sum = 0;
    for (int i = 0; i < args->iterations; i++) {
        sum += i;
    }

    printf("Thread %d (%s): sum = %d\n", args->id, args->name, sum);

    // Return result via heap allocation
    int *result = malloc(sizeof(int));
    *result = sum;
    return result;
}

int main(void) {
    pthread_t threads[3];
    thread_args_t args[3] = {
        {0, "alpha",   100},
        {1, "beta",    200},
        {2, "gamma",   300}
    };

    for (int i = 0; i < 3; i++) {
        int ret = pthread_create(&threads[i], NULL, worker, &args[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
            return 1;
        }
    }

    for (int i = 0; i < 3; i++) {
        void *retval;
        pthread_join(threads[i], &retval);
        printf("Main: thread %d returned %d\n", i, *(int*)retval);
        free(retval);
    }

    return 0;
}
```

### Common Mistake: Passing Loop Variable by Reference

```c
// BUG: All threads may see the same value of i
for (int i = 0; i < N; i++) {
    pthread_create(&threads[i], NULL, worker, &i);  // WRONG!
    // By the time the thread reads *arg, i may have changed
}

// FIX 1: Use an array of arguments
int ids[N];
for (int i = 0; i < N; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, worker, &ids[i]);  // OK
}

// FIX 2: Cast to intptr_t (if arg is just an integer)
for (int i = 0; i < N; i++) {
    pthread_create(&threads[i], NULL, worker, (void*)(intptr_t)i);
}
// In worker:
void *worker(void *arg) {
    int id = (intptr_t)arg;
    // ...
}
```

---

## 4. Thread Completion: pthread_join and pthread_detach

### pthread_join

```c
int pthread_join(
    pthread_t thread,   // thread to wait for
    void **retval       // [out] thread's return value (NULL if not needed)
);
// Blocks calling thread until target thread terminates.
// Returns: 0 on success, error number on failure.
```

**Semantics:**
- Blocks the caller until `thread` terminates
- Retrieves the thread's return value (or the argument to `pthread_exit()`)
- Releases the thread's resources (stack, TCB)
- A thread can only be joined once
- A non-detached, non-joined thread is a "zombie thread" (leaks resources)

### pthread_detach

```c
int pthread_detach(pthread_t thread);
// Marks the thread so its resources are automatically freed on exit.
// Cannot join a detached thread.
```

```c
// Pattern: Fire-and-forget thread
pthread_t tid;
pthread_create(&tid, NULL, background_work, NULL);
pthread_detach(tid);
// No need to join. Resources freed automatically when thread exits.

// Alternative: Set detach state in attributes
pthread_attr_t attr;
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
pthread_create(&tid, &attr, background_work, NULL);
pthread_attr_destroy(&attr);
```

### pthread_exit

```c
void pthread_exit(void *retval);
// Terminates the calling thread.
// retval is available to pthread_join().
// If main() calls pthread_exit(), other threads continue running.
```

```c
void *worker(void *arg) {
    // ... do work ...
    int *result = malloc(sizeof(int));
    *result = 42;
    pthread_exit(result);  // Equivalent to return result;
}
```

> **Quiz: PThreads Thread Lifecycle**
>
> *What happens if `main()` returns before all threads finish?*
>
> **Answer:** The entire process terminates (all threads are killed). To avoid this, either `join()` all threads or call `pthread_exit(NULL)` from `main()` (which terminates only the main thread, letting others continue).

---

## 5. Compiling PThreads Code

```bash
# Linux: Link with -pthread (preferred) or -lpthread
gcc -pthread -o my_program my_program.c
gcc -lpthread -o my_program my_program.c    # older style

# The -pthread flag:
# 1. Defines _REENTRANT macro
# 2. Links libpthread
# 3. May enable thread-safe versions of libc functions

# With optimizations and warnings:
gcc -Wall -Wextra -O2 -pthread -o my_program my_program.c

# With ThreadSanitizer (detects races):
gcc -fsanitize=thread -g -pthread -o my_program my_program.c

# macOS: PThreads is part of the system library
clang -pthread -o my_program my_program.c
```

---

## 6. Mutexes in PThreads

### Static Initialization

```c
// For statically allocated mutexes (file-scope or static local)
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// This is equivalent to:
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);  // but PTHREAD_MUTEX_INITIALIZER is simpler
```

### Dynamic Initialization

```c
// For dynamically allocated or attribute-customized mutexes
pthread_mutex_t *lock = malloc(sizeof(pthread_mutex_t));

pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);

pthread_mutex_init(lock, &attr);
pthread_mutexattr_destroy(&attr);

// ... use lock ...

pthread_mutex_destroy(lock);
free(lock);
```

---

## 7. Mutex Operations: init, lock, trylock, unlock, destroy

### Complete API Reference

```c
// Initialize
int pthread_mutex_init(pthread_mutex_t *mutex,
                       const pthread_mutexattr_t *attr);

// Lock (blocking)
int pthread_mutex_lock(pthread_mutex_t *mutex);
// If mutex is free: acquires it, returns 0
// If mutex is held: blocks until it's free

// Try lock (non-blocking)
int pthread_mutex_trylock(pthread_mutex_t *mutex);
// If mutex is free: acquires it, returns 0
// If mutex is held: returns EBUSY immediately (does NOT block)

// Timed lock (blocks with timeout)
int pthread_mutex_timedlock(pthread_mutex_t *mutex,
                            const struct timespec *abs_timeout);
// If mutex is free: acquires it, returns 0
// If mutex is held: blocks until free OR timeout expires (returns ETIMEDOUT)

// Unlock
int pthread_mutex_unlock(pthread_mutex_t *mutex);
// Releases the mutex. If threads are waiting, one is woken.

// Destroy
int pthread_mutex_destroy(pthread_mutex_t *mutex);
// Frees resources. Mutex must be unlocked. Behavior undefined if locked.
```

### Trylock Pattern for Deadlock Avoidance

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t lock_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_b = PTHREAD_MUTEX_INITIALIZER;

void *thread_func(void *arg) {
    int id = (intptr_t)arg;
    int acquired = 0;

    while (!acquired) {
        pthread_mutex_lock(&lock_a);
        if (pthread_mutex_trylock(&lock_b) == 0) {
            // Got both locks
            printf("Thread %d: got both locks\n", id);
            // ... critical section ...
            pthread_mutex_unlock(&lock_b);
            pthread_mutex_unlock(&lock_a);
            acquired = 1;
        } else {
            // Couldn't get lock_b; release lock_a to avoid deadlock
            pthread_mutex_unlock(&lock_a);
            printf("Thread %d: backoff, retrying\n", id);
            usleep(1000 * (id + 1));  // Backoff
        }
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, (void*)0);
    pthread_create(&t2, NULL, thread_func, (void*)1);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
```

---

## 8. Mutex Attributes and Safety

### Mutex Types

```c
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, type);
```

| Type | Constant | Behavior on Re-lock by Owner |
|------|----------|---------------------------|
| **Normal** | `PTHREAD_MUTEX_NORMAL` | Deadlock (undefined behavior) |
| **Error-check** | `PTHREAD_MUTEX_ERRORCHECK` | Returns `EDEADLK` (detectable) |
| **Recursive** | `PTHREAD_MUTEX_RECURSIVE` | Succeeds; must unlock same number of times |
| **Default** | `PTHREAD_MUTEX_DEFAULT` | Implementation-defined (usually same as NORMAL) |

### Recursive Mutex Example

```c
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t recursive_lock;

void recursive_function(int depth) {
    pthread_mutex_lock(&recursive_lock);
    printf("Depth %d: locked\n", depth);
    if (depth > 0) {
        recursive_function(depth - 1);  // Re-locks the same mutex
    }
    printf("Depth %d: unlocking\n", depth);
    pthread_mutex_unlock(&recursive_lock);
}

int main(void) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&recursive_lock, &attr);
    pthread_mutexattr_destroy(&attr);

    recursive_function(3);

    pthread_mutex_destroy(&recursive_lock);
    return 0;
}
```

### Process-Shared Mutexes

By default, PThreads mutexes are process-private. For use in shared memory between processes:

```c
pthread_mutexattr_t attr;
pthread_mutexattr_init(&attr);
pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
// This mutex can now be placed in shared memory (mmap/shmem)
// and used by multiple processes
```

---

## 9. Condition Variables in PThreads

### Static Initialization

```c
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```

### Dynamic Initialization

```c
pthread_cond_t cond;
pthread_condattr_t cattr;
pthread_condattr_init(&cattr);
// Optionally set attributes (e.g., process-shared, clock)
pthread_cond_init(&cond, &cattr);
pthread_condattr_destroy(&cattr);
```

---

## 10. Condition Variable Operations: wait, signal, broadcast

### Complete API

```c
// Wait: atomically unlock mutex and block on condition
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
// Precondition: mutex MUST be locked by calling thread
// 1. Atomically: releases mutex + adds thread to cond's wait queue
// 2. Blocks until signaled
// 3. On wakeup: re-acquires mutex, then returns

// Timed wait: same as wait but with timeout
int pthread_cond_timedwait(pthread_cond_t *cond,
                           pthread_mutex_t *mutex,
                           const struct timespec *abstime);
// Returns ETIMEDOUT if timeout expires

// Signal: wake ONE waiting thread
int pthread_cond_signal(pthread_cond_t *cond);
// If no threads are waiting, the signal is LOST (not queued)

// Broadcast: wake ALL waiting threads
int pthread_cond_broadcast(pthread_cond_t *cond);
// All waiters wake up; they compete for the mutex
```

### Timed Wait Example

```c
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int data_ready = 0;

void *waiter(void *arg) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;  // 3-second timeout

    pthread_mutex_lock(&mtx);
    while (!data_ready) {
        int ret = pthread_cond_timedwait(&cv, &mtx, &ts);
        if (ret == ETIMEDOUT) {
            printf("Waiter: timed out after 3 seconds\n");
            pthread_mutex_unlock(&mtx);
            return NULL;
        }
    }
    printf("Waiter: data is ready!\n");
    pthread_mutex_unlock(&mtx);
    return NULL;
}

void *signaler(void *arg) {
    sleep(1);  // Change to 5 to see timeout
    pthread_mutex_lock(&mtx);
    data_ready = 1;
    pthread_cond_signal(&cv);
    pthread_mutex_unlock(&mtx);
    printf("Signaler: signaled\n");
    return NULL;
}

int main(void) {
    pthread_t tw, ts;
    pthread_create(&tw, NULL, waiter, NULL);
    pthread_create(&ts, NULL, signaler, NULL);
    pthread_join(tw, NULL);
    pthread_join(ts, NULL);
    return 0;
}
```

---

## 11. Spurious Wakeups and the While Loop Pattern

**POSIX explicitly states** that `pthread_cond_wait` may return even when no thread has called `signal` or `broadcast`. These are **spurious wakeups**.

Causes:
- Kernel scheduling artifacts
- OS/library implementation details
- Multi-processor race conditions in the futex implementation

**The ONLY correct pattern:**

```c
pthread_mutex_lock(&mutex);
while (!condition_is_true) {           // MUST be while, not if
    pthread_cond_wait(&cond, &mutex);  // May return spuriously
}
// condition_is_true is guaranteed here
// ... proceed safely ...
pthread_mutex_unlock(&mutex);
```

**Why `if` is wrong:**

```c
// WRONG:
pthread_mutex_lock(&mutex);
if (!condition_is_true) {              // BUG: only checks once
    pthread_cond_wait(&cond, &mutex);
}
// Here, condition_is_true might STILL be false (spurious wakeup)
// or another thread might have consumed the item between signal and re-acquire
```

---

## 12. Producer-Consumer Implementation

Complete, production-quality producer-consumer with PThreads:

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>

// ----- Bounded Buffer (Thread-Safe Queue) -----

typedef struct {
    int *data;
    int capacity;
    int count;
    int head;       // read position
    int tail;       // write position
    bool shutdown;

    pthread_mutex_t mutex;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} bounded_buffer_t;

int bb_init(bounded_buffer_t *bb, int capacity) {
    bb->data = malloc(capacity * sizeof(int));
    if (!bb->data) return -1;
    bb->capacity = capacity;
    bb->count = 0;
    bb->head = 0;
    bb->tail = 0;
    bb->shutdown = false;
    pthread_mutex_init(&bb->mutex, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    return 0;
}

void bb_destroy(bounded_buffer_t *bb) {
    free(bb->data);
    pthread_mutex_destroy(&bb->mutex);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
}

// Returns 0 on success, -1 on shutdown
int bb_put(bounded_buffer_t *bb, int item) {
    pthread_mutex_lock(&bb->mutex);
    while (bb->count == bb->capacity && !bb->shutdown) {
        pthread_cond_wait(&bb->not_full, &bb->mutex);
    }
    if (bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return -1;
    }
    bb->data[bb->tail] = item;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
    return 0;
}

// Returns 0 on success, -1 on shutdown (and buffer empty)
int bb_get(bounded_buffer_t *bb, int *item) {
    pthread_mutex_lock(&bb->mutex);
    while (bb->count == 0 && !bb->shutdown) {
        pthread_cond_wait(&bb->not_empty, &bb->mutex);
    }
    if (bb->count == 0 && bb->shutdown) {
        pthread_mutex_unlock(&bb->mutex);
        return -1;
    }
    *item = bb->data[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->mutex);
    return 0;
}

void bb_shutdown(bounded_buffer_t *bb) {
    pthread_mutex_lock(&bb->mutex);
    bb->shutdown = true;
    pthread_cond_broadcast(&bb->not_full);
    pthread_cond_broadcast(&bb->not_empty);
    pthread_mutex_unlock(&bb->mutex);
}

// ----- Producer and Consumer Threads -----

typedef struct {
    int id;
    bounded_buffer_t *bb;
    int num_items;
} worker_args_t;

void *producer(void *arg) {
    worker_args_t *wa = (worker_args_t*)arg;
    for (int i = 0; i < wa->num_items; i++) {
        int item = wa->id * 1000 + i;
        if (bb_put(wa->bb, item) < 0) break;
        printf("[P%d] produced %d\n", wa->id, item);
    }
    printf("[P%d] done\n", wa->id);
    return NULL;
}

void *consumer(void *arg) {
    worker_args_t *wa = (worker_args_t*)arg;
    int item;
    int consumed = 0;
    while (bb_get(wa->bb, &item) == 0) {
        printf("[C%d] consumed %d\n", wa->id, item);
        consumed++;
    }
    printf("[C%d] done (consumed %d items)\n", wa->id, consumed);
    return NULL;
}

int main(void) {
    bounded_buffer_t bb;
    bb_init(&bb, 5);

    const int NUM_PRODUCERS = 3;
    const int NUM_CONSUMERS = 2;
    const int ITEMS_PER_PRODUCER = 10;

    pthread_t prod_threads[NUM_PRODUCERS];
    pthread_t cons_threads[NUM_CONSUMERS];
    worker_args_t prod_args[NUM_PRODUCERS];
    worker_args_t cons_args[NUM_CONSUMERS];

    // Start consumers first (they'll block on empty buffer)
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        cons_args[i] = (worker_args_t){i, &bb, 0};
        pthread_create(&cons_threads[i], NULL, consumer, &cons_args[i]);
    }

    // Start producers
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        prod_args[i] = (worker_args_t){i, &bb, ITEMS_PER_PRODUCER};
        pthread_create(&prod_threads[i], NULL, producer, &prod_args[i]);
    }

    // Wait for producers to finish
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prod_threads[i], NULL);
    }

    // Signal consumers to shut down
    bb_shutdown(&bb);

    // Wait for consumers to finish
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(cons_threads[i], NULL);
    }

    printf("All done. Total produced: %d\n",
           NUM_PRODUCERS * ITEMS_PER_PRODUCER);

    bb_destroy(&bb);
    return 0;
}
```

```bash
gcc -Wall -Wextra -pthread -o prodcons prodcons.c && ./prodcons
```

---

## 13. PThreads Pitfalls

### Common Mistakes

| # | Pitfall | Symptom | Fix |
|---|---------|---------|-----|
| 1 | Forgetting `-pthread` flag | Undefined symbols at link time | Always use `gcc -pthread` |
| 2 | Passing stack variable address that goes out of scope | Segfault or garbage data | Use heap allocation or ensure lifetime |
| 3 | Forgetting to join/detach | Resource leak (zombie threads) | Always join or detach every thread |
| 4 | Using `if` instead of `while` for condvar | Race condition, wrong behavior | Always use `while` loop |
| 5 | Calling `signal` without holding mutex | Lost signal race | Signal while holding mutex (preferred) |
| 6 | Forgetting to unlock mutex on error path | Deadlock | Use goto-cleanup pattern or RAII in C++ |
| 7 | Double-locking a non-recursive mutex | Deadlock | Use `PTHREAD_MUTEX_ERRORCHECK` during development |
| 8 | Destroying a locked mutex | Undefined behavior | Always unlock before destroy |
| 9 | Returning pointer to local variable from thread | Dangling pointer | Use malloc or thread-local storage |
| 10 | Not checking return values | Silent failures | Check every PThreads function return |

### Pitfall 6 Fix: Cleanup Pattern

```c
int do_work(void) {
    int result = -1;

    pthread_mutex_lock(&mutex);

    if (allocate_resource() < 0)
        goto cleanup;

    if (do_step_1() < 0)
        goto cleanup;

    if (do_step_2() < 0)
        goto cleanup;

    result = 0;  // success

cleanup:
    pthread_mutex_unlock(&mutex);  // Always unlocked
    return result;
}
```

> **Quiz: PThreads Pitfalls**
>
> *What is the bug in this code?*
> ```c
> void *thread_func(void *arg) {
>     int result = 42;
>     return &result;  // BUG!
> }
> ```
>
> **Answer:** `result` is a local variable on the thread's stack. When the thread returns, the stack frame is deallocated, and the pointer becomes dangling. The joining thread would read garbage. Fix: use `malloc()` or `pthread_exit()` with a heap-allocated value.

---

## 14. Windows Threading API Comparison

| PThreads | Windows | Notes |
|----------|---------|-------|
| `pthread_t` | `HANDLE` | Thread handle |
| `pthread_create()` | `CreateThread()` / `_beginthreadex()` | Use `_beginthreadex` for C runtime safety |
| `pthread_join()` | `WaitForSingleObject(h, INFINITE)` + `GetExitCodeThread()` | |
| `pthread_detach()` | `CloseHandle()` after create | Immediate close if no join needed |
| `pthread_exit()` | `ExitThread()` / `_endthreadex()` | |
| `pthread_self()` | `GetCurrentThreadId()` | |
| `pthread_mutex_t` | `CRITICAL_SECTION` or `SRWLOCK` | CS is fastest for in-process |
| `pthread_mutex_lock()` | `EnterCriticalSection()` | |
| `pthread_mutex_trylock()` | `TryEnterCriticalSection()` | |
| `pthread_mutex_unlock()` | `LeaveCriticalSection()` | |
| `pthread_cond_t` | `CONDITION_VARIABLE` | Added in Vista |
| `pthread_cond_wait()` | `SleepConditionVariableCS()` | |
| `pthread_cond_signal()` | `WakeConditionVariable()` | |
| `pthread_cond_broadcast()` | `WakeAllConditionVariable()` | |
| `pthread_rwlock_t` | `SRWLOCK` (Slim Reader/Writer Lock) | |
| `pthread_barrier_t` | Manual implementation with events | No native equivalent |
| `pthread_key_t` (TLS) | `TlsAlloc()` / `__declspec(thread)` | |

### Full Windows Example: Thread Pool Pattern

```c
#include <windows.h>
#include <stdio.h>

#define NUM_WORKERS 4
#define NUM_TASKS 20

int task_queue[100];
int queue_head = 0, queue_tail = 0, queue_count = 0;
BOOL shutdown_flag = FALSE;

CRITICAL_SECTION cs;
CONDITION_VARIABLE cv_not_empty;

void enqueue_task(int task) {
    EnterCriticalSection(&cs);
    task_queue[queue_tail] = task;
    queue_tail = (queue_tail + 1) % 100;
    queue_count++;
    WakeConditionVariable(&cv_not_empty);
    LeaveCriticalSection(&cs);
}

int dequeue_task(int *task) {
    EnterCriticalSection(&cs);
    while (queue_count == 0 && !shutdown_flag) {
        SleepConditionVariableCS(&cv_not_empty, &cs, INFINITE);
    }
    if (queue_count == 0 && shutdown_flag) {
        LeaveCriticalSection(&cs);
        return -1;
    }
    *task = task_queue[queue_head];
    queue_head = (queue_head + 1) % 100;
    queue_count--;
    LeaveCriticalSection(&cs);
    return 0;
}

DWORD WINAPI worker_thread(LPVOID arg) {
    int id = (int)(intptr_t)arg;
    int task;
    while (dequeue_task(&task) == 0) {
        printf("Worker %d: processing task %d\n", id, task);
        Sleep(100);  // Simulate work
    }
    printf("Worker %d: shutting down\n", id);
    return 0;
}

int main(void) {
    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&cv_not_empty);

    HANDLE workers[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++) {
        workers[i] = CreateThread(NULL, 0, worker_thread,
                                  (LPVOID)(intptr_t)i, 0, NULL);
    }

    // Submit tasks
    for (int i = 0; i < NUM_TASKS; i++) {
        enqueue_task(i);
        Sleep(50);
    }

    // Shutdown
    EnterCriticalSection(&cs);
    shutdown_flag = TRUE;
    WakeAllConditionVariable(&cv_not_empty);
    LeaveCriticalSection(&cs);

    WaitForMultipleObjects(NUM_WORKERS, workers, TRUE, INFINITE);
    for (int i = 0; i < NUM_WORKERS; i++) CloseHandle(workers[i]);
    DeleteCriticalSection(&cs);

    printf("All tasks completed.\n");
    return 0;
}
```

---

## 15. Quizzes and Exercises

### Exercise 1: Thread-Safe Stack

Implement a thread-safe stack using PThreads with `push()`, `pop()`, and `is_empty()`.

### Exercise 2: Dining Philosophers

Implement the dining philosophers problem using PThreads mutexes. Test with 5 philosophers and demonstrate deadlock prevention via lock ordering.

### Exercise 3: Barrier Implementation

Implement a barrier using a mutex and condition variable. Verify with N threads that all reach the barrier before any continues.

```c
// Barrier structure
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    int count;
    int threshold;
    int generation;  // Prevents early wakeup from previous barrier
} barrier_t;

void barrier_init(barrier_t *b, int n) {
    pthread_mutex_init(&b->mutex, NULL);
    pthread_cond_init(&b->cv, NULL);
    b->count = 0;
    b->threshold = n;
    b->generation = 0;
}

void barrier_wait(barrier_t *b) {
    pthread_mutex_lock(&b->mutex);
    int gen = b->generation;
    b->count++;
    if (b->count == b->threshold) {
        b->count = 0;
        b->generation++;
        pthread_cond_broadcast(&b->cv);
    } else {
        while (gen == b->generation) {
            pthread_cond_wait(&b->cv, &b->mutex);
        }
    }
    pthread_mutex_unlock(&b->mutex);
}
```

---

## 16. Key Takeaways

1. PThreads is the standard threading API on Unix/Linux; Windows has its own API with similar semantics.
2. Always pass thread arguments by stable pointer (not loop variable reference); always check return values.
3. Mutexes come in types (normal, errorcheck, recursive); use **errorcheck** during development.
4. Condition variables enable "wait until condition is true" semantics; always use **while loops** to guard against spurious wakeups.
5. The producer-consumer pattern is the canonical condvar use case: `not_full` and `not_empty` conditions.
6. Common pitfalls: zombie threads (no join/detach), returning stack pointers, forgetting to unlock on error paths.
7. `pthread_cond_signal` wakes one thread; `pthread_cond_broadcast` wakes all; use broadcast when multiple types of waiters exist.

---

## 17. Thread-Local Storage (TLS)

Thread-local storage gives each thread its own private copy of a variable - no synchronization needed.

```c
#include <pthread.h>
#include <stdio.h>

/* ── Method 1: __thread keyword (GCC/Clang extension, most common) ── */
__thread int thread_errno = 0;       /* Each thread gets its own copy */
__thread char thread_name[32] = "";

/* ── Method 2: pthread_key_t (POSIX portable, supports destructor) ── */
pthread_key_t tls_key;

void tls_destructor(void *value) {
    printf("Thread exiting, freeing TLS value at %p\n", value);
    free(value);
}

void *worker(void *arg) {
    int id = *(int *)arg;

    /* __thread: just use directly */
    thread_errno = id * 10;
    snprintf(thread_name, sizeof(thread_name), "worker-%d", id);
    printf("Thread %s: thread_errno = %d (addr %p)\n",
           thread_name, thread_errno, &thread_errno);

    /* pthread_key: set per-thread value */
    int *my_data = malloc(sizeof(int));
    *my_data = id * 100;
    pthread_setspecific(tls_key, my_data);

    /* Later: retrieve */
    int *retrieved = (int *)pthread_getspecific(tls_key);
    printf("Thread %d: TLS key value = %d\n", id, *retrieved);

    return NULL;
}

int main(void) {
    /* Create TLS key with destructor */
    pthread_key_create(&tls_key, tls_destructor);

    int ids[] = {1, 2, 3, 4};
    pthread_t threads[4];
    for (int i = 0; i < 4; i++)
        pthread_create(&threads[i], NULL, worker, &ids[i]);
    for (int i = 0; i < 4; i++)
        pthread_join(threads[i], NULL);

    /* tls_destructor called automatically for each thread on exit */
    pthread_key_delete(tls_key);
    return 0;
}
```

```c
/* ── Method 3: C11 _Thread_local (standard C) ── */
#include <threads.h>  /* C11 */

_Thread_local int per_thread_counter = 0;

/* Or equivalently in GCC/Clang: */
thread_local int per_thread_counter2 = 0;  /* C23 keyword */

/*
 * TLS implementation:
 * - Compiler generates each thread a TLS block in its stack region
 * - Access via %fs or %gs segment register on x86-64
 * - Nearly as fast as a global variable (1 extra instruction for segment offset)
 *
 * Common uses:
 *   errno (libc)  - each thread has its own errno
 *   OpenSSL error state
 *   Allocator per-thread caches (tcmalloc, jemalloc)
 */
```

---

## 18. Pthread Cleanup Handlers and Cancellation

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Cleanup handlers ensure resources are released even if the thread
 * is cancelled or exits unexpectedly. They work like a LIFO stack. */

void cleanup_unlock(void *arg) {
    pthread_mutex_t *mtx = (pthread_mutex_t *)arg;
    printf("Cleanup: unlocking mutex\n");
    pthread_mutex_unlock(mtx);
}

void cleanup_free(void *arg) {
    printf("Cleanup: freeing %p\n", arg);
    free(arg);
}

void cleanup_close_fd(void *arg) {
    int fd = *(int *)arg;
    printf("Cleanup: closing fd %d\n", fd);
    close(fd);
}

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *cancellable_worker(void *arg) {
    char *buffer = malloc(1024);

    /* Push cleanup handlers (LIFO order - first pushed, last executed) */
    pthread_cleanup_push(cleanup_free, buffer);
    pthread_cleanup_push(cleanup_unlock, &lock);

    pthread_mutex_lock(&lock);

    /* This is a cancellation point. If cancelled here,
     * cleanup_unlock runs first, then cleanup_free. */
    printf("Working with buffer at %p...\n", buffer);
    sleep(10);  /* Cancellation point: sleep, read, write, etc. */

    pthread_mutex_unlock(&lock);

    /* Pop handlers. Arg=0: don't execute. Arg=1: execute now. */
    pthread_cleanup_pop(0);  /* Don't execute unlock (we already did it) */
    pthread_cleanup_pop(1);  /* Execute free(buffer) */

    return NULL;
}

int main(void) {
    pthread_t t;
    pthread_create(&t, NULL, cancellable_worker, NULL);

    sleep(1);
    printf("Cancelling thread...\n");
    pthread_cancel(t);  /* Sends cancellation request */

    void *retval;
    pthread_join(t, &retval);
    if (retval == PTHREAD_CANCELED)
        printf("Thread was cancelled\n");
    return 0;
}
```

```c
/*
 * Cancellation types:
 *
 * PTHREAD_CANCEL_DEFERRED (default):
 *   Thread is cancelled at the next cancellation point (sleep, read,
 *   write, pthread_cond_wait, pthread_testcancel, etc.)
 *
 * PTHREAD_CANCEL_ASYNCHRONOUS:
 *   Thread can be cancelled at ANY point (dangerous - use only in
 *   pure computation loops with no resource holding)
 *
 * PTHREAD_CANCEL_DISABLE:
 *   Cancellation requests are held pending. Useful for critical sections.
 */

void *safe_worker(void *arg) {
    int old_state;

    /* Disable cancellation during critical operation */
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &old_state);
    /* ... critical section: cannot be cancelled ... */
    write_to_database();
    pthread_setcancelstate(old_state, NULL);  /* Restore */

    /* Explicit cancellation point */
    pthread_testcancel();  /* If cancel pending, thread exits here */

    return NULL;
}
```

---

## 19. Robust Mutexes: Surviving Owner Death

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/*
 * Normal mutex problem: if the owning thread crashes or is killed,
 * the mutex stays locked forever. Other threads deadlock.
 *
 * Robust mutex: kernel detects owner death and returns EOWNERDEAD
 * to the next locker, who can then repair the state.
 */

pthread_mutex_t robust_lock;

void init_robust_mutex(void) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setrobust(&attr, PTHREAD_MUTEX_ROBUST);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);  /* cross-process */
    pthread_mutex_init(&robust_lock, &attr);
    pthread_mutexattr_destroy(&attr);
}

void *lock_and_maybe_die(void *arg) {
    int should_die = *(int *)arg;

    int ret = pthread_mutex_lock(&robust_lock);
    if (ret == EOWNERDEAD) {
        printf("Previous owner died! Making mutex consistent...\n");
        /* Repair shared state here */
        pthread_mutex_consistent(&robust_lock);
    }

    printf("Thread has the lock\n");

    if (should_die) {
        printf("Thread dying while holding the lock!\n");
        pthread_exit(NULL);  /* Exit without unlocking */
    }

    pthread_mutex_unlock(&robust_lock);
    return NULL;
}

int main(void) {
    init_robust_mutex();

    int die = 1, live = 0;
    pthread_t t1, t2;

    /* Thread 1: locks and dies */
    pthread_create(&t1, NULL, lock_and_maybe_die, &die);
    pthread_join(t1, NULL);

    /* Thread 2: gets EOWNERDEAD, recovers */
    pthread_create(&t2, NULL, lock_and_maybe_die, &live);
    pthread_join(t2, NULL);

    pthread_mutex_destroy(&robust_lock);
    return 0;
}
```

---

## 20. POSIX Named Semaphores: Cross-Process Synchronization

```c
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Named semaphores live in the filesystem (/dev/shm on Linux)
 * and can synchronize unrelated processes.
 */

int main(void) {
    /* Create a named semaphore, initial value 1 (acts like a mutex) */
    sem_t *sem = sem_open("/my_semaphore", O_CREAT, 0644, 1);
    if (sem == SEM_FAILED) { perror("sem_open"); return 1; }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child process */
        sem_t *child_sem = sem_open("/my_semaphore", 0);

        printf("Child: waiting for semaphore...\n");
        sem_wait(child_sem);
        printf("Child: acquired semaphore, doing work...\n");
        sleep(2);
        printf("Child: releasing semaphore\n");
        sem_post(child_sem);

        sem_close(child_sem);
        _exit(0);
    }

    /* Parent process */
    sleep(1);  /* Let child start waiting */
    printf("Parent: waiting for semaphore...\n");
    sem_wait(sem);
    printf("Parent: acquired semaphore, doing work...\n");
    sleep(1);
    printf("Parent: releasing semaphore\n");
    sem_post(sem);

    wait(NULL);
    sem_close(sem);
    sem_unlink("/my_semaphore");  /* Remove from filesystem */
    return 0;
}
```

```bash
# Named semaphores on Linux live in /dev/shm
ls -la /dev/shm/sem.*

# Unnamed (anonymous) semaphore: for threads or shared memory
# sem_init(&sem, pshared, initial_value)
#   pshared=0: thread-shared
#   pshared=1: process-shared (must be in shared memory)

# Timed wait (avoid indefinite blocking)
struct timespec ts;
clock_gettime(CLOCK_REALTIME, &ts);
ts.tv_sec += 5;  /* 5 second timeout */
int ret = sem_timedwait(sem, &ts);
if (ret == -1 && errno == ETIMEDOUT)
    printf("Semaphore timed out\n");
```

---

## 21. Thread-Safe Data Structures: Concurrent Hash Map

```c
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
 * Striped-lock concurrent hash map:
 * Instead of one lock for the entire map, use N locks for N buckets.
 * This allows N concurrent readers/writers (one per bucket).
 * Real-world pattern used by Java ConcurrentHashMap, Go sync.Map.
 */

#define NUM_BUCKETS 64
#define NUM_STRIPES 16  /* 16 locks covering 64 buckets (4 buckets per lock) */

typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

typedef struct {
    Entry *buckets[NUM_BUCKETS];
    pthread_rwlock_t locks[NUM_STRIPES];  /* Reader-writer locks per stripe */
} ConcurrentMap;

void cmap_init(ConcurrentMap *map) {
    memset(map->buckets, 0, sizeof(map->buckets));
    for (int i = 0; i < NUM_STRIPES; i++)
        pthread_rwlock_init(&map->locks[i], NULL);
}

static unsigned hash(const char *key) {
    unsigned h = 5381;
    for (; *key; key++) h = h * 33 + *key;
    return h;
}

static int stripe_for(unsigned bucket) {
    return bucket % NUM_STRIPES;
}

void cmap_put(ConcurrentMap *map, const char *key, int value) {
    unsigned b = hash(key) % NUM_BUCKETS;
    int s = stripe_for(b);

    pthread_rwlock_wrlock(&map->locks[s]);  /* Write lock on stripe */

    /* Check if key exists */
    for (Entry *e = map->buckets[b]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            pthread_rwlock_unlock(&map->locks[s]);
            return;
        }
    }

    /* New entry */
    Entry *e = malloc(sizeof(Entry));
    e->key = strdup(key);
    e->value = value;
    e->next = map->buckets[b];
    map->buckets[b] = e;

    pthread_rwlock_unlock(&map->locks[s]);
}

int cmap_get(ConcurrentMap *map, const char *key, int *out) {
    unsigned b = hash(key) % NUM_BUCKETS;
    int s = stripe_for(b);

    pthread_rwlock_rdlock(&map->locks[s]);  /* Read lock: multiple readers OK */

    for (Entry *e = map->buckets[b]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            *out = e->value;
            pthread_rwlock_unlock(&map->locks[s]);
            return 1;
        }
    }

    pthread_rwlock_unlock(&map->locks[s]);
    return 0;  /* Not found */
}

void cmap_destroy(ConcurrentMap *map) {
    for (int b = 0; b < NUM_BUCKETS; b++) {
        Entry *e = map->buckets[b];
        while (e) {
            Entry *next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
    }
    for (int i = 0; i < NUM_STRIPES; i++)
        pthread_rwlock_destroy(&map->locks[i]);
}

/* Benchmark: 8 threads, 1M operations each */
void *bench_worker(void *arg) {
    ConcurrentMap *map = (ConcurrentMap *)arg;
    char key[32];
    for (int i = 0; i < 1000000; i++) {
        snprintf(key, sizeof(key), "key-%d-%d", (int)pthread_self() % 100, i % 10000);
        cmap_put(map, key, i);
        int val;
        cmap_get(map, key, &val);
    }
    return NULL;
}
```

```bash
# Compile and benchmark
gcc -O2 -pthread -o cmap_bench cmap_bench.c && time ./cmap_bench
# With 16 stripes covering 64 buckets: ~4x faster than single-lock
# With read-heavy workload: rwlock gives even bigger gains (readers don't block each other)

# Verify thread safety with TSan
gcc -fsanitize=thread -g -O1 -pthread -o cmap_tsan cmap_bench.c
./cmap_tsan
```

---

## 22. Windows Threading: CreateThread and Synchronization

```c
#include <windows.h>
#include <stdio.h>

/* Windows threading API comparison with PThreads */

typedef struct {
    int id;
    int result;
} ThreadArg;

DWORD WINAPI worker(LPVOID param) {
    ThreadArg *arg = (ThreadArg *)param;
    printf("Thread %d running\n", arg->id);
    arg->result = arg->id * arg->id;
    return 0;
}

int main(void) {
    #define N 4
    HANDLE threads[N];
    ThreadArg args[N];

    /* Create threads */
    for (int i = 0; i < N; i++) {
        args[i].id = i;
        threads[i] = CreateThread(
            NULL,       /* Security attributes */
            0,          /* Stack size (0 = default 1MB) */
            worker,     /* Thread function */
            &args[i],   /* Argument */
            0,          /* Creation flags (0 = run immediately) */
            NULL        /* Thread ID output (optional) */
        );
    }

    /* Wait for all threads */
    WaitForMultipleObjects(N, threads, TRUE /* wait all */, INFINITE);

    /* Get results and cleanup */
    for (int i = 0; i < N; i++) {
        DWORD exit_code;
        GetExitCodeThread(threads[i], &exit_code);
        printf("Thread %d: result=%d, exit_code=%lu\n",
               args[i].id, args[i].result, exit_code);
        CloseHandle(threads[i]);
    }
    return 0;
}
```

```powershell
# PowerShell: Thread inspection
Get-Process -Id $pid | ForEach-Object {
    $_.Threads | Select-Object Id, ThreadState, WaitReason,
        TotalProcessorTime, CurrentPriority
}

# Thread count by process
Get-Process | Where-Object { $_.Threads.Count -gt 10 } |
    Select-Object Name, Id, @{N='Threads';E={$_.Threads.Count}} |
    Sort-Object Threads -Descending | Format-Table
```

---

**Previous:** [P2L2: Threads and Concurrency](P2L2-Threads-and-Concurrency.md)
**Next:** [P2L4: Thread Design Considerations](P2L4-Thread-Design-Considerations.md)

