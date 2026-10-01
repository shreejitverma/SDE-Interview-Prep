#include "spinlocks.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define NUM_ITERATIONS 10000
#define MAX_THREADS 8

typedef enum { TAS, TTAS, BACKOFF, TICKET, ANDERSON, MCS } lock_type_t;

typedef struct {
    int thread_id;
    int num_threads;
    lock_type_t lock_type;
    int iterations;
    volatile int* shared_counter;
    
    tas_lock_t* tas_lock;
    ttas_lock_t* ttas_lock;
    backoff_lock_t* backoff_lock;
    ticket_lock_t* ticket_lock;
    anderson_lock_t* anderson_lock;
    mcs_lock_t* mcs_lock;
} thread_arg_t;

void acquire_lock(thread_arg_t* arg, int* my_slot, mcs_node_t* my_node) {
    switch (arg->lock_type) {
        case TAS: tas_acquire(arg->tas_lock); break;
        case TTAS: ttas_acquire(arg->ttas_lock); break;
        case BACKOFF: backoff_acquire(arg->backoff_lock); break;
        case TICKET: ticket_acquire(arg->ticket_lock); break;
        case ANDERSON: anderson_acquire(arg->anderson_lock, my_slot); break;
        case MCS: mcs_acquire(arg->mcs_lock, my_node); break;
    }
}

void release_lock(thread_arg_t* arg, int my_slot, mcs_node_t* my_node) {
    switch (arg->lock_type) {
        case TAS: tas_release(arg->tas_lock); break;
        case TTAS: ttas_release(arg->ttas_lock); break;
        case BACKOFF: backoff_release(arg->backoff_lock); break;
        case TICKET: ticket_release(arg->ticket_lock); break;
        case ANDERSON: anderson_release(arg->anderson_lock, my_slot); break;
        case MCS: mcs_release(arg->mcs_lock, my_node); break;
    }
}

void* worker(void* ptr) {
    thread_arg_t* arg = (thread_arg_t*)ptr;
    int my_slot = -1;
    mcs_node_t my_node;
    
    for (int i = 0; i < arg->iterations; i++) {
        acquire_lock(arg, &my_slot, &my_node);
        // Critical section
        (*arg->shared_counter)++;
        release_lock(arg, my_slot, &my_node);
    }
    return NULL;
}

double get_time_diff(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) / 1e9;
}

void run_lock(lock_type_t lock_type, const char* name, int num_threads, int iterations, int check_correctness) {
    pthread_t threads[MAX_THREADS];
    thread_arg_t args[MAX_THREADS];
    
    volatile int shared_counter = 0;
    
    tas_lock_t tas_lock;
    ttas_lock_t ttas_lock;
    backoff_lock_t backoff_lock;
    ticket_lock_t ticket_lock;
    anderson_lock_t anderson_lock;
    mcs_lock_t mcs_lock;
    
    tas_init(&tas_lock);
    ttas_init(&ttas_lock);
    backoff_init(&backoff_lock);
    ticket_init(&ticket_lock);
    anderson_init(&anderson_lock, num_threads);
    mcs_init(&mcs_lock);
    
    for (int i = 0; i < num_threads; i++) {
        args[i].thread_id = i;
        args[i].num_threads = num_threads;
        args[i].lock_type = lock_type;
        args[i].iterations = iterations;
        args[i].shared_counter = &shared_counter;
        
        args[i].tas_lock = &tas_lock;
        args[i].ttas_lock = &ttas_lock;
        args[i].backoff_lock = &backoff_lock;
        args[i].ticket_lock = &ticket_lock;
        args[i].anderson_lock = &anderson_lock;
        args[i].mcs_lock = &mcs_lock;
    }
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, worker, &args[i]);
    }
    
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    
    if (lock_type == ANDERSON) {
        anderson_destroy(&anderson_lock);
    }
    
    if (check_correctness) {
        if (shared_counter != num_threads * iterations) {
            printf("FAIL: %s (Expected %d, got %d)\n", name, num_threads * iterations, shared_counter);
            exit(1);
        }
    } else {
        double elapsed = get_time_diff(&start, &end);
        printf("%s,%d,%.6f\n", name, num_threads, elapsed);
    }
}

void run_tests() {
    printf("Running correctness tests...\n");
    const char* names[] = {"TAS", "TTAS", "Backoff", "Ticket", "Anderson", "MCS"};
    lock_type_t types[] = {TAS, TTAS, BACKOFF, TICKET, ANDERSON, MCS};
    int iters = 10000;
    
    for (int i = 0; i < 6; i++) {
        run_lock(types[i], names[i], MAX_THREADS, iters, 1);
        printf("PASS: %s lock is correct.\n", names[i]);
    }
}

void run_benchmark() {
    printf("Running benchmarks...\n");
    printf("Lock,Threads,Time_s\n");
    const char* names[] = {"TAS", "TTAS", "Backoff", "Ticket", "Anderson", "MCS"};
    lock_type_t types[] = {TAS, TTAS, BACKOFF, TICKET, ANDERSON, MCS};
    
    for (int i = 0; i < 6; i++) {
        for (int t = 1; t <= MAX_THREADS; t++) {
            run_lock(types[i], names[i], t, NUM_ITERATIONS, 0);
        }
    }
}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    if (argc > 1 && strcmp(argv[1], "test") == 0) {
        run_tests();
    } else if (argc > 1 && strcmp(argv[1], "benchmark") == 0) {
        run_benchmark();
    } else {
        printf("Usage: %s <test|benchmark>\n", argv[0]);
        return 1;
    }
    return 0;
}
