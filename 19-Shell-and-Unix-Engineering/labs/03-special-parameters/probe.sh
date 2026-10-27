#!/bin/sh
# probe: print what each special parameter expands to, one mode per behavior.
# Usage: probe.sh MODE [ARG...]
# Every line a mode prints is computed by the shell running this script.

# fields ARG...: print the argument count, then each argument between angle brackets.
fields() {
  printf '%d:' "$#"
  if [ "$#" -gt 0 ]; then
    printf ' <%s>' "$@"
  fi
  printf '\n'
}

# comm_of PID: print the executable name of a process.
comm_of() {
  if [ -r "/proc/$1/comm" ]; then
    cat "/proc/$1/comm"
  else
    ps -o comm= -p "$1" | sed 's|.*/||'
  fi
}

mode=${1-}
[ "$#" -eq 0 ] || shift

case $mode in
  at-star)
    # "$@" keeps every field, "$*" joins them; the unquoted forms split and drop empty fields.
    set -- 'a b' '' c
    fields "$@"
    fields "$*"
    # shellcheck disable=SC2068,SC2048,SC2086,SC2248 # the unquoted forms are the subject
    fields $@
    # shellcheck disable=SC2048,SC2086,SC2248 # the unquoted form is the subject
    fields $*
    ;;
  ifs-join)
    # "$*" joins with the first character of IFS, with a space if IFS is unset, with nothing if IFS is null.
    set -- a b c
    IFS=:
    fields "$*"
    IFS=
    fields "$*"
    unset IFS
    fields "$*"
    IFS=':-'
    fields "$*"
    ;;
  empty)
    # With no positional parameters "$@" is zero fields and "$*" is one empty field.
    set --
    fields "$@"
    fields "$*"
    fields "$@" ''
    # shellcheck disable=SC2145 # the embedded "$@" is the subject
    fields x"$@"
    ;;
  embed)
    # "$@" inside a word glues the prefix to the first field and the suffix to the last field.
    set -- a b c
    # shellcheck disable=SC2145 # the embedded "$@" is the subject
    fields "pre$@post"
    set -- only
    # shellcheck disable=SC2145 # the embedded "$@" is the subject
    fields "pre$@post"
    ;;
  count)
    # $# counts positional parameters only; $0 is not one of them; shift renumbers them.
    set -- a b c
    echo "$#"
    shift
    echo "$#" "$1"
    shift 2
    echo "$#"
    set -- "$@" x y
    echo "$#" "$*"
    ;;
  tenth)
    # $10 is $1 followed by a zero; ${10} is the tenth parameter.
    set -- one two three four five six seven eight nine ten
    eval 'echo "$10"'
    echo "${10}"
    ;;
  for-default)
    # A for loop without "in" iterates over "$@".
    set -- 'a b' '' c
    n=0
    for x; do
      n=$((n + 1))
      printf '%s=<%s>\n' "$n" "$x"
    done
    ;;
  status)
    # $? is the status of the most recent pipeline: a negated pipeline, a pipeline's last stage, an assignment with a substitution.
    true
    echo "init $?"
    false
    echo "after-false $?"
    ! true
    echo "negated $?"
    ! false
    echo "negated-false $?"
    true | false
    echo "pipe-last $?"
    false | true
    echo "pipe-last-ok $?"
    x=$(exit 7)
    echo "assign-subst $?"
    # shellcheck disable=SC2312 # the status of the prefix assignment is the subject
    x=$(exit 7) true
    echo "prefix-assign $?"
    (exit 5)
    echo "subshell $?"
    false
    (echo "preserved $?")
    false
    echo "$?" "$?"
    ;;
  status-signal)
    # A command killed by a signal gives a status above 128 that encodes the signal.
    { /bin/sh -c 'kill -TERM $$'; } 2>/dev/null
    echo "$?"
    ;;
  pid)
    # $$ is the invoked shell's pid, unchanged in subshells and in command substitutions.
    me=$$
    # shellcheck disable=SC2116 # the substitution must expand $$ itself
    sub=$(echo "$$")
    grp=$( (echo "$$") )
    [ "$me" = "$sub" ] && echo same-in-substitution
    [ "$me" = "$grp" ] && echo same-in-subshell
    # A child shell has its own pid.
    child=$(/bin/sh -c 'echo $$')
    [ "$child" != "$me" ] && echo child-differs
    kill -0 "$me" && echo live
    ;;
  bang)
    # $! is the pid of the last asynchronous list and survives later foreground commands.
    # The first job sleeps 2 seconds so the poll below has more than a second to see it running.
    /bin/sleep 2 &
    p=$!
    tries=0
    name=$(comm_of "$p")
    until [ "$name" = sleep ] || [ "$tries" -ge 30 ]; do
      tries=$((tries + 1))
      /bin/sleep 0.1
      name=$(comm_of "$p")
    done
    echo "comm=$name"
    [ "$p" != "$$" ] && echo not-shell
    /bin/sleep 0
    [ "$p" = "$!" ] && echo kept-after-foreground
    /bin/sleep 1 &
    [ "$p" != "$!" ] && echo updated-by-next
    q=$!
    wait "$p"
    echo "wait-first $?"
    wait "$q"
    echo "wait-second $?"
    ;;
  options)
    # $- lists the single-letter options currently in force.
    case $- in *u*) echo u-on ;; *) echo u-off ;; esac
    set -u
    case $- in *u*) echo u-on ;; *) echo u-off ;; esac
    set +u
    case $- in *u*) echo u-on ;; *) echo u-off ;; esac
    set -f
    case $- in *f*) echo f-on ;; *) echo f-off ;; esac
    set +f
    case $- in *f*) echo f-on ;; *) echo f-off ;; esac
    case $- in *i*) echo interactive ;; *) echo not-interactive ;; esac
    ;;
  zero)
    # $0 names the script and does not change inside a function.
    echo "top $0"
    f() { echo "in-function $0"; }
    f
    ;;
  function-args)
    # A function call replaces the positional parameters and restores them on return.
    set -- outer1 outer2
    f() {
      echo "in $# $1"
      set -- changed
      echo "set-in $# $1"
    }
    f a b c
    echo "out $# $1 $2"
    ;;
  unset-at)
    # "$@" and "$*" are exempt from set -u when empty (POSIX); $1 is not.
    set -u
    set --
    fields "$@"
    fields "$*"
    ;;
  shift-too-many)
    # shift past $# is an error: a special built-in error ends some shells and returns non-zero in others.
    set -- a b
    (
      shift 3
      echo "continued $? $#"
    ) 2>/dev/null
    echo "subshell $?"
    ;;
  unset-one)
    # An unset numbered parameter under set -u aborts the shell.
    exec 2>/dev/null
    set -u
    set --
    echo "before"
    echo "$1"
    echo "not-reached"
    ;;
  *)
    echo "usage: probe.sh MODE" >&2
    exit 2
    ;;
esac
