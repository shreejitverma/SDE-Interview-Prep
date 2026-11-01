#!/bin/sh
# arith-safe: keep untrusted text away from $(( )) by checking that it is a bounded decimal integer first.
# Usage:
#   arith-safe.sh dec VALUE     print VALUE as a canonical decimal integer (leading zeros stripped)
#   arith-safe.sh add A B       print A + B
#   arith-safe.sh sum [VALUE..] print the sum of all VALUEs (0 for none)
# Exit status: 0 success, 1 not a decimal integer, 2 too many digits (or a sum out of range), 64 usage.
#
# Why this shape:
# - Stripping leading zeros means "010" is ten in every shell, never eight (POSIX octal) or ten by accident.
# - Rejecting anything but an optional sign and digits means no shell ever sees a name, an operator,
#   a base prefix, or an array subscript such as a[$(cmd)], which some shells evaluate recursively.
# - Capping the width at 9 digits keeps every sum below 2^31, so the answer does not depend on whether
#   the shell's integers are 32-bit (mksh) or 64-bit (the rest), and POSIX only guarantees "signed long".

MAXDIGITS=9

# to_dec VALUE: print VALUE as a canonical decimal integer; status 1 if malformed, 2 if too long.
to_dec() {
  td_sign=
  td_digits=$1
  case $td_digits in
    -*)
      td_sign=-
      td_digits=${td_digits#-}
      ;;
    +*)
      td_digits=${td_digits#+}
      ;;
    *) ;;
  esac
  case $td_digits in
    '' | *[!0-9]*) return 1 ;;
    *) ;;
  esac
  td_digits=${td_digits#"${td_digits%%[!0]*}"}
  [ -n "$td_digits" ] || td_digits=0
  [ "${#td_digits}" -le "$MAXDIGITS" ] || return 2
  [ "$td_digits" != 0 ] || td_sign=
  printf '%s%s\n' "$td_sign" "$td_digits"
}

# add A B: validated A + B. The operands are named, not substituted, so a negative B cannot form "--".
add() {
  add_a=$(to_dec "$1") || return $?
  add_b=$(to_dec "$2") || return $?
  printf '%s\n' "$((add_a + add_b))"
}

# sum [VALUE..]: validated running total, bounded to MAXDIGITS digits at every step.
sum() {
  sum_total=0
  for sum_arg in "$@"; do
    sum_v=$(to_dec "$sum_arg") || return $?
    sum_total=$((sum_total + sum_v))
    [ "$sum_total" -le 999999999 ] && [ "$sum_total" -ge -999999999 ] || return 2
  done
  printf '%s\n' "$sum_total"
}

mode=${1-}
[ "$#" -eq 0 ] || shift
case $mode in
  dec)
    [ "$#" -eq 1 ] || exit 64
    to_dec "$1"
    ;;
  add)
    [ "$#" -eq 2 ] || exit 64
    add "$1" "$2"
    ;;
  sum)
    sum "$@"
    ;;
  *)
    echo "usage: arith-safe.sh dec VALUE | add A B | sum [VALUE..]" >&2
    exit 64
    ;;
esac
