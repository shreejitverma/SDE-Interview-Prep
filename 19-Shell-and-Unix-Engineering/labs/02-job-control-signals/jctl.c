/*
 * jctl: probes for the job-control signal lab.
 *
 *   jctl pty OUTDIR [--send MARKER=OCTALS | --hangup MARKER]... --shell WORDS -- ARGS...
 *       run WORDS ARGS in a new session whose controlling terminal is a fresh pseudo-terminal,
 *       type bytes at its master end when MARKER (a file in OUTDIR) appears, and report the exit
 *   jctl clean [--own-group] --shell WORDS -- ARGS...
 *       reset signal dispositions and the mask to defaults, then exec WORDS ARGS;
 *       with --own-group, run it as a child in a new, non-orphaned process group and wait for it
 *   jctl rec LOGDIR NAME ROLE       record dispositions and every job-control signal received
 *   jctl disp LABEL                 print the dispositions this process inherited
 *   jctl ttyread LOG MODE           read the controlling terminal from a background group
 *   jctl ttywrite LOG MODE          write or tcsetattr the controlling terminal from a background group
 *   jctl spin LOG MODE              append a tick to LOG every 100 ms
 *   jctl handoff SHELLPID           say whether this job owns the terminal and shares the shell group
 *   jctl state PID                  print R, S, T, Z, or ? for the process state
 *   jctl size FILE                  print a file size in bytes, or -1
 *   jctl msleep MS                  sleep
 *
 * Every mode that can wait arms alarm(25) first, so nothing outlives a failed test.
 */
#define _GNU_SOURCE
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
#include <time.h>
#include <unistd.h>
#ifdef __APPLE__
#include <sys/sysctl.h>
#endif

#define MAX_WORDS 16
#define MAX_STEPS 8

static volatile sig_atomic_t g_hup;
static volatile sig_atomic_t g_write_failed;
static int g_log_fd = -1;

static long mono_ms(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (long)ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static void sleep_ms(long ms) {
  struct timespec ts;
  ts.tv_sec = ms / 1000;
  ts.tv_nsec = (ms % 1000) * 1000000L;
  nanosleep(&ts, NULL);
}

static int file_exists(const char *path) {
  struct stat st;
  return stat(path, &st) == 0;
}

static const char *sig_name(int sig) {
  switch (sig) {
  case SIGINT:
    return "INT";
  case SIGQUIT:
    return "QUIT";
  case SIGTSTP:
    return "TSTP";
  case SIGCONT:
    return "CONT";
  case SIGTTIN:
    return "TTIN";
  case SIGTTOU:
    return "TTOU";
  case SIGHUP:
    return "HUP";
  default:
    return "OTHER";
  }
}

static const char *err_name(int err) {
  switch (err) {
  case EIO:
    return "EIO";
  case EINTR:
    return "EINTR";
  case EAGAIN:
    return "EAGAIN";
  case ENOTTY:
    return "ENOTTY";
  default:
    return "E?";
  }
}

static const char *disp_of(int sig) {
  struct sigaction sa;
  if (sigaction(sig, NULL, &sa) < 0) {
    return "ERR";
  }
  if (sa.sa_handler == SIG_IGN) {
    return "IGN";
  }
  if (sa.sa_handler == SIG_DFL) {
    return "DFL";
  }
  return "HANDLER";
}

static void print_disp(FILE *out, const char *label) {
  fprintf(out, "%s INT=%s QUIT=%s TSTP=%s TTIN=%s TTOU=%s CONT=%s HUP=%s\n", label, disp_of(SIGINT),
          disp_of(SIGQUIT), disp_of(SIGTSTP), disp_of(SIGTTIN), disp_of(SIGTTOU), disp_of(SIGCONT),
          disp_of(SIGHUP));
}

static void on_signal(int sig) {
  int saved = errno;
  const char *name = sig_name(sig);
  char buf[32];
  size_t n = 4;
  size_t len = strlen(name);
  memcpy(buf, "sig ", 4);
  memcpy(buf + n, name, len);
  n += len;
  buf[n++] = '\n';
  if (write(g_log_fd, buf, n) < 0) {
    g_write_failed = 1;
  }
  if (sig == SIGHUP) {
    g_hup = 1;
  }
  errno = saved;
}

static int open_log(const char *path) {
  return open(path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
}

/* Wait until this process is (want_fg) or is not (!want_fg) in the foreground group of tty. */
static int wait_role(int tty, int want_fg, long timeout_ms) {
  long deadline = mono_ms() + timeout_ms;
  for (;;) {
    pid_t tp = tcgetpgrp(tty);
    int is_fg = (tp == getpgrp());
    if (tp > 0 && is_fg == want_fg) {
      return 1;
    }
    if (mono_ms() > deadline) {
      return 0;
    }
    sleep_ms(20);
  }
}

static int cmd_rec(int argc, char **argv) {
  char path[512];
  char ready[512];
  char stop[512];
  int want_fg;
  int tty;
  long deadline;
  int sigs[] = {SIGINT, SIGQUIT, SIGTSTP, SIGCONT, SIGTTIN, SIGTTOU, SIGHUP};
  size_t i;
  if (argc != 5) {
    return 2;
  }
  alarm(25);
  snprintf(path, sizeof(path), "%s/%s.log", argv[2], argv[3]);
  snprintf(ready, sizeof(ready), "%s/%s.ready", argv[2], argv[3]);
  snprintf(stop, sizeof(stop), "%s/stop", argv[2]);
  want_fg = strcmp(argv[4], "fg") == 0;
  g_log_fd = open_log(path);
  if (g_log_fd < 0) {
    return 3;
  }
  dprintf(g_log_fd, "init INT=%s QUIT=%s TSTP=%s TTIN=%s TTOU=%s\n", disp_of(SIGINT), disp_of(SIGQUIT),
          disp_of(SIGTSTP), disp_of(SIGTTIN), disp_of(SIGTTOU));
  for (i = 0; i < sizeof(sigs) / sizeof(sigs[0]); i++) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    sigaction(sigs[i], &sa, NULL);
  }
  tty = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (tty < 0 || !wait_role(tty, want_fg, 5000)) {
    dprintf(g_log_fd, "notready role=%s\n", argv[4]);
    return 4;
  }
  dprintf(g_log_fd, "ready role=%s pgrp=%ld\n", argv[4], (long)getpgrp());
  {
    int fd = open(ready, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
      return 3;
    }
    close(fd);
  }
  deadline = mono_ms() + 20000;
  while (!file_exists(stop) && !g_hup && mono_ms() < deadline) {
    sleep_ms(50);
  }
  dprintf(g_log_fd, "exit write_failed=%d\n", (int)g_write_failed);
  return 0;
}

static int cmd_ttyread(int argc, char **argv) {
  int tty;
  char c;
  ssize_t n;
  if (argc != 4) {
    return 2;
  }
  alarm(25);
  g_log_fd = open_log(argv[2]);
  tty = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (g_log_fd < 0 || tty < 0 || !wait_role(tty, 0, 5000)) {
    return 4;
  }
  dprintf(g_log_fd, "background pgrp=%ld\n", (long)getpgrp());
  if (strcmp(argv[3], "ign") == 0) {
    signal(SIGTTIN, SIG_IGN);
  } else if (strcmp(argv[3], "blk") == 0) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTTIN);
    sigprocmask(SIG_BLOCK, &set, NULL);
  }
  n = read(tty, &c, 1);
  dprintf(g_log_fd, "read ret=%ld err=%s\n", (long)n, n < 0 ? err_name(errno) : "-");
  return 0;
}

static int cmd_ttywrite(int argc, char **argv) {
  int tty;
  ssize_t n;
  if (argc != 4) {
    return 2;
  }
  alarm(25);
  g_log_fd = open_log(argv[2]);
  tty = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (g_log_fd < 0 || tty < 0 || !wait_role(tty, 0, 5000)) {
    return 4;
  }
  dprintf(g_log_fd, "background pgrp=%ld\n", (long)getpgrp());
  if (strcmp(argv[3], "tcset") == 0) {
    struct termios t;
    if (tcgetattr(tty, &t) < 0) {
      return 5;
    }
    n = tcsetattr(tty, TCSANOW, &t);
    dprintf(g_log_fd, "after tcsetattr ret=%ld err=%s\n", (long)n, n < 0 ? err_name(errno) : "-");
    return 0;
  }
  if (strcmp(argv[3], "ign") == 0) {
    signal(SIGTTOU, SIG_IGN);
  }
  n = write(tty, "<<tty-write>>", 13);
  dprintf(g_log_fd, "after write ret=%ld err=%s\n", (long)n, n < 0 ? err_name(errno) : "-");
  return 0;
}

static int cmd_spin(int argc, char **argv) {
  if (argc != 4) {
    return 2;
  }
  alarm(30);
  g_log_fd = open_log(argv[2]);
  if (g_log_fd < 0) {
    return 3;
  }
  if (strcmp(argv[3], "igncont") == 0) {
    signal(SIGCONT, SIG_IGN);
  } else if (strcmp(argv[3], "igntstp") == 0) {
    signal(SIGTSTP, SIG_IGN);
  }
  for (;;) {
    if (write(g_log_fd, ".", 1) < 0) {
      return 3;
    }
    sleep_ms(100);
  }
}

static int cmd_handoff(int argc, char **argv) {
  int tty;
  pid_t shell;
  if (argc != 3) {
    return 2;
  }
  alarm(25);
  shell = (pid_t)strtol(argv[2], NULL, 10);
  tty = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (tty < 0) {
    return 4;
  }
  printf("job_in_tty_fg=%s job_shares_shell_pgrp=%s\n", tcgetpgrp(tty) == getpgrp() ? "yes" : "no",
         getpgid(shell) == getpgrp() ? "yes" : "no");
  return 0;
}

static int cmd_state(int argc, char **argv) {
  long pid;
  char st = '?';
  if (argc != 3) {
    return 2;
  }
  pid = strtol(argv[2], NULL, 10);
#ifdef __APPLE__
  {
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, (int)pid};
    struct kinfo_proc kp;
    size_t len = sizeof(kp);
    memset(&kp, 0, sizeof(kp));
    if (sysctl(mib, 4, &kp, &len, NULL, 0) == 0 && len > 0) {
      switch (kp.kp_proc.p_stat) {
      case SSTOP:
        st = 'T';
        break;
      case SZOMB:
        st = 'Z';
        break;
      case SRUN:
        st = 'R';
        break;
      default:
        st = 'S';
        break;
      }
    }
  }
#else
  {
    char path[64];
    char buf[512];
    FILE *f;
    char *p;
    snprintf(path, sizeof(path), "/proc/%ld/stat", pid);
    f = fopen(path, "r");
    if (f != NULL) {
      size_t n = fread(buf, 1, sizeof(buf) - 1, f);
      fclose(f);
      buf[n] = '\0';
      p = strrchr(buf, ')');
      if (p != NULL && p[1] == ' ' && p[2] != '\0') {
        st = p[2];
      }
    }
  }
#endif
  printf("%c\n", st);
  return 0;
}

static int cmd_size(int argc, char **argv) {
  struct stat st;
  if (argc != 3) {
    return 2;
  }
  if (stat(argv[2], &st) < 0) {
    printf("-1\n");
  } else {
    printf("%ld\n", (long)st.st_size);
  }
  return 0;
}

static int split_words(char *s, char **out, int max) {
  int n = 0;
  char *tok = strtok(s, " ");
  while (tok != NULL && n < max) {
    out[n++] = tok;
    tok = strtok(NULL, " ");
  }
  return n;
}

static void reset_signals(void) {
  int sig;
  sigset_t empty;
  for (sig = 1; sig < 32; sig++) {
    if (sig != SIGKILL && sig != SIGSTOP) {
      signal(sig, SIG_DFL);
    }
  }
  sigemptyset(&empty);
  sigprocmask(SIG_SETMASK, &empty, NULL);
}

/* Parse "--shell WORDS -- ARGS..." starting at argv[i]; fill exec_argv; return 0 on success. */
static int build_exec(int argc, char **argv, int i, char **exec_argv, char *wordbuf, size_t wordbuf_size) {
  int nwords = 0;
  int j;
  if (i + 3 > argc || strcmp(argv[i], "--shell") != 0) {
    return -1;
  }
  snprintf(wordbuf, wordbuf_size, "%s", argv[i + 1]);
  nwords = split_words(wordbuf, exec_argv, MAX_WORDS - 1);
  if (nwords == 0 || strcmp(argv[i + 2], "--") != 0) {
    return -1;
  }
  for (j = i + 3; j < argc && nwords < 63; j++) {
    exec_argv[nwords++] = argv[j];
  }
  exec_argv[nwords] = NULL;
  return 0;
}

/* Run the command in its own process group, with this process as a parent in another group of the same
 * session. The group is then not orphaned, so SIGTSTP, SIGTTIN and SIGTTOU stop its members; in an
 * orphaned group (the default for everything below a detached session leader) the kernel discards them. */
static int run_in_own_group(char **exec_argv) {
  pid_t child;
  int status = 0;
  alarm(60);
  child = fork();
  if (child < 0) {
    perror("fork");
    return 127;
  }
  if (child == 0) {
    setpgid(0, 0);
    alarm(0);
    reset_signals();
    execvp(exec_argv[0], exec_argv);
    perror("execvp");
    _exit(127);
  }
  setpgid(child, child);
  while (waitpid(child, &status, 0) < 0 && errno == EINTR) {
  }
  kill(-child, SIGKILL);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return 128 + (WIFSIGNALED(status) ? WTERMSIG(status) : 0);
}

static int cmd_clean(int argc, char **argv) {
  char *exec_argv[64];
  char wordbuf[256];
  int i = 2;
  int own_group = 0;
  if (i < argc && strcmp(argv[i], "--own-group") == 0) {
    own_group = 1;
    i++;
  }
  if (build_exec(argc, argv, i, exec_argv, wordbuf, sizeof(wordbuf)) < 0) {
    return 2;
  }
  if (own_group) {
    return run_in_own_group(exec_argv);
  }
  reset_signals();
  execvp(exec_argv[0], exec_argv);
  perror("execvp");
  return 127;
}

struct step {
  char marker[256];
  int hangup;
  char bytes[64];
  size_t nbytes;
};

static size_t parse_octals(const char *s, char *out, size_t max) {
  size_t n = 0;
  while (*s != '\0' && n < max) {
    if (s[0] == '\\' && s[1] >= '0' && s[1] <= '7') {
      int v = 0;
      int digits = 0;
      s++;
      while (digits < 3 && *s >= '0' && *s <= '7') {
        v = v * 8 + (*s - '0');
        s++;
        digits++;
      }
      out[n++] = (char)v;
    } else {
      out[n++] = *s++;
    }
  }
  return n;
}

static int cmd_pty(int argc, char **argv) {
  const char *outdir;
  struct step steps[MAX_STEPS];
  int nsteps = 0;
  int i = 3;
  char *exec_argv[64];
  char wordbuf[256];
  char slave_name[256];
  char path[512];
  char buf[4096];
  int master;
  int out_fd;
  pid_t child;
  int exited = 0;
  int status = 0;
  int cur = 0;
  int sent_any = 0;
  long marker_seen = 0;
  long last_action = 0;
  long settle_until = 0;
  long phase_start;
  int settled = 0;
  int timed_out = 0;

  if (argc < 4) {
    return 2;
  }
  alarm(60);
  outdir = argv[2];
  memset(steps, 0, sizeof(steps));
  while (i < argc && strcmp(argv[i], "--shell") != 0) {
    if (nsteps >= MAX_STEPS) {
      return 2;
    }
    if (strcmp(argv[i], "--send") == 0 && i + 1 < argc) {
      char *eq = strchr(argv[i + 1], '=');
      if (eq == NULL) {
        return 2;
      }
      snprintf(steps[nsteps].marker, sizeof(steps[nsteps].marker), "%s/%.*s", outdir, (int)(eq - argv[i + 1]),
               argv[i + 1]);
      steps[nsteps].nbytes = parse_octals(eq + 1, steps[nsteps].bytes, sizeof(steps[nsteps].bytes));
      nsteps++;
      i += 2;
    } else if (strcmp(argv[i], "--hangup") == 0 && i + 1 < argc) {
      snprintf(steps[nsteps].marker, sizeof(steps[nsteps].marker), "%s/%s", outdir, argv[i + 1]);
      steps[nsteps].hangup = 1;
      nsteps++;
      i += 2;
    } else {
      return 2;
    }
  }
  if (build_exec(argc, argv, i, exec_argv, wordbuf, sizeof(wordbuf)) < 0) {
    return 2;
  }

  master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0 || grantpt(master) < 0 || unlockpt(master) < 0) {
    perror("pty");
    return 3;
  }
  snprintf(slave_name, sizeof(slave_name), "%s", ptsname(master));
  snprintf(path, sizeof(path), "%s/tty.out", outdir);
  out_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (out_fd < 0) {
    perror("open tty.out");
    return 3;
  }

  child = fork();
  if (child < 0) {
    perror("fork");
    return 3;
  }
  if (child == 0) {
    int slave;
    struct termios t;
    close(master);
    if (setsid() < 0) {
      _exit(126);
    }
    slave = open(slave_name, O_RDWR);
    if (slave < 0 || ioctl(slave, TIOCSCTTY, 0) < 0) {
      _exit(126);
    }
    if (tcgetattr(slave, &t) == 0) {
      t.c_lflag |= ISIG | ICANON;
      t.c_lflag &= ~(tcflag_t)TOSTOP;
      t.c_cc[VINTR] = 3;
      t.c_cc[VQUIT] = 28;
      t.c_cc[VSUSP] = 26;
      if (tcsetattr(slave, TCSANOW, &t) < 0) {
        _exit(126);
      }
    }
    dup2(slave, 0);
    dup2(slave, 1);
    dup2(slave, 2);
    if (slave > 2) {
      close(slave);
    }
    reset_signals();
    setenv("LOGDIR", outdir, 1);
    execvp(exec_argv[0], exec_argv);
    _exit(127);
  }

  phase_start = mono_ms();
  while (1) {
    long now;
    struct pollfd pf;
    if (master >= 0) {
      ssize_t n = 0;
      pf.fd = master;
      pf.events = POLLIN;
      pf.revents = 0;
      if (poll(&pf, 1, 50) > 0 && (pf.revents & POLLIN)) {
        n = read(master, buf, sizeof(buf));
        if (n > 0 && write(out_fd, buf, (size_t)n) < 0) {
          perror("write tty.out");
        }
      }
      if (n <= 0) {
        sleep_ms(20);
      }
    } else {
      sleep_ms(50);
    }
    if (!exited && waitpid(child, &status, WNOHANG) == child) {
      exited = 1;
    }
    if (exited) {
      break;
    }
    now = mono_ms();
    if (cur < nsteps) {
      if (file_exists(steps[cur].marker)) {
        if (marker_seen == 0) {
          marker_seen = now;
        }
        if (now >= marker_seen + 500 && now >= last_action + 500) {
          if (steps[cur].hangup) {
            close(master);
            master = -1;
          } else if (write(master, steps[cur].bytes, steps[cur].nbytes) < 0) {
            perror("write master");
          }
          sent_any = 1;
          last_action = now;
          cur++;
          phase_start = now;
        }
      } else if (now > phase_start + 15000) {
        printf("marker_timeout %s\n", steps[cur].marker);
        timed_out = 1;
        break;
      }
      continue;
    }
    if (!settled) {
      if (sent_any && master >= 0) {
        if (settle_until == 0) {
          settle_until = now + 1000;
        }
        if (now < settle_until) {
          continue;
        }
      }
      snprintf(path, sizeof(path), "%s/stop", outdir);
      {
        int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd >= 0) {
          close(fd);
        }
      }
      settled = 1;
      phase_start = now;
      continue;
    }
    if (now > phase_start + 20000) {
      printf("timeout waiting for child\n");
      timed_out = 1;
      break;
    }
  }
  snprintf(path, sizeof(path), "%s/stop", outdir);
  {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
      close(fd);
    }
  }
  if (timed_out) {
    kill(-child, SIGKILL);
    waitpid(child, &status, 0);
    return 3;
  }
  if (WIFEXITED(status)) {
    printf("child_exit=%d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("child_signal=%d\n", WTERMSIG(status));
  }
  return 0;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    return 2;
  }
  if (strcmp(argv[1], "pty") == 0) {
    return cmd_pty(argc, argv);
  }
  if (strcmp(argv[1], "clean") == 0) {
    return cmd_clean(argc, argv);
  }
  if (strcmp(argv[1], "rec") == 0) {
    return cmd_rec(argc, argv);
  }
  if (strcmp(argv[1], "disp") == 0 && argc == 3) {
    print_disp(stdout, argv[2]);
    return 0;
  }
  if (strcmp(argv[1], "ttyread") == 0) {
    return cmd_ttyread(argc, argv);
  }
  if (strcmp(argv[1], "ttywrite") == 0) {
    return cmd_ttywrite(argc, argv);
  }
  if (strcmp(argv[1], "spin") == 0) {
    return cmd_spin(argc, argv);
  }
  if (strcmp(argv[1], "handoff") == 0) {
    return cmd_handoff(argc, argv);
  }
  if (strcmp(argv[1], "state") == 0) {
    return cmd_state(argc, argv);
  }
  if (strcmp(argv[1], "size") == 0) {
    return cmd_size(argc, argv);
  }
  if (strcmp(argv[1], "msleep") == 0 && argc == 3) {
    sleep_ms(strtol(argv[2], NULL, 10));
    return 0;
  }
  fprintf(stderr, "jctl: unknown mode\n");
  return 2;
}
