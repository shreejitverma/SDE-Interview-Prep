#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>
#include <ucontext.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <stdint.h>
#include <time.h>

#define PAGE_SIZE 4096

typedef struct {
    uint16_t offset;
    uint8_t value;
} DiffEntry;

#define MAX_DIFFS 1024

typedef struct {
    int count;
    DiffEntry entries[MAX_DIFFS];
} DiffPacket;

void *shared_page;
void *twin_page;
int is_writable = 0;
int dsm_socket;
const char* node_name = "Unknown";

void sigsegv_handler(int sig, siginfo_t *si, void *unused) {
    (void)sig;
    (void)unused;
    uint8_t *fault_addr = (uint8_t *)si->si_addr;
    uint8_t *page_start = (uint8_t *)shared_page;

    if (fault_addr >= page_start && fault_addr < page_start + PAGE_SIZE) {
        printf("[%s] SIGSEGV trapped at offset %ld. Creating twin and unprotecting page.\n", node_name, fault_addr - page_start);
        memcpy(twin_page, shared_page, PAGE_SIZE);
        if (mprotect(shared_page, PAGE_SIZE, PROT_READ | PROT_WRITE) == -1) {
            perror("mprotect");
            exit(1);
        }
        is_writable = 1;
    } else {
        fprintf(stderr, "[%s] Real SIGSEGV at %p\n", node_name, fault_addr);
        exit(1);
    }
}

void dsm_init(int sock, const char* name) {
    dsm_socket = sock;
    node_name = name;
    shared_page = mmap(NULL, PAGE_SIZE, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    twin_page = malloc(PAGE_SIZE);

    if (shared_page == MAP_FAILED || twin_page == NULL) {
        perror("mmap/malloc");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigsegv_handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void dsm_release() {
    if (!is_writable) {
        printf("[%s] dsm_release(): Page not modified.\n", node_name);
        return;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    DiffPacket packet;
    packet.count = 0;

    uint8_t *shared = (uint8_t *)shared_page;
    uint8_t *twin = (uint8_t *)twin_page;

    for (int i = 0; i < PAGE_SIZE; i++) {
        if (shared[i] != twin[i]) {
            if (packet.count < MAX_DIFFS) {
                packet.entries[packet.count].offset = i;
                packet.entries[packet.count].value = shared[i];
                packet.count++;
            }
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    long diff_time = (end.tv_sec - start.tv_sec) * 1000000000L + (end.tv_nsec - start.tv_nsec);

    printf("[%s] Computed diff in %ld ns. %d modifications found.\n", node_name, diff_time, packet.count);

    size_t packet_size = sizeof(int) + packet.count * sizeof(DiffEntry);
    if (write(dsm_socket, &packet, packet_size) < 0) {
        perror("write");
        exit(1);
    }

    if (mprotect(shared_page, PAGE_SIZE, PROT_READ) == -1) {
        perror("mprotect");
        exit(1);
    }
    is_writable = 0;
}

void dsm_acquire() {
    DiffPacket packet;
    int n = read(dsm_socket, &packet.count, sizeof(int));
    if (n <= 0) return;

    size_t entries_size = packet.count * sizeof(DiffEntry);
    size_t read_bytes = 0;
    while (read_bytes < entries_size) {
        n = read(dsm_socket, ((char*)packet.entries) + read_bytes, entries_size - read_bytes);
        if (n <= 0) break;
        read_bytes += n;
    }

    printf("[%s] LRC pull: Received diff with %d modifications. Applying to local page.\n", node_name, packet.count);
    
    if (mprotect(shared_page, PAGE_SIZE, PROT_READ | PROT_WRITE) == -1) {
        perror("mprotect");
        exit(1);
    }
    uint8_t *shared = (uint8_t *)shared_page;
    for (int i = 0; i < packet.count; i++) {
        shared[packet.entries[i].offset] = packet.entries[i].value;
    }
    if (mprotect(shared_page, PAGE_SIZE, PROT_READ) == -1) {
        perror("mprotect");
        exit(1);
    }
}

int main() {
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {
        perror("socketpair");
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        close(sv[0]);
        dsm_init(sv[1], "Node B");

        printf("[Node B] Initial state: offset 10 = %d, offset 500 = %d\n", ((uint8_t*)shared_page)[10], ((uint8_t*)shared_page)[500]);
        
        printf("[Node B] Writing 42 to offset 500...\n");
        ((uint8_t*)shared_page)[500] = 42; 
        
        dsm_release();
        dsm_acquire();
        
        printf("[Node B] Final state: offset 10 = %d, offset 500 = %d\n", ((uint8_t*)shared_page)[10], ((uint8_t*)shared_page)[500]);
        
        close(sv[1]);
        exit(0);
    } else {
        close(sv[1]);
        dsm_init(sv[0], "Node A");

        // Small sleep to ensure deterministic output order if possible, though not strictly guaranteed.
        usleep(10000); 

        printf("[Node A] Initial state: offset 10 = %d, offset 500 = %d\n", ((uint8_t*)shared_page)[10], ((uint8_t*)shared_page)[500]);

        printf("[Node A] Writing 99 to offset 10...\n");
        ((uint8_t*)shared_page)[10] = 99; 
        
        dsm_release();
        dsm_acquire();
        
        // Wait for child to finish to avoid mixed final output
        wait(NULL);
        printf("[Node A] Final state: offset 10 = %d, offset 500 = %d\n", ((uint8_t*)shared_page)[10], ((uint8_t*)shared_page)[500]);
        
        close(sv[0]);
    }
    
    return 0;
}
