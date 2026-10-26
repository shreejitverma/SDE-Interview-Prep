#!/usr/bin/env bats
# Bats lab matrix for Shell Grammar (POSIX.1-2024 XCU 2.10).

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/grammar-probe.sh"
}

# no_line TEXT: succeed when no line of the last run's output is exactly TEXT.
no_line() {
  local l
  for l in "${lines[@]}"; do
    [ "$l" != "$1" ] || return 1
  done
}

@test "grammar-probe.sh all runs cleanly and produces consistent output" {
  run_lab "$PROBE" all
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1_reserved_words: unquoted=keyword" ]
  [ "${lines[1]}" = "2_esac_pattern: paren=parenthesized quoted=quoted" ]
  [ "${lines[2]}" = "3_prefix_export: env_seen=prefix_val outer_retained=outer" ]
  [ "${lines[3]}" = "4_prefix_standalone: var=persisted_val" ]
  [ "${lines[4]}" = "argc=2 <SUFFIX_VAR=assigned_in_suffix> <regular_arg>" ]
  [ "${lines[5]}" = "5_fn_brace: caller_var=mutated_parent" ]
  [ "${lines[6]}" = "6_fn_deferred: val=second" ]
  [ "${lines[7]}" = "7_linebreaks: valid" ]
  [ "${lines[8]}" = "8_negation: inverted" ]
}

@test "probe reserved-words verifies unquoted keyword recognition" {
  run_lab "$PROBE" reserved-words
  [ "$status" -eq 0 ]
  [ "$output" = "1_reserved_words: unquoted=keyword" ]
}

@test "probe esac-pattern verifies POSIX Rule 4 quoting and parenthesization" {
  run_lab "$PROBE" esac-pattern
  [ "$status" -eq 0 ]
  [ "$output" = "2_esac_pattern: paren=parenthesized quoted=quoted" ]
}

@test "probe prefix-suffix verifies POSIX Rule 7 assignment scoping" {
  run_lab "$PROBE" prefix-suffix
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "3_prefix_export: env_seen=prefix_val outer_retained=outer" ]
  [ "${lines[1]}" = "4_prefix_standalone: var=persisted_val" ]
  [ "${lines[2]}" = "argc=2 <SUFFIX_VAR=assigned_in_suffix> <regular_arg>" ]
}

@test "probe function-bodies verifies POSIX Rule 8 and 9 compound commands" {
  run_lab "$PROBE" function-bodies
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "5_fn_brace: caller_var=mutated_parent" ]
  [ "${lines[1]}" = "6_fn_deferred: val=second" ]
}

@test "probe linebreaks verifies legal multiline continuations" {
  run_lab "$PROBE" linebreaks
  [ "$status" -eq 0 ]
  [ "$output" = "7_linebreaks: valid" ]
}

@test "probe negation verifies single ! pipeline inversion" {
  run_lab "$PROBE" negation
  [ "$status" -eq 0 ]
  [ "$output" = "8_negation: inverted" ]
}

@test "unquoted esac in pattern position without parenthesis is a fatal syntax error" {
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'case "esac" in esac) echo match;; esac'
  case $(lab_shell_family) in
    bash | dash | zsh | zsh-sh | busybox | posh | mksh | yash)
      # POSIX Rule 4: esac recognized as closing reserved word, failing on unexpected )
      [ "$status" -ne 0 ]
      ;;
    ksh93)
      # ksh93u+ 2012 lookahead extension recognized pattern before );
      # ksh93u+m strictly rejects per POSIX Rule 4 (status 3).
      ver=$(lab_shell_version)
      case "$ver" in
        *93u+m*)
          [ "$status" -ne 0 ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "match" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "unquoted esac after an opening parenthesis: rule 4 exception honored only by some shells" {
  # POSIX grammar: pattern_list : '(' WORD /* Do not apply rule 4 */, so (esac) is a pattern.
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'case esac in (esac) echo paren;; *) echo wild;; esac'
  case $(lab_shell_family) in
    dash | ksh93 | zsh | yash | busybox | posh | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "paren" ]
      ;;
    zsh-sh)
      # zsh --emulate sh: parse error near `)'
      [ "$status" -ne 0 ]
      no_line paren
      ;;
    bash)
      ver=$(lab_shell_version)
      case "$ver" in
        3.*)
          # Apple's bash 3.2.57 build (macOS /bin/sh and /bin/bash) accepts it;
          # an upstream bash 3.2.57 build, on any system, rejects it.
          os=$(uname -s)
          if [ "$os" = Darwin ] && { [ "${LAB_SHELL%% *}" = /bin/sh ] || [ "${LAB_SHELL%% *}" = /bin/bash ]; }; then
            [ "$status" -eq 0 ]
            [ "$output" = "paren" ]
          else
            [ "$status" -ne 0 ]
            no_line paren
          fi
          ;;
        4.* | 5.0.* | 5.1.*)
          # Measured on 4.4 and 5.1 (5.0 and 4.0-4.3 not measured): syntax error near `esac'
          [ "$status" -ne 0 ]
          no_line paren
          ;;
        *)
          # bash 5.2 and later
          [ "$status" -eq 0 ]
          [ "$output" = "paren" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "quoted reserved word is parsed as a simple command name, not a keyword" {
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c '"if" true; then echo ok; fi'
  [ "$status" -ne 0 ]
  [[ "$output" == *"then"* ]]
}

@test "subshell allows omitted semicolon before closing parenthesis, brace group requires it" {
  # Parentheses delimit tokens; no semicolon required before )
  run_lab_c '( echo hi )'
  [ "$status" -eq 0 ]
  [ "$output" = "hi" ]

  # Braces are reserved words, not delimiters; omitting separator is a syntax error
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c '{ echo hi }'
  case $(lab_shell_family) in
    dash | bash | ksh93 | zsh-sh | busybox | posh | mksh | yash)
      [ "$status" -ne 0 ]
      ;;
    zsh)
      # Native zsh permits omitted separator before closing brace
      [ "$status" -eq 0 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "empty compound list in conditional constructs is rejected by grammar" {
  # POSIX grammar requires at least one command in compound_list between then and fi
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'if true; then fi'
  case $(lab_shell_family) in
    dash | bash | ksh93 | busybox | posh | mksh)
      [ "$status" -ne 0 ]
      ;;
    zsh | zsh-sh | yash)
      # zsh and yash permit empty compound lists in if/then (treated as null command)
      [ "$status" -eq 0 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "newline immediately following redirection operator is a syntax error" {
  # Redirection filename production lacks linebreak; bare newline is rejected
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'cat <
/dev/null'
  [ "$status" -ne 0 ]
}

@test "function definition syntax strictly requires compound command body" {
  # Subshell as compound command is valid POSIX function body
  run_lab_c 'f() ( exit 0 ); f'
  [ "$status" -eq 0 ]

  # Simple command directly as function body
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'f() echo hi; f'
  case $(lab_shell_family) in
    bash | posh | yash)
      # Strict POSIX XCU 2.10.2: function body must be compound_command
      [ "$status" -ne 0 ]
      ;;
    dash | ksh93 | zsh | zsh-sh | busybox | mksh)
      # Extension allowing simple command directly as function body
      [ "$status" -eq 0 ]
      [ "$output" = "hi" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "case fallthrough ;& (SEMI_AND) support across shell families" {
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'case 1 in 1) printf "a" ;& 2) printf "b" ;; esac'
  case $(lab_shell_family) in
    bash)
      # bash 4.0+ supports ;&, legacy bash 3.2 rejects it
      ver=$(lab_shell_version)
      case "$ver" in
        3.*) [ "$status" -ne 0 ] ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "ab" ]
          ;;
      esac
      ;;
    zsh | zsh-sh | ksh93 | mksh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "ab" ]
      ;;
    dash | busybox | posh)
      [ "$status" -ne 0 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "double negation ! ! pipeline syntax across shell families" {
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c '! ! true'
  case $(lab_shell_family) in
    bash)
      ver=$(lab_shell_version)
      case "$ver" in
        3.*) [ "$status" -ne 0 ] ;;
        *) [ "$status" -eq 0 ] ;;
      esac
      ;;
    ksh93 | mksh | posh)
      [ "$status" -eq 0 ]
      ;;
    dash | busybox | zsh | zsh-sh | yash)
      [ "$status" -ne 0 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "non-interactive eval syntax error aborts shell or sets non-zero status" {
  # shellcheck disable=SC2016 # single quotes protect shell syntax under test
  run_lab_c 'eval "fi"; printf "survived"'
  local orig_status=$status orig_output=$output
  case $(lab_shell_family) in
    dash | busybox | mksh | posh | yash)
      # POSIX 2.8.1 strict enforcement: syntax error aborts non-interactive process
      [ "$orig_status" -ne 0 ]
      [[ "$orig_output" != *"survived"* ]]
      ;;
    zsh | zsh-sh | ksh93)
      # Shell extension: eval syntax error returns non-zero but script continues
      [[ "$orig_output" == *"survived"* ]]
      ;;
    bash)
      # In POSIX mode, bash 4.0+ aborts per POSIX 2.8.1;
      # legacy bash 3.2 in POSIX mode (macOS /bin/sh) and native bash (all versions) survive.
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        ver=$(lab_shell_version)
        case "$ver" in
          3.*)
            [[ "$orig_output" == *"survived"* ]]
            ;;
          *)
            [ "$orig_status" -ne 0 ]
            [[ "$orig_output" != *"survived"* ]]
            ;;
        esac
      else
        [[ "$orig_output" == *"survived"* ]]
      fi
      ;;
    *)
      false
      ;;
  esac
}

@test "deeply nested subshell AST parsing handles recursion" {
  local code
  code=$(awk 'BEGIN { for (i = 0; i < 100; i++) printf "( "; printf "echo ok "; for (i = 0; i < 100; i++) printf ") " }')
  run_lab_c "$code"
  [ "$status" -eq 0 ]
  [ "$output" = "ok" ]
}
