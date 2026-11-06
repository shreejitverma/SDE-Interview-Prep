#!/bin/sh
# term-client.sh MODE: a program that asks its terminal a question.
# It must run with a real terminal on stdin and stdout (pty_probe term provides one).
#   cpr-raw     ask for the cursor position with ESC [ 6 n, read the reply in raw mode, print row and col
#   cpr-cooked  ask, but stay in cooked mode with echo on, so the line discipline echoes the reply
#   title       set the window title, then ask the terminal to report it with ESC [ 21 t
set -eu

mode=${1:-}
esc=$(printf '\033')

# read_until CHAR: read bytes from stdin one at a time into $reply until CHAR or a 2 second silence.
read_until() {
  reply=
  while :; do
    c=$(
      dd bs=1 count=1 2>/dev/null
      echo x
    )
    c=${c%x}
    [ -n "$c" ] || break
    reply=$reply$c
    if [ "$c" = "$1" ]; then
      break
    fi
  done
}

case $mode in
  cpr-raw)
    saved=$(stty -g)
    stty -echo -icanon min 0 time 20
    printf '\033[6n'
    read_until R
    stty "$saved"
    body=${reply#"${esc}["}
    body=${body%R}
    printf 'row=%s col=%s\n' "${body%%;*}" "${body#*;}"
    ;;
  cpr-cooked)
    printf '\033[6n'
    /bin/sleep 1
    ;;
  title)
    saved=$(stty -g)
    stty -echo -icanon min 0 time 20
    printf '\033]2;%s\007' 'id>pwned'
    printf '\033[21t'
    read_until "\\"
    stty "$saved"
    hex=$(printf '%s' "$reply" | od -An -tx1 | tr -s ' \n' ' ')
    printf 'reply-hex=%s\n' "$hex"
    ;;
  *)
    printf 'usage: term-client.sh cpr-raw|cpr-cooked|title\n' >&2
    exit 2
    ;;
esac
