#!/bin/sh
# read-probes.sh: exercise read builtin behaviors across shell implementations.

set -e

mode=${1:-}
shift || true

case "$mode" in
  split)
    read -r v1 v2 v3
    printf 'v1=[%s] v2=[%s] v3=[%s]\n' "$v1" "$v2" "$v3"
    ;;
  raw-vs-escape)
    # shellcheck disable=SC2162 # intentionally testing unescaped read without -r
    read e1 e2
    read -r r1 r2
    printf 'esc=[%s|%s] raw=[%s|%s]\n' "$e1" "$e2" "$r1" "$r2"
    ;;
  delim)
    delim=${1:-:}
    st=0
    # shellcheck disable=SC2248,SC3045 # delim is single-character option argument; -d standardized in POSIX.1-2024
    read -d "$delim" -r v1 || st=$?
    printf 'delim=[%s] status=[%d]\n' "$v1" "$st"
    ;;
  eof-loop)
    while IFS= read -r line || [ -n "$line" ]; do
      printf 'LINE:[%s]\n' "$line"
    done
    ;;
  naive-loop)
    while IFS= read -r line; do
      printf 'LINE:[%s]\n' "$line"
    done
    ;;
  readonly-assign)
    set +e
    readonly ro=fixed
    a=orig
    b=orig
    read -r a ro b 2>/dev/null
    st=$?
    set -e
    printf 'st=%d a=%s ro=%s b=%s\n' "$st" "$a" "$ro" "$b"
    ;;
  stream-handoff)
    read -r first
    printf 'FIRST:[%s]\n' "$first"
    cat
    ;;
  *)
    # shellcheck disable=SC2248 # mode in error diagnostic
    printf 'read-probes: unknown mode: %s\n' "$mode" >&2
    exit 64
    ;;
esac
