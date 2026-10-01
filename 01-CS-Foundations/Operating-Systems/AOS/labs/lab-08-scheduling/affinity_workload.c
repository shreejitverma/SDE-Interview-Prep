#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <string.h>

#define ARRAY_SIZE (1 * 1024 * 1024) // 4 MB per thread, fits in L3 but not L1/L2
#define ITERATIONS 50

int g_iterations = ITERATIONS;

double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void* worker_thread(void* arg) {
    int id = (int)(intptr_t)arg;
    uint32_t *data = malloc(ARRAY_SIZE * sizeof(uint32_t));
    if (!data) return NULL;
    
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        data[i] = i;
    }

    double start = get_time();
    for (int iter = 0; iter < g_iterations; iter++) {
        for (size_t i = 0; i < ARRAY_SIZE; i++) {
            data[i] = (data[i] * 31 + id) ^ i;
        }
    }
    double end = get_time();
    
    // Prevent optimization
    volatile uint32_t dummy = data[ARRAY_SIZE / 2];
    (void)dummy;
    
    printf("Thread %d finished in %.4f seconds\n", id, end - start);
    free(data);
    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = 4;
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    }
    if (argc > 2) {
        g_iterations = atoi(argv[2]);
    }
    
    pthread_t *threads = malloc(num_threads * sizeof(pthread_t));
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker_thread, (void*)(intptr_t)i);
    }

    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    return 0;
}
