#!/bin/sh
# probe.sh: instrument subshell mechanics and execution environment isolation.
# Exercises environment isolation, special parameters ($$ and $PPID),
# trap resets, pipeline subshell divergence, open file description offset sharing,
# file status flags leakage, and subshell exit status propagation.

set -eu

usage() {
  printf 'usage: probe.sh <mode> [scratch_dir]\n' >&2
  exit 1
}

check_noglob() {
  case $- in
    *f*) printf 'on' ;;
    *) printf 'off' ;;
  esac
}

norm_umask() {
  case $(umask) in
    *77) printf '0077' ;;
    *) umask ;;
  esac
}

probe_env_isolation() {
  scratch="${1:-.}"
  var="initial"
  orig_pwd=$(pwd)
  orig_umask=$(norm_umask)
  # shellcheck disable=SC2030 # intentional test of subshell variable isolation
  (
    var="subshell"
    cd "$scratch"
    umask 077
    set -f
    sub_umask=$(norm_umask)
    sub_noglob=$(check_noglob)
    printf 'sub: var=%s umask=%s noglob=%s\n' "$var" "$sub_umask" "$sub_noglob"
  )
  cur_pwd=$(pwd)
  in_orig=0
  if [ "$cur_pwd" = "$orig_pwd" ]; then
    in_orig=1
  fi
  cur_umask=$(norm_umask)
  same_umask=0
  if [ "$cur_umask" = "$orig_umask" ]; then
    same_umask=1
  fi
  caller_noglob=$(check_noglob)
  # shellcheck disable=SC2031 # intentional assertion that subshell mutations did not leak
  printf 'caller_after_sub: var=%s in_orig=%d same_umask=%d noglob=%s\n' \
    "$var" "$in_orig" "$same_umask" "$caller_noglob"
  {
    var="current"
  }
  printf 'caller_after_brace: var=%s\n' "$var"
}

probe_special_params() {
  p_pid=$$
  p_ppid=$PPID
  (
    same_pid=0
    same_ppid=0
    if [ "$$" = "$p_pid" ]; then
      same_pid=1
    fi
    if [ "$PPID" = "$p_ppid" ]; then
      same_ppid=1
    fi
    printf 'sub_params: same_pid=%d same_ppid=%d\n' "$same_pid" "$same_ppid"
  )
}

probe_trap_reset() {
  # Trap EXIT in parent shell
  trap 'printf "main_exit\n"' EXIT
  # A subshell with no trap of its own must not run the parent's EXIT trap when it exits
  (
    printf 'inside_plain\n'
  )
  # A subshell may set its own EXIT trap, which fires at subshell exit without touching the parent
  (
    trap 'printf "sub_exit\n"' EXIT
    printf 'inside_own\n'
  )
  printf 'after_sub\n'
}

probe_pipeline_isolation() {
  scratch="${1:-.}"
  input="$scratch/input.txt"
  printf 'one\ntwo\nthree\n' >"$input"

  # Redirected compound loop executes in the current shell across all POSIX shells
  c1=0
  while IFS= read -r line; do
    if [ -n "$line" ]; then
      c1=$((c1 + 1))
    fi
  done <"$input"

  # Pipelined compound loop executes in a subshell in most shells, but current shell in ksh93/zsh
  c2=0
  # shellcheck disable=SC2002 # intentional pipeline test to inspect subshell isolation
  cat "$input" | while IFS= read -r line; do
    if [ -n "$line" ]; then
      # shellcheck disable=SC2030 # intentional test of pipeline subshell variable isolation
      c2=$((c2 + 1))
    fi
  done

  printf 'redir_count: %d\n' "$c1"
  # shellcheck disable=SC2031 # intentional assertion that pipeline subshell modification was lost
  printf 'pipe_count: %d\n' "$c2"
}

probe_fd_offset_sharing() {
  scratch="${1:-.}"
  shared="$scratch/shared_offset.txt"
  printf '12345678\n' >"$shared"

  # Open shared file description on descriptor 3
  exec 3<"$shared"

  # Subshell consumes first 4 bytes from fd 3
  (dd bs=4 count=1 <&3 >/dev/null 2>&1)

  # Parent reads next 4 bytes from fd 3; proves file offset was advanced in open file description
  part2=$(dd bs=4 count=1 <&3 2>/dev/null)
  exec 3<&-

  printf 'shared_offset: %s\n' "$part2"
}

probe_fd_flags_sharing() {
  scratch="${1:-.}"
  src_dir="${2:-$(dirname "$0")}"
  bin_c="$scratch/subshell_c"
  src_c="$src_dir/subshell_c.c"
  if [ ! -f "$bin_c" ]; then
    "${CC:-cc}" -Wall -Wextra -Werror "$src_c" -o "$bin_c"
  fi

  target_file="$scratch/target_flags.txt"
  printf 'data\n' >"$target_file"

  exec 3<"$target_file"
  "$bin_c" 3 3<&3
  ("$bin_c" 3 s 3<&3)
  "$bin_c" 3 3<&3
  exec 3<&-
}

probe_status_propagation() {
  st=0
  (exit 42) || st=$?
  printf 'sub_status: %d\n' "$st"
}

[ "$#" -ge 1 ] || usage
mode="$1"
shift

case "$mode" in
  env_isolation)
    probe_env_isolation "${1:-.}"
    ;;
  special_params)
    probe_special_params
    ;;
  trap_reset)
    probe_trap_reset
    ;;
  pipeline_isolation)
    probe_pipeline_isolation "${1:-.}"
    ;;
  fd_offset_sharing)
    probe_fd_offset_sharing "${1:-.}"
    ;;
  fd_flags_sharing)
    probe_fd_flags_sharing "${1:-.}" "${2:-}"
    ;;
  status_propagation)
    probe_status_propagation
    ;;
  *)
    usage
    ;;
esac
