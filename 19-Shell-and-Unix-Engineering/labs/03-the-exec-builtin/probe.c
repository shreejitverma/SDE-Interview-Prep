/*
 * probe: report one fact about the process image the exec built-in produced.
 *
 * Usage:
 *   probe pid             print getpid()
 *   probe fd N            print "open" when descriptor N is open in this process, else "closed"
 *   probe argv0           print argv[0]
 *   probe env_names       print "env:" and then the name of every environment entry, one per line
 *   probe getenv NAME     print the value of NAME, or "unset"
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char **argv) {
  if (argc < 2) {
    return 1;
  }
  if (strcmp(argv[1], "pid") == 0) {
    printf("%ld\n", (long)getpid());
    return 0;
  }
  if (strcmp(argv[1], "fd") == 0 && argc >= 3) {
    int fd = atoi(argv[2]);
    if (fcntl(fd, F_GETFD) != -1) {
      printf("open\n");
    } else {
      printf("closed\n");
    }
    return 0;
  }
  if (strcmp(argv[1], "argv0") == 0) {
    printf("%s\n", argv[0]);
    return 0;
  }
  if (strcmp(argv[1], "env_names") == 0) {
    printf("env:\n");
    for (char **entry = environ; entry != NULL && *entry != NULL; entry++) {
      const char *equals = strchr(*entry, '=');
      int length = equals != NULL ? (int)(equals - *entry) : (int)strlen(*entry);
      printf("%.*s\n", length, *entry);
    }
    return 0;
  }
  if (strcmp(argv[1], "getenv") == 0 && argc >= 3) {
    const char *value = getenv(argv[2]);
    printf("%s\n", value != NULL ? value : "unset");
    return 0;
  }
  return 2;
}
