#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t g_mode = 0;

static void handler(int sig) {
  (void)sig;
  if (g_mode == 1) {
    int saved = errno;
    close(-1);
    errno = saved;
  } else {
    close(-1);
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <safe|unsafe>\n", argv[0]);
    return 1;
  }
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, NULL) < 0) {
    perror("sigaction");
    return 1;
  }

  if (strcmp(argv[1], "safe") == 0) {
    g_mode = 1;
  } else if (strcmp(argv[1], "unsafe") == 0) {
    g_mode = 0;
  } else {
    fprintf(stderr, "Unknown mode: %s\n", argv[1]);
    return 1;
  }

  errno = ENOENT;
  raise(SIGUSR1);
  if (errno == ENOENT) {
    printf("preserved\n");
  } else {
    printf("clobbered\n");
  }
  return 0;
}
