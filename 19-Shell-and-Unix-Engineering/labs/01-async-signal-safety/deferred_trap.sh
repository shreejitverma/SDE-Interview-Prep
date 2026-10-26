#!/bin/sh
# deferred_trap.sh - demonstrate deferred trap execution around foreground utilities
set -eu

target="$$"
trap 'printf "trap_executed\n"' USR1

(
  /bin/sleep 0.1
  kill -USR1 "${target}"
) &
sender_pid="$!"

printf "before_cmd\n"
# The utility prints its own marker last, so a trap run while it is still running shows up
# before utility_done; a deferred trap shows up after it.
/bin/sh -c '/bin/sleep 0.5; printf "utility_done\n"'
printf "after_cmd\n"

wait "${sender_pid}" 2>/dev/null || true
