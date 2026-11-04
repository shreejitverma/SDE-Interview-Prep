#!/bin/sh
# stty_ops.sh - Deterministic termios and stty operations wrapper
# Demonstrates safe saving, attribute querying, sandboxed execution, and restoration.

set -eu

usage() {
  printf 'Usage: %s <save|restore <state>|query-flags|with-noecho <cmd> [args...]|sane>\n' "$0" >&2
  exit 1
}

# flag_state TEXT NAME: print 1 if NAME is a whole word of the stty -a output TEXT, else 0.
flag_state() {
  printf '%s\n' "$1" | awk -v name="$2" '
    { gsub(/;/, " "); for (i = 1; i <= NF; i++) if ($i == name) found = 1 }
    END { print found ? 1 : 0 }'
}

[ "$#" -ge 1 ] || usage

action="$1"
shift

case "${action}" in
  save)
    stty -g
    ;;
  restore)
    [ "$#" -ge 1 ] || {
      printf 'Error: missing state operand\n' >&2
      exit 1
    }
    stty "$1"
    ;;
  query-flags)
    out=$(stty -a)
    echo_state=$(flag_state "${out}" echo)
    icanon_state=$(flag_state "${out}" icanon)
    onlcr_state=$(flag_state "${out}" onlcr)
    isig_state=$(flag_state "${out}" isig)
    printf 'echo=%s icanon=%s onlcr=%s isig=%s\n' "${echo_state}" "${icanon_state}" "${onlcr_state}" "${isig_state}"
    ;;
  with-noecho)
    [ "$#" -ge 1 ] || {
      printf 'Error: missing command\n' >&2
      exit 1
    }
    saved=$(stty -g)
    trap 'stty "${saved}" 2>/dev/null || true' EXIT
    trap 'exit 130' INT
    trap 'exit 143' TERM
    trap 'exit 129' HUP
    stty -echo
    "$@"
    ;;
  sane)
    stty sane
    ;;
  *)
    usage
    ;;
esac
