#!/usr/bin/env bats
# Bats lab matrix for Token Recognition (POSIX.1-2024 XCU 2.3).

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/token-recognition-probe.sh"
}

@test "token-recognition-probe.sh matches POSIX token recognition baseline" {
  run_lab "${PROBE}"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_comment_nl: <run1>" ]
  [ "${lines[1]}" = "2_comment_mid: word=<foo#bar> separated=<foo>" ]
  [ "${lines[2]}" = "3_quoted_redir: bare=0 quoted=1 escaped=1 (a2=1, a3=1)" ]
  [ "${lines[3]}" = "4_token_boundary: argc=1 <abcd>" ]
  [ "${lines[4]}" = "5_operator_greedy: <xy>" ]
  case $(lab_shell_family) in
    posh)
      [ "${lines[5]}" = "6_alias_blank: <alias_not_supported>" ]
      [ "${lines[6]}" = "7_alias_bypass: bypassed=alias_not_supported" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | zsh | zsh-sh)
      [ "${lines[5]}" = "6_alias_blank: <expanded>" ]
      [ "${lines[6]}" = "7_alias_bypass: bypassed=yes" ]
      ;;
    *) false ;;
  esac
  [ "${lines[7]}" = "8_here_doc_interleave: <lineA lineB>" ]
}

@test "backslash newline within operator tokens splits in ksh93 and zsh but joins in POSIX shells" {
  # shellcheck disable=SC2016 # code string tested by shell under test
  run_lab_c 'false |\
| printf "%s\n" b'
  case $(lab_shell_family) in
    ksh93 | zsh | zsh-sh)
      [ "$status" -ne 0 ]
      ;;
    bash | dash | mksh | yash | busybox | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "b" ]
      ;;
    *) false ;;
  esac
}

@test "double parentheses ((0)) distinction between compound command and nested subshells" {
  run_lab_c '((0)); printf "exit=%d\n" "$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | busybox | yash | posh)
      # dash, busybox ash, yash, and posh parse ((0)) as nested subshells running non-existent utility '0'
      [ "${lines[${#lines[@]} - 1]}" = "exit=127" ]
      ;;
    bash | ksh93 | mksh | zsh | zsh-sh)
      # shells with (( arithmetic evaluate integer 0 to false (exit status 1)
      [ "${lines[${#lines[@]} - 1]}" = "exit=1" ]
      ;;
    *) false ;;
  esac
}

@test "dollar single quotes ANSI-C escape tokenization" {
  # shellcheck disable=SC2016 # literal $'\\t' construct tested by shell under test
  run_lab_c 'printf "%s\n" $'"'"'a\tb'"'"''
  case $(lab_shell_family) in
    dash | posh)
      [ "$status" -eq 0 ]
      # shellcheck disable=SC2016 # literal dollar string verification
      [ "$output" = '$a\tb' ]
      ;;
    bash | ksh93 | mksh | yash | busybox | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      tab=$(printf '\t')
      [ "$output" = "a${tab}b" ]
      ;;
    *) false ;;
  esac
}

@test "unquoted parameter expansions do not produce operators" {
  # shellcheck disable=SC2016 # code string tested by shell under test
  run_lab_c 'op="; echo injected"; set -- $op; printf "%d args: <%s> <%s> <%s>\n" "$#" "$1" "$2" "$3"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # native zsh does not word-split unquoted parameters by default
      [ "$output" = "1 args: <; echo injected> <> <>" ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | posh | zsh-sh)
      [ "$output" = "3 args: <;> <echo> <injected>" ]
      ;;
    *) false ;;
  esac
}

@test "unlimited line length tokenization handles 100000 byte token" {
  script="$BATS_TEST_TMPDIR/big_token.sh"
  # Generate a 100,000-byte token assignment
  {
    printf 'val='
    awk 'BEGIN { for (i = 0; i < 100000; i++) printf "k" }'
    # shellcheck disable=SC2016 # code string generated for script under test
    printf '\nprintf "%%d\\n" "${#val}"\n'
  } >"${script}"
  run_lab "${script}"
  [ "$status" -eq 0 ]
  [ "$output" = "100000" ]
}

@test "complete_command execution order defers syntax error detection" {
  code=$(printf 'printf "%%s\\n" "cmd_1_executed"\nif [;\n')
  run_lab_c "${code}"
  case $(lab_shell_family) in
    posh | zsh | zsh-sh)
      # zsh and posh parse the entire -c string into an AST prior to execution
      [ "$status" -ne 0 ]
      [[ "$output" != *"cmd_1_executed"* ]]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox)
      # POSIX shells execute complete_command before tokenizing the next command
      [ "$status" -ne 0 ]
      [ "${lines[0]}" = "cmd_1_executed" ]
      ;;
    *) false ;;
  esac
}

@test "nested command substitution with comments and parentheses parses recursively" {
  # shellcheck disable=SC2016 # code string tested by shell under test
  run_lab_c 'out=$(printf "%s\n" "nested" # comment with a closing paren )
printf "%s\n" "second"
); printf "%s %s\n" "$out" "done"'
  case $(lab_shell_family) in
    posh)
      # posh 0.14 prematurely closes command substitution on closing paren inside comments
      [ "$status" -ne 0 ]
      ;;
    bash | dash | ksh93 | mksh | yash | busybox | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      expected=$(printf 'nested\nsecond done')
      [ "$output" = "$expected" ]
      ;;
    *) false ;;
  esac
}
