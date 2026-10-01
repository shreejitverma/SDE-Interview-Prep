#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define PAGEMAP_ENTRY 8
#define GET_BIT(X,Y) (((X) & ((uint64_t)1<<(Y))) >> (Y))
#define GET_PFN(X) ((X) & 0x7FFFFFFFFFFFFFFF)

int main(void) {
    size_t page_size = (size_t)sysconf(_SC_PAGESIZE);
    printf("Page size: %zu bytes\n", page_size);

    char *ptr = mmap(NULL, page_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    int pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
    if (pagemap_fd < 0) {
        perror("open pagemap");
        return 1;
    }

    uint64_t addr = (uint64_t)ptr;
    off_t offset = (addr / page_size) * PAGEMAP_ENTRY;

    uint64_t entry;
    if (pread(pagemap_fd, &entry, PAGEMAP_ENTRY, offset) != PAGEMAP_ENTRY) {
        perror("pread");
        return 1;
    }

    printf("Before touch:\n");
    printf("  Virtual Address: 0x%lx\n", addr);
    printf("  Present: %lu\n", GET_BIT(entry, 63));
    printf("  Swapped: %lu\n", GET_BIT(entry, 62));
    printf("  PFN: 0x%lx\n", GET_PFN(entry));

    // Touch the page
    ptr[0] = 'A';

    if (pread(pagemap_fd, &entry, PAGEMAP_ENTRY, offset) != PAGEMAP_ENTRY) {
        perror("pread");
        return 1;
    }

    printf("After touch:\n");
    printf("  Present: %lu\n", GET_BIT(entry, 63));
    printf("  Swapped: %lu\n", GET_BIT(entry, 62));
    printf("  PFN: 0x%lx\n", GET_PFN(entry));

    close(pagemap_fd);
    munmap(ptr, page_size);
    return 0;
}
