#!/usr/bin/env bats
# Tests for pseudo-terminals: the pair itself (pty_probe.c) and shells running on one (pty_shell.sh).

setup() {
  load ../lib/lab
  SCRIPT="${BATS_TEST_DIRNAME}/pty_shell.sh"
  PROBE="${BATS_TEST_TMPDIR}/pty_probe"
  "${CC:-cc}" -O2 -Wall -Wextra -Werror "${BATS_TEST_DIRNAME}/pty_probe.c" -o "${PROBE}"
  # shellcheck disable=SC2154 # CR is a plain variable used by the helpers below
  CR=$(printf '\r')
  export PROBE SCRIPT CR
}

# probe MODE...: run the compiled probe under the shell under test.
probe() {
  run_lab_c '"$@"' probe "${PROBE}" "$@"
}

# slave_prefix: the directory prefix this platform gives slave names (devpts or BSD-style).
slave_prefix() {
  if [ -d /dev/pts ]; then
    printf '/dev/pts/\n'
  else
    printf '/dev/ttys\n'
  fi
}

# on_pty MODE: run pty_shell.sh MODE as the slave-side process of a new pty, with the shell under test as its shell.
on_pty() {
  local mode=$1
  lab_shell_words
  run_lab "${SCRIPT}" drive "${PROBE}" "${SCRIPT}" "${mode}" "${LAB_SHELL_WORDS[@]}"
}

@test "a new pair: the slave is a terminal owned by the caller, cooked, mode 0620, size 0x0" {
  probe pair
  [ "${status}" -eq 0 ]
  expect="prefix=$(slave_prefix) slave_isatty=1 slave_uid_is_me=1 slave_mode=620 echo=1 icanon=1 isig=1 icrnl=1 opost=1 onlcr=1 rows=0 cols=0"
  [ "${output}" = "${expect}" ]
}

@test "bytes cross the pair through the line discipline: echo, CR to NL, NL to CR NL, and raw mode bypasses it" {
  probe echo
  [ "${status}" -eq 0 ]
  # shellcheck disable=SC2016 # the backslashes are the probe's own escapes, not shell syntax
  expect='cooked_master_sees=hi\r\n cooked_slave_reads=hi\n cooked_out=ok\r\n raw_master_sees=ok\n raw_slave_reads=x\r'
  [ "${output}" = "${expect}" ]
}

@test "when the last slave descriptor closes, Linux keeps the written bytes then fails reads with EIO, macOS drops them and reads 0" {
  probe slave-exit
  [ "${status}" -eq 0 ]
  if [ -d /dev/pts ]; then
    # shellcheck disable=SC2016 # probe escapes
    expect='first_read=bye\r\n second_read_rc=-1 second_read_errno=EIO'
  else
    expect='first_read= second_read_rc=0 second_read_errno=0'
  fi
  [ "${output}" = "${expect}" ]
}

@test "closing the master hangs up the slave: SIGHUP to the session leader, reads give end of file, writes fail with EIO" {
  probe master-close
  [ "${status}" -eq 0 ]
  [ "${output}" = "sighup=1 read_rc=0 read_errno=0 write_rc=-1 write_errno=EIO" ]
}

@test "TIOCSWINSZ on the master changes the slave's size and raises SIGWINCH there" {
  probe winsize
  [ "${status}" -eq 0 ]
  [ "${output}" = "master_initial=0x0 sigwinch=1 slave_rows=31 slave_cols=97" ]
}

@test "the interrupt character written to the master raises SIGINT in the slave's foreground group and echoes as ^C" {
  probe ctrl-c
  [ "${status}" -eq 0 ]
  [ "${output}" = "master_sees=^C sigint=1 slave_read=" ]
}

@test "with ISIG off the same byte arrives as data and no signal is raised" {
  probe ctrl-c-raw
  [ "${status}" -eq 0 ]
  # shellcheck disable=SC2016 # probe escapes
  [ "${output}" = 'master_sees= sigint=0 slave_read=\x03' ]
}

@test "TIOCSTI on one's own terminal works unless Linux's legacy_tiocsti sysctl is 0 and the caller is unprivileged" {
  probe tiocsti
  [ "${status}" -eq 0 ]
  knob=/proc/sys/dev/tty/legacy_tiocsti
  value=none
  if [ -r "${knob}" ]; then
    value=$(cat "${knob}")
  fi
  uid=$(id -u)
  if [ "${value}" = 0 ] && [ "${uid}" -ne 0 ]; then
    [ "${output}" = "tiocsti_rc=-1 tiocsti_errno=EIO slave_reads= uid_is_root=0" ]
  else
    # shellcheck disable=SC2016 # probe escapes
    expect='tiocsti_rc=0 tiocsti_errno=0 slave_reads=x\n uid_is_root='
    [ "${output%uid_is_root=*}uid_is_root=" = "${expect}" ]
  fi
}

@test "run: a command on a pty writes CR NL, and its exit status comes back" {
  # shellcheck disable=SC2016 # $1 is expanded by the shell under test
  run_lab_c '"$1" run -- /bin/sh -c "echo hi; exit 7"' probe "${PROBE}"
  [ "${status}" -eq 7 ]
  [ "${output}" = "hi${CR}" ]
}

@test "run: a command that outlives the limit is killed and the probe reports 124" {
  # shellcheck disable=SC2016 # $1 is expanded by the shell under test
  run_lab_c '"$1" run -t 1 -- /bin/sleep 20' probe "${PROBE}"
  [ "${status}" -eq 124 ]
}

# ls_flavor: which ls this host has: bsd (macOS), gnu, or busybox.
ls_flavor() {
  if [ ! -d /dev/pts ]; then
    printf 'bsd\n'
    return
  fi
  case $(ls --version 2>&1) in
    *"GNU coreutils"*) printf 'gnu\n' ;;
    *) printf 'busybox\n' ;;
  esac
}

@test "the same ls prints columns on a pty and one name per line on a pipe" {
  mkdir "${BATS_TEST_TMPDIR}/dir"
  : >"${BATS_TEST_TMPDIR}/dir/a"
  : >"${BATS_TEST_TMPDIR}/dir/b"
  : >"${BATS_TEST_TMPDIR}/dir/c"
  # shellcheck disable=SC2016 # $1 and $2 are expanded by the shell under test
  run_lab_c '"$1" run -w 24x80 -- ls "$2" | tr -d "\r"' probe "${PROBE}" "${BATS_TEST_TMPDIR}/dir"
  [ "${status}" -eq 0 ]
  [ "${#lines[@]}" -eq 1 ]
  esc=$(printf '\033')
  case $(ls_flavor) in
    bsd) [ "${output}" = "a$(printf '\t')b$(printf '\t')c" ] ;;
    gnu) [ "${output}" = "a  b  c" ] ;;
    busybox) [ "${output}" = "${esc}[0;0ma${esc}[m  ${esc}[0;0mb${esc}[m  ${esc}[0;0mc${esc}[m" ] ;;
    *) false ;;
  esac
  # shellcheck disable=SC2016 # $1 is expanded by the shell under test
  run_lab_c 'ls "$1" | cat' probe "${BATS_TEST_TMPDIR}/dir"
  [ "${status}" -eq 0 ]
  [ "${#lines[@]}" -eq 3 ]
  [ "${output}" = "$(printf 'a\nb\nc')" ]
}

@test "without a pty no standard stream is a terminal" {
  run_lab "${SCRIPT}" inner ids </dev/null
  [ "${status}" -eq 0 ]
  [ "${output}" = "stdin=0 stdout=0 stderr=0 tty_prefix=none" ]
}

@test "on a pty all three streams are terminals and tty names a slave device" {
  on_pty ids
  [ "${status}" -eq 0 ]
  prefix=$(slave_prefix)
  [ "${output}" = "stdin=1 stdout=1 stderr=1 tty_prefix=${prefix}${CR}" ]
}

@test "on a pty set -m turns the monitor option on, except in posh which has no -m" {
  on_pty monitor
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "${status}" -eq 0 ]
      [ "${output}" = "set_m_rc=0 monitor=on${CR}" ]
      ;;
    posh)
      # posh stops at the unknown option, so the script prints nothing
      [ -z "${output}" ]
      ;;
    *) false ;;
  esac
}

@test "without a terminal set -m is accepted by bash, mksh and ksh93 and refused or fatal elsewhere" {
  run_lab "${SCRIPT}" inner monitor </dev/null
  case $(lab_shell_family) in
    bash | mksh | ksh93) [ "${output}" = "set_m_rc=0 monitor=on" ] ;;
    dash | busybox) [ "${output}" = "set_m_rc=0 monitor=off" ] ;;
    yash) [ "${output}" = "set_m_rc=1 monitor=off" ] ;;
    zsh | zsh-sh | posh) [ -z "${output}" ] ;;
    *) false ;;
  esac
}

@test "stty size reports the window size the master set" {
  on_pty size
  [ "${status}" -eq 0 ]
  [ "${output}" = "size=24 80${CR}" ]
}

@test "a line typed on the master is echoed back and then read by the shell (ksh93u+ 2012 echoes it twice)" {
  on_pty read
  [ "${status}" -eq 0 ]
  once="typed${CR}
got=typed${CR}"
  twice="typed${CR}
typed${CR}
got=typed${CR}"
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | dash | posh) [ "${output}" = "${once}" ] ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "${output}" = "${once}" ] ;;
        *) [ "${output}" = "${twice}" ] ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "the end-of-file character at the start of a line makes read fail with status 1" {
  on_pty eof
  [ "${status}" -eq 0 ]
  if [ -d /dev/pts ]; then
    [ "${output}" = "read_rc=1${CR}" ]
  else
    # macOS echoes the end-of-file character as ^D and two backspaces before the shell reads it
    [ "${output}" = "^D$(printf '\b\b')read_rc=1${CR}" ]
  fi
}

@test "a shell with a pty as stdin, stdout and stderr but no controlling terminal sees a tty and cannot open /dev/tty" {
  on_pty noctty
  [ "${status}" -eq 0 ]
  prefix=$(slave_prefix)
  [ "${output}" = "stdin=1 tty_prefix=${prefix} dev_tty=fail${CR}" ]
}

@test "set -m is fatal in zsh without a terminal and in posh everywhere; dash, BusyBox ash and yash warn and carry on" {
  run_lab_c 'set -m; echo after' </dev/null
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "${status}" -eq 1 ]
      [ "${output}" = "zsh:set:1: can't change option: -m" ]
      ;;
    posh)
      [ "${status}" -eq 1 ]
      [ "${output}" = "posh: set: -m: unknown option" ]
      ;;
    dash | busybox)
      [ "${status}" -eq 0 ]
      [ "${lines[${#lines[@]} - 1]}" = after ]
      case ${output} in
        *"can't access tty; job control turned off"*) ;;
        *) false ;;
      esac
      ;;
    yash)
      [ "${status}" -eq 0 ]
      case ${output} in
        *"set: cannot open file \`/dev/tty': No such device or address"*"set: disabling job control"*) ;;
        *) false ;;
      esac
      ;;
    *)
      [ "${status}" -eq 0 ]
      [ "${output}" = after ]
      ;;
  esac
  lab_shell_words
  run "${PROBE}" run -- "${LAB_SHELL_WORDS[@]}" -c 'set -m; echo after'
  case $(lab_shell_family) in
    posh)
      [ "${status}" -eq 1 ]
      [ "${output}" = "posh: set: -m: unknown option${CR}" ]
      ;;
    *)
      [ "${status}" -eq 0 ]
      [ "${output}" = "after${CR}" ]
      ;;
  esac
}

@test "a child that writes one line and exits never loses it: 100 runs" {
  # shellcheck disable=SC2016 # $1 is expanded by the shell under test
  run_lab_c '
    n=0 bad=0
    while [ "$n" -lt 100 ]; do
      out=$("$1" run -- /bin/echo hi | tr -d "\r")
      [ "$out" = hi ] || bad=$((bad + 1))
      n=$((n + 1))
    done
    echo "lost=$bad"' probe "${PROBE}"
  [ "${status}" -eq 0 ]
  [ "${output}" = "lost=0" ]
}

@test "the pty count limit is a readable system setting" {
  if [ -d /dev/pts ]; then
    run_lab_c 'cat /proc/sys/kernel/pty/max'
  else
    run_lab_c 'sysctl -n kern.tty.ptmx_max'
  fi
  [ "${status}" -eq 0 ]
  case ${output} in
    '' | *[!0-9]*) false ;;
    *) [ "${output}" -gt 0 ] ;;
  esac
}
