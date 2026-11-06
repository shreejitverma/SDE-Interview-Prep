#!/bin/sh
# esc-probes.sh MODE [ARGS]: emit and strip ECMA-48 / xterm control sequences.
#   forms                  hex of the bytes each spelling of ESC followed by Z produces, in this shell
#   sgr 16|256|true|colon  an SGR sequence for the named colour model
#   cup ROW COL            CUP, cursor position
#   dec set|reset N        DEC private mode set or reset
#   title TEXT bel|st      OSC 2 with the named terminator
#   strip                  remove CSI and OSC sequences from stdin with sed
#   width                  bytes in a coloured prompt versus its visible width
set -eu

mode=${1:-}
if [ "$#" -gt 0 ]; then
  shift
fi

hex() {
  LC_ALL=C od -An -tx1 | tr -d ' \n'
}

# strip_stream: the usual "remove ANSI" filter: CSI sequences, then OSC strings ended by BEL or ST.
strip_stream() {
  esc=$(printf '\033')
  bel=$(printf '\007')
  LC_ALL=C sed \
    -e "s#${esc}\\[[0-?]*[ -/]*[@-~]##g" \
    -e "s#${esc}][^${bel}${esc}]*${bel}##g" \
    -e "s#${esc}][^${bel}${esc}]*${esc}\\\\##g"
}

case $mode in
  forms)
    v=$(printf '\033Z' | hex)
    printf 'octal=%s\n' "$v"
    v=$(printf '\eZ' | hex)
    printf 'backslash-e=%s\n' "$v"
    v=$(printf '\x1bZ' | hex)
    printf 'hex-escape=%s\n' "$v"
    code="printf %s \$'\\033Z'"
    v=$(eval "$code" | hex)
    printf 'dollar-quote=%s\n' "$v"
    # shellcheck disable=SC3037 # echo -e is the non-portable spelling under test
    v=$(echo -e '\033Z' | hex)
    printf 'echo-e=%s\n' "$v"
    ;;
  sgr)
    case ${1:-} in
      16) printf '\033[1;31;44mx\033[0m' ;;
      256) printf '\033[38;5;196;48;5;232mx\033[0m' ;;
      true) printf '\033[38;2;255;128;0mx\033[0m' ;;
      colon) printf '\033[38:2::255:128:0mx\033[0m' ;;
      *)
        printf 'unknown palette\n' >&2
        exit 1
        ;;
    esac
    ;;
  cup)
    printf '\033[%d;%dH' "${1:-1}" "${2:-1}"
    ;;
  dec)
    case ${1:-} in
      set) printf '\033[?%dh' "${2:-0}" ;;
      reset) printf '\033[?%dl' "${2:-0}" ;;
      *)
        printf 'unknown action\n' >&2
        exit 1
        ;;
    esac
    ;;
  title)
    # shellcheck disable=SC1003 # the trailing backslash is the second byte of ST
    case ${2:-bel} in
      bel) printf '\033]2;%s\007' "${1:-}" ;;
      st) printf '\033]2;%s\033\\' "${1:-}" ;;
      *)
        printf 'unknown terminator\n' >&2
        exit 1
        ;;
    esac
    ;;
  strip)
    strip_stream
    ;;
  width)
    prompt=$(printf '\033[31muser@host\033[0m$ ')
    plain=$(printf '%s' "$prompt" | strip_stream)
    printf 'bytes=%d visible=%d phantom=%d\n' "${#prompt}" "${#plain}" "$((${#prompt} - ${#plain}))"
    ;;
  *)
    printf 'usage: esc-probes.sh forms|sgr|cup|dec|title|strip|width\n' >&2
    exit 2
    ;;
esac
