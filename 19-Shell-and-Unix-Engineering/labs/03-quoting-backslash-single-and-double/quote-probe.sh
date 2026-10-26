#!/bin/sh
# quote-probe.sh: probe shell quoting semantics across backslash, single, and double quotes.
# Used by the quoting lab to verify POSIX.1-2024 compliance and shell divergences.

set -eu

mode="${1:-help}"
shift || true

case "$mode" in
  count-args)
    printf 'count=%d\n' "$#"
    printf 'args:'
    for arg in "$@"; do
      printf ' <%s>' "$arg"
    done
    printf '\n'
    ;;

  backslash-escapes)
    # Unquoted backslash before non-special vs double-quoted backslash
    u_star=\*
    dq_star="\*"
    dq_dollar="\$"
    dq_a="\a"
    dq_quote="\""
    dq_n="\n"

    printf 'unquoted_star: <%s>\n' "$u_star"
    printf 'dq_star: <%s>\n' "$dq_star"
    printf 'dq_dollar: <%s>\n' "$dq_dollar"
    printf 'dq_a: <%s>\n' "$dq_a"
    printf 'dq_quote: <%s>\n' "$dq_quote"
    printf 'dq_n: <%s>\n' "$dq_n"
    ;;

  line-continuation)
    # Line continuation: unquoted, double-quoted, single-quoted
    # shellcheck disable=SC1007 # Testing literal line continuations
    val_unquoted=firstsecond

    val_dq="first\
second"

    val_sq='first\
second'

    printf 'unquoted: <%s>\n' "$val_unquoted"
    printf 'double_quoted: <%s>\n' "$val_dq"
    printf 'single_quoted: <%s>\n' "$val_sq"
    ;;

  empty-retention)
    empty_var=""
    # Test field splitting on unquoted empty vs quoted empty
    # shellcheck disable=SC2086 # SC2086 intentionally unquoted to test field splitting
    set -- $empty_var
    unquoted_cnt="$#"

    # shellcheck disable=SC2248 # Explicitly testing quoted expansion
    set -- "$empty_var"
    quoted_cnt="$#"

    printf 'unquoted_count=%d\n' "$unquoted_cnt"
    printf 'quoted_count=%d\n' "$quoted_cnt"
    ;;

  param-joining)
    ifs_mode="$1"
    shift
    case "$ifs_mode" in
      NULL)
        IFS=""
        ;;
      DEFAULT)
        unset IFS
        ;;
      COLON)
        IFS=":"
        ;;
      *)
        IFS="$ifs_mode"
        ;;
    esac

    # shellcheck disable=SC2248 # Testing "$*" and "$@" parameter expansions
    res_star="$*"
    res_at_cnt="$#"

    printf 'star: <%s>\n' "$res_star"
    printf 'at_count=%d\n' "$res_at_cnt"
    ;;

  subst-nesting)
    # Nested quotes inside $() vs backticks
    res_dollar="$(printf '%s' "nested $(printf '%s' "inner")")"
    res_dollar_escaped="$(printf '%s' "nested $(printf '%s' \"inner\")")"
    # Real nested backticks: depth 2 needs 1 backslash, depth 3 needs 3.
    # The backticks live in eval strings because shfmt rewrites backticks as $(...).
    res_backtick='' res_backtick3=''
    # shellcheck disable=SC2016 # backticks are expanded by eval, not here
    eval 'res_backtick="nested `printf %s \`printf %s inner\``"'
    # shellcheck disable=SC2016 # backticks are expanded by eval, not here
    eval 'res_backtick3=`echo 1 \`echo 2 \\\`echo 3\\\`\``'

    printf 'dollar_subst: <%s>\n' "$res_dollar"
    printf 'dollar_escaped: <%s>\n' "$res_dollar_escaped"
    printf 'backtick_subst: <%s>\n' "$res_backtick"
    printf 'backtick_depth3: <%s>\n' "$res_backtick3"
    ;;

  pattern-quoting)
    str="$1"
    ptype="$2"
    case "$ptype" in
      unquoted)
        # shellcheck disable=SC2295 # Testing pattern matching with unquoted wildcard
        res="${str#*c*}"
        ;;
      quoted_middle)
        res="${str#*"c*"}"
        ;;
      quoted_literal)
        res="${str#*'c*'}"
        ;;
      *)
        printf 'Unknown pattern type: %s\n' "$ptype" >&2
        exit 1
        ;;
    esac
    printf 'result: <%s>\n' "$res"
    ;;

  *)
    printf 'Usage: quote-probe.sh <mode> [args...]\n' >&2
    exit 1
    ;;
esac
