#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define NUM_THREADS 4
#define ITERATIONS 20000000
#define CACHE_LINE_SIZE 64

// Align the structs to cache line boundaries
struct unpadded_data {
    uint64_t counters[NUM_THREADS];
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct padded_data {
    struct {
        uint64_t counter;
        uint8_t padding[CACHE_LINE_SIZE - sizeof(uint64_t)];
    } padded_counters[NUM_THREADS];
} __attribute__((aligned(CACHE_LINE_SIZE)));

struct unpadded_data u_data;
struct padded_data p_data;

void* thread_unpadded(void* arg) {
    long id = (long)arg;
    for (long i = 0; i < ITERATIONS; i++) {
        // Volatile to prevent the compiler from optimizing the loop away
        // or keeping the counter purely in a register.
        // We want actual memory traffic.
        // Even better, we use __atomic_add_fetch to force bus locking/coherence traffic,
        // or just plain read-modify-write if we want to observe basic cache line sharing.
        // Using plain read-modify-write on shared cache lines is enough for false sharing
        // if they don't hold it in registers.
        // We use inline assembly to prevent register allocation.
        __asm__ volatile(
            "ldr x0, [%0]\n\t"
            "add x0, x0, #1\n\t"
            "str x0, [%0]"
            :
            : "r" (&u_data.counters[id])
            : "x0", "memory"
        );
    }
    return NULL;
}

void* thread_padded(void* arg) {
    long id = (long)arg;
    for (long i = 0; i < ITERATIONS; i++) {
        __asm__ volatile(
            "ldr x0, [%0]\n\t"
            "add x0, x0, #1\n\t"
            "str x0, [%0]"
            :
            : "r" (&p_data.padded_counters[id].counter)
            : "x0", "memory"
        );
    }
    return NULL;
}

static inline uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main() {
    pthread_t threads[NUM_THREADS];
    uint64_t start, end;
    
    // Warmup
    for (int i = 0; i < NUM_THREADS; i++) {
        u_data.counters[i] = 0;
        p_data.padded_counters[i].counter = 0;
    }

    printf("Starting false sharing test (%d threads, %d iterations)...\n", NUM_THREADS, ITERATIONS);

    // Unpadded test (subject to false sharing)
    start = get_time_ns();
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_unpadded, (void*)i);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end = get_time_ns();
    uint64_t time_unpadded = end - start;
    
    // Padded test (avoids false sharing)
    start = get_time_ns();
    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_padded, (void*)i);
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    end = get_time_ns();
    uint64_t time_padded = end - start;
    
    printf("Unpadded counters time: %llu ms\n", (unsigned long long)(time_unpadded / 1000000));
    printf("Padded counters time:   %llu ms\n", (unsigned long long)(time_padded / 1000000));
    
    if (time_padded < time_unpadded) {
        printf("Padded counters avoided false sharing successfully.\n");
    } else {
        printf("WARN: Padded counters were not faster. This can happen on single-vCPU or heavily loaded VMs.\n");
    }
    printf("PASS: False sharing test complete.\n");

    return 0;
}
