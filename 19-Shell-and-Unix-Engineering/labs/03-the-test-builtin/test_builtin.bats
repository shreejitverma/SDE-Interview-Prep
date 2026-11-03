#!/usr/bin/env bats
# Tests for the test and [ builtins across shell matrix families.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/test_probe.sh"
}

# assert_lines LINE...: the last `run` succeeded and printed exactly these lines.
assert_lines() {
  local i=0 want
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq "$#" ]
  for want in "$@"; do
    if [ "${lines[i]}" != "$want" ]; then
      printf 'line %d: want [%s], got [%s]\n' "$i" "$want" "${lines[i]}" >&2
      return 1
    fi
    i=$((i + 1))
  done
}

# is_ksh93_2012: true when the shell under test is the AT&T ksh93u+ 2012 build (not ksh93u+m).
is_ksh93_2012() {
  local family
  family=$(lab_shell_family)
  [ "$family" = ksh93 ] || return 1
  case $(lab_shell_version) in
    *93u+m*) return 1 ;;
    *) return 0 ;;
  esac
}

@test "probe arg-count executes 0 through 4 argument POSIX rules" {
  run_lab "$PROBE" arg-count
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "0-arg=1" ]
  [ "${lines[1]}" = "1-arg-nonempty=0 1-arg-empty=1" ]
  [ "${lines[2]}" = "2-arg-neg=0 2-arg-unary=0" ]
  [ "${lines[3]}" = "3-arg-paren=0 3-arg-excl=0" ]
  [ "${lines[4]}" = "4-arg-neg=1" ]
}

@test "probe unquoted-pitfall demonstrates unquoted -n failure" {
  run_lab "$PROBE" unquoted-pitfall
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "unquoted: bug: empty evaluated true" ]
  [ "${lines[1]}" = "quoted: safe: empty evaluated false" ]
}

@test "probe file-ops verifies -ef -nt and -ot primaries" {
  run_lab "$PROBE" file-ops "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "ef=different nt=not_newer ot=not_older" ]
      ;;
    bash | dash | busybox | mksh | yash | zsh | zsh-sh | ksh93)
      [ "$output" = "ef=same_inode nt=newer ot=older" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "0 operands to test and [ exit 1 (false)" {
  run_lab_c 'test; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]

  run_lab_c '[ ]; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]
}

@test "1 operand evaluates string non-emptiness and ignores options" {
  run_lab_c 'test "hello"; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test ""; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]

  run_lab_c 'test --help; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]
}

@test "2 operands evaluate unary primaries and negation" {
  run_lab_c 'test ! ""; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test ! "nonempty"; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]

  run_lab_c 'test -c /dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test -f /dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]
}

@test "3 operands prioritize binary operator over operator-like operands" {
  run_lab_c 'test "(" = "("; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test "!" = "!"; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test -f = -f; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]

  run_lab_c 'test ! -f /dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=0" ]
}

@test "4 operands negate 3-operand evaluation when first operand is !" {
  run_lab_c 'test ! "(" = "("; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]

  run_lab_c 'test ! "!" = "!"; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=1" ]
}

@test "missing closing bracket on [ produces error exit status 2" {
  run_lab_c '[ -f /dev/null 2>/dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=2" ]
}

@test "unquoted empty variable before binary operator exits 2" {
  # shellcheck disable=SC2016 # the expansion belongs to the shell under test
  run_lab_c 'v=""; [ $v = "target" ] 2>/dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "status=2" ]
}

@test "POSIX.1-2024 hard link comparison with -ef" {
  local f1="$BATS_TEST_TMPDIR/hard1"
  local f2="$BATS_TEST_TMPDIR/hard2"
  local f3="$BATS_TEST_TMPDIR/other"
  touch "$f1" "$f3"
  ln "$f1" "$f2"

  case $(lab_shell_family) in
    posh)
      run_lab_c "[ '$f1' -ef '$f2' ] 2>/dev/null; echo \"status=\$?\""
      [ "$status" -eq 0 ]
      [ "$output" = "status=2" ]

      run_lab_c "[ '$f1' -ef '$f3' ] 2>/dev/null; echo \"status=\$?\""
      [ "$status" -eq 0 ]
      [ "$output" = "status=2" ]
      ;;
    bash | dash | busybox | mksh | yash | zsh | zsh-sh | ksh93)
      run_lab_c "[ '$f1' -ef '$f2' ]; echo \"status=\$?\""
      [ "$status" -eq 0 ]
      [ "$output" = "status=0" ]

      run_lab_c "[ '$f1' -ef '$f3' ]; echo \"status=\$?\""
      [ "$status" -eq 0 ]
      [ "$output" = "status=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "string comparison with > is ordered and differs across shell families" {
  run_lab_c '[ b \> a ] 2>/dev/null; echo "gt=$?"; [ a \> b ] 2>/dev/null; echo "gt-reversed=$?"; [ a \> a ] 2>/dev/null; echo "gt-equal=$?"'
  case $(lab_shell_family) in
    zsh | zsh-sh | posh)
      assert_lines "gt=2" "gt-reversed=2" "gt-equal=2"
      ;;
    bash | dash | busybox | mksh | yash | ksh93)
      assert_lines "gt=0" "gt-reversed=1" "gt-equal=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "string comparison with < is ordered and differs across shell families" {
  run_lab_c '[ a \< b ] 2>/dev/null; echo "lt=$?"; [ b \< a ] 2>/dev/null; echo "lt-reversed=$?"; [ a \< a ] 2>/dev/null; echo "lt-equal=$?"'
  case $(lab_shell_family) in
    zsh | zsh-sh | posh)
      assert_lines "lt=2" "lt-reversed=2" "lt-equal=2"
      ;;
    ksh93)
      if is_ksh93_2012; then
        assert_lines "lt=2" "lt-reversed=2" "lt-equal=2"
      else
        assert_lines "lt=0" "lt-reversed=1" "lt-equal=1"
      fi
      ;;
    bash | dash | busybox | mksh | yash)
      assert_lines "lt=0" "lt-reversed=1" "lt-equal=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "integer comparison treats numbers as decimal not octal" {
  run_lab_c '[ 010 -eq 10 ]; echo "eq10=$?"; [ 010 -eq 8 ]; echo "eq8=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "${lines[0]}" = "eq10=1" ]
      [ "${lines[1]}" = "eq8=0" ]
      ;;
    bash | dash | busybox | mksh | yash | zsh | zsh-sh | ksh93)
      [ "${lines[0]}" = "eq10=0" ]
      [ "${lines[1]}" = "eq8=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "integer operands: 2^63 - 1 compares exactly except in ksh93u+ 2012" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c 'M=9223372036854775807; N=9223372036854775806
[ $M -eq $M ]; echo "self=$?"; [ $M -eq $N ]; echo "eq-previous=$?"; [ $M -gt $N ]; echo "gt-previous=$?"; [ $N -lt $M ]; echo "lt-max=$?"'
  if is_ksh93_2012; then
    # The 2012 build evaluates in double precision, so 2^63 - 1 and 2^63 - 2 round to the same value.
    assert_lines "self=0" "eq-previous=0" "gt-previous=1" "lt-max=1"
  else
    assert_lines "self=0" "eq-previous=1" "gt-previous=0" "lt-max=0"
  fi
}

@test "integer operands: 2^63 overflow is an error, a wrap, or a float depending on the shell" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c 'O=9223372036854775808
[ $O -eq $O ] 2>/dev/null; echo "self=$?"
[ $O -eq 9223372036854775807 ] 2>/dev/null; echo "eq-max=$?"
[ $O -eq 0 ] 2>/dev/null; echo "eq-zero=$?"
[ $O -gt 0 ] 2>/dev/null; echo "gt-zero=$?"'
  case $(lab_shell_family) in
    bash | dash | busybox | yash)
      assert_lines "self=2" "eq-max=2" "eq-zero=2" "gt-zero=2"
      ;;
    zsh | zsh-sh)
      assert_lines "self=0" "eq-max=1" "eq-zero=1" "gt-zero=0"
      ;;
    ksh93)
      if is_ksh93_2012; then
        assert_lines "self=0" "eq-max=0" "eq-zero=1" "gt-zero=0"
      else
        assert_lines "self=0" "eq-max=1" "eq-zero=1" "gt-zero=0"
      fi
      ;;
    mksh)
      assert_lines "self=0" "eq-max=1" "eq-zero=0" "gt-zero=1"
      ;;
    posh)
      assert_lines "self=0" "eq-max=1" "eq-zero=1" "gt-zero=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "integer operands: 2^32 wraps only in mksh and 2^64 wraps only in mksh and posh" {
  run_lab_c '[ 4294967296 -eq 0 ] 2>/dev/null; echo "2^32-eq-zero=$?"; [ 4294967296 -gt 1 ] 2>/dev/null; echo "2^32-gt-one=$?"; [ 18446744073709551616 -eq 0 ] 2>/dev/null; echo "2^64-eq-zero=$?"'
  case $(lab_shell_family) in
    mksh)
      assert_lines "2^32-eq-zero=0" "2^32-gt-one=1" "2^64-eq-zero=0"
      ;;
    posh)
      assert_lines "2^32-eq-zero=1" "2^32-gt-one=0" "2^64-eq-zero=0"
      ;;
    bash | dash | busybox | yash)
      assert_lines "2^32-eq-zero=1" "2^32-gt-one=0" "2^64-eq-zero=2"
      ;;
    zsh | zsh-sh | ksh93)
      assert_lines "2^32-eq-zero=1" "2^32-gt-one=0" "2^64-eq-zero=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "obsolescent -o operator in 5 arguments diverges across shell families" {
  run_lab_c 'test -d "=" -o -d "foo" 2>/dev/null; echo "status=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93 | yash | posh)
      [ "$output" = "status=1" ]
      ;;
    bash | dash | busybox | mksh | zsh | zsh-sh)
      [ "$output" = "status=2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "-nt -ot and -ef answer false for the opposite direction and for identical files" {
  local old="$BATS_TEST_TMPDIR/old" new="$BATS_TEST_TMPDIR/new"
  touch -t 202001010000 "$old"
  touch -t 202101010000 "$new"
  run_lab_c "[ '$new' -nt '$old' ] 2>/dev/null; echo \"nt=\$?\"
[ '$old' -nt '$new' ] 2>/dev/null; echo \"nt-reversed=\$?\"
[ '$old' -ot '$new' ] 2>/dev/null; echo \"ot=\$?\"
[ '$new' -ot '$old' ] 2>/dev/null; echo \"ot-reversed=\$?\"
[ '$old' -nt '$old' ] 2>/dev/null; echo \"nt-same=\$?\"
[ '$old' -ef '$old' ] 2>/dev/null; echo \"ef-same=\$?\"
[ '$old' -ef '$new' ] 2>/dev/null; echo \"ef-different=\$?\""
  case $(lab_shell_family) in
    posh)
      assert_lines "nt=2" "nt-reversed=2" "ot=2" "ot-reversed=2" "nt-same=2" "ef-same=2" "ef-different=2"
      ;;
    bash | dash | busybox | mksh | yash | zsh | zsh-sh | ksh93)
      assert_lines "nt=0" "nt-reversed=1" "ot=0" "ot-reversed=1" "nt-same=1" "ef-same=0" "ef-different=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "an operator in operand position: [ = foo ] is an error, a lone ] or -n or ! is a 1-operand string test" {
  run_lab_c '[ = foo ] 2>/dev/null; echo "eq-foo=$?"; test ]; echo "rbracket=$?"; [ -n ]; echo "dash-n=$?"; [ ! ]; echo "bang=$?"'
  assert_lines "eq-foo=2" "rbracket=0" "dash-n=0" "bang=0"
}

@test "the -- delimiter is not special: test -- x has 2 operands and is an error" {
  run_lab_c 'test -- x 2>/dev/null; echo "status=$?"'
  assert_lines "status=2"
}

@test "an empty operand to -eq is an error in Bash, Dash, BusyBox, Yash and posh, and 0 elsewhere" {
  run_lab_c '[ "" -eq 0 ] 2>/dev/null; echo "eq-zero=$?"; [ "" -eq 1 ] 2>/dev/null; echo "eq-one=$?"; [ "" -ne 0 ] 2>/dev/null; echo "ne-zero=$?"'
  case $(lab_shell_family) in
    bash | dash | busybox | yash | posh)
      assert_lines "eq-zero=2" "eq-one=2" "ne-zero=2"
      ;;
    zsh | zsh-sh | ksh93 | mksh)
      assert_lines "eq-zero=0" "eq-one=1" "ne-zero=1"
      ;;
    *)
      false
      ;;
  esac
}

@test "the -v extension: supported from Bash 4.2, in Zsh, ksh93 and mksh, and an error elsewhere" {
  run_lab_c 'V=1; export V; unset NOPE; [ -v V ] 2>/dev/null; echo "set=$?"; [ -v NOPE ] 2>/dev/null; echo "unset=$?"'
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.* | 4.0* | 4.1*)
          assert_lines "set=2" "unset=2"
          ;;
        *)
          assert_lines "set=0" "unset=1"
          ;;
      esac
      ;;
    zsh | zsh-sh | ksh93 | mksh)
      assert_lines "set=0" "unset=1"
      ;;
    dash | busybox | yash | posh)
      assert_lines "set=2" "unset=2"
      ;;
    *)
      false
      ;;
  esac
}

@test "operator injection: an unquoted value splits into operands, except in native zsh" {
  # shellcheck disable=SC2016 # the expansions belong to the shell under test
  run_lab_c 'v="-f"; [ $v = target ] 2>/dev/null; echo "single-f=$?"
v="-f x"; [ $v = target ] 2>/dev/null; echo "split-unquoted=$?"; [ "$v" = target ] 2>/dev/null; echo "quoted=$?"
input="yes -o 1 -eq 1 -o x"; [ $input = yes ] 2>/dev/null; echo "inject-unquoted=$?"; [ "$input" = yes ] 2>/dev/null; echo "inject-quoted=$?"'
  case $(lab_shell_family) in
    zsh)
      # native zsh does not split an unquoted expansion, so the value stays one operand
      assert_lines "single-f=1" "split-unquoted=1" "quoted=1" "inject-unquoted=1" "inject-quoted=1"
      ;;
    ksh93)
      assert_lines "single-f=1" "split-unquoted=1" "quoted=1" "inject-unquoted=0" "inject-quoted=1"
      ;;
    bash | dash | busybox | mksh | yash | posh | zsh-sh)
      assert_lines "single-f=1" "split-unquoted=2" "quoted=1" "inject-unquoted=0" "inject-quoted=1"
      ;;
    *)
      false
      ;;
  esac
}
