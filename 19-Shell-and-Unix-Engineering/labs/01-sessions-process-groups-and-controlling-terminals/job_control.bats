#!/usr/bin/env bats
# Tests for sessions, process groups, and controlling terminals.

setup() {
  load ../lib/lab
  SCRIPT="${BATS_TEST_DIRNAME}/job_control_probe.sh"
  PROBE="${BATS_TEST_TMPDIR}/pgrp_probe"
  if [ ! -x "${PROBE}" ]; then
    gcc -O2 -Wall -Wextra -Werror "${BATS_TEST_DIRNAME}/pgrp_probe.c" -o "${PROBE}"
  fi
  export PROBE
}

@test "under set +m pipelines and background jobs share shell process group" {
  run_lab "${SCRIPT}" pgid-inherited
  [ "${status}" -eq 0 ]
  sh_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*shell_pgid=\([0-9]*\).*/\1/p')
  pipe_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*pipe_pgid=\([0-9]*\).*/\1/p')
  bg_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*bg_pgid=\([0-9]*\).*/\1/p')

  [ -n "${sh_pgid}" ]
  [ "${sh_pgid}" = "${pipe_pgid}" ]
  [ "${sh_pgid}" = "${bg_pgid}" ]
}

@test "under set -m background jobs receive distinct process group" {
  run_lab "${SCRIPT}" pgid-monitored
  case $(lab_shell_family) in
    bash | mksh)
      [ "${status}" -eq 0 ]
      sh_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*shell_pgid=\([0-9]*\).*/\1/p')
      pipe_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*pipe_pgid=\([0-9]*\).*/\1/p')
      bg_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*bg_pgid=\([0-9]*\).*/\1/p')
      [ -n "${sh_pgid}" ]
      [ -n "${bg_pgid}" ]
      [ "${sh_pgid}" != "${bg_pgid}" ]
      [ "${sh_pgid}" != "${pipe_pgid}" ]
      ;;
    ksh93)
      [ "${status}" -eq 0 ]
      sh_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*shell_pgid=\([0-9]*\).*/\1/p')
      pipe_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*pipe_pgid=\([0-9]*\).*/\1/p')
      bg_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*bg_pgid=\([0-9]*\).*/\1/p')
      [ -n "${sh_pgid}" ]
      [ -n "${bg_pgid}" ]
      [ "${sh_pgid}" != "${bg_pgid}" ]
      case $(lab_shell_version) in
        *93u+m*)
          # ksh93u+m modernizes non-interactive monitor mode to isolate synchronous pipelines
          [ "${sh_pgid}" != "${pipe_pgid}" ]
          ;;
        *)
          # Legacy ksh93u+ keeps synchronous pipelines in the parent shell process group
          [ "${sh_pgid}" = "${pipe_pgid}" ]
          ;;
      esac
      ;;
    dash | busybox)
      # Without a controlling terminal, dash and busybox ash disable job control
      [ "${status}" -eq 0 ]
      case "${output}" in
        *"can't access tty; job control turned off"*) ;;
        *) false ;;
      esac
      sh_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*shell_pgid=\([0-9]*\).*/\1/p')
      pipe_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*pipe_pgid=\([0-9]*\).*/\1/p')
      bg_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*bg_pgid=\([0-9]*\).*/\1/p')
      [ -n "${sh_pgid}" ]
      [ "${sh_pgid}" = "${pipe_pgid}" ]
      [ "${sh_pgid}" = "${bg_pgid}" ]
      ;;
    zsh | zsh-sh)
      # zsh refuses to enable monitor mode without a controlling terminal
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"can't change option: -m"*) ;;
        *) false ;;
      esac
      ;;
    yash)
      # yash fails to open /dev/tty and disables job control
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"disabling job control"*) ;;
        *) false ;;
      esac
      ;;
    posh)
      # posh does not implement set -m
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"unknown option"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous commands ignore SIGINT and SIGQUIT without job control" {
  run_lab "${SCRIPT}" async-traps +m
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    mksh)
      case $(lab_shell_version) in
        *2020*)
          # Older mksh (such as R59 2020/10/31) retains default dispositions for async jobs
          [ "${output}" = "sigint=DFL sigquit=DFL" ]
          ;;
        *)
          # Modern mksh (R59 2025/04/26) sets SIG_IGN in compliance with POSIX
          [ "${output}" = "sigint=IGN sigquit=IGN" ]
          ;;
      esac
      ;;
    posh)
      # posh 0.14.1 does not ignore SIGINT and SIGQUIT in asynchronous lists (POSIX says it should)
      [ "${output}" = "sigint=DFL sigquit=DFL" ]
      ;;
    bash | dash | zsh | zsh-sh | yash | busybox | ksh93)
      [ "${output}" = "sigint=IGN sigquit=IGN" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous commands retain default SIGINT and SIGQUIT with job control" {
  run_lab "${SCRIPT}" async-traps -m
  case $(lab_shell_family) in
    bash | mksh | ksh93)
      [ "${status}" -eq 0 ]
      case "${output}" in
        *"sigint=DFL sigquit=DFL"*) ;;
        *) false ;;
      esac
      ;;
    dash | busybox)
      # Job control turned off due to missing tty; falls back to ignoring signals
      [ "${status}" -eq 0 ]
      case "${output}" in
        *"can't access tty; job control turned off"*) ;;
        *) false ;;
      esac
      case "${output}" in
        *"sigint=IGN sigquit=IGN"*) ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh)
      # zsh fails to enable -m without a controlling terminal
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"can't change option: -m"*) ;;
        *) false ;;
      esac
      ;;
    yash)
      # yash fails to open /dev/tty and disables job control
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"disabling job control"*) ;;
        *) false ;;
      esac
      ;;
    posh)
      # posh does not implement set -m
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"unknown option"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "asynchronous commands have stdin redirected to dev null under set +m except zsh" {
  # shellcheck disable=SC2016 # code string belongs to run_lab_c
  run_lab_c '
    if (set +m) 2>/dev/null; then
      set +m 2>/dev/null || true
    fi
    out="${BATS_TEST_TMPDIR}/bg_stdin.out"
    (
      if read -r line; then
        printf "GOT: %s\n" "${line}" >"${out}"
      else
        printf "EOF\n" >"${out}"
      fi
    ) &
    wait
    cat "${out}"
    rm -f "${out}"
  ' <<<"test_payload"
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "${output}" = "GOT: test_payload" ]
      ;;
    bash | dash | posh | mksh | yash | busybox)
      [ "${output}" = "EOF" ]
      ;;
    ksh93)
      [ "${output}" = "GOT: " ]
      ;;
    *)
      false
      ;;
  esac
}

@test "setsid creates a new session and detaches controlling terminal" {
  run_lab "${SCRIPT}" setsid-detach
  [ "${status}" -eq 0 ]
  # The probe's session leader owns a fresh pseudo-terminal, so /dev/tty opens (errno 0) before
  # setsid and fails with ENXIO (6) after it; the leader's own second setsid fails with EPERM (1)
  [ "${output}" = "ctty_devtty_errno=0 leader_setsid_errno=1 child_was_leader=0 setsid_ok sid_is_pid=1 pgid_is_pid=1 devtty_errno=6" ]
}

@test "kernel discards SIGTSTP for an orphaned process group" {
  run_lab "${SCRIPT}" orphan-stop
  [ "${status}" -eq 0 ]
  case "${output}" in
    *"orphan_tstp discarded"*) ;;
    *) false ;;
  esac
}

@test "reading from controlling terminal in an orphaned background group fails with EIO" {
  run_lab "${SCRIPT}" orphan-read
  [ "${status}" -eq 0 ]
  # SIGTTIN stays at its default, so a group that was not orphaned would stop and print no_report
  [ "${output}" = "orphan_read background=1 ret=-1 errno=5" ]
}

@test "with a controlling terminal set -m gives pipelines and background jobs their own process groups" {
  lab_shell_words
  run "${PROBE}" pty "${LAB_SHELL_WORDS[@]}" "${SCRIPT}" pgid-monitored
  case $(lab_shell_family) in
    posh)
      # posh does not implement set -m, terminal or not
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"unknown option"*) ;;
        *) false ;;
      esac
      ;;
    *)
      [ "${status}" -eq 0 ]
      sh_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*shell_pgid=\([0-9]*\).*/\1/p')
      pipe_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*pipe_pgid=\([0-9]*\).*/\1/p')
      bg_pgid=$(printf '%s\n' "${output}" | sed -n 's/.*bg_pgid=\([0-9]*\).*/\1/p')
      [ -n "${sh_pgid}" ]
      [ -n "${pipe_pgid}" ]
      [ -n "${bg_pgid}" ]
      [ "${sh_pgid}" != "${bg_pgid}" ]
      [ "${pipe_pgid}" != "${bg_pgid}" ]
      family=$(lab_shell_family)
      version=$(lab_shell_version)
      case "${family}:${version}" in
        ksh93:*93u+m*)
          [ "${sh_pgid}" != "${pipe_pgid}" ]
          ;;
        ksh93:*)
          # ksh93u+ 2012 runs a non-interactive script's synchronous pipeline in the shell's group
          [ "${sh_pgid}" = "${pipe_pgid}" ]
          ;;
        *)
          [ "${sh_pgid}" != "${pipe_pgid}" ]
          ;;
      esac
      ;;
  esac
}

@test "with a controlling terminal set -m leaves SIGINT and SIGQUIT at default in background jobs" {
  lab_shell_words
  run "${PROBE}" pty "${LAB_SHELL_WORDS[@]}" "${SCRIPT}" async-traps -m
  case $(lab_shell_family) in
    posh)
      [ "${status}" -ne 0 ]
      case "${output}" in
        *"unknown option"*) ;;
        *) false ;;
      esac
      ;;
    *)
      [ "${status}" -eq 0 ]
      case "${output}" in
        *"sigint=DFL sigquit=DFL"*) ;;
        *) false ;;
      esac
      ;;
  esac
}
