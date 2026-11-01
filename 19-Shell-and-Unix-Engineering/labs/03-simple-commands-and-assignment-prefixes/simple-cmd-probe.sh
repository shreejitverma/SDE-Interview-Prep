#!/bin/sh
# simple-cmd-probe.sh: probe POSIX simple-command evaluation order, assignment
# prefix scope, command-less exit status, and declaration-utility splitting.
set -u

mode=${1:-order}
if [ "$#" -gt 0 ]; then
  shift
fi

case ${mode} in
  dump-env)
    printf 'arg1=%s arg2=%s child_env=%s ' "${1-}" "${2-}" "${x-}"
    ;;
  order)
    x=before
    # shellcheck disable=SC2097,SC2098 # intentional: Step 2 argument expansion precedes Step 4 prefix assignment
    x=after "$0" dump-env "${x}" trailing=word
    printf 'parent_after=%s\n' "${x}"
    ;;
  no-cmd-status)
    # shellcheck disable=SC2312 # intentional: testing exit status of bare assignment command substitutions
    a=$(exit 3) b=$(exit 0)
    s1=$?
    # shellcheck disable=SC2312 # intentional: second command substitution determines status
    a=$(exit 0) b=$(exit 7)
    s2=$?
    # shellcheck disable=SC2155,SC2312 # intentional: export return value masks command substitution status
    export c="$(exit 7)"
    s3=$?
    printf 'last_zero=%d last_seven=%d export_masked=%d a=%s b=%s c=%s\n' \
      "${s1}" "${s2}" "${s3}" "${a}" "${b}" "${c}"
    ;;
  scope)
    v=before
    v=bare
    bare_res=${v}
    empty=""
    # shellcheck disable=SC2086,SC2248,SC2250 # intentional: unquoted empty command word leaves no command name
    v=leaked $empty
    empty_res=${v}
    r=before
    r=during true
    reg_res=${r}
    s=before
    s=during :
    spcl_res=${s}
    fn() { :; }
    f=before
    f=during fn
    fn_res=${f}
    printf 'bare=%s empty_cmd=%s regular=%s special=%s func=%s\n' \
      "${bare_res}" "${empty_res}" "${reg_res}" "${spcl_res}" "${fn_res}"
    ;;
  decl-split)
    val="1 b=2"
    b=keep
    q=""
    # shellcheck disable=SC2086,SC2248,SC2250 # intentional: testing declaration-utility assignment context on unquoted $val
    export a=$val
    decl_a=${a}
    decl_b=${b}
    b=keep
    # shellcheck disable=SC1001,SC2086,SC2248,SC2250 # intentional: quoting variable name disables assignment context
    export \q=$val
    printf 'decl_a=%s decl_b=%s quoted_q=%s quoted_b=%s\n' \
      "${decl_a}" "${decl_b}" "${q}" "${b}"
    ;;
  *)
    printf 'usage: %s [order|dump-env|no-cmd-status|scope|decl-split]\n' "$0" >&2
    exit 2
    ;;
esac
