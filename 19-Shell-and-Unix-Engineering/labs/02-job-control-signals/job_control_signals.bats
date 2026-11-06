#!/usr/bin/env bats
# Tests for job control signals: which signals the terminal generates, who receives them, and what a
# background job does when it touches the terminal.
# probe.sh starts inner.sh under the shell under test, in a fresh pseudo-terminal for the pty modes.

setup() {
  load ../lib/lab
  export LAB_SHELL="${LAB_SHELL:-sh}"
  export JCTL="${BATS_TEST_TMPDIR}/jctl"
  export LOGDIR="${BATS_TEST_TMPDIR}/log"
  mkdir -p "${LOGDIR}"
  "${CC:-cc}" -O2 -Wall -Wextra -Werror "${BATS_TEST_DIRNAME}/jctl.c" -o "${JCTL}"
  PROBE="${BATS_TEST_DIRNAME}/probe.sh"
}

# is_posh: succeed in posh, which has no `set -m` at all.
is_posh() {
  local family
  family=$(lab_shell_family)
  [ "${family}" = posh ]
}

# assert_posh_has_no_monitor: the pty tests need `set -m`; posh must refuse it.
assert_posh_has_no_monitor() {
  run_lab_c 'set -m'
  [ "${status}" -ne 0 ]
}

# run_pty MODE: run a pty mode and require the harness to report how the shell ended.
run_pty() {
  run_lab "${PROBE}" "$1"
  [ "${status}" -eq 0 ]
  [[ ${output} == *child_exit=* || ${output} == *child_signal=* ]]
}

# signals_of NAME: the signals recorded by process NAME, in order, as one line.
signals_of() {
  local lines
  lines=$(grep '^sig ' "${LOGDIR}/$1.log" 2>/dev/null) || lines=
  printf '%s\n' "${lines}" | tr '\n' ' '
}

# pgrp_of NAME: the process group a recorder reported.
pgrp_of() {
  sed -n 's/^ready role=[a-z]* pgrp=\([0-9][0-9]*\)$/\1/p' "${LOGDIR}/$1.log"
}

# assert_interactive_handoff: the same probe in an interactive shell hands the terminal to a job in its own group.
assert_interactive_handoff() {
  run_pty handoff_i
  local line
  line=$(cat "${LOGDIR}/handoff.out")
  [ "${line}" = 'job_in_tty_fg=yes job_shares_shell_pgrp=no' ]
}

# assert_has TEXT PART: fail unless TEXT contains PART.
assert_has() {
  [[ $1 == *"$2"* ]]
}

# assert_lacks TEXT PART: fail if TEXT contains PART.
assert_lacks() {
  [[ $1 != *"$2"* ]]
}

@test "terminal INTR, QUIT and SUSP characters signal only the foreground process group" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty keys
  local fg_signals bg_signals fg_pgrp bg_pgrp
  fg_signals=$(signals_of fg)
  bg_signals=$(signals_of bg)
  [ "${fg_signals}" = "sig INT sig QUIT sig TSTP " ]
  [ -z "${bg_signals// /}" ]
  fg_pgrp=$(pgrp_of fg)
  bg_pgrp=$(pgrp_of bg)
  [ -n "${fg_pgrp}" ]
  [ -n "${bg_pgrp}" ]
  [ "${fg_pgrp}" != "${bg_pgrp}" ]
  grep -qx 'init INT=DFL QUIT=DFL TSTP=DFL TTIN=DFL TTOU=DFL' "${LOGDIR}/fg.log"
  grep -qx 'init INT=DFL QUIT=DFL TSTP=DFL TTIN=DFL TTOU=DFL' "${LOGDIR}/bg.log"
}

@test "clearing ISIG makes the same keystrokes arrive as data and deliver no signal" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty isig_off
  local fg_signals tty_text
  fg_signals=$(signals_of fg)
  [ -z "${fg_signals// /}" ]
  tty_text=$(cat "${LOGDIR}/tty.out")
  assert_has "${tty_text}" '^C^\^Z'
}

@test "stty moves the INTR and QUIT characters, so the same key sends a different signal" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty intr_remap
  local fg_signals
  fg_signals=$(signals_of fg)
  [ "${fg_signals}" = "sig QUIT sig INT " ]
}

@test "closing the terminal master sends SIGHUP to the foreground process group" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty hup
  lab_poll 5 grep -qx 'sig HUP' "${LOGDIR}/fg.log"
}

@test "a background read stops the job with SIGTTIN and the shell reports it as stopped" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tin
  local state jobs_out family version machtype
  state=$(cat "${LOGDIR}/inner.log")
  [ "${state}" = tin_state=T ]
  if grep -q '^read' "${LOGDIR}/tin.log"; then
    false
  fi
  jobs_out=$(cat "${LOGDIR}/jobs.out")
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  # Each shell words the state of the stopped job its own way; assert the whole state text, not a prefix, so
  # that a shell reporting "Running" or a different signal name fails. Job number, padding and command vary.
  case ${family} in
    zsh | zsh-sh) assert_has "${jobs_out}" ' suspended (tty input)  ' ;;
    bash)
      # Apple's /bin/bash 3.2.57 asks waitpid for stops only when interactive, so a non-interactive `jobs` says
      # Running; upstream 3.2.57 (musl Linux, or built on macOS) asks whenever job control is on and says Stopped
      # with no signal name. $MACHTYPE tells the Apple build apart: it is a darwin triple.
      # In POSIX mode (bash invoked as sh, or set -o posix) bash names the signal, with no space: Stopped(SIGTTIN).
      run_lab_c "printf %s \"\$MACHTYPE\""
      machtype=$output
      # shellcheck disable=SC2016 # $SHELLOPTS must expand in the shell under test, not in bats
      run_lab_c 'case :$SHELLOPTS: in *:posix:*) echo posix ;; *) echo default ;; esac'
      posix=$output
      case ${version}:${machtype}:${posix} in
        3.*:*-apple-darwin*:*) printf '%s\n' "${jobs_out}" | grep -Eq '^\[[0-9]+\]\+ +Running +[^ ]' ;;
        *:posix) printf '%s\n' "${jobs_out}" | grep -Eq '^\[[0-9]+\]\+ +Stopped\(SIGTTIN\) +[^ (]' ;;
        *) printf '%s\n' "${jobs_out}" | grep -Eq '^\[[0-9]+\]\+ +Stopped +[^ (]' ;;
      esac
      ;;
    dash | busybox | mksh) assert_has "${jobs_out}" ' Stopped (tty input)' ;;
    ksh93) assert_has "${jobs_out}" ' Stopped (SIGTTIN)' ;;
    yash) assert_has "${jobs_out}" ' Stopped(SIGTTIN)' ;;
    *) false ;;
  esac
}

@test "a background read fails with EIO instead of stopping when SIGTTIN is ignored" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tin_ign
  grep -qx 'read ret=-1 err=EIO' "${LOGDIR}/tin.log"
}

@test "a background read fails with EIO instead of stopping when SIGTTIN is blocked" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tin_blk
  grep -qx 'read ret=-1 err=EIO' "${LOGDIR}/tin.log"
}

@test "with TOSTOP set a background write stops the job with SIGTTOU" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tout
  local state tty_text
  state=$(cat "${LOGDIR}/inner.log")
  [ "${state}" = tout_state=T ]
  if grep -q '^after' "${LOGDIR}/tout.log"; then
    false
  fi
  tty_text=$(cat "${LOGDIR}/tty.out")
  assert_lacks "${tty_text}" "<<tty-write>>"
}

@test "without TOSTOP a background write reaches the terminal" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tout_off
  local tty_text
  grep -qx 'after write ret=13 err=-' "${LOGDIR}/tout.log"
  tty_text=$(cat "${LOGDIR}/tty.out")
  assert_has "${tty_text}" "<<tty-write>>"
}

@test "ignoring SIGTTOU lets a background write through even with TOSTOP set" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tout_ign
  local tty_text
  grep -qx 'after write ret=13 err=-' "${LOGDIR}/tout.log"
  tty_text=$(cat "${LOGDIR}/tty.out")
  assert_has "${tty_text}" "<<tty-write>>"
}

@test "tcsetattr from a background job stops it with SIGTTOU even though TOSTOP is clear" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty tset
  local state
  state=$(cat "${LOGDIR}/inner.log")
  [ "${state}" = tset_state=T ]
  if grep -q '^after' "${LOGDIR}/tset.log"; then
    false
  fi
}

@test "under set -m a background job starts with every job control signal at its default action" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty jc_disp
  local line
  line=$(cat "${LOGDIR}/jc_disp.out")
  [ "${line}" = 'jc_async INT=DFL QUIT=DFL TSTP=DFL TTIN=DFL TTOU=DFL CONT=DFL HUP=DFL' ]
}

@test "whether a foreground job owns the terminal and gets its own group depends on the shell" {
  if is_posh; then
    assert_posh_has_no_monitor
    return 0
  fi
  run_pty handoff
  local line family
  line=$(cat "${LOGDIR}/handoff.out")
  family=$(lab_shell_family)
  case ${family} in
    bash | dash | busybox | yash)
      [ "${line}" = 'job_in_tty_fg=yes job_shares_shell_pgrp=no' ]
      ;;
    zsh | zsh-sh | mksh)
      # a zsh or mksh script that only runs `set -m` gives each job a group but never hands it the terminal
      [ "${line}" = 'job_in_tty_fg=no job_shares_shell_pgrp=no' ]
      assert_interactive_handoff
      ;;
    ksh93)
      # ksh93u+ 2012 and ksh93u+m 1.0.10 keep a foreground job in the shell's own group, which owns the terminal
      [ "${line}" = 'job_in_tty_fg=yes job_shares_shell_pgrp=yes' ]
      assert_interactive_handoff
      ;;
    *) false ;;
  esac
}

@test "without job control an asynchronous list starts with SIGINT and SIGQUIT ignored" {
  run_lab "${PROBE}" async_disp
  [ "${status}" -eq 0 ]
  local family version want
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case ${family} in
    bash | zsh | zsh-sh | dash | busybox | ksh93 | yash) want=IGN ;;
    mksh)
      # mksh R59 2020/10/31 (Alpine) leaves them at the default action; R59 2025/04/26 ignores them
      case ${version} in
        *2020*) want=DFL ;;
        *) want=IGN ;;
      esac
      ;;
    posh) want=DFL ;;
    *) false ;;
  esac
  grep -qx 'fg INT=DFL QUIT=DFL TSTP=DFL TTIN=DFL TTOU=DFL CONT=DFL HUP=DFL' "${LOGDIR}/disp.out"
  grep -qx "async INT=${want} QUIT=${want} TSTP=DFL TTIN=DFL TTOU=DFL CONT=DFL HUP=DFL" "${LOGDIR}/disp.out"
}

@test "SIGTSTP stops a process, nothing runs while it is stopped, and SIGCONT resumes it" {
  run_lab "${PROBE}" stop_cont
  [ "${status}" -eq 0 ]
  local result
  result=$(tr '\n' ' ' <"${LOGDIR}/inner.log")
  [ "${result}" = "stopped=yes frozen=yes resumed=yes " ]
}

@test "SIGCONT resumes a stopped process that ignores SIGCONT" {
  run_lab "${PROBE}" cont_ign
  [ "${status}" -eq 0 ]
  local result
  result=$(tr '\n' ' ' <"${LOGDIR}/inner.log")
  [ "${result}" = "stopped=yes frozen=yes resumed=yes " ]
}

@test "a process can ignore SIGTSTP but cannot ignore SIGSTOP" {
  run_lab "${PROBE}" tstp_ign
  [ "${status}" -eq 0 ]
  local result
  result=$(tr '\n' ' ' <"${LOGDIR}/inner.log")
  [ "${result}" = "tstp_stopped=no tstp_ticking=yes stop_stopped=yes " ]
}
