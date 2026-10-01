#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <stdint.h>

#define NUM_ITERS 1000

uint64_t get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int cmp_u64(const void *a, const void *b) {
    uint64_t arg1 = *(const uint64_t *)a;
    uint64_t arg2 = *(const uint64_t *)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

int main(int argc, char **argv) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Measuring MPI_Barrier with %d processes, %d iterations\n", size, NUM_ITERS);
    }

    uint64_t *times = (uint64_t *)malloc(NUM_ITERS * sizeof(uint64_t));

    // Warmup
    for (int i = 0; i < 10; i++) {
        MPI_Barrier(MPI_COMM_WORLD);
    }

    for (int i = 0; i < NUM_ITERS; i++) {
        uint64_t t0 = get_time_ns();
        MPI_Barrier(MPI_COMM_WORLD);
        uint64_t t1 = get_time_ns();
        times[i] = t1 - t0;
    }

    qsort(times, NUM_ITERS, sizeof(uint64_t), cmp_u64);
    uint64_t my_median = times[NUM_ITERS / 2];

    uint64_t *all_medians = NULL;
    if (rank == 0) {
        all_medians = (uint64_t *)malloc(size * sizeof(uint64_t));
    }

    MPI_Gather(&my_median, 1, MPI_UINT64_T, all_medians, 1, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        uint64_t sum = 0;
        for (int i = 0; i < size; i++) {
            sum += all_medians[i];
        }
        printf("MPI Barrier Median Latency: %llu ns\n", (unsigned long long)(sum / size));
        free(all_medians);
    }

    free(times);
    MPI_Finalize();
    return 0;
}
