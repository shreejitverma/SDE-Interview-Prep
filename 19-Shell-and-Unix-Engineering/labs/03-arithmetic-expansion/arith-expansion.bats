#!/usr/bin/env bats
# Tests for POSIX arithmetic expansion: what the specification fixes, and where real shells diverge.
# shellcheck disable=SC2016,SC2030,SC2031 # code strings belong to the shell under test; HIT is exported per test on purpose

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/arith-safe.sh"
  FAMILY=$(lab_shell_family)
  VERSION=$(lab_shell_version)
}

# arith_value EXPR: run `echo $(( EXPR ))` in the shell under test with its diagnostics discarded.
arith_value() {
  run_lab_c "exec 2>/dev/null; echo \$(( $1 ))"
}

# expect_value VALUE: the last arith_value succeeded and printed exactly VALUE.
expect_value() {
  [ "$status" -eq 0 ]
  [ "$output" = "$1" ]
}

# expect_error: the last arith_value failed and printed nothing on standard output.
expect_error() {
  [ "$status" -ne 0 ]
  [ -z "$output" ]
}

# run_chain LINKS: v0=v1, ..., v(LINKS-1)=vLINKS, vLINKS=5, then print $((v0)).
# One link is one hop. zsh 5.9 reaches 5 at 254 links and fails at 255.
run_chain() {
  # shellcheck disable=SC2016 # $i, $j and $((v0)) belong to the shell under test
  run_lab_c 'i=0; while [ "$i" -lt '"$1"' ]; do j=$((i + 1)); eval "v$i=v$j"; i=$j; done; eval "v$i=5"; exec 2>/dev/null; echo "$((v0))"'
}

# --- what POSIX.1-2024 XCU 2.6.4 requires: every shell must agree ---

@test "the POSIX counting loop runs 100 times and ends at zero" {
  run_lab_c 'x=100; n=0; while [ $x -gt 0 ]; do n=$((n + 1)); x=$(($x-1)); done; echo "$n $x"'
  [ "$status" -eq 0 ]
  [ "$output" = "100 0" ]
}

@test "arithmetic and logical operators follow C precedence and associativity" {
  # The last value is 1 only when && binds tighter than ||. Reversed precedence would yield 0.
  run_lab_c 'echo $((2 + 3 * 4)) $(((2 + 3) * 4)) $((10 - 4 - 3)) $((7 & 3 | 8)) $((1 ? 2 : 3)) $((0 || 5 && 6)) $((0 && 1 || 2))'
  [ "$status" -eq 0 ]
  [ "$output" = "14 20 3 11 2 1 1" ]
}

@test "native zsh ranks shifts and bitwise AND above + and *, unlike C" {
  run_lab_c 'echo $((1 << 2 + 1)) $((2 + 6 & 3))'
  [ "$status" -eq 0 ]
  case $FAMILY in
    zsh) [ "$output" = "5 4" ] ;;
    bash | dash | busybox | posh | zsh-sh | mksh | ksh93 | yash) [ "$output" = "8 0" ] ;;
    *) false ;;
  esac
}

@test "division truncates toward zero and the remainder takes the sign of the dividend" {
  run_lab_c 'x=-7; y=7; z=-2; echo $((x / 2)) $((x % 2)) $((y % z))'
  [ "$status" -eq 0 ]
  [ "$output" = "-3 -1 1" ]
}

@test "dividing by a negative number truncates toward zero too, except in ksh93u+ 2012" {
  run_lab_c 'exec 2>/dev/null; y=7; z=-2; echo $((y / z))'
  case $FAMILY in
    ksh93)
      case $VERSION in
        *93u+m*) expect_value -3 ;;
        *) expect_error ;;
      esac
      ;;
    bash | dash | busybox | posh | zsh | zsh-sh | mksh | yash) expect_value -3 ;;
    *) false ;;
  esac
}

@test "comparisons yield 1 or 0, which is how an expression becomes a test" {
  run_lab_c '[ "$((3 < 5))" -eq 1 ] && [ "$((5 < 3))" -eq 0 ] && echo ok'
  [ "$status" -eq 0 ]
  [ "$output" = ok ]
}

@test "an arithmetic expansion is a word, so its value never becomes an exit status" {
  # 5, not 0: a status equal to the value would print "5 5". The assignment status stays 0.
  run_lab_c 'x=$((5)); echo "$x $?"'
  [ "$status" -eq 0 ]
  [ "$output" = "5 0" ]
}

@test "x and \$x name the same value when x holds a signed decimal constant" {
  run_lab_c 'for x in 0 7 -7 123456; do [ "$((x))" = "$(($x))" ] || exit 1; [ "$((x))" = "$x" ] || exit 2; done; echo same'
  [ "$status" -eq 0 ]
  [ "$output" = same ]
}

@test "parameter expansion and command substitution inside the expression run first" {
  run_lab_c 'x=1; echo $(( $(echo 3) + $x )) $(( $((1 + 1)) * 3 ))'
  [ "$status" -eq 0 ]
  [ "$output" = "4 6" ]
}

@test "assignments are visible afterwards, except when the expansion ran in a command substitution" {
  run_lab_c ': $((x = 7)); a=$x; r=$(echo $((y = 9))); echo "$a ${y-unset} $r"'
  [ "$status" -eq 0 ]
  [ "$output" = "7 unset 9" ]
}

@test "set -u turns an unset parameter expanded inside the expression into an expansion error" {
  run_lab_c '(set -u; echo $(($undef + 1)); echo survived) 2>/dev/null; echo "failed=$(($? != 0))"'
  [ "$status" -eq 0 ]
  [ "$output" = "failed=1" ]
}

@test "division by zero is an expansion error that stops a non-interactive shell" {
  run_lab_c 'echo before; echo $((1 / 0)); echo after' 2>&1
  div_status=$status
  div_output=$output
  [[ $div_output == *before* ]]
  [[ $div_output != *after* ]]
  case $FAMILY in
    bash)
      # POSIX mode (macOS /bin/sh, bash --posix) exits 127. Other bash exits 1.
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$div_status" -eq 127 ]
      else
        [ "$div_status" -eq 1 ]
      fi
      ;;
    dash | busybox | yash) [ "$div_status" -eq 2 ] ;;
    zsh | zsh-sh | mksh | ksh93 | posh) [ "$div_status" -eq 1 ] ;;
    *) false ;;
  esac
}

@test "the same error inside a subshell ends only the subshell" {
  run_lab_c '(echo $((1 / 0))) 2>/dev/null; s=$?; echo "after $((s != 0))"'
  [ "$status" -eq 0 ]
  [ "$output" = "after 1" ]
}

@test "text substituted before evaluation parses as an expression, whatever the variable meant" {
  run_lab_c 'v="1+2"; echo $(($v * 2))'
  [ "$status" -eq 0 ]
  [ "$output" = 5 ]
}

@test "a leading-zero variable is octal, as the rationale requires, in most shells" {
  run_lab_c 'x=010; echo $((x + 1))'
  [ "$status" -eq 0 ]
  case $FAMILY in
    bash | dash | busybox | posh | zsh-sh | yash) [ "$output" = 9 ] ;;
    zsh | ksh93 | mksh) [ "$output" = 11 ] ;;
    *) false ;;
  esac
}

@test "a leading-zero variable with an 8 or 9 is an error where zeros mean octal" {
  run_lab_c 'exec 2>/dev/null; x=08; echo $((x))'
  case $FAMILY in
    bash | dash | busybox | posh | zsh-sh)
      [ "$status" -ne 0 ]
      [ -z "$output" ]
      ;;
    zsh | ksh93 | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = 8 ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = 08 ]
      ;;
    *) false ;;
  esac
}

# --- where the specification leaves room, and the shells use it ---

@test "the result of an unquoted expansion is field split, except in zsh and posh" {
  run_lab_c 'IFS=2; set -- $((123)); echo "$#"'
  [ "$status" -eq 0 ]
  case $FAMILY in
    bash | dash | busybox | mksh | ksh93 | yash) [ "$output" = 2 ] ;;
    zsh | zsh-sh | posh) [ "$output" = 1 ] ;;
    *) false ;;
  esac
}

@test "the result of a quoted expansion is never field split" {
  run_lab_c 'IFS=2; set -- "$((123))"; echo "$# $1"'
  [ "$status" -eq 0 ]
  [ "$output" = "1 123" ]
}

@test "integer width: 2147483647 + 1 shows whether integers are 32-bit or at least 64-bit" {
  arith_value '2147483647 + 1'
  case $FAMILY in
    mksh) expect_value -2147483648 ;;
    bash | dash | busybox | posh | zsh | zsh-sh | ksh93 | yash) expect_value 2147483648 ;;
    *) false ;;
  esac
}

@test "signed overflow is undefined in ISO C, so each shell picks its own answer" {
  arith_value '9223372036854775807 + 1'
  case $FAMILY in
    bash | dash | busybox | posh | zsh | zsh-sh) expect_value -9223372036854775808 ;;
    mksh) expect_value 0 ;;
    yash) expect_error ;;
    ksh93)
      case $VERSION in
        *93u+m*) expect_value 9223372036854775808 ;;
        *) expect_value 9223372036854775807 ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "a variable that refers to itself ends in an error instead of looping" {
  run_lab_c 'exec 2>/dev/null; a=a; echo $((a))'
  case $FAMILY in
    bash | dash | busybox | posh | zsh | zsh-sh | mksh | ksh93)
      [ "$status" -ne 0 ]
      [ -z "$output" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = a ]
      ;;
    *) false ;;
  esac
}

@test "recursive evaluation of variable values is bounded: a 1000-link chain" {
  run_chain 1000
  case $FAMILY in
    bash | busybox | posh | mksh) expect_value 5 ;;
    zsh | zsh-sh | dash) expect_error ;;
    ksh93)
      case $VERSION in
        *93u+m*) expect_value 5 ;;
        *) expect_error ;;
      esac
      ;;
    yash) expect_value v1 ;;
    *) false ;;
  esac
}

@test "a 1100-link chain trips bash and ksh93u+m, which cap the depth at 1024" {
  run_chain 1100
  case $FAMILY in
    bash | zsh | zsh-sh | dash | ksh93) expect_error ;;
    busybox | posh | mksh) expect_value 5 ;;
    yash) expect_value v1 ;;
    *) false ;;
  esac
}

@test "zsh fails at 255 links and ksh93u+ 2012 fails at 9" {
  case $FAMILY in
    zsh | zsh-sh)
      run_chain 254
      expect_value 5
      run_chain 255
      expect_error
      ;;
    ksh93)
      case $VERSION in
        *93u+m*)
          run_chain 9
          expect_value 5
          ;;
        *)
          run_chain 8
          expect_value 5
          run_chain 9
          expect_error
          ;;
      esac
      ;;
    bash | busybox | posh | mksh)
      run_chain 255
      expect_value 5
      ;;
    dash)
      run_chain 1
      expect_error
      ;;
    yash)
      run_chain 255
      expect_value v1
      ;;
    *) false ;;
  esac
}

@test "a bare variable name that is unset is an error under set -u in most shells" {
  run_lab_c '(set -u; echo $((undef + 1))) 2>/dev/null; echo "failed=$(($? != 0))"'
  [ "$status" -eq 0 ]
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | posh | yash) [ "$output" = "failed=1" ] ;;
    dash | mksh | busybox) [ "$output" = "$(printf '1\nfailed=0')" ] ;;
    *) false ;;
  esac
}

@test "a variable holding an expression is evaluated recursively only in some shells" {
  run_lab_c 'exec 2>/dev/null; v="1+2"; echo $((v * 2))'
  case $FAMILY in
    bash | busybox | posh | mksh | ksh93 | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = 6 ]
      ;;
    dash | yash)
      [ "$status" -ne 0 ]
      [ -z "$output" ]
      ;;
    *) false ;;
  esac
}

@test "double-star exponentiation is an extension" {
  arith_value '2 ** 10'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | busybox) expect_value 1024 ;;
    dash | posh | mksh | yash) expect_error ;;
    *) false ;;
  esac
}

@test "base#number constants are an extension" {
  arith_value '2#101'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | busybox | mksh | posh) expect_value 5 ;;
    dash | yash) expect_error ;;
    *) false ;;
  esac
}

@test "binary 0b constants: only zsh and a dash built on a C23 strtoimax accept them" {
  probe="$BATS_TEST_TMPDIR/strtoimax-probe"
  "${CC:-cc}" -Wall -Wextra -Werror -D_GNU_SOURCE -o "$probe" "$BATS_TEST_DIRNAME/strtoimax-probe.c"
  libc=$("$probe" 0b11)
  arith_value '0b11'
  case $FAMILY in
    zsh | zsh-sh) expect_value 3 ;;
    bash | ksh93 | mksh | busybox | posh | yash) expect_error ;;
    dash)
      # dash hands every constant to strtoimax(..., 0), so the C library decides
      if [ "$libc" = reject ]; then expect_error; else expect_value "$libc"; fi
      ;;
    *) false ;;
  esac
}

@test "the comma operator is optional in POSIX and missing in dash and yash" {
  arith_value '1, 2'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | busybox | mksh | posh) expect_value 2 ;;
    dash | yash) expect_error ;;
    *) false ;;
  esac
}

@test "an empty expression is zero in most shells and an error in dash, posh and yash" {
  run_lab_c 'exec 2>/dev/null; echo $(( ))'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | busybox | mksh) expect_value 0 ;;
    dash | posh | yash) expect_error ;;
    *) false ;;
  esac
}

@test "prefix increment changes the variable in most shells but silently does nothing in dash" {
  run_lab_c 'exec 2>/dev/null; x=3; echo $((++x)) $x'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | busybox | mksh | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "4 4" ]
      ;;
    dash)
      [ "$status" -eq 0 ]
      [ "$output" = "3 3" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = "4 4" ]
      ;;
    *) false ;;
  esac
}

@test "textual substitution of a negative number can form the token --" {
  run_lab_c 'exec 2>/dev/null; x=-5; echo $((3-$x))'
  case $FAMILY in
    bash | dash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = 8 ]
      ;;
    zsh | zsh-sh | posh | mksh | ksh93 | yash)
      [ "$status" -ne 0 ]
      [ -z "$output" ]
      ;;
    *) false ;;
  esac
}

@test "naming the variable or parenthesizing the substitution is correct everywhere" {
  run_lab_c 'x=-5; echo $((3 - x)) $((3 - $x)) $((3-($x)))'
  [ "$status" -eq 0 ]
  [ "$output" = "8 8 8" ]
}

@test "&& and || short-circuit assignments everywhere except BusyBox ash" {
  run_lab_c 'a=$((0 && (x = 5))); b=$((1 || (y = 5))); echo "${x-unset} ${y-unset}"'
  [ "$status" -eq 0 ]
  case $FAMILY in
    busybox) [ "$output" = "5 5" ] ;;
    bash | dash | zsh | zsh-sh | yash | posh | mksh | ksh93) [ "$output" = "unset unset" ] ;;
    *) false ;;
  esac
}

@test "the dead arm of ?: is not evaluated, even in BusyBox ash" {
  run_lab_c 'a=$((1 ? 0 : (x = 5))); b=$((0 ? (y = 5) : 0)); echo "${x-unset} ${y-unset}"'
  [ "$status" -eq 0 ]
  [ "$output" = "unset unset" ]
}

@test "double parentheses as a command are an extension, not POSIX" {
  run_lab_c 'exec 2>/dev/null; ((x = 3)); echo "$? ${x-unset}"'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "0 3" ]
      ;;
    dash | busybox | posh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "127 unset" ]
      ;;
    *) false ;;
  esac
}

@test "a command substitution that starts with a subshell needs the space after the dollar paren" {
  run_lab_c 'echo $( (echo a) )'
  [ "$status" -eq 0 ]
  [ "$output" = a ]
}

@test "the unspaced form is still a command substitution where the shell falls back from arithmetic" {
  run_lab_c 'exec 2>/dev/null; echo $((echo a); (echo b))'
  case $FAMILY in
    bash | ksh93 | zsh | zsh-sh | mksh | posh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "a b" ]
      ;;
    # A parse error is reported before `exec 2>/dev/null` runs, so bats keeps the diagnostic.
    dash)
      [ "$status" -eq 2 ]
      [[ $output == *"Missing '))'"* ]]
      ;;
    busybox)
      [ "$status" -eq 2 ]
      [[ $output == *"missing '))'"* ]]
      ;;
    *) false ;;
  esac
}

# --- injection: which shells evaluate command substitutions found in a variable's value ---

@test "evaluating a variable named by its value runs a command hidden in an array subscript only in some shells" {
  export HIT="$BATS_TEST_TMPDIR/hit"
  rm -f "$HIT"
  run_lab_c 'exec 2>/dev/null; x=$1; echo $((x))' sh 'a[$(: >"$HIT")]'
  case $FAMILY in
    bash | mksh | posh) [ -e "$HIT" ] ;;
    ksh93)
      case $VERSION in
        *93u+m*) [ ! -e "$HIT" ] ;;
        *) [ -e "$HIT" ] ;;
      esac
      ;;
    dash | busybox | zsh | zsh-sh | yash) [ ! -e "$HIT" ] ;;
    *) false ;;
  esac
}

@test "validating with arith-safe.sh keeps the payload away from every shell" {
  export HIT="$BATS_TEST_TMPDIR/hit"
  rm -f "$HIT"
  run_lab "$PROBE" add 'a[$(: >"$HIT")]' 1
  [ "$status" -eq 1 ]
  [ -z "$output" ]
  [ ! -e "$HIT" ]
}

# --- the validated wrapper in arith-safe.sh ---

@test "dec strips leading zeros so 010 and 08 are decimal in every shell" {
  run_lab "$PROBE" dec 010
  [ "$status" -eq 0 ]
  [ "$output" = 10 ]
  run_lab "$PROBE" dec 08
  [ "$status" -eq 0 ]
  [ "$output" = 8 ]
  run_lab "$PROBE" dec -007
  [ "$output" = -7 ]
}

@test "dec normalizes zero and keeps nine digits" {
  run_lab "$PROBE" dec -000
  [ "$output" = 0 ]
  run_lab "$PROBE" dec +42
  [ "$output" = 42 ]
  run_lab "$PROBE" dec 999999999
  [ "$status" -eq 0 ]
  [ "$output" = 999999999 ]
}

@test "dec rejects anything but a signed run of digits with status 1" {
  for bad in '' abc 1+2 0x10 '1 ' ' 1' - + --5 1.5 '$HOME' 'a[0]'; do
    run_lab "$PROBE" dec "$bad"
    [ "$status" -eq 1 ]
    [ -z "$output" ]
  done
}

@test "dec rejects ten digits with status 2" {
  run_lab "$PROBE" dec 1234567890
  [ "$status" -eq 2 ]
  [ -z "$output" ]
}

@test "add of two nine-digit numbers is the same in 32-bit and 64-bit shells" {
  run_lab "$PROBE" add 999999999 999999999
  [ "$status" -eq 0 ]
  [ "$output" = 1999999998 ]
  run_lab "$PROBE" add -999999999 -999999999
  [ "$output" = -1999999998 ]
}

@test "add accepts a negative operand without forming a -- token" {
  run_lab "$PROBE" add 3 -5
  [ "$status" -eq 0 ]
  [ "$output" = -2 ]
  run_lab "$PROBE" add 010 010
  [ "$output" = 20 ]
}

@test "sum adds any number of arguments and returns 0 for none" {
  run_lab "$PROBE" sum 1 2 3 4
  [ "$status" -eq 0 ]
  [ "$output" = 10 ]
  run_lab "$PROBE" sum
  [ "$status" -eq 0 ]
  [ "$output" = 0 ]
}

@test "sum stops at the first bad argument and refuses a total beyond nine digits" {
  run_lab "$PROBE" sum 1 x 3
  [ "$status" -eq 1 ]
  [ -z "$output" ]
  run_lab "$PROBE" sum 999999999 1
  [ "$status" -eq 2 ]
  [ -z "$output" ]
}

@test "an unknown mode or a wrong operand count is a usage error" {
  run_lab "$PROBE" bogus
  [ "$status" -eq 64 ]
  run_lab "$PROBE" dec
  [ "$status" -eq 64 ]
  run_lab "$PROBE" add 1
  [ "$status" -eq 64 ]
}
