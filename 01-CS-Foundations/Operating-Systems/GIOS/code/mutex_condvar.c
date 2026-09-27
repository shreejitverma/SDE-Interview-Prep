/*
 * mutex_condvar.c
 * Comprehensive mutex and condition variable patterns:
 *   - Basic mutex usage and error handling
 *   - Recursive mutex
 *   - Timed mutex (pthread_mutex_timedlock)
 *   - Read-write lock (pthread_rwlock)
 *   - Condition variable: signal vs broadcast
 *   - Monitor pattern
 *   - Barrier implementation
 *   - Once-initialization (pthread_once)
 *
 * Compile: gcc -Wall -O2 -o mutex_condvar mutex_condvar.c -lpthread
 * Run:     ./mutex_condvar [demo]
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdatomic.h>

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 1: Basic Mutex Patterns                                       */
/* ──────────────────────────────────────────────────────────────────── */

void demo_basic_mutex(void) {
    printf("\n=== Demo 1: Basic Mutex Patterns ===\n");

    /* Static initialization (compile-time, for global/static mutexes) */
    static pthread_mutex_t static_mtx = PTHREAD_MUTEX_INITIALIZER;

    /* Dynamic initialization (runtime, for local or heap mutexes) */
    pthread_mutex_t dynamic_mtx;
    pthread_mutex_init(&dynamic_mtx, NULL);  /* NULL = default attributes */

    /* Basic lock/unlock */
    pthread_mutex_lock(&static_mtx);
    printf("In critical section (static mutex)\n");
    pthread_mutex_unlock(&static_mtx);

    /* trylock: non-blocking attempt */
    if (pthread_mutex_trylock(&static_mtx) == 0) {
        printf("Acquired mutex with trylock\n");
        pthread_mutex_unlock(&static_mtx);
    } else {
        printf("Mutex busy (trylock failed)\n");
    }

    /* Timed lock: wait up to 500ms */
    struct timespec deadline;
    clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_nsec += 500 * 1000000L;  /* Add 500ms */
    if (deadline.tv_nsec >= 1000000000L) {
        deadline.tv_sec++;
        deadline.tv_nsec -= 1000000000L;
    }

#ifdef __linux__
    int rc = pthread_mutex_timedlock(&static_mtx, &deadline);
    if (rc == 0) {
        printf("Acquired mutex with timedlock\n");
        pthread_mutex_unlock(&static_mtx);
    } else if (rc == ETIMEDOUT) {
        printf("Timedlock timed out\n");
    } else {
        fprintf(stderr, "timedlock error: %s\n", strerror(rc));
    }
#else
    /* macOS: pthread_mutex_timedlock not available; trylock instead */
    int rc = pthread_mutex_trylock(&static_mtx);
    if (rc == 0) {
        printf("Acquired mutex with trylock (macOS fallback; no timedlock)\n");
        pthread_mutex_unlock(&static_mtx);
    } else {
        printf("Trylock failed: %s (would be timedout on Linux)\n", strerror(rc));
    }
#endif

    pthread_mutex_destroy(&dynamic_mtx);
    pthread_mutex_destroy(&static_mtx);

    /* Mutex error checking: PTHREAD_MUTEX_ERRORCHECK */
    pthread_mutex_t errcheck_mtx;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&errcheck_mtx, &attr);
    pthread_mutexattr_destroy(&attr);

    pthread_mutex_lock(&errcheck_mtx);
    int err = pthread_mutex_lock(&errcheck_mtx);  /* Double-lock: error! */
    if (err == EDEADLK) {
        printf("ERRORCHECK detected self-deadlock!\n");
    }
    pthread_mutex_unlock(&errcheck_mtx);
    pthread_mutex_destroy(&errcheck_mtx);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 2: Recursive Mutex (re-entrant)                               */
/* ──────────────────────────────────────────────────────────────────── */

pthread_mutex_t rec_mtx;
int rec_counter = 0;

void recursive_function(int depth) {
    pthread_mutex_lock(&rec_mtx);  /* RECURSIVE: re-entry is allowed */
    rec_counter++;
    printf("  depth=%d counter=%d\n", depth, rec_counter);

    if (depth > 0)
        recursive_function(depth - 1);

    pthread_mutex_unlock(&rec_mtx);  /* Must unlock same number of times! */
}

void demo_recursive_mutex(void) {
    printf("\n=== Demo 2: Recursive Mutex ===\n");

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&rec_mtx, &attr);
    pthread_mutexattr_destroy(&attr);

    printf("Recursive lock at depth 3:\n");
    recursive_function(3);
    printf("Final counter: %d\n", rec_counter);

    pthread_mutex_destroy(&rec_mtx);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 3: Read-Write Lock                                            */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    double value;
    pthread_rwlock_t lock;
    int reads, writes;
} SharedFloat;

SharedFloat g_data = { .value = 3.14159 };

void *reader_thread(void *arg) {
    SharedFloat *d = arg;
    for (int i = 0; i < 5; i++) {
        /* Multiple readers can hold read lock simultaneously */
        pthread_rwlock_rdlock(&d->lock);

        int r = __atomic_add_fetch(&d->reads, 1, __ATOMIC_RELAXED);
        printf("  [Reader %lx] value=%.5f (concurrent_readers=%d)\n",
               (unsigned long)(uintptr_t)pthread_self() & 0xFFFF, d->value, r);
        usleep(50000);
        __atomic_sub_fetch(&d->reads, 1, __ATOMIC_RELAXED);

        pthread_rwlock_unlock(&d->lock);
        usleep(20000);
    }
    return NULL;
}

void *writer_thread(void *arg) {
    SharedFloat *d = arg;
    for (int i = 0; i < 3; i++) {
        usleep(80000);

        /* Writer gets exclusive access - blocks while any reader holds lock */
        pthread_rwlock_wrlock(&d->lock);

        d->writes++;
        double old = d->value;
        d->value *= 2.0;
        printf("  [Writer] write #%d: %.5f → %.5f\n", d->writes, old, d->value);
        usleep(30000);  /* Hold write lock briefly */

        pthread_rwlock_unlock(&d->lock);
    }
    return NULL;
}

void demo_rwlock(void) {
    printf("\n=== Demo 3: Read-Write Lock ===\n");
    printf("Multiple readers run concurrently; writer gets exclusive access.\n\n");

    /* Static initialization */
    pthread_rwlock_init(&g_data.lock, NULL);
    /* Or: static pthread_rwlock_t rwl = PTHREAD_RWLOCK_INITIALIZER; */

    /* Prefer writer? Set PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP */
    pthread_rwlockattr_t rwattr;
    pthread_rwlockattr_init(&rwattr);
    /* pthread_rwlockattr_setkind_np(&rwattr,
           PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP); */
    pthread_rwlockattr_destroy(&rwattr);

    pthread_t readers[4], writer;
    for (int i = 0; i < 4; i++)
        pthread_create(&readers[i], NULL, reader_thread, &g_data);
    pthread_create(&writer, NULL, writer_thread, &g_data);

    for (int i = 0; i < 4; i++) pthread_join(readers[i], NULL);
    pthread_join(writer, NULL);

    printf("\nFinal value: %.5f (after %d writes)\n", g_data.value, g_data.writes);
    pthread_rwlock_destroy(&g_data.lock);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 4: Condition Variables - signal vs broadcast                  */
/* ──────────────────────────────────────────────────────────────────── */

/*
 * ALWAYS use pattern:
 *   lock
 *   while (!condition) wait(cond, lock)  // LOOP not IF - spurious wakeups!
 *   do work
 *   unlock
 */

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    int             data;
    int             data_ready;
} SharedQueue;

void *condvar_producer(void *arg) {
    SharedQueue *q = arg;
    for (int i = 1; i <= 5; i++) {
        usleep(100000);

        pthread_mutex_lock(&q->lock);
        q->data = i * 10;
        q->data_ready = 1;
        printf("[Producer] Produced data=%d, signaling ONE consumer\n", q->data);
        pthread_cond_signal(&q->cond);    /* Wake ONE waiting consumer */
        pthread_mutex_unlock(&q->lock);
    }
    return NULL;
}

void *condvar_consumer(void *arg) {
    SharedQueue *q = arg;
    int id = *((int *)((void **)arg)[1]);
    /* Actually using a simpler approach: */

    pthread_mutex_lock(&q->lock);
    /* MUST use while loop - not if - to guard against spurious wakeups */
    while (!q->data_ready) {
        printf("[Consumer] Waiting on condition...\n");
        pthread_cond_wait(&q->cond, &q->lock);
        /* pthread_cond_wait atomically:
           1. Releases lock
           2. Sleeps on condition
           3. On signal/wakeup: reacquires lock
           4. Returns */
    }
    int val = q->data;
    q->data_ready = 0;
    printf("[Consumer %d] Consumed data=%d\n", id, val);
    pthread_mutex_unlock(&q->lock);
    return NULL;
}

/* Broadcast example: all threads wake up when condition satisfied */
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t  go;
    int             start_flag;
    int             n_ready;
} StartGun;

void *race_runner(void *arg) {
    StartGun *sg = arg;

    pthread_mutex_lock(&sg->lock);
    sg->n_ready++;
    printf("  Runner %lx ready (total ready: %d)\n",
           (unsigned long)(uintptr_t)pthread_self() & 0xFFFF, sg->n_ready);
    /* Wait for start gun */
    while (!sg->start_flag)
        pthread_cond_wait(&sg->go, &sg->lock);
    pthread_mutex_unlock(&sg->lock);

    printf("  Runner %lx RUNNING!\n",
           (unsigned long)(uintptr_t)pthread_self() & 0xFFFF);
    return NULL;
}

void demo_condvar(void) {
    printf("\n=== Demo 4: Condition Variables ===\n");

    /* ── signal: wake ONE waiter ── */
    printf("\n--- pthread_cond_signal (wake ONE) ---\n");
    SharedQueue q = {
        .lock = PTHREAD_MUTEX_INITIALIZER,
        .cond = PTHREAD_COND_INITIALIZER,
        .data = 0, .data_ready = 0
    };
    pthread_t prod, cons;
    int cons_id = 1;
    pthread_create(&cons, NULL, condvar_consumer, &q);
    pthread_create(&prod, NULL, condvar_producer, &q);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    pthread_mutex_destroy(&q.lock);
    pthread_cond_destroy(&q.cond);

    /* ── broadcast: wake ALL waiters ── */
    printf("\n--- pthread_cond_broadcast (wake ALL) ---\n");
    StartGun sg = {
        .lock = PTHREAD_MUTEX_INITIALIZER,
        .go   = PTHREAD_COND_INITIALIZER,
        .start_flag = 0, .n_ready = 0
    };
    const int N_RUNNERS = 5;
    pthread_t runners[N_RUNNERS];

    for (int i = 0; i < N_RUNNERS; i++)
        pthread_create(&runners[i], NULL, race_runner, &sg);

    /* Wait for all runners to be ready */
    while (1) {
        pthread_mutex_lock(&sg.lock);
        int ready = sg.n_ready;
        pthread_mutex_unlock(&sg.lock);
        if (ready == N_RUNNERS) break;
        usleep(10000);
    }

    printf("[Starter] All %d runners ready. Firing start gun (broadcast)!\n",
           N_RUNNERS);
    pthread_mutex_lock(&sg.lock);
    sg.start_flag = 1;
    pthread_cond_broadcast(&sg.go);  /* Wake ALL waiting threads */
    pthread_mutex_unlock(&sg.lock);

    for (int i = 0; i < N_RUNNERS; i++)
        pthread_join(runners[i], NULL);

    pthread_mutex_destroy(&sg.lock);
    pthread_cond_destroy(&sg.go);

    printf("\nKey Rules for Condition Variables:\n");
    printf("  1. Always protect condition check with mutex\n");
    printf("  2. Always use WHILE loop (not if) to re-check condition\n");
    printf("  3. Signal while holding mutex, or after releasing it\n");
    printf("  4. signal = wake ONE, broadcast = wake ALL\n");
    printf("  5. Spurious wakeups ARE possible (hence the while loop)\n");
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 5: pthread_barrier (Linux/glibc; not available on macOS)      */
/* ──────────────────────────────────────────────────────────────────── */

#ifdef __linux__
static pthread_barrier_t g_barrier;
static atomic_int phase_results[4] = {0, 0, 0, 0};

void *barrier_worker(void *arg) {
    int id = *(int *)arg;

    for (int phase = 0; phase < 4; phase++) {
        /* Each thread does work for this phase */
        usleep((id + 1) * 20000 + phase * 10000);  /* Variable duration */
        atomic_fetch_add(&phase_results[phase], 1);
        printf("  [Worker %d] Completed phase %d\n", id, phase);

        /* Wait for ALL threads to finish this phase */
        int rc = pthread_barrier_wait(&g_barrier);

        /* One thread (the one returning PTHREAD_BARRIER_SERIAL_THREAD)
           can do post-phase aggregation */
        if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
            printf("[Coordinator] Phase %d complete. %d workers done.\n",
                   phase, atomic_load(&phase_results[phase]));
        }

        /* No worker starts phase N+1 until all finish phase N */
    }
    return NULL;
}

void demo_barrier(void) {
    printf("\n=== Demo 5: Barrier ===\n");
    const int N = 4;

    pthread_barrier_init(&g_barrier, NULL, N);

    pthread_t workers[N];
    int ids[N];
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&workers[i], NULL, barrier_worker, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(workers[i], NULL);

    pthread_barrier_destroy(&g_barrier);
    printf("All phases complete.\n");
}
#else
void demo_barrier(void) {
    printf("\n=== Demo 5: Barrier ===\n");
    printf("pthread_barrier_t is Linux/glibc-specific.\n");
    printf("On macOS, implement barriers with mutex + condvar + counter.\n");
    printf("Reference: pthreads_lifecycle.c demo_attributes()\n");
}
#endif

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 6: pthread_once - guaranteed one-time initialization         */
/* ──────────────────────────────────────────────────────────────────── */

static pthread_once_t init_once = PTHREAD_ONCE_INIT;
static int global_resource = 0;

void initialize_global_resource(void) {
    printf("[once] Initializing global resource (called exactly once!)\n");
    global_resource = 42;
    /* Could: open DB connection, load config file, etc. */
}

void *once_worker(void *arg) {
    int id = *(int *)arg;
    /* Even if 100 threads call this, init runs exactly once */
    pthread_once(&init_once, initialize_global_resource);
    printf("[Worker %d] Sees global_resource=%d\n", id, global_resource);
    return NULL;
}

void demo_once(void) {
    printf("\n=== Demo 6: pthread_once ===\n");
    const int N = 6;
    pthread_t tids[N];
    int ids[N];
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, once_worker, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(tids[i], NULL);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Main                                                               */
/* ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    int demo = (argc > 1) ? atoi(argv[1]) : 0;
    printf("Mutex & Condition Variable Demos\n\n");

    switch (demo) {
        case 1: demo_basic_mutex(); break;
        case 2: demo_recursive_mutex(); break;
        case 3: demo_rwlock(); break;
        case 4: demo_condvar(); break;
        case 5: demo_barrier(); break;
        case 6: demo_once(); break;
        default:
            demo_basic_mutex();
            demo_recursive_mutex();
            demo_rwlock();
            demo_condvar();
            demo_barrier();
            demo_once();
    }
    return 0;
}
