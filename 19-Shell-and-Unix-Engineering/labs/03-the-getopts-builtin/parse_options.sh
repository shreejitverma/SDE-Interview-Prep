#!/bin/sh
# parse_options.sh - Robust POSIX option parsing with getopts.

set -eu

verbose=0
filename=""
output=""

usage() {
  printf 'Usage: %s [-v] [-f file] [-o output] [--] [operands...]\n' "$0" >&2
}

# The leading colon enables silent error handling.
while getopts ":vf:o:" opt; do
  case "$opt" in
    v)
      verbose=$((verbose + 1))
      ;;
    f)
      filename="$OPTARG"
      ;;
    o)
      output="$OPTARG"
      ;;
    :)
      printf '%s: error: option -%c requires an argument\n' "$0" "$OPTARG" >&2
      usage
      exit 2
      ;;
    \?)
      printf '%s: error: unknown option -%c\n' "$0" "$OPTARG" >&2
      usage
      exit 2
      ;;
    *)
      printf '%s: internal error processing option -%c\n' "$0" "$opt" >&2
      exit 2
      ;;
  esac
done

shift "$((OPTIND - 1))"

printf 'verbose=%d\n' "$verbose"
printf 'file=%s\n' "$filename"
printf 'output=%s\n' "$output"
printf 'operands=%d\n' "$#"

for arg in "$@"; do
  printf 'arg: %s\n' "$arg"
done
