#!/usr/bin/env bats
# Tests for The /proc Filesystem mechanics, magic symlinks, virtual metadata, and parsing invariants.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/proc_probe.sh"
  # The kernel, not the probe's own check, decides whether procfs exists: Linux mounts it, macOS has none.
  case $(uname -s) in
    Linux) HAS_PROCFS=yes ;;
    Darwin) HAS_PROCFS=no ;;
    *) false ;;
  esac
}

@test "virtual filesystem availability detection" {
  run_lab "$PROBE" detect
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    [ "$output" = "procfs: available" ]
  else
    [ "$output" = "procfs: unavailable" ]
  fi
}

@test "procfs virtual file reports zero st_size but streams non-zero bytes" {
  run_lab "$PROBE" stat_zero
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    [ "$output" = "stat_size=0 stream_bytes=nonzero" ]
  else
    [ "$output" = "procfs: unavailable" ]
  fi
}

@test "dynamic /proc/self task evaluation diverges from static process id" {
  run_lab "$PROBE" self_vs_dollar
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    case "$output" in
      self_comm=cat\ parent_comm=*)
        parent_comm="${output#self_comm=cat parent_comm=}"
        [ -n "$parent_comm" ]
        [ "$parent_comm" != "cat" ]
        family=$(lab_shell_family)
        case "$family" in
          bash)
            case "$parent_comm" in
              bash* | sh) ;;
              *) false ;;
            esac
            ;;
          dash)
            case "$parent_comm" in
              dash | sh) ;;
              *) false ;;
            esac
            ;;
          zsh | zsh-sh)
            [ "$parent_comm" = "zsh" ]
            ;;
          busybox)
            case "$parent_comm" in
              busybox | ash | sh) ;;
              *) false ;;
            esac
            ;;
          ksh93)
            case "$parent_comm" in
              ksh* | sh) ;;
              *) false ;;
            esac
            ;;
          mksh)
            case "$parent_comm" in
              mksh | sh) ;;
              *) false ;;
            esac
            ;;
          yash)
            [ "$parent_comm" = "yash" ]
            ;;
          posh)
            [ "$parent_comm" = "posh" ]
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
  else
    [ "$output" = "procfs: unavailable" ]
  fi
}

@test "magic symlink preserves unlinked file access and reports deleted suffix" {
  run_lab "$PROBE" magic_fd_deleted "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    [ "$output" = "link_deleted=yes content_preserved=yes" ]
  else
    [ "$output" = "procfs: unavailable" ]
  fi
}

@test "naive whitespace splitting of stat line corrupts field mapping on process names with spaces" {
  run_lab "$PROBE" parse_stat_naive '1234 (worker pool thread) S 100 200 300'
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case "$family" in
    zsh)
      [ "$output" = "field2=<> field3=<> field4=<>" ]
      ;;
    bash | dash | zsh-sh | busybox | ksh93 | mksh | yash | posh)
      [ "$output" = "field2=<(worker> field3=<pool> field4=<thread)>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "robust reverse paren parsing of stat line accurately isolates comm and numeric fields" {
  run_lab "$PROBE" parse_stat_robust '1234 (worker (pool) thread) S 100 200 300'
  [ "$status" -eq 0 ]
  [ "$output" = "pid=<1234> comm=<worker (pool) thread> state=<S> ppid=<100> pgrp=<200>" ]
}

@test "null delimited cmdline reader preserves spaces and empty string arguments" {
  # shellcheck disable=SC2312 # setup helper file
  printf 'cmd\0first arg with spaces\0\0third arg\0' >"$BATS_TEST_TMPDIR/test_cmdline"
  run_lab "$PROBE" parse_cmdline_file "$BATS_TEST_TMPDIR/test_cmdline" "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "<cmd><first arg with spaces><><third arg> count=4" ]
}

@test "live process cmdline inspection" {
  run_lab "$PROBE" live_cmdline
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    case "$output" in
      live_cmdline_parsed=ok\ count=*)
        [ "$status" -eq 0 ]
        ;;
      *)
        false
        ;;
    esac
  else
    [ "$output" = "procfs: unavailable" ]
  fi
}

@test "kernel filesystem magic number verification via statfs C probe" {
  "${CC:-cc}" -Wall -Wextra -pedantic "$BATS_TEST_DIRNAME/proc_magic.c" -o "$BATS_TEST_TMPDIR/proc_magic"
  run "$BATS_TEST_TMPDIR/proc_magic"
  [ "$status" -eq 0 ]
  if [ "$HAS_PROCFS" = yes ]; then
    [ "$output" = "proc_magic=0x9fa0" ]
  else
    [ "$output" = "proc_fs=unavailable" ]
  fi
}

@test "process substitution availability and syntax handling across shell families" {
  # Two substitutions passed as file operands: a real process substitution expands each to a
  # path (/dev/fd/N or /proc/self/fd/N), so cat prints both lines; yash's <(cmd) is a process
  # redirection of standard input instead, so the second redirection wins and cat prints only b.
  family=$(lab_shell_family)
  case "$family" in
    bash)
      # In bash 5.0 and earlier running in POSIX mode, process substitution is rejected as a syntax error.
      # In bash 5.1+ (even with --posix) and standard non-POSIX bash, process substitution is supported.
      is_old_posix=no
      ver=$(lab_shell_version)
      case "$ver" in
        [0-4].* | 5.0.*)
          if run_lab_c '[ -o posix ]' && [ "$status" -eq 0 ]; then
            is_old_posix=yes
          fi
          ;;
        *)
          ;;
      esac
      if [ "$is_old_posix" = yes ]; then
        run_lab_c 'cat <(printf "procsub_ok\n")'
        [ "$status" -ne 0 ]
        case "$output" in
          *[Ss]yntax\ error* | *unexpected*) ;;
          *) false ;;
        esac
      else
        run_lab_c 'cat <(printf "a\n") <(printf "b\n")'
        [ "$status" -eq 0 ]
        [ "$output" = "a
b" ]
      fi
      ;;
    zsh | zsh-sh | ksh93 | busybox)
      run_lab_c 'cat <(printf "a\n") <(printf "b\n")'
      [ "$status" -eq 0 ]
      [ "$output" = "a
b" ]
      ;;
    yash)
      run_lab_c 'cat <(printf "a\n") <(printf "b\n")'
      [ "$status" -eq 0 ]
      [ "$output" = "b" ]
      ;;
    dash | posh | mksh)
      run_lab_c 'cat <(printf "procsub_ok\n")'
      [ "$status" -ne 0 ]
      case "$output" in
        *[Ss]yntax\ error* | *unexpected*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}
