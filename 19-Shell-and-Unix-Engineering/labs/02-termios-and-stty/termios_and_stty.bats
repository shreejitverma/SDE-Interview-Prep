#!/usr/bin/env bats
# termios_and_stty.bats - Verification suite for termios system calls and stty utility
#
# Every suite runs with no controlling terminal and stdin from /dev/null, so each test that
# needs a terminal builds its own pseudo-terminal through pty_test_helper.
# Every test that goes through run_pty_lab runs its script under the shell under test (LAB_SHELL).

setup() {
  load ../lib/lab
  [ -n "${BATS_TEST_TMPDIR-}" ] || skip "BATS_TEST_TMPDIR not set"

  # Compile C helper with host/matrix C compiler into test tmpdir
  "${CC:-cc}" -Wall -Wextra -Werror -pedantic -std=c99 \
    -o "${BATS_TEST_TMPDIR}/pty_test_helper" \
    "${BATS_TEST_DIRNAME}/pty_test_helper.c"
}

# run_pty_lab MODE SCRIPT [ARG...]: run SCRIPT with the shell under test (like `sh -c`) as the
# session leader of a fresh pty; MODE is pty-exec (24x80 window) or pty-exec-nosize (0x0 window).
# The script sees "$0" = sh, then ARG... in "$1"..., and after them the words of the shell under
# test, so a script that does `shift` per ARG can run `"$@" file.sh` to start another copy of it.
run_pty_lab() {
  local mode="$1" script="$2"
  shift 2
  lab_shell_words
  run "${BATS_TEST_TMPDIR}/pty_test_helper" "${mode}" "${LAB_SHELL_WORDS[@]}" -c "${script}" sh "$@" "${LAB_SHELL_WORDS[@]}"
}

# stty_flavor: print gnu, busybox, or bsd for the stty found on PATH (each prints a different
# usage text; BSD stty has no --help and rejects it).
stty_flavor() {
  local help
  help=$(stty --help 2>&1 || true)
  case "${help}" in
    *BusyBox*) echo busybox ;;
    *"Usage: stty [-F DEVICE"*) echo gnu ;;
    *) echo bsd ;;
  esac
}

@test "tcgetattr and basic terminal attributes succeed via termios C API" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" termios-probe
  [ "$status" -eq 0 ]
  [[ "$output" =~ "tcgetattr=OK" ]]
  [[ "$output" =~ "tcsanow_echo_off=OK" ]]
  [[ "$output" =~ "tcflush=OK" ]]
  [[ "$output" =~ "tcflow=OK" ]]
}

@test "TCSAFLUSH flushes pending unread input before applying termios changes" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" verify-tcsaflush
  [ "$status" -eq 0 ]
  [[ "$output" =~ "tcsaflush_discarded=1 accepted_read=ACCEPTED" ]]
}

@test "background process group attempting tcsetattr receives SIGTTOU and the call fails with EINTR" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" verify-sigttou
  [ "$status" -eq 0 ]
  [ "$output" = "CAUGHT_SIGTTOU=1 tcsetattr=FAIL errno=EINTR changed=0" ]
}

@test "background process group ignoring SIGTTOU changes the terminal without any signal" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" verify-sigttou-ignored
  [ "$status" -eq 0 ]
  [ "$output" = "CAUGHT_SIGTTOU=0 tcsetattr=OK errno=none changed=1" ]
}

@test "background process group blocking SIGTTOU changes the terminal without any signal" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" verify-sigttou-blocked
  [ "$status" -eq 0 ]
  [ "$output" = "CAUGHT_SIGTTOU=0 tcsetattr=OK errno=none changed=1" ]
}

@test "closing the last pty master makes termios calls on the slave fail on Linux but not on macOS" {
  run "${BATS_TEST_TMPDIR}/pty_test_helper" verify-hangup
  [ "$status" -eq 0 ]
  case "$(uname -s)" in
    Linux) [ "$output" = "master_closed tcgetattr=EIO tcsetattr=EIO" ] ;;
    Darwin) [ "$output" = "master_closed tcgetattr=OK tcsetattr=OK" ] ;;
    *)
      kernel=$(uname -s)
      printf 'kernel not measured: %s\n' "$kernel" >&2
      false
      ;;
  esac
}

@test "stty on non-terminal descriptor exits 1 with the diagnostic of the installed stty" {
  # The expected diagnostics are the C-locale texts: under a UTF-8 locale GNU stty quotes with U+2018 and U+2019.
  LC_ALL=C run_lab "${BATS_TEST_DIRNAME}/stty_ops.sh" save </dev/null
  [ "$status" -eq 1 ]
  case "$(stty_flavor)" in
    gnu) [ "$output" = "stty: 'standard input': Inappropriate ioctl for device" ] ;;
    busybox) [ "$output" = "stty: standard input: Not a tty" ] ;;
    bsd) [ "$output" = "stty: stdin isn't a terminal" ] ;;
    *) false ;;
  esac
}

@test "stty -g saves and restores attributes idempotently on a PTY" {
  local ops="${BATS_TEST_DIRNAME}/stty_ops.sh"
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec '
    ops=$1
    shift
    saved=$("$@" "$ops" save)
    stty -echo
    before=$("$@" "$ops" query-flags)
    "$@" "$ops" restore "${saved}"
    after=$("$@" "$ops" query-flags)
    again=$("$@" "$ops" save)
    if [ "${again}" = "${saved}" ]; then same=1; else same=0; fi
    printf "BEFORE:%s AFTER:%s SAME_STATE:%s\n" "${before}" "${after}" "${same}"
  ' "${ops}"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "BEFORE:echo=0" ]]
  [[ "$output" =~ "AFTER:echo=1" ]]
  [[ "$output" =~ "SAME_STATE:1" ]]
}

@test "stty sane restores canonical mode echo and onlcr" {
  local ops="${BATS_TEST_DIRNAME}/stty_ops.sh"
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec '
    ops=$1
    shift
    stty -echo -icanon -onlcr
    "$@" "$ops" sane
    "$@" "$ops" query-flags
  ' "${ops}"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "echo=1 icanon=1 onlcr=1 isig=1" ]]
}

@test "with-noecho executes command with echo off and restores state on exit" {
  local ops="${BATS_TEST_DIRNAME}/stty_ops.sh"
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec '
    ops=$1
    shift
    "$@" "$ops" with-noecho "$@" "$ops" query-flags
    "$@" "$ops" query-flags
  ' "${ops}"
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 2 ]
  [[ "${lines[0]}" =~ "echo=0 icanon=1 onlcr=1 isig=1" ]]
  [[ "${lines[1]}" =~ "echo=1 icanon=1 onlcr=1 isig=1" ]]
}

@test "subshell modification of termios alters parent terminal description" {
  local ops="${BATS_TEST_DIRNAME}/stty_ops.sh"
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec '
    ops=$1
    shift
    (stty -echo)
    "$@" "$ops" query-flags
  ' "${ops}"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "echo=0" ]]
}

@test "stty reports an invalid operand with exit status 1 and the diagnostic of the installed stty" {
  # The expected diagnostics are the C-locale texts: under a UTF-8 locale GNU stty quotes with U+2018 and U+2019.
  run env LC_ALL=C "${BATS_TEST_TMPDIR}/pty_test_helper" pty-exec stty invalid_operand_probe_xyz
  [ "$status" -eq 1 ]
  first=${lines[0]//$'\r'/}
  case "$(stty_flavor)" in
    gnu) [ "$first" = "stty: invalid argument 'invalid_operand_probe_xyz'" ] ;;
    busybox) [ "$first" = "stty: invalid number 'invalid_operand_probe_xyz'" ] ;;
    bsd) [ "$first" = "stty: illegal option -- invalid_operand_probe_xyz" ] ;;
    *) false ;;
  esac
}

@test "stty -a on a pty with a 0x0 window size: BusyBox warns on stderr, others stay silent, all exit 0" {
  local errfile="${BATS_TEST_TMPDIR}/stty.err"
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec-nosize 'stty -a >/dev/null 2>"$1"; echo "STTY_EXIT:$?"' "${errfile}"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "STTY_EXIT:0" ]]
  stty_help=$(stty --help 2>&1 || true)
  if printf '%s\n' "$stty_help" | grep -q BusyBox; then
    grep -q '^stty: standard input' "${errfile}"
  else
    [ ! -s "${errfile}" ]
  fi
}

@test "stty -g string layout and stty size on a 24x80 pty match the installed stty" {
  # shellcheck disable=SC2016 # expressions expand in the shell under test
  run_pty_lab pty-exec 'stty -g; stty size'
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 2 ]
  saved=${lines[0]//$'\r'/}
  [ "${lines[1]//$'\r'/}" = "24 80" ]
  case "$(stty_flavor)" in
    gnu | busybox)
      # iflag:oflag:cflag:lflag then 32 control characters (NCCS on Linux), all lowercase hex
      [[ "$saved" =~ ^500:5:bf:8a3b(:[0-9a-f]+){32}$ ]]
      ;;
    bsd)
      [[ "$saved" =~ ^gfmt1:cflag=4b00:iflag=2b02:lflag=5cb:oflag=3:discard=f:dsusp=19:eof=4: ]]
      [[ "$saved" =~ :ispeed=9600:ospeed=9600$ ]]
      ;;
    *) false ;;
  esac
}
