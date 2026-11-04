#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE
#define _DARWIN_C_SOURCE

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
#include <time.h>
#include <sys/time.h>

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

static long now_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* 1 when a wait of ms is at least 150 ms (a 200 ms or longer timer ran) and under 5 s. */
static int waited_for_timer(long ms) {
  return ms >= 150 && ms < 5000;
}

static void ignore_alarm(int sig) {
  (void)sig;
}

static volatile sig_atomic_t caught_sig = 0;
static void handle_sig(int sig) {
  caught_sig = sig;
}

static void apply_raw(struct termios *tio) {
  tio->c_iflag &= ~(tcflag_t)(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tio->c_oflag &= ~(tcflag_t)OPOST;
  tio->c_lflag &= ~(tcflag_t)(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tio->c_cflag &= ~(tcflag_t)(CSIZE | PARENB);
  tio->c_cflag |= (tcflag_t)CS8;
  tio->c_cc[VMIN] = 1;
  tio->c_cc[VTIME] = 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <mode> [args...]\n", argv[0]);
    return 1;
  }
  const char *mode = argv[1];

  if (strcmp(mode, "canonical_line_buffering") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag |= (tcflag_t)ICANON;
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    /* Write 5 bytes without newline */
    if (write(m, "hello", 5) < 0) {
      close(s);
      close(m);
      return 3;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int r1 = poll_read(s, buf, sizeof(buf) - 1, 300);

    /* Now write newline to complete line */
    if (write(m, "\n", 1) < 0) {
      close(s);
      close(m);
      return 4;
    }
    int r2 = poll_read(s, buf, sizeof(buf) - 1, 2000);
    printf("canon_pending=%d canon_delivered=%d data=%.*s",
           r1 <= 0 ? 0 : r1, r2 > 0 ? r2 : 0, r2 > 1 ? r2 - 1 : 0, buf);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "canonical_max_one_line") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag |= (tcflag_t)ICANON;
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    /* Write two distinct lines in a single write call */
    const char lines[] = "alpha\nbeta\n";
    if (write(m, lines, sizeof(lines) - 1) < 0) {
      close(s);
      close(m);
      return 3;
    }
    char buf1[64];
    char buf2[64];
    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));

    int r1 = poll_read(s, buf1, sizeof(buf1) - 1, 2000);
    int r2 = poll_read(s, buf2, sizeof(buf2) - 1, 2000);

    printf("read1_len=%d read1=%.*s read2_len=%d read2=%.*s",
           r1, r1 > 1 ? r1 - 1 : 0, buf1,
           r2, r2 > 1 ? r2 - 1 : 0, buf2);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "canonical_editing") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag |= (tcflag_t)ICANON;
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);
    }
    /* Send foo + Backspace (0x7F) + bar + newline */
    const char in_erase[] = {'f', 'o', 'o', 0x7f, 'b', 'a', 'r', '\n'};
    if (write(m, in_erase, sizeof(in_erase)) < 0) {
      close(s);
      close(m);
      return 3;
    }
    char buf1[64];
    memset(buf1, 0, sizeof(buf1));
    int r1 = poll_read(s, buf1, sizeof(buf1) - 1, 2000);

    /* Send junk + KILL (0x15, Ctrl+U) + clean + newline */
    const char in_kill[] = {'j', 'u', 'n', 'k', 0x15, 'c', 'l', 'e', 'a', 'n', '\n'};
    if (write(m, in_kill, sizeof(in_kill)) < 0) {
      close(s);
      close(m);
      return 4;
    }
    char buf2[64];
    memset(buf2, 0, sizeof(buf2));
    int r2 = poll_read(s, buf2, sizeof(buf2) - 1, 2000);

    printf("erase_len=%d erase_str=%.*s kill_len=%d kill_str=%.*s",
           r1, r1 > 1 ? r1 - 1 : 0, buf1,
           r2, r2 > 1 ? r2 - 1 : 0, buf2);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "noncanon_vmin_vtime") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    /* Case B: VMIN = 3, VTIME = 0 */
    struct termios tio;
    if (tcgetattr(s, &tio) == 0) {
      tio.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
      tio.c_cc[VMIN] = 3;
      tio.c_cc[VTIME] = 0;
      tcsetattr(s, TCSANOW, &tio);
    }
    /* Write 2 bytes (fewer than VMIN) */
    if (write(m, "ab", 2) < 0) {
      close(s);
      close(m);
      return 3;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int r_partial = poll_read(s, buf, sizeof(buf) - 1, 300);

    /* Write 3rd byte to satisfy VMIN */
    if (write(m, "c", 1) < 0) {
      close(s);
      close(m);
      return 4;
    }
    int r_full = poll_read_all(s, buf, 3, 2000);

    /* Case C: VMIN = 0, VTIME = 2 (200 ms timeout) */
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 2;
    tcsetattr(s, TCSANOW, &tio);

    /* Read with no input available -> timer expires and returns 0 */
    memset(buf, 0, sizeof(buf));
    long c_start = now_ms();
    ssize_t r_timeout = read(s, buf, sizeof(buf) - 1);
    long c_ms = now_ms() - c_start;

    /* Case D: VMIN = 0, VTIME = 0 (polling) */
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    tcsetattr(s, TCSANOW, &tio);

    memset(buf, 0, sizeof(buf));
    ssize_t r_empty = read(s, buf, sizeof(buf) - 1);

    printf("caseB_partial=%d caseB_full=%d caseC_timeout=%d caseC_waited=%d caseD_empty=%d\n",
           r_partial <= 0 ? 0 : r_partial,
           r_full,
           (int)r_timeout,
           waited_for_timer(c_ms),
           (int)r_empty);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "noncanon_case_a") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) < 0) {
      return 3;
    }
    tio.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
    tio.c_cc[VMIN] = 3;
    tio.c_cc[VTIME] = 2;
    tcsetattr(s, TCSANOW, &tio);
    char buf[64];

    /* A1: no byte yet, so the inter-byte timer has not started: the read must outlive
     * VTIME (200 ms) and only a 600 ms SIGALRM ends it with EINTR. Case C would return 0. */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = ignore_alarm; /* no SA_RESTART: the read must fail with EINTR */
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
    struct itimerval alarm_after = {{0, 0}, {0, 600000}};
    setitimer(ITIMER_REAL, &alarm_after, NULL);
    ssize_t r_idle = read(s, buf, sizeof(buf));
    int idle_errno = errno;
    struct itimerval disarm = {{0, 0}, {0, 0}};
    setitimer(ITIMER_REAL, &disarm, NULL);

    /* A2: VMIN bytes already queued: returned at once with all 3. */
    if (write(m, "abc", 3) < 0) {
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int r_full = poll_read_all(s, buf, 3, 2000);

    /* A3: fewer than VMIN bytes: the read returns them when the inter-byte timer expires. */
    if (write(m, "ab", 2) < 0) {
      return 5;
    }
    long short_start = now_ms();
    ssize_t r_short = read(s, buf, sizeof(buf));
    long short_ms = now_ms() - short_start;

    /* A4: the timer restarts on every byte. VMIN 6, VTIME 5 (500 ms); a child writes five
     * bytes 200 ms apart (800 ms in all, longer than VTIME, each gap shorter), so the read
     * returns all 5. A timer that ran from the first byte would return 3 at 500 ms. */
    tio.c_cc[VMIN] = 6;
    tio.c_cc[VTIME] = 5;
    tcsetattr(s, TCSANOW, &tio);
    pid_t pid = fork();
    if (pid < 0) {
      return 6;
    }
    if (pid == 0) {
      for (int i = 0; i < 5; i++) {
        if (i > 0) {
          usleep(200000);
        }
        if (write(m, "x", 1) < 0) {
          _exit(1);
        }
      }
      _exit(0);
    }
    ssize_t r_reset = read(s, buf, sizeof(buf));
    waitpid(pid, NULL, 0);

    printf("caseA_idle=%s caseA_full=%d caseA_short=%d caseA_short_waited=%d caseA_reset=%d\n",
           (r_idle < 0 && idle_errno == EINTR) ? "eintr" : (r_idle == 0 ? "zero" : "data"),
           r_full, (int)r_short, waited_for_timer(short_ms), (int)r_reset);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "raw_mode_staircase_and_echo") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }

    /* 1. Echo on: a canonical line written to the master is echoed back to the master as
     * "abc" plus CR LF (ONLCR applies to the echoed newline), 5 bytes. */
    struct termios tio;
    tcgetattr(s, &tio);
    tio.c_lflag |= (tcflag_t)(ICANON | ECHO);
    tio.c_oflag |= (tcflag_t)(OPOST | ONLCR);
    tcsetattr(s, TCSANOW, &tio);
    if (write(m, "abc\n", 4) < 0) {
      close(s);
      close(m);
      return 6;
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int echo_on_bytes = poll_read_all(m, buf, 5, 1000);
    int echo_on_text = (echo_on_bytes == 5 && memcmp(buf, "abc\r\n", 5) == 0) ? 1 : 0;
    char line[64];
    (void)poll_read(s, line, sizeof(line), 1000); /* consume the committed line */

    /* 2. Echo off: raw mode must produce no echo at all. */
    tcgetattr(s, &tio);
    apply_raw(&tio);
    tcsetattr(s, TCSANOW, &tio);

    /* In raw mode, write to master; verify master sees NO echo */
    if (write(m, "rawinput", 8) < 0) {
      close(s);
      close(m);
      return 3;
    }
    memset(buf, 0, sizeof(buf));
    int echo_bytes = poll_read(m, buf, sizeof(buf) - 1, 200);

    /* Drain slave input */
    char drain[64];
    int d = poll_read_all(s, drain, 8, 1000);
    (void)d;

    /* 3. Test output postprocessing (ONLCR) */
    /* Canonical mode: write \n to slave, master sees \r\n */
    if (tcgetattr(s, &tio) == 0) {
      tio.c_oflag |= (tcflag_t)(OPOST | ONLCR);
      tcsetattr(s, TCSANOW, &tio);
    }
    if (write(s, "line\n", 5) < 0) {
      close(s);
      close(m);
      return 4;
    }
    memset(buf, 0, sizeof(buf));
    int canon_out_len = poll_read_all(m, buf, 6, 2000);
    int canon_crlf = (canon_out_len >= 2 && buf[canon_out_len - 2] == '\r' && buf[canon_out_len - 1] == '\n') ? 1 : 0;

    /* Raw mode: OPOST is clear, master sees solitary \n */
    if (tcgetattr(s, &tio) == 0) {
      apply_raw(&tio);
      tcsetattr(s, TCSANOW, &tio);
    }
    if (write(s, "line\n", 5) < 0) {
      close(s);
      close(m);
      return 5;
    }
    memset(buf, 0, sizeof(buf));
    int raw_out_len = poll_read_all(m, buf, 5, 2000);
    int raw_crlf = (raw_out_len >= 2 && buf[raw_out_len - 2] == '\r' && buf[raw_out_len - 1] == '\n') ? 1 : 0;
    int raw_ends_nl = (raw_out_len >= 1 && buf[raw_out_len - 1] == '\n') ? 1 : 0;

    printf("echo_on=%d echo_bytes=%d canon_crlf=%d raw_crlf=%d raw_ends_nl=%d\n",
           echo_on_text, echo_bytes <= 0 ? 0 : echo_bytes, canon_crlf, raw_crlf, raw_ends_nl);
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "raw_mode_signals") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    /* Fork a child that acts as the foreground process group on slave PTY */
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

      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = handle_sig;
      sigaction(SIGINT, &sa, NULL);

      /* Phase 1: Canonical mode with ISIG enabled */
      struct termios tio;
      tcgetattr(s, &tio);
      tio.c_lflag |= (tcflag_t)(ICANON | ISIG);
      tio.c_lflag &= ~(tcflag_t)ECHO;
      tcsetattr(s, TCSANOW, &tio);

      /* Signal readiness to parent by writing 1 byte */
      if (write(s, "R", 1) < 0) {
        _exit(10);
      }

      /* Wait until SIGINT is caught */
      int waits = 0;
      while (!caught_sig && waits < 30) {
        usleep(100000);
        waits++;
      }
      int sig1 = caught_sig;

      /* Phase 2: Raw mode with ISIG disabled */
      caught_sig = 0;
      apply_raw(&tio);
      tcsetattr(s, TCSANOW, &tio);

      /* Signal readiness for Phase 2 */
      if (write(s, "R", 1) < 0) {
        _exit(11);
      }

      /* In raw mode, 0x03 should be read as a byte, not deliver SIGINT */
      char byte_in = 0;
      ssize_t n = read(s, &byte_in, 1);
      int sig2 = caught_sig;

      printf("canon_sig=%d raw_sig=%d raw_byte=0x%02x read_ret=%d\n",
             sig1, sig2, (unsigned char)byte_in, (int)n);
      fflush(stdout);
      _exit(0);
    }

    /* Parent process */
    close(s);
    char ready = 0;
    /* Wait for child Phase 1 ready */
    if (poll_read(m, &ready, 1, 3000) <= 0 || ready != 'R') {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      close(m);
      return 4;
    }

    /* Send VINTR (0x03) to trigger SIGINT */
    usleep(50000);
    const char intr = 0x03;
    if (write(m, &intr, 1) < 0) {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      close(m);
      return 5;
    }

    /* Wait for child Phase 2 ready */
    ready = 0;
    if (poll_read(m, &ready, 1, 3000) <= 0 || ready != 'R') {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      close(m);
      return 6;
    }

    /* Send 0x03 again: in raw mode, this must be read as raw data */
    usleep(50000);
    if (write(m, &intr, 1) < 0) {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      close(m);
      return 7;
    }

    int status = 0;
    waitpid(pid, &status, 0);
    close(m);
    return WEXITSTATUS(status);
  }

  if (strcmp(mode, "inspect_control_defaults") == 0) {
    int m, s;
    if (open_pty_pair(&m, &s) < 0) {
      return 2;
    }
    struct termios tio;
    if (tcgetattr(s, &tio) < 0) {
      close(s);
      close(m);
      return 3;
    }
    long max_canon = fpathconf(s, _PC_MAX_CANON);
    long max_input = fpathconf(s, _PC_MAX_INPUT);
    long vdisable = fpathconf(s, _PC_VDISABLE);
    printf("NCCS=%d VEOF_idx=%d VEOL_idx=%d VMIN_idx=%d VTIME_idx=%d "
           "VEOF_val=0x%02x VMIN_val=0x%02x\n",
           NCCS, VEOF, VEOL, VMIN, VTIME,
           (unsigned char)tio.c_cc[VEOF], (unsigned char)tio.c_cc[VMIN]);
    printf("VTIME_val=0x%02x VEOL_val=0x%02x VERASE_val=0x%02x VKILL_val=0x%02x "
           "VINTR_val=0x%02x VQUIT_val=0x%02x VSUSP_val=0x%02x\n",
           (unsigned char)tio.c_cc[VTIME], (unsigned char)tio.c_cc[VEOL],
           (unsigned char)tio.c_cc[VERASE], (unsigned char)tio.c_cc[VKILL],
           (unsigned char)tio.c_cc[VINTR], (unsigned char)tio.c_cc[VQUIT],
           (unsigned char)tio.c_cc[VSUSP]);
    printf("MAX_CANON=%ld MAX_INPUT=%ld VDISABLE=0x%02lx cc_t_bits=%d\n",
           max_canon, max_input, vdisable & 0xff, (int)(sizeof(cc_t) * 8));
    close(s);
    close(m);
    return 0;
  }

  if (strcmp(mode, "shell_raw_exec") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s shell_raw_exec <cmd> [args...]\n", argv[0]);
      return 1;
    }
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
      dup2(s, STDIN_FILENO);
      dup2(s, STDOUT_FILENO);
      dup2(s, STDERR_FILENO);
      if (s > 2) {
        close(s);
      }
      execvp(argv[2], &argv[2]);
      _exit(127);
    }
    close(s);
    /* Write input payload */
    const char in[] = {'a', 'b', 'c', '\n'};
    if (write(m, in, sizeof(in)) < 0) {
      kill(pid, SIGKILL);
      waitpid(pid, NULL, 0);
      close(m);
      return 4;
    }
    char out[256];
    memset(out, 0, sizeof(out));
    int n = poll_read_all(m, out, sizeof(out) - 1, 3000);
    int status = 0;
    waitpid(pid, &status, 0);
    close(m);
    if (n > 0) {
      fwrite(out, 1, (size_t)n, stdout);
    }
    return WEXITSTATUS(status);
  }

  fprintf(stderr, "Unknown mode: %s\n", mode);
  return 1;
}
