#!/usr/bin/env bash
set -euo pipefail
# requires: linux

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

cd "$WORKDIR"
cp "$SCRIPT_DIR/crash.c" "$SCRIPT_DIR/triage.py" .

# -O2 like a release build; sibling-call optimization off so every frame is a real frame.
gcc -O2 -g -fno-optimize-sibling-calls -Wl,--build-id -fdebug-prefix-map="$WORKDIR"=. crash.c -o app

BUILD_ID="$(eu-readelf -n app | awk '/Build ID:/ {print $3}')"

# Release split: debug info to a side file, then strip everything (including .symtab).
objcopy --only-keep-debug app app.debug
strip --strip-all app
DEBUG_ROOT="$WORKDIR/debug_root"
mkdir -p "$DEBUG_ROOT/.build-id/${BUILD_ID:0:2}"
mv app.debug "$DEBUG_ROOT/.build-id/${BUILD_ID:0:2}/${BUILD_ID:2}.debug"

# Prefer a real kernel core. If core_pattern pipes to a handler (systemd-coredump, apport) or
# names another directory, the core never lands here, so fall back to having gdb write one at the fault.
ulimit -c unlimited
PATTERN="$(cat /proc/sys/kernel/core_pattern)"
case "$PATTERN" in
    "|"*) FALLBACK_REASON="core_pattern pipes to a handler" ;;
    */*) FALLBACK_REASON="core_pattern writes to another directory" ;;
    *) FALLBACK_REASON="" ;;
esac
if [ -z "$FALLBACK_REASON" ]; then
    set +e
    ./app
    status=$?
    set -e
    if [ "$status" -ne 139 ]; then
        echo "expected the program to die with SIGSEGV (exit 139), got $status" >&2
        exit 1
    fi
    CORE="$(find . -maxdepth 1 -name 'core*' -type f -print -quit)"
    CORE_SOURCE="kernel (core_pattern is a file path)"
else
    gdb -batch -nx -ex run -ex "generate-core-file core" ./app >/dev/null 2>&1
    CORE="./core"
    CORE_SOURCE="gdb generate-core-file ($FALLBACK_REASON)"
fi
if [ -z "$CORE" ] || [ ! -s "$CORE" ]; then
    echo "no core file was produced" >&2
    exit 1
fi

# The core records the Build ID of every mapped ELF; it must match the binary we hold.
CORE_BUILD_ID="$(eu-unstrip -n --core="$CORE" | awk '$NF ~ /app$/ || $NF == "-" && NR == 1 {print $2; exit}' | cut -d@ -f1)"
if [ "$BUILD_ID" = "$CORE_BUILD_ID" ]; then BUILD_ID_EQUAL=yes; else BUILD_ID_EQUAL=no; fi
export BUILD_ID_EQUAL CORE_SOURCE

# Pass 1: no debug info reachable, so the stripped binary yields no names or lines.
export TRIAGE_PASS=without-debug-info
gdb -batch -q -nx -ex "set debug-file-directory /nonexistent" -ex "file ./app" \
    -ex "core-file $CORE" -ex "source triage.py" 2>/dev/null | sed -n '/^=== /,$p'
echo
# Pass 2: the .build-id tree supplies DWARF, so every frame resolves to function and line.
export TRIAGE_PASS=with-build-id
gdb -batch -q -nx -ex "set debug-file-directory $DEBUG_ROOT" -ex "file ./app" \
    -ex "core-file $CORE" -ex "source triage.py" 2>/dev/null | sed -n '/^=== /,$p'
