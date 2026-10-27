/* Print how the environment holds a name: NULL when absent, "value" in quotes otherwise. */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  const char *v;

  if (argc != 2) {
    fputs("usage: getenv NAME\n", stderr);
    return 64;
  }
  v = getenv(argv[1]);
  if (v == NULL) {
    puts("NULL");
  } else {
    printf("\"%s\"\n", v);
  }
  return 0;
}
