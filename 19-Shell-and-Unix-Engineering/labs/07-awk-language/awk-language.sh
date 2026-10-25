#!/bin/sh
# Demonstrate the POSIX awk execution lifecycle, field splitting and lazy $0
# reconstruction, numeric-string (STRNUM) comparison rules, and safe I/O.
set -eu

workdir=${1:-}
cleanup_dir=""
if [ -z "$workdir" ]; then
  workdir=$(mktemp -d "${TMPDIR:-/tmp}/awk-lab.XXXXXX")
  cleanup_dir=$workdir
  trap 'rm -rf "$cleanup_dir"' EXIT INT TERM
fi

mkdir -p "$workdir"
printf 'alpha 10\nbeta 20\n' >"$workdir/f1.txt"
printf 'gamma 30\ndelta 40\n' >"$workdir/f2.txt"

printf '=== 1. Execution lifecycle and ARGV timing ===\n'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
awk -v stage=pre_begin '
  BEGIN {
    printf "BEGIN: stage=%s NR=%d FNR=%d\n", stage, NR, FNR
  }
  {
    printf "BODY: stage=%s file=%s NR=%d FNR=%d $1=%s\n",
      stage, (FNR == NR ? "f1" : "f2"), NR, FNR, $1
    if (NR == 3) {
      exit 0
    }
  }
  END {
    printf "END: stage=%s NR=%d FNR=%d\n", stage, NR, FNR
  }
' stage=file1 "$workdir/f1.txt" stage=file2 "$workdir/f2.txt" stage=pre_end

# shellcheck disable=SC2016 # literal $0 in section header
printf '=== 2. FS modes and lazy $0 / OFS coherence ===\n'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf '  a   b  \n' | awk '{ printf "FS_space: NF=%d $1=<%s> $2=<%s>\n", NF, $1, $2 }'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf ':a::b:\n' | awk -F : '{ printf "FS_char: NF=%d $1=<%s> $2=<%s> $4=<%s> $5=<%s>\n", NF, $1, $2, $4, $5 }'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf ':a,:b\n' | awk -F '[:,]+' '{ printf "FS_ere: NF=%d $1=<%s> $2=<%s> $3=<%s>\n", NF, $1, $2, $3 }'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf 'a b c\n' | awk '{ OFS = "|"; print "no_touch:", $0 }'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf 'a b c\n' | awk '{ $1 = $1; OFS = "|"; print "ofs_after:", $0 }'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf 'a b c\n' | awk '{ OFS = "|"; $1 = $1; print "ofs_before:", $0 }'

printf '=== 3. Numeric strings (STRNUM) and IEEE 754 limits ===\n'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
printf '010 10\n0e12 0e99\n9007199254740992 9007199254740993\n' | awk '
  {
    printf "pair=<%s,%s> num_eq=%d str_eq=%d\n",
      $1, $2, ($1 == $2), ($1 "" == $2 "")
  }
  END {
    printf "uninit: u_eq_0=%d u_eq_empty=%d zero_eq_empty=%d\n",
      (u == 0), (u == ""), ("0" == "")
  }
'

printf '=== 4. Safe parameter passing and getline status ===\n'
raw_val='a\nb'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
VAL=$raw_val awk -v v="$raw_val" '
  BEGIN {
    printf "dash_v_len=%d env_len=%d env_val=<%s>\n",
      length(v), length(ENVIRON["VAL"]), ENVIRON["VAL"]
  }
'
# shellcheck disable=SC2016 # single-quoted awk program expands inside awk
awk -v ok="$workdir/f1.txt" -v missing="$workdir/no-such-file.txt" '
  BEGIN {
    r1 = (getline line < ok)
    close(ok)
    r2 = (getline line < missing)
    printf "getline_ok=%d line=<%s> getline_missing=%d\n", r1, line, r2
  }
'
