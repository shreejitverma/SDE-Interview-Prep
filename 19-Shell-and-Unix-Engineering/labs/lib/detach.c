/*
 * detach: run a command as the leader of a new session, with default signal state.
 *
 * Usage: detach CMD [ARG...]
 *
 * run-matrix.sh starts each shell's suite through this, so every suite starts the same way
 * on every host: no controlling terminal, every catchable signal at its default disposition,
 * and an empty signal mask, whatever the caller ignored or blocked.
 * A shell cannot do this itself: a non-interactive shell may not reset a signal that was
 * ignored on entry, and it ignores SIGINT and SIGQUIT in its own background jobs.
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "usage: detach CMD [ARG...]\n");
    return 2;
  }
  if (setsid() < 0) {
    fprintf(stderr, "detach: setsid: %s\n", strerror(errno));
    return 2;
  }
  struct sigaction dfl;
  memset(&dfl, 0, sizeof dfl);
  dfl.sa_handler = SIG_DFL;
  sigemptyset(&dfl.sa_mask);
  for (int sig = 1; sig < NSIG; sig++) {
    if (sig != SIGKILL && sig != SIGSTOP) {
      (void)sigaction(sig, &dfl, NULL); /* numbers the libc reserves fail with EINVAL; skip them */
    }
  }
  sigset_t none;
  sigemptyset(&none);
  sigprocmask(SIG_SETMASK, &none, NULL);
  execvp(argv[1], argv + 1);
  fprintf(stderr, "detach: %s: %s\n", argv[1], strerror(errno));
  return 127;
}
