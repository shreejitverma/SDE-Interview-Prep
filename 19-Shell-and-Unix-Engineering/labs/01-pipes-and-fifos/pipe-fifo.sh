#!/bin/sh
# Lab 01: Pipes and FIFOs execution behavior and IPC patterns.
set -eu

mode=${1:-help}
shift || true

case "$mode" in
  pipeline-subshell)
    # Demonstrates whether pipeline elements execute in subshell environments.
    count=0
    # shellcheck disable=SC2030 # intentional pipeline subshell mutation under test
    printf 'alpha\nbeta\ngamma\n' | while read -r _; do
      count=$((count + 1))
    done
    # shellcheck disable=SC2031 # intentional observation of parent environment variable
    printf 'count:%d\n' "$count"
    ;;

  heredoc-env)
    # Portable POSIX alternative: stdin redirection retains current environment.
    count=0
    while read -r _; do
      count=$((count + 1))
    done <<'EOF'
alpha
beta
gamma
EOF
    printf 'count:%d\n' "$count"
    ;;

  pipeline-status)
    # Demonstrates pipeline exit status with and without pipefail.
    opt=${1:-default}
    code=0
    case "$opt" in
      default)
        sh -c 'exit 42' | true || code=$?
        printf 'status:%d\n' "$code"
        ;;
      pipefail)
        # shellcheck disable=SC3040 # probing shell support for pipefail
        if (set -o pipefail) 2>/dev/null; then
          # shellcheck disable=SC3040 # enabling pipefail when supported
          set -o pipefail
          sh -c 'exit 42' | true || code=$?
          printf 'status:%d\n' "$code"
        else
          printf 'unsupported:pipefail\n'
        fi
        ;;
      *)
        printf 'error:unknown status mode\n' >&2
        exit 2
        ;;
    esac
    ;;

  fifo-rendezvous)
    # Coordinated producer and consumer through a named FIFO.
    fifopath=${1:-}
    payload=${2:-default-payload}
    [ -n "$fifopath" ] || {
      printf 'error:fifo path required\n' >&2
      exit 2
    }
    rm -f "$fifopath" "$fifopath.out"
    mkfifo -m 0600 "$fifopath"
    # Spawn background consumer
    cat "$fifopath" >"$fifopath.out" &
    consumer_pid=$!
    # Write payload from producer
    printf '%s\n' "$payload" >"$fifopath"
    wait "$consumer_pid"
    cat "$fifopath.out"
    rm -f "$fifopath" "$fifopath.out"
    ;;

  fifo-rdwr)
    # Opening FIFO with O_RDWR avoids blocking open when supported.
    fifopath=${1:-}
    payload=${2:-bidirectional-data}
    [ -n "$fifopath" ] || {
      printf 'error:fifo path required\n' >&2
      exit 2
    }
    rm -f "$fifopath"
    mkfifo -m 0600 "$fifopath"
    # Open descriptor 3 in read-write mode
    exec 3<>"$fifopath"
    printf '%s\n' "$payload" >&3
    line=""
    read -r line <&3
    exec 3>&-
    rm -f "$fifopath"
    printf 'rdwr:%s\n' "$line"
    ;;

  fifo-leak-clean)
    # Clean descriptor lifecycle: closing writer yields EOF immediately to reader.
    fifopath=${1:-}
    [ -n "$fifopath" ] || {
      printf 'error:fifo path required\n' >&2
      exit 2
    }
    rm -f "$fifopath"
    mkfifo -m 0600 "$fifopath"
    (
      printf 'line1\nline2\n' >"$fifopath"
    ) &
    writer_pid=$!
    read_count=0
    while read -r _; do
      read_count=$((read_count + 1))
    done <"$fifopath"
    wait "$writer_pid"
    rm -f "$fifopath"
    printf 'lines:%d\n' "$read_count"
    ;;

  *)
    printf 'usage: %s {pipeline-subshell|heredoc-env|pipeline-status|fifo-rendezvous|fifo-rdwr|fifo-leak-clean}\n' "$0" >&2
    exit 2
    ;;
esac
