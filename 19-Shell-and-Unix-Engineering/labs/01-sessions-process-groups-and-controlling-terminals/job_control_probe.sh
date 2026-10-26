#!/bin/sh
# Probe shell process group, session, and terminal behavior.

set -eu

if [ -z "${PROBE:-}" ]; then
  script_dir=$(dirname "$0")
  if [ -x "${script_dir}/pgrp_probe" ]; then
    PROBE="${script_dir}/pgrp_probe"
  elif [ -x "./pgrp_probe" ]; then
    PROBE="./pgrp_probe"
  else
    PROBE="pgrp_probe"
  fi
fi

MODE="${1:-help}"
TMP="${BATS_TEST_TMPDIR:-/tmp}"

case "${MODE}" in
  pgid-inherited)
    if (set +m) 2>/dev/null; then
      set +m 2>/dev/null || true
    fi
    "${PROBE}" info >"${TMP}/sh.out"
    "${PROBE}" info | cat >"${TMP}/pipe.out"
    "${PROBE}" info >"${TMP}/bg.out" &
    wait
    sh_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/sh.out")
    pipe_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/pipe.out")
    bg_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/bg.out")
    rm -f "${TMP}/sh.out" "${TMP}/pipe.out" "${TMP}/bg.out"
    printf 'shell_pgid=%s pipe_pgid=%s bg_pgid=%s\n' "${sh_pgid}" "${pipe_pgid}" "${bg_pgid}"
    ;;

  pgid-monitored)
    set -m
    "${PROBE}" info >"${TMP}/sh.out"
    "${PROBE}" info | cat >"${TMP}/pipe.out"
    "${PROBE}" info >"${TMP}/bg.out" &
    wait
    sh_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/sh.out")
    pipe_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/pipe.out")
    bg_pgid=$(sed -n 's/.*[[:space:]]pgid=\([0-9][0-9]*\).*/\1/p' "${TMP}/bg.out")
    rm -f "${TMP}/sh.out" "${TMP}/pipe.out" "${TMP}/bg.out"
    printf 'shell_pgid=%s pipe_pgid=%s bg_pgid=%s\n' "${sh_pgid}" "${pipe_pgid}" "${bg_pgid}"
    ;;

  async-stdin)
    opt="${2:-+m}"
    case "${opt}" in
      +m)
        if (set +m) 2>/dev/null; then
          set +m 2>/dev/null || true
        fi
        ;;
      -m)
        set -m
        ;;
      *)
        printf 'invalid option: %s\n' "${opt}" >&2
        exit 1
        ;;
    esac

    out_file="${TMP}/async_stdin.out"
    (
      read -r line || line="EOF"
      printf 'read: %s\n' "${line}" >"${out_file}"
    ) &
    wait
    cat "${out_file}"
    rm -f "${out_file}"
    ;;

  async-traps)
    opt="${2:-+m}"
    case "${opt}" in
      +m)
        if (set +m) 2>/dev/null; then
          set +m 2>/dev/null || true
        fi
        ;;
      -m)
        set -m
        ;;
      *)
        printf 'invalid option: %s\n' "${opt}" >&2
        exit 1
        ;;
    esac

    traps_out="${TMP}/async_traps.out"
    "${PROBE}" traps >"${traps_out}" &
    wait
    cat "${traps_out}"
    rm -f "${traps_out}"
    ;;

  setsid-detach)
    "${PROBE}" setsid
    ;;

  orphan-read)
    "${PROBE}" orphan_read
    ;;

  orphan-stop)
    "${PROBE}" orphan_stop
    ;;

  *)
    printf 'Usage: %s {pgid-inherited|pgid-monitored|async-stdin|async-traps|setsid-detach|orphan-read|orphan-stop}\n' "$0" >&2
    exit 1
    ;;
esac
