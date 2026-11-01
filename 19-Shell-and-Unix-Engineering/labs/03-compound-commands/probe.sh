#!/bin/sh
# probe.sh: instrument compound command mechanics in the lab matrix.
# Exercises grouping, environment isolation, redirection scope, exit status,
# lazy case evaluation, fall-through (;&), pipeline subshell divergence, and fd inheritance.

set -eu

usage() {
  printf 'usage: probe.sh <mode> [scratch_dir]\n' >&2
  exit 1
}

probe_redir_scope() {
  scratch="${1:-.}"
  out="$scratch/out.txt"
  {
    printf 'line1\n'
    printf 'line2\n'
  } >"$out"
  printf 'after\n'
  cat "$out"
}

probe_env_isolation() {
  scratch="${1:-.}"
  var="initial"
  orig_pwd=$(pwd)
  # shellcheck disable=SC2030 # intentional test of subshell variable isolation
  (
    var="subshell"
    cd "$scratch"
    printf 'sub: var=%s\n' "$var"
  )
  cur_pwd=$(pwd)
  in_orig=0
  if [ "$cur_pwd" = "$orig_pwd" ]; then
    in_orig=1
  fi
  # shellcheck disable=SC2031 # intentional assertion that subshell change did not leak
  printf 'caller_after_sub: var=%s in_orig=%d\n' "$var" "$in_orig"
  {
    var="current"
  }
  printf 'caller_after_brace: var=%s\n' "$var"
}

probe_case_lazy() {
  scratch="${1:-.}"
  marker="$scratch/marker.txt"
  rm -f "$marker"
  val="target"
  case "$val" in
    target)
      printf 'matched_clause1\n'
      ;;
    "$(
      touch "$marker"
      printf 'target\n'
    )")
      printf 'matched_clause2\n'
      ;;
    *)
      printf 'unmatched\n'
      ;;
  esac
  if [ -f "$marker" ]; then
    printf 'marker: created\n'
  else
    printf 'marker: absent\n'
  fi
}

probe_case_fallthrough() {
  # Feature-detect ;& using eval inside a subshell to avoid fatal parse errors on dash/bash 3.2
  if (eval 'case a in a) : ;& b) : ;; esac') 2>/dev/null; then
    eval '
      x=0; y=1
      case 1 in
        $((y=0))) ;;
        $((x=1))) ;&
        $((x=2))) printf "fallthrough: %d.%d\n" "$x" "$y" ;;
        *) ;;
      esac
    '
  else
    printf 'fallthrough: unsupported\n'
  fi
}

probe_loop_status() {
  set +e
  # shellcheck disable=SC2034 # loop variable unused by design in empty iteration test
  for x in; do printf 'unreached\n'; done
  st_for=$?

  while false; do printf 'unreached\n'; done
  st_while_empty=$?

  until true; do printf 'unreached\n'; done
  st_until_empty=$?

  if false; then printf 'unreached\n'; fi
  st_if_empty=$?

  val="nomatch"
  case "$val" in
    match) printf 'unreached\n' ;;
    *) ;;
  esac
  st_case_empty=$?

  i=0
  while [ "$i" -lt 1 ]; do
    i=$((i + 1))
    (exit 42)
  done
  st_while_body=$?
  set -e

  printf 'for_empty: %d\n' "$st_for"
  printf 'while_empty: %d\n' "$st_while_empty"
  printf 'until_empty: %d\n' "$st_until_empty"
  printf 'if_empty: %d\n' "$st_if_empty"
  printf 'case_empty: %d\n' "$st_case_empty"
  printf 'while_body: %d\n' "$st_while_body"
}

probe_pipeline_scope() {
  scratch="${1:-.}"
  input="$scratch/input.txt"
  printf 'one\ntwo\nthree\n' >"$input"

  # Redirected compound command: executes in current shell across all POSIX shells
  c1=0
  while IFS= read -r line; do
    if [ -n "$line" ]; then
      c1=$((c1 + 1))
    fi
  done <"$input"

  # Pipelined compound command: subshell in most shells, current shell in ksh93/zsh
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

probe_fd_inheritance() {
  scratch="${1:-.}"
  out="$scratch/bg_out.txt"
  fifo="$scratch/sync_fifo"
  rm -f "$out" "$fifo"
  mkfifo "$fifo"

  {
    {
      # Wait on synchronization byte from parent before writing to inherited stdout
      cat <"$fifo" >/dev/null
      printf 'from_bg\n'
    } &
    bg_pid=$!
    printf 'from_main\n'
  } >"$out"

  # The compound command completed and main stdout was restored, but bg_pid retains fd
  printf 'go\n' >"$fifo"
  wait "$bg_pid"

  cat "$out"
  rm -f "$fifo" "$out"
}

[ "$#" -ge 1 ] || usage
mode="$1"
shift

case "$mode" in
  redir_scope)
    probe_redir_scope "${1:-.}"
    ;;
  env_isolation)
    probe_env_isolation "${1:-.}"
    ;;
  case_lazy)
    probe_case_lazy "${1:-.}"
    ;;
  case_fallthrough)
    probe_case_fallthrough
    ;;
  loop_status)
    probe_loop_status
    ;;
  pipeline_scope)
    probe_pipeline_scope "${1:-.}"
    ;;
  fd_inheritance)
    probe_fd_inheritance "${1:-.}"
    ;;
  *)
    usage
    ;;
esac
