#!/bin/sh
# exec_demo.sh: demonstrate descriptor redirection and process replacement with exec.
# Usage: exec_demo.sh <logfile> <cmd> [args...]

set -eu

if [ "$#" -lt 2 ]; then
  printf 'usage: %s <logfile> <cmd> [args...]\n' "$0" >&2
  exit 1
fi

log_file=$1
shift

# Report shell PID before replacement.
printf 'shell_pid: %s\n' "$$"

# Open file descriptor 3 to the log file for writing.
exec 3>"$log_file"

# Save original standard output to file descriptor 4.
exec 4>&1

# Redirect standard output to file descriptor 3 (the log file).
exec 1>&3

# Any standard output now writes to the log file.
printf 'log: starting target command\n'

# Restore standard output from file descriptor 4 and close descriptor 4.
exec 1>&4 4>&-

# Write directly to console now that stdout is restored.
printf 'console: descriptors restored, replacing shell image\n'

# Close descriptor 3 so target process does not inherit the log descriptor.
exec 3>&-

# Replace the shell image with the target command in-place.
exec "$@"
