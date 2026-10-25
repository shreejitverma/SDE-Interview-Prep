#!/usr/bin/env bats
# Tests for Lab 01: Pipes and FIFOs execution behavior and IPC patterns.

setup() {
  load ../lib/lab
  SCRIPT="$BATS_TEST_DIRNAME/pipe-fifo.sh"
}

@test "pipeline-subshell demonstrates whether loop executes in a child environment" {
  run_lab "$SCRIPT" pipeline-subshell
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | ksh93)
      [ "$output" = "count:3" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | dash)
      [ "$output" = "count:0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "heredoc-env runs loop in current environment across all shell families" {
  run_lab "$SCRIPT" heredoc-env
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "count:3" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline-status default reflects exit status of the final pipeline element" {
  run_lab "$SCRIPT" pipeline-status default
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "status:0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline-status pipefail propagates earliest non-zero failure when supported" {
  local pipefail_supported=0
  # shellcheck disable=SC2016 # probe shell under test for pipefail option support
  run_lab_c '(set -o pipefail) 2>/dev/null'
  if [ "$status" -eq 0 ]; then
    pipefail_supported=1
  fi

  run_lab "$SCRIPT" pipeline-status pipefail
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "unsupported:pipefail" ]
      ;;
    dash)
      # Modern dash (Debian 0.5.12-7+, upstream git commit 6347b9fc52d7) supports
      # pipefail, while stock dash 0.5.12 (macOS, Alpine) does not.
      if [ "$pipefail_supported" -eq 1 ]; then
        [ "$output" = "status:42" ]
      else
        [ "$output" = "unsupported:pipefail" ]
      fi
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$output" = "status:42" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "fifo-rendezvous synchronizes producer and consumer over a named pipe" {
  local fifo="$BATS_TEST_TMPDIR/test-fifo-rendezvous"
  run_lab "$SCRIPT" fifo-rendezvous "$fifo" "verified-payload"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "verified-payload" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "fifo-rdwr transfers data without blocking open via read-write descriptor" {
  local fifo="$BATS_TEST_TMPDIR/test-fifo-rdwr"
  run_lab "$SCRIPT" fifo-rdwr "$fifo" "roundtrip-token"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "rdwr:roundtrip-token" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "fifo-leak-clean confirms EOF generation upon write descriptor closure" {
  local fifo="$BATS_TEST_TMPDIR/test-fifo-leak"
  run_lab "$SCRIPT" fifo-leak-clean "$fifo"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "lines:2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline last-stage parameter modification across shell families" {
  # shellcheck disable=SC2016 # code string executed under test
  run_lab_c '
    x=0
    if (set +m; shopt -s lastpipe) 2>/dev/null; then
      set +m
      shopt -s lastpipe
    fi
    printf "payload\n" | read -r x
    printf "x:%s\n" "$x"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      # lastpipe was introduced in Bash 4.2; legacy releases (such as macOS Bash 3.2) lack it
      case $(lab_shell_version) in
        [0-3].* | 4.[01].*)
          [ "$output" = "x:0" ]
          ;;
        *)
          [ "$output" = "x:payload" ]
          ;;
      esac
      ;;
    zsh | ksh93)
      [ "$output" = "x:payload" ]
      ;;
    zsh-sh | yash | busybox | posh | mksh | dash)
      [ "$output" = "x:0" ]
      ;;
    *)
      false
      ;;
  esac
}
