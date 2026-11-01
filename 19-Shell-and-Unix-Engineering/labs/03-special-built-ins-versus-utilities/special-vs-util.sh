#!/bin/sh
# Probe POSIX.1-2024 special built-in semantics versus regular utilities.
# Usage: special-vs-util.sh <assign|redir-err|util-err|precedence|decl|exec-pid> [args...]
set -u

mode=${1:-assign}

case ${mode} in
  assign)
    V=init
    V=persisted :
    after_special=${V}

    V=init
    V=ephemeral true
    after_regular=${V}

    V=init
    V=suppressed command : 2>/dev/null || :
    after_command=${V}

    printf 'special=%s regular=%s command=%s\n' \
      "${after_special}" "${after_regular}" "${after_command}"
    ;;

  redir-err)
    r_special=$(
      exec 2>/dev/null
      : </nonexistent/no_such_file
      printf 'survived\n'
    )
    r_special=${r_special:-aborted}

    r_regular=$(
      exec 2>/dev/null
      true </nonexistent/no_such_file
      printf 'survived\n'
    )
    r_regular=${r_regular:-aborted}

    r_command=$(
      exec 2>/dev/null
      command : </nonexistent/no_such_file
      printf 'survived\n'
    )
    r_command=${r_command:-aborted}

    printf 'special=%s regular=%s command=%s\n' \
      "${r_special}" "${r_regular}" "${r_command}"
    ;;

  util-err)
    u_special=$(
      exec 2>/dev/null
      # shellcheck disable=SC3041 # intentional invalid option to trigger special built-in error
      set -Z
      printf 'survived\n'
    )
    u_special=${u_special:-aborted}

    u_command=$(
      exec 2>/dev/null
      command set -Z
      printf 'survived\n'
    )
    u_command=${u_command:-aborted}

    printf 'special=%s command=%s\n' "${u_special}" "${u_command}"
    ;;

  precedence)
    f_reg=$(
      true() { printf 'func\n'; }
      true
    )
    f_cmd=failed
    if (
      true() { return 7; }
      command true
    ) >/dev/null 2>&1; then
      f_cmd=bypassed
    fi
    f_spec=$(
      exec 2>/dev/null
      eval 'eval() { printf "func\n"; }' >/dev/null
      eval 'printf "builtin\n"'
    ) || f_spec=builtin

    printf 'regular=%s command_true=%s special=%s\n' \
      "${f_reg}" "${f_cmd}" "${f_spec}"
    ;;

  decl)
    v='two words'
    # shellcheck disable=SC2086,SC2248 # intentional unquoted declaration-utility argument
    export d_var=$v
    # shellcheck disable=SC2086,SC2248 # intentional unquoted regular-utility argument
    set -- d_var=$v
    printf 'export=<%s> regular_argc=%d regular_arg1=<%s>\n' \
      "${d_var}" "$#" "$1"
    ;;

  exec-pid)
    bin=$2
    pidfile=$3
    "${bin}" >"${pidfile}"
    read -r ext_pid <"${pidfile}"
    if [ "${ext_pid}" != "$$" ]; then
      ext_rel=forked
    else
      ext_rel=same
    fi
    printf 'external=%s shell_pid=%s\n' "${ext_rel}" "$$"
    exec "${bin}"
    ;;

  *)
    printf 'unknown mode: %s\n' "${mode}" >&2
    exit 2
    ;;
esac
