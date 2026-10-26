/*
 * argv-dump.c: dump argc and argv elements received from execve(2).
 * Verifies that shell quote removal leaves raw byte strings in argv.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int i;
  size_t j, len;

  if (printf("argc=%d\n", argc) < 0) {
    perror("printf");
    return 1;
  }

  for (i = 1; i < argc; i++) {
    len = strlen(argv[i]);
    if (printf("argv[%d]: len=%zu data=<", i, len) < 0) {
      perror("printf");
      return 1;
    }
    for (j = 0; j < len; j++) {
      unsigned char c = (unsigned char)argv[i][j];
      if (c == '\n') {
        if (fputs("\\n", stdout) == EOF) {
          perror("fputs");
          return 1;
        }
      } else if (c == '\\') {
        if (fputs("\\\\", stdout) == EOF) {
          perror("fputs");
          return 1;
        }
      } else {
        if (putchar(c) == EOF) {
          perror("putchar");
          return 1;
        }
      }
    }
    if (printf(">\n") < 0) {
      perror("printf");
      return 1;
    }
  }

  return 0;
}
