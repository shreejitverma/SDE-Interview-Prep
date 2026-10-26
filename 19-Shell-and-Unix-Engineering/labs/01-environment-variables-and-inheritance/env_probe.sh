#!/bin/sh
# env_probe.sh: probe shell variables vs exported environment variables.
# Usage:
#   env_probe.sh dump <varname>
#   env_probe.sh prefix <varname>
#   env_probe.sh child_mutate <varname> <newval>

set -eu

mode="${1:-}"

case "${mode}" in
  dump)
    varname="${2:-}"
    if [ -z "${varname}" ]; then
      printf 'error: missing variable name\n' >&2
      exit 1
    fi
    shell_val="UNSET"
    # Read shell variable value portably across POSIX shells
    # shellcheck disable=SC2154 # assigned dynamically via eval
    eval "shell_val=\${${varname}-UNSET}"
    # Read exported environment value directly from the env utility
    env_out=$(
      export PATH="${PATH:-/usr/bin:/bin}"
      env
    )
    env_val=$(printf '%s\n' "${env_out}" | awk -F= -v k="${varname}" '
      $1 == k {
        sub(/^[^=]*=/, "")
        print
        found = 1
        exit
      }
      END {
        if (!found) print "UNSET"
      }
    ')
    printf 'shell: %s\n' "${shell_val}"
    printf 'env: %s\n' "${env_val}"
    ;;

  prefix)
    varname="${2:-}"
    if [ -z "${varname}" ]; then
      printf 'error: missing variable name\n' >&2
      exit 1
    fi
    val="UNSET"
    # shellcheck disable=SC2154 # assigned dynamically via eval
    eval "val=\${${varname}-UNSET}"
    printf 'prefix_val: %s\n' "${val}"
    ;;

  child_mutate)
    varname="${2:-}"
    newval="${3:-}"
    if [ -z "${varname}" ]; then
      printf 'error: missing variable name\n' >&2
      exit 1
    fi
    # shellcheck disable=SC2034 # referenced in subsequent eval
    mutate_target="${newval}"
    eval "${varname}=\${mutate_target}"
    # shellcheck disable=SC2163 # intentional indirect export of variable named in varname
    export "${varname}"
    val="UNSET"
    # shellcheck disable=SC2154 # assigned dynamically via eval
    eval "val=\${${varname}}"
    printf 'child_set: %s\n' "${val}"
    ;;

  *)
    printf 'usage: env_probe.sh {dump|prefix|child_mutate} [args...]\n' >&2
    exit 1
    ;;
esac
