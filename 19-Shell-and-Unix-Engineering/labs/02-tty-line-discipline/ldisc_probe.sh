#!/bin/sh
# Probe TTY line discipline mechanisms and shell terminal behaviors.

set -eu

if [ -z "${PROBE:-}" ]; then
  script_dir=$(dirname "$0")
  if [ -x "${script_dir}/pty_ldisc_probe" ]; then
    PROBE="${script_dir}/pty_ldisc_probe"
  elif [ -x "./pty_ldisc_probe" ]; then
    PROBE="./pty_ldisc_probe"
  else
    PROBE="pty_ldisc_probe"
  fi
fi

MODE="${1:-help}"
shift || true

case "${MODE}" in
  query-ldisc)
    "${PROBE}" query_ldisc
    ;;

  canonical-erase)
    "${PROBE}" canonical_erase
    ;;

  canonical-kill)
    "${PROBE}" canonical_kill
    ;;

  canonical-werase)
    "${PROBE}" canonical_werase
    ;;

  eof-flush)
    "${PROBE}" eof_flush
    ;;

  crnl-mapping)
    "${PROBE}" crnl_mapping
    ;;

  echo-toggle)
    "${PROBE}" echo_toggle
    ;;

  noncanon-switch)
    "${PROBE}" noncanon_switch
    ;;

  isig-signal)
    "${PROBE}" isig_signal
    ;;

  shell-read)
    "${PROBE}" shell_read "$@"
    ;;

  *)
    printf 'Usage: %s <mode> [args...]\n' "$0" >&2
    exit 1
    ;;
esac
