#!/bin/sh
# pipe_buf_tool.sh: Utility for probing and verifying PIPE_BUF write atomicity.
set -eu

usage() {
  printf 'usage: %s <command> [args...]\n' "${0}" >&2
  printf 'commands:\n' >&2
  printf '  getconf-pipe-buf [path]\n' >&2
  printf '  getconf-posix-pipe-buf [path]\n' >&2
  printf '  probe-compile <src_c> <dest_bin>\n' >&2
  printf '  shell-fifo-atomic <fifo_dir> <payload_size> <count>\n' >&2
  exit 1
}

cmd_getconf_pipe_buf() {
  target_path="${1:-.}"
  pipe_buf=$(getconf PIPE_BUF "${target_path}")
  printf '%s\n' "${pipe_buf}"
}

cmd_getconf_posix_pipe_buf() {
  target_path="${1-}"
  if [ -n "${target_path}" ]; then
    posix_val=$(getconf _POSIX_PIPE_BUF "${target_path}")
  else
    posix_val=$(getconf _POSIX_PIPE_BUF)
  fi
  printf '%s\n' "${posix_val}"
}

cmd_probe_compile() {
  src="${1:?missing source C file}"
  dest="${2:?missing destination binary}"
  cc_cmd="${CC:-cc}"
  "${cc_cmd}" -Wall -Wextra -Werror -pedantic -std=c99 "${src}" -o "${dest}"
}

cmd_shell_fifo_atomic() {
  fifo_dir="${1:?missing fifo dir}"
  payload_size="${2:?missing payload size}"
  count="${3:?missing count}"

  fifo="${fifo_dir}/atomic.fifo"
  out="${fifo_dir}/atomic.out"
  rm -f "${fifo}" "${out}"
  mkfifo "${fifo}"

  # awk reader checks whether any single line contains both A and B
  # Split a multi-word awk ("busybox awk") with eval: native zsh does not field-split ${AWK},
  # and a reader that fails to start would leave the writer blocked opening the FIFO.
  eval "set -- ${AWK:-awk}"
  # The reader opens the FIFO, then waits a second before reading, so the writers fill the
  # pipe and block: every later write then contends for space, where a torn record would show.
  {
    sleep 1
    # shellcheck disable=SC2016 # the awk program is single-quoted on purpose
    "$@" '{
      if ($0 ~ /A/ && $0 ~ /B/) {
        corrupted++
      }
      total++
    } END {
      printf "total=%d corrupted=%d\n", total, (corrupted ? corrupted : 0)
    }'
  } <"${fifo}" >"${out}" &
  reader_pid=$!

  # Generate payload strings of exact length (payload_size - 1 chars + newline = payload_size bytes)
  char_count=$((payload_size - 1))
  payload_a=$(printf "%${char_count}s" "" | tr ' ' 'A')
  payload_b=$(printf "%${char_count}s" "" | tr ' ' 'B')

  # Subshell opens FIFO once for write; concurrent child jobs inherit fd 1
  (
    (
      i=0
      while [ "${i}" -lt "${count}" ]; do
        printf '%s\n' "${payload_a}"
        i=$((i + 1))
      done
    ) &
    w1_pid=$!

    (
      i=0
      while [ "${i}" -lt "${count}" ]; do
        printf '%s\n' "${payload_b}"
        i=$((i + 1))
      done
    ) &
    w2_pid=$!

    wait "${w1_pid}" "${w2_pid}"
  ) >"${fifo}"

  wait "${reader_pid}"
  cat "${out}"
  rm -f "${fifo}" "${out}"
}

main() {
  if [ "$#" -lt 1 ]; then
    usage
  fi

  subcmd="${1}"
  shift

  case "${subcmd}" in
    getconf-pipe-buf)
      cmd_getconf_pipe_buf "${1:-.}"
      ;;
    getconf-posix-pipe-buf)
      cmd_getconf_posix_pipe_buf ${1+"$@"}
      ;;
    probe-compile)
      cmd_probe_compile ${1+"$@"}
      ;;
    shell-fifo-atomic)
      cmd_shell_fifo_atomic ${1+"$@"}
      ;;
    *)
      usage
      ;;
  esac
}

main "$@"
