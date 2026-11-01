#!/bin/sh
# heredoc-kind: hand an N-line here-document to a command and report what that command saw.
# The command is cksum; it reads its standard input, so the kind of descriptor is measured from
# inside the redirected command with test -p and test -f on /dev/stdin.
# Usage: heredoc-kind.sh LINES [quoted|unquoted]
# Prints: kind=<pipe|file|other> delivered=<cksum output> expected=<cksum output>
# Each body line is 40 bytes, so LINES=10 is 400 bytes and LINES=7500 is 300000 bytes.
set -eu
lines=${1:?usage: heredoc-kind.sh LINES [quoted|unquoted]}
mode=${2:-quoted}
case $mode in
  quoted) delim="'EOF'" ;;
  unquoted) delim=EOF ;;
  *)
    echo "heredoc-kind.sh: unknown mode: $mode" >&2
    exit 2
    ;;
esac

dir=$(mktemp -d)
trap 'rm -rf "$dir"' EXIT

body() {
  awk -v n="$lines" 'BEGIN { for (i = 1; i <= n; i++) printf "line %07d abcdefghijklmnopqrstuvwxyz\n", i }'
}

# The generated file holds the here-document, so no formatter can change its layout.
# shellcheck disable=SC2016 # the generated script, not this one, expands $k
{
  printf '{ if [ -p /dev/stdin ]; then k=pipe; elif [ -f /dev/stdin ]; then k=file; else k=other; fi; printf "%%s " "$k"; cksum; } <<%s\n' "$delim"
  body
  printf 'EOF\n'
} >"$dir/gen.sh"

# shellcheck disable=SC1090,SC1091 # generated at run time
got=$(. "$dir/gen.sh")
want=$(body | cksum)
printf 'kind=%s delivered=%s expected=%s\n' "${got%% *}" "${got#* }" "$want"
