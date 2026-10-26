#!/bin/sh
# token-recognition-probe.sh: probe POSIX.1-2024 XCU 2.3 Token Recognition semantics.
set -eu

if [ -n "${BASH_VERSION-}" ]; then
  # shellcheck disable=SC3044 # bash-specific option enabled only under bash
  shopt -s expand_aliases
fi

# 1. Comment non-continuation across backslash-newline (XCU 2.3 Rule 9 and XRAT C.2.3)
t1_raw=$(printf '# comment \\\necho run1\n')
t1_out=$(eval "${t1_raw}")
printf '1_comment_nl: <%s>\n' "${t1_out}"

# 2. Comment hash in word interior vs token boundary (XCU 2.3 Rule 8 vs Rule 9)
t2_mid=$(eval 'echo foo#bar')
t2_sep=$(eval 'echo foo #bar')
printf '2_comment_mid: word=<%s> separated=<%s>\n' "${t2_mid}" "${t2_sep}"

# 3. Quoted vs bare digit redirection (XCU 2.10.1 IO_NUMBER)
tmp_dir="${1:-${BATS_TEST_TMPDIR:-.}/token_probe_$$}"
mkdir -p "${tmp_dir}"
# shellcheck disable=SC2317 # executed in cleanup trap
trap 'rm -rf "${tmp_dir}"' EXIT

# Bare digit before '>' yields IO_NUMBER (0 positional parameters to set)
set -- 1>"${tmp_dir}/out1"
c1=$#

# Quoted digit immediately before '>' yields WORD token '1' (1 positional parameter).
# The code is an eval string so that a formatter cannot insert a blank before '>',
# which would make the digit a separate word whether or not quoting suppresses IO_NUMBER.
# shellcheck disable=SC2016 # expanded by eval
eval 'set -- "1">"${tmp_dir}/out2"'
c2=$#
a2=$1

# Escaped digit immediately before '>' yields WORD token '1' (1 positional parameter)
# shellcheck disable=SC2016 # expanded by eval
eval 'set -- \1>"${tmp_dir}/out3"'
c3=$#
a3=$1

printf '3_quoted_redir: bare=%d quoted=%d escaped=%d (a2=%s, a3=%s)\n' \
  "${c1}" "${c2}" "${c3}" "${a2}" "${a3}"

# 4. Token boundary preservation across quotes and parameter expansion (XCU 2.3 Rule 4 & 5)
var='d'
# shellcheck disable=SC2086,SC2248 # testing token boundary assembly without splitting
set -- a'b'"c"${var}
printf '4_token_boundary: argc=%d <%s>\n' "$#" "$1"

# 5. Greedy multi-character operator assembly (XCU 2.3 Rule 2)
printf 'x' >>"${tmp_dir}/op_out"
printf 'y' >>"${tmp_dir}/op_out"
op_content=$(cat "${tmp_dir}/op_out")
printf '5_operator_greedy: <%s>\n' "${op_content}"

# 6. Alias substitution with trailing blank next-word chaining (XCU 2.3.1)
# 7. Quoted alias token bypass (XCU 2.3.1)
if command -v alias >/dev/null 2>&1; then
  alias pfx='echo '
  alias tgt='expanded'
  t6_out=$(eval 'pfx tgt')
  printf '6_alias_blank: <%s>\n' "${t6_out}"

  alias myprobe_cmd='echo aliased'
  t7_ctrl=$(eval 'myprobe_cmd plain')
  if [ "${t7_ctrl}" = "aliased plain" ]; then
    if (eval '\myprobe_cmd plain') >/dev/null 2>&1; then
      t7_bypass='no'
    else
      t7_bypass='yes'
    fi
  else
    t7_bypass='unsupported'
  fi
  printf '7_alias_bypass: bypassed=%s\n' "${t7_bypass}"
else
  printf '6_alias_blank: <alias_not_supported>\n'
  printf '7_alias_bypass: bypassed=alias_not_supported\n'
fi

# 8. Here-document queuing across single line (XCU 2.3)
t8_raw=$(
  cat <<EOF1 && cat <<EOF2
lineA
EOF1
lineB
EOF2
)
t8_flat=$(printf '%s\n' "${t8_raw}" | tr '\n' ' ' | sed 's/ $//')
printf '8_here_doc_interleave: <%s>\n' "${t8_flat}"
