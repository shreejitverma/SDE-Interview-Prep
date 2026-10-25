#!/usr/bin/env bats
# Bats lab matrix for Field Splitting and IFS (POSIX.1-2024 XCU 2.6.5).

setup() {
  load ../lib/lab
}

@test "field-splitting-probe.sh matches shell family semantics" {
  run_lab "$BATS_TEST_DIRNAME/field-splitting-probe.sh"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "1_candidates: argc=3 <a:b> <c:d> <a:b>" ]
      [ "${lines[1]}" = "2_mixed_ifs:  argc=1 <  : a : : b :  >" ]
      [ "${lines[2]}" = "3_null_ifs:   argc=2 <> <a b>" ]
      [ "${lines[3]}" = "4_unset_ifs:  argc=2 <unset> <a b>" ]
      [ "${lines[4]}" = "5_dollar_star: argc=1 <p 1::p 3>" ]
      [ "${lines[5]}" = "6_dollar_at:   argc=3 <p 1> <> <p 3>" ]
      [ "${lines[6]}" = "7_read_multi:  first=<alpha> rest=<beta : gamma :>" ]
      [ "${lines[7]}" = "8_read_single: first=<alpha> rest=<beta :>" ]
      ;;
    zsh-sh)
      [ "${lines[0]}" = "1_candidates: argc=4 <a> <b> <c:d> <a:b>" ]
      [ "${lines[1]}" = "2_mixed_ifs:  argc=5 <> <a> <> <b> <>" ]
      [ "${lines[2]}" = "3_null_ifs:   argc=2 <> <a b>" ]
      [ "${lines[3]}" = "4_unset_ifs:  argc=3 <unset> <a> <b>" ]
      [ "${lines[4]}" = "5_dollar_star: argc=1 <p 1::p 3>" ]
      [ "${lines[5]}" = "6_dollar_at:   argc=3 <p 1> <> <p 3>" ]
      [ "${lines[6]}" = "7_read_multi:  first=<alpha> rest=<beta : gamma :>" ]
      [ "${lines[7]}" = "8_read_single: first=<alpha> rest=<beta :>" ]
      ;;
    posh)
      [ "${lines[0]}" = "1_candidates: argc=4 <a> <b> <c:d> <a:b>" ]
      [ "${lines[1]}" = "2_mixed_ifs:  argc=4 <> <a> <> <b>" ]
      [ "${lines[2]}" = "3_null_ifs:   argc=1 < a b>" ]
      [ "${lines[3]}" = "4_unset_ifs:  argc=3 <unset> <a> <b>" ]
      [ "${lines[4]}" = "5_dollar_star: argc=1 <p 1::p 3>" ]
      [ "${lines[5]}" = "6_dollar_at:   argc=3 <p 1> <> <p 3>" ]
      [ "${lines[6]}" = "7_read_multi:  first=<alpha> rest=<: beta : gamma :>" ]
      [ "${lines[7]}" = "8_read_single: first=<alpha> rest=<: beta :>" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | *)
      [ "${lines[0]}" = "1_candidates: argc=4 <a> <b> <c:d> <a:b>" ]
      [ "${lines[1]}" = "2_mixed_ifs:  argc=4 <> <a> <> <b>" ]
      [ "${lines[2]}" = "3_null_ifs:   argc=2 <> <a b>" ]
      [ "${lines[3]}" = "4_unset_ifs:  argc=3 <unset> <a> <b>" ]
      [ "${lines[4]}" = "5_dollar_star: argc=1 <p 1::p 3>" ]
      [ "${lines[5]}" = "6_dollar_at:   argc=3 <p 1> <> <p 3>" ]
      [ "${lines[6]}" = "7_read_multi:  first=<alpha> rest=<beta : gamma :>" ]
      [ "${lines[7]}" = "8_read_single: first=<alpha> rest=<beta>" ]
      ;;
  esac
}

@test "startup resets IFS to default regardless of inherited environment" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  IFS=: run_lab_c 'printf "len=%d hex=%s\n" "${#IFS}" "$(printf "%s" "$IFS" | od -An -tx1 | tr -d " \n")"'
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "$output" = "len=4 hex=20090a00" ]
      ;;
    posh)
      [ "$output" = "len=1 hex=3a" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | zsh-sh | *)
      [ "$output" = "len=3 hex=20090a" ]
      ;;
  esac
}

@test "only unquoted parameter, command, and arithmetic expansions split" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  HOME="/tmp/a b" run_lab_c '
    IFS=" :"
    v="x:y"
    set -- ~ $v $(printf "m:n") $((10 + 20)) "p:q" r:s
    printf "%d:" "$#"
    for a in "$@"; do printf "<%s>" "$a"; done
    printf "\n"
  '
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "$output" = "7:</tmp/a b><x:y><m><n><30><p:q><r:s>" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh | *)
      [ "$output" = "8:</tmp/a b><x><y><m><n><30><p:q><r:s>" ]
      ;;
  esac
}

@test "non-whitespace IFS terminator vs separator matrix" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  run_lab_c '
    IFS=:
    for v in "" ":" "::" ":a" "a:" ":a:" "a::b" ":a:b:"; do
      set -- $v
      printf "%s=%d:" "$v" "$#"
      for a in "$@"; do printf "<%s>" "$a"; done
      printf "\n"
    done
  '
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "=0:" ]
      [ "${lines[1]}" = ":=1:<:>" ]
      [ "${lines[2]}" = "::=1:<::>" ]
      [ "${lines[3]}" = ":a=1:<:a>" ]
      [ "${lines[4]}" = "a:=1:<a:>" ]
      [ "${lines[5]}" = ":a:=1:<:a:>" ]
      [ "${lines[6]}" = "a::b=1:<a::b>" ]
      [ "${lines[7]}" = ":a:b:=1:<:a:b:>" ]
      ;;
    zsh-sh)
      [ "${lines[0]}" = "=0:" ]
      [ "${lines[1]}" = ":=2:<><>" ]
      [ "${lines[2]}" = "::=3:<><><>" ]
      [ "${lines[3]}" = ":a=2:<><a>" ]
      [ "${lines[4]}" = "a:=2:<a><>" ]
      [ "${lines[5]}" = ":a:=3:<><a><>" ]
      [ "${lines[6]}" = "a::b=3:<a><><b>" ]
      [ "${lines[7]}" = ":a:b:=4:<><a><b><>" ]
      ;;
    posh)
      [ "${lines[0]}" = "=0:" ]
      [ "${lines[1]}" = ":=0:" ]
      [ "${lines[2]}" = "::=1:<>" ]
      [ "${lines[3]}" = ":a=2:<><a>" ]
      [ "${lines[4]}" = "a:=1:<a>" ]
      [ "${lines[5]}" = ":a:=2:<><a>" ]
      [ "${lines[6]}" = "a::b=3:<a><><b>" ]
      [ "${lines[7]}" = ":a:b:=3:<><a><b>" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | *)
      [ "${lines[0]}" = "=0:" ]
      [ "${lines[1]}" = ":=1:<>" ]
      [ "${lines[2]}" = "::=2:<><>" ]
      [ "${lines[3]}" = ":a=2:<><a>" ]
      [ "${lines[4]}" = "a:=1:<a>" ]
      [ "${lines[5]}" = ":a:=2:<><a>" ]
      [ "${lines[6]}" = "a::b=3:<a><><b>" ]
      [ "${lines[7]}" = ":a:b:=3:<><a><b>" ]
      ;;
  esac
}

@test "dollar-star and dollar-at under custom, null, and unset IFS" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  run_lab_c '
    set -- "a b" "" "c:d"
    IFS=:
    printf "colon_qstar=%s\n" "$*"
    IFS=
    printf "null_qstar=%s\n" "$*"
    unset IFS
    printf "unset_qstar=%s\n" "$*"
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "colon_qstar=a b::c:d" ]
  case $(lab_shell_family) in
    ksh93)
      case $(lab_shell_version) in
        *93u+m/1.0.10*)
          [ "${lines[1]}" = 'null_qstar=a b\c:d' ]
          ;;
        *)
          [ "${lines[1]}" = "null_qstar=a bc:d" ]
          ;;
      esac
      ;;
    *)
      [ "${lines[1]}" = "null_qstar=a bc:d" ]
      ;;
  esac
  [ "${lines[2]}" = "unset_qstar=a b  c:d" ]
}

@test "IFS= read -r preserves leading and trailing whitespace and backslashes" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  run_lab_c '
    IFS= read -r line <<\EOF
  leading and \trailing  
EOF
    printf "<%s>\n" "$line"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "<  leading and \\trailing  >" ]
}

@test "field splitting feeds pathname expansion unless set -f is enabled" {
  # shellcheck disable=SC2016 # single-quoted script belongs to shell under test
  run_lab_c '
    cd "$1" || exit 1
    : > alpha.txt
    : > beta.txt
    IFS=:
    payload="*:literal"
    set -- $payload
    printf "glob_on=%d:" "$#"
    for a in "$@"; do printf "<%s>" "$a"; done
    printf "\n"
    set -f
    set -- $payload
    printf "glob_off=%d:" "$#"
    for a in "$@"; do printf "<%s>" "$a"; done
    printf "\n"
  ' sh "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "glob_on=1:<*:literal>" ]
      [ "${lines[1]}" = "glob_off=1:<*:literal>" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh | *)
      [ "${lines[0]}" = "glob_on=3:<alpha.txt><beta.txt><literal>" ]
      [ "${lines[1]}" = "glob_off=2:<*><literal>" ]
      ;;
  esac
}
