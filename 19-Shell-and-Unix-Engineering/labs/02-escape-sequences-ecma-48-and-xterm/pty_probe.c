/*
 * pty_probe: play the terminal side of a pseudo-terminal pair.
 *
 *   pty_probe echo-ctl on|off
 *       Put the slave in canonical mode with ECHO and ECHOCTL set or cleared,
 *       write "a ESC [ A LF" to the master as if typed, and print what the line
 *       discipline echoed back.
 *
 *   pty_probe term ROW COL CMD [ARG...]
 *       Run CMD with the slave as its controlling terminal, stdin, stdout and
 *       stderr.  Act as a naive terminal: answer ESC [ 6 n with the cursor
 *       position report ESC [ ROW ; COL R, remember the text of the last
 *       ESC ] 2 ; TEXT (BEL or ESC \) and answer ESC [ 2 1 t with
 *       ESC ] l TEXT ESC \.  The replies are written to the master, so the
 *       child reads them from its input exactly as if they had been typed.
 *       Print the child's whole output transcript and its exit status.
 *
 * The transcript is printed with ESC as <ESC>, CR as \r, LF as \n and other
 * control bytes as \xNN.  Every wait is bounded; the child is killed at the
 * deadline.
 */
#ifdef __APPLE__
#define _DARWIN_C_SOURCE 1
#else
#define _GNU_SOURCE 1
#endif
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define TRANSCRIPT_MAX 8192
#define DEADLINE_SECONDS 15

static void print_escaped(const char *buf, size_t len) {
  size_t i;
  for (i = 0; i < len; i++) {
    unsigned char c = (unsigned char)buf[i];
    if (c == 0x1B) {
      fputs("<ESC>", stdout);
    } else if (c == '\r') {
      fputs("\\r", stdout);
    } else if (c == '\n') {
      fputs("\\n", stdout);
    } else if (c < 0x20 || c == 0x7F) {
      printf("\\x%02X", c);
    } else {
      putchar(c);
    }
  }
}

static int open_pair(int *master, int *slave, char *name, size_t name_len) {
  const char *sname;
  *master = posix_openpt(O_RDWR | O_NOCTTY);
  if (*master < 0) {
    return -1;
  }
  if (grantpt(*master) < 0 || unlockpt(*master) < 0) {
    return -1;
  }
  sname = ptsname(*master);
  if (sname == NULL) {
    return -1;
  }
  snprintf(name, name_len, "%s", sname);
  *slave = open(name, O_RDWR | O_NOCTTY);
  return *slave < 0 ? -1 : 0;
}

static double now(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

static int run_echo_ctl(int echoctl_on) {
  int master, slave;
  char name[256];
  struct termios t;
  char out[256];
  size_t got = 0;
  double deadline = now() + 5.0;
  const char typed[] = {'a', 0x1B, '[', 'A', '\n'};
  ssize_t w;

  if (open_pair(&master, &slave, name, sizeof(name)) < 0) {
    fprintf(stderr, "pty_probe: cannot open a pseudo-terminal: %s\n", strerror(errno));
    return 2;
  }
  if (tcgetattr(slave, &t) < 0) {
    fprintf(stderr, "pty_probe: tcgetattr: %s\n", strerror(errno));
    return 2;
  }
  t.c_lflag |= ICANON | ECHO;
  if (echoctl_on) {
    t.c_lflag |= ECHOCTL;
  } else {
    t.c_lflag &= ~(tcflag_t)ECHOCTL;
  }
  t.c_oflag |= OPOST | ONLCR;
  if (tcsetattr(slave, TCSANOW, &t) < 0) {
    fprintf(stderr, "pty_probe: tcsetattr: %s\n", strerror(errno));
    return 2;
  }
  w = write(master, typed, sizeof(typed));
  if (w != (ssize_t)sizeof(typed)) {
    fprintf(stderr, "pty_probe: short write to master\n");
    return 2;
  }
  /* Read until the echo of the final newline (CR LF) has arrived. */
  while (now() < deadline && got < sizeof(out) - 1) {
    struct pollfd pfd;
    ssize_t n;
    pfd.fd = master;
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, 200) <= 0) {
      continue;
    }
    n = read(master, out + got, sizeof(out) - 1 - got);
    if (n <= 0) {
      break;
    }
    got += (size_t)n;
    if (got >= 2 && out[got - 2] == '\r' && out[got - 1] == '\n') {
      break;
    }
  }
  fputs("echo=", stdout);
  print_escaped(out, got);
  putchar('\n');
  close(slave);
  close(master);
  return 0;
}

static const char *find_bytes(const char *buf, size_t len, const char *needle) {
  size_t nlen = strlen(needle);
  size_t i;
  if (nlen == 0 || len < nlen) {
    return NULL;
  }
  for (i = 0; i + nlen <= len; i++) {
    if (memcmp(buf + i, needle, nlen) == 0) {
      return buf + i;
    }
  }
  return NULL;
}

static int send_reply(int master, const char *reply, size_t len) {
  if (write(master, reply, len) != (ssize_t)len) {
    fprintf(stderr, "pty_probe: short write of a reply\n");
    return -1;
  }
  return 0;
}

static int run_term(int row, int col, char **cmd) {
  int master, slave;
  char name[256];
  pid_t pid;
  char transcript[TRANSCRIPT_MAX];
  size_t len = 0;
  int answered_cpr = 0;
  int answered_title = 0;
  char title[256];
  int status = 0;
  int child_done = 0;
  double deadline = now() + DEADLINE_SECONDS;

  title[0] = '\0';
  if (open_pair(&master, &slave, name, sizeof(name)) < 0) {
    fprintf(stderr, "pty_probe: cannot open a pseudo-terminal: %s\n", strerror(errno));
    return 2;
  }
  pid = fork();
  if (pid < 0) {
    return 2;
  }
  if (pid == 0) {
    int fd;
    close(master);
    if (setsid() < 0) {
      _exit(125);
    }
    fd = open(name, O_RDWR);
    if (fd < 0 || ioctl(fd, TIOCSCTTY, 0) < 0) {
      _exit(125);
    }
    close(slave);
    if (dup2(fd, 0) < 0 || dup2(fd, 1) < 0 || dup2(fd, 2) < 0) {
      _exit(125);
    }
    if (fd > 2) {
      close(fd);
    }
    execvp(cmd[0], cmd);
    _exit(127);
  }
  close(slave);

  while (now() < deadline && !child_done) {
    struct pollfd pfd;
    ssize_t n;
    pfd.fd = master;
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, 200) > 0) {
      n = read(master, transcript + len, sizeof(transcript) - 1 - len);
      if (n <= 0) {
        child_done = 1;
      } else {
        len += (size_t)n;
        transcript[len] = '\0';
      }
    }
    /* Look at the bytes that arrived since the last scan. */
    {
      const char *p = find_bytes(transcript, len, "\033]2;");
      if (p != NULL && title[0] == '\0') {
        const char *start = p + 4;
        const char *end = start;
        const char *limit = transcript + len;
        while (end < limit && *end != '\007' && *end != '\033') {
          end++;
        }
        if (end < limit) {
          size_t tl = (size_t)(end - start);
          if (tl >= sizeof(title)) {
            tl = sizeof(title) - 1;
          }
          memcpy(title, start, tl);
          title[tl] = '\0';
        }
      }
    }
    if (!answered_cpr && find_bytes(transcript, len, "\033[6n") != NULL) {
      char reply[64];
      int rl = snprintf(reply, sizeof(reply), "\033[%d;%dR", row, col);
      if (send_reply(master, reply, (size_t)rl) < 0) {
        break;
      }
      answered_cpr = 1;
    }
    if (!answered_title && title[0] != '\0' && find_bytes(transcript, len, "\033[21t") != NULL) {
      char reply[320];
      int rl = snprintf(reply, sizeof(reply), "\033]l%s\033\\", title);
      if (send_reply(master, reply, (size_t)rl) < 0) {
        break;
      }
      answered_title = 1;
    }
    if (len >= sizeof(transcript) - 1) {
      break;
    }
  }

  {
    double reap_by = now() + 5.0;
    pid_t r = 0;
    while (child_done && now() < reap_by) {
      r = waitpid(pid, &status, WNOHANG);
      if (r != 0) {
        break;
      }
      usleep(20000);
    }
    if (r <= 0) {
      /* Deadline, full buffer or a child that will not exit: stop it. */
      kill(pid, SIGKILL);
      if (waitpid(pid, &status, 0) < 0) {
        status = 0;
      }
      printf("deadline=hit\n");
    }
  }
  fputs("transcript=", stdout);
  print_escaped(transcript, len);
  putchar('\n');
  if (WIFEXITED(status)) {
    printf("child_status=%d\n", WEXITSTATUS(status));
  } else {
    printf("child_status=signal-%d\n", WIFSIGNALED(status) ? WTERMSIG(status) : -1);
  }
  close(master);
  return 0;
}

int main(int argc, char **argv) {
  if (argc == 3 && strcmp(argv[1], "echo-ctl") == 0) {
    if (strcmp(argv[2], "on") == 0) {
      return run_echo_ctl(1);
    }
    if (strcmp(argv[2], "off") == 0) {
      return run_echo_ctl(0);
    }
  }
  if (argc >= 5 && strcmp(argv[1], "term") == 0) {
    return run_term(atoi(argv[2]), atoi(argv[3]), argv + 4);
  }
  fprintf(stderr, "usage: pty_probe echo-ctl on|off | term ROW COL CMD [ARG...]\n");
  return 2;
}
