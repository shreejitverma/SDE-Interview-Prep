#!/usr/bin/env bats
# Tests for the POSIX special parameters: @ * # ? - $ ! 0.
# Every expectation comes from probe.sh running under the shell named by LAB_SHELL.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe.sh"
  family=$(lab_shell_family)
}

# expect_lines LINE...: $output must equal the lines joined with newlines.
expect_lines() {
  local want
  want=$(printf '%s\n' "$@")
  [ "$output" = "$want" ]
}

@test "\"\$@\" keeps each parameter as one field and \"\$*\" joins them into one" {
  run_lab "$PROBE" at-star
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = '3: <a b> <> <c>' ]
  [ "${lines[1]}" = '1: <a b  c>' ]
}

@test "unquoted \$@ and \$* split and drop the empty parameter, except in native zsh" {
  run_lab "$PROBE" at-star
  [ "$status" -eq 0 ]
  case $family in
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "${lines[2]}" = '3: <a> <b> <c>' ]
      [ "${lines[3]}" = '3: <a> <b> <c>' ]
      ;;
    zsh)
      [ "${lines[2]}" = '2: <a b> <c>' ]
      [ "${lines[3]}" = '2: <a b> <c>' ]
      ;;
    *) false ;;
  esac
}

@test "\"\$*\" joins with the first IFS character, a space when IFS is unset, nothing when IFS is null" {
  run_lab "$PROBE" ifs-join
  [ "$status" -eq 0 ]
  expect_lines '1: <a:b:c>' '1: <abc>' '1: <a b c>' '1: <a:b:c>'
}

@test "with no parameters \"\$@\" is zero fields and \"\$*\" is one empty field" {
  run_lab "$PROBE" empty
  [ "$status" -eq 0 ]
  expect_lines '0:' '1: <>' '1: <>' '1: <x>'
}

@test "\"\$@\" inside a word glues the prefix to the first field and the suffix to the last" {
  run_lab "$PROBE" embed
  [ "$status" -eq 0 ]
  expect_lines '3: <prea> <b> <cpost>' '1: <preonlypost>'
}

@test "\$# counts parameters without \$0 and tracks shift and set" {
  run_lab "$PROBE" count
  [ "$status" -eq 0 ]
  expect_lines 3 '2 b' 0 '2 x y'
}

@test "\$10 is \$1 followed by a zero and \${10} is the tenth parameter, except in native zsh" {
  run_lab "$PROBE" tenth
  [ "$status" -eq 0 ]
  case $family in
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      expect_lines one0 ten
      ;;
    zsh) expect_lines ten ten ;;
    *) false ;;
  esac
}

@test "a for loop without in iterates over \"\$@\" and keeps the empty parameter" {
  run_lab "$PROBE" for-default
  [ "$status" -eq 0 ]
  expect_lines '1=<a b>' '2=<>' '3=<c>'
}

@test "\$? is the status of the most recent pipeline, taken from its last stage" {
  run_lab "$PROBE" status
  [ "$status" -eq 0 ]
  expect_lines 'init 0' 'after-false 1' 'negated 1' 'negated-false 0' 'pipe-last 1' 'pipe-last-ok 0' 'assign-subst 7' 'prefix-assign 0' 'subshell 5' 'preserved 1' '1 1'
}

@test "\$? after a command killed by SIGTERM encodes the signal in the shell's own way" {
  run_lab "$PROBE" status-signal
  [ "$status" -eq 0 ]
  case $family in
    bash | zsh | zsh-sh | busybox | posh | mksh | dash) [ "$output" = 143 ] ;;
    ksh93) [ "$output" = 271 ] ;;
    yash) [ "$output" = 399 ] ;;
    *) false ;;
  esac
}

@test "\$\$ is the invoking shell's pid in subshells and substitutions, and a child shell has its own" {
  run_lab "$PROBE" pid
  [ "$status" -eq 0 ]
  expect_lines same-in-substitution same-in-subshell child-differs live
}

@test "\$! is the pid of the last asynchronous command and survives foreground commands" {
  run_lab "$PROBE" bang
  [ "$status" -eq 0 ]
  expect_lines comm=sleep not-shell kept-after-foreground updated-by-next 'wait-first 0' 'wait-second 0'
}

@test "\$- reflects set -u and set -f, and has no i in a script" {
  run_lab "$PROBE" options
  [ "$status" -eq 0 ]
  expect_lines u-off u-on u-off f-on f-off not-interactive
}

@test "\$0 is the script name and a function call leaves it alone, except in native zsh" {
  run_lab "$PROBE" zero
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "top $PROBE" ]
  case $family in
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "${lines[1]}" = "in-function $PROBE" ]
      ;;
    zsh) [ "${lines[1]}" = 'in-function f' ] ;;
    *) false ;;
  esac
}

@test "\$0 comes from the first argument after the command string with sh -c" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c 'echo "$0 $1 $#"' name one two
  [ "$status" -eq 0 ]
  [ "$output" = 'name one 2' ]
}

@test "a function sees its own positional parameters and the caller's are restored on return" {
  run_lab "$PROBE" function-args
  [ "$status" -eq 0 ]
  expect_lines 'in 3 a' 'set-in 1 changed' 'out 2 outer1 outer2'
}

@test "under set -u an empty \"\$@\" and \"\$*\" are not errors, except in posh" {
  run_lab "$PROBE" unset-at
  case $family in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      expect_lines '0:' '1: <>'
      ;;
    posh)
      [ "$status" -ne 0 ]
      [[ $output != *'0:'* ]]
      [[ $output == *'@: parameter not set'* ]]
      ;;
    *) false ;;
  esac
}

@test "under set -u an unset numbered parameter ends the shell, except in ksh93u+ 2012" {
  run_lab "$PROBE" unset-one
  case $family in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      [ "$status" -ne 0 ]
      [ "$output" = before ]
      ;;
    ksh93)
      version=$(lab_shell_version)
      if [[ $version == *93u+m* ]]; then
        [ "$status" -ne 0 ]
        [ "$output" = before ]
      else
        [ "$status" -eq 0 ]
        expect_lines before '' not-reached
      fi
      ;;
    *) false ;;
  esac
}

@test "shift past \$# fails: some shells end the subshell, the others carry on with status 1" {
  run_lab "$PROBE" shift-too-many
  case $family in
    bash | zsh | zsh-sh | yash | busybox)
      expect_lines 'continued 1 2' 'subshell 0'
      ;;
    posh | mksh | ksh93)
      [ "$output" = 'subshell 1' ]
      ;;
    dash)
      [ "$output" = 'subshell 2' ]
      ;;
    *) false ;;
  esac
}

@test "an unknown probe mode is rejected" {
  run_lab "$PROBE" nonsense
  [ "$status" -eq 2 ]
}
