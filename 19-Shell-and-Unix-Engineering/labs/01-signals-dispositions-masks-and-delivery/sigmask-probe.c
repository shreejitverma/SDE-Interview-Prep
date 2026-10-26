/*
 * sigmask-probe.c: kernel interface probe for signal dispositions, masks, and delivery.
 * Probes sigaction(2), sigprocmask(2), execve(2) state inheritance, and EINTR.
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void dummy_handler(int signo) {
  (void)signo;
}

static const char *handler_str(void (*h)(int)) {
  if (h == SIG_IGN) {
    return "IGN";
  }
  if (h == SIG_DFL) {
    return "DFL";
  }
  return "CATCH";
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <mode>\n", argv[0]);
    return 1;
  }

  const char *mode = argv[1];

  if (strcmp(mode, "check_disposition") == 0) {
    struct sigaction sa_int, sa_quit, sa_term;
    memset(&sa_int, 0, sizeof(sa_int));
    memset(&sa_quit, 0, sizeof(sa_quit));
    memset(&sa_term, 0, sizeof(sa_term));

    if (sigaction(SIGINT, NULL, &sa_int) != 0 ||
        sigaction(SIGQUIT, NULL, &sa_quit) != 0 ||
        sigaction(SIGTERM, NULL, &sa_term) != 0) {
      perror("sigaction");
      return 1;
    }

    printf("int=%s quit=%s term=%s\n",
           handler_str(sa_int.sa_handler),
           handler_str(sa_quit.sa_handler),
           handler_str(sa_term.sa_handler));
    return 0;
  }

  if (strcmp(mode, "check_mask") == 0) {
    sigset_t cur;
    sigemptyset(&cur);
    if (sigprocmask(SIG_SETMASK, NULL, &cur) != 0) {
      perror("sigprocmask");
      return 1;
    }
    printf("usr1_blocked=%s\n", sigismember(&cur, SIGUSR1) ? "yes" : "no");
    return 0;
  }

  if (strcmp(mode, "check_handler") == 0) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    if (sigaction(SIGUSR1, NULL, &sa) != 0) {
      perror("sigaction");
      return 1;
    }
    printf("usr1_handler=%s\n", handler_str(sa.sa_handler));
    return 0;
  }

  if (strcmp(mode, "test_exec_mask") == 0) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) != 0) {
      perror("sigprocmask");
      return 1;
    }

    char *child_argv[] = {argv[0], "check_mask", NULL};
    execv(argv[0], child_argv);
    perror("execv");
    return 1;
  }

  if (strcmp(mode, "exec_blocked") == 0) {
    /* Block SIGUSR1, then run CMD: shows whether CMD (a shell) keeps or clears the mask. */
    if (argc < 3) {
      fprintf(stderr, "usage: %s exec_blocked CMD [ARG...]\n", argv[0]);
      return 1;
    }
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    if (sigprocmask(SIG_BLOCK, &set, NULL) != 0) {
      perror("sigprocmask");
      return 1;
    }
    execvp(argv[2], argv + 2);
    perror("execvp");
    return 127;
  }

  if (strcmp(mode, "test_exec_catch_to_dfl") == 0) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = dummy_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) != 0) {
      perror("sigaction");
      return 1;
    }

    char *child_argv[] = {argv[0], "check_handler", NULL};
    execv(argv[0], child_argv);
    perror("execv");
    return 1;
  }

  if (strcmp(mode, "test_exec_preserves_ign") == 0) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) != 0) {
      perror("sigaction");
      return 1;
    }

    char *child_argv[] = {argv[0], "check_handler", NULL};
    execv(argv[0], child_argv);
    perror("execv");
    return 1;
  }

  if (strcmp(mode, "test_eintr") == 0) {
    int pfd[2];
    if (pipe(pfd) < 0) {
      perror("pipe");
      return 1;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = dummy_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; /* No SA_RESTART */
    if (sigaction(SIGALRM, &sa, NULL) != 0) {
      perror("sigaction");
      return 1;
    }

    alarm(1);
    char buf[8];
    ssize_t n = read(pfd[0], buf, sizeof(buf));
    if (n < 0 && errno == EINTR) {
      printf("read_interrupted=EINTR\n");
    } else {
      printf("read_result=%zd errno=%d\n", n, errno);
    }
    close(pfd[0]);
    close(pfd[1]);
    return 0;
  }

  fprintf(stderr, "unknown mode: %s\n", mode);
  return 1;
}
