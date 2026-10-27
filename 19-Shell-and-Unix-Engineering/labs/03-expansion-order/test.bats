#!/usr/bin/env bats
# shellcheck disable=SC2154 # BATS_TEST_DIRNAME is set by the bats test runner
# shellcheck disable=SC2016 # literal expressions in single quotes intentionally assert unexpanded syntax
# test.bats: test suite for POSIX word expansion order (POSIX.1-2024 XCU 2.6 and 2.9.1.1).

setup() {
  load ../lib/lab
  PROBE="${BATS_TEST_DIRNAME}/expansion-order-probe.sh"
}

@test "expansion-order-probe.sh matches shell family semantics" {
  run_lab "${PROBE}"
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = '1_brace_param: argc=3 <1> <2> <3>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=2 <~> <$(echo bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=2 <a : b:c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=1 <*.txt *.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=1 <'foo bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a b>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=1 <x_norm=a b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a b>' ]
      ;;
    zsh-sh)
      [ "${lines[0]}" = '1_brace_param: argc=1 <{1..3}>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=3 <~> <$(echo> <bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=4 <a> <b> <c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=3 <alpha.txt> <file with spaces.txt> <beta.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=2 <'foo> <bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a b>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=2 <x_norm=a> <b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a>' ]
      ;;
    ksh93)
      [ "${lines[0]}" = '1_brace_param: argc=3 <1> <2> <3>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=3 <~> <$(echo> <bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=4 <a> <b> <c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=3 <alpha.txt> <file with spaces.txt> <beta.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=2 <'foo> <bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a b>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=2 <x_norm=a> <b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a>' ]
      ;;
    dash | busybox)
      [ "${lines[0]}" = '1_brace_param: argc=1 <{1..3}>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=3 <~> <$(echo> <bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=4 <a> <b> <c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=3 <alpha.txt> <file with spaces.txt> <beta.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=2 <'foo> <bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a b>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=2 <x_norm=a> <b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a b>' ]
      ;;
    bash | mksh | yash)
      [ "${lines[0]}" = '1_brace_param: argc=1 <{1..3}>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=3 <~> <$(echo> <bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=4 <a> <b> <c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=3 <alpha.txt> <file with spaces.txt> <beta.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=2 <'foo> <bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a b>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=2 <x_norm=a> <b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a>' ]
      ;;
    posh)
      [ "${lines[0]}" = '1_brace_param: argc=1 <{1..3}>' ]
      [ "${lines[1]}" = '2_left_to_right: argc=3 <2> <7> <7>' ]
      [ "${lines[2]}" = '3_non_recursive: argc=3 <~> <$(echo> <bad)>' ]
      [ "${lines[3]}" = '4_param_split: argc=4 <a> <b> <c> <a : b:c>' ]
      [ "${lines[4]}" = '5a_glob_spaces: argc=2 <alpha.txt> <file with spaces.txt>' ]
      [ "${lines[5]}" = '5b_split_glob: argc=3 <alpha.txt> <file with spaces.txt> <beta.md>' ]
      [ "${lines[6]}" = "6a_glob_quotes: argc=1 <foo'bar.txt>" ]
      [ "${lines[7]}" = "6b_token_quotes: argc=2 <foo'bar.txt> <fooxyzbar.txt>" ]
      [ "${lines[8]}" = "6c_var_quotes: argc=2 <'foo> <bar'>" ]
      [ "${lines[9]}" = '7_redir_order: x=111 file=<redir_111>' ]
      [ "${lines[10]}" = '8a_decl_export: <a>' ]
      [ "${lines[11]}" = '8b_normal_cmd: argc=2 <x_norm=a> <b>' ]
      [ "${lines[12]}" = '9_dynamic_decl: <a>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 0 brace expansion precedes Stage 1 parameter expansion" {
  run_lab_c 'n=4; set -- {1..$n}; printf "%d:<%s>\n" "$#" "$*"'
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh | ksh93)
      [ "${output}" = '4:<1 2 3 4>' ]
      ;;
    bash | dash | mksh | yash | busybox | posh | zsh-sh)
      [ "${output}" = '1:<{1..4}>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 1 evaluates beginning to end with in-order side effects" {
  run_lab_c 'x=10; set -- "$((x += 2))" "$((x *= 3))" "$x"; printf "%d:<%s>\n" "$#" "$*"'
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh | zsh-sh)
      [ "${output}" = '3:<12 36 36>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 1 substitutions do not rescan expanded text for stage 1 syntax" {
  run_lab_c '
    v="~"
    cmd="\$(echo injected)"
    set -- $v $cmd
    printf "%d:" "$#"
    for a in "$@"; do printf "<%s>" "$a"; done
    printf "\n"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${output}" = '2:<~><$(echo injected)>' ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh)
      [ "${output}" = '3:<~><$(echo><injected)>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 2 field splitting only splits unquoted expansion regions" {
  run_lab_c '
    IFS=":"
    x="a:b"
    set -- "literal:text" "$x" $x
    printf "%d:" "$#"
    for a in "$@"; do printf "<%s>" "$a"; done
    printf "\n"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${output}" = '3:<literal:text><a:b><a:b>' ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh)
      [ "${output}" = '4:<literal:text><a:b><a><b>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 3 pathname expansion runs after field splitting without splitting filenames" {
  run_lab_c '
    dir=$(mktemp -d "${BATS_TEST_TMPDIR:-${PWD}}/glob_test.XXXXXX")
    touch "$dir/file space.txt" "$dir/a.txt" "$dir/b.md"
    (
      cd "$dir"
      set -- *.txt
      printf "glob:%d:<%s> " "$#" "$1"
      p="*.txt *.md"
      set -- $p
      printf "split_p:%d\n" "$#"
    )
    rm -rf "$dir"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${output}" = 'glob:2:<a.txt> split_p:1' ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh)
      [ "${output}" = 'glob:2:<a.txt> split_p:3' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "Stage 4 quote removal strips only original token quotes not expanded quotes" {
  run_lab_c '
    dir=$(mktemp -d "${BATS_TEST_TMPDIR:-${PWD}}/qr_test.XXXXXX")
    touch "$dir/literal'\''quote.txt"
    (
      cd "$dir"
      set -- *.txt
      printf "fs_quote:<%s> " "$1"
    )
    rm -rf "$dir"
    set -- "x"'\''y'\''
    printf "orig_qr:<%s> " "$1"
    from_var="'\''quoted'\''"
    set -- $from_var
    printf "var_qr:<%s>\n" "$1"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh | zsh-sh)
      [ "${output}" = "fs_quote:<literal'quote.txt> orig_qr:<xy> var_qr:<'quoted'>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "POSIX 2.9.1.1 expands command words before redirection operands" {
  run_lab_c '
    dir=$(mktemp -d "${BATS_TEST_TMPDIR:-${PWD}}/redir_test.XXXXXX")
    x=0
    printf "%s %s\n" "$((x += 1))" "$((x += 100))" >"$dir/r_$((x += 10))"
    rfile=""
    for f in "$dir"/r_*; do rfile="${f##*/}"; break; done
    printf "x=%d:rfile=%s\n" "$x" "$rfile"
    rm -rf "$dir"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh | zsh-sh)
      [ "${output}" = 'x=111:rfile=r_111' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "POSIX 2.9.1.1 declaration utilities suppress field splitting on assignments" {
  run_lab_c '
    v="foo bar"
    export decl_var=$v
    f() { printf "f:%d:<%s>\n" "$#" "$*"; }
    f norm_var=$v
    printf "decl:<%s>\n" "$decl_var"
  '
  [ "${status}" -eq 0 ]

  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = 'f:1:<norm_var=foo bar>' ]
      [ "${lines[1]}" = 'decl:<foo bar>' ]
      ;;
    posh)
      [ "${lines[0]}" = 'f:2:<norm_var=foo bar>' ]
      [ "${lines[1]}" = 'decl:<foo>' ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | zsh-sh)
      [ "${lines[0]}" = 'f:2:<norm_var=foo bar>' ]
      [ "${lines[1]}" = 'decl:<foo bar>' ]
      ;;
    *)
      false
      ;;
  esac
}
