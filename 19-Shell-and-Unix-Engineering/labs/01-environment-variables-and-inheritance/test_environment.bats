#!/usr/bin/env bats
# test_environment.bats: test environment variable inheritance, scoping, and limits.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/env_probe.sh"
  HELPER="$BATS_TEST_TMPDIR/env_helper"
  "${CC:-cc}" -O2 -Wall -Wextra -Werror "$BATS_TEST_DIRNAME/env_helper.c" -o "$HELPER"
}

@test "unexported shell variables are inherited by subshells but not external commands" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    LOCAL_VAR="alpha"
    ( [ "$LOCAL_VAR" = "alpha" ] ) || exit 1
    "$1" dump LOCAL_VAR
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "shell: UNSET" ]
  [ "${lines[1]}" = "env: UNSET" ]
}

@test "export attribute marks variable for external command inheritance" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    export EXP_VAR="bravo"
    "$1" dump EXP_VAR
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "shell: bravo" ]
  [ "${lines[1]}" = "env: bravo" ]
}

@test "unset clears both variable value and export attribute" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    export FOO="charlie"
    unset FOO
    FOO="delta"
    child_out=$("$1" dump FOO)
    printf "parent: %s\n%s\n" "$FOO" "$child_out"
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "parent: delta" ]
  [ "${lines[1]}" = "shell: UNSET" ]
  [ "${lines[2]}" = "env: UNSET" ]
}

@test "child process environment modification cannot mutate parent environment" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    PARENT_VAR="original"
    export PARENT_VAR
    child_out=$("$1" child_mutate PARENT_VAR "mutated")
    [ "$child_out" = "child_set: mutated" ] || exit 1
    printf "%s\n" "$PARENT_VAR"
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "original" ]
}

@test "command prefix assignment exports variable temporarily for simple command" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    PREFIX_VAR="base"
    out=$(PREFIX_VAR="override" "$1" prefix PREFIX_VAR)
    printf "%s|%s\n" "$out" "$PREFIX_VAR"
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "prefix_val: override|base" ]
}

@test "command prefix assignment preceding special builtin persists per POSIX XCU 2.9.1.2" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    unset SPEC_VAR
    SPEC_VAR="persisted" :
    printf "%s\n" "${SPEC_VAR-}"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | mksh | ksh93 | busybox | yash | zsh-sh)
      [ "$output" = "persisted" ]
      ;;
    bash)
      lab_shell_words
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        [ "$output" = "persisted" ]
      else
        [ "$output" = "" ]
      fi
      ;;
    zsh)
      [ "$output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command prefix assignment preceding shell function persistence divergence" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    dummy_fn() { :; }
    unset FN_VAR
    FN_VAR="temp" dummy_fn
    printf "%s\n" "${FN_VAR-}"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93 | zsh-sh)
      [ "$output" = "temp" ]
      ;;
    bash)
      lab_shell_words
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        version=$(lab_shell_version)
        case "$version" in
          [0-4].*)
            [ "$output" = "temp" ]
            ;;
          *)
            [ "$output" = "" ]
            ;;
        esac
      else
        [ "$output" = "" ]
      fi
      ;;
    dash | posh | mksh | busybox | yash | zsh)
      [ "$output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -a automatically exports subsequent assignments" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    set -a
    AUTO_VAR="auto_exported"
    "$1" dump AUTO_VAR
  ' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "shell: auto_exported" ]
  [ "${lines[1]}" = "env: auto_exported" ]
}

@test "initial environ array starts on stack and relocates to heap upon modification" {
  run "$HELPER" mem
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "initial: stack" ]
  [ "${lines[1]}" = "post-setenv: heap" ]
}

@test "libc getenv retrieves first match of duplicate keys in envp" {
  run "$HELPER" getenv_dup KEY
  [ "$status" -eq 0 ]
  [ "$output" = "getenv: first" ]
}

@test "shell import of duplicate keys resolves deterministically" {
  lab_shell_words
  shell_bin=$(command -v "${LAB_SHELL_WORDS[0]}")
  cmd_words=("$shell_bin")
  if [ "${#LAB_SHELL_WORDS[@]}" -gt 1 ]; then
    cmd_words+=("${LAB_SHELL_WORDS[@]:1}")
  fi
  # shellcheck disable=SC2016 # expression belongs to shell under test
  run "$HELPER" exec_dup "${cmd_words[@]}" 'printf "%s\n" "$KEY"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | dash | posh | yash | busybox)
      [ "$output" = "second" ]
      ;;
    zsh)
      [ "$output" = "first" ]
      ;;
    ksh93)
      version=$(lab_shell_version)
      case "$version" in
        *93u+m*) [ "$output" = "second" ] ;;
        *) [ "$output" = "first" ] ;;
      esac
      ;;
    mksh)
      # Measured: Debian mksh R59 2025/04/26 keeps the first entry, Alpine mksh R59 2020/10/31
      # keeps the last; the cause of the build difference is not identified, so other builds
      # may report either.
      version=$(lab_shell_version)
      case "$version" in
        *"R59 2020/10/31"*) [ "$output" = "second" ] ;;
        *"R59 2025/04/26"*) [ "$output" = "first" ] ;;
        *)
          case "$output" in
            first | second) ;;
            *) false ;;
          esac
          ;;
      esac
      ;;
    zsh-sh)
      [ "$output" = "first" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "invalid variable identifier names in envp are handled per implementation" {
  lab_shell_words
  shell_bin=$(command -v "${LAB_SHELL_WORDS[0]}")
  cmd_words=("$shell_bin")
  if [ "${#LAB_SHELL_WORDS[@]}" -gt 1 ]; then
    cmd_words+=("${LAB_SHELL_WORDS[@]:1}")
  fi
  # shellcheck disable=SC2016 # expression belongs to shell under test
  run "$HELPER" exec_invalid "${cmd_words[@]}" 'export PATH="/usr/bin:/bin"; env | awk -F= "/^bad-ident=/ { found=1; print \$2 } END { if (!found) print \"dropped\" }"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | mksh)
      [ "$output" = "dropped" ]
      ;;
    bash | ksh93 | yash)
      [ "$output" = "probe_value" ]
      ;;
    busybox)
      version=$(lab_shell_version)
      case "$version" in
        n/a)
          [ "$output" = "dropped" ]
          ;;
        *)
          [ "$output" = "probe_value" ]
          ;;
      esac
      ;;
    zsh | zsh-sh)
      # zsh 5.9 keeps the invalid name and passes it on, natively and under --emulate sh
      [ "$output" = "probe_value" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "export -p round-trips variable values including spaces and quotes" {
  # shellcheck disable=SC2016 # tested under shell under test
  run_lab_c '
    export ROUNDTRIP_VAR="alpha '\''beta'\'' gamma"
    saved=$(export -p | grep "ROUNDTRIP_VAR")
    unset ROUNDTRIP_VAR
    eval "$saved"
    printf "%s\n" "$ROUNDTRIP_VAR"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 export -p fails to quote values containing spaces, truncating at first space
      [ "$output" = "alpha" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "$output" = "alpha 'beta' gamma" ]
      ;;
    *)
      false
      ;;
  esac
}
