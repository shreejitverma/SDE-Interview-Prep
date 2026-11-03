#!/bin/sh
# eval-probes.sh: exercise eval builtin mechanisms across shell implementations.

set -e

mode=${1:-}
shift || true

case "$mode" in
  concat-space)
    # Demonstrate that eval joins multiple arguments with space characters regardless of IFS.
    IFS=:
    # shellcheck disable=SC2294 # intentionally testing eval concatenation with multiple arguments
    eval "printf '[%s]'" "$@"
    printf '\n'
    ;;
  dynamic-pipeline)
    # Demonstrate second-pass parsing of pipeline operators.
    # shellcheck disable=SC2089 # intentionally testing unquoted pipeline command string
    cmd='echo alpha | tr a-z A-Z'
    submode=${1:-eval}
    if [ "$submode" = "eval" ]; then
      eval "$cmd"
    else
      # shellcheck disable=SC2086,SC2090,SC2248 # intentionally unquoted to demonstrate absence of pipeline parsing
      $cmd
    fi
    ;;
  dynamic-redir)
    # Demonstrate second-pass parsing of redirection operators.
    target=$1
    submode=${2:-eval}
    # shellcheck disable=SC2089 # intentionally testing unquoted redirection command string
    cmd="echo payload > $target"
    if [ "$submode" = "eval" ]; then
      eval "$cmd"
    else
      # shellcheck disable=SC2086,SC2090,SC2248 # intentionally unquoted to demonstrate absence of redirection parsing
      $cmd
    fi
    ;;
  env-mutation)
    # Demonstrate that eval modifies the caller environment in the current shell.
    var_x=initial
    var_y=initial
    eval "var_x=modified; var_y=updated"
    printf 'var_x=%s var_y=%s\n' "$var_x" "$var_y"
    ;;
  flow-break)
    # Demonstrate that eval can break caller loop structures.
    idx=0
    for i in 1 2 3 4 5; do
      idx=$i
      if [ "$i" -eq 3 ]; then
        eval "break"
      fi
    done
    printf 'loop_idx=%d\n' "$idx"
    ;;
  flow-return)
    # Demonstrate that eval executes return in the caller function scope.
    code=${1:-7}
    probe_func() {
      eval "return $1"
      printf 'UNREACHED\n'
    }
    set +e
    probe_func "$code"
    st=$?
    set -e
    printf 'func_ret=%d\n' "$st"
    ;;
  prefix-assign)
    # Demonstrate special built-in variable assignment prefix persistence.
    pvar=initial
    pvar=modified eval :
    printf 'pvar=%s\n' "$pvar"
    ;;
  redir-abort)
    # Demonstrate special built-in redirection error causing non-interactive shell to abort.
    set +e
    { eval : </nonexistent_file_for_eval_abort; } 2>/dev/null
    printf 'SURVIVED\n'
    ;;
  cmd-eval-prefix)
    # Demonstrate that the command utility removes special built-in prefix persistence.
    cvar=initial
    cvar=modified command eval :
    printf 'cvar=%s\n' "$cvar"
    ;;
  cmd-redir-abort)
    # Demonstrate that the command utility suppresses non-interactive abort on redirection error.
    set +e
    { command eval : </nonexistent_file_for_eval_abort; } 2>/dev/null
    printf 'SURVIVED\n'
    ;;
  dash-dash)
    # Demonstrate divergence on leading '--' operand vs option handling.
    eval -- echo "'[$1]'"
    ;;
  space-prefix)
    # Demonstrate the XCU eval Application Usage idiom: a leading space keeps a dash-leading operand from being read as an option.
    eval " $1"
    ;;
  bare-operand)
    # Demonstrate the same operand without the leading space, which option-aware shells parse as an eval option.
    eval "$1"
    ;;
  quote-safe)
    # Demonstrate safe single-quote escaping for untrusted data interpolated into eval.
    sq="'"
    raw_payload=$1
    rem=$raw_payload
    out=""
    while case "$rem" in *"$sq"*) true ;; *) false ;; esac do
      prefix=${rem%%"$sq"*}
      out="${out}${prefix}'\\''"
      rem=${rem#*"$sq"}
    done
    escaped="'${out}${rem}'"
    evaluated_safe=""
    eval "evaluated_safe=$escaped"
    printf 'evaluated_safe=[%s]\n' "$evaluated_safe"
    ;;
  incremental)
    # Demonstrate incremental line-by-line vs whole-string evaluation on syntax error.
    eval "$(printf '%s\n%s\n' 'echo FIRST' 'if')"
    printf 'FINISHED\n'
    ;;
  no-operands)
    # Demonstrate that eval with no operands, or only null operands, returns status 0 even after a failure.
    set +e
    false
    eval
    a=$?
    false
    eval ""
    b=$?
    false
    eval "" ""
    c=$?
    printf '%s %s %s\n' "$a" "$b" "$c"
    ;;
  syntax-status)
    # Demonstrate the status of a syntax error in the eval string, and whether the shell survives it.
    set +e
    eval 'if' 2>/dev/null
    printf 'CONTINUED=%s\n' "$?"
    ;;
  arg-max-payload)
    # Demonstrate that an operand larger than ARG_MAX stays inside the shell: eval accepts it, execve(2) rejects it.
    payload=$(awk -v n="$1" 'BEGIN { s = "x"; while (length(s) < n) s = s s; printf "%s", substr(s, 1, n) }')
    actual=""
    eval "actual=$payload"
    printf 'len=%d\n' "${#actual}"
    env_bin=$(command -v env)
    "$env_bin" true "$payload"
    ;;
  *)
    # shellcheck disable=SC2248 # mode in diagnostic message
    printf 'eval-probes: unknown mode: %s\n' "$mode" >&2
    exit 64
    ;;
esac
