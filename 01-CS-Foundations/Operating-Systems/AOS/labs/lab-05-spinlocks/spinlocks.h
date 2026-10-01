#ifndef SPINLOCKS_H
#define SPINLOCKS_H

#include <stdatomic.h>
#include <stdbool.h>

// TAS Lock
typedef struct {
    atomic_bool locked;
} tas_lock_t;
void tas_init(tas_lock_t* lock);
void tas_acquire(tas_lock_t* lock);
void tas_release(tas_lock_t* lock);

// TTAS Lock
typedef struct {
    atomic_bool locked;
} ttas_lock_t;
void ttas_init(ttas_lock_t* lock);
void ttas_acquire(ttas_lock_t* lock);
void ttas_release(ttas_lock_t* lock);

// Backoff Lock
typedef struct {
    atomic_bool locked;
} backoff_lock_t;
void backoff_init(backoff_lock_t* lock);
void backoff_acquire(backoff_lock_t* lock);
void backoff_release(backoff_lock_t* lock);

// Ticket Lock
typedef struct {
    atomic_size_t next_ticket;
    atomic_size_t now_serving;
} ticket_lock_t;
void ticket_init(ticket_lock_t* lock);
void ticket_acquire(ticket_lock_t* lock);
void ticket_release(ticket_lock_t* lock);

// Anderson Array Lock
// Cache line size is typically 64 bytes
#define CACHE_LINE_SIZE 64
typedef struct {
    atomic_bool has_lock;
    char padding[CACHE_LINE_SIZE - sizeof(atomic_bool)];
} anderson_flag_t;

typedef struct {
    anderson_flag_t* flags;
    atomic_size_t next_slot;
    int max_threads;
} anderson_lock_t;
void anderson_init(anderson_lock_t* lock, int max_threads);
void anderson_acquire(anderson_lock_t* lock, int* my_slot);
void anderson_release(anderson_lock_t* lock, int my_slot);
void anderson_destroy(anderson_lock_t* lock);

// MCS Lock
typedef struct mcs_node {
    atomic_bool locked;
    struct mcs_node * volatile _Atomic next; // stdatomic points to mcs_node
} mcs_node_t;

typedef struct {
    mcs_node_t * volatile _Atomic tail;
} mcs_lock_t;

void mcs_init(mcs_lock_t* lock);
void mcs_acquire(mcs_lock_t* lock, mcs_node_t* my_node);
void mcs_release(mcs_lock_t* lock, mcs_node_t* my_node);

#endif // SPINLOCKS_H
