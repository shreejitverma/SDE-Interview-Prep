#!/bin/sh
# task_runner.sh - Deterministic POSIX task runner demonstrating function scope,
# parameter stack isolation, subshell environments, and recursion boundaries.

set -eu

# Global configuration state
RUNNER_MAX_DEPTH=5
RUNNER_CALL_COUNT=0
RUNNER_ENV_STATE="pristine"

# Redirection bound directly to function definition (evaluated each invocation)
log_diag() {
  printf "DIAG: %s\n" "$*"
} >&2

# Subshell compound command as function body: isolates variable mutations and cwd
# shellcheck disable=SC2030 # intentional subshell isolation demonstration
run_sandboxed() (
  _rs_var="mutated_inside_subshell"
  RUNNER_ENV_STATE="${_rs_var}"
  cd "$1"
  printf "SANDBOX_CWD=%s\n" "${PWD}"
  printf "SANDBOX_STATE=%s\n" "${RUNNER_ENV_STATE}"
)

# Recursive worker demonstrating function call frame and depth bounding
walk_depth() {
  _wd_target="$1"
  _wd_current="$2"

  RUNNER_CALL_COUNT=$((RUNNER_CALL_COUNT + 1))
  if [ "${_wd_current}" -ge "${RUNNER_MAX_DEPTH}" ]; then
    log_diag "Maximum recursion depth reached: ${_wd_current}"
    return 2
  fi

  if [ "${_wd_current}" -eq "${_wd_target}" ]; then
    printf "RECURSION_TARGET_REACHED=%d\n" "${_wd_current}"
    return 0
  fi

  walk_depth "${_wd_target}" "$((_wd_current + 1))"
}

# Worker demonstrating positional parameter replacement and shift isolation
process_batch() {
  _pb_count=0
  # Shifting inside a function consumes function arguments, not caller arguments
  while [ "$#" -gt 0 ]; do
    _pb_item="$1"
    _pb_count=$((_pb_count + 1))
    printf "BATCH_ITEM_%d=%s\n" "${_pb_count}" "${_pb_item}"
    shift
  done
  return 0
}

# Safe command execution: separate assignment to prevent masking exit status
safe_eval_task() {
  _st_cmd="$1"
  _st_out=""
  _st_code=0
  if _st_out=$(eval "${_st_cmd}"); then
    _st_code=0
  else
    _st_code=$?
  fi
  printf "TASK_CODE=%d\n" "${_st_code}"
  printf "TASK_OUTPUT=%s\n" "${_st_out}"
  return "${_st_code}"
}

main() {
  if [ "$#" -lt 1 ]; then
    printf "Usage: %s <run|sandbox|depth|batch|safe> [args...]\n" "$0" >&2
    exit 1
  fi

  _action="$1"
  shift

  case "${_action}" in
    run)
      # Positional parameters before and after function call
      _orig_params="$*"
      process_batch "$@"
      _caller_match="false"
      if [ "$*" = "${_orig_params}" ]; then
        _caller_match="true"
      fi
      printf "CALLER_PARAMS_PRESERVED=%s\n" "$*"
      printf "CALLER_MATCH=%s\n" "${_caller_match}"
      ;;
    sandbox)
      _target_dir="${1:-/tmp}"
      _orig_dir="${PWD}"
      run_sandboxed "${_target_dir}"
      _dir_match="false"
      if [ "${PWD}" = "${_orig_dir}" ]; then
        _dir_match="true"
      fi
      printf "PARENT_CWD=%s\n" "${PWD}"
      # shellcheck disable=SC2031 # verifying subshell mutation did not leak to parent
      printf "PARENT_STATE=%s\n" "${RUNNER_ENV_STATE}"
      printf "CWD_PRESERVED=%s\n" "${_dir_match}"
      ;;
    depth)
      _target_depth="${1:-3}"
      walk_depth "${_target_depth}" 0
      printf "TOTAL_CALLS=%d\n" "${RUNNER_CALL_COUNT}"
      ;;
    batch)
      process_batch "$@"
      ;;
    safe)
      safe_eval_task "$1"
      ;;
    *)
      printf "Unknown command: %s\n" "${_action}" >&2
      exit 1
      ;;
  esac
}

main "$@"
