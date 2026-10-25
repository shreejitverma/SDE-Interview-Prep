#!/bin/sh
# startup-probe.sh: inspect argv boundaries, argc, $0, and environment variables.
# Used by labs/01-program-startup-argv-envp-and-arg_max/test.bats.

printf 'cmd: <%s>\n' "$0"
printf 'argc: %d\n' "$#"

printf 'args:'
if [ "$#" -gt 0 ]; then
  for arg in "$@"; do
    printf ' <%s>' "${arg}"
  done
fi
printf '\n'

printf 'env: <%s>\n' "${STARTUP_PROBE_ENV:-UNSET}"
