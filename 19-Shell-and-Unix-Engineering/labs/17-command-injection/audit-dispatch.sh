#!/bin/sh
# Lab 17: Command Injection surfaces and hardened dispatch patterns.
set -eu

mode=${1:-help}
shift || true

case $mode in
  vuln-subshell)
    # Anti-pattern: concatenating untrusted data into a sh -c command string.
    payload=${1:-}
    sh -c "printf 'record:%s\n' $payload"
    ;;

  safe-argv)
    # Hardened: pass untrusted data in argv ($1), never inside the code string.
    payload=${1:-}
    # shellcheck disable=SC2016 # $1 belongs to the inner shell
    sh -c 'printf "record:%s\n" "$1"' sh "$payload"
    ;;

  quote-boundary)
    # Demonstrates that ordinary parameter expansion does not re-tokenize ; or $().
    payload=${1:-}
    printf 'quoted:%s\n' "$payload"
    ;;

  vuln-arith)
    # Anti-pattern: evaluating unvalidated input in an arithmetic context.
    a=0
    metric=${1:-0}
    val=$((metric + 1))
    printf 'val:%s a:%s\n' "$val" "$a"
    ;;

  safe-arith)
    # Hardened: accept only canonical ASCII decimals of at most 9 digits before arithmetic expansion.
    a=0
    metric=${1:-}
    case $metric in
      '' | *[!0-9]* | 0?* | ??????????*)
        printf 'error:invalid integer\n' >&2
        exit 2
        ;;
      *)
        ;;
    esac
    val=$((metric + 1))
    printf 'val:%s a:%s\n' "$val" "$a"
    ;;

  vuln-test-eq)
    # Anti-pattern: passing unvalidated input to test -eq (evaluates math in Korn family).
    a=0
    metric=${1:-0}
    [ "$metric" -eq 0 ]
    printf 'zero a:%s\n' "$a"
    ;;

  vuln-indirection)
    # Anti-pattern: passing an unvalidated variable name to read.
    a=0
    target=${1:-}
    val=${2:-data}
    printf '%s\n' "$val" | {
      # shellcheck disable=SC2229 # intentional dynamic target variable name under test
      read -r "$target"
      printf 'a:%s\n' "$a"
    }
    ;;

  safe-indirection)
    # Hardened: validate POSIX Name (XBD 3.216) before dynamic assignment.
    a=0
    target=${1:-}
    val=${2:-data}
    case $target in
      '' | [!a-zA-Z_]* | *[!a-zA-Z0-9_]*)
        printf 'error:invalid identifier\n' >&2
        exit 2
        ;;
      *)
        ;;
    esac
    out=""
    printf '%s\n' "$val" | {
      # shellcheck disable=SC2229 # validated dynamic variable name
      read -r "$target"
      eval "out=\${$target}"
      printf '%s:%s\n' "$target" "$out"
    }
    ;;

  *)
    printf 'usage: %s MODE [ARGS...]\n' "$0" >&2
    exit 64
    ;;
esac
