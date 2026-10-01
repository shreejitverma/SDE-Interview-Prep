#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdint.h>

#define ITERS 10000000

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int main() {
    uint64_t start, end;
    
    // Warmup
    for (int i = 0; i < 1000; i++) {
        syscall(SYS_getpid);
    }
    
    start = get_time_ns();
    for (int i = 0; i < ITERS; i++) {
        syscall(SYS_getpid);
    }
    end = get_time_ns();
    
    uint64_t sys_elapsed = end - start;
    
    // Warmup getpid (glibc)
    for (int i = 0; i < 1000; i++) {
        getpid();
    }
    
    start = get_time_ns();
    for (int i = 0; i < ITERS; i++) {
        getpid();
    }
    end = get_time_ns();
    
    uint64_t libc_elapsed = end - start;
    
    printf("Syscall (SYS_getpid) cost: %lu ns/call\n", sys_elapsed / ITERS);
    printf("Syscall (libc getpid) cost: %lu ns/call\n", libc_elapsed / ITERS);
    
    return 0;
}
