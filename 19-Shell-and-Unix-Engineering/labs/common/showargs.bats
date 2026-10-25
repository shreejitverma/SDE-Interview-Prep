#!/usr/bin/env bats
# Tests for showargs.sh, and a smoke test that the matrix really runs each shell under test.

setup() {
  load ../lib/lab
  SHOWARGS="$BATS_TEST_DIRNAME/showargs.sh"
}

@test "no arguments prints a zero count" {
  run_lab "$SHOWARGS"
  [ "$status" -eq 0 ]
  [ "$output" = "0 args:" ]
}

@test "quoted arguments keep their spaces and empty arguments survive" {
  run_lab "$SHOWARGS" a 'b c' ''
  [ "$status" -eq 0 ]
  [ "$output" = "3 args: <a> <b c> <>" ]
}

@test "glob characters in a quoted argument are not expanded" {
  run_lab "$SHOWARGS" '*'
  [ "$output" = "1 args: <*>" ]
}

@test "an unquoted expansion is field split, except in native zsh" {
  # shellcheck disable=SC2016 # the expansion belongs to the shell under test
  run_lab_c 'x="a  b"; set -- $x; echo "$#"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh) [ "$output" = 1 ] ;; # zsh does not split unquoted parameters unless SH_WORD_SPLIT is set
    *) [ "$output" = 2 ] ;;
  esac
}

@test "the shell under test runs and is the family the matrix expects" {
  run_lab_c 'echo ok'
  [ "$status" -eq 0 ]
  [ "$output" = ok ]
  if [ -n "${LAB_EXPECT_FAMILY-}" ]; then
    family=$(lab_shell_family)
    [ "$family" = "$LAB_EXPECT_FAMILY" ]
  fi
}
