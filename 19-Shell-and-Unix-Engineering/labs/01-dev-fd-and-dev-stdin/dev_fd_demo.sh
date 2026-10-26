#!/bin/sh
# Demonstrates /dev/stdin, /dev/fd, and descriptor duplication mechanics.

set -eu

if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <stream|reread|dup|procsub>" >&2
  exit 2
fi

action=$1

case "${action}" in
  stream)
    # Read the full payload from /dev/stdin through an external tool
    cat /dev/stdin
    ;;

  reread)
    # Attempt two sequential reads from /dev/stdin
    read -r line1 </dev/stdin
    read -r line2 </dev/stdin
    printf '1: %s\n2: %s\n' "${line1}" "${line2}"
    ;;

  dup)
    # Read from file descriptor 3, expected to be duplicated from stdin via 3<&0
    read -r line <&3
    printf 'dup: %s\n' "${line}"
    ;;

  procsub)
    # Detect process substitution vs process redirection without syntax errors
    out=$(eval 'echo X <(:) Y' 2>/dev/null) || out=""
    case "${out}" in
      "X "/*" Y")
        printf 'procsub: supported\n'
        ;;
      "X Y")
        printf 'procsub: redirection only\n'
        ;;
      *)
        printf 'procsub: unsupported\n'
        ;;
    esac
    ;;

  *)
    echo "Unknown action: ${action}" >&2
    exit 2
    ;;
esac
