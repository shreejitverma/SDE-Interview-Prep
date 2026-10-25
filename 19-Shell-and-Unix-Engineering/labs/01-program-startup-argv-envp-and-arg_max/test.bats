#!/usr/bin/env bats
# Tests for Program Startup: argv, envp, and ARG_MAX.

setup() {
  load ../lib/lab
  STARTUP_PROBE="$BATS_TEST_DIRNAME/startup-probe.sh"
  ARGV0_PROBE="$BATS_TEST_TMPDIR/argv0-probe"
  if [ ! -x "$ARGV0_PROBE" ]; then
    if command -v gcc >/dev/null 2>&1; then
      gcc -O2 "$BATS_TEST_DIRNAME/argv0-probe.c" -o "$ARGV0_PROBE"
    elif command -v cc >/dev/null 2>&1; then
      cc -O2 "$BATS_TEST_DIRNAME/argv0-probe.c" -o "$ARGV0_PROBE"
    elif command -v clang >/dev/null 2>&1; then
      clang -O2 "$BATS_TEST_DIRNAME/argv0-probe.c" -o "$ARGV0_PROBE"
    fi
  fi
}

@test "startup-probe preserves spaces and empty arguments" {
  run_lab "$STARTUP_PROBE" "first arg" "" "third arg"
  [ "$status" -eq 0 ]
  [ "${lines[1]}" = "argc: 3" ]
  [ "${lines[2]}" = "args: <first arg> <> <third arg>" ]
  [ "${lines[3]}" = "env: <UNSET>" ]
}

@test "quoted glob character is literal while unquoted glob is expanded by shell" {
  mkdir -p "$BATS_TEST_TMPDIR/glob_test"
  touch "$BATS_TEST_TMPDIR/glob_test/file1.tmp" "$BATS_TEST_TMPDIR/glob_test/file2.tmp"

  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c '
    probe="$1"
    dir="$2"
    cd "$dir" || exit 1
    # shellcheck disable=SC2086,SC2248 # intentional unquoted glob to demonstrate expansion
    "$probe" "*" *
  ' sh "$STARTUP_PROBE" "$BATS_TEST_TMPDIR/glob_test"

  [ "$status" -eq 0 ]
  [ "${lines[1]}" = "argc: 3" ]
  [ "${lines[2]}" = "args: <*> <file1.tmp> <file2.tmp>" ]
}

@test "exported environment variable is inherited while unexported variable is omitted" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c '
    probe="$1"
    STARTUP_PROBE_ENV="inherited_val"
    export STARTUP_PROBE_ENV
    UNEXPORTED_VAR="secret_val"
    "$probe"
  ' sh "$STARTUP_PROBE"

  [ "$status" -eq 0 ]
  [ "${lines[3]}" = "env: <inherited_val>" ]
}

@test "shell builtin handles arguments larger than ARG_MAX without calling execve" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c '
    big=$(dd if=/dev/zero bs=1024 count=1100 2>/dev/null | tr "\0" "a")
    printf "%d\n" "${#big}"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "1126400" ]
}

@test "external command execution exceeding ARG_MAX fails with E2BIG exit status" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c '
    big=$(dd if=/dev/zero bs=1024 count=1100 2>/dev/null | tr "\0" "a")
    /bin/echo "$big"
  '
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$status" -eq 127 ]
      ;;
    posh)
      [ "$status" -eq 1 ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox)
      [ "$status" -eq 126 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "exec -a sets custom argv0 in supporting shells and fails in strict POSIX shells" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c 'probe="$1"; exec -a custom_proc_name "$probe"' sh "$ARGV0_PROBE"

  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "custom_proc_name" ]
      ;;
    dash | posh)
      [ "$status" -ne 0 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "positional parameter count matches exact argument list length" {
  run_lab "$STARTUP_PROBE" a b c d e
  [ "$status" -eq 0 ]
  [ "${lines[1]}" = "argc: 5" ]
  [ "${lines[2]}" = "args: <a> <b> <c> <d> <e>" ]
}
