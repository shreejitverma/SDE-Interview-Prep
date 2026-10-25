#!/bin/sh
# Probe POSIX XCU 2.6.5 Field Splitting and IFS semantics across shells.
set -eu

fmt_args() {
  printf 'argc=%d' "$#"
  for arg in "$@"; do
    printf ' <%s>' "$arg"
  done
  printf '\n'
}

# 1. Candidate regions (unquoted expansion) vs literal text vs double quotes
IFS=:
x='a:b'
# shellcheck disable=SC2086,SC2248 # intentional unquoted expansion to test field splitting
set -- $x c:d "$x"
printf '1_candidates: '
fmt_args "$@"

# 2. Mixed IFS whitespace and non-whitespace: ERE (s*ns*|s+) and field termination
IFS=' :'
val='  : a : : b :  '
# shellcheck disable=SC2086 # intentional unquoted expansion to test mixed IFS
set -- $val
printf '2_mixed_ifs:  '
fmt_args "$@"

# 3. Null IFS="" disables splitting but still elides unquoted empty expansion
IFS=
empty=''
spaced='a b'
# shellcheck disable=SC2086 # intentional unquoted expansions under null IFS
set -- $empty "$empty" $spaced
printf '3_null_ifs:   '
fmt_args "$@"

# 4. Unset IFS behaves as unset for ${IFS-unset} but splits on <space><tab><newline>
unset IFS
# shellcheck disable=SC2086 # intentional unquoted expansions under unset IFS
set -- ${IFS-unset} $spaced
printf '4_unset_ifs:  '
fmt_args "$@"

# 5. "$*" joins positional parameters with IFS[0]; "$@" preserves fields
IFS=:
set -- 'p 1' '' 'p 3'
printf '5_dollar_star: '
fmt_args "$*"
printf '6_dollar_at:   '
fmt_args "$@"

# 6. read modified field splitting with multiple and single trailing delimiters
IFS=' :' read -r first rest <<'EOF'
  alpha : beta : gamma :  
EOF
printf '7_read_multi:  first=<%s> rest=<%s>\n' "$first" "$rest"

IFS=' :' read -r first rest <<'EOF'
  alpha : beta :  
EOF
printf '8_read_single: first=<%s> rest=<%s>\n' "$first" "$rest"
