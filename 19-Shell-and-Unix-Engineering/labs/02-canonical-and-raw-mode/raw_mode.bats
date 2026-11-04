#!/usr/bin/env bats
# Tests for canonical and raw terminal modes, VMIN/VTIME timings, and shell interactions.

setup() {
  load ../lib/lab
  SCRIPT="${BATS_TEST_DIRNAME}/raw_probe.sh"
  PROBE="${BATS_TEST_TMPDIR}/pty_raw_probe"
  if [ ! -x "${PROBE}" ]; then
    "${CC:-cc}" -O2 -Wall -Wextra -Werror "${BATS_TEST_DIRNAME}/pty_raw_probe.c" -o "${PROBE}"
  fi
  export PROBE
}

@test "canonical mode buffers input until newline arrives" {
  run_lab "${SCRIPT}" line-buffering
  [ "${status}" -eq 0 ]
  [ "${output}" = "canon_pending=0 canon_delivered=6 data=hello" ]
}

@test "canonical mode returns at most one line per read syscall" {
  run_lab "${SCRIPT}" max-one-line
  [ "${status}" -eq 0 ]
  [ "${output}" = "read1_len=6 read1=alpha read2_len=5 read2=beta" ]
}

@test "canonical line editing processes erase and kill characters" {
  run_lab "${SCRIPT}" editing
  [ "${status}" -eq 0 ]
  [ "${output}" = "erase_len=6 erase_str=fobar kill_len=6 kill_str=clean" ]
}

@test "noncanonical mode honors VMIN and VTIME cases" {
  run_lab "${SCRIPT}" vmin-vtime
  [ "${status}" -eq 0 ]
  [ "${output}" = "caseB_partial=0 caseB_full=3 caseC_timeout=0 caseC_waited=1 caseD_empty=0" ]
}

@test "noncanonical Case A starts the VTIME timer at the first byte and restarts it on every byte" {
  run_lab "${SCRIPT}" case-a
  [ "${status}" -eq 0 ]
  [ "${output}" = "caseA_idle=eintr caseA_full=3 caseA_short=2 caseA_short_waited=1 caseA_reset=5" ]
}

@test "raw mode disables visual echo and output carriage-return translation" {
  run_lab "${SCRIPT}" staircase-echo
  [ "${status}" -eq 0 ]
  [ "${output}" = "echo_on=1 echo_bytes=0 canon_crlf=1 raw_crlf=0 raw_ends_nl=1" ]
}

@test "raw mode disables signal generation on control characters" {
  run_lab "${SCRIPT}" signals
  [ "${status}" -eq 0 ]
  [ "${output}" = "canon_sig=2 raw_sig=0 raw_byte=0x03 read_ret=1" ]
}

@test "inspect control characters reflects termios NCCS and VMIN VTIME" {
  run_lab "${SCRIPT}" defaults
  [ "${status}" -eq 0 ]
  case $(uname -s) in
    Darwin)
      [ "${output}" = "NCCS=20 VEOF_idx=0 VEOL_idx=1 VMIN_idx=16 VTIME_idx=17 VEOF_val=0x04 VMIN_val=0x01
VTIME_val=0x00 VEOL_val=0xff VERASE_val=0x7f VKILL_val=0x15 VINTR_val=0x03 VQUIT_val=0x1c VSUSP_val=0x1a
MAX_CANON=1024 MAX_INPUT=1024 VDISABLE=0xff cc_t_bits=8" ]
      ;;
    Linux)
      [ "${output}" = "NCCS=32 VEOF_idx=4 VEOL_idx=11 VMIN_idx=6 VTIME_idx=5 VEOF_val=0x04 VMIN_val=0x01
VTIME_val=0x00 VEOL_val=0x00 VERASE_val=0x7f VKILL_val=0x15 VINTR_val=0x03 VQUIT_val=0x1c VSUSP_val=0x1a
MAX_CANON=255 MAX_INPUT=255 VDISABLE=0x00 cc_t_bits=8" ]
      ;;
    *) false ;;
  esac
}

@test "shell execution inside PTY handles canonical input across shell families" {
  lab_shell_words
  # shellcheck disable=SC2016 # $line expands inside the spawned child shell
  run_lab "${SCRIPT}" shell-exec "${LAB_SHELL_WORDS[@]}" -c 'read -r line; printf "SHELL_LINE=<%s>\n" "$line"'
  [ "${status}" -eq 0 ]
  clean_out=$(printf '%s\n' "${output}" | tr -d '\r')
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      case "${clean_out}" in
        *"SHELL_LINE=<abc>"*) ;;
        *) false ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "shell terminal probe runs under all supported shell families" {
  run_lab_c 'echo raw_mode_ok'
  [ "${status}" -eq 0 ]
  [ "${output}" = "raw_mode_ok" ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash) ;;
    *) false ;;
  esac
}
