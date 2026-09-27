/*
 * mmap_shared_memory.c
 * Demonstrates shared memory via mmap (anonymous and file-backed),
 * POSIX shared memory (shm_open), and System V shared memory (shmget).
 *
 * Compile (Linux): gcc -Wall -O2 -o mmap_shmem mmap_shared_memory.c -lpthread -lrt
 * Compile (macOS): gcc -Wall -O2 -o mmap_shmem mmap_shared_memory.c -lpthread
 * Run:             ./mmap_shmem
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 1: Anonymous mmap (parent-child only)                     */
/* ──────────────────────────────────────────────────────────────────── */

void demo_anonymous_mmap(void) {
    printf("\n=== Demo 1: Anonymous mmap (parent-child) ===\n");

    /* Shared counter in anonymous mapping */
    typedef struct {
        int            value;
        pthread_mutex_t lock;
    } SharedCounter;

    SharedCounter *sc = mmap(
        NULL,                              /* kernel chooses address */
        sizeof(SharedCounter),             /* size */
        PROT_READ | PROT_WRITE,            /* permissions */
        MAP_SHARED | MAP_ANONYMOUS,        /* shared, anonymous (no file) */
        -1,                               /* fd (-1 for anonymous) */
        0                                 /* offset */
    );

    if (sc == MAP_FAILED) { perror("mmap"); exit(1); }

    sc->value = 0;
    /* Initialize mutex for cross-process sharing */
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&sc->lock, &attr);
    pthread_mutexattr_destroy(&attr);

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: increment counter 1000 times */
        for (int i = 0; i < 1000; i++) {
            pthread_mutex_lock(&sc->lock);
            sc->value++;
            pthread_mutex_unlock(&sc->lock);
        }
        printf("[Child] Final value after my increments: %d\n", sc->value);
        exit(0);
    } else {
        /* Parent: increment counter 1000 times concurrently */
        for (int i = 0; i < 1000; i++) {
            pthread_mutex_lock(&sc->lock);
            sc->value++;
            pthread_mutex_unlock(&sc->lock);
        }
        wait(NULL);  /* Wait for child */
        printf("[Parent] Final shared counter: %d (expected 2000)\n", sc->value);
    }

    pthread_mutex_destroy(&sc->lock);
    munmap(sc, sizeof(SharedCounter));
}

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 2: File-backed mmap (any process can open the file)       */
/* ──────────────────────────────────────────────────────────────────── */

void demo_file_backed_mmap(void) {
    printf("\n=== Demo 2: File-backed mmap ===\n");

    const char *filename = "/tmp/gios_mmap_demo";
    const size_t file_size = 4096;  /* One page */

    /* Create and size the backing file */
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
    if (fd < 0) { perror("open"); return; }

    /* Extend file to desired size */
    if (ftruncate(fd, file_size) < 0) { perror("ftruncate"); return; }

    /* Map the file into memory */
    char *mem = mmap(NULL, file_size, PROT_READ | PROT_WRITE,
                     MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED) { perror("mmap"); return; }
    close(fd);  /* Can close fd after mmap - mapping persists */

    /* Write to memory → writes to file */
    snprintf(mem, file_size, "Hello from mmap! PID=%d time=%ld\n",
             getpid(), time(NULL));
    printf("Wrote to mmap: %s", mem);

    /* Ensure changes are flushed to file (otherwise OS may delay) */
    if (msync(mem, file_size, MS_SYNC) < 0)
        perror("msync");

    /* Verify by reading the file directly */
    char buf[256];
    fd = open(filename, O_RDONLY);
    read(fd, buf, sizeof(buf));
    close(fd);
    printf("Read from file: %s", buf);

    /* mmap flags demo */
    printf("\nUseful mmap flags:\n");
    printf("  MAP_SHARED   - writes visible to other mappings (and file)\n");
    printf("  MAP_PRIVATE  - copy-on-write, writes NOT visible to others\n");
    printf("  MAP_ANONYMOUS- no backing file (MAP_ANON on macOS)\n");
    printf("  MAP_FIXED    - map at exact address (dangerous!)\n");
    printf("  MAP_POPULATE - fault in pages immediately (prefault)\n");
    printf("  MAP_LOCKED   - lock pages in RAM (no swap, needs CAP_IPC_LOCK)\n");
    printf("  MAP_HUGETLB  - use huge pages (2MB/1GB) for TLB efficiency\n");

    munmap(mem, file_size);
    unlink(filename);
}

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 3: POSIX Shared Memory (shm_open)                        */
/* ──────────────────────────────────────────────────────────────────── */

#define SHM_NAME  "/gios_demo_shm"
#define SHM_SIZE  (4096)

/* Shared memory layout */
typedef struct {
    sem_t   mutex;
    sem_t   data_ready;
    int     message_count;
    char    message[256];
    int     done;
} SharedRegion;

void posix_shm_server(void) {
    /* Create shared memory */
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if (fd < 0) { perror("shm_open"); exit(1); }

    ftruncate(fd, SHM_SIZE);

    SharedRegion *sr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);
    close(fd);

    if (sr == MAP_FAILED) { perror("mmap"); exit(1); }

    /* Initialize semaphores for inter-process use */
    sem_init(&sr->mutex,      1, 1);  /* 1 = process-shared, initial=1 */
    sem_init(&sr->data_ready, 1, 0);
    sr->message_count = 0;
    sr->done = 0;

    printf("[Server] Shared memory created: %s (%d bytes)\n",
           SHM_NAME, SHM_SIZE);

    /* Send 5 messages */
    for (int i = 1; i <= 5; i++) {
        sem_wait(&sr->mutex);
        snprintf(sr->message, sizeof(sr->message),
                 "Message #%d from server PID %d", i, getpid());
        sr->message_count = i;
        sem_post(&sr->mutex);

        sem_post(&sr->data_ready);  /* Signal client: new data */
        printf("[Server] Sent: %s\n", sr->message);
        usleep(100000);  /* 100ms between messages */
    }

    /* Signal client we're done */
    sem_wait(&sr->mutex);
    sr->done = 1;
    sem_post(&sr->mutex);
    sem_post(&sr->data_ready);

    /* Wait a bit for client to process */
    sleep(1);

    /* Cleanup */
    sem_destroy(&sr->mutex);
    sem_destroy(&sr->data_ready);
    munmap(sr, SHM_SIZE);
    shm_unlink(SHM_NAME);  /* Remove from /dev/shm */
    printf("[Server] Cleaned up shared memory\n");
}

void posix_shm_client(void) {
    /* Open existing shared memory */
    int fd = shm_open(SHM_NAME, O_RDWR, 0);
    if (fd < 0) { perror("shm_open (client)"); exit(1); }

    SharedRegion *sr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);
    close(fd);

    if (sr == MAP_FAILED) { perror("mmap (client)"); exit(1); }

    printf("[Client] Opened shared memory, waiting for messages...\n");

    while (1) {
        sem_wait(&sr->data_ready);  /* Wait for new data */

        sem_wait(&sr->mutex);
        int done = sr->done;
        if (!done) {
            printf("[Client] Received: %s\n", sr->message);
        }
        sem_post(&sr->mutex);

        if (done) break;
    }

    munmap(sr, SHM_SIZE);
    printf("[Client] Done.\n");
}

void demo_posix_shm(void) {
    printf("\n=== Demo 3: POSIX Shared Memory (shm_open) ===\n");

    pid_t pid = fork();
    if (pid == 0) {
        usleep(50000);  /* Let server create shared mem first */
        posix_shm_client();
        exit(0);
    } else {
        posix_shm_server();
        wait(NULL);
    }
}

/* ──────────────────────────────────────────────────────────────────── */
/*  APPROACH 4: System V Shared Memory (shmget/shmat)                 */
/* ──────────────────────────────────────────────────────────────────── */

void demo_sysv_shm(void) {
    printf("\n=== Demo 4: System V Shared Memory (shmget) ===\n");

    const int SHM_KEY = 0xBEEF1234;
    const int SHM_SZ  = 4096;

    /* Create or open shared memory segment */
    int shmid = shmget(SHM_KEY, SHM_SZ, IPC_CREAT | 0666);
    if (shmid < 0) { perror("shmget"); return; }

    printf("Created SysV SHM: id=%d key=0x%x size=%d\n",
           shmid, SHM_KEY, SHM_SZ);

    /* Attach to our address space */
    char *mem = shmat(shmid, NULL, 0);  /* NULL = kernel picks address */
    if (mem == (char *)-1) { perror("shmat"); return; }

    pid_t pid = fork();
    if (pid == 0) {
        /* Child: read message from shared mem */
        usleep(10000);  /* Let parent write first */
        printf("[Child] Read from SysV SHM: '%s'\n", mem);

        /* Write reply */
        strncpy(mem, "Reply from child!", SHM_SZ);

        /* Detach */
        shmdt(mem);
        exit(0);
    } else {
        /* Parent: write to shared mem */
        strncpy(mem, "Hello from parent via SysV SHM!", SHM_SZ);
        printf("[Parent] Wrote to SHM, waiting for child...\n");

        wait(NULL);

        printf("[Parent] Child wrote: '%s'\n", mem);

        /* Detach and destroy */
        shmdt(mem);
        shmctl(shmid, IPC_RMID, NULL);  /* Remove the segment */
        printf("[Parent] Deleted SysV SHM segment\n");
    }
}

/* ──────────────────────────────────────────────────────────────────── */
/*  mmap Performance: Zero-copy file I/O                               */
/* ──────────────────────────────────────────────────────────────────── */

void demo_mmap_zero_copy(void) {
    printf("\n=== Demo 5: mmap Zero-Copy vs read() Benchmark ===\n");

    const char *filename = "/tmp/gios_bench_file";
    const size_t file_size = 64 * 1024 * 1024;  /* 64 MB */

    /* Create test file */
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
    ftruncate(fd, file_size);

    /* Fill with known pattern */
    char *wbuf = malloc(4096);
    memset(wbuf, 0xAB, 4096);
    for (size_t i = 0; i < file_size / 4096; i++)
        write(fd, wbuf, 4096);
    free(wbuf);

    struct timespec t0, t1;
    long sum;

    /* ── read() approach ─────────────────────── */
    {
        lseek(fd, 0, SEEK_SET);
        char *rbuf = malloc(65536);
        sum = 0;

        clock_gettime(CLOCK_MONOTONIC, &t0);
        ssize_t n;
        while ((n = read(fd, rbuf, 65536)) > 0) {
            for (ssize_t i = 0; i < n; i++) sum += rbuf[i];
        }
        clock_gettime(CLOCK_MONOTONIC, &t1);

        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        printf("read()  64MB: %.3f s (%.0f MB/s) sum=%ld\n",
               elapsed, file_size / elapsed / 1e6, sum);
        free(rbuf);
    }

    /* ── mmap approach ──────────────────────── */
    {
        char *mem = mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
        if (mem == MAP_FAILED) { perror("mmap"); goto cleanup; }

        /* Advise kernel about access pattern */
        madvise(mem, file_size, MADV_SEQUENTIAL);

        sum = 0;
        clock_gettime(CLOCK_MONOTONIC, &t0);
        for (size_t i = 0; i < file_size; i++) sum += mem[i];
        clock_gettime(CLOCK_MONOTONIC, &t1);

        double elapsed = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;
        printf("mmap()  64MB: %.3f s (%.0f MB/s) sum=%ld\n",
               elapsed, file_size / elapsed / 1e6, sum);
        munmap(mem, file_size);
    }

cleanup:
    close(fd);
    unlink(filename);

    printf("\nmmap advantages:\n");
    printf("  - No extra copy from kernel buffer to user buffer\n");
    printf("  - Pages shared with page cache (no double-buffering)\n");
    printf("  - OS handles read-ahead automatically\n");
    printf("  - Allows random access without seek()\n");
    printf("  - sendfile(2) for network: even more zero-copy!\n");
}

/* ──────────────────────────────────────────────────────────────────── */
/*  Linux /proc inspection of mappings                                 */
/* ──────────────────────────────────────────────────────────────────── */

void show_memory_maps(void) {
    printf("\n=== My /proc/self/maps ===\n");
    FILE *f = fopen("/proc/self/maps", "r");
    if (!f) { printf("Not on Linux\n"); return; }

    char line[512];
    int count = 0;
    while (fgets(line, sizeof(line), f) && count++ < 20)
        printf("%s", line);
    if (count >= 20)
        printf("... (truncated, run: cat /proc/self/maps)\n");
    fclose(f);
}

int main(void) {
    printf("=== mmap and Shared Memory Demos ===\n");
    printf("PID: %d\n", getpid());

    demo_anonymous_mmap();
    demo_file_backed_mmap();
    demo_posix_shm();
    demo_sysv_shm();
    demo_mmap_zero_copy();
    show_memory_maps();

    printf("\n=== Quick Reference ===\n");
    printf("POSIX SHM location: ls /dev/shm/\n");
    printf("SysV SHM inspect:   ipcs -m\n");
    printf("SysV SHM delete:    ipcrm -m <shmid>\n");
    printf("Process maps:       cat /proc/<pid>/maps\n");
    printf("Detailed smaps:     cat /proc/<pid>/smaps\n");
    printf("pmap output:        pmap -x <pid>\n");

    return 0;
}
