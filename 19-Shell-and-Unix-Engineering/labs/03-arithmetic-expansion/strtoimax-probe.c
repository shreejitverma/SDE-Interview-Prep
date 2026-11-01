/* strtoimax-probe: print what the C library's strtoimax(text, &end, 0) makes of its argument.
 * dash scans every numeric constant in $(( )) with strtoimax(..., 0), so a constant is valid in
 * dash exactly when this program prints a number for it. Build with -D_GNU_SOURCE, as dash does.
 * Usage: strtoimax-probe TEXT   prints the value, or "reject" when the whole text is not consumed.
 */
#include <errno.h>
#include <inttypes.h>
#include <stdio.h>

int main(int argc, char **argv)
{
  char *end;
  intmax_t value;

  if (argc != 2) {
    fputs("usage: strtoimax-probe TEXT\n", stderr);
    return 64;
  }
  errno = 0;
  value = strtoimax(argv[1], &end, 0);
  if (end == argv[1] || *end != '\0' || errno != 0) {
    puts("reject");
  } else {
    printf("%jd\n", value);
  }
  return 0;
}
