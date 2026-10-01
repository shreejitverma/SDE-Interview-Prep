#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define CHUNK_SIZE 8192

long get_nanos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000L + ts.tv_nsec;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <rw|sendfile>\n", argv[0]);
        return 1;
    }
    const char *mode = argv[1];
    
    int sv[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) != 0) return 1;
    
    int fd = open("testdata.bin", O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) return 1;
    if (ftruncate(fd, 100 * 1024 * 1024) != 0) return 1; // 100 MB
    
    pid_t pid = fork();
    if (pid < 0) return 1;
    
    if (pid == 0) {
        close(sv[0]);
        char buf[CHUNK_SIZE];
        while (read(sv[1], buf, sizeof(buf)) > 0) {
            // Discard
        }
        close(sv[1]);
        exit(0);
    } else {
        close(sv[1]);
        struct stat st;
        fstat(fd, &st);
        
        long start = get_nanos();
        
        if (strcmp(mode, "rw") == 0) {
            char buf[CHUNK_SIZE];
            off_t offset = 0;
            while (offset < st.st_size) {
                ssize_t n = pread(fd, buf, CHUNK_SIZE, offset);
                if (n <= 0) break;
                ssize_t written = 0;
                while (written < n) {
                    ssize_t w = write(sv[0], buf + written, n - written);
                    if (w <= 0) break;
                    written += w;
                }
                offset += n;
            }
        } else if (strcmp(mode, "sendfile") == 0) {
            off_t offset = 0;
            while (offset < st.st_size) {
                ssize_t n = sendfile(sv[0], fd, &offset, st.st_size - offset);
                if (n <= 0) break;
            }
        }
        long end = get_nanos();
        
        close(sv[0]);
        wait(NULL);
        
        printf("%s 100MB: %ld ms\n", mode, (end - start) / 1000000);
        close(fd);
        unlink("testdata.bin");
    }
    return 0;
}
