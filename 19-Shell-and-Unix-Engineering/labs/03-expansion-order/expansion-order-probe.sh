#!/bin/sh
# expansion-order-probe.sh: deterministic probe demonstrating POSIX word expansion order.
# Exercises:
#   Stage 0: Brace expansion vs Parameter expansion ({1..$n})
#   Stage 1: Beginning-to-end left-to-right evaluation with arithmetic side effects
#   Stage 1: Non-recursive evaluation (expanded tildes and command substitutions are not rescanned)
#   Stage 1 vs Stage 2: Parameter expansion vs Field splitting (unquoted splitting vs quoted protection)
#   Stage 2 vs Stage 3: Field splitting vs Pathname expansion (spaces in filenames vs space-delimited patterns)
#   Stage 3 vs Stage 4: Pathname expansion vs Quote removal (filesystem quotes preserved vs token quotes stripped)
#   Simple command processing (POSIX.1-2024 XCU 2.9.1.1): Command words expanded before redirection operands
#   Declaration utilities: Static assignment context vs non-declaration command vs dynamic command name
set -eu

fmt_args() {
  printf 'argc=%d' "$#"
  for arg in "$@"; do
    printf ' <%s>' "${arg}"
  done
  printf '\n'
}

# 1. Stage 0 (Brace expansion) vs Stage 1 (Parameter expansion)
n=3
# shellcheck disable=SC2086,SC2248,SC3009 # intentional unquoted expansion testing brace vs parameter order
set -- {1..${n}}
printf '1_brace_param: '
fmt_args "$@"

# 2. Stage 1 left-to-right evaluation across words
x=1
set -- "$((x += 1))" "$((x += 5))" "${x}"
printf '2_left_to_right: '
fmt_args "$@"

# 3. Stage 1 non-recursive evaluation (no re-expansion of produced syntax)
t='~'
# shellcheck disable=SC2016 # single quotes intentionally contain unexpanded syntax
c='$(echo bad)'
# shellcheck disable=SC2086,SC2248 # intentional unquoted expansion verifying lack of rescanning
set -- ${t} ${c}
printf '3_non_recursive: '
fmt_args "$@"

# 4. Stage 1 (Parameter expansion) vs Stage 2 (Field splitting)
IFS=' :'
val='a : b:c'
# shellcheck disable=SC2086,SC2248 # testing unquoted splitting vs double-quoted protection
set -- ${val} "${val}"
printf '4_param_split: '
fmt_args "$@"

# Set up scratch directory within test tmpdir or current working directory
tmpdir=$(mktemp -d "${BATS_TEST_TMPDIR:-${PWD}}/exp_probe.XXXXXX")
trap 'rm -rf "${tmpdir}"' EXIT

touch "${tmpdir}/file with spaces.txt"
touch "${tmpdir}/alpha.txt" "${tmpdir}/beta.md"

# 5. Stage 2 (Field splitting) vs Stage 3 (Pathname expansion)
(
  cd "${tmpdir}"
  set -- *.txt
  printf '5a_glob_spaces: '
  fmt_args "$@"

  pattern='*.txt *.md'
  # shellcheck disable=SC2086,SC2248 # intentional unquoted expansion testing splitting before globbing
  set -- ${pattern}
  printf '5b_split_glob: '
  fmt_args "$@"
)

# 6. Stage 3 (Pathname expansion) vs Stage 4 (Quote removal)
touch "${tmpdir}/foo'bar.txt"
touch "${tmpdir}/fooxyzbar.txt"

(
  cd "${tmpdir}"
  # shellcheck disable=SC2086,SC2248 # pathname expansion preserves literal quotes from directory entries
  set -- *\'*
  printf '6a_glob_quotes: '
  fmt_args "$@"

  set -- 'foo'*'bar.txt'
  printf '6b_token_quotes: '
  fmt_args "$@"
)

# shellcheck disable=SC2089 # literal quotes in variable intentionally test lack of quote removal
val_quotes="'foo bar'"
# shellcheck disable=SC2086,SC2090,SC2248 # intentional unquoted expansion verifying quotes are not removed
set -- ${val_quotes}
printf '6c_var_quotes: '
fmt_args "$@"

# 7. Simple command arguments before redirections (POSIX.1-2024 XCU 2.9.1.1)
x=0
# shellcheck disable=SC2257 # arithmetic side effect in redirection intentionally tests evaluation order
printf '%s %s\n' "arg1:$((x += 1))" "arg2:$((x += 100))" >"${tmpdir}/redir_$((x += 10))"
redir_file=""
for f in "${tmpdir}"/redir_*; do
  redir_file="${f##*/}"
  break
done
printf '7_redir_order: x=%d file=<%s>\n' "${x}" "${redir_file}"

# 8. Declaration utility assignment context (POSIX.1-2024 XCU 2.9.1.1 / Defect 351)
test_func() {
  fmt_args "$@"
}
v="a b"
# shellcheck disable=SC2086 # export is a declaration utility in POSIX.1-2024
export x_decl=${v}
printf '8a_decl_export: <%s>\n' "${x_decl}"

printf '8b_normal_cmd: '
# shellcheck disable=SC2086,SC2248 # non-declaration utility arguments undergo field splitting
test_func x_norm=${v}

# 9. Dynamic declaration utility (lexical vs dynamic command name detection)
cmd='export'
y_dyn=''
# shellcheck disable=SC2086,SC2248 # dynamic command name tests lexical vs dynamic declaration detection
"${cmd}" y_dyn=${v}
printf '9_dynamic_decl: <%s>\n' "${y_dyn}"
