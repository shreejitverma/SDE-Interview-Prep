#!/usr/bin/env bats
# Tests for TTY line discipline mechanisms and terminal interactions.

setup() {
  load ../lib/lab
  SCRIPT="${BATS_TEST_DIRNAME}/ldisc_probe.sh"
  PROBE="${BATS_TEST_TMPDIR}/pty_ldisc_probe"
  if [ ! -x "${PROBE}" ]; then
    "${CC:-cc}" -O2 -Wall -Wextra -Werror "${BATS_TEST_DIRNAME}/pty_ldisc_probe.c" -o "${PROBE}"
  fi
  export PROBE
}

@test "line discipline defaults to N_TTY (0) via TIOCGETD" {
  run_lab "${SCRIPT}" query-ldisc
  [ "${status}" -eq 0 ]
  [ "${output}" = "ldisc=0" ]
}

@test "canonical line editing erases previous character on VERASE" {
  run_lab "${SCRIPT}" canonical-erase
  [ "${status}" -eq 0 ]
  [ "${output}" = "read_len=4 read_str=abd" ]
}

@test "canonical line editing discards full line on VKILL" {
  run_lab "${SCRIPT}" canonical-kill
  [ "${status}" -eq 0 ]
  [ "${output}" = "read_len=5 read_str=kept" ]
}

@test "canonical line editing removes previous word on VWERASE" {
  run_lab "${SCRIPT}" canonical-werase
  [ "${status}" -eq 0 ]
  [ "${output}" = "read_len=12 read_str=first third" ]
}

@test "VEOF flushes pending buffer without newline then returns zero bytes on empty line" {
  run_lab "${SCRIPT}" eof-flush
  [ "${status}" -eq 0 ]
  part1_len=$(printf '%s\n' "${output}" | sed -n 's/.*part1_len=\(.*\)/\1/p')
  part1=$(printf '%s\n' "${output}" | sed -n 's/.*part1_str=\(.*\)/\1/p')
  part2_len=$(printf '%s\n' "${output}" | sed -n 's/.*part2_len=\(.*\)/\1/p')
  # exactly the eight bytes of "flush_me": VEOF appends no newline
  [ "${part1_len}" = "8" ]
  [ "${part1}" = "flush_me" ]
  [ "${part2_len}" = "0" ]
}

@test "input translation ICRNL maps carriage return to newline and output ONLCR maps newline to CRLF" {
  run_lab "${SCRIPT}" crnl-mapping
  [ "${status}" -eq 0 ]
  last_byte=$(printf '%s\n' "${output}" | sed -n 's/.*last_byte=\(0x[0-9a-fA-F]*\).*/\1/p')
  ends_crnl=$(printf '%s\n' "${output}" | sed -n 's/.*ends_crnl=\([0-9]*\).*/\1/p')
  [ "${last_byte}" = "0x0a" ]
  [ "${ends_crnl}" = "1" ]
}

@test "line discipline echoes input to master queue when ECHO is active and silences echo when cleared" {
  run_lab "${SCRIPT}" echo-toggle
  [ "${status}" -eq 0 ]
  echo_on_len=$(printf '%s\n' "${output}" | sed -n 's/.*echo_on_len=\([0-9]*\).*/\1/p')
  noecho_len=$(printf '%s\n' "${output}" | sed -n 's/.*noecho_len=\([0-9]*\).*/\1/p')
  slave_read_len=$(printf '%s\n' "${output}" | sed -n 's/.*slave_read_len=\([0-9]*\).*/\1/p')
  # "ping" plus the echoed newline, which ONLCR turns into CR LF
  [ "${echo_on_len}" = "6" ]
  [ "${noecho_len}" = "0" ]
  [ "${slave_read_len}" = "6" ]
}

@test "noncanonical mode releases characters immediately without waiting for newline" {
  run_lab "${SCRIPT}" noncanon-switch
  [ "${status}" -eq 0 ]
  canon_poll=$(printf '%s\n' "${output}" | sed -n 's/.*canonical_poll=\([0-9]*\).*/\1/p')
  noncanon_poll=$(printf '%s\n' "${output}" | sed -n 's/.*noncanon_poll=\([0-9]*\).*/\1/p')
  content=$(printf '%s\n' "${output}" | sed -n 's/.*content=\(.*\)/\1/p')
  [ "${canon_poll}" = "0" ]
  [ "${noncanon_poll}" = "1" ]
  [ "${content}" = "xy" ]
}

@test "VINTR generates SIGINT to foreground process group under ISIG" {
  run_lab "${SCRIPT}" isig-signal
  [ "${status}" -eq 0 ]
  sig=$(printf '%s\n' "${output}" | sed -n 's/.*CAUGHT_SIGNAL=\([0-9]*\).*/\1/p')
  [ "${sig}" = "2" ]
}

@test "shell read builtin inside PTY receives canonical erased input" {
  lab_shell_words
  # shellcheck disable=SC2016 # $line expands inside the spawned child shell
  run_lab "${SCRIPT}" shell-read "${LAB_SHELL_WORDS[@]}" -c 'read -r line; printf "SHELL_READ=<%s>\n" "$line"'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "${clean_out}" = "SHELL_READ=<hello>" ]
      ;;
    *) false ;;
  esac
}

@test "a shell started on a pty sees a terminal on stdin, stdout, and stderr, and outside it sees none" {
  lab_shell_words
  # shellcheck disable=SC2016 # the test builtin and tty run inside the spawned child shell
  run_lab "${SCRIPT}" shell-read "${LAB_SHELL_WORDS[@]}" -c \
    'if [ -t 0 ] && [ -t 1 ] && [ -t 2 ]; then echo ISATTY=yes; else echo ISATTY=no; fi; echo "TTYNAME=$(tty)"'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  printf '%s\n' "${clean_out}" | grep -qx 'ISATTY=yes'
  case $(printf '%s\n' "${clean_out}" | sed -n 's/^TTYNAME=//p') in
    /dev/pts/* | /dev/ttys*) ;;
    *) false ;;
  esac
  # the same test outside the pty: bats gives the shell a pipe on stdout
  run_lab_c 'if [ -t 1 ]; then echo ISATTY=yes; else echo ISATTY=no; fi'
  [ "${status}" -eq 0 ]
  [ "${output}" = "ISATTY=no" ]
}

@test "read -n is accepted or rejected per shell family on a pty" {
  lab_shell_words
  # shellcheck disable=SC2016 # $? and $c belong to the spawned child shell
  run_lab "${SCRIPT}" shell-read "${LAB_SHELL_WORDS[@]}" -c 'read -n 1 c 2>/dev/null; echo "RC=$? C=$c"'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  case $(lab_shell_family) in
    # accepted: returns the first typed character without waiting for the line
    bash | mksh | ksh93 | busybox) [ "${clean_out}" = "RC=0 C=h" ] ;;
    # zsh's -n has another meaning; it reads nothing here
    zsh | zsh-sh) [ "${clean_out}" = "RC=0 C=" ] ;;
    # rejected as an invalid option (status 2 in dash and yash, 1 in posh)
    dash | yash) [ "${clean_out}" = "RC=2 C=" ] ;;
    posh) [ "${clean_out}" = "RC=1 C=" ] ;;
    *) false ;;
  esac
}

@test "stty sane resets ixon on macOS but leaves it as found on GNU and BusyBox stty" {
  lab_shell_words
  # shellcheck disable=SC2016 # the pipeline runs inside the spawned child shell
  run_lab "${SCRIPT}" shell-read "${LAB_SHELL_WORDS[@]}" -c \
    'stty -ixon -opost -onlcr; stty sane; stty -a 2>/dev/null | tr -s " \t;" "\n\n\n" | grep -x -e ixon -e -ixon -e opost -e -opost -e onlcr -e -onlcr'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  os=$(uname -s)
  if [ "${os}" = Darwin ]; then
    [ "${clean_out}" = "$(printf 'ixon\nopost\nonlcr')" ]
  else
    [ "${clean_out}" = "$(printf -- '-ixon\nopost\nonlcr')" ]
  fi
}

@test "stty raw clears OPOST and leaves ONLCR set" {
  lab_shell_words
  # shellcheck disable=SC2016 # the pipeline runs inside the spawned child shell
  run_lab "${SCRIPT}" shell-read "${LAB_SHELL_WORDS[@]}" -c \
    'stty raw; stty -a 2>/dev/null | tr -s " \t;" "\n\n\n" | grep -x -e opost -e -opost -e onlcr -e -onlcr'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  [ "${clean_out}" = "$(printf -- '-opost\nonlcr')" ]
}
