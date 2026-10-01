#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>

#define CACHE_LINE_SIZE 64
#define ITERATIONS 500000

atomic_int x __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int y __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int begin __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int end0 __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int end1 __attribute__((aligned(CACHE_LINE_SIZE)));

int r0[ITERATIONS];
int r1[ITERATIONS];

void* sb_t0(void* arg) {
    bool seq_cst = (bool)(intptr_t)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (atomic_load_explicit(&begin, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
        if (seq_cst) {
            atomic_store_explicit(&x, 1, memory_order_seq_cst);
            r0[i] = atomic_load_explicit(&y, memory_order_seq_cst);
        } else {
            atomic_store_explicit(&x, 1, memory_order_relaxed);
            r0[i] = atomic_load_explicit(&y, memory_order_relaxed);
        }
        atomic_store_explicit(&end0, i + 1, memory_order_release);
    }
    return NULL;
}

void* sb_t1(void* arg) {
    bool seq_cst = (bool)(intptr_t)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (atomic_load_explicit(&begin, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
        if (seq_cst) {
            atomic_store_explicit(&y, 1, memory_order_seq_cst);
            r1[i] = atomic_load_explicit(&x, memory_order_seq_cst);
        } else {
            atomic_store_explicit(&y, 1, memory_order_relaxed);
            r1[i] = atomic_load_explicit(&x, memory_order_relaxed);
        }
        atomic_store_explicit(&end1, i + 1, memory_order_release);
    }
    return NULL;
}

void run_sb(bool seq_cst) {
    pthread_t th0, th1;
    atomic_init(&x, 0);
    atomic_init(&y, 0);
    atomic_init(&begin, 0);
    atomic_init(&end0, 0);
    atomic_init(&end1, 0);
    
    pthread_create(&th0, NULL, sb_t0, (void*)(intptr_t)seq_cst);
    pthread_create(&th1, NULL, sb_t1, (void*)(intptr_t)seq_cst);
    
    int weak = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_store_explicit(&x, 0, memory_order_relaxed);
        atomic_store_explicit(&y, 0, memory_order_relaxed);
        atomic_store_explicit(&begin, i + 1, memory_order_release);
        
        while (atomic_load_explicit(&end0, memory_order_acquire) != i + 1 ||
               atomic_load_explicit(&end1, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
               
        if (r0[i] == 0 && r1[i] == 0) {
            weak++;
        }
    }
    
    pthread_join(th0, NULL);
    pthread_join(th1, NULL);
    
    printf("Store Buffering (%s): %d weak behaviors out of %d\n", seq_cst ? "seq_cst" : "relaxed", weak, ITERATIONS);
}

atomic_int data __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int flag __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int mp_begin __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int mp_end0 __attribute__((aligned(CACHE_LINE_SIZE)));
atomic_int mp_end1 __attribute__((aligned(CACHE_LINE_SIZE)));
int mp_r[ITERATIONS];

void* mp_t0(void* arg) {
    bool strict = (bool)(intptr_t)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (atomic_load_explicit(&mp_begin, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
        if (strict) {
            atomic_store_explicit(&data, 1, memory_order_relaxed);
            atomic_store_explicit(&flag, 1, memory_order_release);
        } else {
            atomic_store_explicit(&data, 1, memory_order_relaxed);
            atomic_store_explicit(&flag, 1, memory_order_relaxed);
        }
        atomic_store_explicit(&mp_end0, i + 1, memory_order_release);
    }
    return NULL;
}

void* mp_t1(void* arg) {
    bool strict = (bool)(intptr_t)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        while (atomic_load_explicit(&mp_begin, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
        
        int r_data = 0;
        if (strict) {
            while (atomic_load_explicit(&flag, memory_order_acquire) == 0) {
                __asm__ volatile("yield" ::: "memory");
            }
            r_data = atomic_load_explicit(&data, memory_order_relaxed);
        } else {
            while (atomic_load_explicit(&flag, memory_order_relaxed) == 0) {
                __asm__ volatile("yield" ::: "memory");
            }
            r_data = atomic_load_explicit(&data, memory_order_relaxed);
        }
        mp_r[i] = r_data;
        atomic_store_explicit(&mp_end1, i + 1, memory_order_release);
    }
    return NULL;
}

void run_mp(bool strict) {
    pthread_t th0, th1;
    atomic_init(&data, 0);
    atomic_init(&flag, 0);
    atomic_init(&mp_begin, 0);
    atomic_init(&mp_end0, 0);
    atomic_init(&mp_end1, 0);
    
    pthread_create(&th0, NULL, mp_t0, (void*)(intptr_t)strict);
    pthread_create(&th1, NULL, mp_t1, (void*)(intptr_t)strict);
    
    int weak = 0;
    for (int i = 0; i < ITERATIONS; i++) {
        atomic_store_explicit(&data, 0, memory_order_relaxed);
        atomic_store_explicit(&flag, 0, memory_order_relaxed);
        atomic_store_explicit(&mp_begin, i + 1, memory_order_release);
        
        while (atomic_load_explicit(&mp_end0, memory_order_acquire) != i + 1 ||
               atomic_load_explicit(&mp_end1, memory_order_acquire) != i + 1) {
            __asm__ volatile("yield" ::: "memory");
        }
               
        if (mp_r[i] == 0) {
            weak++;
        }
    }
    
    pthread_join(th0, NULL);
    pthread_join(th1, NULL);
    
    printf("Message Passing (%s): %d weak behaviors out of %d\n", strict ? "release/acquire" : "relaxed", weak, ITERATIONS);
}

int main() {
    printf("Starting memory ordering litmus tests (%d iterations each)...\n", ITERATIONS);
    run_sb(false);
    run_sb(true);
    run_mp(false);
    run_mp(true);
    printf("PASS: Litmus tests complete.\n");
    return 0;
}
