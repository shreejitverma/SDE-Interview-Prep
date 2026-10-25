#!/bin/sh
# Wrapper to compile and execute spawn_harness inside BATS_TEST_TMPDIR.

set -eu

tmpdir=$1
mode=$2
shift 2 || true

bin="$tmpdir/spawn_harness"
if [ ! -x "$bin" ]; then
  script_dir=$(cd "$(dirname "$0")" && pwd)
  c_src="$script_dir/spawn_harness.c"
  if command -v cc >/dev/null 2>&1; then
    cc -O2 -Wall -Wextra -std=c99 "$c_src" -o "$bin"
  elif command -v clang >/dev/null 2>&1; then
    clang -O2 -Wall -Wextra -std=c99 "$c_src" -o "$bin"
  elif command -v gcc >/dev/null 2>&1; then
    gcc -O2 -Wall -Wextra -std=c99 "$c_src" -o "$bin"
  else
    printf 'no_c_compiler\n'
    exit 0
  fi
fi

"$bin" "$mode" "$@"
