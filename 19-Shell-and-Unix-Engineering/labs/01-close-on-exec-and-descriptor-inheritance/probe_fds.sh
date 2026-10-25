#!/bin/sh
# probe_fds.sh - report open/closed status of specified file descriptors.
# Usage: ./probe_fds.sh [fd...]
# If no arguments are given, probes fds 0 through 4.

set -eu

if [ "$#" -eq 0 ]; then
  set -- 0 1 2 3 4
fi

probe_single_fd() {
  fd="$1"
  if [ "$fd" -eq 9 ]; then
    (: 8<&9) 2>/dev/null
  else
    eval "(: 9<&\$fd) 2>/dev/null"
  fi
}

out=""
for target_fd in "$@"; do
  # shellcheck disable=SC2310 # probe failure indicates a closed descriptor and is handled by the if branch
  if probe_single_fd "$target_fd"; then
    state="open"
  else
    state="closed"
  fi
  if [ -z "$out" ]; then
    out="${target_fd}:${state}"
  else
    out="${out} ${target_fd}:${state}"
  fi
done

printf '%s\n' "$out"
