#!/bin/sh
# Probes shell process creation invariants: $$ persistence, exit codes,
# environment isolation, file descriptor inheritance, and signal masks.

set -eu

probe_pid_subshell() {
  parent_pid=$$
  # shellcheck disable=SC2116 # command substitution is intentionally tested here
  subshell_pid=$(echo "$$")
  subshell_nested=$( (echo "$$") )
  if [ "$parent_pid" = "$subshell_pid" ] && [ "$parent_pid" = "$subshell_nested" ]; then
    printf 'pid_match: %s\n' "$parent_pid"
  else
    printf 'pid_mismatch: %s vs %s vs %s\n' "$parent_pid" "$subshell_pid" "$subshell_nested" >&2
    return 1
  fi
}

probe_exit_codes() {
  target_dir=$1

  # 1. Nonexistent utility must exit 127 (POSIX XCU 2.8.2)
  set +e
  "$target_dir/nonexistent_utility_binary_probe" 2>/dev/null
  code_missing=$?
  set -e

  # 2. File found but not executable must exit 126 (POSIX XCU 2.8.2)
  noexec_file="$target_dir/unexecutable_probe.txt"
  printf 'echo unexecutable\n' >"$noexec_file"
  chmod 000 "$noexec_file"
  set +e
  "$noexec_file" 2>/dev/null
  code_noexec=$?
  set -e

  printf 'missing:%d noexec:%d\n' "$code_missing" "$code_noexec"
}

probe_env_isolation() {
  # shellcheck disable=SC2030,SC2031 # intentional subshell isolation test
  export SPAWN_TEST_VAR="initial_parent_value"
  # Run a child shell or subshell that modifies SPAWN_TEST_VAR
  (
    # shellcheck disable=SC2030,SC2031 # intentional subshell isolation test
    SPAWN_TEST_VAR="mutated_child_value"
    export SPAWN_TEST_VAR
  )
  # shellcheck disable=SC2031 # verify parent value is retained
  printf 'parent_val:%s\n' "$SPAWN_TEST_VAR"
}

probe_fd_inheritance() {
  target_file=$1
  # Open file descriptor 3 pointing to target_file, then exec an external child that writes to it.
  # The child sees fd 3 only if the shell left FD_CLOEXEC clear on it.
  exec 3>"$target_file"
  # shellcheck disable=SC2016 # the redirection belongs to the child shell
  if ! /bin/sh -c 'printf "inherited_write\n" >&3' 2>/dev/null; then
    printf 'not_inherited\n' >"$target_file"
  fi
  exec 3>&-
  cat "$target_file"
}

mode=${1:-}
shift || true

case "$mode" in
  pid-subshell)
    probe_pid_subshell
    ;;
  exit-codes)
    probe_exit_codes "$@"
    ;;
  env-isolation)
    probe_env_isolation
    ;;
  fd-inheritance)
    probe_fd_inheritance "$@"
    ;;
  *)
    printf 'Usage: %s {pid-subshell|exit-codes <dir>|env-isolation|fd-inheritance <file>}\n' "$0" >&2
    exit 2
    ;;
esac
