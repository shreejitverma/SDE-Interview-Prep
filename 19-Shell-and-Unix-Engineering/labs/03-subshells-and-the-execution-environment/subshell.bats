#!/usr/bin/env bats
# Tests for POSIX subshells and the execution environment:
# variable and state isolation, special parameters ($$ and $PPID),
# trap resets and inheritance, pipeline subshell divergence,
# shared open file description offsets and status flags, and exit status propagation.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe.sh"
}

@test "subshell isolates variable mutations, working directory, umask, and options while brace group modifies current environment" {
  run_lab "$PROBE" env_isolation "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "sub: var=subshell umask=0077 noglob=on" ]
  [ "${lines[1]}" = "caller_after_sub: var=initial in_orig=1 same_umask=1 noglob=off" ]
  [ "${lines[2]}" = "caller_after_brace: var=current" ]
}

@test "special parameters \$\$ and \$PPID remain identical inside subshell across all shell families" {
  run_lab "$PROBE" special_params
  [ "$status" -eq 0 ]
  [ "$output" = "sub_params: same_pid=1 same_ppid=1" ]
}

@test "subshell does not run the parent EXIT trap at subshell exit, runs its own EXIT trap, and the parent trap fires once at parent exit" {
  run_lab "$PROBE" trap_reset
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 5 ]
  [ "${lines[0]}" = "inside_plain" ]
  [ "${lines[1]}" = "inside_own" ]
  [ "${lines[2]}" = "sub_exit" ]
  [ "${lines[3]}" = "after_sub" ]
  [ "${lines[4]}" = "main_exit" ]
}

@test "redirected while loop preserves variables while pipeline stage subshell diverges" {
  run_lab "$PROBE" pipeline_isolation "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "redir_count: 3" ]
  case $(lab_shell_family) in
    ksh93 | zsh)
      [ "${lines[1]}" = "pipe_count: 3" ]
      ;;
    bash | zsh-sh | dash | busybox | posh | mksh | yash)
      [ "${lines[1]}" = "pipe_count: 0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "subshell advances file offset on shared open file description in the kernel" {
  run_lab "$PROBE" fd_offset_sharing "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "shared_offset: 5678" ]
}

@test "subshell modifying O_NONBLOCK leaks file status flags to parent via shared open file description" {
  run_lab "$PROBE" fd_flags_sharing "$BATS_TEST_TMPDIR" "$BATS_TEST_DIRNAME"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "fd 3 nonblock: no" ]
  [ "${lines[1]}" = "set_nonblock" ]
  [ "${lines[2]}" = "fd 3 nonblock: yes" ]
}

@test "subshell exit status propagates to parent shell through wait status" {
  run_lab "$PROBE" status_propagation
  [ "$status" -eq 0 ]
  [ "$output" = "sub_status: 42" ]
}

@test "command substitution with single trap command exposes parent traps according to shell implementation" {
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c 'trap "echo usr" USR1; out=$(trap); [ -n "$out" ] && echo preserved || echo reset'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | busybox | yash)
      [ "$output" = "preserved" ]
      ;;
    dash | zsh | zsh-sh | posh | mksh)
      # dash, zsh, posh, and mksh reset traps immediately upon subshell entry
      [ "$output" = "reset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "BASH_SUBSHELL and ZSH_SUBSHELL count subshell depth and no other probed shell sets either" {
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '
    report_sub() {
      echo "bash=${BASH_SUBSHELL-unset} zsh=${ZSH_SUBSHELL-unset}"
    }
    report_sub
    ( report_sub; ( report_sub ) )
  '
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 3 ]
  case $(lab_shell_family) in
    bash)
      [ "${lines[0]}" = "bash=0 zsh=unset" ]
      [ "${lines[1]}" = "bash=1 zsh=unset" ]
      [ "${lines[2]}" = "bash=2 zsh=unset" ]
      ;;
    zsh | zsh-sh)
      # ZSH_SUBSHELL is set under native zsh and under emulate sh
      [ "${lines[0]}" = "bash=unset zsh=0" ]
      [ "${lines[1]}" = "bash=unset zsh=1" ]
      [ "${lines[2]}" = "bash=unset zsh=2" ]
      ;;
    dash | yash | busybox | posh | mksh | ksh93)
      [ "${lines[0]}" = "bash=unset zsh=unset" ]
      [ "${lines[1]}" = "bash=unset zsh=unset" ]
      [ "${lines[2]}" = "bash=unset zsh=unset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "dotted ksh93 variable name sh.subshell counts depth in ksh93 and is a substitution error everywhere else" {
  # The trailing ":" keeps the subshell from being the last command, since macOS ksh93u+ 2012 reports depth 0 inside a final subshell
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c 'eval '\''echo "${.sh.subshell}"; ( echo "${.sh.subshell}"; ( echo "${.sh.subshell}" ) ); :'\'''
  case $(lab_shell_family) in
    ksh93)
      [ "$status" -eq 0 ]
      [ "${#lines[@]}" -eq 3 ]
      [ "${lines[0]}" = "0" ]
      [ "${lines[1]}" = "1" ]
      [ "${lines[2]}" = "2" ]
      ;;
    bash | dash | zsh | zsh-sh | busybox | posh | mksh)
      [ "$status" -ne 0 ]
      [[ $output == *[Bb]ad\ substitution* ]]
      ;;
    yash)
      [ "$status" -ne 0 ]
      [[ $output == *"parameter name is missing or invalid"* ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "BASHPID reports the subshell process ID in bash 4 and later and mksh, and is unset elsewhere" {
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '( if [ "${BASHPID-unset}" = unset ]; then echo unset; elif [ "$BASHPID" != "$$" ]; then echo differs; else echo same; fi )'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      # BASHPID was added in Bash 4.0; Bash 3.2 does not set it
      case $(lab_shell_version) in
        [123].*) [ "$output" = "unset" ] ;;
        *) [ "$output" = "differs" ] ;;
      esac
      ;;
    mksh)
      [ "$output" = "differs" ]
      ;;
    dash | zsh | zsh-sh | yash | busybox | posh | ksh93)
      [ "$output" = "unset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "parentheses do not require delimiter whitespace whereas braces require delimiters in POSIX shells" {
  # Subshell parenthesis is a control operator, so no whitespace required
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '(printf "paren\n")'
  [ "$status" -eq 0 ]
  [ "$output" = "paren" ]

  # Brace group without delimiter whitespace is treated as command name or syntax error in standard shells
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '{printf "brace\n";}'
  case $(lab_shell_family) in
    zsh)
      # Native zsh accepts {cmd without space as an extension
      [ "$status" -eq 0 ]
      [ "$output" = "brace" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$status" -ne 0 ]
      ;;
    *)
      false
      ;;
  esac
}
