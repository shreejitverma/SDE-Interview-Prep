#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define PAGE_SIZE 4096
#define NUM_TXNS 1000

double get_time_diff(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
}

int cmp_double(const void *a, const void *b) {
    double da = *(const double*)a;
    double db = *(const double*)b;
    return (da > db) - (da < db);
}

void test_wal() {
    int fd_log = open("wal_log.bin", O_CREAT | O_RDWR | O_TRUNC, 0666);
    int fd_data = open("wal_data.bin", O_CREAT | O_RDWR | O_TRUNC, 0666);
    
    char buf[PAGE_SIZE];
    memset(buf, 0, PAGE_SIZE);
    for (int i = 0; i < 10; i++) {
        if (write(fd_data, buf, PAGE_SIZE) != PAGE_SIZE) {
            perror("write");
            exit(1);
        }
    }
    fdatasync(fd_data);

    double times[NUM_TXNS];

    for (int i = 0; i < NUM_TXNS; i++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        char log_entry[128];
        snprintf(log_entry, sizeof(log_entry), "TXN %d UPDATE PAGE 5", i);
        if (write(fd_log, log_entry, sizeof(log_entry)) < 0) {
            perror("write");
            exit(1);
        }
        fdatasync(fd_log);

        if (pwrite(fd_data, buf, PAGE_SIZE, 5 * PAGE_SIZE) != PAGE_SIZE) {
            perror("pwrite");
            exit(1);
        }
        // WAL allows delaying data fdatasync()

        clock_gettime(CLOCK_MONOTONIC, &end);
        times[i] = get_time_diff(start, end);
    }

    qsort(times, NUM_TXNS, sizeof(double), cmp_double);
    printf("WAL (log sync only)      median commit latency: %.6f seconds\n", times[NUM_TXNS/2]);

    close(fd_log);
    close(fd_data);
    unlink("wal_log.bin");
    unlink("wal_data.bin");
}

void test_shadow_paging() {
    int fd_data = open("shadow_data.bin", O_CREAT | O_RDWR | O_TRUNC, 0666);
    
    char buf[PAGE_SIZE];
    memset(buf, 0, PAGE_SIZE);
    for (int i = 0; i < 20; i++) {
        if (write(fd_data, buf, PAGE_SIZE) != PAGE_SIZE) {
            perror("write");
            exit(1);
        }
    }
    fdatasync(fd_data);

    double times[NUM_TXNS];
    int next_free_page = 10;
    int root_ptr_offset = 0;

    for (int i = 0; i < NUM_TXNS; i++) {
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        if (pwrite(fd_data, buf, PAGE_SIZE, next_free_page * PAGE_SIZE) != PAGE_SIZE) {
            perror("pwrite");
            exit(1);
        }
        fdatasync(fd_data);

        char ptr_buf[64];
        snprintf(ptr_buf, sizeof(ptr_buf), "ROOT -> %d", next_free_page);
        if (pwrite(fd_data, ptr_buf, sizeof(ptr_buf), root_ptr_offset) < 0) {
            perror("pwrite");
            exit(1);
        }
        fdatasync(fd_data);

        next_free_page++;

        clock_gettime(CLOCK_MONOTONIC, &end);
        times[i] = get_time_diff(start, end);
    }

    qsort(times, NUM_TXNS, sizeof(double), cmp_double);
    printf("Shadow Paging (2 syncs)  median commit latency: %.6f seconds\n", times[NUM_TXNS/2]);

    close(fd_data);
    unlink("shadow_data.bin");
}

int main() {
    printf("Measuring WAL vs Shadow Paging Commit Latencies (%d iterations)...\n", NUM_TXNS);
    test_wal();
    test_shadow_paging();
    return 0;
}
