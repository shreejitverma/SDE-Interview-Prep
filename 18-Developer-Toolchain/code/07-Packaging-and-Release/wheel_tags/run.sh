#!/usr/bin/env bash
set -euo pipefail
# requires: macos, network

# Builds the same C abi3 extension with two CPython builds, and a Rust abi3 extension, then shows
# what each wheel's tags promise versus what its binary needs, and whether it imports on 3.15 and 3.14.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

# Interpreter provenance decides the macOS floor: python-build-standalone (uv managed) is built
# with a low deployment target; Homebrew builds for the host it was built on.
MANAGED_PY="$(uv python find --no-config --managed-python 3.15 2>/dev/null || { uv python install --no-config -q 3.15 && uv python find --no-config --managed-python 3.15; })"
SYSTEM_PY="$(uv python find --no-config --no-managed-python 3.15)"
OLD_PY="$(uv python find --no-config --no-managed-python 3.14)"

describe() { # describe <python>
    "$1" -c 'import sys, sysconfig; print("%d.%d, sysconfig MACOSX_DEPLOYMENT_TARGET=%s" % (sys.version_info[0], sys.version_info[1], sysconfig.get_config_var("MACOSX_DEPLOYMENT_TARGET")))'
}
echo "managed (python-build-standalone) CPython: $(describe "$MANAGED_PY")"
echo "system (non-managed) CPython:              $(describe "$SYSTEM_PY")"
echo "older system CPython for import tests:     $(describe "$OLD_PY")"
echo

# Each build gets a fresh copy of the project: setuptools reuses an existing build/ directory and
# skips the compiler, so a shared tree would silently ship the previous build's binary.
build() { # build <python> <project> <label> [env assignments...]
    local py="$1" proj="$2" label="$3"
    shift 3
    mkdir "$TMP_DIR/src-$label"
    cp -R "$SCRIPT_DIR/$proj/." "$TMP_DIR/src-$label/"
    if ! env "$@" uv build --no-config --python "$py" --wheel --out-dir "$TMP_DIR/$label" "$TMP_DIR/src-$label" >"$TMP_DIR/build.log" 2>&1; then
        cat "$TMP_DIR/build.log" >&2
        exit 1
    fi
}
build "$MANAGED_PY" c_extension c_managed
build "$SYSTEM_PY" c_extension c_system
build "$SYSTEM_PY" c_extension c_system_target11 MACOSX_DEPLOYMENT_TARGET=11.0
build "$SYSTEM_PY" rust_extension rust_system

import_check() { # import_check <python> <wheel> <module>
    local venv="$TMP_DIR/venv-$RANDOM"
    uv venv --no-config -q --python "$1" "$venv"
    uv pip install --no-config -q --python "$venv/bin/python" "$2"
    if out="$("$venv/bin/python" -c "import $3" 2>&1)"; then
        echo "ok"
    else
        echo "$out" | tail -n 1 | sed -E "s#'[^']*'#'...'#g"
    fi
    rm -rf "$venv"
}

for label in c_managed c_system c_system_target11 rust_system; do
    wheel="$(find "$TMP_DIR/$label" -name '*.whl' | head -n 1)"
    module="c_abi3_example"
    [ "$label" = rust_system ] && module="rust_abi3_example"
    echo "=== $label ==="
    uv run --no-config --no-project -q --python "$SYSTEM_PY" --with "packaging==25.0" \
        python "$SCRIPT_DIR/inspect_wheel.py" "$wheel" "$TMP_DIR/unz-$label"
    so="$(find "$TMP_DIR/unz-$label" -name '*.so' | head -n 1)"
    echo "Binary minimum macOS (LC_BUILD_VERSION minos): $(otool -l "$so" | awk '/LC_BUILD_VERSION/{f=1} f && /minos/{print $2; exit}')"
    if uvx --no-config -q --from "delocate==0.13.0" delocate-wheel --require-target-macos-version 11.0 \
        -w "$TMP_DIR/deloc-$label" "$wheel" >"$TMP_DIR/deloc.log" 2>&1; then
        echo "delocate --require-target-macos-version 11.0: accepted, wrote $(basename "$(find "$TMP_DIR/deloc-$label" -name '*.whl' | head -n 1)")"
    else
        echo "delocate --require-target-macos-version 11.0: rejected"
    fi
    echo "Import on $("$SYSTEM_PY" -c 'import sys; print("%d.%d" % sys.version_info[:2])'): $(import_check "$SYSTEM_PY" "$wheel" "$module")"
    echo "Import on $("$OLD_PY" -c 'import sys; print("%d.%d" % sys.version_info[:2])'): $(import_check "$OLD_PY" "$wheel" "$module")"
    echo
done
