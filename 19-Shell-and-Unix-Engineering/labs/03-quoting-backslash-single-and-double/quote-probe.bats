#!/usr/bin/env bats
# Tests for POSIX shell quoting: backslash, single quotes, double quotes,
# quote removal, and shell family divergences.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/quote-probe.sh"
  ARGV_DUMP="$BATS_TEST_TMPDIR/argv-dump"
  "${CC:-cc}" -Wall -Wextra -Werror -o "$ARGV_DUMP" "$BATS_TEST_DIRNAME/argv-dump.c"
}

@test "count-args preserves whitespace, empty arguments, and glob characters" {
  run_lab "$PROBE" count-args "first item" "" "*"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "count=3" ]
  [ "${lines[1]}" = "args: <first item> <> <*>" ]
}

@test "backslash escapes preserve literal backslash before non-special in double quotes" {
  run_lab "$PROBE" backslash-escapes
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "unquoted_star: <*>" ]
  [ "${lines[1]}" = 'dq_star: <\*>' ]
  [ "${lines[2]}" = "dq_dollar: <$>" ]
  [ "${lines[3]}" = 'dq_a: <\a>' ]
  [ "${lines[4]}" = 'dq_quote: <">' ]
  [ "${lines[5]}" = 'dq_n: <\n>' ]
}

@test "line continuation strips backslash and newline except in single quotes" {
  run_lab "$PROBE" line-continuation
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "unquoted: <firstsecond>" ]
  [ "${lines[1]}" = "double_quoted: <firstsecond>" ]
  [ "${lines[2]}" = "single_quoted: <first\\" ]
  [ "${lines[3]}" = "second>" ]
}

@test "empty parameter retention drops unquoted empty but preserves quoted empty" {
  run_lab "$PROBE" empty-retention
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "unquoted_count=0" ]
  [ "${lines[1]}" = "quoted_count=1" ]
}

@test "param-joining joins positional parameters with IFS or null" {
  run_lab "$PROBE" param-joining COLON a b c
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "star: <a:b:c>" ]
  [ "${lines[1]}" = "at_count=3" ]

  run_lab "$PROBE" param-joining NULL a b c
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "star: <abc>" ]
  [ "${lines[1]}" = "at_count=3" ]
}

@test "null IFS parameter joining with empty positional parameter diverges in ksh93u+m" {
  # shellcheck disable=SC2016 # expressions run under shell under test
  run_lab_c 'IFS=; set -- a "" b; printf "%s\n" "$*"'
  case $(lab_shell_family) in
    ksh93)
      case $(lab_shell_version) in
        *93u+m*1.0.10*)
          [ "$status" -eq 0 ]
          [ "$output" = 'a\b' ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "ab" ]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "ab" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "subst-nesting demonstrates clean recursion in dollar-substitution vs backticks" {
  run_lab "$PROBE" subst-nesting
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "dollar_subst: <nested inner>" ]
  [ "${lines[1]}" = 'dollar_escaped: <nested "inner">' ]
  [ "${lines[2]}" = "backtick_subst: <nested inner>" ]
  [ "${lines[3]}" = "backtick_depth3: <1 2 3>" ]
}

@test "pattern-quoting suppresses wildcards inside parameter expansion" {
  run_lab "$PROBE" pattern-quoting "abc*def" unquoted
  [ "$status" -eq 0 ]
  [ "$output" = "result: <*def>" ]

  run_lab "$PROBE" pattern-quoting "abc*def" quoted_middle
  [ "$status" -eq 0 ]
  [ "$output" = "result: <def>" ]

  run_lab "$PROBE" pattern-quoting "abc*def" quoted_literal
  [ "$status" -eq 0 ]
  [ "$output" = "result: <def>" ]
}

@test "empty at-parameter under set -u expands to zero arguments except in posh" {
  # shellcheck disable=SC2016 # expressions run under shell under test
  run_lab_c 'set -u; set --; printf "<%s>" "$@"; echo done'
  case $(lab_shell_family) in
    posh)
      [ "$status" -ne 0 ]
      case $output in
        *"parameter not set"*) ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      [ "$output" = "<>done" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "dollar double quote string translation differs between bash/ksh93/mksh and strict POSIX" {
  # shellcheck disable=SC2016 # expressions run under shell under test
  run_lab_c 'printf "%s\n" $"hello"'
  case $(lab_shell_family) in
    bash | ksh93 | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "hello" ]
      ;;
    dash | zsh | zsh-sh | posh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "\$hello" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "quotes inside arithmetic expansion differ between standard and extended parsers" {
  # shellcheck disable=SC2016 # expressions run under shell under test
  run_lab_c 'echo $(( "1 + 2" ))'
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*)
          [ "$status" -ne 0 ]
          [ "$output" != 3 ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = 3 ]
          ;;
      esac
      ;;
    zsh | zsh-sh | ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = 3 ]
      ;;
    dash | busybox | mksh | posh | yash)
      [ "$status" -ne 0 ]
      [ "$output" != 3 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "execve argument boundary and quote removal verified by C probe" {
  # shellcheck disable=SC2016 # testing argument passing into C executable
  run_lab_c '"$1" "hello world" '\''single'\'' "empty:" "" "has space"' _ "$ARGV_DUMP"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "argc=6" ]
  [ "${lines[1]}" = "argv[1]: len=11 data=<hello world>" ]
  [ "${lines[2]}" = "argv[2]: len=6 data=<single>" ]
  [ "${lines[3]}" = "argv[3]: len=6 data=<empty:>" ]
  [ "${lines[4]}" = "argv[4]: len=0 data=<>" ]
  [ "${lines[5]}" = "argv[5]: len=9 data=<has space>" ]
}
