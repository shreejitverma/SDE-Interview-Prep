#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdint.h>
#include <sys/wait.h>

#define ITERS 100000

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int p1[2];
int p2[2];

void *thread_func(void *arg) {
    (void)arg;
    char c = 'a';
    for (int i = 0; i < ITERS; i++) {
        if (read(p1[0], &c, 1) != 1) break;
        if (write(p2[1], &c, 1) != 1) break;
    }
    return NULL;
}

void measure_thread_switch() {
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        perror("pipe");
        exit(1);
    }
    pthread_t t;
    pthread_create(&t, NULL, thread_func, NULL);
    
    char c = 'a';
    uint64_t start = get_time_ns();
    for (int i = 0; i < ITERS; i++) {
        if (write(p1[1], &c, 1) != 1) break;
        if (read(p2[0], &c, 1) != 1) break;
    }
    uint64_t end = get_time_ns();
    pthread_join(t, NULL);
    
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    
    uint64_t elapsed_ns = end - start;
    printf("Thread context switch (same-address-space): %lu ns/switch\n", elapsed_ns / (2 * ITERS));
}

void measure_process_switch() {
    if (pipe(p1) < 0 || pipe(p2) < 0) {
        perror("pipe");
        exit(1);
    }
    
    fflush(stdout); // Prevent child from duplicating buffered output
    pid_t pid = fork();
    if (pid == 0) {
        char c = 'a';
        for (int i = 0; i < ITERS; i++) {
            if (read(p1[0], &c, 1) != 1) break;
            if (write(p2[1], &c, 1) != 1) break;
        }
        exit(0);
    } else if (pid > 0) {
        char c = 'a';
        uint64_t start = get_time_ns();
        for (int i = 0; i < ITERS; i++) {
            if (write(p1[1], &c, 1) != 1) break;
            if (read(p2[0], &c, 1) != 1) break;
        }
        uint64_t end = get_time_ns();
        waitpid(pid, NULL, 0);
        
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        
        uint64_t elapsed_ns = end - start;
        printf("Process context switch (cross-address-space): %lu ns/switch\n", elapsed_ns / (2 * ITERS));
    } else {
        perror("fork");
        exit(1);
    }
}

int main() {
    measure_thread_switch();
    measure_process_switch();
    return 0;
}
