#!/bin/sh
# pty_shell: observe a shell from the inside of a pseudo-terminal.
#
# Usage:
#   pty_shell.sh drive PROBE SCRIPT MODE SHELL [SHELL_ARG...]
#       Runs under the shell being tested but with no terminal. Starts SHELL on a new pty through
#       PROBE (the compiled pty_probe), running "SCRIPT inner MODE" as its slave-side process.
#       Prints everything the inner shell wrote to the pty, byte for byte (so CR NL, not NL).
#   pty_shell.sh inner MODE
#       The slave-side process. MODE is one of:
#         ids      which of stdin, stdout, stderr are terminals, and which device the tty utility names
#         monitor  whether set -m is accepted and turns the monitor option on
#         size     what stty size reports (rows then columns)
#         read     read one line typed on the master and print it back
#         eof      read when the master sends the end-of-file character at the start of a line
#         noctty   on a pty that is stdin, stdout, and stderr but not the controlling terminal: is
#                  stdin a terminal, which device does tty name, can /dev/tty be opened
set -u

script_drive() {
  probe=$1
  script=$2
  mode=$3
  shift 3
  case $mode in
    size) "$probe" run -w 24x80 -- "$@" "$script" inner "$mode" ;;
    read) "$probe" run -i 'typed\n' -- "$@" "$script" inner "$mode" ;;
    eof) "$probe" run -i '\004' -- "$@" "$script" inner "$mode" ;;
    ids | monitor) "$probe" run -- "$@" "$script" inner "$mode" ;;
    noctty) "$probe" run -n -- "$@" "$script" inner "$mode" ;;
    *)
      printf 'pty_shell: unknown mode %s\n' "$mode" >&2
      return 2
      ;;
  esac
}

script_inner() {
  case $1 in
    ids)
      in=0
      out=0
      err=0
      [ -t 0 ] && in=1
      [ -t 1 ] && out=1
      [ -t 2 ] && err=1
      name=$(tty 2>/dev/null)
      case $name in
        /dev/*) prefix=${name%%[0-9]*} ;;
        *) prefix=none ;;
      esac
      printf 'stdin=%s stdout=%s stderr=%s tty_prefix=%s\n' "$in" "$out" "$err" "$prefix"
      ;;
    monitor)
      set -m 2>/dev/null
      rc=$?
      case $- in
        *m*) flag=on ;;
        *) flag=off ;;
      esac
      printf 'set_m_rc=%s monitor=%s\n' "$rc" "$flag"
      ;;
    size)
      size=$(stty size 2>/dev/null) || size=none
      printf 'size=%s\n' "$size"
      ;;
    read)
      IFS= read -r line
      printf 'got=%s\n' "$line"
      ;;
    eof)
      IFS= read -r line
      printf 'read_rc=%s\n' "$?"
      ;;
    noctty)
      in=0
      [ -t 0 ] && in=1
      name=$(tty 2>/dev/null)
      case $name in
        /dev/*) prefix=${name%%[0-9]*} ;;
        *) prefix=none ;;
      esac
      if (: </dev/tty) 2>/dev/null; then dev_tty=ok; else dev_tty=fail; fi
      printf 'stdin=%s tty_prefix=%s dev_tty=%s\n' "$in" "$prefix" "$dev_tty"
      ;;
    *)
      printf 'pty_shell: unknown mode %s\n' "$1" >&2
      return 2
      ;;
  esac
}

case ${1-} in
  drive)
    shift
    script_drive "$@"
    ;;
  inner)
    shift
    script_inner "$@"
    ;;
  *)
    printf 'usage: pty_shell.sh drive PROBE SCRIPT MODE SHELL... | inner MODE\n' >&2
    exit 2
    ;;
esac
