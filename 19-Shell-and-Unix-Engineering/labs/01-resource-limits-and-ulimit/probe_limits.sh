#!/bin/sh
# probe_limits.sh: probe shell ulimit builtin behavior and kernel rlimit mechanics.
# shellcheck disable=SC3045 # ulimit flags (-H, -S, -n, -u, -p, -f) are under direct test across shell implementations

set -eu

mode="${1:-}"

case "${mode}" in
  subshell_isolation)
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    orig="$(ulimit -n)"
    sub="$( (
      ulimit -n 256
      ulimit -n
    ))"
    parent="$(ulimit -n)"
    printf 'subshell=%s parent=%s orig=%s\n' "${sub}" "${parent}" "${orig}"
    ;;

  pipeline_isolation)
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    orig="$(ulimit -n)"
    # shellcheck disable=SC2312 # intentional pipeline subshell demonstration
    stage="$({ ulimit -n 256 && ulimit -n; } | cat)"
    parent="$(ulimit -n)"
    printf 'stage=%s parent=%s orig=%s\n' "${stage}" "${parent}" "${orig}"
    ;;

  lower_and_raise_hard)
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    # Lowers both soft and hard limits to 128
    ulimit -S -n 128
    ulimit -H -n 128
    # Attempts to raise hard limit to 256 as unprivileged user
    if (ulimit -H -n 256) 2>/dev/null; then
      printf 'raise_status=0\n'
    else
      printf 'raise_status=1\n'
    fi
    ;;

  lower_hard_below_soft)
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    # Attempting to set rlim_max < rlim_cur without lowering rlim_cur
    # In POSIX shells (bash, dash, ksh93, mksh, busybox, yash) setrlimit fails with EINVAL (status 1)
    # In zsh, rlimits.c automatically clamps rlim_cur down to rlim_max (status 0)
    # The harness soft limit is above 64 (256 or more), so a clamp shows as soft=64.
    if soft=$( (ulimit -H -n 64 && ulimit -S -n) 2>/dev/null); then
      printf 'invalid_status=0 soft=%s\n' "${soft}"
    else
      printf 'invalid_status=1\n'
    fi
    ;;

  fsize_enforcement)
    target_file="${2:-}"
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    # Set file limit to 1 block (512 or 1024 bytes)
    ulimit -f 1
    # dd writing 10240 bytes will exceed limit and deliver SIGXFSZ
    # Grouped in compound block to suppress shell job termination notice on stderr
    if { dd if=/dev/zero of="${target_file}" bs=1024 count=10 2>/dev/null; } 2>/dev/null; then
      printf 'dd_status=0\n'
    else
      dd_status="$?"
      printf 'dd_status=%d\n' "${dd_status}"
    fi
    ;;

  fsize_efbig)
    target_file="${2:-}"
    inspector="${3:-}"
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    ulimit -f 1
    "${inspector}" write_efbig "${target_file}"
    ;;

  nofile_boundary)
    inspector="${2:-}"
    command -v ulimit >/dev/null 2>&1 || {
      printf 'ulimit: not found\n' >&2
      exit 127
    }
    ulimit -n 10
    "${inspector}" probe_fds
    ;;

  nproc_option)
    u_ok=0
    p_ok=0
    if (ulimit -u) >/dev/null 2>&1; then
      u_ok=1
    fi
    if (ulimit -p) >/dev/null 2>&1; then
      p_ok=1
    fi
    printf 'u=%d p=%d\n' "${u_ok}" "${p_ok}"
    ;;

  *)
    printf 'Unknown mode: %s\n' "${mode}" >&2
    exit 1
    ;;
esac
