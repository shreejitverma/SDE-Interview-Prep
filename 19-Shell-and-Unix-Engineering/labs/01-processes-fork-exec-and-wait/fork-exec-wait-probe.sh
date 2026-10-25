#!/bin/sh
# shellcheck shell=sh
# Probe script for Process model: fork, exec, wait behavior.

set -u

# 1. Subshell isolation (fork copies state, child mutations do not affect parent)
var="parent"
# shellcheck disable=SC2030 # deliberate subshell environment isolation test
(
  var="child"
  : "$var"
)
# shellcheck disable=SC2031 # verifying parent retained pre-subshell value
printf '1_subshell_isolation: orig_var=<%s> child_var=<%s>\n' "parent" "$var"

# 2. Subshell $$ invariant (POSIX.1-2024 XCU 2.13: $$ expands to invoking shell PID)
parent_pid="$$"
sub_pid=$(printf '%s' "$$")
if [ "$parent_pid" = "$sub_pid" ]; then
  printf '2_subshell_dollar_dollar: parent_pid=child_pid=match\n'
else
  printf '2_subshell_dollar_dollar: parent_pid=child_pid=diverge\n'
fi

# 3. Exec replaces process image without changing PID
# Child shell records its PID, then execs another shell instance to print its PID
# shellcheck disable=SC2016 # single quotes protect inner expansions
exec_match=$(sh -c 'p="$$"; exec sh -c "echo \"$p == \$\$\""')
case $exec_match in
  *" == "*)
    left=${exec_match%% == *}
    right=${exec_match##* == }
    if [ "$left" = "$right" ]; then
      printf '3_exec_pid_preservation: before=after=match\n'
    else
      printf '3_exec_pid_preservation: before=after=mismatch\n'
    fi
    ;;
  *)
    printf '3_exec_pid_preservation: error\n'
    ;;
esac

# 4. Asynchronous job ($!) and wait reaping exit code
sh -c 'exit 42' &
bg_pid=$!
wait "$bg_pid"
bg_rc=$?
printf '4_async_wait_reap: status=%s\n' "$bg_rc"

# 5. Signal termination exit status (kill -15 / SIGTERM)
# Kill only after the child has exec'd sleep: a SIGTERM that lands between fork and execve
# hits a shell that may catch it (mksh does), which makes the observed status a race.
# /bin/sleep, not sleep: ksh93's builtin sleep runs inside a forked ksh whose name never becomes sleep.
/bin/sleep 30 &
term_pid=$!
tries=0
while [ "$tries" -lt 500 ]; do
  comm=""
  if [ -r "/proc/$term_pid/comm" ]; then
    read -r comm <"/proc/$term_pid/comm"
  else
    comm=$(ps -o comm= -p "$term_pid" 2>/dev/null) || comm=""
  fi
  case $comm in
    *sleep) break ;;
    *) ;;
  esac
  sleep 0.01 2>/dev/null || sleep 1
  tries=$((tries + 1))
done
kill -15 "$term_pid" 2>/dev/null
wait "$term_pid" 2>/dev/null
term_rc=$?
printf '5_signal_termination: status=%s\n' "$term_rc"

# 6. Pipeline execution environment (subshell vs current environment)
pipe_var="initial"
# shellcheck disable=SC2030,SC2031 # testing intentional pipeline environment mutation
printf 'data\n' | { pipe_var="mutated"; }
# In POSIX, pipeline elements may run in subshell; ksh93 and zsh run last stage in main shell
# shellcheck disable=SC2031 # intentional test of pipeline variable scope
printf '6_pipeline_isolation: var=%s\n' "$pipe_var"

# 7. Wait on unknown / non-child PID returns 127
wait 999999 2>/dev/null
unknown_rc=$?
printf '7_wait_unknown_pid: status=%s\n' "$unknown_rc"
