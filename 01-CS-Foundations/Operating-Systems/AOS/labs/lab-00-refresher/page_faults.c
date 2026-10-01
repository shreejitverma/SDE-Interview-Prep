#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void print_faults(struct rusage *start, struct rusage *end, const char *msg) {
    long minor = end->ru_minflt - start->ru_minflt;
    long major = end->ru_majflt - start->ru_majflt;
    printf("%s:\n", msg);
    printf("  Minor page faults: %ld\n", minor);
    printf("  Major page faults: %ld\n", major);
}

int main(void) {
    size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
    size_t alloc_size = 10 * 1024 * 1024; // 10 MB
    struct rusage start, end;

    printf("--- Minor Faults Demonstration ---\n");
    getrusage(RUSAGE_SELF, &start);
    char *anon_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (anon_ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    getrusage(RUSAGE_SELF, &end);
    print_faults(&start, &end, "After mmap anonymous");

    getrusage(RUSAGE_SELF, &start);
    for (size_t i = 0; i < alloc_size; i += page_size) {
        anon_ptr[i] = 'A';
    }
    getrusage(RUSAGE_SELF, &end);
    print_faults(&start, &end, "After touching anonymous pages");
    munmap(anon_ptr, alloc_size);

    printf("\n--- Major Faults Demonstration ---\n");
    int fd = open("/var/tmp/fault_test.dat", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    if (ftruncate(fd, alloc_size) < 0) {
        perror("ftruncate");
        return 1;
    }
    
    char *file_ptr = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (file_ptr == MAP_FAILED) {
        perror("mmap file");
        return 1;
    }
    
    // Touch to fault in and populate page cache
    for (size_t i = 0; i < alloc_size; i += page_size) {
        file_ptr[i] = 'B';
    }
    
    // Write out dirty pages so they can be dropped from cache
    msync(file_ptr, alloc_size, MS_SYNC);
    
    // Tell kernel we don't need this, dropping from page cache
    printf("  Dropping caches via sudo...\n");
    if (system("sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'") < 0) {
        perror("system");
    }
    
    // Also advise we don't need it so PTEs are dropped
    madvise(file_ptr, alloc_size, MADV_DONTNEED);
    
    // Disable readahead to force major faults on our reads
    madvise(file_ptr, alloc_size, MADV_RANDOM);
    
    getrusage(RUSAGE_SELF, &start);
    volatile char sum = 0;
    for (size_t i = 0; i < alloc_size; i += page_size) {
        sum += file_ptr[i];
    }
    getrusage(RUSAGE_SELF, &end);
    print_faults(&start, &end, "After reading dropped file pages");
    
    munmap(file_ptr, alloc_size);
    close(fd);
    unlink("/var/tmp/fault_test.dat");

    return 0;
}
