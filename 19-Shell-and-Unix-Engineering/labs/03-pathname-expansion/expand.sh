#!/bin/sh
# expand: run CODE inside DIR under the shell that runs this script, with `show` available.
# `show` prints the argument count and each argument between angle brackets, so the output is
# exactly what the shell's expansions produced for the command line.
# Usage: expand.sh DIR CODE
# Example: expand.sh /tmp/fx 'show *.c'  ->  3 args: <a.c> <b.c> <sp ace.c>
cd "$1" || exit 9
show() {
  printf '%d args:' "$#"
  if [ "$#" -gt 0 ]; then
    printf ' <%s>' "$@"
  fi
  printf '\n'
}
eval "$2"
