#!/bin/sh
# Helper script demonstrating shell backgrounding, descriptor redirection,
# and subshell double-fork execution patterns.

set -eu

mode="${1:-}"
if [ -z "$mode" ]; then
  echo "Usage: $0 <double-fork|bg-redirect|is-stdin-eof> [args...]" >&2
  exit 1
fi
shift

case "$mode" in
  double-fork)
    if [ "$#" -lt 2 ]; then
      echo "Usage: $0 double-fork <pid_file> <cmd> [args...]" >&2
      exit 1
    fi
    pid_file="$1"
    shift
    # The worker's PID goes to pid_file and the intermediate subshell's PID to
    # pid_file.parent, so a caller can check that the worker outlives its parent.
    # exec makes the background subshell become the worker: without it, yash
    # keeps a saved copy of the caller's stdout open in a waiting subshell.
    (
      (
        exec "$@" </dev/null >/dev/null 2>&1 &
        echo "$!" >"$pid_file"
      ) &
      echo "$!" >"$pid_file.parent"
    )
    exit 0
    ;;
  bg-redirect)
    if [ "$#" -lt 3 ]; then
      echo "Usage: $0 bg-redirect <pid_file> <outfile> <cmd> [args...]" >&2
      exit 1
    fi
    pid_file="$1"
    outfile="$2"
    shift 2
    "$@" </dev/null >"$outfile" 2>&1 &
    echo "$!" >"$pid_file"
    exit 0
    ;;
  is-stdin-eof)
    if read -r _; then
      echo "stdin: data"
      exit 1
    else
      echo "stdin: eof"
      exit 0
    fi
    ;;
  *)
    echo "Unknown mode: $mode" >&2
    exit 1
    ;;
esac
