#!/usr/bin/env bats
# Tests for PIPE_BUF write atomicity, limits, and non-blocking failure modes.

setup() {
  load ../lib/lab
  TOOL="${BATS_TEST_DIRNAME}/pipe_buf_tool.sh"
  SRC="${BATS_TEST_DIRNAME}/probe.c"
  PROBE="${BATS_TEST_TMPDIR}/probe"
}

@test "getconf _POSIX_PIPE_BUF behavior across glibc and BSD or musl implementations" {
  family=$(lab_shell_family)
  case "${family}" in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      ;;
    *)
      false
      ;;
  esac

  # glibc before 2.43 maps _POSIX_PIPE_BUF to pathconf(_PC_PIPE_BUF); 2.43 fixed it (BZ #29147).
  glibc=$(getconf GNU_LIBC_VERSION 2>/dev/null) || glibc=""
  glibc_minor=${glibc#glibc 2.}
  glibc_minor=${glibc_minor%%.*}
  if [ -n "${glibc}" ] && [ "${glibc_minor}" -lt 43 ]; then
    # glibc getconf classifies _POSIX_PIPE_BUF under PATHCONF (_PC_PIPE_BUF) (glibc BZ #29147),
    # so invoking it without a pathname fails and outputs usage instructions.
    run_lab "${TOOL}" getconf-posix-pipe-buf
    [ "${status}" -ne 0 ]
    [[ "${output}" =~ Usage:[[:space:]]*getconf ]]

    # When supplied a path, glibc evaluates pathconf(_PC_PIPE_BUF) and reports the filesystem limit (>= 512).
    run_lab "${TOOL}" getconf-posix-pipe-buf "${BATS_TEST_TMPDIR}"
    [ "${status}" -eq 0 ]
    [ "${output}" -ge 512 ]
  else
    # BSD (macOS), musl (Alpine), and glibc 2.43+ print the <limits.h> minimum 512 without a path,
    # as POSIX getconf requires for the "Minimum Values" names.
    run_lab "${TOOL}" getconf-posix-pipe-buf
    [ "${status}" -eq 0 ]
    [ "${output}" -eq 512 ]
  fi
}

@test "getconf PIPE_BUF is greater than or equal to 512" {
  family=$(lab_shell_family)
  case "${family}" in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      ;;
    *)
      false
      ;;
  esac

  run_lab "${TOOL}" getconf-pipe-buf "${BATS_TEST_TMPDIR}"
  [ "${status}" -eq 0 ]
  [ "${output}" -ge 512 ]
}

@test "probe compiles cleanly and matches getconf limit" {
  run_lab "${TOOL}" probe-compile "${SRC}" "${PROBE}"
  [ "${status}" -eq 0 ]
  [ -x "${PROBE}" ]

  run "${PROBE}" limits "${BATS_TEST_TMPDIR}"
  [ "${status}" -eq 0 ]

  getconf_val=$("${TOOL}" getconf-pipe-buf "${BATS_TEST_TMPDIR}")
  echo "${output}" | grep "FPATHCONF_PIPE_BUF=${getconf_val}"
}

@test "concurrent writes of exactly PIPE_BUF bytes are never torn" {
  if [ ! -x "${PROBE}" ]; then
    run_lab "${TOOL}" probe-compile "${SRC}" "${PROBE}"
    [ "${status}" -eq 0 ]
  fi

  # The runtime limit (512 on macOS, 4096 on Linux), not the 512 floor, so the test sits on the
  # boundary; the 8192-byte test below shows the same detector does catch torn records.
  pipe_buf=$("${TOOL}" getconf-pipe-buf "${BATS_TEST_TMPDIR}")
  run "${PROBE}" concurrent "${pipe_buf}" 300
  [ "${status}" -eq 0 ]
  [ "${output}" = "total=600 corrupted=0" ]
}

@test "concurrent writes of size 8192 bytes exceed PIPE_BUF and tear" {
  if [ ! -x "${PROBE}" ]; then
    run_lab "${TOOL}" probe-compile "${SRC}" "${PROBE}"
    [ "${status}" -eq 0 ]
  fi

  run "${PROBE}" concurrent 8192 300
  [ "${status}" -eq 0 ]
  corrupted=$(echo "${output}" | sed -n 's/.*corrupted=\([0-9]*\).*/\1/p')
  [ "${corrupted}" -gt 0 ]
}

@test "shell built-in concurrent writers to FIFO maintain atomicity for small payloads" {
  family=$(lab_shell_family)
  case "${family}" in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      ;;
    *)
      false
      ;;
  esac

  awks_list=$(lab_awks)
  while IFS= read -r awk_cmd; do
    [ -n "${awk_cmd}" ] || continue
    # shellcheck disable=SC2086 # intentional word splitting for multi-token commands like "busybox awk"
    flavor=$(lab_awk_flavor ${awk_cmd})
    case "${flavor}" in
      gawk | mawk | onetrue | busybox)
        ;;
      *)
        false
        ;;
    esac

    # 200 records per writer overfill the pipe while the reader waits, so writes contend for space.
    AWK="${awk_cmd}" run_lab "${TOOL}" shell-fifo-atomic "${BATS_TEST_TMPDIR}" 500 200
    [ "${status}" -eq 0 ]
    [ "${output}" = "total=400 corrupted=0" ]
  done <<<"${awks_list}"
}

@test "non-blocking write of PIPE_BUF bytes is refused whole where PIPE_BUF + 1 is split" {
  if [ ! -x "${PROBE}" ]; then
    run_lab "${TOOL}" probe-compile "${SRC}" "${PROBE}"
    [ "${status}" -eq 0 ]
  fi

  pipe_buf=$("${TOOL}" getconf-pipe-buf "${BATS_TEST_TMPDIR}")
  run "${PROBE}" nonblock-atomic "${pipe_buf}"
  [ "${status}" -eq 0 ]
  ret=$(echo "${output}" | sed -n 's/.*atomic_ret=\([-0-9]*\).*/\1/p')
  err=$(echo "${output}" | sed -n 's/.*errno=\([0-9]*\).*/\1/p')
  big=$(echo "${output}" | sed -n 's/.*big_ret=\([-0-9]*\).*/\1/p')
  [ "${ret}" -eq -1 ]
  # Linux EAGAIN is 11; macOS/BSD EAGAIN/EWOULDBLOCK is 35
  case "${err}" in
    11 | 35) ;;
    *) false ;;
  esac
  # The larger write found room and took part of it, so the refused write had room too.
  [ "${big}" -gt 0 ]
  [ "${big}" -lt "${pipe_buf}" ]
}

@test "opening FIFO write-only with O_NONBLOCK without reader fails with ENXIO" {
  if [ ! -x "${PROBE}" ]; then
    run_lab "${TOOL}" probe-compile "${SRC}" "${PROBE}"
    [ "${status}" -eq 0 ]
  fi

  fifo="${BATS_TEST_TMPDIR}/nonblock_probe.fifo"
  rm -f "${fifo}"
  mkfifo "${fifo}"

  run "${PROBE}" fifo-nonblock-open "${fifo}"
  rm -f "${fifo}"

  [ "${status}" -eq 0 ]
  [ "${output}" = "fd=-1 errno=6" ]
}
