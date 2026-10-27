#!/usr/bin/env bats
# Proves the difference between a null and an unset parameter: what the eight
# parameter-expansion operators do to each state, how to tell the states apart,
# what set -u and ${x:?} do about an unset one, and how the environment keeps them.

# shellcheck disable=SC2016,SC2030,SC2031 # probed expansions stay literal; bats runs each test in a subshell

setup() {
  bats_require_minimum_version 1.5.0 # run -N in the sh -c test
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe.sh"
}

# expect_output EXPECTED: fail unless $output equals EXPECTED exactly.
expect_output() {
  if [ "$output" != "$1" ]; then
    printf 'expected:\n%s\nactual:\n%s\n' "$1" "$output" >&2
    return 1
  fi
}

@test "the eight operators follow the POSIX table for unset, null and set" {
  run_lab "$PROBE" table
  [ "$status" -eq 0 ]
  expect_output 'unset ${x:-W} -> result=<W> x=<unset>
unset ${x-W} -> result=<W> x=<unset>
unset ${x:=W} -> result=<W> x=<W>
unset ${x=W} -> result=<W> x=<W>
unset ${x:+W} -> result=<> x=<unset>
unset ${x+W} -> result=<> x=<unset>
unset ${x:?W} -> error
unset ${x?W} -> error
null ${x:-W} -> result=<W> x=<>
null ${x-W} -> result=<> x=<>
null ${x:=W} -> result=<W> x=<W>
null ${x=W} -> result=<> x=<>
null ${x:+W} -> result=<> x=<>
null ${x+W} -> result=<W> x=<>
null ${x:?W} -> error
null ${x?W} -> result=<>
set ${x:-W} -> result=<v> x=<v>
set ${x-W} -> result=<v> x=<v>
set ${x:=W} -> result=<v> x=<v>
set ${x=W} -> result=<v> x=<v>
set ${x:+W} -> result=<W> x=<v>
set ${x+W} -> result=<W> x=<v>
set ${x:?W} -> result=<v>
set ${x?W} -> result=<v>'
}

@test "a plain -z test cannot tell null from unset, but a plus expansion can" {
  run_lab "$PROBE" classify
  [ "$status" -eq 0 ]
  expect_output 'unset unset
x= null
x=v set
z-on-unset true
z-on-null true'
}

@test "export and readonly without a value leave a variable unset, except in native zsh" {
  run_lab "$PROBE" declared
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      expect_output 'export-only null
readonly-only null
assign-then-unset unset
unset-of-unset status 0'
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      expect_output 'export-only unset
readonly-only unset
assign-then-unset unset
unset-of-unset status 0'
      ;;
    *) false ;;
  esac
}

# has_line LINE: succeed when $output contains LINE as a whole line.
has_line() {
  case "
$output
" in
    *"
$1
"*) return 0 ;;
    *) return 1 ;;
  esac
}

# line_verdict EXPR: print ok or fail, the verdict probe.sh gave for EXPR; fail the test otherwise.
line_verdict() {
  if has_line "$1 ok"; then
    echo ok
  elif has_line "$1 fail"; then
    echo fail
  else
    printf 'no verdict for %s\n%s\n' "$1" "$output" >&2
    return 1
  fi
}

@test "set -u fails on an unset name but not on a null one, a default, or at-star" {
  run_lab "$PROBE" nounset
  [ "$status" -eq 0 ]
  local line
  for line in '${u} fail' '${#u} fail' '${u-} ok' '${u:-d} ok' '${u+a} ok' '${u:+a} ok' \
    '${x} ok' '${x:-d} ok' '$(( x )) ok' '${1-} ok' '${1+"$@"} ok' '$# ok' '$? ok'; do
    if ! has_line "$line"; then
      printf 'missing line: %s\n%s\n' "$line" "$output" >&2
      return 1
    fi
  done
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 aborts on an empty "$@" and "$*" under set -u
      has_line '"$@" fail'
      has_line '"$*" fail'
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      has_line '"$@" ok'
      has_line '"$*" ok'
      ;;
    *) false ;;
  esac
}

@test "set -u on arithmetic and pattern removal differs between shells" {
  run_lab "$PROBE" nounset
  [ "$status" -eq 0 ]
  local arith pattern positional
  arith=$(line_verdict '$((u))')
  pattern=$(line_verdict '${u%x}')
  positional=$(line_verdict '$1')
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*) [ "$arith $pattern" = "ok ok" ] ;;
        *) [ "$arith $pattern" = "fail fail" ] ;;
      esac
      [ "$positional" = fail ]
      ;;
    zsh | zsh-sh | yash | posh)
      [ "$arith $pattern $positional" = "fail fail fail" ]
      ;;
    dash | busybox | mksh)
      [ "$arith $pattern $positional" = "ok fail fail" ]
      ;;
    ksh93)
      [ "$arith $pattern" = "fail fail" ]
      # ksh93u+ 2012 does not treat an unset $1 as an error; ksh93u+m does
      case $(lab_shell_version) in
        *93u+m*) [ "$positional" = fail ] ;;
        *) [ "$positional" = ok ] ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "the word of a default or alternative is expanded only when it is used" {
  run_lab "$PROBE" lazy
  [ "$status" -eq 0 ]
  expect_output 'set:-  n=0
set:+  n=10
unset- n=110
unset+ n=110'
}

@test "the colon-question form exits a non-interactive shell for null or unset" {
  local case_name expected
  case $(lab_shell_family) in
    dash | busybox | yash) expected=2 ;;
    bash | zsh | zsh-sh | mksh | posh | ksh93) expected=1 ;;
    *) false ;;
  esac
  for case_name in colon-unset colon-null plain-unset; do
    run_lab "$PROBE" error "$case_name"
    if [ "$status" -ne "$expected" ]; then
      printf '%s: status %s, expected %s\n%s\n' "$case_name" "$status" "$expected" "$output" >&2
      return 1
    fi
    case $output in
      *x:*missing*) ;;
      *)
        printf '%s: no diagnostic naming x\n%s\n' "$case_name" "$output" >&2
        return 1
        ;;
    esac
    case $output in
      *reached*) return 1 ;;
      *) ;;
    esac
  done
}

@test "under sh -c, bash turns a failed colon-question expansion into status 127" {
  local expected
  case $(lab_shell_family) in
    bash) expected=127 ;;
    dash | busybox | yash) expected=2 ;;
    zsh | zsh-sh | mksh | posh | ksh93) expected=1 ;;
    *) false ;;
  esac
  lab_shell_words
  # run -N fails the test unless the status is exactly N (and declares 127 as expected)
  run "-$expected" "${LAB_SHELL_WORDS[@]}" -c 'unset x; : "${x:?missing}"; echo reached'
  case $output in
    *reached*) return 1 ;;
    *x:*missing*) ;;
    *) return 1 ;;
  esac
}

@test "the plain question form accepts a null variable and the shell goes on" {
  run_lab "$PROBE" error plain-null
  [ "$status" -eq 0 ]
  expect_output 'reached'
}

@test "the error of the question form ends only the subshell it runs in" {
  run_lab "$PROBE" error subshell
  [ "$status" -eq 0 ]
  expect_output 'parent continued
reached'
}

@test "assigning with colon-equals to a positional parameter fails, except in zsh" {
  run_lab_c 'set --; ( : "${1:=w}"; echo "assigned <$1>" ) 2>/dev/null'
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$status" -eq 0 ]
      expect_output 'assigned <w>'
      ;;
    bash | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$status" -ne 0 ]
      expect_output ''
      ;;
    *) false ;;
  esac
}

@test "null IFS joins and stops splitting, unset IFS means space, tab and newline" {
  run_lab "$PROBE" ifs
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # native zsh never splits an unquoted parameter expansion
      expect_output 'null "$*" <abc>
null split 1
unset "$*" <a b c>
unset split 1'
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      expect_output 'null "$*" <abc>
null split 1
unset "$*" <a b c>
unset split 2'
      ;;
    *) false ;;
  esac
}

@test "a child process sees an exported null variable as empty and an unset one as absent" {
  "${CC:-cc}" -Wall -Werror -o "$BATS_TEST_TMPDIR/getenv" "$BATS_TEST_DIRNAME/getenv.c"
  export getenv="$BATS_TEST_TMPDIR/getenv"
  run_lab "$PROBE" environ
  [ "$status" -eq 0 ]
  expect_output 'unset-var child: NULL
unexported-null child: NULL
exported-null child: ""
exported-null-in-env: 1
after-unset child: NULL
prefix-assignment child: ""'
}

@test "a shell started with an empty variable in its environment holds it as null" {
  lab_shell_words
  # shellcheck disable=SC2016 # the expansion belongs to the shell under test
  local code='if [ "${x+s}" != s ]; then echo unset; elif [ -z "$x" ]; then echo null; else echo set; fi'
  run env x= "${LAB_SHELL_WORDS[@]}" -c "$code"
  [ "$status" -eq 0 ]
  expect_output 'null'
  unset x
  run_lab_c "$code"
  [ "$status" -eq 0 ]
  expect_output 'unset'
}
