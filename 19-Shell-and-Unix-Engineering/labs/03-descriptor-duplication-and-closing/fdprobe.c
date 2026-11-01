/*
 * fdprobe: print the number of every file descriptor this process has open, in 0..63.
 * An external program is the only honest witness of what a shell passed to a child:
 * the shell's own builtins can see descriptors that exec would never hand on.
 * Usage: fdprobe        prints e.g. "0 1 2 3 4 8 " followed by a newline
 */
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
  int fd;

  for (fd = 0; fd < 64; fd++) {
    if (fcntl(fd, F_GETFD) != -1) {
      printf("%d ", fd);
    }
  }
  printf("\n");
  return 0;
}
