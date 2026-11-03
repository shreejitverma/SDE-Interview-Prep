#!/usr/bin/env bats
# Tests for Traps and EXIT: POSIX.1-2024 XCU 2.12, 2.13, and 2.15 (trap special built-in).
# Code under test runs through run_lab SCRIPT or run_lab_c CODE via labs/lib/lab.bash.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/trap-probe.sh"
  WCE_SRC="$BATS_TEST_DIRNAME/wce-probe.c"
  WCE_BIN="$BATS_TEST_TMPDIR/wce-probe"
}

# --- Probe Script Modes ---

@test "probe exit-clean creates scratch directory and deletes it in EXIT trap" {
  run_lab "$PROBE" exit-clean "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [[ "$output" =~ marker=0 ]]
  [[ "$output" =~ cleaned ]]

  # Extract created directory path and verify it was deleted by the trap
  created_dir=$(printf '%s\n' "$output" | sed -n 's/^created=\([^ ]*\).*/\1/p')
  [ -n "$created_dir" ]
  [ ! -d "$created_dir" ]
}

@test "probe exit-status preserves command exit status across EXIT trap" {
  run_lab "$PROBE" exit-status
  [ "$status" -eq 42 ]
  [[ "$output" =~ trap-seen-rc=42 ]]
}

@test "probe exit-override allows trap handler to set process exit status" {
  run_lab "$PROBE" exit-override
  [ "$status" -eq 77 ]
}

@test "probe subshell-trap verifies independent subshell EXIT trap lifecycle" {
  run_lab "$PROBE" subshell-trap
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "subshell-running" ]
  [ "${lines[1]}" = "subshell-exit" ]
  [ "${lines[2]}" = "parent-continuing" ]
  [ "${lines[3]}" = "parent-exit" ]
}

@test "probe subshell-reset: a subshell inherits neither the parent USR1 trap nor the parent EXIT trap" {
  # POSIX.1-2024 XCU 2.13: traps that are not ignored are reset to default on subshell entry.
  # The subshells set no trap of their own: one dies of USR1 instead of running the parent action,
  # one exits normally, and the parent EXIT action runs once, for the parent only.
  run_lab "$PROBE" subshell-reset
  [ "$status" -eq 0 ]
  sub_st=$(printf '%s\n' "$output" | sed -n 's/^sub-status=//p')
  [ "$sub_st" -gt 128 ]
  [[ ! "$output" =~ parent-usr1 ]]
  [[ ! "$output" =~ subshell-survived ]]
  [[ "$output" =~ subshell-normal ]]
  [[ "$output" =~ parent-continuing ]]
  exit_lines=$(printf '%s\n' "$output" | grep -c '^parent-exit$') || true
  [ "$exit_lines" -eq 1 ]
}

@test "probe sig-forward forwards signal to EXIT trap and exits with signal status" {
  run_lab "$PROBE" sig-forward "$BATS_TEST_TMPDIR"
  [[ "$output" =~ signal-cleaned ]]
  case $(lab_shell_family) in
    mksh)
      # mksh bug: nested EXIT trap execution via unwind(LEXIT) leaves is_last=0 in runtrap,
      # clobbering exit status with the exit status of the trap handler's last command (0).
      [ "$status" -eq 0 ]
      ;;
    bash | dash | ksh93 | yash | busybox | posh | zsh | zsh-sh)
      [ "$status" -eq 143 ]
      ;;
    *)
      false
      ;;
  esac
}

# --- POSIX trap Syntax and Specification Rules ---

@test "trap 0 and unsigned decimal integer reset condition to default" {
  # POSIX.1-2024 XCU 2.15: If the first operand is an unsigned decimal integer,
  # the shell shall treat all operands as conditions and reset each to default.
  # posh 0.14.1 treats the integer as an action and rejects "trap 0" with "no signals specified".
  run_lab_c 'trap "echo custom-exit" EXIT; trap 0; exit 0'
  case $(lab_shell_family) in
    posh)
      [ "$status" -ne 0 ]
      [[ "$output" =~ "no signals specified" ]]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ -z "$output" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "trap signal naming: symbolic name without SIG required by POSIX; dash and posh reject SIG prefix" {
  # shellcheck disable=SC2016 # probe runs inside the shell under test
  run_lab_c '
    trap "echo ok" INT >/dev/null 2>&1; s1=$?
    trap "echo ok" SIGINT >/dev/null 2>&1; s2=$?
    printf "%d %d" "$s1" "$s2"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh)
      [ "$output" = "0 1" ] || [ "$output" = "0 2" ]
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$output" = "0 0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "untrapped terminating signal: bash, ksh93, and mksh execute EXIT trap; dash, zsh, yash, posh, and BusyBox ash do not" {
  # POSIX.1-2024 XCU 2.15: EXIT condition occurs on normal termination,
  # and MAY occur on abnormal termination from delivery of a signal.
  # shellcheck disable=SC2016 # code belongs to shell under test
  run_lab_c '
    trap "echo exit-trap-fired" EXIT
    ( sleep 0.5; kill -TERM $$ ) &
    sleep 2
  '
  case $(lab_shell_family) in
    mksh)
      # mksh executes EXIT trap on untrapped signal, but unwinding clobbers exit status to 0
      [ "$status" -eq 0 ]
      [[ "$output" =~ exit-trap-fired ]]
      ;;
    bash | ksh93)
      [ "$status" -eq 143 ]
      [[ "$output" =~ exit-trap-fired ]]
      ;;
    dash | zsh | zsh-sh | busybox | posh | yash)
      [ "$status" -eq 143 ]
      [ -z "$output" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "non-interactive shell cannot trap or reset signals ignored on entry" {
  # POSIX.1-2024 XCU 2.15: Signals ignored on entry to a non-interactive shell
  # cannot be trapped or reset.
  lab_shell_words
  # shellcheck disable=SC2016 # code belongs to shell under test
  run_lab_c '
    trap "" HUP
    "$@" -c '\''
      trap "echo caught-HUP" HUP
      kill -HUP $$
      echo survived
    '\''
  ' sh "${LAB_SHELL_WORDS[@]}"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [[ "$output" =~ caught-HUP ]]
      [[ "$output" =~ survived ]]
      ;;
    bash | dash | ksh93 | mksh | posh | busybox | yash)
      [[ ! "$output" =~ caught-HUP ]]
      [[ "$output" =~ survived ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "command substitution containing only trap command: preserves parent traps in bash and ksh" {
  # POSIX.1-2024 XCU 2.15: command substitution containing only a single trap command
  # need not alter traps in subshell.
  # shellcheck disable=SC2016 # code belongs to shell under test
  run_lab_c '
    trap "echo handler" INT
    res=$(trap)
    printf "res=%s\n" "$res"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | yash | busybox)
      [[ "$output" =~ handler ]]
      ;;
    dash | zsh | zsh-sh | mksh | posh)
      [ "$output" = "res=" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "signal trap action preserves exit status of command preceding signal" {
  # POSIX.1-2024 XCU 2.15: The value of "$?" after the trap action completes
  # shall be the value it had before the trap action was executed.
  # shellcheck disable=SC2016 # code belongs to shell under test
  run_lab_c '
    trap "false" USR1
    kill -USR1 $$
    echo "post-trap-status=$?"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "post-trap-status=0" ]
}

@test "wait interrupted by trapped signal returns status > 128 well before the child ends and fires trap" {
  # POSIX.1-2024 XCU 2.12: When waiting via wait for asynchronous commands to complete,
  # receipt of a signal for which a trap is set causes wait to return immediately > 128.
  # The child sleeps 5 s and the signal arrives after 0.5 s, so a wait that did not return
  # at the signal would report an elapsed time of about 5 s; the bound is 2 s.
  # Note: in ksh93, interrupted wait returns 1 (known divergence / job_wait exitval=1).
  # Note: in yash, signals use base 384.
  # In zsh 5.9, sender must linger so SIGCHLD does not race and swallow the signal.
  # shellcheck disable=SC2016 # code belongs to shell under test
  run_lab_c '
    trap "echo trapped-USR1" USR1
    /bin/sleep 5 & pid=$!
    ( sleep 0.5; kill -USR1 $$; sleep 1 ) &
    t0=$(date +%s)
    wait $pid
    st=$?
    t1=$(date +%s)
    echo "wait-st=$st"
    echo "wait-elapsed=$((t1 - t0))"
    kill $pid
    wait $pid || :
  '
  [ "$status" -eq 0 ]
  [[ "$output" =~ trapped-USR1 ]]
  wait_st=$(printf '%s\n' "$output" | sed -n 's/^wait-st=//p')
  wait_elapsed=$(printf '%s\n' "$output" | sed -n 's/^wait-elapsed=//p')
  [ "$wait_elapsed" -le 2 ]
  case $(lab_shell_family) in
    ksh93)
      [ "$wait_st" -eq 1 ]
      ;;
    yash)
      [ "$wait_st" -gt 384 ]
      ;;
    bash | dash | zsh | zsh-sh | busybox | posh | mksh)
      [ "$wait_st" -gt 128 ]
      ;;
    *)
      false
      ;;
  esac
}

# --- Portability Rows ---

@test "trap -p: bash, ksh93, and yash print the trap; zsh prints nothing; dash, mksh, posh, and BusyBox ash reject it" {
  run_lab_c '( trap "echo x" INT; trap -p INT ) 2>&1; echo "rc=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | yash)
      [[ "$output" =~ "echo x" ]]
      [[ "$output" =~ rc=0 ]]
      ;;
    zsh | zsh-sh)
      [ "$output" = "rc=0" ]
      ;;
    dash | mksh | posh | busybox)
      [[ ! "$output" =~ "echo x" ]]
      [[ "$output" =~ rc=[1-9] ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "lowercase signal names: accepted by bash, dash, ksh93, mksh, yash, and BusyBox ash; rejected by zsh and posh" {
  run_lab_c '( trap "echo ok" int; echo "rc=$?"; trap ) 2>&1'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | dash | ksh93 | mksh | yash | busybox)
      [[ "$output" =~ "echo ok" ]]
      [[ "$output" =~ rc=0 ]]
      ;;
    zsh | zsh-sh | posh)
      [[ ! "$output" =~ "echo ok" ]]
      [[ "$output" =~ rc=1 ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "numeric condition 0 names EXIT in every shell" {
  run_lab_c 'trap "echo zero-fired" 0; echo body'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "body" ]
  [ "${lines[1]}" = "zero-fired" ]
}

@test "ERR pseudo-signal: fires in bash, zsh, ksh93, mksh, and BusyBox ash; rejected by dash, posh, and yash" {
  run_lab_c '( trap "echo err-fired" ERR; false; : ) 2>&1'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | mksh | busybox)
      [[ "$output" =~ err-fired ]]
      ;;
    dash | posh | yash)
      [[ ! "$output" =~ err-fired ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "RETURN pseudo-signal: accepted only by bash" {
  run_lab_c '( trap : RETURN ) >/dev/null 2>&1; echo "rc=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      [ "$output" = "rc=0" ]
      ;;
    dash | zsh | zsh-sh | ksh93 | mksh | yash | posh | busybox)
      [[ "$output" =~ rc=[1-9] ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "EXIT trap set in a POSIX function: function-local in native zsh, global elsewhere" {
  run_lab_c 'f() { trap "echo fn-exit" EXIT; }; f; echo after-f'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "fn-exit" ]
      [ "${lines[1]}" = "after-f" ]
      ;;
    bash | dash | zsh-sh | ksh93 | mksh | yash | posh | busybox)
      [ "${lines[0]}" = "after-f" ]
      [ "${lines[1]}" = "fn-exit" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "EXIT trap set in a ksh-style function: local in ksh93 and native zsh, global in bash, mksh, yash, BusyBox ash; syntax error in dash and posh" {
  run_lab_c 'function f { trap "echo fn-exit" EXIT; }; f; echo after-f'
  case $(lab_shell_family) in
    ksh93 | zsh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "fn-exit" ]
      [ "${lines[1]}" = "after-f" ]
      ;;
    bash | zsh-sh | mksh | yash | busybox)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "after-f" ]
      [ "${lines[1]}" = "fn-exit" ]
      ;;
    dash | posh)
      [ "$status" -ne 0 ]
      [[ ! "$output" =~ after-f ]]
      ;;
    *)
      false
      ;;
  esac
}

# --- C Verification Probe ---

@test "wce probe demonstrates WIFEXITED on exit(130) versus WIFSIGNALED on SIGINT" {
  "${CC:-cc}" -O2 -Wall -Wextra -Werror "$WCE_SRC" -o "$WCE_BIN"
  run "$WCE_BIN" exit
  [ "$status" -eq 0 ]
  [ "$output" = "status=exited code=130" ]

  run "$WCE_BIN" signal
  [ "$status" -eq 0 ]
  [ "$output" = "status=signaled sig=2" ]
}
