#!/bin/sh
# errexit-probe: mechanism probes for set -e (errexit) semantics and exceptions.
# Conforms to POSIX.1-2024 XCU 2.15 (set) and related execution environment rules.
# Usage: errexit-probe.sh andor
#        errexit-probe.sh fn_cond
#        errexit-probe.sh cmdsub_assign
#        errexit-probe.sh cmdsub_inherit
#        errexit-probe.sh pipe_pipefail
#        errexit-probe.sh subshell_pipe
#        errexit-probe.sh fn_invert
#        errexit-probe.sh trap_exit

mode=${1:?mode}

case $mode in
  andor)
    # Exception 2: -e is ignored for any command in an AND-OR list other than the last.
    # Non-terminal operand: false && true ignores -e for false; true is skipped; shell survives.
    # shellcheck disable=SC2015 # intentional test of set -e on and-or list
    out_nonterm=$(
      set -e
      false && true
      printf 'survived'
    )
    st_nonterm=$?

    # Terminal operand: true && false must cause set -e to trigger on false (the last command).
    # posh 0.14.1 diverges by suppressing set -e across all operands (pdksh XERROK bug).
    # shellcheck disable=SC2015 # intentional test of set -e on and-or list
    out_term=$(
      set -e
      true && false
      printf 'survived'
    )
    st_term=$?

    printf 'nonterm_st=%d nonterm_out=%s term_st=%d term_out=%s\n' \
      "$st_nonterm" "${out_nonterm:-empty}" "$st_term" "${out_term:-empty}"
    ;;

  fn_cond)
    # Exception 2: -e is ignored when executing a compound list following if/while/until or on LHS of ||.
    # When a function is invoked in these contexts, -e is suppressed for commands inside the function.
    fn_if=$(
      set -e
      inner() {
        false
        printf 'reached'
      }
      # shellcheck disable=SC2310 # intentional probe of set -e suppression inside function in if condition
      if inner; then
        printf ' branch'
      fi
    )

    fn_or=$(
      set -e
      inner() {
        false
        printf 'reached'
      }
      # shellcheck disable=SC2310 # intentional probe of set -e suppression inside function in || condition
      inner || true
    )

    printf 'if=%s or=%s\n' "$fn_if" "$fn_or"
    ;;

  cmdsub_assign)
    # POSIX.1-2024 XCU 2.9.1:
    # In bare assignment (no command word), exit status is that of the last command substitution.
    # Under set -e, status > 0 causes immediate shell exit.
    # In utility declarations (export, readonly, local), the command word is the utility.
    # The utility returns 0, masking the command substitution failure (SC2155).
    out_bare=$(
      set -e
      # shellcheck disable=SC2030,SC2034 # x is intentionally assigned in subshell to probe status
      x=$(exit 42)
      printf 'survived'
    )
    st_bare=$?

    out_export=$(
      set -e
      # shellcheck disable=SC2031,SC2155 # intentional probe of declaration return masking in subshell
      export x="$(exit 42)"
      printf 'survived'
    )
    st_export=$?

    printf 'bare_st=%d bare_out=%s export_st=%d export_out=%s\n' \
      "$st_bare" "${out_bare:-empty}" "$st_export" "${out_export:-empty}"
    ;;

  cmdsub_inherit)
    # POSIX.1-2024 XCU 2.15 example (Austin Group Defect 1150):
    # set -e; echo $(false; echo one) two
    # In POSIX conforming shells, command substitution subshell inherits set -e.
    # false causes the subshell to exit without executing echo one; word expands to "two".
    # In native bash (non-posix mode) and BusyBox ash, set -e is disabled in command substitutions,
    # resulting in "one two" unless shopt -s inherit_errexit is enabled in bash.
    # shellcheck disable=SC2312 # intentional probe of command substitution errexit inheritance
    out_inherit=$(
      set -e
      printf '%s\n' "$(
        false
        printf 'one '
      )two"
    )
    st_inherit=$?

    printf 'inherit_st=%d inherit_out=%s\n' "$st_inherit" "$out_inherit"
    ;;

  pipe_pipefail)
    # Exception 1: failure of individual command in multi-command pipeline does not trigger set -e.
    # Under default semantics, pipeline status is that of the last stage.
    # Under set -o pipefail (POSIX.1-2024), pipeline status is the rightmost non-zero stage.
    # shellcheck disable=SC2312 # intentional probe of pipeline status
    out_default=$(
      set -e
      false | true
      printf 'survived'
    )
    st_default=$?

    # Feature-detect set -o pipefail
    # shellcheck disable=SC3040 # pipefail is feature-detected
    if (set -o pipefail 2>/dev/null); then
      # shellcheck disable=SC3040 # pipefail is feature-detected
      out_pipefail=$(
        set -e
        set -o pipefail
        # shellcheck disable=SC2312 # intentional probe of pipeline status
        false | true
        printf 'survived'
      )
      st_pipefail=$?
    else
      out_pipefail="unsupported"
      st_pipefail=-1
    fi

    printf 'def_st=%d def_out=%s pf_st=%d pf_out=%s\n' \
      "$st_default" "${out_default:-empty}" "$st_pipefail" "${out_pipefail:-empty}"
    ;;

  subshell_pipe)
    # POSIX.1-2024 XCU 2.15 example:
    # set -e; (false; echo one) | cat; echo two
    # false causes the subshell to exit without executing echo one.
    # However, echo two is executed because the exit status of the pipeline is zero.
    # shellcheck disable=SC2312 # intentional probe of subshell pipeline
    out=$(
      set -e
      (
        false
        printf 'one\n'
      ) | cat
      printf 'two\n'
    )
    printf 'subshell_pipe=%s\n' "$out"
    ;;

  fn_invert)
    # Exception 2: pipeline beginning with ! ignores -e.
    # When a function executes ! true, the ! pipeline succeeds in running without aborting inside.
    # But when the function returns 1, the caller's simple command f has status 1.
    # In bash, dash, ksh93, mksh, yash, busybox: simple command f triggers set -e (exits 1).
    # In zsh and posh: set -e is suppressed on the function return, surviving.
    out_invert=$(
      set -e
      f() {
        ! true
      }
      f
      printf 'survived'
    )
    st_invert=$?

    printf 'invert_st=%d invert_out=%s\n' "$st_invert" "${out_invert:-empty}"
    ;;

  trap_exit)
    # POSIX.1-2024 XCU 2.15:
    # When set -e causes the shell to exit, it executes the exit special built-in utility
    # with no arguments, preserving the exit status and executing the EXIT trap.
    out_trap=$(
      set -e
      trap 'printf "trap=%d\n" "$?"' EXIT
      false
      printf 'survived\n'
    )
    st_trap=$?

    printf 'trap_st=%d trap_out=%s\n' "$st_trap" "$out_trap"
    ;;

  *)
    printf 'unknown mode: %s\n' "$mode" >&2
    exit 1
    ;;
esac
