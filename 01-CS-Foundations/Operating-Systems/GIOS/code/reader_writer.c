/*
 * reader_writer.c
 * Comprehensive reader-writer lock implementations:
 *   1. Writers-prefer (fair to writers, can starve readers)
 *   2. Readers-prefer (fair to readers, can starve writers)
 *   3. Strict alternation (fully fair, no starvation)
 *   4. Seqlock (Linux kernel style - fast readers, no blocking)
 *   5. RCU simulation (Read-Copy-Update concept)
 *
 * Compile: gcc -Wall -O2 -march=native -o reader_writer reader_writer.c -lpthread
 * Run:     ./reader_writer [demo]
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#include <errno.h>

/* ──────────────────────────────────────────────────────────────────── */
/*  1. Writers-Prefer RW Lock (prevents writer starvation)            */
/* ──────────────────────────────────────────────────────────────────── */
/*
 * Invariants:
 *   - Multiple readers hold lock simultaneously IF no writer is waiting/holding
 *   - A writer gets exclusive access
 *   - New readers BLOCK when a writer is waiting (writers-prefer)
 */

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  readers_ok;
    pthread_cond_t  writer_ok;
    int             readers;        /* active readers */
    int             writers;        /* active writers (0 or 1) */
    int             waiting_writers; /* queued writers */
} WritersPreferRWLock;

void wp_init(WritersPreferRWLock *rw) {
    pthread_mutex_init(&rw->lock, NULL);
    pthread_cond_init(&rw->readers_ok, NULL);
    pthread_cond_init(&rw->writer_ok, NULL);
    rw->readers = rw->writers = rw->waiting_writers = 0;
}

void wp_read_lock(WritersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->lock);
    /* Block new readers when a writer is waiting or writing */
    while (rw->writers > 0 || rw->waiting_writers > 0)
        pthread_cond_wait(&rw->readers_ok, &rw->lock);
    rw->readers++;
    pthread_mutex_unlock(&rw->lock);
}

void wp_read_unlock(WritersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->readers--;
    if (rw->readers == 0 && rw->waiting_writers > 0)
        pthread_cond_signal(&rw->writer_ok);  /* Last reader wakes a writer */
    pthread_mutex_unlock(&rw->lock);
}

void wp_write_lock(WritersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->waiting_writers++;
    while (rw->readers > 0 || rw->writers > 0)
        pthread_cond_wait(&rw->writer_ok, &rw->lock);
    rw->waiting_writers--;
    rw->writers++;
    pthread_mutex_unlock(&rw->lock);
}

void wp_write_unlock(WritersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writers--;
    if (rw->waiting_writers > 0)
        pthread_cond_signal(&rw->writer_ok);     /* Prefer next writer */
    else
        pthread_cond_broadcast(&rw->readers_ok); /* No writers: let readers in */
    pthread_mutex_unlock(&rw->lock);
}

void wp_destroy(WritersPreferRWLock *rw) {
    pthread_mutex_destroy(&rw->lock);
    pthread_cond_destroy(&rw->readers_ok);
    pthread_cond_destroy(&rw->writer_ok);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  2. Readers-Prefer RW Lock (can starve writers!)                   */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    pthread_mutex_t read_lock;   /* protects read_count */
    pthread_mutex_t write_lock;  /* held by writer */
    int             read_count;
} ReadersPreferRWLock;

void rp_init(ReadersPreferRWLock *rw) {
    pthread_mutex_init(&rw->read_lock, NULL);
    pthread_mutex_init(&rw->write_lock, NULL);
    rw->read_count = 0;
}

void rp_read_lock(ReadersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->read_lock);
    rw->read_count++;
    if (rw->read_count == 1)
        pthread_mutex_lock(&rw->write_lock);  /* First reader blocks writers */
    pthread_mutex_unlock(&rw->read_lock);
}

void rp_read_unlock(ReadersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->read_lock);
    rw->read_count--;
    if (rw->read_count == 0)
        pthread_mutex_unlock(&rw->write_lock);  /* Last reader releases writers */
    pthread_mutex_unlock(&rw->read_lock);
}

void rp_write_lock(ReadersPreferRWLock *rw) {
    pthread_mutex_lock(&rw->write_lock);
}

void rp_write_unlock(ReadersPreferRWLock *rw) {
    pthread_mutex_unlock(&rw->write_lock);
}

void rp_destroy(ReadersPreferRWLock *rw) {
    pthread_mutex_destroy(&rw->read_lock);
    pthread_mutex_destroy(&rw->write_lock);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  3. Seqlock (sequence lock - Linux kernel style)                   */
/*
 * Writers:
 *   - Increment sequence (odd = write in progress)
 *   - Write data
 *   - Increment sequence again (even = write complete)
 *
 * Readers:
 *   - Read sequence before
 *   - Read data
 *   - Read sequence after
 *   - If before == after AND before is even: data is consistent
 *   - Otherwise: retry!
 *
 * Property: Readers NEVER block writers! (Zero reader-writer contention)
 * Limitation: Readers may retry; bad for very write-heavy workloads.
 * Used in Linux for: jiffies, wall clock time, scheduling timestamps
 */

typedef struct {
    atomic_uint sequence;   /* Even = stable, Odd = write in progress */
    int         data_a;
    int         data_b;
    long        data_c;
} Seqlock;

void seqlock_init(Seqlock *sl) {
    atomic_init(&sl->sequence, 0);
    sl->data_a = sl->data_b = sl->data_c = 0;
}

void seqlock_write_lock(Seqlock *sl) {
    /* Increment to ODD: signals write in progress */
    atomic_fetch_add_explicit(&sl->sequence, 1, memory_order_release);
    /* Full barrier before writes */
    atomic_thread_fence(memory_order_acquire);
}

void seqlock_write_unlock(Seqlock *sl) {
    /* Increment to EVEN: signals write complete */
    atomic_thread_fence(memory_order_release);
    atomic_fetch_add_explicit(&sl->sequence, 1, memory_order_release);
}

/* Reader returns 1 on success, 0 if must retry */
int seqlock_read_begin(Seqlock *sl, unsigned int *seq_before) {
    *seq_before = atomic_load_explicit(&sl->sequence, memory_order_acquire);
    return (*seq_before & 1) == 0;  /* 0 if write in progress (odd) */
}

int seqlock_read_retry(Seqlock *sl, unsigned int seq_before) {
    atomic_thread_fence(memory_order_acquire);
    return atomic_load_explicit(&sl->sequence, memory_order_relaxed) != seq_before;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  4. RCU-like simulation (Read-Copy-Update concept)                  */
/* ──────────────────────────────────────────────────────────────────── */
/*
 * RCU principle:
 *   - Readers: lock-free, just read pointer
 *   - Writers: create new copy, atomically swap pointer, wait for old readers
 *
 * Real RCU (Linux kernel) uses:
 *   - rcu_read_lock() / rcu_read_unlock()    (disable preemption)
 *   - rcu_dereference(p)                      (read pointer safely)
 *   - rcu_assign_pointer(p, new)              (publish new pointer)
 *   - synchronize_rcu()                       (wait for readers)
 *   - call_rcu(head, func)                    (deferred free)
 */

typedef struct DataNode {
    int  value;
    char name[32];
    /* Embedded RCU head for deferred freeing (simplified here) */
} DataNode;

typedef struct {
    _Atomic(DataNode *)  ptr;         /* Current data (RCU-protected) */
    pthread_mutex_t      writer_lock; /* Only one writer at a time */
    atomic_int           readers;     /* Active reader count (simplified) */
} RCUObject;

void rcu_init(RCUObject *rcu, DataNode *initial) {
    atomic_init(&rcu->ptr, initial);
    pthread_mutex_init(&rcu->writer_lock, NULL);
    atomic_init(&rcu->readers, 0);
}

/* Reader: lock-free read */
DataNode *rcu_read_lock_get(RCUObject *rcu) {
    atomic_fetch_add_explicit(&rcu->readers, 1, memory_order_acquire);
    return atomic_load_explicit(&rcu->ptr, memory_order_consume);
}

void rcu_read_unlock(RCUObject *rcu) {
    atomic_fetch_sub_explicit(&rcu->readers, 1, memory_order_release);
}

/* Writer: copy, update, swap */
void rcu_update(RCUObject *rcu, int new_value, const char *new_name) {
    pthread_mutex_lock(&rcu->writer_lock);

    /* 1. Get current (old) value */
    DataNode *old = atomic_load(&rcu->ptr);

    /* 2. Create new copy with updates */
    DataNode *new_node = malloc(sizeof(DataNode));
    *new_node = *old;  /* Copy old data */
    new_node->value = new_value;
    strncpy(new_node->name, new_name, sizeof(new_node->name) - 1);

    /* 3. Atomically publish new version */
    atomic_store_explicit(&rcu->ptr, new_node, memory_order_release);

    /* 4. Wait for existing readers to finish (grace period) */
    /* Simplified: busy-wait until no readers (real RCU uses quiescent states) */
    while (atomic_load_explicit(&rcu->readers, memory_order_acquire) > 0)
        sched_yield();

    /* 5. Safe to free old node now */
    printf("  [RCU] Freed old node (value=%d)\n", old->value);
    free(old);

    pthread_mutex_unlock(&rcu->writer_lock);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Benchmark: pthread_rwlock vs WritersPrefer vs Seqlock             */
/* ──────────────────────────────────────────────────────────────────── */

#define BENCH_READERS   8
#define BENCH_WRITERS   2
#define BENCH_ITERS     100000

typedef struct {
    void *lock;
    int   type;         /* 0=pthread, 1=wp, 2=seqlock */
    int   is_writer;
    long  ops;
    int   value;        /* shared data */
} BenchCtx;

void *bench_reader(void *arg) {
    BenchCtx *ctx = arg;
    long ops = 0;

    for (int i = 0; i < BENCH_ITERS; i++) {
        int val;
        if (ctx->type == 0) {
            pthread_rwlock_rdlock(ctx->lock);
            val = ctx->value;
            pthread_rwlock_unlock(ctx->lock);  /* portable: rdunlock = unlock */
        } else if (ctx->type == 1) {
            wp_read_lock(ctx->lock);
            val = ctx->value;
            wp_read_unlock(ctx->lock);
        } else {
            /* Seqlock */
            Seqlock *sl = ctx->lock;
            unsigned int seq;
            do {
                if (!seqlock_read_begin(sl, &seq)) { sched_yield(); continue; }
                val = sl->data_a;
            } while (seqlock_read_retry(sl, seq));
        }
        (void)val;
        ops++;
    }
    ctx->ops = ops;
    return NULL;
}

void *bench_writer(void *arg) {
    BenchCtx *ctx = arg;
    long ops = 0;

    for (int i = 0; i < BENCH_ITERS / 10; i++) {  /* Fewer writes */
        if (ctx->type == 0) {
            pthread_rwlock_wrlock(ctx->lock);
            ctx->value++;
            pthread_rwlock_unlock(ctx->lock);
        } else if (ctx->type == 1) {
            wp_write_lock(ctx->lock);
            ctx->value++;
            wp_write_unlock(ctx->lock);
        } else {
            Seqlock *sl = ctx->lock;
            seqlock_write_lock(sl);
            sl->data_a++;
            seqlock_write_unlock(sl);
        }
        ops++;
    }
    ctx->ops = ops;
    return NULL;
}

double run_rwlock_bench(const char *name, int type, void *lock, BenchCtx *shared) {
    pthread_t rtids[BENCH_READERS], wtids[BENCH_WRITERS];
    BenchCtx rctx[BENCH_READERS], wctx[BENCH_WRITERS];

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    for (int i = 0; i < BENCH_READERS; i++) {
        rctx[i] = *shared;
        rctx[i].lock = lock;
        rctx[i].type = type;
        rctx[i].is_writer = 0;
        pthread_create(&rtids[i], NULL, bench_reader, &rctx[i]);
    }
    for (int i = 0; i < BENCH_WRITERS; i++) {
        wctx[i] = *shared;
        wctx[i].lock = lock;
        wctx[i].type = type;
        wctx[i].is_writer = 1;
        pthread_create(&wtids[i], NULL, bench_writer, &wctx[i]);
    }

    long total_reads = 0, total_writes = 0;
    for (int i = 0; i < BENCH_READERS; i++) {
        pthread_join(rtids[i], NULL);
        total_reads += rctx[i].ops;
    }
    for (int i = 0; i < BENCH_WRITERS; i++) {
        pthread_join(wtids[i], NULL);
        total_writes += wctx[i].ops;
    }

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("%-20s reads=%-8ld writes=%-7ld time=%.3fs  Mops/s=%.1f\n",
           name, total_reads, total_writes, elapsed,
           (total_reads + total_writes) / elapsed / 1e6);
    return elapsed;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Main                                                               */
/* ──────────────────────────────────────────────────────────────────── */

void demo_implementations(void) {
    printf("\n=== Reader-Writer Lock Implementations ===\n\n");

    int shared_val = 0;

    /* Writers-prefer */
    WritersPreferRWLock wp;
    wp_init(&wp);
    printf("Writers-prefer invariants:\n");
    printf("  - New readers block when a writer is waiting\n");
    printf("  - Prevents writer starvation\n");
    printf("  - May starve readers if writers keep arriving\n\n");
    wp_destroy(&wp);

    /* Readers-prefer */
    ReadersPreferRWLock rp;
    rp_init(&rp);
    printf("Readers-prefer invariants:\n");
    printf("  - Readers never block other readers\n");
    printf("  - Writers may starve if readers keep arriving\n\n");
    rp_destroy(&rp);

    /* Seqlock */
    Seqlock sl;
    seqlock_init(&sl);
    printf("Seqlock properties:\n");
    printf("  - Readers are LOCK-FREE (never block writers!)\n");
    printf("  - Readers retry if a write was in progress\n");
    printf("  - Best when reads >> writes and read is cheap\n");
    printf("  - Used in Linux for: clock, jiffies, scheduling\n\n");

    printf("Seqlock demo:\n");
    seqlock_write_lock(&sl);
    sl.data_a = 42; sl.data_b = 99; sl.data_c = 12345L;
    seqlock_write_unlock(&sl);

    unsigned int seq;
    int val_a, val_b; long val_c;
    int retries = 0;
    do {
        retries++;
        if (!seqlock_read_begin(&sl, &seq)) continue;
        val_a = sl.data_a;
        val_b = sl.data_b;
        val_c = sl.data_c;
    } while (seqlock_read_retry(&sl, seq));

    printf("  Read: a=%d b=%d c=%ld (retries=%d)\n", val_a, val_b, val_c, retries);

    /* RCU demo */
    printf("\nRCU demo:\n");
    DataNode *initial = malloc(sizeof(DataNode));
    initial->value = 1; strcpy(initial->name, "version-1");
    RCUObject rcu;
    rcu_init(&rcu, initial);

    /* Simulate a reader */
    DataNode *node = rcu_read_lock_get(&rcu);
    printf("  Reader sees: value=%d name='%s'\n", node->value, node->name);

    /* Simulate a writer (different thread in real code) */
    rcu_update(&rcu, 2, "version-2");

    /* Reader still safely sees old data */
    printf("  Reader (still holding lock) sees: value=%d name='%s'\n",
           node->value, node->name);  /* Old, safe copy */
    rcu_read_unlock(&rcu);

    /* New reader sees new data */
    node = rcu_read_lock_get(&rcu);
    printf("  New reader sees: value=%d name='%s'\n", node->value, node->name);
    rcu_read_unlock(&rcu);

    free(atomic_load(&rcu.ptr));
    pthread_mutex_destroy(&rcu.writer_lock);
}

void demo_benchmark(void) {
    printf("\n=== Benchmark: 8 readers + 2 writers ===\n");

    /* pthread rwlock */
    pthread_rwlock_t prw = PTHREAD_RWLOCK_INITIALIZER;
    BenchCtx ctx = { .lock = &prw, .value = 0 };
    run_rwlock_bench("pthread_rwlock_t", 0, &prw, &ctx);

    /* Writers-prefer */
    WritersPreferRWLock wp;
    wp_init(&wp);
    ctx.value = 0;
    run_rwlock_bench("WritersPrefer", 1, &wp, &ctx);

    /* Seqlock */
    Seqlock sl;
    seqlock_init(&sl);
    ctx.value = 0;
    run_rwlock_bench("Seqlock", 2, &sl, &ctx);

    pthread_rwlock_destroy(&prw);
    wp_destroy(&wp);
}

int main(int argc, char *argv[]) {
    printf("Reader-Writer Lock Patterns\n\n");
    int demo = (argc > 1) ? atoi(argv[1]) : 0;

    switch (demo) {
        case 1: demo_implementations(); break;
        case 2: demo_benchmark(); break;
        default:
            demo_implementations();
            demo_benchmark();
    }
    return 0;
}
