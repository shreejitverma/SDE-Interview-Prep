/*
 * zombie_orphan_probe.c - Kernel process state and reaping probe.
 *
 * Demonstrates:
 *   - zombie state (EXIT_ZOMBIE / <defunct>) prior to waitpid
 *   - orphan reparenting to init (PID 1) or nearest subreaper
 *   - automatic reaping and ECHILD under SA_NOCLDWAIT
 *   - SIGHUP and SIGCONT kernel delivery to stopped orphaned process groups
 */

#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 600

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t g_got_sighup = 0;
static volatile sig_atomic_t g_got_sigcont = 0;

static void on_signal(int sig) {
  if (sig == SIGHUP) {
    g_got_sighup = 1;
  } else if (sig == SIGCONT) {
    g_got_sigcont = 1;
  }
}

static int run_zombie_probe(void) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    /* Child terminates immediately with exit code 42 */
    _exit(42);
  }

  /* Parent announces child pid */
  printf("ZOMBIE_PID=%d\n", (int)pid);
  fflush(stdout);

  /* Parent sleeps 3 seconds to allow caller / ps inspection */
  sleep(3);

  /* Parent reaps zombie */
  int status = 0;
  pid_t reaped = waitpid(pid, &status, 0);
  if (reaped < 0) {
    perror("waitpid");
    return 1;
  }

  if (WIFEXITED(status)) {
    printf("REAPED=%d\n", WEXITSTATUS(status));
  } else {
    printf("REAPED_SIGNAL=%d\n", WTERMSIG(status));
  }
  fflush(stdout);
  return 0;
}

static int run_orphan_probe(void) {
  pid_t original_parent = getpid();
  int pipefd[2];
  if (pipe(pipefd) < 0) {
    perror("pipe");
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    /* Child process */
    close(pipefd[1]);
    /* Wait for parent to close its write end / exit */
    char dummy;
    while (read(pipefd[0], &dummy, 1) > 0) {
      /* wait */
    }
    close(pipefd[0]);

    /* Give the kernel time to complete reparenting */
    sleep(1);

    pid_t new_ppid = getppid();
    printf("ORIGINAL_PARENT=%d\n", (int)original_parent);
    printf("REPARENTED_PARENT=%d\n", (int)new_ppid);
    if (new_ppid != original_parent) {
      printf("ORPHAN_REPARENTED=1\n");
    } else {
      printf("ORPHAN_REPARENTED=0\n");
    }
    fflush(stdout);
    _exit(0);
  }

  /* Parent closes read end, prints child PID, and exits */
  close(pipefd[0]);
  printf("CHILD_PID=%d\n", (int)pid);
  fflush(stdout);
  close(pipefd[1]);
  _exit(0);
}

static int run_nocldwait_probe(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = SIG_IGN;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_NOCLDWAIT;
  if (sigaction(SIGCHLD, &sa, NULL) < 0) {
    perror("sigaction");
    return 1;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }
  if (pid == 0) {
    _exit(42);
  }

  printf("NOCLDWAIT_PID=%d\n", (int)pid);
  fflush(stdout);

  /* Sleep 1 second to ensure child termination and automatic kernel discard */
  sleep(1);

  int status = 0;
  pid_t ret = waitpid(pid, &status, 0);
  if (ret < 0 && errno == ECHILD) {
    printf("NOCLDWAIT_ECHILD=1\n");
  } else {
    printf("NOCLDWAIT_ECHILD=0 ret=%d errno=%d\n", (int)ret, errno);
  }
  fflush(stdout);
  return 0;
}

static int run_orphaned_pgrp_probe(void) {
  int ready_pipe[2];
  if (pipe(ready_pipe) < 0) {
    perror("pipe");
    return 1;
  }

  pid_t intermediate = fork();
  if (intermediate < 0) {
    perror("fork");
    return 1;
  }

  if (intermediate == 0) {
    /* Intermediate creates new session */
    if (setsid() < 0) {
      perror("setsid");
      _exit(1);
    }

    pid_t gc = fork();
    if (gc < 0) {
      perror("fork");
      _exit(1);
    }

    if (gc == 0) {
      /* Grandchild: leader of its own process group in this session */
      if (setpgid(0, 0) < 0) {
        perror("setpgid");
        _exit(1);
      }

      struct sigaction sa;
      memset(&sa, 0, sizeof(sa));
      sa.sa_handler = on_signal;
      sigemptyset(&sa.sa_mask);
      sa.sa_flags = 0;
      sigaction(SIGHUP, &sa, NULL);
      sigaction(SIGCONT, &sa, NULL);

      /* Tell intermediate we are ready and about to stop */
      char byte = 'K';
      if (write(ready_pipe[1], &byte, 1) != 1) {
        _exit(1);
      }
      close(ready_pipe[1]);
      close(ready_pipe[0]);

      /* Stop process */
      raise(SIGSTOP);

      /* Woken up by SIGCONT from kernel upon parent termination */
      printf("SIGHUP=%d\n", (int)g_got_sighup);
      printf("SIGCONT=%d\n", (int)g_got_sigcont);
      fflush(stdout);
      _exit(0);
    }

    /* Intermediate waits for grandchild ready */
    close(ready_pipe[1]);
    char byte = 0;
    if (read(ready_pipe[0], &byte, 1) != 1) {
      _exit(1);
    }
    close(ready_pipe[0]);

    /* Wait until grandchild is stopped */
    int status = 0;
    if (waitpid(gc, &status, WUNTRACED) < 0) {
      _exit(1);
    }

    /* Intermediate exits, orphaning grandchild process group */
    _exit(0);
  }

  close(ready_pipe[0]);
  close(ready_pipe[1]);

  /* Wait for intermediate to terminate */
  waitpid(intermediate, NULL, 0);

  /* Give grandchild time to receive signals, run handler, and print output */
  sleep(1);
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <zombie|orphan|nocldwait|orphaned_pgrp>\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "zombie") == 0) {
    return run_zombie_probe();
  }
  if (strcmp(argv[1], "orphan") == 0) {
    return run_orphan_probe();
  }
  if (strcmp(argv[1], "nocldwait") == 0) {
    return run_nocldwait_probe();
  }
  if (strcmp(argv[1], "orphaned_pgrp") == 0) {
    return run_orphaned_pgrp_probe();
  }

  fprintf(stderr, "unknown mode: %s\n", argv[1]);
  return 1;
}
