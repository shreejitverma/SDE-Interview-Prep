#!/usr/bin/env bats

setup() {
  load ../lib/lab
  SCRIPT="$BATS_TEST_DIRNAME/audit-dispatch.sh"
}

@test "vuln-subshell executes injected command after semicolon while safe-argv treats it as data" {
  payload='alpha; printf "PWNED_%s\n" "SUBSHELL"'

  run_lab "$SCRIPT" vuln-subshell "$payload"
  [ "$status" -eq 0 ]
  [[ "$output" == *"record:alpha"* ]]
  [[ "$output" == *"PWNED_SUBSHELL"* ]]

  run_lab "$SCRIPT" safe-argv "$payload"
  [ "$status" -eq 0 ]
  [ "$output" = 'record:alpha; printf "PWNED_%s\n" "SUBSHELL"' ]
}

@test "ordinary parameter expansion never re-parses command substitution or control operators" {
  # shellcheck disable=SC2016 # literal $(...) and ; are the untrusted payload under test
  payload='$(printf "PWNED_%s\n" "EXPAND");id'

  run_lab "$SCRIPT" quote-boundary "$payload"
  [ "$status" -eq 0 ]
  # shellcheck disable=SC2016 # verifying literal unexpanded output
  [ "$output" = 'quoted:$(printf "PWNED_%s\n" "EXPAND");id' ]
}

@test "vuln-arith recursively evaluates variable assignment in shells with recursive math" {
  run_lab "$SCRIPT" vuln-arith 'a=41'
  case $(lab_shell_family) in
    dash | yash)
      [ "$status" -ne 0 ]
      [[ "$output" != *"val:42"* ]]
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "val:42 a:41" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "vuln-arith executes command substitution inside array subscript in shells with subscript expansion" {
  # shellcheck disable=SC2016 # literal command substitution inside array subscript payload
  payload='a[$(printf "PWNED_%s\n" "ARITH" >&2)0]'
  run_lab "$SCRIPT" vuln-arith "$payload"
  case $(lab_shell_family) in
    dash | yash | busybox)
      [ "$status" -ne 0 ]
      [[ "$output" != *"PWNED_ARITH"* ]]
      ;;
    ksh93)
      version=$(lab_shell_version)
      if [[ "$version" == *"93u+m"* ]]; then
        [ "$status" -eq 1 ]
        [[ "$output" == *"arithmetic syntax error"* ]]
        [[ "$output" != *"PWNED_ARITH"* ]]
      else
        [ "$status" -eq 0 ]
        [[ "$output" == *"PWNED_ARITH"* ]]
        [[ "$output" == *"val:1 a:0"* ]]
      fi
      ;;
    bash | zsh | zsh-sh | mksh | posh)
      [ "$status" -eq 0 ]
      [[ "$output" == *"PWNED_ARITH"* ]]
      [[ "$output" == *"val:1 a:0"* ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "safe-arith blocks both assignment and subscript command injection across all shells" {
  run_lab "$SCRIPT" safe-arith '41'
  [ "$status" -eq 0 ]
  [ "$output" = "val:42 a:0" ]

  run_lab "$SCRIPT" safe-arith 'a=41'
  [ "$status" -eq 2 ]
  [[ "$output" == *"error:invalid integer"* ]]

  # shellcheck disable=SC2016 # literal command substitution payload
  run_lab "$SCRIPT" safe-arith 'a[$(printf "PWNED_%s\n" "ARITH" >&2)0]'
  [ "$status" -eq 2 ]
  [[ "$output" != *"PWNED_ARITH"* ]]
}

@test "safe-arith rejects leading zeros and over-long values that shells evaluate inconsistently" {
  run_lab "$SCRIPT" safe-arith '0'
  [ "$status" -eq 0 ]
  [ "$output" = "val:1 a:0" ]

  run_lab "$SCRIPT" safe-arith '999999999'
  [ "$status" -eq 0 ]
  [ "$output" = "val:1000000000 a:0" ]

  for metric in 010 08 00 1234567890; do
    run_lab "$SCRIPT" safe-arith "$metric"
    [ "$status" -eq 2 ]
    [ "$output" = "error:invalid integer" ]
  done
}

@test "vuln-test-eq executes subscript command substitution in Korn-derived test builtins" {
  # shellcheck disable=SC2016 # literal command substitution inside array subscript payload
  payload='a[$(printf "PWNED_%s\n" "TESTEQ" >&2)0]'
  run_lab "$SCRIPT" vuln-test-eq "$payload"
  case $(lab_shell_family) in
    dash | bash | zsh | zsh-sh | yash | busybox)
      [ "$status" -ne 0 ]
      [[ "$output" != *"PWNED_TESTEQ"* ]]
      ;;
    ksh93)
      version=$(lab_shell_version)
      if [[ "$version" == *"93u+m"* ]]; then
        [ "$status" -eq 2 ]
        [[ "$output" == *"arithmetic syntax error"* ]]
        [[ "$output" != *"PWNED_TESTEQ"* ]]
      else
        [ "$status" -eq 0 ]
        [[ "$output" == *"PWNED_TESTEQ"* ]]
        [[ "$output" == *"zero a:0"* ]]
      fi
      ;;
    mksh | posh)
      [ "$status" -eq 0 ]
      [[ "$output" == *"PWNED_TESTEQ"* ]]
      [[ "$output" == *"zero a:0"* ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "vuln-indirection via read executes array subscript command substitution where supported" {
  # shellcheck disable=SC2016 # literal command substitution payload for read variable name
  payload='a[$(printf "PWNED_%s\n" "READ" >&2)0]'
  run_lab "$SCRIPT" vuln-indirection "$payload" "secret"
  case $(lab_shell_family) in
    dash | busybox | ksh93)
      [ "$status" -ne 0 ]
      [[ "$output" != *"PWNED_READ"* ]]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [[ "$output" != *"PWNED_READ"* ]]
      [ "$output" = "a:0" ]
      ;;
    zsh)
      [ "$status" -ne 0 ]
      [[ "$output" == *"PWNED_READ"* ]]
      ;;
    bash | zsh-sh | mksh | posh)
      [ "$status" -eq 0 ]
      [[ "$output" == *"PWNED_READ"* ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "safe-indirection validates POSIX identifier before read" {
  run_lab "$SCRIPT" safe-indirection "clean_var" "ok_value"
  [ "$status" -eq 0 ]
  [ "$output" = "clean_var:ok_value" ]

  # shellcheck disable=SC2016 # literal command substitution payload
  run_lab "$SCRIPT" safe-indirection 'a[$(printf "PWNED_%s\n" "READ" >&2)0]' "ok_value"
  [ "$status" -eq 2 ]
  [[ "$output" != *"PWNED_READ"* ]]
  [[ "$output" == *"error:invalid identifier"* ]]
}

@test "positional-parameter batching prevents filename command injection in sh -c wrappers" {
  hostile="$BATS_TEST_TMPDIR/item;printf PWNED_FILE.txt"
  : >"$hostile"

  # shellcheck disable=SC2016 # $f belongs to the inner shell under test
  run_lab_c 'for f do printf "file:%s\n" "${f##*/}"; done' sh "$hostile"
  [ "$status" -eq 0 ]
  [ "$output" = "file:item;printf PWNED_FILE.txt" ]
}
