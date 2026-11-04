#!/bin/sh
# stty_ops.sh - Deterministic termios and stty operations wrapper
# Demonstrates safe saving, attribute querying, sandboxed execution, and restoration.

set -eu

usage() {
  printf 'Usage: %s <save|restore <state>|query-flags|with-noecho <cmd> [args...]|sane>\n' "$0" >&2
  exit 1
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
    has_echo=0
    has_icanon=0
    has_onlcr=0
    has_isig=0
    case "${out}" in
      *"-echo "* | *"-echo\n"* | *"-echo;"* | *"-echo	"*) has_echo=0 ;;
      *"echo "* | *"echo\n"* | *"echo;"* | *"echo	"*) has_echo=1 ;;
      *) has_echo=0 ;;
    esac
    case "${out}" in
      *"-icanon "* | *"-icanon\n"* | *"-icanon;"* | *"-icanon	"*) has_icanon=0 ;;
      *"icanon "* | *"icanon\n"* | *"icanon;"* | *"icanon	"*) has_icanon=1 ;;
      *) has_icanon=0 ;;
    esac
    case "${out}" in
      *"-onlcr "* | *"-onlcr\n"* | *"-onlcr;"* | *"-onlcr	"*) has_onlcr=0 ;;
      *"onlcr "* | *"onlcr\n"* | *"onlcr;"* | *"onlcr	"*) has_onlcr=1 ;;
      *) has_onlcr=0 ;;
    esac
    case "${out}" in
      *"-isig "* | *"-isig\n"* | *"-isig;"* | *"-isig	"*) has_isig=0 ;;
      *"isig "* | *"isig\n"* | *"isig;"* | *"isig	"*) has_isig=1 ;;
      *) has_isig=0 ;;
    esac
    printf 'echo=%d icanon=%d onlcr=%d isig=%d\n' "${has_echo}" "${has_icanon}" "${has_onlcr}" "${has_isig}"
    ;;
  with-noecho)
    [ "$#" -ge 1 ] || {
      printf 'Error: missing command\n' >&2
      exit 1
    }
    saved=$(stty -g)
    trap 'stty "${saved}" 2>/dev/null || true' EXIT INT TERM HUP
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
