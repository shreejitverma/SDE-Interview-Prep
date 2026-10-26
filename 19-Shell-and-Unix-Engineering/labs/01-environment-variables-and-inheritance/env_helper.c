#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;

static int is_near_stack(void *ptr, void *stack_ref) {
  intptr_t diff = (intptr_t)((uintptr_t)ptr - (uintptr_t)stack_ref);
  if (diff < 0) {
    diff = -diff;
  }
  /* On Unix systems, initial stack allocation places environ within 64 KiB of frame */
  return diff < 65536;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: %s {mem|getenv|getenv_dup|exec_dup|exec_invalid} [args...]\n", argv[0]);
    return 1;
  }

  const char *mode = argv[1];

  if (strcmp(mode, "mem") == 0) {
    int dummy = 0;
    int init_stack = is_near_stack((void *)environ, (void *)&dummy);
    printf("initial: %s\n", init_stack ? "stack" : "other");

    /* Calling setenv forces libc to relocate environ to heap */
    if (setenv("LAB_PROBE_HEAP_VAR", "allocated", 1) != 0) {
      perror("setenv");
      return 1;
    }
    int post_stack = is_near_stack((void *)environ, (void *)&dummy);
    printf("post-setenv: %s\n", post_stack ? "stack" : "heap");
    return 0;
  }

  if (strcmp(mode, "getenv") == 0) {
    if (argc < 3) {
      fprintf(stderr, "error: getenv requires a variable name\n");
      return 1;
    }
    const char *val = getenv(argv[2]);
    printf("getenv: %s\n", val ? val : "UNSET");
    return 0;
  }

  if (strcmp(mode, "getenv_dup") == 0) {
    if (argc < 3) {
      fprintf(stderr, "error: getenv_dup requires a variable name\n");
      return 1;
    }
    char *custom_envp[] = {
      (char *)"KEY=first",
      (char *)"KEY=second",
      NULL
    };
    char *child_argv[] = {
      argv[0],
      (char *)"getenv",
      argv[2],
      NULL
    };
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork");
      return 1;
    }
    if (pid == 0) {
      execve(argv[0], child_argv, custom_envp);
      perror("execve");
      _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
      perror("waitpid");
      return 1;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
  }

  if (strcmp(mode, "exec_dup") == 0) {
    if (argc < 4) {
      fprintf(stderr, "error: exec_dup requires shell words and command\n");
      return 1;
    }
    char *custom_envp[] = {
      (char *)"KEY=first",
      (char *)"KEY=second",
      (char *)"PATH=/usr/bin:/bin",
      NULL
    };
    /* argv[2 .. argc-2] are shell words, argv[argc-1] is script */
    int num_words = argc - 3;
    char **new_argv = malloc((size_t)(num_words + 3) * sizeof(char *));
    if (!new_argv) {
      perror("malloc");
      return 1;
    }
    int idx = 0;
    for (int i = 2; i < argc - 1; i++) {
      new_argv[idx++] = argv[i];
    }
    new_argv[idx++] = (char *)"-c";
    new_argv[idx++] = argv[argc - 1];
    new_argv[idx] = NULL;

    execve(argv[2], new_argv, custom_envp);
    perror("execve");
    free(new_argv);
    return 1;
  }

  if (strcmp(mode, "exec_invalid") == 0) {
    if (argc < 4) {
      fprintf(stderr, "error: exec_invalid requires shell words and command\n");
      return 1;
    }
    char *custom_envp[] = {
      (char *)"bad-ident=probe_value",
      (char *)"VALID_KEY=valid_value",
      (char *)"PATH=/usr/bin:/bin",
      NULL
    };
    int num_words = argc - 3;
    char **new_argv = malloc((size_t)(num_words + 3) * sizeof(char *));
    if (!new_argv) {
      perror("malloc");
      return 1;
    }
    int idx = 0;
    for (int i = 2; i < argc - 1; i++) {
      new_argv[idx++] = argv[i];
    }
    new_argv[idx++] = (char *)"-c";
    new_argv[idx++] = argv[argc - 1];
    new_argv[idx] = NULL;

    execve(argv[2], new_argv, custom_envp);
    perror("execve");
    free(new_argv);
    return 1;
  }

  fprintf(stderr, "unknown mode: %s\n", mode);
  return 1;
}
