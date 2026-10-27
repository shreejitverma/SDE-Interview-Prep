#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <mode> [args...]\n", argv[0]);
    return 1;
  }

  if (strcmp(argv[1], "--dump-args") == 0) {
    printf("argc=%d\n", argc - 2);
    for (int i = 2; i < argc; i++) {
      printf("argv[%d]: len=%zu <%s>\n", i - 2, strlen(argv[i]), argv[i]);
    }
    return 0;
  }

  if (strcmp(argv[1], "--getpwnam") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Missing username\n");
      return 1;
    }
    struct passwd *pw = getpwnam(argv[2]);
    if (!pw) {
      printf("user=%s: not found\n", argv[2]);
      return 2;
    }
    printf("user=%s uid=%u dir=%s\n", pw->pw_name, (unsigned int)pw->pw_uid,
           pw->pw_dir);
    return 0;
  }

  if (strcmp(argv[1], "--limits") == 0) {
    long pw_max = sysconf(_SC_GETPW_R_SIZE_MAX);
    long log_max = sysconf(_SC_LOGIN_NAME_MAX);
    printf("GETPW_R_SIZE_MAX=%ld\n", pw_max);
    printf("LOGIN_NAME_MAX=%ld\n", log_max);
    return 0;
  }

  fprintf(stderr, "Unknown mode: %s\n", argv[1]);
  return 1;
}
