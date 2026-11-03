#!/bin/sh
# printf-echo-probe.sh: probe behavioral divergences between echo and printf
# across leading-dash arguments, backslash escapes, newline suppression,
# format string reuse, closed descriptor errors, format injection, the -v option,
# space-flag conversions, numbered conversions, extra operands, and external utilities.
set -u

mode=${1:-leading-dash}
if [ "$#" -gt 0 ]; then
  shift
fi

fmt_token() {
  case ${1} in
    *"
")
      # Replace trailing newline with literal \n marker for single-line output
      printf '%s\\n' "${1%
}"
      ;;
    "")
      printf 'EMPTY'
      ;;
    *)
      printf '%s' "${1}"
      ;;
  esac
}

# show_nl TEXT: print TEXT with every newline written as <NL>, so a result fits on one line.
show_nl() {
  _rest=$1
  _acc=
  while [ -n "${_rest}" ]; do
    _rest_tail=${_rest#?}
    _ch=${_rest%"${_rest_tail}"}
    _rest=${_rest_tail}
    case ${_ch} in
      "
") _acc="${_acc}<NL>" ;;
      *) _acc="${_acc}${_ch}" ;;
    esac
  done
  printf '%s' "${_acc}"
}

# classify_err TEXT: name the kind of diagnostic: none, bad-fd, io-error, or other.
classify_err() {
  case $1 in
    "") printf 'none' ;;
    *"ad file descriptor"*) printf 'bad-fd' ;;
    *"I/O error"*) printf 'io-error' ;;
    *) printf 'other' ;;
  esac
}

# inject_v FORMAT ARG ARG: the vulnerable helper `printf "$1" "$2" "$3"`, with all output discarded.
inject_v() {
  # shellcheck disable=SC2059 # intentional: demonstrate format string injection
  printf "$1" "$2" "$3" >/dev/null 2>&1
}

case ${mode} in
  leading-dash)
    v_n="-n"
    v_e="-e"
    v_opt="--"

    raw_echo_n=$(
      echo "${v_n}"
      printf 'X'
    )
    out_echo_n=${raw_echo_n%X}

    raw_printf_n=$(
      printf '%s\n' "${v_n}"
      printf 'X'
    )
    out_printf_n=${raw_printf_n%X}

    raw_echo_e=$(
      echo "${v_e}"
      printf 'X'
    )
    out_echo_e=${raw_echo_e%X}

    raw_printf_e=$(
      printf '%s\n' "${v_e}"
      printf 'X'
    )
    out_printf_e=${raw_printf_e%X}

    raw_echo_dd=$(
      echo -- "${v_opt}"
      printf 'X'
    )
    out_echo_dd=${raw_echo_dd%X}

    raw_printf_dd=$(
      printf '%s\n' "${v_opt}"
      printf 'X'
    )
    out_printf_dd=${raw_printf_dd%X}

    tok_echo_n=$(fmt_token "${out_echo_n}")
    tok_printf_n=$(fmt_token "${out_printf_n}")
    tok_echo_e=$(fmt_token "${out_echo_e}")
    tok_printf_e=$(fmt_token "${out_printf_e}")
    tok_echo_dd=$(fmt_token "${out_echo_dd}")
    tok_printf_dd=$(fmt_token "${out_printf_dd}")

    printf 'echo_n=%s printf_n=%s echo_e=%s printf_e=%s echo_dd=%s printf_dd=%s\n' \
      "${tok_echo_n}" "${tok_printf_n}" "${tok_echo_e}" "${tok_printf_e}" "${tok_echo_dd}" "${tok_printf_dd}"
    ;;

  escapes)
    v_nl='a\nb'
    v_c='a\cb'

    raw_echo_nl=$(
      echo "${v_nl}"
      printf 'X'
    )
    out_echo_nl=${raw_echo_nl%X}
    body_echo_nl=${out_echo_nl%
}

    raw_printf_s_nl=$(
      printf '%s\n' "${v_nl}"
      printf 'X'
    )
    out_printf_s_nl=${raw_printf_s_nl%X}

    raw_printf_b_nl=$(
      printf '%b\n' "${v_nl}"
      printf 'X'
    )
    out_printf_b_nl=${raw_printf_b_nl%X}
    body_printf_b_nl=${out_printf_b_nl%
}

    raw_echo_c=$(
      echo "${v_c}"
      printf 'X'
    )
    out_echo_c=${raw_echo_c%X}

    raw_printf_s_c=$(
      printf '%s\n' "${v_c}"
      printf 'X'
    )
    out_printf_s_c=${raw_printf_s_c%X}

    raw_printf_b_c=$(
      printf '%b\n' "${v_c}"
      printf 'X'
    )
    out_printf_b_c=${raw_printf_b_c%X}

    # Does echo expand \n into a multiline body?
    echo_nl_multi=no
    case ${body_echo_nl} in
      *"
"*) echo_nl_multi=yes ;;
      *) echo_nl_multi=no ;;
    esac

    # Does printf %s keep \n as literal two bytes?
    printf_s_literal=no
    if [ "${out_printf_s_nl}" = "a\\nb
" ]; then
      printf_s_literal=yes
    fi

    # Does printf %b expand \n into a multiline body?
    printf_b_multi=no
    case ${body_printf_b_nl} in
      *"
"*) printf_b_multi=yes ;;
      *) printf_b_multi=no ;;
    esac

    # Did \c halt output at 'a' without trailing newline?
    echo_c_halted=no
    if [ "${out_echo_c}" = "a" ]; then
      echo_c_halted=yes
    fi

    printf_b_c_halted=no
    if [ "${out_printf_b_c}" = "a" ]; then
      printf_b_c_halted=yes
    fi

    printf_s_c_literal=no
    if [ "${out_printf_s_c}" = "a\\cb
" ]; then
      printf_s_c_literal=yes
    fi

    printf 'echo_nl_multi=%s printf_s_literal=%s printf_b_multi=%s echo_c_halted=%s printf_b_c_halted=%s printf_s_c_literal=%s\n' \
      "${echo_nl_multi}" "${printf_s_literal}" "${printf_b_multi}" "${echo_c_halted}" "${printf_b_c_halted}" "${printf_s_c_literal}"
    ;;

  suppress-newline)
    v_n="-n"

    # Attempt to print '-n' without a newline using echo -n
    # shellcheck disable=SC3037 # intentional: test non-portable echo -n behavior
    raw_echo_nonl=$(
      echo -n "${v_n}"
      printf 'X'
    )
    out_echo_nonl=${raw_echo_nonl%X}

    # Attempt to print '-n' without a newline using printf '%s'
    raw_printf_nonl=$(
      printf '%s' "${v_n}"
      printf 'X'
    )
    out_printf_nonl=${raw_printf_nonl%X}

    tok_echo_nonl=$(fmt_token "${out_echo_nonl}")
    tok_printf_nonl=$(fmt_token "${out_printf_nonl}")

    printf 'echo_nonl=%s printf_nonl=%s\n' "${tok_echo_nonl}" "${tok_printf_nonl}"
    ;;

  reuse)
    # Format recycling with matching arguments
    r_multi=$(printf '<%s>' alpha beta gamma)

    # Format recycling with odd argument count: missing argument converts to null string
    # shellcheck disable=SC2183 # intentional: verify POSIX format reuse with missing operand
    r_pair=$(printf '<%s,%s>' 1 2 3)

    # Missing arguments for all conversion specifiers
    # shellcheck disable=SC2183 # intentional: verify POSIX default operand conversion
    r_empty=$(printf '<%s><%d>')

    printf 'multi=%s pair=%s empty=%s\n' \
      "${r_multi}" "${r_pair}" "${r_empty}"
    ;;

  closed-fd)
    # Test error reporting when stdout is closed
    # shellcheck disable=SC2312 # intentional: capture subshell exit status
    (
      exec 1>&-
      echo "payload" 2>/dev/null
    )
    s_echo=$?

    # shellcheck disable=SC2312 # intentional: capture subshell exit status
    (
      exec 1>&-
      printf '%s\n' "payload" 2>/dev/null
    )
    s_printf=$?

    printf 'echo_status=%d printf_status=%d\n' "${s_echo}" "${s_printf}"
    ;;

  format-injection)
    v_spec="100% discount"
    # shellcheck disable=SC2059 # intentional: demonstrate format string injection
    raw_vuln=$(printf "${v_spec}\n" 2>&1)
    raw_safe=$(printf '%s\n' "${v_spec}")

    v_missing="user: %s"
    # shellcheck disable=SC2059 # intentional: demonstrate format string injection
    raw_missing=$(printf "${v_missing}\n" 2>&1)

    printf 'vuln=[%s] safe=[%s] missing=[%s]\n' \
      "${raw_vuln}" "${raw_safe}" "${raw_missing}"
    ;;

  printf-v)
    # A variable format operand that begins with -v: does it assign a shell variable?
    NAME=orig
    inject_v -vNAME value x
    v_joined=${NAME}

    NAME=orig
    inject_v -v NAME value
    v_split=${NAME}

    # No format operand remains after -v NAME, so this is a usage error where -v exists.
    NAME=orig
    # shellcheck disable=SC3045 # intentional: probe the non-POSIX -v option
    printf -v NAME >/dev/null 2>&1
    v_bare_rc=$?
    v_bare_name=${NAME}
    v_bare=ok
    if [ "${v_bare_rc}" -ne 0 ]; then
      v_bare=fail
    fi

    printf 'joined=%s split=%s bare=%s bare_name=%s\n' \
      "${v_joined}" "${v_split}" "${v_bare}" "${v_bare_name}"
    ;;

  percent-off)
    # "50% off" as a format: "% o" is the octal conversion with the space flag and a missing operand.
    v_off="50% off"
    # shellcheck disable=SC2059 # intentional: demonstrate format string injection
    raw_off=$(
      printf "${v_off}" 2>/dev/null
      printf 'X'
    )
    out_off=${raw_off%X}
    # shellcheck disable=SC2059 # intentional: demonstrate format string injection
    printf "${v_off}" >/dev/null 2>&1
    s_off=$?
    tok_off=$(show_nl "${out_off}")
    printf 'out=[%s] status=%d\n' "${tok_off}" "${s_off}"
    ;;

  positional)
    # Numbered conversions (%n$) are optional in POSIX.1-2024: does this printf accept them?
    # shellcheck disable=SC2016 # intentional: %2$s is a numbered conversion, not a shell expansion
    raw_pos=$(
      printf '%2$s %1$s\n' a b 2>/dev/null
      printf 'X'
    )
    out_pos=${raw_pos%X}
    p_pair=reject
    if [ "${out_pos}" = "b a
" ]; then
      p_pair=accept
    fi

    # The 4096th operand, the NL_ARGMAX value on Linux glibc.
    set -- v1
    p_i=1
    while [ "${p_i}" -lt 4096 ]; do
      p_i=$((p_i + 1))
      set -- "$@" "v${p_i}"
    done
    # shellcheck disable=SC2016 # intentional: %4096$s is a numbered conversion, not a shell expansion
    raw_big=$(
      printf '%4096$s\n' "$@" 2>/dev/null
      printf 'X'
    )
    out_big=${raw_big%X}
    p_big=reject
    if [ "${out_big}" = "v4096
" ]; then
      p_big=accept
    fi
    printf 'pair=%s big=%s\n' "${p_pair}" "${p_big}"
    ;;

  closed-fd-stderr)
    # What a failed write to a closed stdout says on stderr (the status is covered by closed-fd).
    err_echo=$(
      (
        exec 1>&-
        echo payload
      ) 2>&1
    )
    err_printf=$(
      (
        exec 1>&-
        printf '%s\n' payload
      ) 2>&1
    )
    tok_err_echo=$(classify_err "${err_echo}")
    tok_err_printf=$(classify_err "${err_printf}")
    printf 'echo_err=%s printf_err=%s\n' "${tok_err_echo}" "${tok_err_printf}"
    ;;

  extra-operands)
    # A format with no conversions but with operands present: unspecified by POSIX.1-2024 rule 9.
    # shellcheck disable=SC2182 # intentional: operands with no conversion in the format
    raw_extra=$(
      printf 'hi\n' a b c 2>/dev/null
      printf 'X'
    )
    out_extra=${raw_extra%X}
    # shellcheck disable=SC2182 # intentional: operands with no conversion in the format
    err_extra=$(printf 'hi\n' a b c 2>&1 >/dev/null)
    warn=no
    if [ -n "${err_extra}" ]; then
      warn=yes
    fi
    tok_extra=$(show_nl "${out_extra}")
    printf 'out=[%s] stderr=%s\n' "${tok_extra}" "${warn}"
    ;;

  octal)
    # %b reads \0ddd as an octal byte (zero plus up to three digits): 'A', 'x', then 0xFF.
    oct_hex=$(printf '%b' '\0101x\0377' | od -An -tx1 | tr -d ' \n')
    printf 'octal=%s\n' "${oct_hex}"
    ;;

  external)
    # /bin/echo and /usr/bin/printf are separate programs: the shell under test does not change them.
    if [ ! -x /bin/echo ] || [ ! -x /usr/bin/printf ]; then
      printf 'unavailable\n'
      exit 0
    fi
    ext_run() {
      _raw=$(
        /bin/echo "$@" 2>/dev/null
        printf 'X'
      )
      show_nl "${_raw%X}"
    }
    x_n=$(ext_run -n)
    x_e=$(ext_run -e)
    x_dd=$(ext_run -- x)
    x_nl=$(ext_run 'a\nb')
    x_c=$(ext_run 'a\cb')
    x_nn=$(ext_run -n -n)
    raw_xp=$(
      /usr/bin/printf 'hi\n' a b c 2>/dev/null
      printf 'X'
    )
    x_p=$(show_nl "${raw_xp%X}")
    x_p_err=$(/usr/bin/printf 'hi\n' a b c 2>&1 >/dev/null)
    x_p_warn=no
    if [ -n "${x_p_err}" ]; then
      x_p_warn=yes
    fi
    # shellcheck disable=SC2182 # intentional: operands with no conversion in the format
    x_p_rc=0
    # shellcheck disable=SC2320 # the $? is that of the /usr/bin/printf on the same line
    /usr/bin/printf 'hi\n' a b c >/dev/null 2>&1 || x_p_rc=$?
    raw_xs=$(
      /usr/bin/printf '%s\n' -n 2>/dev/null
      printf 'X'
    )
    x_ps=$(show_nl "${raw_xs%X}")
    raw_xb=$(
      /usr/bin/printf '%b\n' 'a\nb' 2>/dev/null
      printf 'X'
    )
    x_pb=$(show_nl "${raw_xb%X}")
    raw_xd=$(
      /usr/bin/printf -- 'foo\n' 2>/dev/null
      printf 'X'
    )
    x_pd=$(show_nl "${raw_xd%X}")
    x_closed=0
    (
      exec 1>&-
      /bin/echo payload
    ) 2>/dev/null || x_closed=$?
    printf 'n=[%s] e=[%s] dd=[%s] nl=[%s] c=[%s] nn=[%s] closed_status=%d\n' \
      "${x_n}" "${x_e}" "${x_dd}" "${x_nl}" "${x_c}" "${x_nn}" "${x_closed}"
    printf 'extra=[%s] extra_stderr=%s extra_status=%d s=[%s] b=[%s] dd=[%s]\n' \
      "${x_p}" "${x_p_warn}" "${x_p_rc}" "${x_ps}" "${x_pb}" "${x_pd}"
    ;;

  *)
    printf 'usage: %s [leading-dash|escapes|suppress-newline|reuse|closed-fd|format-injection|octal|printf-v|percent-off|positional|closed-fd-stderr|extra-operands|external]\n' "$0" >&2
    exit 2
    ;;
esac
