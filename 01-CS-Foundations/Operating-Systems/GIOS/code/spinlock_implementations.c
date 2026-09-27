/*
 * spinlock_implementations.c
 * Demonstrates TAS, TTAS, Ticket, MCS, and CLH spinlocks
 * with performance benchmarks.
 *
 * Compile: gcc -Wall -O2 -march=native -o spinlocks spinlock_implementations.c -lpthread
 * Run:     ./spinlocks [num_threads] [iterations]
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#define CACHE_LINE_SIZE 64

/* ──────────────────────────────────────────────────────────────────── */
/*  1. TAS Spinlock (Test-and-Set)                                     */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    atomic_int locked;  /* 0 = free, 1 = held */
} TASLock;

void tas_init(TASLock *l) { atomic_init(&l->locked, 0); }

void tas_lock(TASLock *l) {
    /* Spin until we atomically set locked from 0→1 */
    while (atomic_exchange_explicit(&l->locked, 1, memory_order_acquire) == 1) {
        /* Busy wait - hammers cache line shared by all waiters */
        /* Every CAS broadcasts cache invalidation to all cores! */
    }
}

void tas_unlock(TASLock *l) {
    atomic_store_explicit(&l->locked, 0, memory_order_release);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  2. TTAS Spinlock (Test-and-Test-and-Set)                           */
/*     Read (shared) before attempting atomic write                    */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    atomic_int locked;
} TTASLock;

void ttas_init(TTASLock *l) { atomic_init(&l->locked, 0); }

void ttas_lock(TTASLock *l) {
    while (1) {
        /* Phase 1: spin on read (shared cache line, no invalidation) */
        while (atomic_load_explicit(&l->locked, memory_order_relaxed) == 1) {
            /* read-only spin: multiple cores can do this simultaneously
               without invalidating each other's cache lines! */
#if defined(__x86_64__)
            __asm__ volatile("pause" ::: "memory");  /* PAUSE hint for CPU */
#elif defined(__aarch64__)
            __asm__ volatile("yield" ::: "memory");
#endif
        }
        /* Phase 2: attempt the expensive write */
        if (atomic_exchange_explicit(&l->locked, 1, memory_order_acquire) == 0) {
            return;  /* We got the lock */
        }
        /* If we failed (another thread beat us), go back to reading */
    }
}

void ttas_unlock(TTASLock *l) {
    atomic_store_explicit(&l->locked, 0, memory_order_release);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  3. Ticket Lock (fair FIFO ordering)                                */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    atomic_int next_ticket;    /* Next ticket to give out */
    atomic_int now_serving;    /* Ticket currently being served */
    char _pad[CACHE_LINE_SIZE - 2 * sizeof(atomic_int)];
} TicketLock;

void ticket_init(TicketLock *l) {
    atomic_init(&l->next_ticket, 0);
    atomic_init(&l->now_serving, 0);
}

void ticket_lock(TicketLock *l) {
    /* Get our ticket number (atomic fetch-and-add) */
    int my_ticket = atomic_fetch_add_explicit(&l->next_ticket, 1,
                                               memory_order_relaxed);
    /* Wait until it's our turn */
    while (atomic_load_explicit(&l->now_serving, memory_order_acquire)
           != my_ticket) {
#if defined(__x86_64__)
        __asm__ volatile("pause" ::: "memory");
#endif
    }
}

void ticket_unlock(TicketLock *l) {
    /* Advance the counter: next ticket becomes current */
    atomic_fetch_add_explicit(&l->now_serving, 1, memory_order_release);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  4. MCS Lock (Mellor-Crummey Scott)                                 */
/*     Each thread spins on its OWN cache line - no invalidation storm! */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct MCSNode {
    atomic_int          locked;   /* 1 = still waiting */
    struct MCSNode     *next;     /* next waiter in queue */
    char _pad[CACHE_LINE_SIZE - sizeof(atomic_int) - sizeof(void *)];
} MCSNode;

typedef struct {
    _Atomic(MCSNode *) tail;     /* End of the wait queue */
} MCSLock;

void mcs_init(MCSLock *l) {
    atomic_init(&l->tail, NULL);
}

void mcs_lock(MCSLock *l, MCSNode *node) {
    node->next = NULL;
    atomic_init(&node->locked, 1);  /* Start as "waiting" */

    /* Atomically add ourselves to end of queue */
    MCSNode *prev = atomic_exchange_explicit(&l->tail, node, memory_order_acq_rel);

    if (prev != NULL) {
        /* Queue was non-empty: link ourselves after prev */
        prev->next = node;
        /* Spin on OUR OWN node's locked flag (not global state!) */
        while (atomic_load_explicit(&node->locked, memory_order_acquire) == 1) {
#if defined(__x86_64__)
            __asm__ volatile("pause" ::: "memory");
#endif
        }
    }
    /* If prev == NULL, queue was empty → we have the lock directly */
}

void mcs_unlock(MCSLock *l, MCSNode *node) {
    if (node->next == NULL) {
        /* Attempt to set tail to NULL (no waiters) */
        MCSNode *expected = node;
        if (atomic_compare_exchange_strong_explicit(
                &l->tail, &expected, NULL,
                memory_order_release, memory_order_relaxed)) {
            return;  /* Success, no waiters */
        }
        /* A new waiter joined between our check and the CAS */
        /* Spin until they link themselves */
        while (node->next == NULL) {
#if defined(__x86_64__)
            __asm__ volatile("pause" ::: "memory");
#endif
        }
    }
    /* Hand lock to next waiter by clearing their locked flag */
    atomic_store_explicit(&node->next->locked, 0, memory_order_release);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Benchmark infrastructure                                           */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    void  *lock;
    void (*lock_fn)(void *, MCSNode *);
    void (*unlock_fn)(void *, MCSNode *);
    long   counter;       /* protected by the lock */
    int    n_threads;
    long   iterations;
} BenchArgs;

typedef struct {
    BenchArgs *ba;
    int        thread_id;
} ThreadArgs;

/* Lock adapters with uniform signature */
void tas_lock_adapter(void *l, MCSNode *n) { (void)n; tas_lock(l); }
void tas_unlock_adapter(void *l, MCSNode *n) { (void)n; tas_unlock(l); }
void ttas_lock_adapter(void *l, MCSNode *n) { (void)n; ttas_lock(l); }
void ttas_unlock_adapter(void *l, MCSNode *n) { (void)n; ttas_unlock(l); }
void ticket_lock_adapter(void *l, MCSNode *n) { (void)n; ticket_lock(l); }
void ticket_unlock_adapter(void *l, MCSNode *n) { (void)n; ticket_unlock(l); }
void mcs_lock_adapter(void *l, MCSNode *n) { mcs_lock(l, n); }
void mcs_unlock_adapter(void *l, MCSNode *n) { mcs_unlock(l, n); }

void *bench_worker(void *arg) {
    ThreadArgs *ta = arg;
    BenchArgs  *ba = ta->ba;
    MCSNode     node;  /* Thread-local MCS queue node */

    for (long i = 0; i < ba->iterations; i++) {
        ba->lock_fn(ba->lock, &node);
        ba->counter++;  /* Critical section */
        ba->unlock_fn(ba->lock, &node);
    }
    return NULL;
}

double run_benchmark(const char *name, BenchArgs *ba) {
    pthread_t threads[ba->n_threads];
    ThreadArgs targs[ba->n_threads];

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int i = 0; i < ba->n_threads; i++) {
        targs[i] = (ThreadArgs){ .ba = ba, .thread_id = i };
        pthread_create(&threads[i], NULL, bench_worker, &targs[i]);
    }
    for (int i = 0; i < ba->n_threads; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    long expected = (long)ba->n_threads * ba->iterations;
    printf("%-12s threads=%-3d iters=%-8ld result=%-10ld time=%.3fs "
           "Mops/s=%.1f %s\n",
           name, ba->n_threads, ba->iterations,
           ba->counter, elapsed,
           expected / elapsed / 1e6,
           (ba->counter == expected) ? "OK" : "RACE CONDITION!");
    return elapsed;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Main                                                               */
/* ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    int  n_threads  = (argc > 1) ? atoi(argv[1]) : 4;
    long iterations = (argc > 2) ? atol(argv[2]) : 1000000L;

    printf("=== Spinlock Benchmark ===\n");
    printf("Threads: %d  Iterations/thread: %ld\n\n", n_threads, iterations);

    /* TAS */
    TASLock tas = {0}; tas_init(&tas);
    BenchArgs tas_args = { &tas, tas_lock_adapter, tas_unlock_adapter,
                           0, n_threads, iterations };
    run_benchmark("TAS", &tas_args);

    /* TTAS */
    TTASLock ttas = {0}; ttas_init(&ttas);
    BenchArgs ttas_args = { &ttas, ttas_lock_adapter, ttas_unlock_adapter,
                            0, n_threads, iterations };
    run_benchmark("TTAS", &ttas_args);

    /* Ticket */
    TicketLock ticket = {0}; ticket_init(&ticket);
    BenchArgs ticket_args = { &ticket, ticket_lock_adapter, ticket_unlock_adapter,
                              0, n_threads, iterations };
    run_benchmark("Ticket", &ticket_args);

    /* MCS */
    MCSLock mcs; mcs_init(&mcs);
    BenchArgs mcs_args = { &mcs, mcs_lock_adapter, mcs_unlock_adapter,
                           0, n_threads, iterations };
    run_benchmark("MCS", &mcs_args);

    /* Pthread mutex (for comparison) */
    pthread_mutex_t pmtx = PTHREAD_MUTEX_INITIALIZER;
    /* Use wrapper to fit uniform interface */
    /* (direct pthread_mutex_lock doesn't match our adapter signature) */

    printf("\nExpected counter per lock: %ld\n",
           (long)n_threads * iterations);
    printf("\nPerformance notes:\n");
    printf("  TAS    - Simple but poor under contention (cache invalidation storm)\n");
    printf("  TTAS   - Better: reads before atomic write, uses PAUSE hint\n");
    printf("  Ticket - Fair (FIFO) but still all spin on same cache line\n");
    printf("  MCS    - Best: each thread spins on its own cache line\n");

    return 0;
}
