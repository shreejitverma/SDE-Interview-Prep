#!/bin/sh
# tilde-probe.sh: probe POSIX.1-2024 tilde expansion semantics and shell divergences.
# Used by the Tilde-Expansion lab to verify POSIX compliance and implementation limits.
# shellcheck disable=SC2248 # Explicitly testing unquoted tilde behavior

set -eu

mode="${1:-help}"
shift || true

case "$mode" in
  basic-home)
    # Tests unquoted ~ expansion to HOME
    HOME="${1:-/test/home}"
    # shellcheck disable=SC2088 # Tilde expansion is under test
    printf 'home=<%s>\n' ~
    ;;

  whitespace-home)
    # Tests that expanded HOME is treated as if quoted (no field splitting)
    dump="$1"
    HOME="dir with spaces"
    # shellcheck disable=SC2086,SC2088 # Testing unquoted tilde argument passing
    "$dump" --dump-args ~
    ;;

  unset-home)
    # Tests ~ expansion when HOME is unset
    unset HOME || true
    # shellcheck disable=SC2088 # Testing unset HOME expansion
    printf '<%s>\n' ~
    ;;

  user-lookup)
    # Tests lookup of root user directory
    # shellcheck disable=SC2088 # User lookup under test
    printf 'root=<%s>\n' ~root
    ;;

  unknown-user)
    # Tests unrecognized login name handling
    # shellcheck disable=SC2088 # Testing unknown user behavior
    printf 'unknown=<%s>\n' ~nosuchuser99999
    ;;

  dir-stack)
    # Tests ~+ and ~- directory stack extensions
    dir_a="$1"
    dir_b="$2"
    cd "$dir_a"
    cd "$dir_b"
    # shellcheck disable=SC2088 # Testing ~+ and ~- extensions
    printf 'pwd=<%s> oldpwd=<%s>\n' ~+ ~-
    ;;

  quoted-prefix)
    # Tests quoting within or before tilde prefix
    # shellcheck disable=SC1001,SC2088 # Escaped characters and tildes are under test
    printf 'backslash=<%s> double=<%s> single=<%s> empty_double=<%s> quoted_user=<%s> escaped_char=<%s>\n' \
      \~ "~" '~' ~"" ~"root" ~r\oot
    ;;

  expansion-order)
    # Tests tilde expansion relative to parameter expansion (tilde runs first)
    u="root"
    # shellcheck disable=SC2088 # Testing order: tilde before variable expansion
    printf 'var=<%s>\n' ~$u
    ;;

  assignment-colon)
    # Tests assignment context and colon-separated tilde prefixes
    HOME="/mock/home"
    # shellcheck disable=SC2088 # Testing assignment tilde expansion
    a=~/bin:~root/bin
    b=:~
    # shellcheck disable=SC1001 # Escaped colon is under test
    c=foo\:~
    d="foo:":~
    printf 'a=<%s> b=<%s> c=<%s> d=<%s>\n' "$a" "$b" "$c" "$d"
    ;;

  command-operand)
    # Tests assignment syntax in command operand vs declaration utility
    HOME="/mock/home"
    # Normal command operand: not an assignment
    # shellcheck disable=SC2088 # Testing lack of expansion in operand
    printf 'operand=<%s>\n' PATH=~/bin:~root/bin
    # Declaration utility: treated as assignment
    # shellcheck disable=SC2088 # Testing declaration utility expansion
    export FOO=~/bin:~root/bin
    printf 'export=<%s>\n' "$FOO"
    ;;

  pe-default)
    # Tests tilde expansion in unquoted vs double-quoted parameter expansion default
    HOME="/mock/home"
    unset v || true
    # shellcheck disable=SC2086,SC2088 # Unquoted parameter expansion default is explicitly under test
    printf 'unquoted=<%s> quoted=<%s>\n' ${v:-~} "${v:-~}"
    ;;

  redirection)
    # Tests tilde expansion in redirection targets
    target_dir="$1"
    HOME="$target_dir"
    # shellcheck disable=SC2088 # Testing tilde in redirection
    printf 'ok\n' >~/probe.out
    if [ -f "$target_dir/probe.out" ]; then
      printf 'redirect=ok\n'
    else
      printf 'redirect=fail\n'
    fi
    ;;

  *)
    printf 'Usage: %s <mode> [args...]\n' "$0" >&2
    exit 1
    ;;
esac
