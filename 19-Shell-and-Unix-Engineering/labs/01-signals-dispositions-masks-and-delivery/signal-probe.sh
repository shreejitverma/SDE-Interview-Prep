#!/bin/sh
# shellcheck shell=sh
# Probe script for Signals: Dispositions, Masks, and Delivery (POSIX.1-2024 XCU/XSH).

set -u

# Poll until background process has replaced its process image via execve.
wait_for_exec() {
  target_pid=$1
  iter=0
  while [ "$iter" -lt 100 ]; do
    if [ -f "/proc/$target_pid/comm" ]; then
      comm=$(cat "/proc/$target_pid/comm" 2>/dev/null) || comm=""
      case $comm in
        *sleep*)
          break
          ;;
        *)
          :
          ;;
      esac
    else
      comm=$(ps -o comm= -p "$target_pid" 2>/dev/null) || comm=""
      case $comm in
        *sleep*)
          break
          ;;
        *)
          :
          ;;
      esac
    fi
    iter=$((iter + 1))
    sleep 0.02 2>/dev/null || true
  done
}

# 1. Signal exit status (SIGTERM termination of external command).
/bin/sleep 10 &
term_pid=$!
wait_for_exec "$term_pid"
kill -15 "$term_pid" 2>/dev/null
wait "$term_pid" 2>/dev/null
term_rc=$?
printf '1_signal_exit_status: status=%s\n' "$term_rc"

# 2. Trap execution at command boundary and $? preservation.
trap 'trap_fired=1' USR1
trap_fired=0
sh -c 'kill -USR1 "$PPID"; exit 42'
rc=$?
printf '2_trap_execution: trap_fired=%s rc=%s\n' "$trap_fired" "$rc"
trap - USR1

# 3. Subshell resets caught traps to SIG_DFL.
trap 'printf "PARENT_TRAP\n"' USR1
subshell_out=$(
  (
    : &
    wait "$!" 2>/dev/null || true
    sh -c 'kill -USR1 "$PPID"' 2>/dev/null
    printf 'survived\n'
  ) 2>/dev/null
) 2>/dev/null
trap - USR1
case $subshell_out in
  *PARENT_TRAP*)
    sub_reset="no"
    ;;
  *survived*)
    sub_reset="no"
    ;;
  *)
    sub_reset="yes"
    ;;
esac
printf '3_subshell_trap_reset: reset_to_dfl=%s\n' "$sub_reset"

# 4. Subshell preserves ignored signals (SIG_IGN).
trap '' USR2
sub_ign_out=$(
  (
    : &
    wait "$!" 2>/dev/null || true
    sh -c 'kill -USR2 "$PPID"' 2>/dev/null
    printf 'subshell_survived\n'
  ) 2>/dev/null
)
trap - USR2
case $sub_ign_out in
  *subshell_survived*)
    sub_ign="yes"
    ;;
  *)
    sub_ign="no"
    ;;
esac
printf '4_subshell_ignored_preserved: ignored=%s\n' "$sub_ign"

# 5. Wait interrupted by trapped signal returns > 128.
trap 'wait_trap_fired=1' USR1
wait_trap_fired=0
/bin/sleep 5 &
sleep_bg=$!
# The killer lingers for a second after signalling and then exits by itself: in zsh, a
# SIGCHLD from the killer exiting at the same moment as the trapped SIGUSR1 sometimes
# lets wait resume (about 1 run in 10), and killing the killer instead makes bash 3.2
# print a "Terminated" job notice on the shell's own stderr.
(
  /bin/sleep 0.2
  kill -USR1 "$$" 2>/dev/null
  exec /bin/sleep 1
) &
killer_bg=$!
wait "$sleep_bg" 2>/dev/null
wait_status=$?
# The group redirection also catches job notices that bash prints between commands.
{
  kill "$sleep_bg" || true
  wait "$sleep_bg" || true
  wait "$killer_bg" || true
  :
} 2>/dev/null
trap - USR1
if [ "$wait_status" -gt 128 ]; then
  wait_gt128="yes"
else
  wait_gt128="no"
fi
printf '5_wait_signal_interrupted: gt_128=%s trap_fired=%s\n' "$wait_gt128" "$wait_trap_fired"
