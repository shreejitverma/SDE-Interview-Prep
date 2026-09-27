/*
 * producer_consumer_posix.c
 * Classic Producer-Consumer using POSIX mutexes, condition variables,
 * and semaphores. Demonstrates multiple synchronization approaches.
 *
 * Compile: gcc -Wall -O2 -o prod_cons producer_consumer_posix.c -lpthread
 * Run:     ./prod_cons [buffer_size] [num_producers] [num_consumers] [items_per_producer]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdint.h>
#include <stdatomic.h>

/* ── Buffer Configuration ─────────────────────────────────────────── */
#define DEFAULT_BUF_SIZE      8
#define DEFAULT_N_PRODUCERS   2
#define DEFAULT_N_CONSUMERS   3
#define DEFAULT_ITEMS_PER_PRD 20

typedef struct {
    int value;
    int producer_id;
    int sequence;
} Item;

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 1: Mutex + Condition Variables                            */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    Item        *buf;
    int          capacity;
    int          head, tail, count;
    pthread_mutex_t lock;
    pthread_cond_t  not_full;
    pthread_cond_t  not_empty;
    /* Stats */
    atomic_long  total_produced;
    atomic_long  total_consumed;
    atomic_long  producer_waits;
    atomic_long  consumer_waits;
} BoundedBuffer;

BoundedBuffer *bb_create(int capacity) {
    BoundedBuffer *bb = calloc(1, sizeof(BoundedBuffer));
    bb->buf      = calloc(capacity, sizeof(Item));
    bb->capacity = capacity;
    pthread_mutex_init(&bb->lock, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
    return bb;
}

void bb_destroy(BoundedBuffer *bb) {
    pthread_mutex_destroy(&bb->lock);
    pthread_cond_destroy(&bb->not_full);
    pthread_cond_destroy(&bb->not_empty);
    free(bb->buf);
    free(bb);
}

void bb_produce(BoundedBuffer *bb, Item item) {
    pthread_mutex_lock(&bb->lock);

    /* Wait while buffer is full */
    while (bb->count == bb->capacity) {
        atomic_fetch_add(&bb->producer_waits, 1);
        pthread_cond_wait(&bb->not_full, &bb->lock);
    }

    /* Insert at tail (circular buffer) */
    bb->buf[bb->tail] = item;
    bb->tail = (bb->tail + 1) % bb->capacity;
    bb->count++;
    atomic_fetch_add(&bb->total_produced, 1);

    /* Signal a waiting consumer */
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->lock);
}

Item bb_consume(BoundedBuffer *bb) {
    pthread_mutex_lock(&bb->lock);

    /* Wait while buffer is empty */
    while (bb->count == 0) {
        atomic_fetch_add(&bb->consumer_waits, 1);
        pthread_cond_wait(&bb->not_empty, &bb->lock);
    }

    /* Remove from head (circular buffer) */
    Item item = bb->buf[bb->head];
    bb->head = (bb->head + 1) % bb->capacity;
    bb->count--;
    atomic_fetch_add(&bb->total_consumed, 1);

    /* Signal a waiting producer */
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->lock);

    return item;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 2: Semaphore-based (Dijkstra's classic solution)          */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    Item    *buf;
    int      capacity;
    int      head, tail;
    sem_t    empty_slots;   /* counts available slots for producers */
    sem_t    filled_slots;  /* counts items available for consumers */
    sem_t    mutex;         /* mutual exclusion for head/tail */
} SemBuffer;

SemBuffer *sembuf_create(int capacity) {
    SemBuffer *sb = calloc(1, sizeof(SemBuffer));
    sb->buf      = calloc(capacity, sizeof(Item));
    sb->capacity = capacity;
    sem_init(&sb->empty_slots, 0, capacity);  /* initially: all slots empty */
    sem_init(&sb->filled_slots, 0, 0);        /* initially: no items */
    sem_init(&sb->mutex, 0, 1);               /* binary semaphore = mutex */
    return sb;
}

void sem_produce(SemBuffer *sb, Item item) {
    sem_wait(&sb->empty_slots);  /* Block if no empty slots */
    sem_wait(&sb->mutex);        /* Enter critical section */

    sb->buf[sb->tail] = item;
    sb->tail = (sb->tail + 1) % sb->capacity;

    sem_post(&sb->mutex);        /* Leave critical section */
    sem_post(&sb->filled_slots); /* Signal: one more item available */
}

Item sem_consume(SemBuffer *sb) {
    sem_wait(&sb->filled_slots); /* Block if no items */
    sem_wait(&sb->mutex);        /* Enter critical section */

    Item item = sb->buf[sb->head];
    sb->head = (sb->head + 1) % sb->capacity;

    sem_post(&sb->mutex);        /* Leave critical section */
    sem_post(&sb->empty_slots);  /* Signal: one more slot available */
    return item;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Thread functions                                                   */
/* ──────────────────────────────────────────────────────────────────── */

typedef struct {
    BoundedBuffer *bb;
    int            id;
    int            items_to_produce;
    int            delay_us;  /* simulated work delay */
} ProducerArgs;

typedef struct {
    BoundedBuffer *bb;
    int            id;
    int            total_items_to_consume;
    int            delay_us;
    atomic_int    *consumed_count;
} ConsumerArgs;

static atomic_int g_done = 0;

void *producer_thread(void *arg) {
    ProducerArgs *a = arg;

    for (int i = 0; i < a->items_to_produce; i++) {
        /* Simulate work (e.g., reading from network, processing) */
        if (a->delay_us > 0)
            usleep(a->delay_us + rand() % a->delay_us);

        Item item = {
            .value       = rand() % 1000,
            .producer_id = a->id,
            .sequence    = i
        };
        bb_produce(a->bb, item);
        printf("[Producer %d] Produced item %d (val=%d)\n",
               a->id, i, item.value);
    }
    printf("[Producer %d] Done producing %d items\n", a->id, a->items_to_produce);
    return NULL;
}

void *consumer_thread(void *arg) {
    ConsumerArgs *a = arg;

    while (1) {
        /* Check if we've consumed everything */
        if (atomic_load(&g_done) &&
            atomic_load(a->consumed_count) >= a->total_items_to_consume)
            break;

        Item item = bb_consume(a->bb);

        /* Simulate processing */
        if (a->delay_us > 0)
            usleep(a->delay_us + rand() % a->delay_us);

        int consumed = atomic_fetch_add(a->consumed_count, 1) + 1;
        printf("[Consumer %d] Consumed item from P%d seq=%d val=%d (total=%d)\n",
               a->id, item.producer_id, item.sequence, item.value, consumed);

        if (consumed >= a->total_items_to_consume) break;
    }
    printf("[Consumer %d] Done.\n", a->id);
    return NULL;
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Main                                                               */
/* ──────────────────────────────────────────────────────────────────── */

int main(int argc, char *argv[]) {
    int buf_size    = (argc > 1) ? atoi(argv[1]) : DEFAULT_BUF_SIZE;
    int n_producers = (argc > 2) ? atoi(argv[2]) : DEFAULT_N_PRODUCERS;
    int n_consumers = (argc > 3) ? atoi(argv[3]) : DEFAULT_N_CONSUMERS;
    int items_pp    = (argc > 4) ? atoi(argv[4]) : DEFAULT_ITEMS_PER_PRD;
    int total_items = n_producers * items_pp;

    printf("=== Producer-Consumer (mutex + condvar) ===\n");
    printf("Buffer: %d slots | Producers: %d | Consumers: %d | Items: %d total\n\n",
           buf_size, n_producers, n_consumers, total_items);

    srand(42);

    BoundedBuffer *bb = bb_create(buf_size);

    pthread_t producers[n_producers];
    pthread_t consumers[n_consumers];
    ProducerArgs pargs[n_producers];
    ConsumerArgs cargs[n_consumers];
    atomic_int consumed_count = 0;

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    /* Start consumers */
    for (int i = 0; i < n_consumers; i++) {
        cargs[i] = (ConsumerArgs){
            .bb = bb,
            .id = i,
            .total_items_to_consume = total_items,
            .delay_us = 500,
            .consumed_count = &consumed_count
        };
        pthread_create(&consumers[i], NULL, consumer_thread, &cargs[i]);
    }

    /* Start producers */
    for (int i = 0; i < n_producers; i++) {
        pargs[i] = (ProducerArgs){
            .bb               = bb,
            .id               = i,
            .items_to_produce = items_pp,
            .delay_us         = 300
        };
        pthread_create(&producers[i], NULL, producer_thread, &pargs[i]);
    }

    /* Wait for all producers to finish */
    for (int i = 0; i < n_producers; i++)
        pthread_join(producers[i], NULL);

    atomic_store(&g_done, 1);

    /* Wake up any stuck consumers (buffer may be empty but they're waiting) */
    pthread_mutex_lock(&bb->lock);
    pthread_cond_broadcast(&bb->not_empty);
    pthread_mutex_unlock(&bb->lock);

    /* Wait for all consumers */
    for (int i = 0; i < n_consumers; i++)
        pthread_join(consumers[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    printf("\n=== Results ===\n");
    printf("Total produced: %ld\n", atomic_load(&bb->total_produced));
    printf("Total consumed: %ld\n", atomic_load(&bb->total_consumed));
    printf("Producer waits: %ld\n", atomic_load(&bb->producer_waits));
    printf("Consumer waits: %ld\n", atomic_load(&bb->consumer_waits));
    printf("Elapsed: %.3f seconds\n", elapsed);
    printf("Throughput: %.0f items/sec\n", total_items / elapsed);

    bb_destroy(bb);
    return 0;
}
