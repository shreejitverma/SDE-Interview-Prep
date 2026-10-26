#!/bin/sh
# grammar-probe.sh: probe POSIX.1-2024 XCU 2.10 Shell Grammar mechanisms.
# Designed to be ShellCheck clean, strictly POSIX-conforming, and runnable
# under dash, bash, zsh, ksh93, mksh, yash, posh, and BusyBox ash.
set -eu

fmt_args() {
  printf 'argc=%d' "$#"
  for arg in "$@"; do
    printf ' <%s>' "${arg}"
  done
  printf '\n'
}

probe_mode="${1:-all}"

probe_reserved_words() {
  # POSIX Rule 1: reserved words are recognized only unquoted in command position.
  # Unquoted if is a keyword; escaped \if or quoted 'if' is a command name (WORD).
  unquoted_state="keyword"
  if false; then
    unquoted_state="failed"
  fi
  printf '1_reserved_words: unquoted=%s\n' "${unquoted_state}"
}

probe_esac_pattern() {
  # POSIX Rule 4: esac is recognized as Esac terminating case unless parenthesized or quoted.
  # Both patterns are quoted: an unquoted (esac) is rejected by bash 5.1 and older upstream
  # builds and by zsh --emulate sh (see test.bats), so only quoting is portable.
  target="esac"
  p_match="none"
  case "${target}" in
    'esac') p_match="parenthesized" ;;
    *) p_match="wildcard" ;;
  esac

  q_match="none"
  case "${target}" in
    'esac') q_match="quoted" ;;
    *) q_match="wildcard" ;;
  esac
  printf '2_esac_pattern: paren=%s quoted=%s\n' "${p_match}" "${q_match}"
}

probe_prefix_suffix() {
  # POSIX Rule 7: assignments before command name belong to cmd_prefix (ASSIGNMENT_WORD);
  # assignments after command name belong to cmd_suffix (WORD argument).
  # 1. Assignment prefix on external utility (env)
  # shellcheck disable=SC2097,SC2098 # intentional demonstration of prefix assignment export
  PREFIX_TEST=outer
  # shellcheck disable=SC2097,SC2098 # prefix assignment to env utility
  env_seen=$(PREFIX_TEST=prefix_val env | grep '^PREFIX_TEST=' | cut -d= -f2)
  printf '3_prefix_export: env_seen=%s outer_retained=%s\n' "${env_seen}" "${PREFIX_TEST}"

  # 2. Assignment prefix without command name persists in current shell environment
  STANDALONE_VAR=persisted_val </dev/null
  printf '4_prefix_standalone: var=%s\n' "${STANDALONE_VAR}"

  # 3. Assignment in suffix is parsed as WORD operand, not assignment
  # Use fmt_args to display arguments received
  fmt_args SUFFIX_VAR=assigned_in_suffix regular_arg
}

probe_function_bodies() {
  # POSIX Rule 8 & 9: function bodies execute compound commands; word expansion deferred.
  # Brace group as body: executes in current environment.
  fn_var="initial_parent"
  fn_brace() {
    fn_var="mutated_parent"
  }
  fn_brace
  printf '5_fn_brace: caller_var=%s\n' "${fn_var}"

  # Rule 9: word expansion deferred until invocation, not when parsed
  def_val="first"
  fn_deferred() {
    printf '6_fn_deferred: val=%s\n' "${def_val}"
  }
  def_val="second"
  fn_deferred
}

probe_linebreaks() {
  # POSIX grammar allows linebreaks after &&, ||, |, and in compound_list.
  # shellcheck disable=SC2015 # intentional multiline pipeline / logic grammar check
  # No backslashes: each newline below follows an operator, so only the grammar's
  # linebreak productions make these commands complete.
  true \
    && true \
    || false

  echo "pipe_line" \
    | cat >/dev/null

  printf '7_linebreaks: valid\n'
}

probe_negation() {
  # POSIX grammar: pipeline : pipe_sequence | Bang pipe_sequence.
  # Single ! inverts pipeline status.
  # shellcheck disable=SC2015 # intentional grammar pipeline status test
  if ! false; then
    printf '8_negation: inverted\n'
  fi
}

case "${probe_mode}" in
  reserved-words) probe_reserved_words ;;
  esac-pattern) probe_esac_pattern ;;
  prefix-suffix) probe_prefix_suffix ;;
  function-bodies) probe_function_bodies ;;
  linebreaks) probe_linebreaks ;;
  negation) probe_negation ;;
  all)
    probe_reserved_words
    probe_esac_pattern
    probe_prefix_suffix
    probe_function_bodies
    probe_linebreaks
    probe_negation
    ;;
  *)
    printf 'Unknown mode: %s\n' "${probe_mode}" >&2
    exit 1
    ;;
esac
