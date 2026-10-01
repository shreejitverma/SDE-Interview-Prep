#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>

#define NUM_READERS 4
#define NUM_WRITERS 1
#define READ_ITERATIONS 5000000
#define WRITE_ITERATIONS 50

struct config {
    int data1;
    int data2;
};

// ==========================================
// 1. RWLock Implementation
// ==========================================
struct config rwlock_config = {0, 0};
pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void* rwlock_reader(void* arg) {
    (void)arg;
    uint64_t local_sum = 0;
    for (int i = 0; i < READ_ITERATIONS; i++) {
        pthread_rwlock_rdlock(&rwlock);
        local_sum += rwlock_config.data1 + rwlock_config.data2;
        pthread_rwlock_unlock(&rwlock);
    }
    return (void*)local_sum;
}

void* rwlock_writer(void* arg) {
    (void)arg;
    for (int i = 0; i < WRITE_ITERATIONS; i++) {
        pthread_rwlock_wrlock(&rwlock);
        rwlock_config.data1++;
        rwlock_config.data2++;
        pthread_rwlock_unlock(&rwlock);
        usleep(1000); 
    }
    return NULL;
}

// ==========================================
// 2. RCU-like (Epoch-based) Implementation
// ==========================================
_Atomic(struct config*) rcu_config_ptr;
_Atomic uint64_t global_epoch = 1;
_Atomic uint64_t reader_epoch[NUM_READERS];

void* rcu_reader(void* arg) {
    long tid = (long)arg;
    uint64_t local_sum = 0;
    for (int i = 0; i < READ_ITERATIONS; i++) {
        uint64_t epoch = atomic_load_explicit(&global_epoch, memory_order_acquire);
        atomic_store_explicit(&reader_epoch[tid], epoch, memory_order_release);
        
        struct config* current = atomic_load_explicit(&rcu_config_ptr, memory_order_acquire);
        local_sum += current->data1 + current->data2;
        
        atomic_store_explicit(&reader_epoch[tid], 0, memory_order_release);
    }
    return (void*)local_sum;
}

void synchronize_rcu() {
    uint64_t epoch = atomic_fetch_add(&global_epoch, 1) + 1;
    for (int i = 0; i < NUM_READERS; i++) {
        while (1) {
            uint64_t rep = atomic_load_explicit(&reader_epoch[i], memory_order_acquire);
            if (rep == 0 || rep >= epoch) break; 
            // spin (a real implementation might yield)
        }
    }
}

void* rcu_writer(void* arg) {
    (void)arg;
    for (int i = 0; i < WRITE_ITERATIONS; i++) {
        struct config* old_conf = atomic_load_explicit(&rcu_config_ptr, memory_order_acquire);
        
        struct config* new_conf = malloc(sizeof(struct config));
        new_conf->data1 = old_conf->data1 + 1;
        new_conf->data2 = old_conf->data2 + 1;
        
        atomic_store_explicit(&rcu_config_ptr, new_conf, memory_order_release);
        
        synchronize_rcu();
        free(old_conf);
        
        usleep(1000); 
    }
    return NULL;
}

// ==========================================
// Benchmarking
// ==========================================
double get_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

void run_test(const char* name, void* (*r_func)(void*), void* (*w_func)(void*)) {
    pthread_t r_threads[NUM_READERS];
    pthread_t w_threads[NUM_WRITERS];
    
    double start = get_time();

    for (long i = 0; i < NUM_READERS; i++) {
        pthread_create(&r_threads[i], NULL, r_func, (void*)i);
    }
    for (long i = 0; i < NUM_WRITERS; i++) {
        pthread_create(&w_threads[i], NULL, w_func, (void*)i);
    }

    uint64_t total_sum = 0;
    for (int i = 0; i < NUM_READERS; i++) {
        void* ret;
        pthread_join(r_threads[i], &ret);
        total_sum += (uint64_t)ret;
    }
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(w_threads[i], NULL);
    }

    double end = get_time();
    printf("%-20s: %.4f seconds\n", name, end - start);
    // Print total_sum to prevent optimization
    // printf("  (sum=%lu)\n", total_sum);
}

int main() {
    printf("Running Read-Mostly Test (%d readers, %d writers)...\n", NUM_READERS, NUM_WRITERS);
    
    // Init RCU
    for (int i = 0; i < NUM_READERS; i++) {
        reader_epoch[i] = 0;
    }
    struct config* init_conf = malloc(sizeof(struct config));
    init_conf->data1 = 0;
    init_conf->data2 = 0;
    atomic_store(&rcu_config_ptr, init_conf);

    run_test("RWLock", rwlock_reader, rwlock_writer);
    run_test("RCU-like", rcu_reader, rcu_writer);

    return 0;
}
