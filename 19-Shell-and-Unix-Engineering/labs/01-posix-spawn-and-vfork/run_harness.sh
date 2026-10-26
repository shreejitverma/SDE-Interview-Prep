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
  "${CC:-cc}" -O2 -Wall -Wextra -std=c99 "$c_src" -o "$bin"
fi

"$bin" "$mode" ${1+"$@"}
