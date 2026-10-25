#!/usr/bin/env bats
# Tests for descriptor inheritance and close-on-exec behavior across shells.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe_fds.sh"
}

@test "standard descriptors 0, 1, and 2 are inherited across process execution" {
  run_lab "$PROBE" 0 1 2
  [ "$status" -eq 0 ]
  [ "$output" = "0:open 1:open 2:open" ]
}

@test "descriptor 3 opened via exec is inherited in most shells but closed in ksh93 and mksh" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 3>/dev/null; "$1" 0 1 2 3' inline "$PROBE"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93 | mksh)
      [ "$output" = "0:open 1:open 2:open 3:closed" ]
      ;;
    bash | zsh | zsh-sh | dash | yash | busybox | posh)
      [ "$output" = "0:open 1:open 2:open 3:open" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "descriptor 3 is passed to child across all shells with explicit command redirection" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 3>/dev/null; "$1" 3<&3 0 1 2 3' inline "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "0:open 1:open 2:open 3:open" ]
}

@test "explicit descriptor closure with exec 3>&- closes descriptor in shell and child" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 3>/dev/null; exec 3>&-; "$1" 0 1 2 3' inline "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "0:open 1:open 2:open 3:closed" ]
}

@test "subshell preserves open descriptor without execve across all shells" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 3>/dev/null; ( if (: 9<&3) 2>/dev/null; then echo open; else echo closed; fi )'
  [ "$status" -eq 0 ]
  [ "$output" = "open" ]
}

@test "closing stdin reuses descriptor 0 on next open" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 0<&-; exec 0</dev/null; "$1" 0' inline "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "0:open" ]
}

@test "pipe writer closure allows reader to detect EOF" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    fifo="$1/fifo"
    mkfifo "$fifo"
    (
      exec 3>"$fifo"
      printf "data\n" >&3
      exec 3>&-
    ) &
    read -r line <"$fifo"
    rm -f "$fifo"
    printf "%s\n" "$line"
  ' inline "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "data" ]
}
