#!/usr/bin/env bats
# Tests for Zombies, Orphans, Kernel Reaping, and Shell Wait Semantics.

setup() {
  load ../lib/lab
  LAB_DIR="${BATS_TEST_DIRNAME}"
  PROBE_SRC="${LAB_DIR}/zombie_orphan_probe.c"
  PROBE_BIN="${BATS_TEST_TMPDIR}/zombie_orphan_probe"
  REAP_MONITOR="${LAB_DIR}/reap_monitor.sh"

  "${CC:-cc}" -Wall -Wextra -pedantic -std=c99 \
    -D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=600 \
    "${PROBE_SRC}" -o "${PROBE_BIN}"
}

@test "zombie process exists in defunct state prior to parent waitpid" {
  probe_out="${BATS_TEST_TMPDIR}/zombie_probe.out"
  "${PROBE_BIN}" zombie >"${probe_out}" &
  parent_pid=$!

  # Allow child to fork and exit into zombie state
  /bin/sleep 1

  zombie_pid=""
  if [ -s "${probe_out}" ]; then
    zombie_line=$(head -n 1 "${probe_out}")
    zombie_pid="${zombie_line#ZOMBIE_PID=}"
  fi

  [ -n "${zombie_pid}" ]

  # Check that process is in zombie / defunct state
  stat_col=$(ps -o stat= -p "${zombie_pid}" 2>/dev/null || ps -o state= -p "${zombie_pid}" 2>/dev/null || true)
  case "${stat_col}" in
    *Z* | *z*) ;;
    *)
      # Fallback to /proc if ps does not show state
      if [ -r "/proc/${zombie_pid}/status" ]; then
        grep -q 'State:.*Z' "/proc/${zombie_pid}/status"
      else
        false
      fi
      ;;
  esac

  # Wait for parent probe to call waitpid and finish
  wait "${parent_pid}"

  grep -q 'REAPED=42' "${probe_out}"

  # Verify process is completely removed from process table
  run kill -0 "${zombie_pid}" 2>/dev/null
  [ "$status" -ne 0 ]
}

@test "orphan process is reparented to init or subreaper when original parent exits" {
  run "${PROBE_BIN}" orphan
  [ "$status" -eq 0 ]
  [[ "$output" =~ "ORPHAN_REPARENTED=1" ]]
}

@test "SA_NOCLDWAIT automatically reaps child and causes waitpid to fail with ECHILD" {
  run "${PROBE_BIN}" nocldwait
  [ "$status" -eq 0 ]
  [[ "$output" =~ "NOCLDWAIT_ECHILD=1" ]]
}

@test "stopped orphaned process group receives SIGHUP followed by SIGCONT from kernel" {
  run "${PROBE_BIN}" orphaned_pgrp
  [ "$status" -eq 0 ]
  [[ "$output" =~ "SIGHUP=1" ]]
  [[ "$output" =~ "SIGCONT=1" ]]
}

@test "reap_monitor spawns background job and reaps exit status with wait" {
  run_lab "${REAP_MONITOR}" spawn-reap 42
  [ "$status" -eq 0 ]
  [ "$output" = "STATUS=42" ]
}

@test "second wait on same PID diverges between caching shells and POSIX removing shells" {
  run_lab "${REAP_MONITOR}" double-wait 42
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      # In POSIX mode (e.g. macOS /bin/sh or bash --posix), bash discards known PID and returns 127.
      # In default native mode, bash retains exited status up to CHILD_MAX and returns 42.
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        # POSIX mode
        run_lab "${REAP_MONITOR}" double-wait 42
        [ "$output" = "$(printf 'FIRST_WAIT=42\nSECOND_WAIT=127')" ]
      else
        run_lab "${REAP_MONITOR}" double-wait 42
        [ "$output" = "$(printf 'FIRST_WAIT=42\nSECOND_WAIT=42')" ]
      fi
      ;;
    dash | busybox)
      [ "$output" = "$(printf 'FIRST_WAIT=42\nSECOND_WAIT=42')" ]
      ;;
    zsh | zsh-sh | posh | mksh | ksh93 | yash)
      [ "$output" = "$(printf 'FIRST_WAIT=42\nSECOND_WAIT=127')" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "wait without operands discards the status of a job it waited for in bash, yash, mksh, and posh" {
  run_lab "${REAP_MONITOR}" wait-all 17 running
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | yash | mksh | posh)
      [ "$output" = "$(printf 'WAIT_ALL=0\nWAIT_PID_AFTER_ALL=127')" ]
      ;;
    dash | busybox | ksh93 | zsh | zsh-sh)
      [ "$output" = "$(printf 'WAIT_ALL=0\nWAIT_PID_AFTER_ALL=17')" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "wait without operands keeps the status of an already exited job in every shell but bash and yash" {
  run_lab "${REAP_MONITOR}" wait-all 17 exited
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | yash)
      [ "$output" = "$(printf 'WAIT_ALL=0\nWAIT_PID_AFTER_ALL=127')" ]
      ;;
    dash | busybox | ksh93 | zsh | zsh-sh | posh | mksh)
      [ "$output" = "$(printf 'WAIT_ALL=0\nWAIT_PID_AFTER_ALL=17')" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "reap_monitor subshell-orphan leaves background child alive after subshell exits" {
  pid_file="${BATS_TEST_TMPDIR}/orphan.pid"
  run_lab "${REAP_MONITOR}" subshell-orphan "${pid_file}"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "IS_ALIVE=1" ]]
}
