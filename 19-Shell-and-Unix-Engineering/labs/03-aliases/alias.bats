#!/usr/bin/env bats
# Tests for POSIX alias substitution semantics and shell family divergences.

setup() {
  load ../lib/lab
  PROBE="${BATS_TEST_DIRNAME}/alias-probe.sh"
}

@test "probe runs and verifies trailing space, quoted bypass, recursion, and parse barrier" {
  case $(lab_shell_family) in
    posh)
      run_lab "${PROBE}"
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_trailing_space: pfx: target_expanded" ]
      [ "${lines[1]}" = "2_quoted_bypass:  alias_active:normal | real_cmd:backslash | real_cmd:doublequote | real_cmd:singlequote" ]
      [ "${lines[2]}" = "3_self_recursion: wrapped(inner outer)" ]
      [ "${lines[3]}" = "4_mutual_cycle:   cycle_target(arg)" ]
      [ "${lines[4]}" = "5_parse_barrier:  barrier_held" ]
      [ "${lines[5]}" = "6_eval_dynamic:   eval_succeeded" ]
      [ "${lines[6]}" = "7_operator_fuse:  unquoted=<0> quoted=<0>" ]
      ;;
    dash | ksh93 | yash | busybox | mksh)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_trailing_space: pfx: target_expanded" ]
      [ "${lines[1]}" = "2_quoted_bypass:  alias_active:normal | real_cmd:backslash | real_cmd:doublequote | real_cmd:singlequote" ]
      [ "${lines[2]}" = "3_self_recursion: wrapped(inner outer)" ]
      [ "${lines[3]}" = "4_mutual_cycle:   cycle_target(arg)" ]
      [ "${lines[4]}" = "5_parse_barrier:  barrier_held" ]
      [ "${lines[5]}" = "6_eval_dynamic:   eval_succeeded" ]
      [ "${lines[6]}" = "7_operator_fuse:  unquoted=<> quoted=<0>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "quoted words suppress alias substitution" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias f="printf alias"'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        f() { printf "fn:%s " "${*}"; }
        alias f="printf alias:%s "
        eval "\f quoted; f unquoted"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "fn:quoted alias:unquoted" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "compound command barrier delays alias visibility until subsequent parsing unit" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias test_cmd="printf expanded"'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        if true; then
          alias test_cmd="printf expanded"
          test_cmd 2>/dev/null || printf "barrier_held"
        fi
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "barrier_held" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "eval executes runtime parser to resolve newly defined alias inside compound command" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias test_cmd="printf expanded"'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        if true; then
          alias test_cmd="printf expanded"
          eval test_cmd
        fi
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "expanded" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "trailing blank triggers chained alias substitution across words" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias a="pfx "'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        pfx() { printf "pfx:%s" "${*}"; }
        alias a="pfx "
        alias b="final"
        eval "a b"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "pfx:final" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "direct self-recursion terminates at recursion depth 1" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias wrap="wrap base"'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        wrap() { printf "wrap:%s" "${*}"; }
        alias wrap="wrap base"
        eval "wrap user"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "wrap:base user" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "mutual alias recursion cycle terminates without hanging" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias cycle_target="cycle_step_b "'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        cycle_target() { printf "cycle_target(%s)" "${*}"; }
        alias cycle_target="cycle_step_b "
        alias cycle_step_b="cycle_target "
        eval "cycle_target arg"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "cycle_target(arg)" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "non-interactive bash disables alias expansion by default unless posix or shopt is set" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias unexp="printf active"'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "${status}" -eq 0 ]; then
        run_lab_c 'alias unexp="printf posix_active"; eval "unexp"'
        [ "${status}" -eq 0 ]
        [ "${output}" = "posix_active" ]
      else
        run_lab_c 'alias unexp="printf fail"; eval "unexp 2>/dev/null || printf disabled"'
        [ "${status}" -eq 0 ]
        [ "${output}" = "disabled" ]
      fi
      ;;
    dash | ksh93 | zsh | zsh-sh | yash | busybox | mksh)
      run_lab_c 'alias unexp="printf active"; eval "unexp"'
      [ "${status}" -eq 0 ]
      [ "${output}" = "active" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unalias removes single alias and unalias -a removes all aliases" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'unalias a'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"unalias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        alias a="printf a" b="printf b"
        unalias a
        alias a >/dev/null 2>&1 || printf "a_removed "
        unalias -a
        printf "[%s]" "$(alias)"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "a_removed []" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "a trailing blank re-substitutes a repeated input word" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias s="f "'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        [ -n "${BASH_VERSION-}" ] && shopt -s expand_aliases
        f() { printf "%s," "$@"; }
        alias s="f "
        eval "s s s x"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "f,f,x," ]
      ;;
    *)
      false
      ;;
  esac
}

@test "alias NAME fails for an undefined alias and the diagnostic stream differs by shell" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh)
      run_lab_c 'alias nonexistent_zz'
      [ "${status}" -eq 127 ]
      case "${output}" in
        *"alias: not found"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | yash | busybox | mksh)
      run_lab_c '
        out=$(alias nonexistent_zz 2>/dev/null)
        st=$?
        err=$(alias nonexistent_zz 2>&1 >/dev/null)
        unalias -a
        ua=$?
        printf "%s|%s|%s|%s" "${st}" "${out}" "${err:+err}" "${ua}"
      '
      [ "${status}" -eq 0 ]
      case $(lab_shell_family) in
        mksh)
          [ "${output}" = "1|nonexistent_zz alias not found||0" ]
          ;;
        zsh | zsh-sh)
          [ "${output}" = "1|||0" ]
          ;;
        bash | dash | ksh93 | yash | busybox)
          [ "${output}" = "1||err|0" ]
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
