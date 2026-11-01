#!/usr/bin/env bats
# Tests for errexit Semantics and Its Exceptions: POSIX.1-2024 XCU 2.15 (set -e),
# 2.8.1 (Consequences of Shell Errors), 2.9.1 (Simple Commands), and 2.9.2 (Pipelines).
# Code under test runs through run_lab SCRIPT or run_lab_c CODE via labs/lib/lab.bash.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/errexit-probe.sh"
}

# detect_vanilla_bash32: set VANILLA_BASH32 to 1 when the shell under test is GNU Bash 3.2 as released
# upstream, else 0. Apple's /bin/bash reports the same 3.2.57 version but is built for darwin and carries
# an added errexit check in execute_cmd.c, so MACHTYPE tells the two apart.
# Call it before the code under test: it overwrites bats's status and output.
detect_vanilla_bash32() {
  VANILLA_BASH32=0
  case $(lab_shell_version) in
    3.2*)
      # shellcheck disable=SC2016
      run_lab_c 'printf %s "$MACHTYPE"'
      case $output in
        *-apple-darwin*) ;;
        *) VANILLA_BASH32=1 ;;
      esac
      ;;
    *) ;;
  esac
}

# --- Mechanism Probes via errexit-probe.sh ---

@test "probe andor verifies set -e suppression on non-terminal operands and termination on terminal" {
  run_lab "$PROBE" andor
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 pdksh bug: suppresses errexit across all operands of AND-OR list
      [ "$output" = "nonterm_st=0 nonterm_out=survived term_st=0 term_out=survived" ]
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      # POSIX.1-2024 XCU 2.15 exception 2: non-terminal operand ignored, terminal operand active
      [ "$output" = "nonterm_st=0 nonterm_out=survived term_st=1 term_out=empty" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe fn_cond verifies set -e suppression in function body under if and OR conditions" {
  run_lab "$PROBE" fn_cond
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      # POSIX.1-2024 XCU 2.15 exception 2: functions inherit conditional ignore context
      [ "$output" = "if=reached branch or=reached" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe cmdsub_assign verifies exit status propagation in bare assignment vs masking in declaration" {
  run_lab "$PROBE" cmdsub_assign
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      # POSIX.1-2024 XCU 2.9.1: bare assignment takes command sub status; export masks with 0
      [ "$output" = "bare_st=42 bare_out=empty export_st=0 export_out=survived" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe cmdsub_inherit verifies command substitution errexit inheritance" {
  run_lab "$PROBE" cmdsub_inherit
  [ "$status" -eq 0 ]
  probe_output="$output"
  case $(lab_shell_family) in
    bash)
      # Native bash disables errexit in command substitutions unless inherit_errexit or posix mode
      # shellcheck disable=SC2016 # probe bash posix mode feature
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$probe_output" = "inherit_st=0 inherit_out=two" ]
      else
        [ "$probe_output" = "inherit_st=0 inherit_out=one two" ]
      fi
      ;;
    busybox)
      # BusyBox ash clears eflag in evalbackcmd (shell/ash.c), disabling errexit in $(...)
      [ "$probe_output" = "inherit_st=0 inherit_out=one two" ]
      ;;
    zsh | zsh-sh | dash | ksh93 | mksh | yash | posh)
      # POSIX.1-2024 XCU 2.15: subshell command substitution inherits errexit
      [ "$probe_output" = "inherit_st=0 inherit_out=two" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe pipe_pipefail verifies pipeline errexit with and without pipefail" {
  run_lab_c 'set -o pipefail 2>/dev/null'
  has_pipefail=$status
  run_lab "$PROBE" pipe_pipefail
  [ "$status" -eq 0 ]
  probe_output="$output"
  case $(lab_shell_family) in
    posh)
      [ "$probe_output" = "def_st=0 def_out=survived pf_st=-1 pf_out=unsupported" ]
      ;;
    dash)
      if [ "$has_pipefail" -eq 0 ]; then
        [ "$probe_output" = "def_st=0 def_out=survived pf_st=1 pf_out=empty" ]
      else
        [ "$probe_output" = "def_st=0 def_out=survived pf_st=-1 pf_out=unsupported" ]
      fi
      ;;
    ksh93)
      version=$(lab_shell_version)
      case $version in
        *93u+m*)
          [ "$probe_output" = "def_st=0 def_out=survived pf_st=1 pf_out=empty" ]
          ;;
        *)
          # ksh93u+ 2012 bug: pipefail status is not inspected by errexit if last stage succeeded
          [ "$probe_output" = "def_st=0 def_out=survived pf_st=0 pf_out=survived" ]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | mksh | yash | busybox)
      [ "$probe_output" = "def_st=0 def_out=survived pf_st=1 pf_out=empty" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe subshell_pipe verifies canonical POSIX subshell pipeline isolation" {
  run_lab "$PROBE" subshell_pipe
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      [ "$output" = "subshell_pipe=two" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe fn_invert verifies pipeline inversion in function call" {
  run_lab "$PROBE" fn_invert
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh | posh)
      # zsh and posh suppress errexit when returning from a function whose last command was ! inverted
      [ "$output" = "invert_st=0 invert_out=survived" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox)
      # POSIX simple command f returns 1; caller simple command has status 1 and triggers errexit
      [ "$output" = "invert_st=1 invert_out=empty" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe trap_exit verifies EXIT trap fires on errexit failure with matching status" {
  run_lab "$PROBE" trap_exit
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      [ "$output" = "trap_st=1 trap_out=trap=1" ]
      ;;
    *)
      false
      ;;
  esac
}

# --- Direct Specification Assertions via run_lab_c ---

@test "if condition suppresses errexit but then body enforces it" {
  # POSIX.1-2024 XCU 2.15 exception 2: compound list following if is ignored; body is active
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    if false; then
      printf "unreached\n"
    else
      printf "else_branch\n"
    fi
  '
  [ "$status" -eq 0 ]
  [ "$output" = "else_branch" ]

  # Inside then body: failure must cause shell exit
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    if true; then
      false
      printf "unreached\n"
    fi
  '
  [ "$status" -eq 1 ]
  [ "$output" = "" ]
}

@test "while condition suppresses errexit but loop body enforces it" {
  # POSIX.1-2024 XCU 2.15 exception 2: compound list following while is ignored
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    while false; do
      printf "unreached\n"
    done
    printf "after_while\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "after_while" ]

  # Inside while body: failure must cause shell exit
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    while true; do
      false
      printf "unreached\n"
    done
  '
  [ "$status" -eq 1 ]
  [ "$output" = "" ]
}

@test "pipeline beginning with ! suppresses errexit on the pipeline itself" {
  # POSIX.1-2024 XCU 2.15 exception 2: pipeline beginning with ! reserved word ignores -e
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    ! true
    printf "survived\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "survived" ]
}

@test "bare subshell failure triggers parent errexit except in upstream Bash 3.2" {
  # POSIX.1-2024 XCU 2.15: a subshell command is not excluded from errexit, so its failure exits the parent
  detect_vanilla_bash32
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    (exit 7)
    printf "unreached\n"
  '
  if [ "$VANILLA_BASH32" -eq 1 ]; then
    # Upstream Bash 3.2.57 (before Bash 4.0 CHANGES item l) ignores the subshell status;
    # Apple's patched 3.2.57 and every later Bash exit with 7.
    [ "$status" -eq 0 ]
    [ "$output" = "unreached" ]
  else
    case $(lab_shell_family) in
      bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
        [ "$status" -eq 7 ]
        [ "$output" = "" ]
        ;;
      *)
        false
        ;;
    esac
  fi
}

@test "exception 3: a compound command whose status came from an ignored failure does not trigger errexit" {
  # POSIX.1-2024 XCU 2.15 exception 3: false is the non-last operand of an AND-OR list, so -e is ignored for it;
  # the group, if, and for commands take its status 1, and -e does not apply to them either.
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    { false && true; }
    printf "group\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "group" ]
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    if true; then false && true; fi
    printf "if\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "if" ]
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    for i in 1; do false && true; done
    printf "for\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "for" ]
}

@test "exception 3 excludes a subshell command: its ignored failure still exits the parent" {
  detect_vanilla_bash32
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    (false && true)
    printf "unreached\n"
  '
  if [ "$VANILLA_BASH32" -eq 1 ]; then
    # Upstream Bash 3.2.57 ignores the subshell status entirely (see the bare subshell test)
    [ "$status" -eq 0 ]
    [ "$output" = "unreached" ]
  else
    [ "$status" -eq 1 ]
    [ "$output" = "" ]
  fi
}

@test "last operand of an OR list triggers errexit except in posh" {
  # POSIX.1-2024 XCU 2.15 exception 2 ignores -e only for commands other than the last in an AND-OR list
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    false || false
    printf "survived\n"
  '
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 passes XERROK to both operands in execute()
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "explicit return restores errexit after an inverted last command in a function" {
  # The function status is 1 and the caller is a simple command, so every shell exits;
  # zsh and posh survive only when the function ends in the inverted pipeline itself (fn_invert probe).
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    f() {
      ! true
      return 1
    }
    f
    printf "unreached\n"
  '
  [ "$status" -eq 1 ]
  [ "$output" = "" ]
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    f() {
      ! true
      false
    }
    f
    printf "unreached\n"
  '
  [ "$status" -eq 1 ]
  [ "$output" = "" ]
}

@test "set -o posix with errexit: command substitution keeps -e, or the option is rejected" {
  # The option is not specified by POSIX.1-2024 XCU 2.15; this records what each shell does with it.
  # shellcheck disable=SC2016
  run_lab_c '
    exec 2>/dev/null
    set -o posix
    set -e
    echo $(false; echo 1) 2
  '
  case $(lab_shell_family) in
    bash | mksh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "2" ]
      ;;
    zsh | zsh-sh | posh)
      # option rejected: the shell exits with status 1 before the echo
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$status" -eq 0 ]
          [ "$output" = "2" ]
          ;;
        *)
          [ "$status" -eq 2 ]
          [ "$output" = "" ]
          ;;
      esac
      ;;
    busybox)
      # option rejected but the script continues, and the substitution does not inherit -e
      [ "$status" -eq 0 ]
      [ "$output" = "1 2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "redirection error on a utility, compound command, or function call does not exit the shell" {
  # POSIX.1-2024 XCU 2.8.1 consequences of shell errors
  # shellcheck disable=SC2016
  run_lab_c '
    exec 2>/dev/null
    cat </nonexistent/x
    printf "utility\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "utility" ]
  # shellcheck disable=SC2016
  run_lab_c '
    exec 2>/dev/null
    { :; } </nonexistent/x
    printf "group\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "group" ]
  # shellcheck disable=SC2016
  run_lab_c '
    exec 2>/dev/null
    f() { :; }
    f </nonexistent/x
    printf "function\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "function" ]
}

@test "redirection error on a special built-in exits a non-interactive shell except in bash, native zsh, and yash" {
  # POSIX.1-2024 XCU 2.8.1: a redirection error on a special built-in utility shall exit a non-interactive shell
  family=$(lab_shell_family)
  posix=1
  if [ "$family" = bash ]; then
    run_lab_c '[ -o posix ]'
    posix=$status
  fi
  # shellcheck disable=SC2016
  run_lab_c '
    exec 2>/dev/null
    : </nonexistent/x
    printf "survived\n"
  '
  case $family in
    bash)
      if [ "$posix" -eq 0 ]; then
        [ "$status" -eq 1 ]
        [ "$output" = "" ]
      else
        [ "$status" -eq 0 ]
        [ "$output" = "survived" ]
      fi
      ;;
    zsh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "" ]
      ;;
    zsh-sh | ksh93 | mksh | busybox | posh)
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "wrapping a function in a subshell on the left of || keeps errexit off; the set +e capture idiom keeps it on" {
  # Austin Group Defects 52 and 537: -e stays ignored inside a function called where -e is ignored
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    f() {
      false
      printf "continued\n"
    }
    (f) || printf "handled\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "continued" ]
  # shellcheck disable=SC2016
  run_lab_c '
    set -e
    f() {
      false
      printf "continued\n"
    }
    set +e
    (
      set -e
      f
    )
    st=$?
    set -e
    printf "st=%d\n" "$st"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "st=1" ]
}

@test "local or typeset declaration masks return value in function" {
  # SC2155 check: declaration utility masks command substitution return value
  case $(lab_shell_family) in
    ksh93)
      # ksh93 uses typeset rather than local for scope declaration
      # shellcheck disable=SC2016
      run_lab_c '
        set -e
        test_typeset() {
          typeset v="$(false)"
          printf "local_status=%d\n" "$?"
        }
        test_typeset
      '
      [ "$status" -eq 0 ]
      [ "$output" = "local_status=0" ]
      ;;
    bash | zsh | zsh-sh | dash | mksh | yash | busybox | posh)
      # shellcheck disable=SC2016
      run_lab_c '
        set -e
        test_local() {
          local v="$(false)"
          printf "local_status=%d\n" "$?"
        }
        test_local
      '
      [ "$status" -eq 0 ]
      [ "$output" = "local_status=0" ]
      ;;
    *)
      false
      ;;
  esac
}
