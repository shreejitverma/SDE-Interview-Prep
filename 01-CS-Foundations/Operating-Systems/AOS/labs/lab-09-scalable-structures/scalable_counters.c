#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <stdatomic.h>

#define NUM_THREADS 4
#define ITERATIONS 10000000

// 1. Locked Counter
uint64_t locked_counter = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// 2. Atomic Counter
_Atomic uint64_t atomic_counter = 0;

// 3. Per-CPU Counter (simulated per-thread)
// Pad to 64 bytes (typical cache line) to avoid false sharing
typedef struct {
    uint64_t count;
    uint8_t padding[56]; 
} __attribute__((aligned(64))) per_thread_counter_t;

per_thread_counter_t per_thread_counters[NUM_THREADS];

void* worker_locked(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&lock);
        locked_counter++;
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* worker_atomic(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add_explicit(&atomic_counter, 1, memory_order_relaxed);
    }
    return NULL;
}

void* worker_per_thread(void* arg) {
    long tid = (long)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        per_thread_counters[tid].count++;
    }
    return NULL;
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void run_test(const char* name, void* (*func)(void*), int use_tid) {
    pthread_t threads[NUM_THREADS];
    double start = get_time();

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, func, use_tid ? (void*)i : NULL);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time();
    printf("%-20s: %.4f seconds\n", name, end - start);
}

int main() {
    printf("Running Scalable Counters Test (%d threads, %d iterations)...\n", NUM_THREADS, ITERATIONS);

    // Initialize per-thread counters
    for (int i = 0; i < NUM_THREADS; i++) {
        per_thread_counters[i].count = 0;
    }

    run_test("Locked Counter", worker_locked, 0);
    run_test("Atomic Counter", worker_atomic, 0);
    run_test("Per-Thread Counter", worker_per_thread, 1);

    uint64_t total_per_thread = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        total_per_thread += per_thread_counters[i].count;
    }

    printf("\nFinal counts (should be %d):\n", NUM_THREADS * ITERATIONS);
    printf("Locked : %lu\n", locked_counter);
    printf("Atomic : %lu\n", atomic_counter);
    printf("Per-Thr: %lu\n", total_per_thread);

    return 0;
}
