#!/usr/bin/env bats
# Bats test suite for The eval Builtin (POSIX.1-2024 XCU eval and XCU 2.15).
# shellcheck disable=SC2016 # expressions belong to shell under test

setup() {
  load ../lib/lab
  PROBES=$BATS_TEST_DIRNAME/eval-probes.sh
  cd "$BATS_TEST_TMPDIR" || return 1
}

@test "eval concatenates arguments with spaces ignoring IFS" {
  run_lab "$PROBES" concat-space a b c
  [ "$status" -eq 0 ]
  [ "$output" = "[a][b][c]" ]
}

@test "eval parses dynamic pipelines on second evaluation pass unlike bare expansion" {
  run_lab "$PROBES" dynamic-pipeline bare
  case $(lab_shell_family) in
    zsh)
      # Native zsh does not field-split unquoted expansions, attempting to exec the string as a command name.
      [ "$status" -eq 127 ]
      case $output in
        *"command not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh-sh | dash | ksh93 | mksh | yash | posh | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = 'alpha | tr a-z A-Z' ]
      ;;
    *)
      false
      ;;
  esac

  run_lab "$PROBES" dynamic-pipeline eval
  [ "$status" -eq 0 ]
  [ "$output" = "ALPHA" ]
}

@test "eval parses dynamic redirections on second evaluation pass unlike bare expansion" {
  d=$BATS_TEST_TMPDIR
  run_lab "$PROBES" dynamic-redir "$d/bare.txt" bare
  case $(lab_shell_family) in
    zsh)
      [ "$status" -eq 127 ]
      ;;
    bash | zsh-sh | dash | ksh93 | mksh | yash | posh | busybox)
      [ "$status" -eq 0 ]
      ;;
    *)
      false
      ;;
  esac
  [ ! -f "$d/bare.txt" ]

  run_lab "$PROBES" dynamic-redir "$d/eval.txt" eval
  [ "$status" -eq 0 ]
  [ -f "$d/eval.txt" ]
  read -r content <"$d/eval.txt"
  [ "$content" = "payload" ]
}

@test "eval executes in current shell environment and mutates caller variables" {
  run_lab "$PROBES" env-mutation
  [ "$status" -eq 0 ]
  [ "$output" = "var_x=modified var_y=updated" ]
}

@test "eval executes loop break in caller execution scope" {
  run_lab "$PROBES" flow-break
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "loop_idx=3" ]
      ;;
    posh)
      # posh 0.14.1 fails to break enclosing loop across eval boundary, reporting "break: cannot break"
      [ "$status" -eq 0 ]
      case $output in
        *"break: cannot break"*"loop_idx=5"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "eval executes return in caller function scope with specified exit status" {
  run_lab "$PROBES" flow-return 42
  [ "$status" -eq 0 ]
  [ "$output" = "func_ret=42" ]
}

@test "eval with no arguments or only null arguments returns exit status 0" {
  run_lab "$PROBES" no-operands
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 leaves the previous status in place, against XCU eval EXIT STATUS.
      [ "$output" = "1 1 1" ]
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      [ "$output" = "0 0 0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "variable assignment prefix persistence on eval adheres to shell family standard" {
  run_lab "$PROBES" prefix-assign
  local p_status=$status
  local p_output=$output
  [ "$p_status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      run_lab_c 'if [ -o posix ]; then echo posix; else echo native; fi'
      if [ "$output" = "posix" ]; then
        [ "$p_output" = "pvar=modified" ]
      else
        [ "$p_output" = "pvar=initial" ]
      fi
      ;;
    zsh)
      [ "$p_output" = "pvar=initial" ]
      ;;
    dash | ksh93 | mksh | yash | posh | busybox | zsh-sh)
      [ "$p_output" = "pvar=modified" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "redirection error on special built-in eval aborts non-interactive POSIX shells" {
  run_lab "$PROBES" redir-abort
  local p_status=$status
  local p_output=$output
  case $(lab_shell_family) in
    bash)
      run_lab_c 'if [ -o posix ]; then echo posix; else echo native; fi'
      if [ "$output" = "posix" ]; then
        [ "$p_status" -eq 1 ]
        [ "$p_output" = "" ]
      else
        [ "$p_status" -eq 0 ]
        [ "$p_output" = "SURVIVED" ]
      fi
      ;;
    zsh)
      [ "$p_status" -eq 0 ]
      [ "$p_output" = "SURVIVED" ]
      ;;
    yash)
      run_lab_c 'if [ -o posixly-correct ]; then echo posix; else echo native; fi'
      if [ "$output" = "posix" ]; then
        [ "$p_status" -eq 2 ]
        [ "$p_output" = "" ]
      else
        [ "$p_status" -eq 0 ]
        [ "$p_output" = "SURVIVED" ]
      fi
      ;;
    dash)
      [ "$p_status" -eq 2 ]
      [ "$p_output" = "" ]
      ;;
    ksh93 | mksh | posh | busybox | zsh-sh)
      [ "$p_status" -eq 1 ]
      [ "$p_output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command utility strips special built-in prefix persistence on eval" {
  run_lab "$PROBES" cmd-eval-prefix
  case $(lab_shell_family) in
    zsh)
      # Native zsh command builtin searches PATH only, failing on eval which has no filesystem binary.
      [ "$status" -eq 127 ]
      case $output in
        *"command not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh-sh | dash | busybox | mksh | yash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "cvar=initial" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          # ksh93u+m adheres to POSIX by stripping special built-in assignment persistence under command.
          [ "$status" -eq 0 ]
          [ "$output" = "cvar=initial" ]
          ;;
        *)
          # Legacy ksh93 (e.g. ksh93u+ 2012) preserves preceding variable assignments on builtins even under command.
          [ "$status" -eq 0 ]
          [ "$output" = "cvar=modified" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "command utility suppresses non-interactive abort on eval redirection error" {
  run_lab "$PROBES" cmd-redir-abort
  [ "$status" -eq 0 ]
  [ "$output" = "SURVIVED" ]
}

@test "eval leading dash-dash handling diverges between option-aware and operand-only shells" {
  run_lab "$PROBES" dash-dash hello
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | yash | mksh | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "[hello]" ]
      ;;
    dash | busybox)
      [ "$status" -eq 127 ]
      case $output in
        *"--: not found"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "leading space keeps a dash-leading eval operand from being parsed as an option" {
  run_lab "$PROBES" space-prefix "-n"
  [ "$status" -eq 127 ]
  case $output in
    *"invalid option"* | *"unknown option"* | *"not a valid option"* | *"usage"* | *"Usage"*) false ;;
    *"-n"*) ;;
    *) false ;;
  esac

  run_lab "$PROBES" bare-operand "-n"
  case $(lab_shell_family) in
    bash | ksh93 | yash)
      [ "$status" -eq 2 ]
      case $output in
        *"invalid option"* | *"unknown option"* | *"not a valid option"*) ;;
        *) false ;;
      esac
      ;;
    mksh | posh)
      [ "$status" -eq 1 ]
      case $output in
        *"unknown option"* | *"invalid option"*) ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh | dash | busybox)
      [ "$status" -eq 127 ]
      case $output in
        *"-n"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "in-process eval evaluates payload exceeding system ARG_MAX while execve rejects it with E2BIG" {
  arg_max=$(getconf ARG_MAX 2>/dev/null || echo 1048576)
  target_len=$((arg_max + 65536))
  run_lab "$PROBES" arg-max-payload "$target_len"
  case $(lab_shell_family) in
    zsh | zsh-sh) [ "$status" -eq 127 ] ;;
    posh) [ "$status" -eq 1 ] ;;
    bash)
      # bash 3.2 reports the failed exec as status 1; bash 4 and later report 126.
      case $(lab_shell_version) in
        3.*) [ "$status" -eq 1 ] ;;
        *) [ "$status" -eq 126 ] ;;
      esac
      ;;
    dash | ksh93 | mksh | yash | busybox) [ "$status" -eq 126 ] ;;
    *) false ;;
  esac
  case $output in
    "len=$target_len"*) ;;
    *) false ;;
  esac
  case $output in
    *"rgument list too long"*) ;;
    *) false ;;
  esac
}

@test "single-quote escaping neutralizes command injection payloads in eval" {
  d=$BATS_TEST_TMPDIR
  marker=$d/injected.txt
  payload="it's; touch $marker; echo 'x' \$(touch $marker) \`touch $marker\` \"q\" end'"
  run_lab "$PROBES" quote-safe "$payload"
  [ "$status" -eq 0 ]
  [ ! -f "$marker" ]
  [ "$output" = "evaluated_safe=[$payload]" ]
}

@test "incremental vs whole-string parsing of multiline eval with syntax error" {
  run_lab "$PROBES" incremental
  [ "$status" -ne 0 ]
  case $output in
    *"FINISHED"*) false ;;
    *) ;;
  esac
  case $(lab_shell_family) in
    bash | dash | busybox | mksh | yash | posh)
      case $output in
        "FIRST"*) ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh | ksh93)
      case $output in
        *"FIRST"*) false ;;
        *) ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "syntax error in an eval string yields a shell-specific status and may end the shell" {
  run_lab "$PROBES" syntax-status
  local p_status=$status
  local p_output=$output
  case $(lab_shell_family) in
    bash)
      run_lab_c 'if [ -o posix ]; then echo posix; else echo native; fi; echo "${BASH_VERSINFO[0]}"'
      local mode=${lines[0]} major=${lines[1]}
      if [ "$major" -ge 5 ] && [ "$mode" = "posix" ]; then
        [ "$p_status" -eq 2 ]
        [ "$p_output" = "" ]
      elif [ "$major" -ge 5 ]; then
        [ "$p_status" -eq 0 ]
        [ "$p_output" = "CONTINUED=2" ]
      else
        [ "$p_status" -eq 0 ]
        [ "$p_output" = "CONTINUED=1" ]
      fi
      ;;
    zsh | zsh-sh)
      [ "$p_status" -eq 0 ]
      [ "$p_output" = "CONTINUED=1" ]
      ;;
    ksh93)
      [ "$p_status" -eq 0 ]
      [ "$p_output" = "CONTINUED=3" ]
      ;;
    dash | busybox | yash)
      [ "$p_status" -eq 2 ]
      [ "$p_output" = "" ]
      ;;
    mksh | posh)
      [ "$p_status" -eq 1 ]
      [ "$p_output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "eval-probes returns status 64 on unknown mode" {
  run_lab "$PROBES" unknown-mode-xyz
  [ "$status" -eq 64 ]
  case $output in
    *"eval-probes: unknown mode: unknown-mode-xyz"*) ;;
    *) false ;;
  esac
}
