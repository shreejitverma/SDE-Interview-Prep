/* argv0-probe.c: inspect and print argv[0] */
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc > 0 && argv[0]) {
    printf("%s\n", argv[0]);
  }
  return 0;
}
