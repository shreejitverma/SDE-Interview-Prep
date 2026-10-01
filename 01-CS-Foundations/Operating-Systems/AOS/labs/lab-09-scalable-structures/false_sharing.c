#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <stdatomic.h>

#define NUM_THREADS 4
#define ITERATIONS 100000000

// We use _Atomic to ensure memory is accessed, preventing register accumulation.
// Relaxed memory order avoids full memory barriers so we just measure cache line contention.
_Atomic uint64_t unpadded_counters[NUM_THREADS];

typedef struct {
    _Atomic uint64_t count;
    uint8_t padding[56]; 
} __attribute__((aligned(64))) padded_counter_t;

padded_counter_t padded_counters[NUM_THREADS];

void* worker_false_sharing(void* arg) {
    long tid = (long)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add_explicit(&unpadded_counters[tid], 1, memory_order_relaxed);
    }
    return NULL;
}

void* worker_no_false_sharing(void* arg) {
    long tid = (long)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_fetch_add_explicit(&padded_counters[tid].count, 1, memory_order_relaxed);
    }
    return NULL;
}

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void run_test(const char* name, void* (*func)(void*)) {
    pthread_t threads[NUM_THREADS];
    double start = get_time();

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, func, (void*)i);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    double end = get_time();
    printf("%-20s: %.4f seconds\n", name, end - start);
}

int main() {
    printf("Running False Sharing Test (%d threads, %d iterations)...\n", NUM_THREADS, ITERATIONS);
    
    for(int i=0; i<NUM_THREADS; i++) {
        atomic_init(&unpadded_counters[i], 0);
        atomic_init(&padded_counters[i].count, 0);
    }

    run_test("Unpadded (False Sh.)", worker_false_sharing);
    run_test("Padded (No False S.)", worker_no_false_sharing);

    return 0;
}
