#!/usr/bin/env bats
# Tests for POSIX compound commands: grouping, environment isolation,
# single-open redirection scope, exit status rules, lazy pattern matching,
# fall-through (; &), pipeline subshell divergence, and fd inheritance.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe.sh"
}

# parse_error_status: print the measured exit status of a syntax error in `-c` code.
parse_error_status() {
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        4.4*) echo 1 ;;
        *) echo 2 ;;
      esac
      ;;
    dash | busybox | yash) echo 2 ;;
    ksh93) echo 3 ;;
    mksh | posh | zsh | zsh-sh) echo 1 ;;
    *) return 1 ;;
  esac
}

# unexpected_token_diagnostic TOKEN ZSH_TOKEN: print the measured syntax error text for a stray TOKEN.
unexpected_token_diagnostic() {
  case $(lab_shell_family) in
    bash) printf '%s\n' "syntax error near unexpected token \`$1'" ;;
    dash) printf '%s\n' "Syntax error: \"$1\" unexpected" ;;
    busybox) printf '%s\n' "syntax error: unexpected \"$1\"" ;;
    ksh93) printf '%s\n' "syntax error at line 1: \`$1' unexpected" ;;
    mksh) printf '%s\n' "syntax error: unexpected '$1'" ;;
    posh) printf '%s\n' "syntax error: \`$1' unexpected" ;;
    yash) printf '%s\n' "syntax error: encountered \`$1' without a matching \`{'" ;;
    zsh | zsh-sh) printf '%s\n' "parse error near \`$2'" ;;
    *) return 1 ;;
  esac
}

# assert_parse_error DIAGNOSTIC: the last run stopped at parse time, not at run time: it has the
# measured status and diagnostic, the command before the error on the same line never ran, and
# neither did the next line. Lines are compared whole because bash echoes the source in its message.
assert_parse_error() {
  local expected line
  expected=$(parse_error_status)
  [ "$status" -eq "$expected" ]
  case $output in
    *"$1"*) ;;
    *) return 1 ;;
  esac
  for line in "${lines[@]}"; do
    case $line in
      before | after*) return 1 ;;
      *) ;;
    esac
  done
}

# redirection_failure_outcome: print "continues", or the status set -e exits with, when the
# redirection on a brace group or loop cannot be opened.
redirection_failure_outcome() {
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2* | 4.4*) echo continues ;;
        *) echo 1 ;;
      esac
      ;;
    ksh93 | busybox) echo continues ;;
    zsh | zsh-sh | mksh | posh) echo 1 ;;
    dash | yash) echo 2 ;;
    *) return 1 ;;
  esac
}

# bash_is_posix_mode: succeed when the bash under test runs in POSIX mode (invoked as sh).
bash_is_posix_mode() {
  lab_shell_words
  "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]'
}

@test "brace group modifies current environment while subshell isolates changes" {
  run_lab "$PROBE" env_isolation "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "sub: var=subshell" ]
  [ "${lines[1]}" = "caller_after_sub: var=initial in_orig=1" ]
  [ "${lines[2]}" = "caller_after_brace: var=current" ]
}

@test "compound command redirection applies to all commands and restores descriptor" {
  run_lab "$PROBE" redir_scope "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "after" ]
  [ "${lines[1]}" = "line1" ]
  [ "${lines[2]}" = "line2" ]
}

@test "case statement pattern expansion is lazy across all shells" {
  run_lab "$PROBE" case_lazy "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "matched_clause1" ]
  [ "${lines[1]}" = "marker: absent" ]
}

@test "case fallthrough with ;& behaves according to shell capability" {
  run_lab "$PROBE" case_fallthrough
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2*)
          [ "$output" = "fallthrough: unsupported" ]
          ;;
        *)
          [ "$output" = "fallthrough: 1.0" ]
          ;;
      esac
      ;;
    dash | posh | busybox)
      [ "$output" = "fallthrough: unsupported" ]
      ;;
    ksh93 | zsh | zsh-sh | mksh | yash)
      [ "$output" = "fallthrough: 1.0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "compound command exit status rules conform to POSIX Defect 1309" {
  run_lab "$PROBE" loop_status
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "for_empty: 0" ]
  [ "${lines[1]}" = "while_empty: 0" ]
  [ "${lines[2]}" = "until_empty: 0" ]
  [ "${lines[3]}" = "if_empty: 0" ]
  [ "${lines[4]}" = "case_empty: 0" ]
  [ "${lines[5]}" = "while_body: 42" ]
}

@test "redirected while loop preserves variables while pipeline stage subshell diverges" {
  run_lab "$PROBE" pipeline_scope "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "redir_count: 3" ]
  case $(lab_shell_family) in
    ksh93 | zsh)
      [ "${lines[1]}" = "pipe_count: 3" ]
      ;;
    bash | zsh-sh | dash | busybox | posh | mksh | yash)
      [ "${lines[1]}" = "pipe_count: 0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "background job inside compound command inherits redirected file descriptor" {
  run_lab "$PROBE" fd_inheritance "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "from_main" ]
  [ "${lines[1]}" = "from_bg" ]
}

@test "assignment prefix preceding compound command is rejected as syntax error" {
  # shellcheck disable=SC2016 # intentional test of shell grammar rejection
  run_lab_c 'printf "before\n"; VAR=1 { echo "$VAR"; }
printf "after\n"'
  local diagnostic
  diagnostic=$(unexpected_token_diagnostic '}' '{')
  assert_parse_error "$diagnostic"
}

@test "parentheses do not require delimiter whitespace while braces require delimiters in POSIX shells" {
  # Subshell parenthesis is a control operator, so no spaces required
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '(printf "paren\n")'
  [ "$status" -eq 0 ]
  [ "$output" = "paren" ]

  # Without the space, {printf is an ordinary word, so the closing } is a stray reserved word
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c 'printf "before\n"; {printf "brace\n";}
printf "after\n"'
  case $(lab_shell_family) in
    zsh)
      # Native zsh accepts {cmd without space as a brace group extension
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "before" ]
      [ "${lines[1]}" = "brace" ]
      [ "${lines[2]}" = "after" ]
      ;;
    *)
      local diagnostic
      diagnostic=$(unexpected_token_diagnostic '}' '}')
      assert_parse_error "$diagnostic"
      ;;
  esac
}

@test "early return inside redirected compound command restores original descriptors" {
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '
    helper() {
      {
        printf "inside\n"
        return 0
        printf "unreached\n"
      } >/dev/null
    }
    helper
    printf "restored\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "restored" ]
}

@test "redirection failure on a brace group or loop under set -e aborts or continues per shell" {
  local code outcome
  outcome=$(redirection_failure_outcome)
  for code in \
    '{ printf "body\n"; }' \
    'for i in 1; do printf "body\n"; done' \
    'while :; do printf "body\n"; break; done' \
    'until false; do printf "body\n"; break; done'; do
    run_lab_c "set -e
$code >/nonexistent/dir/f
printf 'after %s\n' \"\$?\""
    case ${lines[0]} in
      */nonexistent/dir/f*) ;;
      *) return 1 ;;
    esac
    if [ "$outcome" = continues ]; then
      # The body is skipped, the status is 1, and the next command runs on the original stdout
      [ "$status" -eq 0 ]
      [ "${#lines[@]}" -eq 2 ]
      [ "${lines[1]}" = "after 1" ]
    else
      [ "$status" -eq "$outcome" ]
      [ "${#lines[@]}" -eq 1 ]
    fi
  done
}

@test "numeric for variable is a parse error, a run-time error, or an extension per shell" {
  run_lab_c 'printf "before\n"; for 123 in a; do printf "body\n"; done
printf "after %s\n" "$?"'
  case $(lab_shell_family) in
    bash)
      [ "${lines[0]}" = "before" ]
      case ${lines[1]} in
        *"\`123': not a valid identifier") ;;
        *) return 1 ;;
      esac
      if bash_is_posix_mode; then
        [ "$status" -eq 2 ]
        [ "${#lines[@]}" -eq 2 ]
      else
        [ "$status" -eq 0 ]
        [ "${#lines[@]}" -eq 3 ]
        [ "${lines[2]}" = "after 1" ]
      fi
      ;;
    ksh93)
      [ "$status" -eq 1 ]
      [ "${#lines[@]}" -eq 2 ]
      [ "${lines[0]}" = "before" ]
      case ${lines[1]} in
        *"123: invalid variable name") ;;
        *) return 1 ;;
      esac
      ;;
    zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "before" ]
      [ "${lines[1]}" = "body" ]
      [ "${lines[2]}" = "after 0" ]
      ;;
    dash) assert_parse_error "Syntax error: Bad for loop variable" ;;
    busybox) assert_parse_error "syntax error: bad for loop variable" ;;
    mksh | posh) assert_parse_error "for: bad identifier" ;;
    yash) assert_parse_error "syntax error: \`123' is not a valid identifier" ;;
    *) false ;;
  esac
}

@test "arithmetic for loop runs or is a parse error per shell" {
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c 'printf "before\n"; for ((i = 0; i < 2; i++)); do printf "body %s\n" "$i"; done
printf "after %s\n" "$?"'
  case $(lab_shell_family) in
    bash | ksh93 | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "${#lines[@]}" -eq 4 ]
      [ "${lines[0]}" = "before" ]
      [ "${lines[1]}" = "body 0" ]
      [ "${lines[2]}" = "body 1" ]
      [ "${lines[3]}" = "after 0" ]
      ;;
    dash) assert_parse_error "Syntax error: Bad for loop variable" ;;
    busybox) assert_parse_error "syntax error: bad for loop variable" ;;
    mksh) assert_parse_error "syntax error: unexpected '(('" ;;
    posh) assert_parse_error "syntax error: \`(' unexpected" ;;
    yash) assert_parse_error "syntax error: an identifier is required after \`for'" ;;
    *) false ;;
  esac
}

@test "double bracket test is built in or not found per shell" {
  # An empty PATH proves the shell's own support: Alpine's BusyBox installs a /usr/bin/[[ applet
  # that a shell without the feature would otherwise run.
  # shellcheck disable=SC2016 # inline code executed under shell under test
  run_lab_c '
    saved=$PATH
    PATH=/nonexistent
    [[ a == a ]]
    s1=$?
    [[ a == b ]]
    s2=$?
    PATH=$saved
    printf "result %s %s\n" "$s1" "$s2"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | zsh | zsh-sh | mksh | busybox | yash)
      [ "$output" = "result 0 1" ]
      ;;
    dash | posh)
      [ "${#lines[@]}" -eq 3 ]
      case ${lines[0]} in
        *"[[: not found") ;;
        *) return 1 ;;
      esac
      [ "${lines[2]}" = "result 127 127" ]
      ;;
    *)
      false
      ;;
  esac

  # With the normal PATH, a shell without the feature runs that applet where it is installed
  local family
  family=$(lab_shell_family)
  if [ "$family" = dash ] && [ -x '/usr/bin/[[' ]; then
    run_lab_c '[[ a == a ]]'
    [ "$status" -eq 0 ]
    [ -z "$output" ]
  fi
}

@test "case with zero patterns has status 0 even after a failure" {
  # The preceding false proves the 0 comes from case, not from an inherited $?
  run_lab_c 'false
case x in
esac
printf "after %s\n" "$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "after 0" ]

  run_lab_c 'false
case x in esac
printf "after %s\n" "$?"'
  local family version
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case $family:$version in
    ksh93:*93u+m*)
      [ "$status" -eq 0 ]
      [ "$output" = "after 0" ]
      ;;
    ksh93:*)
      # ksh93u+ 2012 requires the newline before esac
      [ "$status" -eq 3 ]
      case $output in
        *"syntax error at line 3: \`newline' unexpected"*) ;;
        *) return 1 ;;
      esac
      ;;
    *)
      [ "$status" -eq 0 ]
      [ "$output" = "after 0" ]
      ;;
  esac
}
