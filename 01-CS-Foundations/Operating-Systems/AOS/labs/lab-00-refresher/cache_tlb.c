#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define MIN_SIZE (1024)         // 1 KB
#define MAX_SIZE (64 * 1024 * 1024) // 64 MB

double get_time_ns(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec * 1e9 + (double)ts.tv_nsec;
}

int main(void) {
    void **buffer = mmap(NULL, MAX_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (buffer == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    printf("Size(KB)\tStride(B)\tTime(ns/access)\n");

    int strides[] = { 64, 4096 }; // 64 for Cache, 4096 for TLB
    int num_strides = sizeof(strides) / sizeof(strides[0]);

    for (int s = 0; s < num_strides; s++) {
        size_t stride_bytes = strides[s];
        size_t stride_elements = stride_bytes / sizeof(void*);
        
        for (size_t size_bytes = MIN_SIZE; size_bytes <= MAX_SIZE; size_bytes *= 2) {
            size_t size_elements = size_bytes / sizeof(void*);
            
            memset(buffer, 0, size_bytes);
            
            size_t last_idx = 0;
            for (size_t i = 0; i < size_elements; i += stride_elements) {
                size_t next_idx = (i + stride_elements) % size_elements;
                buffer[i] = &buffer[next_idx];
                last_idx = i;
            }
            // Close the loop
            buffer[last_idx] = &buffer[0];

            // Touch everything to fault in pages
            for (size_t i = 0; i < size_elements; i += 4096 / sizeof(void*)) {
                volatile void *tmp = buffer[i];
                (void)tmp;
            }

            size_t num_accesses = 10000000;
            if (size_bytes > 8 * 1024 * 1024) num_accesses = 5000000;

            void **p = &buffer[0];

            double start = get_time_ns();
            for (size_t i = 0; i < num_accesses; i++) {
                p = (void **)*p;
            }
            double end = get_time_ns();

            // Prevent compiler from optimizing away the loop
            if (p == NULL) printf("p is NULL\n");

            double ns_per_access = (end - start) / num_accesses;
            printf("%zu\t\t%zu\t\t%.2f\n", size_bytes / 1024, stride_bytes, ns_per_access);
        }
    }

    munmap(buffer, MAX_SIZE);
    return 0;
}
