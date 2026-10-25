#!/bin/sh
# showargs: print the argument count, then each argument between angle brackets.
# The standard probe for seeing exactly what quoting, field splitting, and globbing produced.
# Usage: showargs.sh [ARG...]
# Example: showargs.sh a 'b c' ''  ->  3 args: <a> <b c> <>
printf '%d args:' "$#"
if [ "$#" -gt 0 ]; then
  printf ' <%s>' "$@"
fi
printf '\n'
