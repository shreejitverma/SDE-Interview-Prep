#define _XOPEN_SOURCE 700
#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <poll.h>

enum { SIGTTOU_HANDLED, SIGTTOU_IGNORED, SIGTTOU_BLOCKED };

static volatile sig_atomic_t g_caught_sigttou = 0;

static void sigttou_handler(int sig) {
  (void)sig;
  g_caught_sigttou = 1;
}

/*
 * Real terminals always have a window size.
 * A fresh pseudo-terminal has 0 rows and 0 columns, which BusyBox stty reports as an error,
 * so callers that model an interactive terminal pass with_size = 1 (24 rows, 80 columns).
 */
static int open_pty_pair(int *master_fd, int *slave_fd, int with_size) {
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
  if (with_size) {
    struct winsize ws;
    memset(&ws, 0, sizeof(ws));
    ws.ws_row = 24;
    ws.ws_col = 80;
    if (ioctl(s, TIOCSWINSZ, &ws) < 0) {
      close(s);
      close(m);
      return -1;
    }
  }
  *master_fd = m;
  *slave_fd = s;
  return 0;
}

/* Wait up to timeout_ms for fd to become readable; returns 1 when readable, 0 on timeout. */
static int wait_readable(int fd, int timeout_ms) {
  struct pollfd p;
  int r;
  p.fd = fd;
  p.events = POLLIN;
  p.revents = 0;
  do {
    r = poll(&p, 1, timeout_ms);
  } while (r < 0 && errno == EINTR);
  return r > 0 && (p.revents & POLLIN) != 0;
}

static const char *errno_name(int e) {
  static char other[32];
  if (e == 0) {
    return "none";
  } else if (e == EINTR) {
    return "EINTR";
  } else if (e == EIO) {
    return "EIO";
  }
  snprintf(other, sizeof(other), "errno%d", e);
  return other;
}

static int do_termios_probe(void) {
  int m, s;
  if (open_pty_pair(&m, &s, 1) < 0) {
    fprintf(stderr, "open_pty_pair failed\n");
    return 1;
  }

  struct termios tio;
  if (tcgetattr(s, &tio) != 0) {
    fprintf(stderr, "tcgetattr failed: %s\n", strerror(errno));
    close(s);
    close(m);
    return 1;
  }
  printf("tcgetattr=OK\n");

  /* Test TCSANOW: disable ECHO */
  tio.c_lflag &= ~(tcflag_t)ECHO;
  if (tcsetattr(s, TCSANOW, &tio) != 0) {
    fprintf(stderr, "tcsetattr TCSANOW failed: %s\n", strerror(errno));
    close(s);
    close(m);
    return 1;
  }

  struct termios verify_tio;
  if (tcgetattr(s, &verify_tio) != 0) {
    close(s);
    close(m);
    return 1;
  }
  if ((verify_tio.c_lflag & ECHO) == 0) {
    printf("tcsanow_echo_off=OK\n");
  } else {
    printf("tcsanow_echo_off=FAIL\n");
  }

  /* Test tcflush */
  if (tcflush(s, TCIFLUSH) == 0 && tcflush(s, TCOFLUSH) == 0) {
    printf("tcflush=OK\n");
  } else {
    printf("tcflush=FAIL\n");
  }

  /* Test tcflow */
  if (tcflow(s, TCOOFF) == 0 && tcflow(s, TCOON) == 0) {
    printf("tcflow=OK\n");
  } else {
    printf("tcflow=FAIL\n");
  }

  close(s);
  close(m);
  return 0;
}

static int do_verify_tcsaflush(void) {
  int m, s;
  if (open_pty_pair(&m, &s, 1) < 0) {
    return 1;
  }

  /* Put slave into non-canonical mode with ECHO disabled so bytes are readable immediately */
  struct termios tio;
  if (tcgetattr(s, &tio) != 0) {
    close(s);
    close(m);
    return 1;
  }
  tio.c_lflag &= ~(tcflag_t)(ICANON | ECHO);
  tio.c_cc[VMIN] = 1;
  tio.c_cc[VTIME] = 0;
  if (tcsetattr(s, TCSANOW, &tio) != 0) {
    close(s);
    close(m);
    return 1;
  }

  /* Write initial bytes into master; these land in slave's input queue */
  const char discard_payload[] = "DISCARDED";
  if (write(m, discard_payload, strlen(discard_payload)) < 0) {
    close(s);
    close(m);
    return 1;
  }

  /* Wait (bounded) until the bytes are queued and readable, so the flush has something to discard */
  if (!wait_readable(s, 3000)) {
    printf("tcsaflush_setup_failed=1 reason=input_never_queued\n");
    close(s);
    close(m);
    return 1;
  }

  /* Now call tcsetattr with TCSAFLUSH; this must flush unread input */
  tio.c_iflag ^= (tcflag_t)IGNBRK; /* Minor toggle */
  if (tcsetattr(s, TCSAFLUSH, &tio) != 0) {
    close(s);
    close(m);
    return 1;
  }

  /* Write new payload */
  const char accept_payload[] = "ACCEPTED\n";
  if (write(m, accept_payload, strlen(accept_payload)) < 0) {
    close(s);
    close(m);
    return 1;
  }

  /* Read from slave until the newline arrives or a bounded wait expires */
  char buf[64];
  size_t used = 0;
  memset(buf, 0, sizeof(buf));
  while (used < sizeof(buf) - 1 && strchr(buf, '\n') == NULL && wait_readable(s, 3000)) {
    ssize_t n = read(s, buf + used, sizeof(buf) - 1 - used);
    if (n <= 0) {
      break;
    }
    used += (size_t)n;
  }

  int rc = 1;
  if (used == 0) {
    printf("tcsaflush_read_failed=1\n");
  } else if (strstr(buf, "DISCARDED") == NULL && strstr(buf, "ACCEPTED") != NULL) {
    printf("tcsaflush_discarded=1 accepted_read=ACCEPTED\n");
    rc = 0;
  } else {
    printf("tcsaflush_discarded=0 buf=%.*s\n", (int)used, buf);
  }

  close(s);
  close(m);
  return rc;
}

/*
 * A background process group calls tcsetattr on its session's controlling terminal.
 * disposition SIGTTOU_HANDLED: SIGTTOU has a handler; the call must fail with EINTR and change nothing.
 * disposition SIGTTOU_IGNORED: SIGTTOU is ignored; the call must succeed and change the terminal.
 * disposition SIGTTOU_BLOCKED: SIGTTOU is blocked; the call must succeed and the signal stays unhandled.
 * The master descriptor stays open until the whole tree has exited: on Linux closing the last
 * master hangs up the slave, the session loses its controlling terminal, and no SIGTTOU is generated.
 */
static int do_verify_sigttou(int disposition) {
  int m, s;
  if (open_pty_pair(&m, &s, 1) < 0) {
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    close(s);
    close(m);
    return 1;
  }

  if (pid == 0) {
    close(m);
    if (setsid() < 0) {
      _exit(2);
    }
    /* The pty slave becomes the controlling terminal of the new session; the leader is foreground */
    if (ioctl(s, TIOCSCTTY, 0) < 0) {
      _exit(6);
    }
    if (tcsetpgrp(s, getpid()) < 0) {
      _exit(7);
    }

    pid_t gpid = fork();
    if (gpid < 0) {
      _exit(3);
    }
    if (gpid == 0) {
      if (setpgid(0, 0) < 0) {
        _exit(4);
      }
      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = disposition == SIGTTOU_IGNORED ? SIG_IGN : sigttou_handler;
      sigemptyset(&sa.sa_mask);
      if (sigaction(SIGTTOU, &sa, NULL) < 0) {
        _exit(5);
      }
      if (disposition == SIGTTOU_BLOCKED) {
        sigset_t block;
        sigemptyset(&block);
        sigaddset(&block, SIGTTOU);
        if (sigprocmask(SIG_BLOCK, &block, NULL) < 0) {
          _exit(10);
        }
      }

      struct termios gtio;
      if (tcgetattr(s, &gtio) != 0) {
        _exit(8);
      }
      tcflag_t echo_before = gtio.c_lflag & (tcflag_t)ECHO;
      gtio.c_lflag ^= (tcflag_t)ECHO;
      errno = 0;
      int rc = tcsetattr(s, TCSANOW, &gtio);
      int err = errno;

      struct termios after;
      if (tcgetattr(s, &after) != 0) {
        _exit(9);
      }
      int changed = (after.c_lflag & (tcflag_t)ECHO) != echo_before;

      printf("CAUGHT_SIGTTOU=%d tcsetattr=%s errno=%s changed=%d\n", (int)g_caught_sigttou,
             rc == 0 ? "OK" : "FAIL", errno_name(rc == 0 ? 0 : err), changed);
      if (disposition != SIGTTOU_HANDLED) {
        _exit(rc == 0 && changed && !g_caught_sigttou ? 0 : 1);
      }
      _exit(rc != 0 && err == EINTR && !changed && g_caught_sigttou ? 0 : 1);
    }

    int gstatus = 0;
    waitpid(gpid, &gstatus, 0);
    _exit(WIFEXITED(gstatus) ? WEXITSTATUS(gstatus) : 1);
  }

  close(s);
  int status = 0;
  waitpid(pid, &status, 0);
  close(m);
  return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

/*
 * Close the last master descriptor and probe the termios calls on the slave.
 * Both kernels hang the slave up (reads give end of file, writes fail with EIO).
 * Linux then fails tcgetattr and tcsetattr with EIO; macOS still serves them.
 */
static int do_verify_hangup(void) {
  int m, s;
  if (open_pty_pair(&m, &s, 1) < 0) {
    return 1;
  }

  struct termios tio;
  if (tcgetattr(s, &tio) != 0) {
    close(s);
    close(m);
    return 1;
  }
  close(m);

  errno = 0;
  int get_rc = tcgetattr(s, &tio);
  int get_err = errno;
  tio.c_lflag ^= (tcflag_t)ECHO;
  errno = 0;
  int set_rc = tcsetattr(s, TCSANOW, &tio);
  int set_err = errno;

  printf("master_closed tcgetattr=%s tcsetattr=%s\n", get_rc == 0 ? "OK" : errno_name(get_err),
         set_rc == 0 ? "OK" : errno_name(set_err));
  close(s);
  return 0;
}

static int do_pty_exec(int argc, char **argv, int with_size) {
  if (argc < 3) {
    fprintf(stderr, "Usage: pty-exec <cmd> [args...]\n");
    return 1;
  }

  int m, s;
  if (open_pty_pair(&m, &s, with_size) < 0) {
    perror("open_pty_pair");
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    close(s);
    close(m);
    return 1;
  }

  if (pid == 0) {
    close(m);
    if (setsid() < 0) {
      /* continue anyway */
    }
#ifdef TIOCSCTTY
    (void)ioctl(s, TIOCSCTTY, 0);
#endif
    if (dup2(s, STDIN_FILENO) < 0 || dup2(s, STDOUT_FILENO) < 0 || dup2(s, STDERR_FILENO) < 0) {
      _exit(126);
    }
    if (s > STDERR_FILENO) {
      close(s);
    }
    execvp(argv[2], &argv[2]);
    _exit(127);
  }

  close(s);

  /* Parent: relay output from master pty to stdout until child exits */
  char buf[512];
  ssize_t nr;
  while ((nr = read(m, buf, sizeof(buf))) > 0) {
    ssize_t written = 0;
    while (written < nr) {
      ssize_t nw = write(STDOUT_FILENO, buf + written, (size_t)(nr - written));
      if (nw <= 0) {
        break;
      }
      written += nw;
    }
  }

  close(m);
  int status = 0;
  waitpid(pid, &status, 0);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  } else if (WIFSIGNALED(status)) {
    return 128 + WTERMSIG(status);
  }
  return 1;
}

int main(int argc, char **argv) {
  setvbuf(stdout, NULL, _IONBF, 0);
  if (argc < 2) {
    fprintf(stderr,
            "Usage: %s <termios-probe|verify-tcsaflush|verify-sigttou|verify-sigttou-ignored|"
            "verify-sigttou-blocked|verify-hangup|pty-exec|pty-exec-nosize> [args...]\n",
            argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "termios-probe") == 0) {
    return do_termios_probe();
  } else if (strcmp(argv[1], "verify-tcsaflush") == 0) {
    return do_verify_tcsaflush();
  } else if (strcmp(argv[1], "verify-sigttou") == 0) {
    return do_verify_sigttou(SIGTTOU_HANDLED);
  } else if (strcmp(argv[1], "verify-sigttou-ignored") == 0) {
    return do_verify_sigttou(SIGTTOU_IGNORED);
  } else if (strcmp(argv[1], "verify-sigttou-blocked") == 0) {
    return do_verify_sigttou(SIGTTOU_BLOCKED);
  } else if (strcmp(argv[1], "verify-hangup") == 0) {
    return do_verify_hangup();
  } else if (strcmp(argv[1], "pty-exec") == 0) {
    return do_pty_exec(argc, argv, 1);
  } else if (strcmp(argv[1], "pty-exec-nosize") == 0) {
    return do_pty_exec(argc, argv, 0);
  }

  fprintf(stderr, "Unknown command: %s\n", argv[1]);
  return 1;
}
