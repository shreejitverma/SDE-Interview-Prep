#!/usr/bin/env bats
# Tests for reading the POSIX standard: verifying XCU Chapter 2 specification
# rules across conforming shells and matrix families.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/posix_rules.sh"
}

@test "special built-in preceding assignments persist in execution environment" {
  run_lab "$PROBE" assign-persist
  case $(lab_shell_family) in
    dash | ksh93 | mksh | posh | busybox | yash | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "colon:active true:unset" ]
      ;;
    bash)
      probe_output="$output"
      run_lab_c '[ -o posix ] && echo posix || echo native'
      [ "$status" -eq 0 ]
      if [ "$output" = "posix" ]; then
        [ "$probe_output" = "colon:active true:unset" ]
      else
        [ "$probe_output" = "colon:unset true:unset" ]
      fi
      ;;
    zsh)
      [ "$status" -eq 0 ]
      [ "$output" = "colon:unset true:unset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "redirection error on special built-in aborts non-interactive shell" {
  run_lab "$PROBE" error-abort --direct "$BATS_TEST_TMPDIR/nonexistent"
  case $(lab_shell_family) in
    dash | ksh93 | mksh | posh | busybox | zsh-sh)
      [ "$status" -ne 0 ]
      case $output in
        *direct-survived*) false ;;
        *) true ;;
      esac
      ;;
    bash)
      probe_status="$status"
      probe_output="$output"
      run_lab_c '[ -o posix ] && echo posix || echo native'
      [ "$status" -eq 0 ]
      if [ "$output" = "posix" ]; then
        [ "$probe_status" -ne 0 ]
        case $probe_output in
          *direct-survived*) false ;;
          *) true ;;
        esac
      else
        [ "$probe_status" -eq 0 ]
        case $probe_output in
          *direct-survived*) true ;;
          *) false ;;
        esac
      fi
      ;;
    zsh | yash)
      [ "$status" -eq 0 ]
      case $output in
        *direct-survived*) true ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "command utility shields special built-in redirection error from aborting" {
  run_lab "$PROBE" error-abort --shielded "$BATS_TEST_TMPDIR/nonexistent"
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      [ "$status" -eq 0 ]
      case $output in
        *shielded-survived*) true ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "declaration utility suppresses field splitting on variable assignment" {
  run_lab "$PROBE" declaration-context --literal
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "<alpha beta gamma>" ]
      ;;
    posh)
      [ "$status" -eq 0 ]
      [ "$output" = "<alpha>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "dynamic command execution makes assignment context unspecified" {
  run_lab "$PROBE" declaration-context --dynamic
  case $(lab_shell_family) in
    bash | ksh93 | mksh | posh | yash | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "<alpha>" ]
      ;;
    dash | busybox | zsh)
      [ "$status" -eq 0 ]
      [ "$output" = "<alpha beta gamma>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "function definition overrides regular utility" {
  run_lab "$PROBE" function-precedence --regular
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "shadowed-true" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "function definition targeting special built-in behaves per implementation" {
  run_lab "$PROBE" function-precedence --special
  case $(lab_shell_family) in
    dash | posh | ksh93 | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "rejected" ]
      ;;
    mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "builtin-ran" ]
      ;;
    zsh | zsh-sh | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "function-ran" ]
      ;;
    bash)
      probe_output="$output"
      run_lab_c '[ -o posix ] && echo posix || echo native'
      [ "$status" -eq 0 ]
      if [ "$output" = "posix" ]; then
        [ "$probe_output" = "rejected" ]
      else
        [ "$probe_output" = "function-ran" ]
      fi
      ;;
    *)
      false
      ;;
  esac
}

@test "unfound utility returns exit status 127" {
  run_lab "$PROBE" lookup-status "$BATS_TEST_TMPDIR/missing_utility_cmd"
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "127" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "non-executable utility returns exit status 126" {
  noexec_target="$BATS_TEST_TMPDIR/noexec_file"
  touch "$noexec_target"
  chmod -x "$noexec_target"

  run_lab "$PROBE" lookup-status "$noexec_target"
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "126" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "signal termination exit status complies with XCU 2.8.2 greater than 128" {
  # shellcheck disable=SC2016 # expression evaluated inside shell under test
  run_lab_c '{ sh -c "kill -s TERM \$\$"; } 2>/dev/null; echo "$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93)
      [ "$output" = "271" ]
      ;;
    yash)
      [ "$output" = "399" ]
      ;;
    dash | bash | zsh | zsh-sh | mksh | posh | busybox)
      [ "$output" = "143" ]
      ;;
    *)
      false
      ;;
  esac
}
