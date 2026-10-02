#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include <sched.h>
#include <string.h>
#include <errno.h>

#define N_SAMPLES 1000
#define INTERVAL_NS 1000000 // 1 ms

struct sched_attr {
    uint32_t size;
    uint32_t sched_policy;
    uint64_t sched_flags;
    int32_t sched_nice;
    uint32_t sched_priority;
    uint64_t sched_runtime;
    uint64_t sched_deadline;
    uint64_t sched_period;
};

#include <sys/syscall.h>
int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(SYS_sched_setattr, pid, attr, flags);
}

static inline void timespec_add_ns(struct timespec *ts, uint64_t ns) {
    ts->tv_nsec += ns;
    while (ts->tv_nsec >= 1000000000) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000;
    }
}

static inline int64_t timespec_diff_ns(struct timespec *a, struct timespec *b) {
    return (a->tv_sec - b->tv_sec) * 1000000000LL + (a->tv_nsec - b->tv_nsec);
}

int cmp_int64(const void *a, const void *b) {
    int64_t arg1 = *(const int64_t*)a;
    int64_t arg2 = *(const int64_t*)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void print_stats(const char *name, int64_t *diffs, int n) {
    qsort(diffs, n, sizeof(int64_t), cmp_int64);
    int64_t min = diffs[0];
    int64_t max = diffs[n - 1];
    int64_t median = diffs[n / 2];
    int64_t sum = 0;
    for (int i = 0; i < n; i++) sum += diffs[i];
    int64_t avg = sum / n;
    printf("%-10s: min=%6ld ns, avg=%6ld ns, median=%6ld ns, max=%6ld ns\n", name, min, avg, median, max);
}

void test_oneshot() {
    struct timespec expected, now;
    int64_t diffs[N_SAMPLES];

    clock_gettime(CLOCK_MONOTONIC, &expected);
    timespec_add_ns(&expected, INTERVAL_NS);

    for (int i = 0; i < N_SAMPLES; i++) {
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &expected, NULL);
        clock_gettime(CLOCK_MONOTONIC, &now);
        diffs[i] = timespec_diff_ns(&now, &expected);
        timespec_add_ns(&expected, INTERVAL_NS);
    }
    print_stats("One-shot", diffs, N_SAMPLES);
}

void test_periodic() {
    struct timespec expected, now;
    int64_t diffs[N_SAMPLES];

    int fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (fd < 0) { perror("timerfd_create"); return; }

    clock_gettime(CLOCK_MONOTONIC, &expected);
    timespec_add_ns(&expected, INTERVAL_NS);

    struct itimerspec its;
    its.it_value = expected;
    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = INTERVAL_NS;

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &its, NULL) < 0) {
        perror("timerfd_settime");
        close(fd);
        return;
    }

    for (int i = 0; i < N_SAMPLES; i++) {
        uint64_t expirations;
        ssize_t s = read(fd, &expirations, sizeof(expirations));
        if (s != sizeof(expirations)) { perror("read"); break; }

        clock_gettime(CLOCK_MONOTONIC, &now);
        diffs[i] = timespec_diff_ns(&now, &expected);
        timespec_add_ns(&expected, INTERVAL_NS * expirations);
    }
    close(fd);
    print_stats("Periodic", diffs, N_SAMPLES);
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        if (strcmp(argv[1], "fifo") == 0) {
            struct sched_param param;
            param.sched_priority = 90;
            if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
                perror("sched_setscheduler(SCHED_FIFO)");
            }
        } else if (strcmp(argv[1], "deadline") == 0) {
            struct sched_attr attr;
            memset(&attr, 0, sizeof(attr));
            attr.size = sizeof(attr);
            attr.sched_policy = SCHED_DEADLINE;
            attr.sched_runtime  =  500 * 1000; // 500 us
            attr.sched_deadline = 1000 * 1000; // 1 ms
            attr.sched_period   = 1000 * 1000; // 1 ms
            if (sched_setattr(0, &attr, 0) == -1) {
                perror("sched_setattr(SCHED_DEADLINE)");
            }
        }
    }

    test_oneshot();
    test_periodic();

    return 0;
}
