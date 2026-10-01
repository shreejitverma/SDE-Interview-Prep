#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <omp.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define NUM_ITERS 1000

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int cmp_u64(const void *a, const void *b) {
    uint64_t arg1 = *(const uint64_t *)a;
    uint64_t arg2 = *(const uint64_t *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

pthread_barrier_t barrier;
uint64_t thread_times[64][NUM_ITERS];
int num_threads;

void* pthread_worker(void* arg) {
    int tid = (int)(intptr_t)arg;
    for (int i = 0; i < NUM_ITERS; i++) {
        uint64_t t0 = get_time_ns();
        pthread_barrier_wait(&barrier);
        uint64_t t1 = get_time_ns();
        thread_times[tid][i] = t1 - t0;
    }
    return NULL;
}

int main(int argc, char **argv) {
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    } else {
        num_threads = 4;
    }
    if (num_threads > 64) num_threads = 64;

    printf("Measuring barriers with %d threads, %d iterations\n", num_threads, NUM_ITERS);

    // 1. Pthread Barrier
    pthread_barrier_init(&barrier, NULL, num_threads);
    pthread_t threads[64];
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, pthread_worker, (void*)(intptr_t)i);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_barrier_destroy(&barrier);

    uint64_t p_meds[64];
    uint64_t p_sum_meds = 0;
    for (int i = 0; i < num_threads; i++) {
        qsort(thread_times[i], NUM_ITERS, sizeof(uint64_t), cmp_u64);
        p_meds[i] = thread_times[i][NUM_ITERS / 2];
        p_sum_meds += p_meds[i];
    }
    printf("Pthread Barrier Median Latency: %llu ns\n", (unsigned long long)(p_sum_meds / num_threads));

    // 2. OpenMP Barrier
    memset(thread_times, 0, sizeof(thread_times));
    omp_set_num_threads(num_threads);
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        for (int i = 0; i < NUM_ITERS; i++) {
            uint64_t t0 = get_time_ns();
            #pragma omp barrier
            uint64_t t1 = get_time_ns();
            thread_times[tid][i] = t1 - t0;
        }
    }

    uint64_t omp_meds[64];
    uint64_t omp_sum_meds = 0;
    for (int i = 0; i < num_threads; i++) {
        qsort(thread_times[i], NUM_ITERS, sizeof(uint64_t), cmp_u64);
        omp_meds[i] = thread_times[i][NUM_ITERS / 2];
        omp_sum_meds += omp_meds[i];
    }
    printf("OpenMP Barrier Median Latency: %llu ns\n", (unsigned long long)(omp_sum_meds / num_threads));

    return 0;
}
