#!/bin/sh
# test_probe.sh: probe test and [ builtin mechanisms and edge cases.
# Usage: test_probe.sh <mode> [args...]
# Modes:
#   arg-count        Test 0, 1, 2, 3, 4 argument evaluation
#   unquoted-pitfall Demonstrate unquoted variable collapse with -n and =
#   file-ops <dir>   Test POSIX.1-2024 -ef, -nt, and -ot primaries in <dir>

set -eu

mode="${1:-}"

case "$mode" in
  arg-count)
    # 0 arguments: test exits 1 (false)
    test && zero=0 || zero=$?
    # 1 argument: non-empty string is true, empty string is false
    test "nonempty" && one_true=0 || one_true=$?
    test "" && one_false=0 || one_false=$?
    # 2 arguments: unary primary vs negation
    test ! "" && two_neg=0 || two_neg=$?
    test -n "val" && two_unary=0 || two_unary=$?
    # 3 arguments: binary primary takes precedence over unary
    test "(" = "(" && three_bin=0 || three_bin=$?
    test "!" = "!" && three_excl=0 || three_excl=$?
    # 4 arguments: leading ! negates 3-argument evaluation
    test ! "(" = "(" && four_neg=0 || four_neg=$?

    printf '0-arg=%d\n' "$zero"
    printf '1-arg-nonempty=%d 1-arg-empty=%d\n' "$one_true" "$one_false"
    printf '2-arg-neg=%d 2-arg-unary=%d\n' "$two_neg" "$two_unary"
    printf '3-arg-paren=%d 3-arg-excl=%d\n' "$three_bin" "$three_excl"
    printf '4-arg-neg=%d\n' "$four_neg"
    ;;

  unquoted-pitfall)
    # Unquoted empty variable: field splitting produces zero fields, test sees 1 argument ("-n")
    empty_var=""
    # shellcheck disable=SC2070,SC2086,SC2248 # intentional unquoted expansion to demonstrate the bug
    if [ -n $empty_var ]; then
      unquoted_res="bug: empty evaluated true"
    else
      unquoted_res="safe: empty evaluated false"
    fi

    # Quoted empty variable: produces empty string field, test sees 2 arguments ("-n", "")
    if [ -n "$empty_var" ]; then
      quoted_res="bug: empty evaluated true"
    else
      quoted_res="safe: empty evaluated false"
    fi

    printf 'unquoted: %s\n' "$unquoted_res"
    printf 'quoted: %s\n' "$quoted_res"
    ;;

  file-ops)
    dir="${2:-}"
    if [ -z "$dir" ] || [ ! -d "$dir" ]; then
      printf 'Error: directory required for file-ops\n' >&2
      exit 2
    fi

    file1="$dir/file1"
    file2="$dir/file2"
    file1_hard="$dir/file1_hard"

    touch "$file1"
    ln "$file1" "$file1_hard"
    # Delay to ensure distinct mtime across filesystem resolutions
    sleep 1
    touch "$file2"

    [ "$file1" -ef "$file1_hard" ] 2>/dev/null && ef_res="same_inode" || ef_res="different"
    [ "$file2" -nt "$file1" ] 2>/dev/null && nt_res="newer" || nt_res="not_newer"
    [ "$file1" -ot "$file2" ] 2>/dev/null && ot_res="older" || ot_res="not_older"

    printf 'ef=%s nt=%s ot=%s\n' "$ef_res" "$nt_res" "$ot_res"
    ;;

  *)
    printf 'Usage: %s {arg-count|unquoted-pitfall|file-ops <dir>}\n' "$0" >&2
    exit 2
    ;;
esac
