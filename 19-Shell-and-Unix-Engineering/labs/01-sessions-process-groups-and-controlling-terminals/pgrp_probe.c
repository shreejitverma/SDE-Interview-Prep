#define _GNU_SOURCE /* glibc declares posix_openpt, grantpt, unlockpt, and ptsname only on request */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>

static void print_info(void) {
  pid_t pid = getpid();
  pid_t ppid = getppid();
  pid_t pgid = getpgrp();
  pid_t sid = getsid(0);
  pid_t tpgid = tcgetpgrp(0);
  int isatty_stdin = isatty(0);
  printf("pid=%ld ppid=%ld pgid=%ld sid=%ld tpgid=%ld isatty=%d\n",
         (long)pid, (long)ppid, (long)pgid, (long)sid, (long)tpgid, isatty_stdin);
}

/*
 * The lab runs with no controlling terminal, so the tests that need one make their own:
 * the parent opens a pseudo-terminal master and keeps it open (closing it would hang up the slave),
 * and a child that has just called setsid() makes the slave its controlling terminal.
 * The slave is opened with O_NOCTTY and acquired with TIOCSCTTY, which works the same on Linux, macOS,
 * and the BSDs (the BSDs never acquire a terminal on open).
 */
static int open_pty_master(char *name, size_t len) {
  int master = posix_openpt(O_RDWR | O_NOCTTY);
  if (master < 0) {
    return -1;
  }
  const char *slave = (grantpt(master) == 0 && unlockpt(master) == 0) ? ptsname(master) : NULL;
  if (slave == NULL) {
    close(master);
    return -1;
  }
  snprintf(name, len, "%s", slave);
  return master;
}

static int acquire_ctty(const char *name) {
  int slave = open(name, O_RDWR | O_NOCTTY);
  if (slave >= 0 && ioctl(slave, TIOCSCTTY, 0) < 0) {
    close(slave);
    return -1;
  }
  return slave;
}

static int devtty_errno(void) {
  int fd = open("/dev/tty", O_RDWR | O_NOCTTY);
  if (fd < 0) {
    return errno;
  }
  close(fd);
  return 0;
}

/* Wait for one line on fd for at most timeout_ms; return its length, or -1. */
static ssize_t read_report(int fd, char *buf, size_t len, int timeout_ms) {
  struct pollfd pf = {.fd = fd, .events = POLLIN};
  if (poll(&pf, 1, timeout_ms) <= 0) {
    return -1;
  }
  ssize_t n = read(fd, buf, len - 1);
  if (n > 0) {
    buf[n] = '\0';
  }
  return n;
}

/*
 * A session leader with a real controlling terminal opens /dev/tty, tries setsid() again
 * (EPERM: it is already a group leader), and forks a child that is not a group leader;
 * that child's setsid() succeeds and leaves it with no controlling terminal (ENXIO).
 */
static int do_setsid_test(void) {
  char name[128];
  int master = open_pty_master(name, sizeof name);
  if (master < 0) {
    printf("pty_failed errno=%d\n", errno);
    return 1;
  }
  fflush(stdout);
  pid_t child = fork();
  if (child < 0) {
    perror("fork");
    close(master);
    return 1;
  }
  if (child == 0) {
    close(master);
    if (setsid() < 0 || acquire_ctty(name) < 0) {
      printf("ctty_failed errno=%d\n", errno);
      _exit(1);
    }
    int before = devtty_errno();
    int again = (setsid() < 0) ? errno : 0;
    fflush(stdout);
    pid_t grandchild = fork();
    if (grandchild < 0) {
      _exit(3);
    }
    if (grandchild == 0) {
      int was_leader = (getpgrp() == getpid());
      if (setsid() < 0) {
        printf("setsid_failed errno=%d\n", errno);
        _exit(1);
      }
      printf("ctty_devtty_errno=%d leader_setsid_errno=%d child_was_leader=%d setsid_ok "
             "sid_is_pid=%d pgid_is_pid=%d devtty_errno=%d\n",
             before, again, was_leader, getsid(0) == getpid(), getpgrp() == getpid(), devtty_errno());
      fflush(stdout);
      _exit(0);
    }
    waitpid(grandchild, NULL, 0);
    _exit(0);
  }
  waitpid(child, NULL, 0);
  close(master);
  return 0;
}

/*
 * A session leader L takes the pseudo-terminal as its controlling terminal and stays alive.
 * L's child M moves to a new group in L's session and forks G, then exits: G's parent becomes
 * the reaper, in another session, so G's group (pgid M) is orphaned while still in L's session
 * and still a background group of L's terminal. G keeps SIGTTIN at its default, so only the
 * orphaned-group rule can turn its read into EIO; without that rule G would stop.
 * Every wait is bounded and both groups are killed afterwards.
 */
static int do_orphan_read_test(void) {
  char name[128];
  int master = open_pty_master(name, sizeof name);
  if (master < 0) {
    printf("pty_failed errno=%d\n", errno);
    return 1;
  }
  int report[2], group[2];
  if (pipe(report) < 0 || pipe(group) < 0) {
    perror("pipe");
    return 1;
  }
  fflush(stdout);
  pid_t leader = fork();
  if (leader < 0) {
    perror("fork");
    return 1;
  }
  if (leader == 0) {
    close(master);
    close(report[0]);
    close(group[0]);
    int slave = (setsid() < 0) ? -1 : acquire_ctty(name);
    if (slave < 0) {
      dprintf(report[1], "ctty_failed errno=%d\n", errno);
      _exit(1);
    }
    pid_t mid = fork();
    if (mid < 0) {
      _exit(3);
    }
    if (mid == 0) {
      pid_t me = getpid(); /* read before the fork: M may exit before G runs */
      if (setpgid(0, 0) < 0 || write(group[1], &me, sizeof me) != (ssize_t)sizeof me) {
        _exit(2);
      }
      pid_t grandchild = fork();
      if (grandchild < 0) {
        _exit(3);
      }
      if (grandchild == 0) {
        while (getppid() == me) {
          usleep(1000);
        }
        sigset_t ttin;
        sigemptyset(&ttin);
        sigaddset(&ttin, SIGTTIN);
        sigprocmask(SIG_UNBLOCK, &ttin, NULL);
        signal(SIGTTIN, SIG_DFL);
        int background = (tcgetpgrp(slave) != getpgrp());
        char b;
        ssize_t n = read(slave, &b, 1);
        dprintf(report[1], "orphan_read background=%d ret=%zd errno=%d\n", background, n,
                (n < 0) ? errno : 0);
        _exit(0);
      }
      _exit(0);
    }
    waitpid(mid, NULL, 0);
    for (;;) {
      pause(); /* stay the controlling process until the parent kills this group */
    }
  }
  close(report[1]);
  close(group[1]);
  pid_t orphan_pgid = 0;
  struct pollfd pf = {.fd = group[0], .events = POLLIN};
  if (poll(&pf, 1, 10000) > 0 &&
      read(group[0], &orphan_pgid, sizeof orphan_pgid) != (ssize_t)sizeof orphan_pgid) {
    orphan_pgid = 0;
  }
  char buf[128];
  if (read_report(report[0], buf, sizeof buf, 10000) > 0) {
    printf("%s", buf);
  } else {
    printf("orphan_read no_report\n");
  }
  if (orphan_pgid > 0) {
    kill(-orphan_pgid, SIGKILL);
  }
  kill(-leader, SIGKILL);
  waitpid(leader, NULL, 0);
  close(report[0]);
  close(group[0]);
  close(master);
  return 0;
}

/*
 * pty CMD [ARG...]: run CMD as the leader of a new session whose controlling terminal is a fresh
 * pseudo-terminal, with that terminal as its standard input; standard output and error are inherited.
 * Anything written to the terminal is drained and discarded; CMD is killed after 30 seconds.
 */
static int do_pty_run(char **argv) {
  char name[128];
  int master = open_pty_master(name, sizeof name);
  if (master < 0) {
    fprintf(stderr, "pty_failed errno=%d\n", errno);
    return 126;
  }
  fflush(stdout);
  pid_t child = fork();
  if (child < 0) {
    perror("fork");
    return 126;
  }
  if (child == 0) {
    close(master);
    int slave = (setsid() < 0) ? -1 : acquire_ctty(name);
    if (slave < 0 || dup2(slave, 0) < 0) {
      fprintf(stderr, "ctty_failed errno=%d\n", errno);
      _exit(126);
    }
    if (slave > 0) {
      close(slave);
    }
    execvp(argv[0], argv);
    fprintf(stderr, "exec %s: %s\n", argv[0], strerror(errno));
    _exit(127);
  }
  int status = 0;
  for (int waited_ms = 0;; waited_ms += 50) {
    pid_t done = waitpid(child, &status, WNOHANG);
    if (done == child) {
      break;
    }
    if (waited_ms >= 30000) {
      kill(-child, SIGKILL);
      waitpid(child, &status, 0);
      break;
    }
    char sink[256];
    struct pollfd pf = {.fd = master, .events = POLLIN};
    if (poll(&pf, 1, 50) > 0 && read(master, sink, sizeof sink) < 0) {
      usleep(50000); /* the slave is not open yet or already closed */
    }
  }
  close(master);
  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  return 128 + (WIFSIGNALED(status) ? WTERMSIG(status) : 0);
}

/*
 * The child starts a new session, so the grandchild's group is orphaned once the child exits:
 * its only member's parent is then the reaper, which is in another session.
 * (A plain setpgid would not do: a reaper in the same session keeps the group unorphaned.)
 * The read is bounded, so a grandchild that stops (or never reports) is killed, never waited on.
 */
static int do_orphan_stop_test(void) {
  int pfd[2];
  if (pipe(pfd) < 0) {
    perror("pipe");
    return 1;
  }
  pid_t child = fork();
  if (child < 0) {
    perror("fork");
    close(pfd[0]);
    close(pfd[1]);
    return 1;
  }
  if (child == 0) {
    close(pfd[0]);
    if (setsid() < 0) {
      _exit(2);
    }
    pid_t leader = getpid(); /* read before the fork: the leader may exit before the grandchild runs */
    pid_t grandchild = fork();
    if (grandchild < 0) {
      _exit(3);
    }
    if (grandchild == 0) {
      while (getppid() == leader) {
        usleep(1000);
      }
      signal(SIGTSTP, SIG_DFL);
      kill(getpid(), SIGTSTP);
      if (write(pfd[1], "discarded\n", 10) < 0) {
        _exit(4);
      }
      _exit(0);
    }
    _exit(0);
  }
  close(pfd[1]);
  waitpid(child, NULL, 0);
  struct pollfd pf = {.fd = pfd[0], .events = POLLIN};
  int ready = poll(&pf, 1, 10000);
  char buf[32];
  ssize_t n = (ready > 0) ? read(pfd[0], buf, sizeof(buf) - 1) : -1;
  if (n > 0) {
    buf[n] = '\0';
    printf("orphan_tstp %s", buf);
  } else {
    printf("orphan_tstp no_report\n");
  }
  kill(-child, SIGKILL); /* the session's group id is the child's pid */
  close(pfd[0]);
  return 0;
}

static int do_traps_test(void) {
  struct sigaction sa_int, sa_quit;
  if (sigaction(SIGINT, NULL, &sa_int) < 0) {
    return 1;
  }
  if (sigaction(SIGQUIT, NULL, &sa_quit) < 0) {
    return 1;
  }
  const char *int_desc = (sa_int.sa_handler == SIG_IGN) ? "IGN" : "DFL";
  const char *quit_desc = (sa_quit.sa_handler == SIG_IGN) ? "IGN" : "DFL";
  printf("sigint=%s sigquit=%s\n", int_desc, quit_desc);
  return 0;
}

int main(int argc, char **argv) {
  const char *cmd = (argc > 1) ? argv[1] : "info";

  if (strcmp(cmd, "info") == 0) {
    print_info();
    return 0;
  }
  if (strcmp(cmd, "setsid") == 0) {
    return do_setsid_test();
  }
  if (strcmp(cmd, "orphan_read") == 0) {
    return do_orphan_read_test();
  }
  if (strcmp(cmd, "orphan_stop") == 0) {
    return do_orphan_stop_test();
  }
  if (strcmp(cmd, "traps") == 0) {
    return do_traps_test();
  }
  if (strcmp(cmd, "pty") == 0 && argc > 2) {
    return do_pty_run(argv + 2);
  }

  fprintf(stderr, "unknown cmd: %s\n", cmd);
  return 1;
}
