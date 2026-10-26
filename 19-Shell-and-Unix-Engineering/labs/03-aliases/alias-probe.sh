#!/bin/sh
# Probe POSIX XCU 2.3.1 Alias Substitution semantics across shells.
set -eu

# Ensure alias expansion is enabled in non-interactive bash
# shellcheck disable=SC3044 # shopt is guarded by BASH_VERSION check
if [ -n "${BASH_VERSION-}" ]; then
  shopt -s expand_aliases
fi

# 1. Trailing space chaining
# An alias whose value ends with an unquoted blank causes the next token
# to be checked for alias substitution.
echo_func() {
  printf 'pfx: %s\n' "${*}"
}
alias echo_pfx='echo_func '
alias target_cmd='target_expanded'
printf '1_trailing_space: '
echo_pfx target_cmd

# 2. Quoted bypass
# Quoting any character of a token prevents alias substitution.
probe_cmd() {
  printf 'real_cmd:%s' "${*}"
}
alias probe_cmd='printf "alias_active:%s"'
res1=$(probe_cmd normal)
# shellcheck disable=SC1001 # intentional backslash to test alias suppression
res2=$(\probe_cmd backslash)
res3=$("probe_cmd" doublequote)
res4=$('probe_cmd' singlequote)
printf '2_quoted_bypass:  %s | %s | %s | %s\n' "${res1}" "${res2}" "${res3}" "${res4}"

# 3. Self-recursion prevention
# An alias expanding to its own name terminates recursion at depth 1.
wrap_base() {
  printf 'wrapped(%s)\n' "${*}"
}
alias wrap_base='wrap_base inner'
printf '3_self_recursion: '
wrap_base outer

# 4. Mutual cycle termination
# A cycle of aliases terminates when an ancestor in the recursion stack is seen.
cycle_target() {
  printf 'cycle_target(%s)\n' "${*}"
}
alias cycle_target='cycle_step_b '
alias cycle_step_b='cycle_target '
printf '4_mutual_cycle:   '
cycle_target arg

# 5. Compound command parse barrier
# The entire compound command is parsed before any command in it executes.
compound_res="unexpanded"
if true; then
  # shellcheck disable=SC2262,SC2263 # intentional test of compound command parse barrier
  alias dyn_cmd='compound_res="expanded"'
  dyn_cmd 2>/dev/null || compound_res="barrier_held"
fi
printf '5_parse_barrier:  %s\n' "${compound_res}"

# 6. Eval dynamic execution
# eval invokes the parser at runtime, observing alias table updates.
eval_res="unset"
if true; then
  alias dyn_cmd2='eval_res="eval_succeeded"'
  eval dyn_cmd2
fi
printf '6_eval_dynamic:   %s\n' "${eval_res}"

# 7. Operator token boundary divergence
# bash (shell_getc at the end of the alias text) and zsh (checkalias) add a space,
# so zero_unquoted>&2 passes 0 to printf and redirects fd 1 to 2.
# dash, ksh93, and others do not add space, fusing 0 with >&2 into 0>&2.
# Quoting "0" prevents fusing across all shells.
# eval keeps the alias name and >&2 adjacent: shfmt would insert a space in plain code.
alias zero_unquoted='printf "%s" 0'
alias zero_quoted='printf "%s" "0"'
err_unquoted=$(eval '(zero_unquoted>&2)' 2>&1 >/dev/null)
err_quoted=$(eval '(zero_quoted>&2)' 2>&1 >/dev/null)
printf '7_operator_fuse:  unquoted=<%s> quoted=<%s>\n' "${err_unquoted}" "${err_quoted}"
