#!/bin/sh
# status_probe.sh: probe shell wait status and exit status mechanics.

set -u

mode="${1:-}"

case "${mode}" in
  direct_exit)
    target="${2:-0}"
    exit "${target}"
    ;;

  subshell_exit)
    target="${2:-0}"
    (exit "${target}")
    echo "$?"
    ;;

  cmdsub_exit)
    target="${2:-0}"
    # shellcheck disable=SC2312 # intentional capture of command substitution status
    _val=$(exit "${target}")
    echo "$?"
    ;;

  notfound_cmd)
    # POSIX requires exit status 127 when a command is not found.
    /nonexistent_command_xyz_12345 2>/dev/null || true
    /nonexistent_command_xyz_12345 2>/dev/null
    echo "$?"
    ;;

  noexec_cmd)
    target="${2:-}"
    # POSIX requires exit status 126 when a command is not executable.
    "${target}" 2>/dev/null
    echo "$?"
    ;;

  wait_unknown)
    # POSIX requires wait on unknown PID to return 127.
    wait 99999999 2>/dev/null
    echo "$?"
    ;;

  declaration_mask_trap)
    target="${2:-42}"
    # POSIX export masking demonstration
    # shellcheck disable=SC2155,SC2312 # deliberate demonstration of status masking by declaration
    export masked_exp="$(exit "${target}")"
    s_masked_exp="$?"

    # Unmasked separate assignment
    # shellcheck disable=SC2312 # deliberate capture of status
    unmasked_exp="$(exit "${target}")"
    s_unmasked_exp="$?"
    export unmasked_exp

    loc_res=""
    if command -v local >/dev/null 2>&1; then
      f_loc() {
        # shellcheck disable=SC2034,SC2155,SC2312,SC3043 # deliberate local masking demonstration
        local m="$(exit "${target}")"
        echo "local_masked:$?"
      }
      loc_res=$(f_loc)
    elif command -v typeset >/dev/null 2>&1; then
      f_type() {
        # shellcheck disable=SC2034,SC2155,SC2312,SC3044 # deliberate typeset masking demonstration
        typeset m="$(exit "${target}")"
        echo "typeset_masked:$?"
      }
      loc_res=$(f_type)
    fi

    echo "export_masked:${s_masked_exp} export_unmasked:${s_unmasked_exp} ${loc_res}"
    ;;

  signal_child)
    # Background a real external binary (/bin/sleep) to avoid builtin sleep differences.
    /bin/sleep 30 &
    child_pid="$!"

    # Ensure child process has execve'd to avoid mksh race condition where kill is swallowed before exec.
    waited=0
    while [ "${waited}" -lt 20 ]; do
      if [ -r "/proc/${child_pid}/comm" ]; then
        comm=$(cat "/proc/${child_pid}/comm" 2>/dev/null || true)
        case "${comm}" in
          *sleep*) break ;;
          *) ;;
        esac
      else
        comm=$(ps -o comm= -p "${child_pid}" 2>/dev/null || true)
        case "${comm}" in
          *sleep*) break ;;
          *) ;;
        esac
      fi
      sleep 0.1 2>/dev/null || /bin/sleep 1 2>/dev/null || true
      waited=$((waited + 1))
    done

    # Terminate with SIGTERM
    kill -TERM "${child_pid}" 2>/dev/null || true
    # Suppress shell stderr diagnostic job control message
    wait "${child_pid}" 2>/dev/null
    echo "$?"
    ;;

  repeat_wait)
    target="${2:-42}"
    (exit "${target}") &
    child_pid="$!"
    wait "${child_pid}"
    ret1="$?"
    wait "${child_pid}" 2>/dev/null
    ret2="$?"
    echo "${ret1} ${ret2}"
    ;;

  pipeline_default)
    # Without pipefail, exit status is the rightmost command.
    sh -c 'exit 11' | sh -c 'exit 22' | sh -c 'exit 0'
    echo "$?"
    ;;

  pipeline_pipefail)
    # With pipefail, exit status is the rightmost non-zero exit code.
    # shellcheck disable=SC3040 # test whether optional pipefail is supported
    if (set -o pipefail 2>/dev/null); then
      # shellcheck disable=SC3040 # enable pipefail where supported
      set -o pipefail
      sh -c 'exit 11' | sh -c 'exit 22' | sh -c 'exit 0'
      echo "$?"
    else
      echo "unsupported"
    fi
    ;;

  *)
    echo "Usage: $0 {direct_exit|subshell_exit|cmdsub_exit|notfound_cmd|noexec_cmd|wait_unknown|declaration_mask_trap|signal_child|repeat_wait|pipeline_default|pipeline_pipefail} [arg]" >&2
    exit 2
    ;;
esac
