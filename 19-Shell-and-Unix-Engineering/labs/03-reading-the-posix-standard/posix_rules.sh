#!/bin/sh
# posix_rules.sh: Probe script verifying POSIX.1-2024 shell language rules.
# Exercises environment persistence, error abort consequences, declaration
# utilities, command search precedence, and standard exit statuses.

set -u

mode="${1:-}"
shift || true

case "${mode}" in
  assign-persist)
    unset PERSIST_VAL
    PERSIST_VAL=active :
    colon_result="${PERSIST_VAL:-unset}"
    unset PERSIST_VAL
    PERSIST_VAL=active true
    true_result="${PERSIST_VAL:-unset}"
    printf 'colon:%s true:%s\n' "${colon_result}" "${true_result}"
    ;;

  error-abort)
    submode="${1:-}"
    file="${2:-}"
    case "${submode}" in
      --direct)
        # Redirection error directly on special built-in (XCU 2.8.1)
        # Put 2>/dev/null first so diagnostic is silenced under left-to-right evaluation
        : 2>/dev/null <"${file}"
        printf 'direct-survived\n'
        ;;
      --shielded)
        # Redirection error shielded by command utility (XCU 2.8.1 Note 1)
        command : 2>/dev/null <"${file}"
        printf 'shielded-survived\n'
        ;;
      *)
        printf 'Unknown error-abort submode: %s\n' "${submode}" >&2
        exit 2
        ;;
    esac
    ;;

  declaration-context)
    submode="${1:-}"
    val="alpha beta gamma"
    case "${submode}" in
      --literal)
        # Literal declaration utility: assignment context suppresses field splitting (XCU 2.9.1.1)
        # shellcheck disable=SC2086,SC2248 # intentional unquoted expansion to verify assignment context
        export DECL_VAR=${val}
        printf '<%s>\n' "${DECL_VAR}"
        ;;
      --dynamic)
        # Dynamic expansion: lexical parsing does not see export (XCU 2.9.1.1)
        cmd='export'
        # shellcheck disable=SC2086,SC2248 # intentional dynamic command word expansion
        ${cmd} DECL_VAR=${val}
        printf '<%s>\n' "${DECL_VAR}"
        ;;
      *)
        printf 'Unknown declaration-context submode: %s\n' "${submode}" >&2
        exit 2
        ;;
    esac
    ;;

  function-precedence)
    submode="${1:-}"
    case "${submode}" in
      --regular)
        # Functions shadow regular built-ins and external utilities (XCU 2.9.1.4)
        true() {
          printf 'shadowed-true\n'
        }
        true
        ;;
      --special)
        # Function definition with special built-in name (XCU 2.9.5, XCU 2.9.1.4).
        # Run inside eval within a subshell so that syntax errors during definition
        # on strict POSIX shells (dash, posh, ksh93, yash, bash posix) do not abort the script.
        res=$(eval '(:() { printf "func-colon\n"; }; :)' 2>/dev/null)
        rc=$?
        if [ "${rc}" -ne 0 ]; then
          printf 'rejected\n'
        elif [ "${res}" = "func-colon" ]; then
          printf 'function-ran\n'
        else
          printf 'builtin-ran\n'
        fi
        ;;
      *)
        printf 'Unknown function-precedence submode: %s\n' "${submode}" >&2
        exit 2
        ;;
    esac
    ;;

  lookup-status)
    target="${1:-}"
    if [ -z "${target}" ]; then
      printf 'lookup-status requires target operand\n' >&2
      exit 2
    fi
    exit_code=0
    "${target}" 2>/dev/null || exit_code="$?"
    printf '%d\n' "${exit_code}"
    ;;

  *)
    printf 'Usage: %s {assign-persist|error-abort|declaration-context|function-precedence|lookup-status} [args...]\n' "$0" >&2
    exit 2
    ;;
esac
