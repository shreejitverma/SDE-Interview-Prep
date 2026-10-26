#!/usr/bin/env bats
# Tests for process resource limits, ulimit builtin semantics, and kernel enforcement.

setup() {
  load ../lib/lab
  "${CC:-cc}" -O2 "$BATS_TEST_DIRNAME/inspect_rlimit.c" -o "$BATS_TEST_TMPDIR/inspect_rlimit"
  PROBE="$BATS_TEST_DIRNAME/probe_limits.sh"
}

@test "ulimit builtin reports limits and isolates changes inside subshells" {
  run_lab "$PROBE" subshell_isolation
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      # Output format: subshell=256 parent=<orig> orig=<orig>
      case "$output" in
        "subshell=256 parent="*)
          # Verify parent was restored to orig
          parent_val="${output#*parent=}"
          parent_val="${parent_val% orig=*}"
          orig_val="${output#*orig=}"
          [ "$parent_val" = "$orig_val" ]
          ;;
        *)
          false
          ;;
      esac
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "pipeline stage runs in subshell and does not leak limit changes to parent" {
  run_lab "$PROBE" pipeline_isolation
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      case "$output" in
        "stage=256 parent="*)
          parent_val="${output#*parent=}"
          parent_val="${parent_val% orig=*}"
          orig_val="${output#*orig=}"
          [ "$parent_val" = "$orig_val" ]
          ;;
        *)
          false
          ;;
      esac
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "lowering hard limit is irreversible for unprivileged processes" {
  run_lab "$PROBE" lower_and_raise_hard
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "raise_status=1" ]
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "lowering hard limit below soft limit fails in POSIX shells but clamps soft in zsh" {
  run_lab "$PROBE" lower_hard_below_soft
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "invalid_status=1" ]
      ;;
    zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "invalid_status=0 soft=64" ]
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "exceeding RLIMIT_FSIZE delivers SIGXFSZ terminating child process" {
  run_lab "$PROBE" fsize_enforcement "$BATS_TEST_TMPDIR/toolarge.tmp"
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | zsh | zsh-sh | busybox | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "dd_status=153" ]
      ;;
    ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = "dd_status=281" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = "dd_status=409" ]
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "write past RLIMIT_FSIZE fails with EFBIG when SIGXFSZ is ignored" {
  run_lab "$PROBE" fsize_efbig "$BATS_TEST_TMPDIR/efbig.tmp" "$BATS_TEST_TMPDIR/inspect_rlimit"
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "write_result=EFBIG" ]
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "RLIMIT_NOFILE boundary enforces descriptor allocation ceiling" {
  run_lab "$PROBE" nofile_boundary "$BATS_TEST_TMPDIR/inspect_rlimit"
  family=$(lab_shell_family)
  case "$family" in
    bash | dash | ksh93 | mksh | busybox | yash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "fd9=ok fd10=EBADF emfile=ok" ]
      ;;
    posh)
      [ "$status" -eq 127 ]
      case "$output" in
        *"ulimit: not found"*)
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

@test "process limit option differs across shells" {
  run_lab "$PROBE" nproc_option
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case "$family" in
    dash | mksh)
      [ "$output" = "u=0 p=1" ]
      ;;
    bash | ksh93)
      [ "$output" = "u=1 p=1" ]
      ;;
    yash | zsh | zsh-sh | busybox)
      [ "$output" = "u=1 p=0" ]
      ;;
    posh)
      [ "$output" = "u=0 p=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "ulimit -f without flags sets both soft and hard limits in POSIX shells but soft only in zsh" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'ulimit -f 100; "$1" get fsize' inline "$BATS_TEST_TMPDIR/inspect_rlimit"
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case "$family" in
    dash | ksh93 | mksh | busybox | yash)
      [ "$output" = "fsize cur=51200 max=51200" ]
      ;;
    bash)
      # bash 4.0 and later use 512-byte blocks in POSIX mode and 1024-byte blocks otherwise;
      # bash 3.2 uses 1024-byte blocks even in POSIX mode (macOS /bin/sh).
      version=$(lab_shell_version)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ] && [ "${version%%.*}" -ge 4 ]; then
        expected="fsize cur=51200 max=51200"
      else
        expected="fsize cur=102400 max=102400"
      fi
      # shellcheck disable=SC2016 # expansions belong to the shell under test
      run_lab_c 'ulimit -f 100; "$1" get fsize' inline "$BATS_TEST_TMPDIR/inspect_rlimit"
      [ "$output" = "$expected" ]
      ;;
    zsh | zsh-sh)
      [ "$output" = "fsize cur=51200 max=unlimited" ]
      ;;
    posh)
      case "$output" in
        *"ulimit: not found"*"fsize cur=unlimited max=unlimited"*)
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
