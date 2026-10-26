/*
 * byte-dump.c: inspect exact byte streams and execve(2) argument boundaries.
 * Verifies dollar-single-quote decoding, null byte termination, and hex escapes.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc > 1 && strcmp(argv[1], "-s") == 0) {
    unsigned char buf[4096];
    unsigned char data[65536];
    size_t total = 0;
    ssize_t n;

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
      if (total + (size_t)n <= sizeof(data)) {
        memcpy(data + total, buf, (size_t)n);
      }
      total += (size_t)n;
    }
    if (n < 0) {
      perror("read");
      return 1;
    }

    if (printf("stdin: count=%zu hex=", total) < 0) {
      perror("printf");
      return 1;
    }
    size_t limit = total < sizeof(data) ? total : sizeof(data);
    for (size_t i = 0; i < limit; i++) {
      if (printf(" %02x", data[i]) < 0) {
        perror("printf");
        return 1;
      }
    }
    if (printf("\n") < 0) {
      perror("printf");
      return 1;
    }
    return 0;
  }

  if (printf("argc=%d\n", argc) < 0) {
    perror("printf");
    return 1;
  }
  for (int i = 1; i < argc; i++) {
    size_t len = strlen(argv[i]);
    if (printf("argv[%d]: len=%zu hex=", i, len) < 0) {
      perror("printf");
      return 1;
    }
    for (size_t j = 0; j < len; j++) {
      if (printf(" %02x", (unsigned char)argv[i][j]) < 0) {
        perror("printf");
        return 1;
      }
    }
    if (printf("\n") < 0) {
      perror("printf");
      return 1;
    }
  }

  return 0;
}
