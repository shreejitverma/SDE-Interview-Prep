#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*
 * daemon_probe: C probe exercising Unix process session, controlling terminal,
 * descriptor allocation, and daemonization mechanisms.
 */

static int mode_setsid_leader(void) {
  /*
   * Calling setsid() when already a process group leader must fail
   * with EPERM according to POSIX.1-2024 setsid (tag 17_532).
   */
  if (setpgid(0, 0) < 0) {
    perror("setpgid");
    return 1;
  }
  pid_t s = setsid();
  if (s == (pid_t)-1 && errno == EPERM) {
    printf("setsid_leader: EPERM\n");
    fflush(stdout);
    return 0;
  }
  fprintf(stderr, "unexpected setsid return: %ld, errno=%d (%s)\n",
          (long)s, errno, strerror(errno));
  fflush(stderr);
  return 1;
}

static int mode_setsid_child(void) {
  /*
   * Fork a child. The child's PID differs from its inherited PGID,
   * guaranteeing it is not a process group leader. setsid() must succeed.
   */
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    pid_t sid = setsid();
    if (sid < 0) {
      perror("child setsid");
      fflush(stderr);
      _exit(1);
    }
    pid_t my_pid = getpid();
    pid_t my_pgid = getpgrp();
    pid_t my_sid = getsid(0);
    if (sid != my_pid || my_pgid != my_pid || my_sid != my_pid) {
      fprintf(stderr, "mismatch: sid=%ld my_pid=%ld my_pgid=%ld my_sid=%ld\n",
              (long)sid, (long)my_pid, (long)my_pgid, (long)my_sid);
      fflush(stderr);
      _exit(2);
    }
    /* Verify controlling terminal detachment: opening /dev/tty must fail with ENXIO */
    int fd = open("/dev/tty", O_RDWR);
    if (fd >= 0) {
      close(fd);
      fprintf(stderr, "error: /dev/tty opened after setsid\n");
      fflush(stderr);
      _exit(3);
    }
    if (errno != ENXIO) {
      fprintf(stderr, "error: expected ENXIO on /dev/tty, got %d (%s)\n",
              errno, strerror(errno));
      fflush(stderr);
      _exit(4);
    }
    printf("setsid_child: ok\n");
    fflush(stdout);
    _exit(0);
  }
  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid");
    return 1;
  }
  return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : 1;
}

static int mode_double_fork(const char *outfile) {
  /*
   * Classical Stevens double-fork:
   * 1. First fork: parent exits immediately.
   * 2. Child 1: calls setsid(), becoming session leader.
   * 3. Second fork: Child 1 exits immediately.
   * 4. Grandchild: inherited session, but PID != SID (not session leader).
   *    Grandchild can never acquire a controlling terminal on open().
   */
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("pipe");
    return 1;
  }

  pid_t pid1 = fork();
  if (pid1 < 0) {
    perror("first fork");
    return 1;
  }

  if (pid1 == 0) {
    /* Child 1 */
    close(pipefd[0]);
    pid_t sid1 = setsid();
    if (sid1 < 0) {
      perror("child1 setsid");
      fflush(stderr);
      _exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
      perror("second fork");
      fflush(stderr);
      _exit(2);
    }

    if (pid2 == 0) {
      /* Grandchild (Child 2) */
      pid_t g_pid = getpid();
      pid_t g_pgid = getpgrp();
      pid_t g_sid = getsid(0);
      int is_leader = (g_pid == g_sid) ? 1 : 0;

      /* Attempt to open /dev/tty: must fail with ENXIO */
      int tty_fd = open("/dev/tty", O_RDWR);
      int tty_err = (tty_fd >= 0) ? 0 : errno;
      if (tty_fd >= 0) close(tty_fd);

      FILE *f = fopen(outfile, "w");
      if (f) {
        fprintf(f, "pid=%ld pgid=%ld sid=%ld is_leader=%d tty_err=%d\n",
                (long)g_pid, (long)g_pgid, (long)g_sid, is_leader, tty_err);
        fclose(f);
      }
      char notify = 'K';
      if (write(pipefd[1], &notify, 1) != 1) {
        /* write notification */
      }
      close(pipefd[1]);
      _exit(0);
    }

    /* Child 1 exits immediately so grandchild is orphaned */
    _exit(0);
  }

  /* Original Parent: wait for child 1 to terminate, and wait for grandchild notify */
  close(pipefd[1]);
  int status1 = 0;
  if (waitpid(pid1, &status1, 0) < 0) {
    perror("waitpid pid1");
    close(pipefd[0]);
    return 1;
  }

  char buf = 0;
  ssize_t n = read(pipefd[0], &buf, 1);
  close(pipefd[0]);
  if (n != 1 || buf != 'K') {
    fprintf(stderr, "failed to receive grandchild notification\n");
    fflush(stderr);
    return 1;
  }
  printf("double_fork: completed\n");
  fflush(stdout);
  return 0;
}

static int mode_fd_reuse(const char *tempfile) {
  /*
   * Case A: Close fd 0 without redirecting.
   * POSIX requires open() to allocate the lowest-numbered unused descriptor.
   * Therefore, open() must return 0!
   */
  if (close(0) < 0) {
    perror("close 0");
    return 1;
  }
  int fd_reused = open(tempfile, O_RDWR | O_CREAT | O_TRUNC, 0600);
  if (fd_reused < 0) {
    perror("open after close(0)");
    return 1;
  }
  printf("closed_fd0: allocated_fd=%d\n", fd_reused);
  fflush(stdout);

  /*
   * Case B: Redirect fd 0 to /dev/null using dup2().
   * Now fd 0 is safely occupied by /dev/null, not the file, and opening
   * another file must return >= 3.
   */
  int devnull = open("/dev/null", O_RDONLY);
  if (devnull < 0) {
    perror("open /dev/null");
    return 1;
  }
  if (dup2(devnull, 0) < 0) {
    perror("dup2");
    return 1;
  }
  if (devnull != 0) {
    close(devnull);
  }
  struct stat null_st, fd0_st;
  if (stat("/dev/null", &null_st) < 0 || fstat(0, &fd0_st) < 0) {
    perror("stat fd 0");
    return 1;
  }
  int fd0_is_null =
      fd0_st.st_dev == null_st.st_dev && fd0_st.st_ino == null_st.st_ino;

  int fd_safe = open(tempfile, O_RDWR);
  if (fd_safe < 0) {
    perror("open after dup2");
    return 1;
  }
  printf("redirected_fd0: fd0=%s allocated_fd=%d\n",
         fd0_is_null ? "devnull" : "other", fd_safe);
  fflush(stdout);
  close(fd_safe);
  return 0;
}

static int mode_print_attrs(void) {
  printf("pid=%ld ppid=%ld pgid=%ld sid=%ld\n",
         (long)getpid(), (long)getppid(), (long)getpgrp(), (long)getsid(0));
  fflush(stdout);
  return 0;
}

static int mode_check_ctty(void) {
  int fd = open("/dev/tty", O_RDWR);
  if (fd >= 0) {
    close(fd);
    printf("ctty: present\n");
    fflush(stdout);
    return 0;
  }
  if (errno == ENXIO) {
    printf("ctty: none (ENXIO)\n");
    fflush(stdout);
    return 0;
  }
  printf("ctty: error (%d: %s)\n", errno, strerror(errno));
  fflush(stdout);
  return 1;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <mode> [args...]\n", argv[0]);
    return 1;
  }
  const char *mode = argv[1];
  if (strcmp(mode, "setsid-leader") == 0) {
    return mode_setsid_leader();
  } else if (strcmp(mode, "setsid-child") == 0) {
    return mode_setsid_child();
  } else if (strcmp(mode, "double-fork") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s double-fork <outfile>\n", argv[0]);
      return 1;
    }
    return mode_double_fork(argv[2]);
  } else if (strcmp(mode, "fd-reuse") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Usage: %s fd-reuse <tempfile>\n", argv[0]);
      return 1;
    }
    return mode_fd_reuse(argv[2]);
  } else if (strcmp(mode, "print-attrs") == 0) {
    return mode_print_attrs();
  } else if (strcmp(mode, "check-ctty") == 0) {
    return mode_check_ctty();
  }

  fprintf(stderr, "Unknown mode: %s\n", mode);
  return 1;
}
