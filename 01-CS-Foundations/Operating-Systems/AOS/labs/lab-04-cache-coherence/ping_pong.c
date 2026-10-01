#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define CACHE_LINE_SIZE 64
#define ITERATIONS 1000000

struct ping_pong {
    volatile int flag; // 0 = thread 0's turn, 1 = thread 1's turn
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct ping_pong pp;

void* thread0(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (pp.flag != 0) {
            __asm__ volatile("yield" ::: "memory");
        }
        pp.flag = 1;
    }
    return NULL;
}

void* thread1(void* arg) {
    (void)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (pp.flag != 1) {
            __asm__ volatile("yield" ::: "memory");
        }
        pp.flag = 0;
    }
    return NULL;
}

static inline uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main() {
    pthread_t t0, t1;
    uint64_t start, end;
    
    pp.flag = 0;
    
    printf("Starting ping-pong test (%d iterations)...\n", ITERATIONS);
    
    start = get_time_ns();
    pthread_create(&t0, NULL, thread0, NULL);
    pthread_create(&t1, NULL, thread1, NULL);
    
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    end = get_time_ns();
    
    uint64_t total_ns = end - start;
    uint64_t rtts = ITERATIONS;
    double ns_per_rtt = (double)total_ns / rtts;
    double ns_per_transfer = ns_per_rtt / 2.0;
    
    printf("Total time: %llu ms\n", (unsigned long long)(total_ns / 1000000));
    printf("One-way coherence transfer latency: %.2f ns\n", ns_per_transfer);
    printf("PASS: Ping-pong complete.\n");
    return 0;
}
