/*
 * argv-inspect: print what execve(2) delivered, so a test can see the bytes a program receives
 * after the shell has done quote removal.
 * Usage: argv-inspect [ARG...]
 * Output: "argc=N", then per argument "argN len=L quotes=Q" where Q counts the characters
 * single quote, double quote and backslash that are left in the argument.
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    int i;

    printf("argc=%d\n", argc - 1);
    for (i = 1; i < argc; i++) {
        size_t len = strlen(argv[i]);
        size_t quotes = 0;
        size_t j;

        for (j = 0; j < len; j++) {
            if (argv[i][j] == '\'' || argv[i][j] == '"' || argv[i][j] == '\\') {
                quotes++;
            }
        }
        printf("arg%d len=%zu quotes=%zu\n", i, len, quotes);
    }
    return 0;
}
