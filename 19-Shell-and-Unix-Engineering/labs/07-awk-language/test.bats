#!/usr/bin/env bats

setup() {
  load ../lib/lab
}

@test "awk-language.sh runs end-to-end lifecycle, FS, STRNUM, and I/O checks" {
  run_lab "$BATS_TEST_DIRNAME/awk-language.sh" "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "=== 1. Execution lifecycle and ARGV timing ===" ]
  [ "${lines[1]}" = "BEGIN: stage=pre_begin NR=0 FNR=0" ]
  [ "${lines[2]}" = "BODY: stage=file1 file=f1 NR=1 FNR=1 \$1=alpha" ]
  [ "${lines[3]}" = "BODY: stage=file1 file=f1 NR=2 FNR=2 \$1=beta" ]
  [ "${lines[4]}" = "BODY: stage=file2 file=f2 NR=3 FNR=1 \$1=gamma" ]
  [ "${lines[5]}" = "END: stage=file2 NR=3 FNR=1" ]
  [ "${lines[6]}" = "=== 2. FS modes and lazy \$0 / OFS coherence ===" ]
  [ "${lines[7]}" = "FS_space: NF=2 \$1=<a> \$2=<b>" ]
  [ "${lines[8]}" = "FS_char: NF=5 \$1=<> \$2=<a> \$4=<b> \$5=<>" ]
  [ "${lines[9]}" = "FS_ere: NF=3 \$1=<> \$2=<a> \$3=<b>" ]
  [ "${lines[10]}" = "no_touch:|a b c" ]
  [ "${lines[11]}" = "ofs_after:|a b c" ]
  [ "${lines[12]}" = "ofs_before:|a|b|c" ]
  [ "${lines[13]}" = "=== 3. Numeric strings (STRNUM) and IEEE 754 limits ===" ]
  [ "${lines[14]}" = "pair=<010,10> num_eq=1 str_eq=0" ]
  [ "${lines[15]}" = "pair=<0e12,0e99> num_eq=1 str_eq=0" ]
  [ "${lines[16]}" = "pair=<9007199254740992,9007199254740993> num_eq=1 str_eq=0" ]
  [ "${lines[17]}" = "uninit: u_eq_0=1 u_eq_empty=1 zero_eq_empty=0" ]
  [ "${lines[18]}" = "=== 4. Safe parameter passing and getline status ===" ]
  [ "${lines[19]}" = "dash_v_len=3 env_len=4 env_val=<a\\nb>" ]
  [ "${lines[20]}" = "getline_ok=1 line=<alpha 10> getline_missing=-1" ]
}

@test "RS paragraph mode makes newline a field separator regardless of FS" {
  local awks awk_cmd flavor
  awks=$(lab_awks)
  [ -n "$awks" ] || awks="awk"
  while IFS= read -r awk_cmd; do
    [ -n "$awk_cmd" ] || continue
    # shellcheck disable=SC2086 # split multi-word command like "busybox awk" into distinct arguments
    flavor=$(lab_awk_flavor $awk_cmd)
    # shellcheck disable=SC2016 # single-quoted code runs inside LAB_SHELL and awk
    run_lab_c '
      printf "\n\na:b\nc:d\n\ne:f\n\n" | '"$awk_cmd"' -F : '\''
        BEGIN { RS = "" }
        { printf "rec=%d NF=%d $1=%s $2=%s $NF=%s\n", NR, NF, $1, $2, $NF }
      '\''
    '
    [ "$status" -eq 0 ]
    case $flavor in
      mawk)
        [ "${#lines[@]}" -eq 3 ]
        [ "${lines[0]}" = "rec=1 NF=3 \$1=a \$2=b" ]
        [ "${lines[1]}" = "c \$NF=d" ]
        [ "${lines[2]}" = "rec=2 NF=2 \$1=e \$2=f \$NF=f" ]
        ;;
      gawk | onetrue | busybox | *)
        [ "${#lines[@]}" -eq 2 ]
        [ "${lines[0]}" = "rec=1 NF=4 \$1=a \$2=b \$NF=d" ]
        [ "${lines[1]}" = "rec=2 NF=2 \$1=e \$2=f \$NF=f" ]
        ;;
    esac
  done <<EOF
$awks
EOF
}

@test "user-defined functions pass scalars by value, arrays by reference, and pad locals" {
  # shellcheck disable=SC2016 # single-quoted code runs inside LAB_SHELL and awk
  run_lab_c '
    awk '\''
      function mutate(s, arr,   local_tmp) {
        local_tmp = s + 10
        s = 999
        arr["k"] = local_tmp
        return local_tmp
      }
      BEGIN {
        orig = 5
        local_tmp = "global_untouched"
        res = mutate(orig, box)
        printf "orig=%d res=%d box=%d global=%s\n", orig, res, box["k"], local_tmp
      }
    '\''
  '
  [ "$status" -eq 0 ]
  [ "$output" = "orig=5 res=15 box=15 global=global_untouched" ]
}

@test "ARGV[1] deletion in BEGIN passes arbitrary strings without blocking stdin" {
  # shellcheck disable=SC2016 # single-quoted code runs inside LAB_SHELL and awk
  run_lab_c '
    printf "payload\n" | awk '\''
      BEGIN {
        prefix = ARGV[1] ""
        delete ARGV[1]
      }
      { print prefix "|" $0 }
    '\'' '\''foo=bar\n@/[/ '\''
  '
  [ "$status" -eq 0 ]
  [ "$output" = 'foo=bar\n@/[/ |payload' ]
}

@test "unquoted shell variable holding awk options splits in sh families but not native zsh" {
  # shellcheck disable=SC2016 # single-quoted code runs inside LAB_SHELL and awk
  run_lab_c '
    opts="-F : -v tag=ok"
    printf "a:b\n" | awk $opts '\''{ printf "<%s><%s><%d>\n", tag, $1, NF }'\''
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "$output" = "<><a:b><1>" ]
      ;;
    bash | zsh-sh | dash | ksh93 | mksh | yash | posh | busybox)
      [ "$output" = "<ok><a><2>" ]
      ;;
    *)
      [ "$output" = "<ok><a><2>" ]
      ;;
  esac
}
