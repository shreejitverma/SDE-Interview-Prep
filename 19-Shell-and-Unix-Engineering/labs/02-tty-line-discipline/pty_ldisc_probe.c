#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/wait.h>
#include <poll.h>
#include <errno.h>

static int open_pty_pair(int *master_fd, int *slave_fd) {
  int m = posix_openpt(O_RDWR | O_NOCTTY);
  if (m < 0) {
    return -1;
  }
  if (grantpt(m) < 0 || unlockpt(m) < 0) {
    close(m);
    return -1;
  }
  char *pts = ptsname(m);
  if (!pts) {
    close(m);
    return -1;
  }
  int s = open(pts, O_RDWR | O_NOCTTY);
  if (s < 0) {
    close(m);
    return -1;
  }
  *master_fd = m;
  *slave_fd = s;
  return 0;
}

static int poll_read(int fd, char *buf, size_t maxlen, int timeout_ms) {
  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLIN;
  pfd.revents = 0;
  int ret = poll(&pfd, 1, timeout_ms);
  if (ret <= 0) {
    return ret;
  }
  ssize_t n = read(fd, buf, maxlen);
  return (int)n;
}

static int poll_read_all(int fd, char *buf, size_t count, int timeout_ms) {
  size_t total = 0;
  while (total < count) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    int ret = poll(&pfd, 1, timeout_ms);
    if (ret <= 0) {
      return total > 0 ? (int)total : ret;
    }
    ssize_t n = read(fd, buf + total, count - total);
    if (n <= 0) {
      break;
    }
    total += (size_t)n;
  }
  return (int)total;
}

/* Wait for pid for at most timeout_ms; on timeout kill it and return -1, else return its wait status. */
static int wait_bounded(pid_t pid, int timeout_ms) {
  for (int waited = 0; waited <= timeout_ms; waited += 10) {
    int status = 0;
    pid_t r = waitpid(pid, &status, WNOHANG);
    if (r == pid) {
      return status;
    }
    if (r < 0) {
      return -1;
    }
    poll(NULL, 0, 10);
  }
  kill(pid, SIGKILL);
  (void)waitpid(pid, NULL, 0);
  return -1;
}

/* Read from fd until end of file, a read error, or idle_ms without data; returns the byte count. */
static size_t drain(int fd, char *buf, size_t cap, int idle_ms) {
  size_t total = 0;
  while (total + 1 < cap) {
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, idle_ms) <= 0) {
      break;
    }
    ssize_t n = read(fd, buf + total, cap - 1 - total);
    if (n <= 0) {
      break;
    }
    total += (size_t)n;
  }
  buf[total] = '\0';
  return total;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <mode> [args...]\n", argv[0]);
    return 1;
  }
  const char *mode = argv[1];

  if (strcmp(mode, "query_ldisc") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    int ldisc = -1;
    if (ioctl(s, TIOCGETD, &ldisc) < 0) {
      perror("TIOCGETD");
      close(s);
      close(m);
      return 3;
    }
    printf("ldisc=%d\n", ldisc);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "canonical_erase") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    const char in[] = {'a', 'b', 'c', 0x7f, 'd', '\n'};
    if (write(m, in, sizeof(in)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = poll_read(s, buf, sizeof(buf) - 1, 2000);
    if (n <= 0) {
      close(s);
      close(m);
      return 5;
    }
    printf("read_len=%d read_str=%.*s", n, n, buf);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "canonical_kill") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    const char in[] = {'d', 'i', 's', 'c', 'a', 'r', 'd', 0x15, 'k', 'e', 'p', 't', '\n'};
    if (write(m, in, sizeof(in)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = poll_read(s, buf, sizeof(buf) - 1, 2000);
    if (n <= 0) {
      close(s);
      close(m);
      return 5;
    }
    printf("read_len=%d read_str=%.*s", n, n, buf);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "canonical_werase") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    const char in[] = {'f', 'i', 'r', 's', 't', ' ', 's', 'e', 'c', 'o', 'n', 'd', 0x17, 't', 'h', 'i', 'r', 'd', '\n'};
    if (write(m, in, sizeof(in)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = poll_read(s, buf, sizeof(buf) - 1, 2000);
    if (n <= 0) {
      close(s);
      close(m);
      return 5;
    }
    printf("read_len=%d read_str=%.*s", n, n, buf);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "eof_flush") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    const char in1[] = {'f', 'l', 'u', 's', 'h', '_', 'm', 'e', 0x04};
    if (write(m, in1, sizeof(in1)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n1 = poll_read(s, buf, sizeof(buf) - 1, 2000);
    printf("part1_len=%d\npart1_str=%.*s\n", n1, n1 > 0 ? n1 : 0, buf);

    const char in2[] = {0x04};
    if (write(m, in2, sizeof(in2)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int n2 = poll_read(s, buf, sizeof(buf) - 1, 2000);
    printf("part2_len=%d\n", n2);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "crnl_mapping") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    const char in[] = {'t', 'e', 's', 't', '\r'};
    if (write(m, in, sizeof(in)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = poll_read(s, buf, sizeof(buf) - 1, 2000);
    printf("icrnl_len=%d last_byte=0x%02x\n", n, n > 0 ? (unsigned char)buf[n - 1] : 0);

    if (write(s, "out\n", 4) < 0) {
      close(s);
      close(m);
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int n_out = poll_read_all(m, buf, 5, 2000);
    printf("onlcr_len=%d ends_crnl=%d\n", n_out, (n_out >= 2 && buf[n_out - 2] == '\r' && buf[n_out - 1] == '\n') ? 1 : 0);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "echo_toggle") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    if (write(m, "ping\n", 5) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int echo_len = poll_read_all(m, buf, 6, 1000);

    char scratch[64];
    int d1 = poll_read(s, scratch, sizeof(scratch), 1000);
    (void)d1;

    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    if (write(m, "quiet\n", 6) < 0) {
      close(s);
      close(m);
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int noecho_len = poll_read(m, buf, sizeof(buf) - 1, 500);
    int d2 = poll_read(s, scratch, sizeof(scratch), 1000);
    printf("echo_on_len=%d noecho_len=%d slave_read_len=%d\n", echo_len, noecho_len, d2);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "noncanon_switch") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    // 1. In canonical mode, write 'x' without newline -> must block/timeout
    if (write(m, "x", 1) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int r1 = poll_read(s, buf, sizeof(buf) - 1, 300);

    // 2. Disable ICANON and ECHO, set VMIN=1, VTIME=0
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
      tio.c_cc[VMIN] = 1;
      tio.c_cc[VTIME] = 0;
      tcsetattr(s, TCSANOW, &tio);
    }
    // Write 'y' without newline -> must be immediately available with 'x'
    if (write(m, "y", 1) < 0) {
      close(s);
      close(m);
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int r2 = poll_read_all(s, buf, 2, 2000);
    printf("canonical_poll=%d noncanon_poll=%d read_bytes=%d content=%.*s\n",
           r1, r2 > 0 ? 1 : 0, r2 > 0 ? r2 : 0, r2 > 0 ? r2 : 0, buf);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "isig_signal") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    pid_t pid = fork();
    if (pid < 0) {
      close(s);
      close(m);
      return 3;
    }
    if (pid == 0) {
      close(m);
      setsid();
#ifdef TIOCSCTTY
      ioctl(s, TIOCSCTTY, 0);
#endif
      tcsetpgrp(s, getpid());
      sigset_t mask;
      sigemptyset(&mask);
      sigaddset(&mask, SIGINT);
      sigprocmask(SIG_BLOCK, &mask, NULL);

      if (write(s, "READY\n", 6) < 0) {
        _exit(10);
      }
      int sig = 0;
      if (sigwait(&mask, &sig) == 0) {
        printf("CAUGHT_SIGNAL=%d\n", sig);
        fflush(stdout);
      }
      close(s);
      _exit(0);
    }
    close(s);
    char sync_buf[16];
    int sr = poll_read(m, sync_buf, sizeof(sync_buf), 2000);
    if (sr <= 0) {
      close(m);
      return 6;
    }
    const char intr = 0x03;
    if (write(m, &intr, 1) < 0) {
      close(m);
      return 7;
    }
    int status = wait_bounded(pid, 3000);
    close(m);
    if (status < 0 || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
      fprintf(stderr, "isig_signal: child did not exit cleanly within 3 s (no signal delivered?)\n");
      return 8;
    }
    return 0;
  }

  if (strcmp(mode, "shell_read") == 0) {
    if (argc < 3) {
      fprintf(stderr, "shell_read requires shell command\n");
      return 1;
    }
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    pid_t pid = fork();
    if (pid < 0) {
      close(s);
      close(m);
      return 3;
    }
    if (pid == 0) {
      close(m);
      setsid();
#ifdef TIOCSCTTY
      ioctl(s, TIOCSCTTY, 0);
#endif
      dup2(s, STDIN_FILENO);
      dup2(s, STDOUT_FILENO);
      dup2(s, STDERR_FILENO);
      if (s > STDERR_FILENO) {
        close(s);
      }
      execvp(argv[2], &argv[2]);
      _exit(127);
    }
    close(s);
    const char in[] = {'h', 'e', 'l', 'l', 'x', 0x7f, 'o', '\n'};
    if (write(m, in, sizeof(in)) < 0) {
      close(m);
      return 4;
    }
    char buf[4096];
    size_t got = drain(m, buf, sizeof(buf), 2000);
    if (got > 0) {
      printf("%s", buf);
    }
    close(m);
    (void)wait_bounded(pid, 500);
    return 0;
  }

  fprintf(stderr, "Unknown mode: %s\n", mode);
  return 1;
}
