#!/usr/bin/env bats
# Tests for Unix daemonization, setsid, descriptor isolation, and background mechanics.

setup() {
  load ../lib/lab
  DAEMONIZE="$BATS_TEST_DIRNAME/daemonize.sh"
  PROBE_SRC="$BATS_TEST_DIRNAME/daemon_probe.c"
  PROBE="$BATS_TEST_TMPDIR/daemon_probe"

  "${CC:-cc}" -O2 -Wall -Wextra -pedantic "$PROBE_SRC" -o "$PROBE"
}

# poll SECONDS CMD [ARG...]: retry CMD every 0.1 s until it succeeds; fail after SECONDS.
poll() {
  local tries=$(($1 * 10))
  shift
  until "$@"; do
    tries=$((tries - 1))
    [ "$tries" -gt 0 ] || return 1
    sleep 0.1
  done
}

# proc_ppid PID: print the parent PID of PID (procfs on Linux, ps elsewhere).
proc_ppid() {
  if [ -r "/proc/$1/status" ]; then
    awk '$1 == "PPid:" { print $2 }' "/proc/$1/status"
  else
    local ppid
    ppid=$(ps -o ppid= -p "$1") || return 1
    printf '%s\n' "${ppid// /}"
  fi
}

# proc_comm PID: print the command name of PID (procfs on Linux, ps elsewhere).
proc_comm() {
  if [ -r "/proc/$1/comm" ]; then
    cat "/proc/$1/comm"
  else
    ps -o comm= -p "$1"
  fi
}

file_nonempty() { [ -s "$1" ]; }
file_is() {
  local content
  content=$(cat "$1" 2>/dev/null) || return 1
  [ "$content" = "$2" ]
}
# pid_gone PID: true once PID has exited (reaped, or a zombie its new parent has not reaped yet).
pid_gone() {
  kill -0 "$1" 2>/dev/null || return 0
  if [ -r "/proc/$1/status" ]; then
    awk '$1 == "State:" { exit ($2 == "Z") ? 0 : 1 }' "/proc/$1/status"
  else
    case $(ps -o stat= -p "$1") in Z*) return 0 ;; *) return 1 ;; esac
  fi
}
is_sleep() { case $(proc_comm "$1") in *sleep) return 0 ;; *) return 1 ;; esac }

@test "setsid fails with EPERM when called by a process group leader" {
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" setsid-leader' -- "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "setsid_leader: EPERM" ]
}

@test "setsid succeeds in a non-leader child and detaches controlling terminal" {
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" setsid-child' -- "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "setsid_child: ok" ]
}

@test "classical double fork creates a new session where grandchild is not session leader" {
  local outfile="$BATS_TEST_TMPDIR/double_fork.out"
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" double-fork "$2"' -- "$PROBE" "$outfile"
  [ "$status" -eq 0 ]
  [ "$output" = "double_fork: completed" ]
  [ -f "$outfile" ]

  # Assert grandchild PID != SID and is_leader == 0
  run grep "is_leader=0" "$outfile"
  [ "$status" -eq 0 ]
  # Assert /dev/tty could not be opened (tty_err non-zero, specifically ENXIO=6)
  run grep "tty_err=6" "$outfile"
  [ "$status" -eq 0 ]
}

@test "closing fd 0 reuses fd 0 on subsequent open whereas dev null redirection preserves low fds" {
  local tempfile="$BATS_TEST_TMPDIR/fd_test.tmp"
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" fd-reuse "$2"' -- "$PROBE" "$tempfile"
  [ "$status" -eq 0 ]
  # First line proves closed fd 0 is reallocated to new file
  # Second line proves dup2 /dev/null to fd 0 keeps low descriptors occupied so next open gets >= 3
  [ "${lines[0]}" = "closed_fd0: allocated_fd=0" ]
  [[ "${lines[1]}" =~ ^redirected_fd0:[[:space:]]allocated_fd=[3-9][0-9]*$ ]]
}

@test "probe print-attrs reports matching process group and session IDs under direct invocation" {
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" print-attrs' -- "$PROBE"
  [ "$status" -eq 0 ]
  [[ "$output" =~ pid=[0-9]+[[:space:]]ppid=[0-9]+[[:space:]]pgid=[0-9]+[[:space:]]sid=[0-9]+ ]]
}

@test "probe check-ctty detects controlling terminal status" {
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c '"$1" check-ctty' -- "$PROBE"
  [ "$status" -eq 0 ]
  case "$output" in
    "ctty: present" | "ctty: none (ENXIO)")
      true
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous list & without job control automatically redirects stdin from dev null" {
  # shellcheck disable=SC2016 # parameter expansion belongs to the shell under test
  run_lab_c 'case $- in *m*) set +m ;; esac; "$1" is-stdin-eof & wait "$!"' -- "$DAEMONIZE" <<<"DATA"
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$status" -eq 1 ]
      [ "$output" = "stdin: data" ]
      ;;
    bash | dash | posh | mksh | ksh93 | yash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: eof" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "background execution with descriptor redirection captures output safely" {
  local pidfile="$BATS_TEST_TMPDIR/bg_pid"
  local outfile="$BATS_TEST_TMPDIR/bg_out"
  run_lab "$DAEMONIZE" bg-redirect "$pidfile" "$outfile" printf '%s\n' "daemon output"
  [ "$status" -eq 0 ]
  [ -f "$pidfile" ]
  # The script exits right after forking the job, so the job may not have written yet:
  # poll for up to 3 seconds (printf finishes in milliseconds).
  poll 3 file_is "$outfile" "daemon output"
}

@test "subshell double fork runs asynchronously and orphans grandchild" {
  local pidfile="$BATS_TEST_TMPDIR/df_pid"
  # The worker sleeps 15 s; the four polls below are bounded at 3 s each, so the
  # worker is still alive at the last check even in the worst case (3 s margin).
  run_lab "$DAEMONIZE" double-fork "$pidfile" /bin/sleep 15
  [ "$status" -eq 0 ]
  # Both PID files are written by background subshells after the script returns.
  poll 3 file_nonempty "$pidfile"
  poll 3 file_nonempty "$pidfile.parent"
  local bg_pid mid_pid
  bg_pid=$(cat "$pidfile")
  mid_pid=$(cat "$pidfile.parent")
  # The intermediate subshell exits right after forking the worker.
  poll 3 pid_gone "$mid_pid"
  # The worker has exec'd sleep (so a TERM below is not caught by a shell) and
  # is still running although its parent has exited: it was orphaned and reparented.
  poll 3 is_sleep "$bg_pid"
  kill -0 "$bg_pid"
  local ppid
  ppid=$(proc_ppid "$bg_pid")
  [ -n "$ppid" ]
  [ "$ppid" != "$mid_pid" ]
  kill -TERM "$bg_pid"
}

@test "disown builtin availability across shell families" {
  # shellcheck disable=SC2016 # disown command check belongs to the shell under test
  run_lab_c 'if command -v disown >/dev/null 2>&1; then echo yes; else echo no; fi'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | yash)
      [ "$output" = "yes" ]
      ;;
    dash | mksh | posh | busybox)
      [ "$output" = "no" ]
      ;;
    *)
      false
      ;;
  esac
}
