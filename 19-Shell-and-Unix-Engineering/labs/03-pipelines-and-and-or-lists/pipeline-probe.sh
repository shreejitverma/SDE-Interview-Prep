#!/bin/sh
# pipeline-probe: mechanism probes for pipelines and and-or lists.
# Usage: pipeline-probe.sh assoc
#        pipeline-probe.sh subshell
#        pipeline-probe.sh pipefail
#        pipeline-probe.sh errexit
#        pipeline-probe.sh redir DIR

mode=${1:?mode}

case $mode in
  assoc)
    # Probe 1: Left-associativity of && and || (equal precedence).
    # In C, && binds tighter than ||, so (true || (false && true)) would print nothing.
    # In POSIX shell, equal precedence and left associativity evaluate ((true || false) && echo r1).
    res1=$(true || echo "skip1" && echo "run1")

    # Probe 2: Short-circuiting and the fake ternary trap.
    # In a ternary condition ? true_branch : false_branch, if condition is true,
    # the false_branch must never execute.
    # In shell A && B || C, if B returns non-zero, C executes even though A succeeded.
    res2=$([ "$mode" = "assoc" ] && false || echo "fallback")

    printf 'assoc=%s ternary=%s\n' "$res1" "$res2"
    ;;

  subshell)
    # Test whether the last pipeline stage executes in the current shell environment
    # or in a subshell environment (POSIX.1-2024 XCU 2.13).
    val="initial"
    # shellcheck disable=SC2030 # val is intentionally modified inside pipeline stage
    printf 'data\n' | { val="modified"; }
    # shellcheck disable=SC2031 # intentional probe whether pipeline subshell modified parent val
    printf 'val=%s\n' "$val"
    ;;

  pipefail)
    # Test pipeline exit status derivation with and without pipefail.
    # Without pipefail, exit status is always the last command (POSIX.1-2024 XCU 2.9.2).
    # shellcheck disable=SC2312 # intentional probe of exit status
    false | true
    st_default=$?

    # Feature-detect set -o pipefail (standardized in POSIX.1-2024, extension in older shells)
    # shellcheck disable=SC3040 # pipefail is feature-detected
    if (set -o pipefail 2>/dev/null); then
      # Under pipefail, exit status is the rightmost non-zero command
      # shellcheck disable=SC2312,SC3040 # intentional probe of pipefail status
      st_pipefail=$(
        set -o pipefail
        false | true
        printf '%d' "$?"
      )
    else
      st_pipefail="unsupported"
    fi

    printf 'default=%d pipefail=%s\n' "$st_default" "$st_pipefail"
    ;;

  errexit)
    # Test set -e suppression rules in AND-OR lists (POSIX.1-2024 XCU 2.15).
    # In false && true, -e is ignored for false (not the last command), and true is skipped.
    # The shell must survive.
    # shellcheck disable=SC2015 # intentional test of set -e on and-or list
    surv_first=$(
      set -e
      false && true
      printf 'survived'
    )

    # In true && false, false IS the last command, so -e is NOT ignored.
    # The subshell must exit immediately on false without reaching the printf.
    # shellcheck disable=SC2015 # intentional test of set -e on and-or list
    surv_last=$(
      set -e
      true && false
      printf 'survived'
    )
    [ -z "$surv_last" ] && surv_last="exited"

    printf 'first_operand=%s last_operand=%s\n' "$surv_first" "$surv_last"
    ;;

  redir)
    # Test pipeline redirection binding order (POSIX.1-2024 XCU 2.9.2).
    # Pipeline connects stdout to pipe BEFORE command redirections are processed.
    # Thus >file redirects stdout away from the pipe into the file,
    # and the downstream command receives EOF (0 bytes).
    dir=${2:?dir}
    outfile="$dir/pipeline.out"
    # shellcheck disable=SC2260 # intentional test that redirection overrides the pipeline stdout
    read_bytes=$(
      printf 'payload\n' >"$outfile" | wc -c
    )
    # Strip any whitespace from wc output
    read_bytes=$(printf '%d' "$read_bytes")
    file_bytes=$(wc -c <"$outfile")
    file_bytes=$(printf '%d' "$file_bytes")
    printf 'downstream_bytes=%d file_bytes=%d\n' "$read_bytes" "$file_bytes"
    ;;

  *)
    exit 2
    ;;
esac
