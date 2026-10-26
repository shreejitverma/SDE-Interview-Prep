#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int s_pipe[2];

static void handler(int sig) {
  int saved_errno = errno;
  char b = (char)sig;
  if (write(s_pipe[1], &b, 1) < 0) {
    /* EAGAIN: the pipe is full, so a wake-up byte is already pending. */
  }
  errno = saved_errno;
}

int main(void) {
  if (pipe(s_pipe) < 0) {
    perror("pipe");
    return 1;
  }
  for (int i = 0; i < 2; i++) {
    int flags = fcntl(s_pipe[i], F_GETFL, 0);
    if (flags >= 0) {
      fcntl(s_pipe[i], F_SETFL, flags | O_NONBLOCK);
    }
    fcntl(s_pipe[i], F_SETFD, FD_CLOEXEC);
  }

  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGUSR1, &sa, NULL) < 0) {
    perror("sigaction");
    return 1;
  }

  raise(SIGUSR1);

  struct pollfd pfd;
  pfd.fd = s_pipe[0];
  pfd.events = POLLIN;
  pfd.revents = 0;

  int ret = poll(&pfd, 1, 1000);
  if (ret > 0 && (pfd.revents & POLLIN)) {
    char buf[16];
    ssize_t n = read(s_pipe[0], buf, sizeof(buf));
    if (n > 0) {
      printf("self_pipe_success\n");
    }
  } else {
    printf("poll_timeout\n");
  }

  close(s_pipe[0]);
  close(s_pipe[1]);
  return 0;
}
