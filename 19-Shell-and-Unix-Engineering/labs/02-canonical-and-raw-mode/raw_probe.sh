#!/bin/sh
# Probe canonical and raw terminal modes, VMIN/VTIME timings, and shell interactions.

set -eu

if [ -z "${PROBE:-}" ]; then
  script_dir=$(dirname "$0")
  if [ -x "${script_dir}/pty_raw_probe" ]; then
    PROBE="${script_dir}/pty_raw_probe"
  elif [ -x "./pty_raw_probe" ]; then
    PROBE="./pty_raw_probe"
  else
    PROBE="pty_raw_probe"
  fi
fi

MODE="${1:-help}"
shift || true

case "${MODE}" in
  line-buffering)
    "${PROBE}" canonical_line_buffering
    ;;

  max-one-line)
    "${PROBE}" canonical_max_one_line
    ;;

  editing)
    "${PROBE}" canonical_editing
    ;;

  vmin-vtime)
    "${PROBE}" noncanon_vmin_vtime
    ;;

  case-a)
    "${PROBE}" noncanon_case_a
    ;;

  staircase-echo)
    "${PROBE}" raw_mode_staircase_and_echo
    ;;

  signals)
    "${PROBE}" raw_mode_signals
    ;;

  defaults)
    "${PROBE}" inspect_control_defaults
    ;;

  shell-exec)
    "${PROBE}" shell_raw_exec "$@"
    ;;

  *)
    printf 'Usage: %s <mode> [args...]\n' "$0" >&2
    exit 1
    ;;
esac
