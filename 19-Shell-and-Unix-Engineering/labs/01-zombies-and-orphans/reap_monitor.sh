#!/bin/sh
# reap_monitor.sh - Probe shell background job management, wait, and exit status retention.

set -eu

mode="${1:-}"

case "${mode}" in
  spawn-reap)
    code="${2:-0}"
    # Spawn background job with specified exit code
    (exit "${code}") &
    job_pid=$!
    reap_rc=0
    wait "${job_pid}" 2>/dev/null || reap_rc=$?
    printf 'STATUS=%d\n' "${reap_rc}"
    ;;

  double-wait)
    code="${2:-42}"
    (exit "${code}") &
    job_pid=$!
    status1=0
    wait "${job_pid}" 2>/dev/null || status1=$?
    status2=0
    wait "${job_pid}" 2>/dev/null || status2=$?
    printf 'FIRST_WAIT=%d\nSECOND_WAIT=%d\n' "${status1}" "${status2}"
    ;;

  wait-all)
    code="${2:-17}"
    # running: the first job is still alive when the bare wait starts; exited: it ended a second before.
    # pdksh-derived shells (mksh, posh) forget only the jobs the bare wait itself waited for.
    timing="${3:-running}"
    case "${timing}" in
      running) (/bin/sleep 1 && exit "${code}") & ;;
      exited) (exit "${code}") & ;;
      *)
        printf 'wait-all: timing must be running or exited\n' >&2
        exit 1
        ;;
    esac
    first_pid=$!
    (exit 0) &
    if [ "${timing}" = exited ]; then
      /bin/sleep 1
    fi
    wait
    all_status=$?
    after_status=0
    wait "${first_pid}" 2>/dev/null || after_status=$?
    printf 'WAIT_ALL=%d\nWAIT_PID_AFTER_ALL=%d\n' "${all_status}" "${after_status}"
    ;;

  subshell-orphan)
    pid_file="${2:-}"
    # Explicit subshell spawns background sleeper and terminates immediately
    (
      /bin/sleep 2 >/dev/null 2>&1 &
      printf '%d\n' "$!" >"${pid_file}"
    )
    child_pid=$(cat "${pid_file}")
    printf 'ORPHAN_PID=%d\n' "${child_pid}"
    # Verify child is alive even though its spawner subshell exited
    if kill -0 "${child_pid}" 2>/dev/null; then
      printf 'IS_ALIVE=1\n'
    else
      printf 'IS_ALIVE=0\n'
    fi
    # Clean up orphan
    kill "${child_pid}" 2>/dev/null || true
    ;;

  *)
    printf 'usage: %s <spawn-reap|double-wait|wait-all CODE running|exited|subshell-orphan> [args]\n' "$0" >&2
    exit 1
    ;;
esac
