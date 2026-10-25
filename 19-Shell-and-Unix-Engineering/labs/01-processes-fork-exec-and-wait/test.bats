#!/usr/bin/env bats
# Bats lab matrix for Processes: fork, exec, and wait (POSIX.1-2024 XSH/XCU).

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/fork-exec-wait-probe.sh"
}

@test "fork-exec-wait-probe.sh matches shell family semantics" {
  run_lab "$PROBE"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | busybox | zsh-sh)
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=143" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=initial" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    zsh)
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=143" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=mutated" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    ksh93)
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=271" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=mutated" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    yash)
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=399" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=initial" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    mksh)
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=143" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=initial" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    posh) # no sleep builtin: the external sleep dies by SIGTERM, so posh reports 128 + 15
      [ "${lines[0]}" = "1_subshell_isolation: orig_var=<parent> child_var=<parent>" ]
      [ "${lines[1]}" = "2_subshell_dollar_dollar: parent_pid=child_pid=match" ]
      [ "${lines[2]}" = "3_exec_pid_preservation: before=after=match" ]
      [ "${lines[3]}" = "4_async_wait_reap: status=42" ]
      [ "${lines[4]}" = "5_signal_termination: status=143" ]
      [ "${lines[5]}" = "6_pipeline_isolation: var=initial" ]
      [ "${lines[6]}" = "7_wait_unknown_pid: status=127" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "subshell isolates variable and directory modifications from parent" {
  # shellcheck disable=SC2016 # single quotes protect shell under test code
  run_lab_c 'var=orig; (var=mutated; cd /); echo "$var"'
  [ "$status" -eq 0 ]
  [ "$output" = "orig" ]
}

@test "dollar dollar in subshell expands to parent PID (POSIX.1-2024 XCU 2.13)" {
  # shellcheck disable=SC2016 # expansions belong to shell under test
  run_lab_c 'p=$$; ( [ "$p" = "$$" ] && echo match )'
  [ "$status" -eq 0 ]
  [ "$output" = "match" ]
}

@test "exec replaces process image and preserves open file descriptors" {
  local data_file="$BATS_TEST_TMPDIR/fd_data.txt"
  printf 'stream_payload\n' >"$data_file"

  # shellcheck disable=SC2016 # single quotes protect code passed to shell under test
  run_lab_c '
    exec < "$1"
    exec sh -c '\''read -r val; echo "$val"'\'' dummy
  ' "sh" "$data_file"

  [ "$status" -eq 0 ]
  [ "$output" = "stream_payload" ]
}

@test "asynchronous jobs record PID in dollar-bang and wait reaps exit status" {
  # shellcheck disable=SC2016 # code string for shell under test
  run_lab_c 'sh -c "exit 65" & pid=$!; wait "$pid"'
  [ "$status" -eq 65 ]
}

@test "wait without arguments blocks until all background jobs terminate" {
  # shellcheck disable=SC2016 # code string for shell under test
  run_lab_c 'sh -c "exit 0" & sh -c "exit 0" & wait; echo done'
  [ "$status" -eq 0 ]
  [ "$output" = "done" ]
}

@test "wait on unknown PID returns 127 (POSIX.1-2024 XCU wait)" {
  # shellcheck disable=SC2016 # code string for shell under test
  run_lab_c 'wait 999999 2>/dev/null; echo "$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "127" ]
}

@test "signal termination after exec yields exit status greater than 128" {
  # Kill only once the child has exec'd sleep; an earlier SIGTERM races the shell's own handler (mksh).
  # /bin/sleep bypasses ksh93's builtin sleep, whose process name would never become sleep.
  # shellcheck disable=SC2016 # code string for shell under test
  run_lab_c '/bin/sleep 30 & pid=$!; n=0; while [ "$n" -lt 500 ]; do c=""; if [ -r "/proc/$pid/comm" ]; then read -r c <"/proc/$pid/comm"; else c=$(ps -o comm= -p "$pid" 2>/dev/null); fi; case $c in *sleep) break ;; *) ;; esac; sleep 0.01 2>/dev/null || sleep 1; n=$((n + 1)); done; kill -15 "$pid" 2>/dev/null; wait "$pid" 2>/dev/null; echo "$?"'
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    ksh93)
      [ "$output" = "271" ]
      ;;
    yash)
      [ "$output" = "399" ]
      ;;
    bash | zsh | zsh-sh | dash | mksh | busybox | posh)
      [ "$output" = "143" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline stages isolate subshell environments except in ksh93 and native zsh" {
  # shellcheck disable=SC2016 # code string for shell under test
  run_lab_c 'val=0; echo ping | { val=1; }; echo "$val"'
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    ksh93 | zsh)
      [ "$output" = "1" ]
      ;;
    bash | zsh-sh | dash | mksh | yash | posh | busybox)
      [ "$output" = "0" ]
      ;;
    *)
      false
      ;;
  esac
}
