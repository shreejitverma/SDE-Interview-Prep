#!/bin/sh
# dollar-probe.sh: probe ANSI-C quoting ($'...') semantics across shell families.
# Used by the Dollar-Single-Quotes lab to verify POSIX.1-2024 compliance and divergences.
# shellcheck disable=SC3003 # Testing POSIX.1-2024 dollar-single-quotes feature across shells

set -eu

mode="${1:-help}"
shift || true

case "$mode" in
  basic-escapes)
    # Tests basic escape sequence decoding (\t, \n)
    printf 'data=<%s>\n' $'hello\tworld\n'
    ;;

  single-quote)
    # Tests escaped single quote inside dollar-single-quote.
    # Eval with heredoc is used so shells lacking $'...' report the syntax divergence at runtime.
    read -r cmd <<'EOF'
printf "<%s>\n" $'it\'s'
EOF
    eval "$cmd"
    ;;

  no-expansion)
    # Proves expansions ($var, $(cmd), $((expr)), *) are disabled inside $'...'
    # shellcheck disable=SC2016 # Characters are intended to be literal within $'...'
    printf '<%s>\n' $'x=$x cmd=$(echo fail) arith=$((1+1)) glob=*'
    ;;

  field-splitting)
    # Proves $'...' evaluated content is treated as a single quoted field
    # shellcheck disable=SC2016 # Wildcard * is intended to be literal within $'*'
    set -- $'one two\tthree\nfour' $'*'
    printf '%d args: <%s> <%s>\n' "$#" "$1" "$2"
    ;;

  double-quoted)
    # Proves $'...' inside double quotes remains literal
    printf '<%s>\n' "$'literal\nnewline'"
    ;;

  hex-limit)
    dump="$1"
    # Tests whether \x consumes more than two hex digits
    "$dump" $'\x4142'
    ;;

  control-chars)
    dump="$1"
    # Tests \ca, \c? control character decoding
    printf '%s' $'\ca\c?' | "$dump" -s
    ;;

  null-byte-builtin)
    dump="$1"
    # Tests shell builtin handling of embedded null byte
    printf '%s' a$'b\0c'd | "$dump" -s
    ;;

  null-byte-exec)
    dump="$1"
    # Tests kernel execve(2) argument truncation on embedded null byte
    "$dump" a$'b\0c'd
    ;;

  ifs-newline)
    # Tests setting IFS to newline via $'...'
    IFS=$'\n'
    str="line1 line2
line3"
    # shellcheck disable=SC2086,SC2248 # Explicitly testing field splitting of unquoted parameter
    set -- $str
    printf 'count=%d: <%s>' "$#" "$1"
    if [ "$#" -gt 1 ]; then
      printf ' <%s>' "$2"
    fi
    printf '\n'
    ;;

  *)
    printf 'Usage: %s <mode> [args...]\n' "$0" >&2
    exit 1
    ;;
esac
