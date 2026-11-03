#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
  int fds[2];
  if (pipe(fds) < 0) {
    perror("pipe");
    return 1;
  }
  int flags = fcntl(fds[1], F_GETFL, 0);
  if (flags < 0 || fcntl(fds[1], F_SETFL, flags | O_NONBLOCK) < 0) {
    perror("fcntl");
    return 1;
  }
  char buf[512];
  for (int i = 0; i < 512; i++) {
    buf[i] = 'A';
  }
  long total = 0;
  while (1) {
    ssize_t n = write(fds[1], buf, sizeof(buf));
    if (n > 0) {
      total += n;
    } else {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        break;
      }
      perror("write");
      return 1;
    }
  }
  close(fds[0]);
  close(fds[1]);
  printf("pipe_capacity=%ld\n", total);
  return 0;
}
