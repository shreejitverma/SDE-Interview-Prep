#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE 4096
#define NUM_PAGES 10000

int main() {
    size_t size = NUM_PAGES * PAGE_SIZE;
    void *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // Fill with a specific pattern so pages are identical
    for (int i = 0; i < NUM_PAGES; i++) {
        memset((char*)ptr + i * PAGE_SIZE, 0xAB, PAGE_SIZE);
    }

    // Mark as mergeable
    if (madvise(ptr, size, MADV_MERGEABLE) != 0) {
        perror("madvise");
        return 1;
    }

    printf("Allocated and marked %zu bytes as mergeable.\n", size);
    
    // Wait for KSM to merge
    sleep(60);

    munmap(ptr, size);
    return 0;
}
