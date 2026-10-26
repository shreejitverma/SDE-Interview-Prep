#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int probe_offset(const char *tempfile) {
  int fd1 = open(tempfile, O_CREAT | O_RDWR | O_TRUNC, 0600);
  if (fd1 < 0) {
    perror("open tempfile");
    return 1;
  }
  if (write(fd1, "0123456789", 10) != 10) {
    perror("write");
    close(fd1);
    return 1;
  }
  if (lseek(fd1, 5, SEEK_SET) != 5) {
    perror("lseek");
    close(fd1);
    return 1;
  }

  char path[64];
  snprintf(path, sizeof(path), "/dev/fd/%d", fd1);
  int fd2 = open(path, O_RDONLY);
  if (fd2 < 0) {
    perror("open /dev/fd/N");
    close(fd1);
    return 1;
  }

  char buf[3] = {0};
  ssize_t n = read(fd2, buf, 2);
  if (n != 2) {
    perror("read fd2");
    close(fd1);
    close(fd2);
    return 1;
  }

  close(fd1);
  close(fd2);
  unlink(tempfile);

  if (strcmp(buf, "01") == 0) {
    printf("offset: linux-new-ofd\n");
  } else if (strcmp(buf, "56") == 0) {
    printf("offset: bsd-dup-ofd\n");
  } else {
    printf("offset: unexpected-data %s\n", buf);
  }
  return 0;
}

static int probe_socket(void) {
  int sv[2];
  if (socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0) {
    perror("socketpair");
    return 1;
  }

  char path[64];
  snprintf(path, sizeof(path), "/dev/fd/%d", sv[0]);
  int fd = open(path, O_RDWR);
  if (fd >= 0) {
    printf("socket: dup-ok\n");
    close(fd);
  } else {
    if (errno == ENXIO) {
      printf("socket: enxio\n");
    } else {
      printf("socket: failed errno=%d\n", errno);
    }
  }

  close(sv[0]);
  close(sv[1]);
  return 0;
}

static int probe_closed(void) {
  /* Find an unused file descriptor or open and immediately close one */
  int fd = open("/dev/null", O_RDONLY);
  if (fd < 0) {
    perror("open /dev/null");
    return 1;
  }
  int target_fd = fd;
  close(fd);

  char path[64];
  snprintf(path, sizeof(path), "/dev/fd/%d", target_fd);
  int test_fd = open(path, O_RDONLY);
  if (test_fd >= 0) {
    printf("closed: unexpected-success\n");
    close(test_fd);
  } else {
    if (errno == ENOENT) {
      printf("closed: enoent\n");
    } else if (errno == EBADF) {
      printf("closed: ebadf\n");
    } else {
      printf("closed: failed errno=%d\n", errno);
    }
  }
  return 0;
}

static int probe_pipe(void) {
  int p[2];
  if (pipe(p) < 0) {
    perror("pipe");
    return 1;
  }

  char path[64];
  snprintf(path, sizeof(path), "/dev/fd/%d", p[0]);
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    printf("pipe: open-failed errno=%d\n", errno);
    close(p[0]);
    close(p[1]);
    return 1;
  }

  const char *msg = "pipe-payload\n";
  size_t len = strlen(msg);
  if (write(p[1], msg, len) != (ssize_t)len) {
    perror("write to pipe");
    close(p[0]);
    close(p[1]);
    close(fd);
    return 1;
  }

  char buf[32] = {0};
  ssize_t n = read(fd, buf, len);
  close(p[0]);
  close(p[1]);
  close(fd);

  if (n == (ssize_t)len && strcmp(buf, msg) == 0) {
    printf("pipe: ok\n");
    return 0;
  }
  printf("pipe: corrupted\n");
  return 1;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <offset <tmpfile> | socket | closed | pipe>\n", argv[0]);
    return 2;
  }

  if (strcmp(argv[1], "offset") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s offset <tmpfile>\n", argv[0]);
      return 2;
    }
    return probe_offset(argv[2]);
  } else if (strcmp(argv[1], "socket") == 0) {
    return probe_socket();
  } else if (strcmp(argv[1], "closed") == 0) {
    return probe_closed();
  } else if (strcmp(argv[1], "pipe") == 0) {
    return probe_pipe();
  }

  fprintf(stderr, "Unknown mode: %s\n", argv[1]);
  return 2;
}
