#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <time.h>
#include <stdatomic.h>
#include <linux/futex.h>
#include <sys/syscall.h>

#define MAX_MSG_SIZE 65536
#define ITERATIONS 10000

struct shm_data {
    atomic_int turn; // 0 for parent, 1 for child
    char buffer[MAX_MSG_SIZE];
};

static void futex_wait(atomic_int *futex_word, int expected) {
    while (atomic_load_explicit(futex_word, memory_order_acquire) == expected) {
        syscall(SYS_futex, (int*)futex_word, FUTEX_WAIT, expected, NULL, NULL, 0);
    }
}

static void futex_wake(atomic_int *futex_word) {
    syscall(SYS_futex, (int*)futex_word, FUTEX_WAKE, 1, NULL, NULL, 0);
}

long get_nanos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int cmp_long(const void *a, const void *b) {
    long la = *(const long*)a;
    long lb = *(const long*)b;
    return (la > lb) - (la < lb);
}

ssize_t read_all(int fd, char *buf, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = read(fd, buf + total, size - total);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

ssize_t write_all(int fd, const char *buf, size_t size) {
    size_t total = 0;
    while (total < size) {
        ssize_t n = write(fd, buf + total, size - total);
        if (n <= 0) return -1;
        total += n;
    }
    return total;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <pipe|unix|shm> <msg_size>\n", argv[0]);
        return 1;
    }

    const char *mode = argv[1];
    int msg_size = atoi(argv[2]);
    if (msg_size > MAX_MSG_SIZE || msg_size <= 0) {
        fprintf(stderr, "Invalid message size. Max %d\n", MAX_MSG_SIZE);
        return 1;
    }

    int p2c[2], c2p[2];
    int sv[2];
    struct shm_data *shm = NULL;

    if (strcmp(mode, "pipe") == 0) {
        if (pipe(p2c) != 0 || pipe(c2p) != 0) return 1;
    } else if (strcmp(mode, "unix") == 0) {
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) return 1;
    } else if (strcmp(mode, "shm") == 0) {
        shm = mmap(NULL, sizeof(struct shm_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (shm == MAP_FAILED) return 1;
        atomic_init(&shm->turn, 0);
    } else {
        fprintf(stderr, "Unknown mode\n");
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        // Child
        char *buf = malloc(msg_size);
        for (int i = 0; i < ITERATIONS; i++) {
            if (strcmp(mode, "pipe") == 0) {
                read_all(p2c[0], buf, msg_size);
                write_all(c2p[1], buf, msg_size);
            } else if (strcmp(mode, "unix") == 0) {
                read_all(sv[1], buf, msg_size);
                write_all(sv[1], buf, msg_size);
            } else if (strcmp(mode, "shm") == 0) {
                futex_wait(&shm->turn, 0);
                memcpy(buf, (void*)shm->buffer, msg_size);
                atomic_store_explicit(&shm->turn, 0, memory_order_release);
                futex_wake(&shm->turn);
            }
        }
        free(buf);
        exit(0);
    } else {
        // Parent
        char *buf = malloc(msg_size);
        memset(buf, 'A', msg_size);
        long *latencies = malloc(ITERATIONS * sizeof(long));

        for (int i = 0; i < ITERATIONS; i++) {
            long start = get_nanos();
            if (strcmp(mode, "pipe") == 0) {
                write_all(p2c[1], buf, msg_size);
                read_all(c2p[0], buf, msg_size);
            } else if (strcmp(mode, "unix") == 0) {
                write_all(sv[0], buf, msg_size);
                read_all(sv[0], buf, msg_size);
            } else if (strcmp(mode, "shm") == 0) {
                memcpy((void*)shm->buffer, buf, msg_size);
                atomic_store_explicit(&shm->turn, 1, memory_order_release);
                futex_wake(&shm->turn);
                futex_wait(&shm->turn, 1);
            }
            long end = get_nanos();
            latencies[i] = end - start;
        }
        wait(NULL);

        qsort(latencies, ITERATIONS, sizeof(long), cmp_long);
        printf("%s %d bytes: median RTT = %ld ns\n", mode, msg_size, latencies[ITERATIONS/2]);
        
        free(buf);
        free(latencies);
    }

    return 0;
}
