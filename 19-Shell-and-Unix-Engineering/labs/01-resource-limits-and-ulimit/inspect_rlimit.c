#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <unistd.h>

static void print_limit(const char *name, int resource) {
  struct rlimit rl;
  if (getrlimit(resource, &rl) != 0) {
    fprintf(stderr, "getrlimit(%s): %s\n", name, strerror(errno));
    exit(1);
  }
  printf("%s cur=", name);
  if (rl.rlim_cur == RLIM_INFINITY) {
    printf("unlimited");
  } else {
    printf("%llu", (unsigned long long)rl.rlim_cur);
  }
  printf(" max=");
  if (rl.rlim_max == RLIM_INFINITY) {
    printf("unlimited\n");
  } else {
    printf("%llu\n", (unsigned long long)rl.rlim_max);
  }
}

static void test_write_efbig(const char *path) {
  signal(SIGXFSZ, SIG_IGN);
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    fprintf(stderr, "open(%s): %s\n", path, strerror(errno));
    exit(1);
  }
  char buf[512];
  memset(buf, 'A', sizeof(buf));
  int caught_efbig = 0;
  for (int i = 0; i < 32; i++) {
    ssize_t n = write(fd, buf, sizeof(buf));
    if (n < 0) {
      if (errno == EFBIG) {
        caught_efbig = 1;
      } else {
        fprintf(stderr, "unexpected write errno: %d (%s)\n", errno, strerror(errno));
      }
      break;
    }
  }
  close(fd);
  if (caught_efbig) {
    printf("write_result=EFBIG\n");
  } else {
    printf("write_result=NO_ERROR\n");
  }
}

static void test_probe_fds(void) {
  int r9 = dup2(0, 9);
  int e9 = errno;
  int r10 = dup2(0, 10);
  int e10 = errno;

  const char *s9 = (r9 >= 0) ? "ok" : ((e9 == EBADF) ? "EBADF" : "err");
  const char *s10 = (r10 >= 0) ? "ok" : ((e10 == EBADF) ? "EBADF" : "err");

  if (r9 >= 0) {
    close(9);
  }
  if (r10 >= 0) {
    close(10);
  }

  int fds[32];
  int count = 0;
  int saw_emfile = 0;
  for (int i = 0; i < 32; i++) {
    int fd = open("/dev/null", O_RDONLY);
    if (fd < 0) {
      if (errno == EMFILE) {
        saw_emfile = 1;
      }
      break;
    }
    fds[count++] = fd;
  }
  for (int i = 0; i < count; i++) {
    close(fds[i]);
  }

  printf("fd9=%s fd10=%s emfile=%s\n", s9, s10, saw_emfile ? "ok" : "fail");
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <get|write_efbig|probe_fds> [args...]\n", argv[0]);
    return 1;
  }
  const char *cmd = argv[1];
  if (strcmp(cmd, "get") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s get <fsize|nofile|core|stack|cpu|as|nproc>\n", argv[0]);
      return 1;
    }
    const char *res = argv[2];
    if (strcmp(res, "fsize") == 0) {
      print_limit("fsize", RLIMIT_FSIZE);
    } else if (strcmp(res, "nofile") == 0) {
      print_limit("nofile", RLIMIT_NOFILE);
    } else if (strcmp(res, "core") == 0) {
      print_limit("core", RLIMIT_CORE);
    } else if (strcmp(res, "stack") == 0) {
      print_limit("stack", RLIMIT_STACK);
    } else if (strcmp(res, "cpu") == 0) {
      print_limit("cpu", RLIMIT_CPU);
#ifdef RLIMIT_AS
    } else if (strcmp(res, "as") == 0) {
      print_limit("as", RLIMIT_AS);
#endif
#ifdef RLIMIT_NPROC
    } else if (strcmp(res, "nproc") == 0) {
      print_limit("nproc", RLIMIT_NPROC);
#endif
    } else {
      fprintf(stderr, "Unknown resource: %s\n", res);
      return 1;
    }
  } else if (strcmp(cmd, "write_efbig") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s write_efbig <filepath>\n", argv[0]);
      return 1;
    }
    test_write_efbig(argv[2]);
  } else if (strcmp(cmd, "probe_fds") == 0) {
    test_probe_fds();
  } else {
    fprintf(stderr, "Unknown command: %s\n", cmd);
    return 1;
  }
  return 0;
}
