#!/usr/bin/env bats
# Bats tests for Signals: Dispositions, Masks, and Delivery (POSIX.1-2024 XCU/XSH).

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/signal-probe.sh"
  PROBE_C="$BATS_TEST_DIRNAME/sigmask-probe.c"
  PROBE_BIN="$BATS_TEST_TMPDIR/sigmask-probe"

  if [ ! -x "$PROBE_BIN" ]; then
    ${CC:-cc} -O2 -Wall -Wextra "$PROBE_C" -o "$PROBE_BIN"
  fi
}

@test "kernel execve preserves blocked signal mask" {
  run "$PROBE_BIN" test_exec_mask
  [ "$status" -eq 0 ]
  [ "$output" = "usr1_blocked=yes" ]
}

@test "kernel execve resets caught signal handler to SIG_DFL" {
  run "$PROBE_BIN" test_exec_catch_to_dfl
  [ "$status" -eq 0 ]
  [ "$output" = "usr1_handler=DFL" ]
}

@test "kernel execve preserves SIG_IGN disposition" {
  run "$PROBE_BIN" test_exec_preserves_ign
  [ "$status" -eq 0 ]
  [ "$output" = "usr1_handler=IGN" ]
}

@test "kernel slow syscall without SA_RESTART returns EINTR on signal" {
  run "$PROBE_BIN" test_eintr
  [ "$status" -eq 0 ]
  [ "$output" = "read_interrupted=EINTR" ]
}

@test "shells differ on clearing a signal mask inherited across execve" {
  lab_shell_words
  # shellcheck disable=SC2016 # single quotes protect code passed to shell under test
  run "$PROBE_BIN" exec_blocked "${LAB_SHELL_WORDS[@]}" -c '"$1" check_mask; :' sh "$PROBE_BIN"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    dash | ksh93 | mksh)
      [ "$output" = "usr1_blocked=no" ]
      ;;
    bash | zsh | zsh-sh | yash | posh | busybox)
      [ "$output" = "usr1_blocked=yes" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "signal-probe.sh matches shell family semantics" {
  run_lab "$PROBE"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | busybox | zsh-sh)
      [ "${lines[0]}" = "1_signal_exit_status: status=143" ]
      [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
      [ "${lines[2]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
      [ "${lines[3]}" = "4_subshell_ignored_preserved: ignored=yes" ]
      [ "${lines[4]}" = "5_wait_signal_interrupted: gt_128=yes trap_fired=1" ]
      ;;
    zsh)
      [ "${lines[0]}" = "1_signal_exit_status: status=143" ]
      [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
      [ "${lines[2]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
      [ "${lines[3]}" = "4_subshell_ignored_preserved: ignored=no" ]
      [ "${lines[4]}" = "5_wait_signal_interrupted: gt_128=yes trap_fired=1" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "${lines[0]}" = "1_signal_exit_status: status=271" ]
          [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
          [ "${lines[2]}" = "PARENT_TRAP" ]
          [ "${lines[3]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
          [ "${lines[4]}" = "4_subshell_ignored_preserved: ignored=yes" ]
          [ "${lines[5]}" = "5_wait_signal_interrupted: gt_128=no trap_fired=1" ]
          ;;
        *)
          [ "${lines[0]}" = "1_signal_exit_status: status=271" ]
          [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
          [ "${lines[2]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
          [ "${lines[3]}" = "4_subshell_ignored_preserved: ignored=yes" ]
          [ "${lines[4]}" = "5_wait_signal_interrupted: gt_128=no trap_fired=1" ]
          ;;
      esac
      ;;
    yash)
      [ "${lines[0]}" = "1_signal_exit_status: status=399" ]
      [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
      [ "${lines[2]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
      [ "${lines[3]}" = "4_subshell_ignored_preserved: ignored=yes" ]
      [ "${lines[4]}" = "5_wait_signal_interrupted: gt_128=yes trap_fired=1" ]
      ;;
    mksh | posh)
      [ "${lines[0]}" = "1_signal_exit_status: status=143" ]
      [ "${lines[1]}" = "2_trap_execution: trap_fired=1 rc=42" ]
      [ "${lines[2]}" = "3_subshell_trap_reset: reset_to_dfl=yes" ]
      [ "${lines[3]}" = "4_subshell_ignored_preserved: ignored=yes" ]
      [ "${lines[4]}" = "5_wait_signal_interrupted: gt_128=yes trap_fired=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous jobs inherit SIG_IGN for SIGINT and SIGQUIT when job control is disabled" {
  # shellcheck disable=SC2016 # single quotes protect code passed to shell under test
  run_lab_c '"$1" check_disposition & wait "$!"' "sh" "$PROBE_BIN"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | busybox | zsh | zsh-sh | ksh93 | yash)
      [ "$output" = "int=IGN quit=IGN term=DFL" ]
      ;;
    posh)
      [ "$output" = "int=DFL quit=DFL term=DFL" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020*)
          [ "$output" = "int=DFL quit=DFL term=DFL" ]
          ;;
        *)
          [ "$output" = "int=IGN quit=IGN term=DFL" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous jobs keep SIG_DFL for SIGINT and SIGQUIT when job control is enabled" {
  # shellcheck disable=SC2016 # single quotes protect code passed to shell under test
  run_lab_c 'set -m; "$1" check_disposition & wait "$!"' "sh" "$PROBE_BIN"

  case $(lab_shell_family) in
    bash | ksh93 | mksh)
      [ "$status" -eq 0 ]
      case $output in
        *"int=DFL quit=DFL term=DFL"*)
          ;;
        *)
          false
          ;;
      esac
      ;;
    dash | busybox)
      [ "$status" -eq 0 ]
      case $output in
        *"can't access tty; job control turned off"*"int=IGN quit=IGN term=DFL"*)
          ;;
        *)
          false
          ;;
      esac
      ;;
    yash)
      [ "$status" -eq 0 ]
      case $output in
        *"disabling job control"*"int=IGN quit=IGN term=DFL"*)
          ;;
        *)
          false
          ;;
      esac
      ;;
    zsh | zsh-sh)
      [ "$status" -ne 0 ]
      case $output in
        *"can't change option: -m"*)
          ;;
        *)
          false
          ;;
      esac
      ;;
    posh)
      [ "$status" -ne 0 ]
      case $output in
        *"unknown option"*)
          ;;
        *)
          false
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "inherited SIG_IGN cannot be trapped in non-interactive POSIX shells" {
  lab_shell_words
  # shellcheck disable=SC2016 # single quotes protect inner script
  run_lab_c '
    trap "" INT
    "$@" -c '\''
      trap "printf \"TRAPPED\n\"" INT
      kill -INT "$$" 2>/dev/null
      printf "SURVIVED\n"
    '\''
  ' "sh" "${LAB_SHELL_WORDS[@]}"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | busybox | mksh | posh | ksh93 | yash)
      [ "$output" = "SURVIVED" ]
      ;;
    zsh | zsh-sh)
      [ "${lines[0]}" = "TRAPPED" ]
      [ "${lines[1]}" = "SURVIVED" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "trap without operands prints registered conditions" {
  # shellcheck disable=SC2016 # single quotes protect code passed to shell under test
  run_lab_c 'trap "echo usr1_action" USR1; trap'
  [ "$status" -eq 0 ]
  case $output in
    *"usr1_action"*)
      ;;
    *)
      false
      ;;
  esac
}
