#!/usr/bin/env bats
# Tests for async-signal-safety, trap deferral, and signal handling across shells.

setup() {
  load ../lib/lab
  DEFERRED="$BATS_TEST_DIRNAME/deferred_trap.sh"
  PROBE_ERRNO="$BATS_TEST_TMPDIR/probe_errno"
  SELF_PIPE="$BATS_TEST_TMPDIR/self_pipe"
  cc -O2 -Wall -Wextra -Werror -o "$PROBE_ERRNO" "$BATS_TEST_DIRNAME/probe_errno.c"
  cc -O2 -Wall -Wextra -Werror -o "$SELF_PIPE" "$BATS_TEST_DIRNAME/self_pipe.c"
}

@test "trap execution is deferred until foreground utility completion" {
  run_lab "$DEFERRED"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "before_cmd" ]
  [ "${lines[1]}" = "utility_done" ]
  [ "${lines[2]}" = "trap_executed" ]
  [ "${lines[3]}" = "after_cmd" ]
}

@test "wait is interrupted immediately by trapped signal returning exit status > 128" {
  # The sender sleeps after its kill so that its exit (a SIGCHLD to the shell) lands a second
  # after the USR1: zsh 5.9 waitforpid() and zwaitjob() test only last_signal after each
  # signal_suspend(), so a SIGCHLD handled in the same wake-up hides the USR1 and wait resumes.
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    trap "echo trap_fired" USR1
    /bin/sleep 2 &
    child=$!
    target=$$
    ( /bin/sleep 0.1; kill -USR1 "$target"; /bin/sleep 1 ) &
    sender=$!
    wait "$child"
    rc=$?
    if kill -0 "$child" 2>/dev/null; then echo child_running; else echo child_gone; fi
    wait "$child" 2>/dev/null || true
    wait "$sender" 2>/dev/null || true
    echo "wait_status:$rc"
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "trap_fired" ]
  [ "${lines[1]}" = "child_running" ]
  status_val="${lines[2]#wait_status:}"
  case $(lab_shell_family) in
    ksh93)
      # ksh93 job_wait() (sh/jobs.c) sets the exit status to 1 when the wait builtin is
      # interrupted with a trap pending (AT&T RELEASE 10-11-29), not the > 128 POSIX requires.
      [ "$status_val" -eq 1 ]
      ;;
    yash)
      [ "$status_val" -gt 384 ]
      ;;
    bash | zsh | zsh-sh | dash | busybox | mksh | posh)
      [ "$status_val" -gt 128 ]
      [ "$status_val" -lt 256 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "exit status is preserved across trap handler execution" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    trap "false; echo in_trap" USR1
    target=$$
    ( /bin/sleep 0.05; kill -USR1 "$target" ) &
    sender=$!
    sh -c "sleep 0.2; exit 42"
    echo "saved_status:$?"
    wait "$sender" 2>/dev/null || true
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "in_trap" ]
  [ "${lines[1]}" = "saved_status:42" ]
}

@test "rapid standard signals coalesce during foreground utility execution" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    count=0
    target=$$
    trap "count=\$((count + 1))" USR1
    (
      /bin/sleep 0.05
      kill -USR1 "$target"
      kill -USR1 "$target"
      kill -USR1 "$target"
      kill -USR1 "$target"
      kill -USR1 "$target"
    ) &
    sender=$!
    /bin/sleep 0.2
    wait "$sender" 2>/dev/null || true
    echo "$count"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$output" -ge 1 ]
      ;;
    bash | dash | ksh93 | yash | busybox | mksh | posh)
      [ "$output" -eq 1 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "ignored signals remain ignored in subshells" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    trap "" USR1
    (
      /bin/sleep 0.5
      echo "survived"
    ) &
    child=$!
    /bin/sleep 0.1
    kill -USR1 "$child" 2>/dev/null
    wait "$child" 2>/dev/null
    echo "child_exit:$?"
  '
  case $(lab_shell_family) in
    zsh)
      [ "$status" -eq 0 ]
      status_val="${lines[0]#child_exit:}"
      [ "$status_val" -gt 128 ]
      ;;
    bash | zsh-sh | dash | ksh93 | yash | busybox | mksh | posh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "survived" ]
      [ "${lines[1]}" = "child_exit:0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "non-ignored traps reset to default action upon subshell entry" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    trap "echo trap_fired" USR1
    (
      /bin/sleep 0.5
      echo "subshell_survived"
    ) &
    child=$!
    /bin/sleep 0.1
    kill -USR1 "$child" 2>/dev/null
    wait "$child" 2>/dev/null
    rc=$?
    echo "subshell_exit:$rc"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93)
      status_val="${lines[0]#subshell_exit:}"
      [ "$status_val" -gt 256 ]
      ;;
    yash)
      status_val="${lines[0]#subshell_exit:}"
      [ "$status_val" -gt 384 ]
      ;;
    bash | zsh | zsh-sh | dash | busybox | mksh | posh)
      status_val="${lines[0]#subshell_exit:}"
      [ "$status_val" -gt 128 ]
      [ "$status_val" -lt 256 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "errno is clobbered when unpreserved and preserved when restored in signal handler" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '"$1" unsafe' inline "$PROBE_ERRNO"
  [ "$status" -eq 0 ]
  [ "$output" = "clobbered" ]

  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '"$1" safe' inline "$PROBE_ERRNO"
  [ "$status" -eq 0 ]
  [ "$output" = "preserved" ]
}

@test "self-pipe trick safely transfers asynchronous signal to synchronous event loop" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '"$1"' inline "$SELF_PIPE"
  [ "$status" -eq 0 ]
  [ "$output" = "self_pipe_success" ]
}
