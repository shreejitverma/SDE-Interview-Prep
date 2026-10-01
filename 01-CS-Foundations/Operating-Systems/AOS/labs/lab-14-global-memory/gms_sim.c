#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>

#define PAGE_SIZE 4096
#define N_NODES 4
#define PAGES_PER_NODE 50
#define TOTAL_PAGES (N_NODES * PAGES_PER_NODE)

typedef struct {
    int id;
    int page_timestamps[PAGES_PER_NODE];
    int weight; // Number of pages older than MinAge
} Node;

Node nodes[N_NODES];
int min_age_cutoff = 0;
int current_time = 0;

int cmp_int_desc(const void *a, const void *b) {
    return (*(const int*)b) - (*(const int*)a);
}

// Emulate an epoch update
void trigger_epoch() {
    int ages[TOTAL_PAGES];
    int idx = 0;
    for (int i = 0; i < N_NODES; i++) {
        for (int j = 0; j < PAGES_PER_NODE; j++) {
            // age = current_time - timestamp
            ages[idx++] = current_time - nodes[i].page_timestamps[j];
        }
    }
    
    // Sort descending to find oldest
    qsort(ages, TOTAL_PAGES, sizeof(int), cmp_int_desc);
    
    // Threshold for oldest 25% of pages globally
    int cutoff_idx = TOTAL_PAGES / 4;
    min_age_cutoff = ages[cutoff_idx];
    
    // Calculate weights
    for (int i = 0; i < N_NODES; i++) {
        nodes[i].weight = 0;
        for (int j = 0; j < PAGES_PER_NODE; j++) {
            if ((current_time - nodes[i].page_timestamps[j]) >= min_age_cutoff) {
                nodes[i].weight++;
            }
        }
    }
    
    printf("[Epoch] Time: %d, MinAge: %d\n", current_time, min_age_cutoff);
    for (int i = 0; i < N_NODES; i++) {
        printf("  Node %d weight (pages older than MinAge): %d\n", i, nodes[i].weight);
    }
}

// Find node with largest weight to host a global page
int select_target_node() {
    int max_weight = -1;
    int target = -1;
    for (int i = 0; i < N_NODES; i++) {
        if (nodes[i].weight > max_weight) {
            max_weight = nodes[i].weight;
            target = i;
        }
    }
    return target;
}

// Cost measurement
long get_nanos() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int cmp_long(const void *a, const void *b) {
    long la = *(const long*)a;
    long lb = *(const long*)b;
    return (la > lb) - (la < lb);
}

void measure_paging_costs() {
    int fds[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) < 0) {
        perror("socketpair");
        exit(1);
    }
    
    char *src = malloc(PAGE_SIZE);
    char *dst = malloc(PAGE_SIZE);
    memset(src, 0xAA, PAGE_SIZE);
    
    int trials = 10000;
    long *local_times = malloc(trials * sizeof(long));
    long *remote_times = malloc(trials * sizeof(long));
    
    // Warmup
    for (int i=0; i<100; i++) {
        memcpy(dst, src, PAGE_SIZE);
    }
    
    for (int i=0; i<trials; i++) {
        long t0 = get_nanos();
        memcpy(dst, src, PAGE_SIZE);
        long t1 = get_nanos();
        local_times[i] = t1 - t0;
    }
    
    // Warmup
    for (int i=0; i<100; i++) {
        if (write(fds[0], src, PAGE_SIZE) != PAGE_SIZE) exit(1);
        if (read(fds[1], dst, PAGE_SIZE) != PAGE_SIZE) exit(1);
    }
    
    for (int i=0; i<trials; i++) {
        long t0 = get_nanos();
        if (write(fds[0], src, PAGE_SIZE) != PAGE_SIZE) exit(1);
        if (read(fds[1], dst, PAGE_SIZE) != PAGE_SIZE) exit(1);
        long t1 = get_nanos();
        remote_times[i] = t1 - t0;
    }
    
    qsort(local_times, trials, sizeof(long), cmp_long);
    qsort(remote_times, trials, sizeof(long), cmp_long);
    
    long local_med = local_times[trials/2];
    long remote_med = remote_times[trials/2];
    
    printf("[Costs] Local memory copy (4KB): %ld ns\n", local_med);
    printf("[Costs] Remote socket fetch (4KB): %ld ns\n", remote_med);
    if (remote_med > 0) {
        printf("[Costs] Remote is ~%ldx slower than local\n", remote_med / (local_med == 0 ? 1 : local_med));
    }
    printf("PASS: Cost measurement completed.\n");
    
    free(src); free(dst);
    free(local_times); free(remote_times);
    close(fds[0]); close(fds[1]);
}

int main() {
    printf("--- Global Memory Simulator ---\n");
    
    // Initialize timestamps randomly to simulate skew
    srand(42);
    for (int i = 0; i < N_NODES; i++) {
        nodes[i].id = i;
        for (int j = 0; j < PAGES_PER_NODE; j++) {
            // Some nodes have very recent pages, some very old
            if (i == 0) {
                nodes[i].page_timestamps[j] = current_time - (rand() % 100); // hot
            } else if (i == N_NODES - 1) {
                nodes[i].page_timestamps[j] = current_time - 1000 - (rand() % 100); // cold
            } else {
                nodes[i].page_timestamps[j] = current_time - (rand() % 500); // mixed
            }
        }
    }
    
    current_time = 2000;
    trigger_epoch();
    
    int target = select_target_node();
    printf("\nNode selected to receive global page (largest weight): Node %d\n", target);
    if (nodes[target].weight > 0) {
        printf("PASS: Found target node with non-zero weight.\n\n");
    } else {
        printf("FAIL: No node has weight.\n\n");
    }
    
    measure_paging_costs();
    
    return 0;
}
