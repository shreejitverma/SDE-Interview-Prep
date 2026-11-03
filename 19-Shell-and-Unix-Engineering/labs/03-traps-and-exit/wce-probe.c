/*
 * wce-probe.c: demonstrates the difference between exit(130) and termination
 * by SIGINT for waitpid status inspection (WIFEXITED vs WIFSIGNALED).
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s <exit|signal>\n", argv[0]);
    return 2;
  }

  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    return 1;
  }

  if (pid == 0) {
    if (strcmp(argv[1], "exit") == 0) {
      exit(130);
    } else if (strcmp(argv[1], "signal") == 0) {
      signal(SIGINT, SIG_DFL);
      if (kill(getpid(), SIGINT) < 0) {
        perror("kill");
      }
      pause();
      exit(1);
    } else {
      fprintf(stderr, "unknown mode: %s\n", argv[1]);
      exit(2);
    }
  }

  int status = 0;
  if (waitpid(pid, &status, 0) < 0) {
    perror("waitpid");
    return 1;
  }

  if (WIFEXITED(status)) {
    printf("status=exited code=%d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("status=signaled sig=%d\n", WTERMSIG(status));
  } else {
    printf("status=other\n");
  }

  return 0;
}
