#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#define ITERATIONS 10000000

int main() {
    struct timespec start, end;
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < ITERATIONS; i++) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    uint64_t nsec_start = (uint64_t)start.tv_sec * 1000000000ULL + start.tv_nsec;
    uint64_t nsec_end = (uint64_t)end.tv_sec * 1000000000ULL + end.tv_nsec;
    uint64_t diff = nsec_end - nsec_start;
    
    printf("--- clock_gettime(CLOCK_MONOTONIC) overhead ---\n");
    printf("Iterations: %d\n", ITERATIONS);
    printf("Total time: %lu ns\n", (unsigned long)diff);
    printf("Average overhead: %lu ns per call\n", (unsigned long)(diff / ITERATIONS));

    return 0;
}
