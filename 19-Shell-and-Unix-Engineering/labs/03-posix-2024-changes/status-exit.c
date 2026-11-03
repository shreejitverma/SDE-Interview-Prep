/*
 * status-exit.c: deterministic external helper for POSIX.1-2024 changes lab.
 * Provides guaranteed exit codes and null-delimited streams
 * to avoid shell builtin optimization or subshell quirks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    if (fprintf(stderr, "Usage: %s <exit N | stream-nul>\n", argv[0]) < 0) {
      return 1;
    }
    return 1;
  }

  if (strcmp(argv[1], "exit") == 0 && argc >= 3) {
    int code = atoi(argv[2]);
    return code;
  }

  if (strcmp(argv[1], "stream-nul") == 0) {
    const char rec1[] = "alpha\nitem";
    const char rec2[] = "beta item";
    if (fwrite(rec1, 1, sizeof(rec1), stdout) != sizeof(rec1)) {
      perror("fwrite");
      return 1;
    }
    if (fwrite(rec2, 1, sizeof(rec2), stdout) != sizeof(rec2)) {
      perror("fwrite");
      return 1;
    }
    return 0;
  }

  if (fprintf(stderr, "Unknown command: %s\n", argv[1]) < 0) {
    return 1;
  }
  return 1;
}
