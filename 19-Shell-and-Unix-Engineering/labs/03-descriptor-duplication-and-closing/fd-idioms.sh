#!/bin/sh
# fd-idioms: run the classic descriptor idioms against a noisy command and report what each stream received.
# Usage: fd-idioms.sh MODE DIR
#   direct       - noisy, no extra redirection (baseline)
#   swap         - noisy 3>&1 1>&2 2>&3 3>&-            (stdout and stderr exchanged)
#   prefix       - tag every stderr line with "E: ", leave stdout alone, keep noisy's exit status
#   merge-first  - noisy >merged 2>&1                  (both streams reach the file)
#   merge-last   - noisy 2>&1 >merged                  (only stdout reaches the file)
# DIR must exist; the script (re)creates DIR/stdout, DIR/stderr, DIR/status and DIR/merged and removes nothing.
# Output: stdout=[...] stderr=[...] status=N merged=[...], newlines inside a stream shown as commas.

noisy() {
  echo out
  echo err >&2
  return 3
}

# prefix_stderr PREFIX CMD [ARG...]: stdout passes through, stderr is prefixed, the status is CMD's.
# Descriptor 3 holds the real stdout and 4 carries the status past the pipe;
# the command gets neither of them as an extra open descriptor.
prefix_stderr() {
  pfx=$1
  shift
  exec 3>&1
  # shellcheck disable=SC2312 # the pipeline status is sed's by design; the command's own status travels on descriptor 4
  st=$(
    {
      {
        "$@" 2>&1 1>&3 3>&- 4>&-
        echo "$?" >&4
      } | sed "s/^/$pfx/" >&2
    } 4>&1
  )
  exec 3>&-
  return "$st"
}

# merge_last: noisy 2>&1 >merged - stderr is duplicated before stdout moves, so only stdout reaches the file.
merge_last() {
  # shellcheck disable=SC2069 # the order is the point of this mode
  noisy 2>&1 >"$dir/merged"
}

show() {
  printf '%s=[' "$1"
  if [ -f "$dir/$1" ]; then
    tr '\n' ',' <"$dir/$1"
  fi
  printf '] '
}

mode=${1:?usage: fd-idioms.sh MODE DIR}
dir=${2:?usage: fd-idioms.sh MODE DIR}

: >"$dir/merged"
(
  case $mode in
    direct) noisy ;;
    swap) noisy 3>&1 1>&2 2>&3 3>&- ;;
    prefix) prefix_stderr "E: " noisy ;;
    merge-first) noisy >"$dir/merged" 2>&1 ;;
    merge-last) merge_last ;;
    *)
      echo "fd-idioms: unknown mode: $mode" >&2
      exit 64
      ;;
  esac
  echo "$?" >"$dir/status"
) >"$dir/stdout" 2>"$dir/stderr"
rc=$?
if [ "$rc" -ne 0 ]; then
  cat "$dir/stderr" >&2
  exit "$rc"
fi

show stdout
show stderr
code=$(cat "$dir/status")
printf 'status=%s ' "$code"
show merged
printf '\n'
