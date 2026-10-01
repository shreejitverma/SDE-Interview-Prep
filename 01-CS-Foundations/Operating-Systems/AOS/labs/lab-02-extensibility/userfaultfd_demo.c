#define _GNU_SOURCE
#include <inttypes.h>
#include <sys/types.h>
#include <stdio.h>
#include <linux/userfaultfd.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <time.h>

static int page_size;

static void *fault_handler_thread(void *arg) {
    long uffd = (long)arg;
    struct pollfd evt;
    int nready;
    struct uffd_msg msg;
    struct uffdio_copy uffdio_copy;
    ssize_t readval;
    char *page = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (page == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    evt.fd = uffd;
    evt.events = POLLIN;

    printf("[uffd] Thread started\n");
    fflush(stdout);

    while (1) {
        nready = poll(&evt, 1, -1);
        if (nready == -1) {
            perror("poll");
            exit(1);
        }
        printf("[uffd] poll returned %d, revents=0x%x\n", nready, evt.revents);
        fflush(stdout);

        readval = read(uffd, &msg, sizeof(msg));
        if (readval == 0) {
            printf("EOF on userfaultfd!\n");
            exit(1);
        }
        if (readval == -1) {
            if (errno == EAGAIN) continue;
            perror("read");
            exit(1);
        }

        if (msg.event != UFFD_EVENT_PAGEFAULT) {
            fprintf(stderr, "Unexpected event on userfaultfd\n");
            exit(1);
        }

        printf("[uffd] Page fault at address: 0x%llx\n", (unsigned long long)msg.arg.pagefault.address);
        fflush(stdout);
        
        sprintf(page, "Hello from Exokernel userfaultfd handler! (Address: 0x%llx)", (unsigned long long)msg.arg.pagefault.address);

        uffdio_copy.src = (unsigned long)page;
        uffdio_copy.dst = (unsigned long)msg.arg.pagefault.address & ~(page_size - 1);
        uffdio_copy.len = page_size;
        uffdio_copy.mode = 0;
        uffdio_copy.copy = 0;

        if (ioctl(uffd, UFFDIO_COPY, &uffdio_copy) == -1) {
            perror("ioctl-UFFDIO_COPY");
            exit(1);
        }

        printf("[uffd] Page mapped successfully.\n");
    }
    return NULL;
}

int main(void) {
    long uffd;
    char *addr;
    unsigned long len;
    pthread_t thr;
    struct uffdio_api uffdio_api;
    struct uffdio_register uffdio_register;

    page_size = sysconf(_SC_PAGE_SIZE);
    len = 4 * page_size;

    uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
    if (uffd == -1) {
        perror("syscall/userfaultfd");
        fprintf(stderr, "Hint: Run with sudo if unprivileged_userfaultfd is disabled.\n");
        exit(1);
    }

    uffdio_api.api = UFFD_API;
    uffdio_api.features = 0;
    if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1) {
        perror("ioctl-UFFDIO_API");
        exit(1);
    }

    addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    printf("Mapped %lu bytes at %p\n", len, addr);

    uffdio_register.range.start = (unsigned long)addr;
    uffdio_register.range.len = len;
    uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
    if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1) {
        perror("ioctl-UFFDIO_REGISTER");
        exit(1);
    }

    if (pthread_create(&thr, NULL, fault_handler_thread, (void *)uffd)) {
        perror("pthread_create");
        exit(1);
    }

    usleep(100000);

    printf("Accessing memory at %p...\n", addr);
    fflush(stdout);
    addr[0] = 'X'; // trigger fault with a write
    printf("Read value: '%s'\n", addr); 
    
    printf("Accessing memory at %p...\n", addr + page_size);
    fflush(stdout);
    addr[page_size] = 'Y';
    printf("Read value: '%s'\n", addr + page_size);

    return 0;
}
