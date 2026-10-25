#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <time.h>

static double get_time_us(void) {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return (double)ts.tv_sec * 1000000.0 + (double)ts.tv_nsec / 1000.0;
}

static int run_spawn_with_actions(const char *outfile) {
  posix_spawn_file_actions_t actions;
  posix_spawnattr_t attr;
  sigset_t mask;
  pid_t pid;
  int err, status;

  err = posix_spawn_file_actions_init(&actions);
  if (err != 0) return err;

  /* Redirect standard output (fd 1) to outfile */
  err = posix_spawn_file_actions_addopen(&actions, STDOUT_FILENO, outfile,
                                         O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (err != 0) {
    posix_spawn_file_actions_destroy(&actions);
    return err;
  }

  err = posix_spawnattr_init(&attr);
  if (err != 0) {
    posix_spawn_file_actions_destroy(&actions);
    return err;
  }

  /* Block SIGUSR1 in the child via spawn attributes */
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  posix_spawnattr_setsigmask(&attr, &mask);
  posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSIGMASK);

  char *const argv[] = {"/bin/echo", "posix_spawn_file_actions_ok", NULL};
  extern char **environ;

  err = posix_spawn(&pid, "/bin/echo", &actions, &attr, argv, environ);
  posix_spawn_file_actions_destroy(&actions);
  posix_spawnattr_destroy(&attr);

  if (err != 0) return err;

  if (waitpid(pid, &status, 0) == -1) return errno;
  return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int reaped_exit_zero(pid_t pid) {
  int status;
  if (waitpid(pid, &status, 0) == -1) return 0;
  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

static int test_missing_binary(void) {
  pid_t pid;
  char *const argv[] = {"nonexistent_binary_xyz", NULL};
  extern char **environ;

  /* posix_spawn returns the error code directly */
  int err = posix_spawn(&pid, "/nonexistent_binary_xyz", NULL, NULL, argv, environ);
  return err;
}

int main(int argc, char **argv) {
  const char *mode = (argc > 1) ? argv[1] : "demo";

  if (strcmp(mode, "actions") == 0) {
    const char *outfile = (argc > 2) ? argv[2] : "/tmp/spawn_out.txt";
    int ret = run_spawn_with_actions(outfile);
    if (ret != 0) {
      fprintf(stderr, "run_spawn_with_actions failed: %d (%s)\n", ret, strerror(ret));
      return 1;
    }
    printf("spawn_actions_success\n");
    return 0;
  }

  if (strcmp(mode, "missing") == 0) {
    int err = test_missing_binary();
    printf("missing_err:%d\n", err);
    return (err == ENOENT) ? 0 : 1;
  }

  if (strcmp(mode, "bench") == 0) {
    int iterations = 100;
    char *const child_argv[] = {"/usr/bin/true", NULL};
    extern char **environ;
    double t0, t1;

    /* Measure posix_spawn */
    t0 = get_time_us();
    for (int i = 0; i < iterations; i++) {
      pid_t pid;
      int err = posix_spawn(&pid, "/usr/bin/true", NULL, NULL, child_argv, environ);
      if (err != 0) {
        fprintf(stderr, "posix_spawn failed: %d (%s)\n", err, strerror(err));
        return 1;
      }
      if (!reaped_exit_zero(pid)) {
        fprintf(stderr, "posix_spawn child did not exit 0\n");
        return 1;
      }
    }
    t1 = get_time_us();
    printf("posix_spawn_latency_us: %.1f\n", (t1 - t0) / iterations);

    /* Measure fork+exec */
    t0 = get_time_us();
    for (int i = 0; i < iterations; i++) {
      pid_t pid = fork();
      if (pid == -1) {
        fprintf(stderr, "fork failed: %d (%s)\n", errno, strerror(errno));
        return 1;
      }
      if (pid == 0) {
        execve("/usr/bin/true", child_argv, environ);
        _exit(127);
      }
      if (!reaped_exit_zero(pid)) {
        fprintf(stderr, "fork+exec child did not exit 0\n");
        return 1;
      }
    }
    t1 = get_time_us();
    printf("fork_exec_latency_us: %.1f\n", (t1 - t0) / iterations);
    return 0;
  }

  printf("Usage: %s {actions <outfile>|missing|bench}\n", argv[0]);
  return 2;
}
