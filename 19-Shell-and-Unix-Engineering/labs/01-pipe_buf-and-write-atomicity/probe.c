#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>

static int do_limits(const char *path) {
    long pc_val = -1;
    int fd = open(path ? path : ".", O_RDONLY);
    if (fd >= 0) {
        pc_val = fpathconf(fd, _PC_PIPE_BUF);
        close(fd);
    }
#ifdef _POSIX_PIPE_BUF
    printf("POSIX_PIPE_BUF=%d\n", _POSIX_PIPE_BUF);
#else
    printf("POSIX_PIPE_BUF=-1\n");
#endif
    printf("FPATHCONF_PIPE_BUF=%ld\n", pc_val);
    return 0;
}

static int do_concurrent(int sz, int count) {
    if (sz < 2 || count < 1) {
        fprintf(stderr, "invalid arguments\n");
        return 1;
    }
    int p[2];
    if (pipe(p) < 0) {
        perror("pipe");
        return 1;
    }

    pid_t p1 = fork();
    if (p1 < 0) { perror("fork"); return 1; }
    if (p1 == 0) {
        close(p[0]);
        char *buf = malloc((size_t)sz);
        if (!buf) _exit(2);
        memset(buf, 'A', (size_t)(sz - 1));
        buf[sz - 1] = '\n';
        for (int i = 0; i < count; i++) {
            size_t off = 0;
            while (off < (size_t)sz) {
                ssize_t n = write(p[1], buf + off, (size_t)sz - off);
                if (n < 0) {
                    if (errno == EINTR) continue;
                    break;
                }
                off += (size_t)n;
            }
        }
        free(buf);
        close(p[1]);
        _exit(0);
    }

    pid_t p2 = fork();
    if (p2 < 0) { perror("fork"); return 1; }
    if (p2 == 0) {
        close(p[0]);
        char *buf = malloc((size_t)sz);
        if (!buf) _exit(2);
        memset(buf, 'B', (size_t)(sz - 1));
        buf[sz - 1] = '\n';
        for (int i = 0; i < count; i++) {
            size_t off = 0;
            while (off < (size_t)sz) {
                ssize_t n = write(p[1], buf + off, (size_t)sz - off);
                if (n < 0) {
                    if (errno == EINTR) continue;
                    break;
                }
                off += (size_t)n;
            }
        }
        free(buf);
        close(p[1]);
        _exit(0);
    }

    close(p[1]);

    int corrupted = 0;
    int total = 0;
    char chunk[4096];
    size_t cap = 262144;
    char *line = malloc(cap);
    size_t line_len = 0;
    ssize_t n;

    while ((n = read(p[0], chunk, sizeof(chunk))) > 0) {
        for (ssize_t i = 0; i < n; i++) {
            if (chunk[i] == '\n') {
                line[line_len] = '\0';
                total++;
                if (strchr(line, 'A') && strchr(line, 'B')) {
                    corrupted++;
                }
                line_len = 0;
            } else {
                if (line_len + 2 >= cap) {
                    cap *= 2;
                    char *tmp = realloc(line, cap);
                    if (!tmp) {
                        free(line);
                        return 1;
                    }
                    line = tmp;
                }
                line[line_len++] = chunk[i];
            }
        }
    }
    close(p[0]);
    waitpid(p1, NULL, 0);
    waitpid(p2, NULL, 0);
    free(line);
    printf("total=%d corrupted=%d\n", total, corrupted);
    return 0;
}

/*
 * Leave a non-blocking pipe with free space below pipe_buf but above zero, then show that a
 * write of pipe_buf bytes is refused whole (EAGAIN) while a write of pipe_buf + 1 bytes on the
 * same pipe is accepted in part. A refused write transfers nothing, so the partial count is the
 * room the first write would have had: a kernel that split small writes would have used it.
 * Setup: fill with 1-byte writes until EAGAIN, drain pipe_buf bytes, then write 1 byte back.
 */
static int do_nonblock_atomic(int pipe_buf) {
    if (pipe_buf < 1) {
        fprintf(stderr, "invalid arguments\n");
        return 1;
    }
    int p[2];
    if (pipe(p) < 0) {
        perror("pipe");
        return 1;
    }
    int flags = fcntl(p[1], F_GETFL, 0);
    if (flags < 0 || fcntl(p[1], F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl");
        return 1;
    }

    char one = 'X';
    while (write(p[1], &one, 1) == 1) {
        /* fill until EAGAIN */
    }
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("fill");
        return 1;
    }

    char *buf = malloc((size_t)pipe_buf + 1);
    if (!buf) return 1;
    memset(buf, 'Z', (size_t)pipe_buf + 1);

    size_t drained = 0;
    while (drained < (size_t)pipe_buf) {
        ssize_t n = read(p[0], buf, (size_t)pipe_buf - drained);
        if (n <= 0) {
            perror("drain");
            free(buf);
            return 1;
        }
        drained += (size_t)n;
    }
    if (write(p[1], &one, 1) != 1) {
        perror("refill");
        free(buf);
        return 1;
    }

    errno = 0;
    ssize_t atomic_ret = write(p[1], buf, (size_t)pipe_buf);
    int atomic_err = errno;
    ssize_t big_ret = write(p[1], buf, (size_t)pipe_buf + 1);
    free(buf);
    close(p[0]);
    close(p[1]);

    printf("atomic_ret=%ld errno=%d big_ret=%ld\n", (long)atomic_ret, atomic_err, (long)big_ret);
    return 0;
}

static int do_fifo_nonblock_open(const char *path) {
    errno = 0;
    int fd = open(path, O_WRONLY | O_NONBLOCK);
    int err = errno;
    if (fd >= 0) close(fd);
    printf("fd=%d errno=%d\n", fd, err);
    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <limits|concurrent|nonblock-atomic|fifo-nonblock-open> [args...]\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "limits") == 0) {
        return do_limits(argc >= 3 ? argv[2] : NULL);
    } else if (strcmp(argv[1], "concurrent") == 0) {
        if (argc < 4) return 1;
        return do_concurrent(atoi(argv[2]), atoi(argv[3]));
    } else if (strcmp(argv[1], "nonblock-atomic") == 0) {
        if (argc < 3) return 1;
        return do_nonblock_atomic(atoi(argv[2]));
    } else if (strcmp(argv[1], "fifo-nonblock-open") == 0) {
        if (argc < 3) return 1;
        return do_fifo_nonblock_open(argv[2]);
    }
    return 1;
}
