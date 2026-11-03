#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int target_fd = 3;
  if (argc > 1) {
    target_fd = atoi(argv[1]);
  }
  int flags = fcntl(target_fd, F_GETFL);
  if (flags < 0) {
    perror("fcntl F_GETFL");
    return 1;
  }
  if (argc > 2 && argv[2][0] == 's') {
    if (fcntl(target_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
      perror("fcntl F_SETFL");
      return 1;
    }
    printf("set_nonblock\n");
  } else {
    printf("fd %d nonblock: %s\n", target_fd,
           (flags & O_NONBLOCK) ? "yes" : "no");
  }
  return 0;
}
