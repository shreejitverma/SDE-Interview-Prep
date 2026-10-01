#include "spinlocks.h"
#include <stdlib.h>
#include <time.h>
#include <sched.h>
#include <stdint.h>

// Yield function to relax CPU
static inline void cpu_relax() {
#if defined(__aarch64__)
    __asm__ volatile("yield" ::: "memory");
#elif defined(__x86_64__) || defined(__i386__)
    __asm__ volatile("pause" ::: "memory");
#else
    sched_yield();
#endif
}

// TAS
void tas_init(tas_lock_t* lock) {
    atomic_init(&lock->locked, false);
}
void tas_acquire(tas_lock_t* lock) {
    while (atomic_exchange_explicit(&lock->locked, true, memory_order_acquire)) {
        cpu_relax();
    }
}
void tas_release(tas_lock_t* lock) {
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}

// TTAS
void ttas_init(ttas_lock_t* lock) {
    atomic_init(&lock->locked, false);
}
void ttas_acquire(ttas_lock_t* lock) {
    while (true) {
        while (atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            cpu_relax();
        }
        if (!atomic_exchange_explicit(&lock->locked, true, memory_order_acquire)) {
            return;
        }
    }
}
void ttas_release(ttas_lock_t* lock) {
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}

// Exponential Backoff Lock
#define MIN_DELAY 10
#define MAX_DELAY 1000

static void delay(int limit) {
    volatile int delay = 0;
    while (delay < limit) {
        delay++;
        cpu_relax();
    }
}

void backoff_init(backoff_lock_t* lock) {
    atomic_init(&lock->locked, false);
}
void backoff_acquire(backoff_lock_t* lock) {
    int limit = MIN_DELAY;
    while (true) {
        while (atomic_load_explicit(&lock->locked, memory_order_relaxed)) {
            cpu_relax();
        }
        if (!atomic_exchange_explicit(&lock->locked, true, memory_order_acquire)) {
            return;
        }
        delay((rand() % limit) + 1);
        limit = limit * 2;
        if (limit > MAX_DELAY) {
            limit = MAX_DELAY;
        }
    }
}
void backoff_release(backoff_lock_t* lock) {
    atomic_store_explicit(&lock->locked, false, memory_order_release);
}

// Ticket Lock
void ticket_init(ticket_lock_t* lock) {
    atomic_init(&lock->next_ticket, 0);
    atomic_init(&lock->now_serving, 0);
}
void ticket_acquire(ticket_lock_t* lock) {
    size_t my_ticket = atomic_fetch_add_explicit(&lock->next_ticket, 1, memory_order_relaxed);
    while (atomic_load_explicit(&lock->now_serving, memory_order_acquire) != my_ticket) {
        cpu_relax();
    }
}
void ticket_release(ticket_lock_t* lock) {
    atomic_fetch_add_explicit(&lock->now_serving, 1, memory_order_release);
}

// Anderson Array Lock
void anderson_init(anderson_lock_t* lock, int max_threads) {
    lock->max_threads = max_threads;
    atomic_init(&lock->next_slot, 0);
    // Align to cache line
    void* ptr = NULL;
    if (posix_memalign(&ptr, CACHE_LINE_SIZE, max_threads * sizeof(anderson_flag_t)) != 0) {
        exit(1);
    }
    lock->flags = (anderson_flag_t*)ptr;
    atomic_init(&lock->flags[0].has_lock, true);
    for (int i = 1; i < max_threads; i++) {
        atomic_init(&lock->flags[i].has_lock, false);
    }
}
void anderson_acquire(anderson_lock_t* lock, int* my_slot) {
    size_t slot = atomic_fetch_add_explicit(&lock->next_slot, 1, memory_order_relaxed);
    *my_slot = slot % lock->max_threads;
    while (!atomic_load_explicit(&lock->flags[*my_slot].has_lock, memory_order_acquire)) {
        cpu_relax();
    }
    // We have the lock. Prepare it for the next time it's used
    atomic_store_explicit(&lock->flags[*my_slot].has_lock, false, memory_order_relaxed);
}
void anderson_release(anderson_lock_t* lock, int my_slot) {
    int next = (my_slot + 1) % lock->max_threads;
    atomic_store_explicit(&lock->flags[next].has_lock, true, memory_order_release);
}
void anderson_destroy(anderson_lock_t* lock) {
    free(lock->flags);
}

// MCS Lock
void mcs_init(mcs_lock_t* lock) {
    atomic_init(&lock->tail, NULL);
}
void mcs_acquire(mcs_lock_t* lock, mcs_node_t* my_node) {
    atomic_init(&my_node->next, NULL);
    atomic_init(&my_node->locked, true);
    
    mcs_node_t* pred = atomic_exchange_explicit(&lock->tail, my_node, memory_order_acq_rel);
    if (pred != NULL) {
        atomic_store_explicit(&pred->next, my_node, memory_order_release);
        while (atomic_load_explicit(&my_node->locked, memory_order_acquire)) {
            cpu_relax();
        }
    }
}
void mcs_release(mcs_lock_t* lock, mcs_node_t* my_node) {
    mcs_node_t* next = atomic_load_explicit(&my_node->next, memory_order_acquire);
    if (next == NULL) {
        mcs_node_t* expected = my_node;
        if (atomic_compare_exchange_strong_explicit(&lock->tail, &expected, NULL, memory_order_release, memory_order_relaxed)) {
            return;
        }
        while ((next = atomic_load_explicit(&my_node->next, memory_order_acquire)) == NULL) {
            cpu_relax();
        }
    }
    atomic_store_explicit(&next->locked, false, memory_order_release);
}
