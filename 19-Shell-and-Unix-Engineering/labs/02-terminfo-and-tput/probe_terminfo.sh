#!/bin/sh
# probe_terminfo.sh - small tput/terminfo probes for the terminfo-and-tput lab.
# Every value printed is computed from a real tput run or a real file.
#
#   status CAP [ARG...]    run tput; print the exit status and the stdout bytes in hex
#   cache-safe CAP [ARG...]    capture under set -e with an || guard
#   cache-unsafe CAP [ARG...]  capture under set -e with no guard
#   header FILE            decode a compiled terminfo header and check the file size

hex() {
  od -An -v -tx1 | tr -d ' \n'
}

mode=${1-}
[ "$#" -gt 0 ] && shift

case $mode in
  status)
    out=$(
      tput "$@" 2>/dev/null
      printf 'rc=%s' "$?"
    )
    rc=${out##*rc=}
    body=${out%rc=*}
    h=$(printf "%s" "$body" | hex)
    printf "rc=%s hex=%s\n" "$rc" "$h"
    ;;
  cache-safe)
    set -e
    val=$(tput "$@" 2>/dev/null) || val=
    h=$(printf "%s" "$val" | hex)
    printf "reached cached=%s\n" "$h"
    ;;
  cache-unsafe)
    set -e
    val=$(tput "$@" 2>/dev/null)
    h=$(printf "%s" "$val" | hex)
    printf "reached cached=%s\n" "$h"
    ;;
  header)
    od -An -v -tu1 "$1" | awk '
      { for (i = 1; i <= NF; i++) b[n++] = $i }
      END {
        magic = b[0] + 256 * b[1]
        names = b[2] + 256 * b[3]
        bools = b[4] + 256 * b[5]
        nums = b[6] + 256 * b[7]
        offs = b[8] + 256 * b[9]
        table = b[10] + 256 * b[11]
        width = (magic == 282) ? 2 : 4
        want = 12 + names + bools + (names + bools) % 2 + nums * width + offs * 2 + table
        printf "magic=0x%04x names=%d bools=%d nums=%d offsets=%d table=%d size=%d expected=%d\n", \
          magic, names, bools, nums, offs, table, n, want
      }'
    ;;
  *)
    printf 'usage: %s status|cache-safe|cache-unsafe|header ...\n' "$0" >&2
    exit 2
    ;;
esac
