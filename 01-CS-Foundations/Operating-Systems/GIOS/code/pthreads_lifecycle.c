/*
 * pthreads_lifecycle.c
 * Complete PThreads API reference: thread creation, attributes, joining,
 * detaching, cancellation, thread-local storage (TLS), cleanup handlers,
 * and thread pools.
 *
 * Compile (Linux): gcc -Wall -O2 -o pthreads_lifecycle pthreads_lifecycle.c -lpthread
 * Compile (macOS): gcc -Wall -O2 -o pthreads_lifecycle pthreads_lifecycle.c -lpthread
 * Run:     ./pthreads_lifecycle [demo_number]
 *          demos: 1=basic 2=attributes 3=cancel 4=tls 5=cleanup 6=pool
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sched.h>   /* sched_getcpu() */
#include <stdatomic.h>

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 1: Basic thread lifecycle - create, join, detach              */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    int    id;
    double input;
    double result;
} WorkItem;

void *basic_worker(void *arg) {
    WorkItem *w = arg;
#ifdef __linux__
    printf("[Thread %d] Started on CPU %d, computing sqrt(%.2f)\n",
           w->id, sched_getcpu(), w->input);
#else
    printf("[Thread %d] Started, computing sqrt(%.2f)\n",
           w->id, w->input);
#endif

    /* Simulate work */
    usleep(w->id * 50000);

    w->result = w->input * w->input;  /* return: square, not sqrt (no math.h) */
    printf("[Thread %d] Done. result=%.2f\n", w->id, w->result);

    /* Thread exits by returning from its function */
    return w;  /* Returned pointer retrievable via pthread_join */
}

void demo_basic_lifecycle(void) {
    printf("\n=== Demo 1: Basic Thread Lifecycle ===\n");
    const int N = 5;
    pthread_t tids[N];
    WorkItem items[N];

    /* ── Create threads ── */
    for (int i = 0; i < N; i++) {
        items[i] = (WorkItem){ .id = i, .input = (i + 1) * 10.0 };
        int rc = pthread_create(
            &tids[i],          /* Thread ID output */
            NULL,              /* Default attributes */
            basic_worker,      /* Start function */
            &items[i]          /* Argument */
        );
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            exit(1);
        }
        printf("[Main] Created thread %d (tid=%lu)\n", i, (unsigned long)tids[i]);
    }

    /* ── Join threads (wait for completion, get return value) ── */
    for (int i = 0; i < N; i++) {
        void *retval;
        int rc = pthread_join(tids[i], &retval);
        if (rc != 0) {
            fprintf(stderr, "pthread_join failed: %s\n", strerror(rc));
        } else {
            WorkItem *w = retval;
            printf("[Main] Thread %d joined. result=%.2f\n", i, w->result);
        }
    }
    printf("[Main] All threads completed.\n");
}

/* ── Detached thread: fire-and-forget ── */
void *detached_worker(void *arg) {
    int id = *(int *)arg;
    free(arg);  /* Detached threads must free their own args */

    printf("[Detached Thread %d] Running\n", id);
    usleep(100000);
    printf("[Detached Thread %d] Exiting (main may have already gone!)\n", id);
    return NULL;
}

void demo_detached(void) {
    printf("\n--- Detached Threads ---\n");
    for (int i = 0; i < 3; i++) {
        pthread_t tid;
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        int *id = malloc(sizeof(int));
        *id = i;
        pthread_create(&tid, &attr, detached_worker, id);
        pthread_attr_destroy(&attr);
        /* Can NOT join detached threads - they clean themselves up */
    }
    usleep(200000);  /* Give detached threads time to run */
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 2: Thread Attributes                                          */
/* ──────────────────────────────────────────────────────────────────── */

void demo_thread_attributes(void) {
    printf("\n=== Demo 2: Thread Attributes ===\n");

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    /* ── Stack size ── */
    size_t stack_size = 2 * 1024 * 1024;  /* 2 MB */
    pthread_attr_setstacksize(&attr, stack_size);

    size_t actual_size;
    pthread_attr_getstacksize(&attr, &actual_size);
    printf("Stack size set to: %zu bytes (%.1f MB)\n",
           actual_size, actual_size / (1024.0 * 1024.0));

    /* ── Stack address (custom stack allocation) ── */
    void *stack_mem = malloc(stack_size);
    /* pthread_attr_setstack(&attr, stack_mem, stack_size); */
    /* Note: user-allocated stacks require manual management */

    /* ── Scheduling policy ── */
    struct sched_param sp;
    sp.sched_priority = 10;

    /* Note: SCHED_FIFO/RR require root or CAP_SYS_NICE */
    /* pthread_attr_setschedpolicy(&attr, SCHED_FIFO); */
    /* pthread_attr_setschedparam(&attr, &sp); */
    /* pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED); */

    /* ── Guard size (protects against stack overflow) ── */
    size_t guard = 4096;
    pthread_attr_setguardsize(&attr, guard);

    size_t actual_guard;
    pthread_attr_getguardsize(&attr, &actual_guard);
    printf("Guard page size: %zu bytes\n", actual_guard);

    /* ── Detach state ── */
    int detach;
    pthread_attr_getdetachstate(&attr, &detach);
    printf("Detach state: %s\n",
           detach == PTHREAD_CREATE_JOINABLE ? "JOINABLE" : "DETACHED");

    /* ── Scope ── */
    int scope;
    pthread_attr_getscope(&attr, &scope);
    printf("Contention scope: %s\n",
           scope == PTHREAD_SCOPE_SYSTEM ? "SYSTEM (KLT)" : "PROCESS (ULT)");

    /* Create thread with these attributes */
    pthread_t tid;
    int id = 99;
    pthread_create(&tid, &attr, basic_worker,
                   &(WorkItem){ .id = id, .input = 42.0 });
    pthread_join(tid, NULL);

    pthread_attr_destroy(&attr);
    free(stack_mem);

    /* ── CPU Affinity (Linux only) ── */
    printf("\n--- CPU Affinity ---\n");
#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);  /* Pin to CPU 0 */

    if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0) {
        printf("Pinned main thread to CPU 0\n");
        printf("Now running on CPU: %d\n", sched_getcpu());
    } else {
        perror("pthread_setaffinity_np");
    }

    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    printf("Allowed CPUs: ");
    long nproc = sysconf(_SC_NPROCESSORS_ONLN);
    for (int cpu = 0; cpu < nproc; cpu++) {
        if (CPU_ISSET(cpu, &cpuset)) printf("%d ", cpu);
    }
    printf("\n");
#else
    printf("CPU affinity APIs are Linux-specific (pthread_setaffinity_np).\n");
    printf("On macOS use: pthread_mach_thread_np() + thread_policy_set().\n");
#endif
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 3: Thread Cancellation                                        */
/* ──────────────────────────────────────────────────────────────────── */

volatile int g_cancel_demo_running = 0;

void *cancellable_worker(void *arg) {
    (void)arg;
    /* Set cancellation type:
     * PTHREAD_CANCEL_DEFERRED    - cancel only at cancellation points
     * PTHREAD_CANCEL_ASYNCHRONOUS - cancel at any instruction (dangerous!) */
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    /* Set cancellation state:
     * PTHREAD_CANCEL_ENABLE  - accept cancellation (default)
     * PTHREAD_CANCEL_DISABLE - ignore cancellation requests */
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    printf("[Worker] Running. Will check for cancellation...\n");

    for (int i = 0; i < 100; i++) {
        printf("[Worker] Step %d\n", i);
        usleep(100000);

        /* Explicit cancellation point: where thread can be cancelled */
        pthread_testcancel();

        /* Many library functions are implicit cancellation points:
           read(), write(), sleep(), pthread_cond_wait(), recv(), etc. */
    }

    printf("[Worker] Finished normally\n");
    return NULL;
}

void demo_cancellation(void) {
    printf("\n=== Demo 3: Thread Cancellation ===\n");

    pthread_t tid;
    pthread_create(&tid, NULL, cancellable_worker, NULL);

    /* Let it run for a bit, then cancel it */
    usleep(350000);
    printf("[Main] Sending cancellation request...\n");
    pthread_cancel(tid);

    void *retval;
    pthread_join(tid, &retval);

    /* Check how thread ended */
    if (retval == PTHREAD_CANCELED) {
        printf("[Main] Thread was cancelled (returned PTHREAD_CANCELED)\n");
    } else {
        printf("[Main] Thread finished normally\n");
    }
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 4: Thread-Local Storage (TLS)                                 */
/* ──────────────────────────────────────────────────────────────────── */

/* GCC/Clang __thread keyword - per-thread global variable */
__thread int tls_counter = 0;
__thread char tls_name[64];

/* POSIX pthread_key_t - dynamic TLS with destructor */
static pthread_key_t error_key;
static pthread_once_t key_once = PTHREAD_ONCE_INIT;

static void tls_destructor(void *val) {
    printf("[TLS] Destructor called for value: %s\n", (char *)val);
    free(val);
}

static void create_tls_key(void) {
    pthread_key_create(&error_key, tls_destructor);
}

void *tls_worker(void *arg) {
    int id = *(int *)arg;

    /* GCC __thread: per-thread, initialized per thread */
    tls_counter = id * 100;
    snprintf(tls_name, sizeof(tls_name), "thread-%d", id);

    /* POSIX TLS: dynamic key with destructor */
    pthread_once(&key_once, create_tls_key);
    char *error_msg = malloc(64);
    snprintf(error_msg, 64, "Error context for thread %d", id);
    pthread_setspecific(error_key, error_msg);  /* Set this thread's value */

    usleep(id * 20000);

    /* Each thread sees its own copy */
    char *my_error = pthread_getspecific(error_key);
    printf("[Thread %d] tls_counter=%d tls_name='%s' error='%s'\n",
           id, tls_counter, tls_name, my_error);

    /* Note: tls_destructor will be called automatically when thread exits */
    return NULL;
}

void demo_tls(void) {
    printf("\n=== Demo 4: Thread-Local Storage (TLS) ===\n");
    const int N = 4;
    pthread_t tids[N];
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, tls_worker, &ids[i]);
    }
    for (int i = 0; i < N; i++)
        pthread_join(tids[i], NULL);

    pthread_key_delete(error_key);

    printf("\nNote: Each thread has its own tls_counter, tls_name, and error_key.\n");
    printf("Use cases: errno (per-thread), database connections, random state.\n");
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 5: Cleanup Handlers (resource safety on cancellation/exit)   */
/* ──────────────────────────────────────────────────────────────────── */

void cleanup_mutex(void *arg) {
    pthread_mutex_t *mtx = arg;
    printf("[Cleanup] Releasing mutex\n");
    pthread_mutex_unlock(mtx);
}

void cleanup_file(void *arg) {
    FILE **fp = arg;
    if (*fp) {
        printf("[Cleanup] Closing file\n");
        fclose(*fp);
        *fp = NULL;
    }
}

void cleanup_memory(void *arg) {
    char **ptr = arg;
    if (*ptr) {
        printf("[Cleanup] Freeing buffer: '%s'\n", *ptr);
        free(*ptr);
        *ptr = NULL;
    }
}

static pthread_mutex_t cleanup_mtx = PTHREAD_MUTEX_INITIALIZER;

void *cleanup_worker(void *arg) {
    (void)arg;
    FILE *fp = NULL;
    char *buf = malloc(128);
    snprintf(buf, 128, "important data");

    pthread_mutex_lock(&cleanup_mtx);

    /* Register cleanup handlers (LIFO order: last pushed = first called) */
    pthread_cleanup_push(cleanup_memory, &buf);
    pthread_cleanup_push(cleanup_file, &fp);
    pthread_cleanup_push(cleanup_mutex, &cleanup_mtx);

    printf("[Worker] Acquired mutex and allocated resources\n");

    /* Open a file */
    fp = fopen("/tmp/cleanup_test.txt", "w");
    if (fp) {
        fprintf(fp, "Test data\n");
        printf("[Worker] File opened\n");
    }

    /* Simulate work that gets cancelled */
    sleep(1);  /* Cancellation point - will be hit */

    /* If not cancelled, pop handlers without calling them */
    pthread_cleanup_pop(0);  /* mutex  - 0 = don't call */
    pthread_cleanup_pop(0);  /* file   - 0 = don't call */
    pthread_cleanup_pop(0);  /* buffer - 0 = don't call */

    /* Manually clean up */
    if (fp) fclose(fp);
    free(buf);
    pthread_mutex_unlock(&cleanup_mtx);

    return NULL;
}

void demo_cleanup(void) {
    printf("\n=== Demo 5: Cleanup Handlers ===\n");
    printf("Cleanup handlers ensure resources are freed even on cancellation.\n\n");

    pthread_t tid;
    pthread_create(&tid, NULL, cleanup_worker, NULL);

    usleep(200000);
    printf("[Main] Cancelling worker...\n");
    pthread_cancel(tid);

    void *retval;
    pthread_join(tid, &retval);
    printf("[Main] Thread ended. All cleanup handlers called in LIFO order.\n");

    unlink("/tmp/cleanup_test.txt");
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Demo 6: Thread Pool                                                */
/* ──────────────────────────────────────────────────────────────────── */

#define POOL_SIZE   4
#define QUEUE_SIZE  64

typedef struct {
    void  (*func)(void *);
    void   *arg;
} Task;

typedef struct {
    pthread_t        workers[POOL_SIZE];
    Task             queue[QUEUE_SIZE];
    int              head, tail, count;
    pthread_mutex_t  lock;
    pthread_cond_t   not_empty;
    pthread_cond_t   not_full;
    int              shutdown;
    atomic_long      tasks_completed;
} ThreadPool;

static ThreadPool g_pool;

void *pool_worker(void *arg) {
    ThreadPool *p = arg;
    printf("[Pool Worker %lu] Started\n", (unsigned long)pthread_self());

    while (1) {
        pthread_mutex_lock(&p->lock);

        /* Wait for work or shutdown signal */
        while (p->count == 0 && !p->shutdown)
            pthread_cond_wait(&p->not_empty, &p->lock);

        if (p->shutdown && p->count == 0) {
            pthread_mutex_unlock(&p->lock);
            break;
        }

        /* Dequeue task */
        Task t = p->queue[p->head];
        p->head = (p->head + 1) % QUEUE_SIZE;
        p->count--;
        pthread_cond_signal(&p->not_full);
        pthread_mutex_unlock(&p->lock);

        /* Execute task outside lock */
        t.func(t.arg);
        atomic_fetch_add(&p->tasks_completed, 1);
    }
    printf("[Pool Worker %lu] Exiting\n", (unsigned long)pthread_self());
    return NULL;
}

void pool_init(ThreadPool *p) {
    p->head = p->tail = p->count = p->shutdown = 0;
    atomic_init(&p->tasks_completed, 0);
    pthread_mutex_init(&p->lock, NULL);
    pthread_cond_init(&p->not_empty, NULL);
    pthread_cond_init(&p->not_full, NULL);

    for (int i = 0; i < POOL_SIZE; i++)
        pthread_create(&p->workers[i], NULL, pool_worker, p);
}

void pool_submit(ThreadPool *p, void (*func)(void *), void *arg) {
    pthread_mutex_lock(&p->lock);
    while (p->count == QUEUE_SIZE)  /* Wait if queue full */
        pthread_cond_wait(&p->not_full, &p->lock);

    p->queue[p->tail] = (Task){ func, arg };
    p->tail = (p->tail + 1) % QUEUE_SIZE;
    p->count++;
    pthread_cond_signal(&p->not_empty);
    pthread_mutex_unlock(&p->lock);
}

void pool_shutdown(ThreadPool *p) {
    pthread_mutex_lock(&p->lock);
    p->shutdown = 1;
    pthread_cond_broadcast(&p->not_empty);  /* Wake all workers */
    pthread_mutex_unlock(&p->lock);

    for (int i = 0; i < POOL_SIZE; i++)
        pthread_join(p->workers[i], NULL);

    pthread_mutex_destroy(&p->lock);
    pthread_cond_destroy(&p->not_empty);
    pthread_cond_destroy(&p->not_full);
}

/* Task functions for the pool */
void task_compute(void *arg) {
    int n = *(int *)arg;
    volatile long result = 0;
    for (long i = 0; i < n * 100000L; i++) result += i;
    printf("  Task(n=%d) done, result=%ld\n", n, result % 1000);
}

void demo_thread_pool(void) {
    printf("\n=== Demo 6: Thread Pool ===\n");
    printf("Pool size: %d workers, Queue: %d tasks\n\n", POOL_SIZE, QUEUE_SIZE);

    pool_init(&g_pool);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    /* Submit 20 tasks */
    int task_args[20];
    for (int i = 0; i < 20; i++) {
        task_args[i] = i + 1;
        pool_submit(&g_pool, task_compute, &task_args[i]);
    }

    printf("[Main] All tasks submitted, waiting for completion...\n");

    /* Wait for all tasks to complete */
    while (atomic_load(&g_pool.tasks_completed) < 20)
        usleep(1000);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("[Main] All %ld tasks completed in %.3f seconds\n",
           atomic_load(&g_pool.tasks_completed), elapsed);

    pool_shutdown(&g_pool);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Main                                                               */
/* ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    printf("PThreads Lifecycle Demo\n");
#ifdef __linux__
    printf("Main thread: PID=%d TID=%lu CPU=%d\n",
           getpid(), (unsigned long)pthread_self(), sched_getcpu());
#else
    printf("Main thread: PID=%d TID=%lu\n",
           getpid(), (unsigned long)pthread_self());
#endif

    int demo = (argc > 1) ? atoi(argv[1]) : 0;
    switch (demo) {
        case 1: demo_basic_lifecycle(); break;
        case 2: demo_thread_attributes(); break;
        case 3: demo_cancellation(); break;
        case 4: demo_tls(); break;
        case 5: demo_cleanup(); break;
        case 6: demo_thread_pool(); break;
        default:
            demo_basic_lifecycle();
            demo_detached();
            demo_thread_attributes();
            demo_cancellation();
            demo_tls();
            demo_cleanup();
            demo_thread_pool();
    }
    return 0;
}
