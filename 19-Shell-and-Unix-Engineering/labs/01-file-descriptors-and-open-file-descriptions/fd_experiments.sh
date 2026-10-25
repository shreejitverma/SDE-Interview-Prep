#!/bin/sh
# File descriptor and open file description demonstration utility.
# Demonstrates kernel file table sharing, independent opens, and redirection mechanics.

set -eu

usage() {
  printf 'Usage: %s <action> <file>\n' "$0" >&2
  printf 'Actions:\n' >&2
  printf '  shared-dup <file>          - Write via duplicated fds (shared OFD offset)\n' >&2
  printf '  independent-open <file>    - Write via two open() calls (clobbering offsets)\n' >&2
  printf '  append-open <file>         - Write via two append opens (atomic EOF offset)\n' >&2
  printf '  read-write-inplace <file>  - Read and overwrite in place using <>\n' >&2
  printf '  order-stdout-stderr <file> - Redirect stdout then dup stderr (>file 2>&1)\n' >&2
  printf '  order-stderr-stdout <file> - Dup stderr then redirect stdout (2>&1 >file)\n' >&2
  printf '  subshell-sharing <file>    - Compound block sharing a single open across fork\n' >&2
  exit 1
}

if [ "$#" -ne 2 ]; then
  usage
fi

action=$1
target=$2

case "$action" in
  shared-dup)
    # Duplicating an fd creates a new descriptor pointing to the same OFD.
    # Writes to fd 3 and fd 4 advance the same offset sequentially.
    exec 3>"$target"
    exec 4>&3
    printf 'AAAAA' >&3
    printf 'BB' >&4
    exec 3>&-
    exec 4>&-
    ;;

  independent-open)
    # Two separate opens allocate two distinct Open File Descriptions.
    # Each starts with an independent file offset of 0.
    exec 3>"$target"
    exec 4>"$target"
    printf 'AAAAA' >&3
    printf 'BB' >&4
    exec 3>&-
    exec 4>&-
    ;;

  append-open)
    # Two separate opens, but with O_APPEND (>>).
    # The kernel positions the offset to end-of-file atomically before each write.
    exec 3>>"$target"
    exec 4>>"$target"
    printf 'AAAAA' >&3
    printf 'BB' >&4
    exec 3>&-
    exec 4>&-
    ;;

  read-write-inplace)
    # Open for read-write without truncation via <>.
    # Reading advances the offset in the OFD; subsequent write starts at that offset.
    exec 3<>"$target"
    # Read first 4 bytes into variable chunk using dd
    dd bs=1 count=4 <&3 >/dev/null 2>&1
    printf 'XX' >&3
    exec 3>&-
    ;;

  order-stdout-stderr)
    # >target 2>&1: target opened on fd 1, fd 2 duplicated from fd 1.
    # Both stdout and stderr write through the same OFD into target.
    {
      printf 'STDOUT_MSG\n'
      printf 'STDERR_MSG\n' >&2
    } >"$target" 2>&1
    ;;

  order-stderr-stdout)
    # 2>&1 >target: fd 2 duplicated to current fd 1, then fd 1 redirected to target.
    # stderr goes to original stdout; stdout goes into target.
    # shellcheck disable=SC2069 # intentional demonstration of 2>&1 before >target
    {
      printf 'STDOUT_MSG\n'
      printf 'STDERR_MSG\n' >&2
    } 2>&1 >"$target"
    ;;

  subshell-sharing)
    # Subshell redirection: shell opens target once.
    # Each external env printf is a forked child that inherits fd 1 pointing to the same OFD,
    # so the second child writes at the offset the first one advanced.
    (
      env printf 'CHILD_1\n'
      env printf 'CHILD_2\n'
    ) >"$target"
    ;;

  *)
    usage
    ;;
esac
