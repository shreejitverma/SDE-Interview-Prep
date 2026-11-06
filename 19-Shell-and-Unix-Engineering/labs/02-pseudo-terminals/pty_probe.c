/*
 * pty_probe: measure the behavior of a pseudo-terminal pair.
 *
 * Usage:
 *   pty_probe pair                  default termios, ownership, and window size of a new slave
 *   pty_probe echo                  what the line discipline does to bytes crossing the pair
 *   pty_probe slave-exit            what the master sees when the last slave descriptor closes
 *   pty_probe master-close          what the slave side sees when the master closes
 *   pty_probe winsize               TIOCSWINSZ on the master reaches the slave and raises SIGWINCH
 *   pty_probe ctrl-c                0x03 written to the master raises SIGINT in the slave's foreground group
 *   pty_probe ctrl-c-raw            the same byte reaches the slave as data once ISIG is off
 *   pty_probe tiocsti               TIOCSTI on the slave's own controlling terminal
 *   pty_probe run [-n] [-i INPUT] [-w ROWSxCOLS] [-t SECS] -- CMD [ARG...]
 *                                   run CMD on a new pty (its controlling terminal, stdin, stdout, stderr)
 *                                   and copy everything it writes to our stdout; INPUT understands
 *                                   \n, \r, \\ and \ooo escapes; the exit status is CMD's;
 *                                   -n starts CMD in a new session with the slave as stdin, stdout, and
 *                                   stderr but never makes it the controlling terminal (no TIOCSCTTY)
 *
 * Every mode prints one line of key=value pairs, bytes shown with \r, \n and \xHH escapes.
 * Every wait is bounded, and an alarm ends the whole probe if anything blocks.
 */
#define _GNU_SOURCE /* glibc declares posix_openpt, grantpt, unlockpt, and ptsname only on request */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define LIMIT_SECONDS 25

static const char *errname(int e) {
  switch (e) {
  case 0:
    return "0";
  case EIO:
    return "EIO";
  case EPERM:
    return "EPERM";
  case EAGAIN:
    return "EAGAIN";
  case ENOTTY:
    return "ENOTTY";
  default:
    return "other";
  }
}

/* Append BUF[0..N) to OUT as printable text. */
static void show(const char *buf, ssize_t n, char *out, size_t cap) {
  size_t len = strlen(out);
  for (ssize_t i = 0; i < n && len + 6 < cap; i++) {
    unsigned char c = (unsigned char)buf[i];
    if (c == '\r') {
      len += (size_t)snprintf(out + len, cap - len, "\\r");
    } else if (c == '\n') {
      len += (size_t)snprintf(out + len, cap - len, "\\n");
    } else if (c < 0x20 || c > 0x7e) {
      len += (size_t)snprintf(out + len, cap - len, "\\x%02x", c);
    } else {
      out[len++] = (char)c;
      out[len] = '\0';
    }
  }
}

/* Open a master and name its slave: the portable POSIX sequence. Returns the master or -1. */
static int open_master(char *name, size_t len) {
  int master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0) {
    return -1;
  }
  const char *slave = NULL;
  if (grantpt(master) == 0 && unlockpt(master) == 0) {
    slave = ptsname(master);
  }
  if (slave == NULL) {
    close(master);
    return -1;
  }
  snprintf(name, len, "%s", slave);
  return master;
}

/* Read what is available on FD, waiting at most MS for the first byte and 100 ms for each later one. */
static ssize_t read_some(int fd, char *buf, size_t cap, int ms) {
  size_t total = 0;
  int wait_ms = ms;
  for (;;) {
    struct pollfd pf = {.fd = fd, .events = POLLIN};
    if (poll(&pf, 1, wait_ms) <= 0) {
      return (ssize_t)total;
    }
    ssize_t n = read(fd, buf + total, cap - total);
    if (n < 0) {
      return total > 0 ? (ssize_t)total : -1;
    }
    if (n == 0) {
      return (ssize_t)total;
    }
    total += (size_t)n;
    wait_ms = 100;
    if (total >= cap) {
      return (ssize_t)total;
    }
  }
}

static int write_all(int fd, const char *buf, size_t n) {
  while (n > 0) {
    ssize_t w = write(fd, buf, n);
    if (w < 0) {
      return -1;
    }
    buf += w;
    n -= (size_t)w;
  }
  return 0;
}

static int flag_is_set(tcflag_t flags, tcflag_t bit) {
  return (flags & bit) != 0;
}

static int mode_pair(void) {
  char name[128];
  int master = open_master(name, sizeof name);
  if (master < 0) {
    printf("open_failed errno=%s\n", errname(errno));
    return 1;
  }
  int slave = open(name, O_RDWR | O_NOCTTY);
  if (slave < 0) {
    printf("slave_open_failed errno=%s\n", errname(errno));
    return 1;
  }
  struct termios t;
  struct winsize ws;
  struct stat st;
  if (tcgetattr(slave, &t) < 0 || ioctl(slave, TIOCGWINSZ, &ws) < 0 || fstat(slave, &st) < 0) {
    printf("query_failed errno=%s\n", errname(errno));
    return 1;
  }
  size_t end = strlen(name);
  while (end > 0 && name[end - 1] >= '0' && name[end - 1] <= '9') {
    end--;
  }
  name[end] = '\0';
  printf("prefix=%s slave_isatty=%d slave_uid_is_me=%d slave_mode=%03o echo=%d icanon=%d isig=%d icrnl=%d opost=%d onlcr=%d rows=%d cols=%d\n",
         name, isatty(slave), st.st_uid == getuid(), (unsigned)(st.st_mode & 0777),
         flag_is_set(t.c_lflag, ECHO), flag_is_set(t.c_lflag, ICANON), flag_is_set(t.c_lflag, ISIG),
         flag_is_set(t.c_iflag, ICRNL), flag_is_set(t.c_oflag, OPOST), flag_is_set(t.c_oflag, ONLCR),
         ws.ws_row, ws.ws_col);
  return 0;
}

static int mode_echo(void) {
  char name[128], buf[256], a[256] = "", b[256] = "", c[256] = "", d[256] = "", e[256] = "";
  int master = open_master(name, sizeof name);
  if (master < 0) {
    printf("open_failed errno=%s\n", errname(errno));
    return 1;
  }
  int slave = open(name, O_RDWR | O_NOCTTY);
  if (slave < 0) {
    printf("slave_open_failed errno=%s\n", errname(errno));
    return 1;
  }
  ssize_t n;
  /* master -> slave in cooked mode: the line discipline echoes, maps CR to NL, and buffers a line */
  if (write_all(master, "hi\r", 3) < 0) {
    return 1;
  }
  n = read_some(master, buf, sizeof buf, 1000);
  show(buf, n, a, sizeof a);
  n = read_some(slave, buf, sizeof buf, 1000);
  show(buf, n, b, sizeof b);
  /* slave -> master in cooked mode: output processing turns NL into CR NL */
  if (write_all(slave, "ok\n", 3) < 0) {
    return 1;
  }
  n = read_some(master, buf, sizeof buf, 1000);
  show(buf, n, c, sizeof c);
  /* raw slave: no echo, no CR mapping, no output processing; bytes cross unchanged */
  struct termios t;
  if (tcgetattr(slave, &t) < 0) {
    return 1;
  }
  cfmakeraw(&t);
  if (tcsetattr(slave, TCSANOW, &t) < 0) {
    return 1;
  }
  if (write_all(slave, "ok\n", 3) < 0 || write_all(master, "x\r", 2) < 0) {
    return 1;
  }
  n = read_some(master, buf, sizeof buf, 1000);
  show(buf, n, d, sizeof d);
  n = read_some(slave, buf, sizeof buf, 1000);
  show(buf, n, e, sizeof e);
  printf("cooked_master_sees=%s cooked_slave_reads=%s cooked_out=%s raw_master_sees=%s raw_slave_reads=%s\n",
         a, b, c, d, e);
  return 0;
}

static int mode_slave_exit(void) {
  char name[128], buf[256], first[256] = "";
  int master = open_master(name, sizeof name);
  if (master < 0) {
    printf("open_failed errno=%s\n", errname(errno));
    return 1;
  }
  int slave = open(name, O_RDWR | O_NOCTTY);
  if (slave < 0) {
    printf("slave_open_failed errno=%s\n", errname(errno));
    return 1;
  }
  if (write_all(slave, "bye\n", 4) < 0) {
    return 1;
  }
  close(slave);
  /* the data written before the close is still readable; only then does the end of file show */
  struct pollfd pf = {.fd = master, .events = POLLIN};
  if (poll(&pf, 1, 2000) <= 0) {
    printf("master_poll_timeout\n");
    return 1;
  }
  ssize_t n = read(master, buf, sizeof buf);
  show(buf, n, first, sizeof first);
  ssize_t second = -2;
  int second_errno = 0;
  if (poll(&pf, 1, 2000) > 0) {
    errno = 0;
    second = read(master, buf, sizeof buf);
    second_errno = errno;
  }
  printf("first_read=%s second_read_rc=%ld second_read_errno=%s\n", first, (long)second,
         errname(second_errno));
  return 0;
}

/* Child side of the controlling-terminal modes: become a session leader with SLAVE as controlling terminal. */
static volatile sig_atomic_t g_selfpipe_fd = -1;

static void on_signal(int sig) {
  char c = (char)sig;
  int saved = errno;
  if (write((int)g_selfpipe_fd, &c, 1) < 0) {
    /* nothing to do: the parent times out and reports the missing signal */
  }
  errno = saved;
}

static int child_open_ctty(const char *name) {
  if (setsid() < 0) {
    return -1;
  }
  int slave = open(name, O_RDWR | O_NOCTTY);
  if (slave < 0 || ioctl(slave, TIOCSCTTY, 0) < 0) {
    return -1;
  }
  return slave;
}

/*
 * Like child_open_ctty but stop before TIOCSCTTY: a new session without a controlling terminal.
 * The caller must not exec as the session leader, because a leader without a controlling terminal
 * may adopt a terminal it opens (macOS /bin/bash 3.2.57 was measured doing so); see run_detached_leader.
 */
static int child_open_no_ctty(const char *name) {
  if (setsid() < 0) {
    return -1;
  }
  return open(name, O_RDWR | O_NOCTTY);
}

/* Fork once more and wait, so the process that execs is not a session leader and cannot adopt a terminal. */
static void run_detached_leader(void) {
  pid_t pid = fork();
  if (pid < 0) {
    _exit(126);
  }
  if (pid == 0) {
    return;
  }
  int status = 0;
  if (waitpid(pid, &status, 0) != pid) {
    _exit(126);
  }
  _exit(WIFSIGNALED(status) ? 128 + WTERMSIG(status) : WEXITSTATUS(status));
}

/* Wait up to MS for the self-pipe to deliver SIG; return 1 when it did. */
static int signal_arrived(int fd, int sig, int ms) {
  char c;
  struct pollfd pf = {.fd = fd, .events = POLLIN};
  int pr;
  /* the handler itself interrupts poll with EINTR after it has written to the pipe, so retry */
  do {
    pr = poll(&pf, 1, ms);
  } while (pr < 0 && errno == EINTR);
  if (pr > 0 && read(fd, &c, 1) == 1) {
    return c == sig;
  }
  return 0;
}

static void report(int fd, const char *line) {
  if (write_all(fd, line, strlen(line)) < 0) {
    _exit(2);
  }
}

static void child_master_close(const char *name, int up) {
  int slave = child_open_ctty(name);
  int sp[2];
  if (slave < 0 || pipe(sp) < 0) {
    report(up, "child_setup_failed\n");
    _exit(1);
  }
  g_selfpipe_fd = sp[1];
  struct sigaction sa;
  memset(&sa, 0, sizeof sa);
  sa.sa_handler = on_signal;
  sigaction(SIGHUP, &sa, NULL);
  report(up, "ready\n");
  int got = signal_arrived(sp[0], SIGHUP, 4000);
  char buf[16], line[200];
  errno = 0;
  ssize_t r = read(slave, buf, sizeof buf);
  int r_errno = errno;
  errno = 0;
  ssize_t w = write(slave, "x", 1);
  int w_errno = errno;
  snprintf(line, sizeof line, "sighup=%d read_rc=%ld read_errno=%s write_rc=%ld write_errno=%s\n", got,
           (long)r, errname(r_errno), (long)w, errname(w_errno));
  report(up, line);
  _exit(0);
}

static void child_winsize(const char *name, int up) {
  int slave = child_open_ctty(name);
  int sp[2];
  if (slave < 0 || pipe(sp) < 0) {
    report(up, "child_setup_failed\n");
    _exit(1);
  }
  g_selfpipe_fd = sp[1];
  struct sigaction sa;
  memset(&sa, 0, sizeof sa);
  sa.sa_handler = on_signal;
  sigaction(SIGWINCH, &sa, NULL);
  report(up, "ready\n");
  int got = signal_arrived(sp[0], SIGWINCH, 4000);
  struct winsize ws = {0, 0, 0, 0};
  ioctl(slave, TIOCGWINSZ, &ws);
  char line[200];
  snprintf(line, sizeof line, "sigwinch=%d slave_rows=%d slave_cols=%d\n", got, ws.ws_row, ws.ws_col);
  report(up, line);
  _exit(0);
}

static void child_ctrl_c(const char *name, int up, int raw) {
  int slave = child_open_ctty(name);
  int sp[2];
  if (slave < 0 || pipe(sp) < 0) {
    report(up, "child_setup_failed\n");
    _exit(1);
  }
  g_selfpipe_fd = sp[1];
  struct sigaction sa;
  memset(&sa, 0, sizeof sa);
  sa.sa_handler = on_signal;
  sigaction(SIGINT, &sa, NULL);
  if (raw) {
    struct termios t;
    if (tcgetattr(slave, &t) < 0) {
      _exit(1);
    }
    cfmakeraw(&t);
    if (tcsetattr(slave, TCSANOW, &t) < 0) {
      _exit(1);
    }
  }
  report(up, "ready\n");
  char buf[16], data[64] = "", line[200];
  ssize_t n = 0;
  if (raw) {
    n = read_some(slave, buf, 1, 2000);
    show(buf, n, data, sizeof data);
  }
  int got = signal_arrived(sp[0], SIGINT, raw ? 1000 : 4000);
  snprintf(line, sizeof line, "sigint=%d slave_read=%s\n", got, data);
  report(up, line);
  _exit(0);
}

static void child_tiocsti(const char *name, int up) {
  int slave = child_open_ctty(name);
  if (slave < 0) {
    report(up, "child_setup_failed\n");
    _exit(1);
  }
  char line[200], buf[16], data[64] = "";
  int rc = 0, saved = 0;
  const char chars[] = {'x', '\n'};
  for (int i = 0; i < 2; i++) {
    errno = 0;
    if (ioctl(slave, TIOCSTI, &chars[i]) < 0) {
      rc = -1;
      saved = errno;
      break;
    }
  }
  if (rc == 0) {
    ssize_t n = read_some(slave, buf, sizeof buf, 1000);
    show(buf, n, data, sizeof data);
  }
  snprintf(line, sizeof line, "tiocsti_rc=%d tiocsti_errno=%s slave_reads=%s uid_is_root=%d\n", rc,
           errname(saved), data, getuid() == 0);
  report(up, line);
  _exit(0);
}

/* Parent side shared by the controlling-terminal modes. */
static int mode_with_child(const char *mode) {
  char name[128];
  int master = open_master(name, sizeof name);
  if (master < 0) {
    printf("open_failed errno=%s\n", errname(errno));
    return 1;
  }
  int up[2];
  if (pipe(up) < 0) {
    return 1;
  }
  pid_t pid = fork();
  if (pid < 0) {
    return 1;
  }
  if (pid == 0) {
    alarm(LIMIT_SECONDS - 5);
    close(master);
    close(up[0]);
    if (strcmp(mode, "master-close") == 0) {
      child_master_close(name, up[1]);
    } else if (strcmp(mode, "winsize") == 0) {
      child_winsize(name, up[1]);
    } else if (strcmp(mode, "ctrl-c") == 0) {
      child_ctrl_c(name, up[1], 0);
    } else if (strcmp(mode, "ctrl-c-raw") == 0) {
      child_ctrl_c(name, up[1], 1);
    } else {
      child_tiocsti(name, up[1]);
    }
    _exit(2);
  }
  close(up[1]);
  char buf[256], echoed[256] = "", result[256] = "";
  if (strcmp(mode, "tiocsti") != 0) {
    ssize_t n = read_some(up[0], buf, 6, 4000);
    if (n != 6 || memcmp(buf, "ready\n", 6) != 0) {
      printf("child_not_ready\n");
      kill(-pid, SIGKILL);
      waitpid(pid, NULL, 0);
      return 1;
    }
  }
  int master_open = 1;
  if (strcmp(mode, "winsize") == 0) {
    struct winsize ws = {0, 0, 0, 0};
    ioctl(master, TIOCGWINSZ, &ws);
    printf("master_initial=%dx%d ", ws.ws_row, ws.ws_col);
    ws.ws_row = 31;
    ws.ws_col = 97;
    if (ioctl(master, TIOCSWINSZ, &ws) < 0) {
      printf("tiocswinsz_failed\n");
    }
  } else if (strncmp(mode, "ctrl-c", 6) == 0) {
    if (write_all(master, "\003", 1) < 0) {
      return 1;
    }
    ssize_t n = read_some(master, buf, sizeof buf, 500);
    show(buf, n, echoed, sizeof echoed);
  } else if (strcmp(mode, "master-close") == 0) {
    close(master);
    master_open = 0;
  }
  ssize_t n = read_some(up[0], result, sizeof result - 1, 6000);
  if (n < 0) {
    n = 0;
  }
  result[n] = '\0';
  if (strncmp(mode, "ctrl-c", 6) == 0) {
    size_t len = strlen(result);
    while (len > 0 && result[len - 1] == '\n') {
      result[--len] = '\0';
    }
    printf("master_sees=%s %s\n", echoed, result);
  } else {
    printf("%s", result);
  }
  if (master_open) {
    close(master);
  }
  kill(-pid, SIGKILL);
  waitpid(pid, NULL, 0);
  return n > 0 ? 0 : 1;
}

/* Expand \n, \r, \\ and \ooo in IN into OUT; return the length. */
static size_t unescape(const char *in, char *out) {
  size_t len = 0;
  while (*in) {
    if (*in != '\\') {
      out[len++] = *in++;
      continue;
    }
    in++;
    if (*in == 'n') {
      out[len++] = '\n';
      in++;
    } else if (*in == 'r') {
      out[len++] = '\r';
      in++;
    } else if (*in == '\\') {
      out[len++] = '\\';
      in++;
    } else if (*in >= '0' && *in <= '7') {
      int v = 0;
      for (int i = 0; i < 3 && *in >= '0' && *in <= '7'; i++, in++) {
        v = v * 8 + (*in - '0');
      }
      out[len++] = (char)v;
    } else {
      out[len++] = '\\';
    }
  }
  return len;
}

static int mode_run(int argc, char **argv) {
  const char *input = NULL;
  struct winsize ws = {0, 0, 0, 0};
  int have_ws = 0, limit = 15, no_ctty = 0, i = 2;
  for (; i < argc && strcmp(argv[i], "--") != 0; i++) {
    if (strcmp(argv[i], "-n") == 0) {
      no_ctty = 1;
    } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
      input = argv[++i];
    } else if (strcmp(argv[i], "-w") == 0 && i + 1 < argc) {
      unsigned r, c;
      if (sscanf(argv[++i], "%ux%u", &r, &c) != 2) {
        fprintf(stderr, "pty_probe: bad -w value\n");
        return 2;
      }
      ws.ws_row = (unsigned short)r;
      ws.ws_col = (unsigned short)c;
      have_ws = 1;
    } else if (strcmp(argv[i], "-t") == 0 && i + 1 < argc) {
      limit = atoi(argv[++i]);
    } else {
      fprintf(stderr, "pty_probe: bad option %s\n", argv[i]);
      return 2;
    }
  }
  if (i >= argc - 1) {
    fprintf(stderr, "pty_probe: run needs -- CMD\n");
    return 2;
  }
  char **cmd = &argv[i + 1];
  char name[128];
  int master = open_master(name, sizeof name);
  if (master < 0) {
    fprintf(stderr, "pty_probe: cannot open a pty: %s\n", strerror(errno));
    return 2;
  }
  fflush(stdout);
  pid_t pid = fork();
  if (pid < 0) {
    return 2;
  }
  if (pid == 0) {
    close(master);
    int slave = no_ctty ? child_open_no_ctty(name) : child_open_ctty(name);
    if (slave < 0) {
      _exit(126);
    }
    /* macOS refuses TIOCSWINSZ on a master whose slave is not open yet, so size it through the slave */
    if (have_ws && ioctl(slave, TIOCSWINSZ, &ws) < 0) {
      _exit(126);
    }
    if (dup2(slave, 0) < 0 || dup2(slave, 1) < 0 || dup2(slave, 2) < 0) {
      _exit(126);
    }
    if (slave > 2) {
      close(slave);
    }
    if (no_ctty) {
      run_detached_leader();
    }
    execvp(cmd[0], cmd);
    _exit(127);
  }
  if (input != NULL) {
    char bytes[1024];
    size_t n = unescape(input, bytes);
    if (write_all(master, bytes, n) < 0) {
      fprintf(stderr, "pty_probe: cannot write input\n");
    }
  }
  int status = 0, exited = 0, timed_out = 0;
  int ticks = limit * 10;
  char buf[4096];
  for (;;) {
    struct pollfd pf = {.fd = master, .events = POLLIN};
    int pr = poll(&pf, 1, 100);
    if (pr > 0) {
      ssize_t n = read(master, buf, sizeof buf);
      if (n > 0) {
        if (write_all(1, buf, (size_t)n) < 0) {
          break;
        }
        continue;
      }
      /* end of file (macOS) or EIO (Linux): every slave descriptor is closed */
      break;
    }
    if (!exited && waitpid(pid, &status, WNOHANG) == pid) {
      exited = 1;
    }
    if (--ticks <= 0) {
      timed_out = 1;
      break;
    }
  }
  if (timed_out) {
    kill(-pid, SIGKILL);
  }
  if (!exited) {
    waitpid(pid, &status, 0);
  }
  if (timed_out) {
    return 124;
  }
  if (WIFSIGNALED(status)) {
    return 128 + WTERMSIG(status);
  }
  return WEXITSTATUS(status);
}

int main(int argc, char **argv) {
  alarm(LIMIT_SECONDS);
  if (argc < 2) {
    fprintf(stderr, "usage: pty_probe MODE\n");
    return 2;
  }
  const char *mode = argv[1];
  if (strcmp(mode, "pair") == 0) {
    return mode_pair();
  }
  if (strcmp(mode, "echo") == 0) {
    return mode_echo();
  }
  if (strcmp(mode, "slave-exit") == 0) {
    return mode_slave_exit();
  }
  if (strcmp(mode, "master-close") == 0 || strcmp(mode, "winsize") == 0 || strcmp(mode, "ctrl-c") == 0 ||
      strcmp(mode, "ctrl-c-raw") == 0 || strcmp(mode, "tiocsti") == 0) {
    return mode_with_child(mode);
  }
  if (strcmp(mode, "run") == 0) {
    return mode_run(argc, argv);
  }
  fprintf(stderr, "pty_probe: unknown mode %s\n", mode);
  return 2;
}
