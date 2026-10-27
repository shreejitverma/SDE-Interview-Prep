#!/bin/sh
# Probe for "Null versus Unset": prints what each parameter-expansion operator does to an
# unset, a null (set to the empty string) and a non-null variable.
# Usage: probe.sh MODE
#   table    the eight operators against the three states
#   classify how a test can tell the three states apart
#   declared what export, readonly and unset do to the state
#   nounset  which expansions fail under set -u
#   lazy     whether the word is expanded only when it is needed
#   error    what ${x:?} and ${x?} do to the shell
#   ifs      unset IFS versus null IFS
#   environ  what a child process sees for an unset and a null variable
# shellcheck disable=SC2016 # single quotes keep the probed expansions literal

mode=${1-}

# state NAME: print unset, null or set, computed with ${NAME+s} and a length test.
state() {
  eval "if [ \"\${$1+s}\" != s ]; then echo unset; elif [ -z \"\$$1\" ]; then echo null; else echo set; fi"
}

# put STATE: put x into STATE.
put() {
  case $1 in
    unset) unset x ;;
    null) x= ;;
    set) x=v ;;
    *) return 1 ;;
  esac
}

# show: print x as <unset>, <> or <value>.
show() {
  if [ "${x+s}" = s ]; then printf '<%s>' "$x"; else printf '<unset>'; fi
}

mode_table() {
  for st in unset null set; do
    for op in ':-' '-' ':=' '=' ':+' '+'; do
      put "$st"
      eval 'r=${x'"$op"'W}'
      printf '%s ${x%sW} -> result=<%s> x=' "$st" "$op" "$r"
      show
      printf '\n'
    done
    for op in ':?' '?'; do
      put "$st"
      if r=$(eval 'printf "%s" "${x'"$op"'W}"' 2>/dev/null); then
        printf '%s ${x%sW} -> result=<%s>\n' "$st" "$op" "$r"
      else
        printf '%s ${x%sW} -> error\n' "$st" "$op"
      fi
    done
  done
}

mode_classify() {
  unset x
  v=$(state x)
  printf 'unset %s\n' "$v"
  x=
  v=$(state x)
  printf 'x= %s\n' "$v"
  x=v
  v=$(state x)
  printf 'x=v %s\n' "$v"
  unset x
  if [ -z "$x" ] 2>/dev/null; then echo "z-on-unset true"; else echo "z-on-unset false"; fi
  x=
  if [ -z "$x" ]; then echo "z-on-null true"; else echo "z-on-null false"; fi
}

mode_declared() {
  unset x y
  export x
  v=$(state x)
  printf 'export-only %s\n' "$v"
  readonly y
  v=$(state y)
  printf 'readonly-only %s\n' "$v"
  z=1
  : "$z"
  unset z
  v=$(state z)
  printf 'assign-then-unset %s\n' "$v"
  unset never_was_set
  printf 'unset-of-unset status %s\n' "$?"
}

mode_nounset() {
  x=
  for expr in '${u}' '${u-}' '${u:-d}' '${u+a}' '${u:+a}' '${#u}' '$((u))' '${u%x}' '${x}' '${x:-d}' '$(( x ))' '$1' '${1-}' '${1+"$@"}' '"$@"' '"$*"' '$#' '$?'; do
    if (
      set -u
      set --
      unset u
      eval "echo $expr" >/dev/null 2>&1
    ); then
      printf '%s ok\n' "$expr"
    else
      printf '%s fail\n' "$expr"
    fi
  done
}

mode_lazy() {
  n=0
  x=v
  : "${x:-$((n = n + 1))}"
  echo "set:-  n=$n"
  : "${x:+$((n = n + 10))}"
  echo "set:+  n=$n"
  unset x
  : "${x-$((n = n + 100))}"
  echo "unset- n=$n"
  : "${x+$((n = n + 1000))}"
  echo "unset+ n=$n"
}

mode_error() {
  case ${2-} in
    colon-unset)
      unset x
      : "${x:?missing}"
      ;;
    colon-null)
      x=
      : "${x:?missing}"
      ;;
    plain-null)
      x=
      : "${x?missing}"
      ;;
    plain-unset)
      unset x
      : "${x?missing}"
      ;;
    subshell)
      unset x
      (: "${x:?missing}") 2>/dev/null
      echo "parent continued"
      ;;
    *) exit 64 ;;
  esac
  echo reached
}

mode_ifs() {
  set -- a b c
  IFS=
  printf 'null "$*" <%s>\n' "$*"
  v='p q'
  # shellcheck disable=SC2086 # the field splitting is the point
  set -- $v
  printf 'null split %s\n' "$#"
  unset IFS
  set -- a b c
  printf 'unset "$*" <%s>\n' "$*"
  # shellcheck disable=SC2086 # the field splitting is the point
  set -- $v
  printf 'unset split %s\n' "$#"
}

mode_environ() {
  unset x
  export y=
  # shellcheck disable=SC2154 # lab input
  : "${getenv:?path of the getenv helper}"
  v=$("$getenv" x)
  printf 'unset-var child: %s\n' "$v"
  x=
  v=$("$getenv" x)
  printf 'unexported-null child: %s\n' "$v"
  export x
  v=$("$getenv" x)
  printf 'exported-null child: %s\n' "$v"
  v=$(env | grep -c '^x=$')
  printf 'exported-null-in-env: %s\n' "$v"
  unset x
  v=$("$getenv" x)
  printf 'after-unset child: %s\n' "$v"
  v=$(x='' "$getenv" x)
  printf 'prefix-assignment child: %s\n' "$v"
}

case $mode in
  table | classify | declared | nounset | lazy | error | ifs | environ) "mode_$mode" "$@" ;;
  *)
    echo "usage: probe.sh table|classify|declared|nounset|lazy|error|ifs|environ" >&2
    exit 64
    ;;
esac
