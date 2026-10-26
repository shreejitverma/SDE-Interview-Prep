/*
 * inspect_wstatus.c: demonstrate kernel wait status bitfields and waitid siginfo.
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void usage(const char *prog) {
  fprintf(stderr,
          "Usage: %s {exit_val <n>|kill_sig <signo>|stop_sig|waitid_probe <n>}\n",
          prog);
  exit(2);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    usage(argv[0]);
  }

  const char *mode = argv[1];

  if (strcmp(mode, "exit_val") == 0) {
    if (argc < 3) {
      usage(argv[0]);
    }
    int val = atoi(argv[2]);
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return 1;
    }
    if (pid == 0) {
      _exit(val);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      return 1;
    }
    printf("raw=0x%04x WIFEXITED=%d WEXITSTATUS=%d\n",
           status,
           WIFEXITED(status) ? 1 : 0,
           WEXITSTATUS(status));
    return 0;
  }

  if (strcmp(mode, "kill_sig") == 0) {
    if (argc < 3) {
      usage(argv[0]);
    }
    int signo = atoi(argv[2]);
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return 1;
    }
    if (pid == 0) {
      kill(getpid(), signo);
      pause();
      _exit(1);
    }
    int status = 0;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid");
      return 1;
    }
    int coredump = 0;
#ifdef WCOREDUMP
    coredump = WCOREDUMP(status) ? 1 : 0;
#endif
    printf("raw=0x%04x WIFSIGNALED=%d WTERMSIG=%d WCOREDUMP=%d\n",
           status,
           WIFSIGNALED(status) ? 1 : 0,
           WTERMSIG(status),
           coredump);
    return 0;
  }

  if (strcmp(mode, "stop_sig") == 0) {
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return 1;
    }
    if (pid == 0) {
      kill(getpid(), SIGSTOP);
      _exit(0);
    }
    int status = 0;
    if (waitpid(pid, &status, WUNTRACED) == -1) {
      perror("waitpid");
      return 1;
    }
    printf("raw=0x%04x WIFSTOPPED=%d WSTOPSIG=%d\n",
           status,
           WIFSTOPPED(status) ? 1 : 0,
           WSTOPSIG(status));
    kill(pid, SIGKILL);
    waitpid(pid, &status, 0);
    return 0;
  }

  if (strcmp(mode, "waitid_probe") == 0) {
    if (argc < 3) {
      usage(argv[0]);
    }
    int val = atoi(argv[2]);
    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      return 1;
    }
    if (pid == 0) {
      _exit(val);
    }
    siginfo_t info;
    memset(&info, 0, sizeof(info));
    if (waitid(P_PID, (id_t)pid, &info, WEXITED) == -1) {
      perror("waitid");
      return 1;
    }
    printf("si_pid=%ld si_code=%d si_status=%d\n",
           (long)info.si_pid,
           info.si_code,
           info.si_status);
    return 0;
  }

  usage(argv[0]);
  return 2;
}
