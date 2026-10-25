#!/usr/bin/env bats
# Tests for File Descriptors and Open File Descriptions.
# Verifies kernel sharing mechanisms, independent offset clobbering, and shell parsing divergences.

setup() {
  load ../lib/lab
  FD_EXPERIMENTS="$BATS_TEST_DIRNAME/fd_experiments.sh"
}

@test "duplicated file descriptors share open file description offset" {
  target="$BATS_TEST_TMPDIR/shared_dup.txt"
  run_lab "$FD_EXPERIMENTS" shared-dup "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "AAAAABB" ]
}

@test "independent opens create separate open file descriptions that clobber offset 0" {
  target="$BATS_TEST_TMPDIR/independent.txt"
  run_lab "$FD_EXPERIMENTS" independent-open "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "BBAAA" ]
}

@test "append mode open guarantees atomic end of file write despite independent descriptions" {
  target="$BATS_TEST_TMPDIR/append.txt"
  run_lab "$FD_EXPERIMENTS" append-open "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "AAAAABB" ]
}

@test "read write operator <> opens without truncation and preserves shared offset" {
  target="$BATS_TEST_TMPDIR/rw_inplace.txt"
  printf '0123456789' >"$target"
  run_lab "$FD_EXPERIMENTS" read-write-inplace "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "0123XX6789" ]
}

@test "stdout redirection followed by stderr duplication shares single open file description" {
  target="$BATS_TEST_TMPDIR/order_stdout_stderr.txt"
  run_lab "$FD_EXPERIMENTS" order-stdout-stderr "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "$(printf 'STDOUT_MSG\nSTDERR_MSG')" ]
}

@test "stderr duplication followed by stdout redirection leaves stderr on pre-redirection stream" {
  target="$BATS_TEST_TMPDIR/order_stderr_stdout.txt"
  run_lab "$FD_EXPERIMENTS" order-stderr-stdout "$target"
  [ "$status" -eq 0 ]
  [ "$output" = "STDERR_MSG" ]
  content=$(cat "$target")
  [ "$content" = "STDOUT_MSG" ]
}

@test "subshell compound block shares single open file description across forked children" {
  target="$BATS_TEST_TMPDIR/subshell.txt"
  run_lab "$FD_EXPERIMENTS" subshell-sharing "$target"
  [ "$status" -eq 0 ]
  content=$(cat "$target")
  [ "$content" = "$(printf 'CHILD_1\nCHILD_2')" ]
}

@test "multi-digit file descriptor redirection parsing across shell families" {
  target="$BATS_TEST_TMPDIR/multidigit.txt"
  family=$(lab_shell_family)
  case "$family" in
    bash | busybox | yash)
      # shellcheck disable=SC2016 # target expansion belongs to the shell under test
      run_lab_c 'target=$1; { printf "FD10_DATA\n" >&10; } 10>"$target"' sh "$target"
      [ "$status" -eq 0 ]
      content=$(cat "$target")
      [ "$content" = "FD10_DATA" ]
      ;;
    dash | posh | mksh | ksh93 | zsh | zsh-sh)
      # Non-supporting shells fail parsing on 10> as an io_number redirection
      # shellcheck disable=SC2016 # target expansion belongs to the shell under test
      run_lab_c 'target=$1; ( eval "{ printf \"FD10_DATA\n\" >&10; } 10>\"\$target\"" ) 2>/dev/null || exit 1' sh "$target"
      [ "$status" -eq 1 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "brace-variable dynamic descriptor allocation support across shell families" {
  target="$BATS_TEST_TMPDIR/brace_var.txt"
  family=$(lab_shell_family)
  case "$family" in
    bash)
      version=$(lab_shell_version)
      case "$version" in
        3.2*)
          # Bash 3.2 does not support {var}> dynamic allocation
          # shellcheck disable=SC2016 # target expansion belongs to the shell under test
          run_lab_c 'target=$1; ( eval "exec {allocated_fd}>\"\$target\"" ) 2>/dev/null || exit 1' sh "$target"
          [ "$status" -eq 1 ]
          ;;
        *)
          # Bash 4.1+ supports {var}> dynamic allocation
          # shellcheck disable=SC2016 # target expansion belongs to the shell under test
          run_lab_c 'target=$1; eval "exec {allocated_fd}>\"\$target\"" && printf "DYNAMIC\n" >&"$allocated_fd" && eval "exec {allocated_fd}>&-"' sh "$target"
          [ "$status" -eq 0 ]
          content=$(cat "$target")
          [ "$content" = "DYNAMIC" ]
          ;;
      esac
      ;;
    ksh93 | zsh)
      # shellcheck disable=SC2016 # target expansion belongs to the shell under test
      run_lab_c 'target=$1; eval "exec {allocated_fd}>\"\$target\"" && printf "DYNAMIC\n" >&"$allocated_fd" && eval "exec {allocated_fd}>&-"' sh "$target"
      [ "$status" -eq 0 ]
      content=$(cat "$target")
      [ "$content" = "DYNAMIC" ]
      ;;
    dash | posh | busybox | mksh | zsh-sh | yash)
      # shellcheck disable=SC2016 # target expansion belongs to the shell under test
      run_lab_c 'target=$1; ( eval "exec {allocated_fd}>\"\$target\"" ) 2>/dev/null || exit 1' sh "$target"
      [ "$status" -eq 1 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "combined redirection &> vs background operator trap across shell families" {
  target="$BATS_TEST_TMPDIR/combined_redir.txt"
  # shellcheck disable=SC2016 # evaluation belongs to shell under test
  run_lab_c 'target=$1; eval "printf COMBINED &> \"\$target\""; sleep 0.1' sh "$target"
  family=$(lab_shell_family)
  case "$family" in
    bash | zsh | zsh-sh | mksh)
      # Modern bash, native zsh, zsh sh-mode, and mksh treat &> as combined redirection
      [ "$status" -eq 0 ]
      content=$(cat "$target")
      [ "$content" = "COMBINED" ]
      ;;
    ksh93)
      version=$(lab_shell_version)
      case "$version" in
        *93u+m*)
          # ksh93u+m 1.0+ supports &> as redirection shorthand for >file 2>&1
          [ "$status" -eq 0 ]
          content=$(cat "$target")
          [ "$content" = "COMBINED" ]
          ;;
        *)
          # Historic ksh93 (such as 93u+ 2012) treats & as background separator; file is created empty
          [ "$status" -eq 0 ]
          content=$(cat "$target")
          [ "$content" = "" ]
          ;;
      esac
      ;;
    dash | posh | yash)
      # dash, posh, and yash treat & as background separator; file is created empty
      [ "$status" -eq 0 ]
      content=$(cat "$target")
      [ "$content" = "" ]
      ;;
    busybox)
      # In busybox ash, with CONFIG_ASH_BASH_COMPAT &> is combined redirection; without, & backgrounds
      [ "$status" -eq 0 ]
      content=$(cat "$target")
      case "$content" in
        COMBINED | "")
          ;;
        *)
          false
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}
