#!/bin/sh
# probe.sh - probe set builtin options and positional parameter behavior
# Usage: probe.sh <mode> [args...]

set -e

mode=${1:-}
shift || true

case "$mode" in
  params)
    # Demonstrate positional parameter replacement and clearing with set --
    set -- "$@"
    printf 'count=%d\n' "$#"
    if [ "$#" -gt 0 ]; then
      printf 'first=%s\n' "$1"
    fi
    # Clear all positional parameters
    set --
    printf 'cleared_count=%d\n' "$#"
    # Assign parameters that begin with hyphens safely
    set -- -a -b --long-flag
    printf 'hyphen_count=%d\n' "$#"
    printf 'hyphen_first=%s\n' "$1"
    ;;

  glob-toggle)
    # Demonstrate toggling -f (noglob) on and off
    target_dir=$1
    # shellcheck disable=SC2086,SC2248 # intentional pathname expansion to test globbing
    set -- "$target_dir"/*
    printf 'glob_on_count=%d\n' "$#"
    # Turn off pathname expansion
    set -f
    # shellcheck disable=SC2086,SC2248 # intentional expansion under set -f
    set -- "$target_dir"/*
    printf 'glob_off_count=%d\n' "$#"
    printf 'glob_off_literal=%s\n' "$1"
    # Re-enable pathname expansion
    set +f
    # shellcheck disable=SC2086,SC2248 # intentional expansion under set +f
    set -- "$target_dir"/*
    printf 'glob_restored_count=%d\n' "$#"
    ;;

  save-restore)
    # Demonstrate saving option state via set +o and restoring via eval
    saved=$(set +o)
    set -f
    case "$-" in
      *f*) printf 'during_mutation=f_present\n' ;;
      *) printf 'during_mutation=f_absent\n' ;;
    esac
    eval "$saved"
    case "$-" in
      *f*) printf 'after_restore=f_present\n' ;;
      *) printf 'after_restore=f_absent\n' ;;
    esac
    ;;

  assignment)
    # Demonstrate preceding variable assignment on special built-in
    unset PERSISTED_VAR
    PERSISTED_VAR=persisted_val set --
    printf 'persisted_var=%s\n' "${PERSISTED_VAR:-unset}"
    ;;

  *)
    printf 'unknown mode: %s\n' "$mode" >&2
    exit 1
    ;;
esac
