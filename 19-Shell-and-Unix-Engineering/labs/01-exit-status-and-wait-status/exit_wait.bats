#!/usr/bin/env bats
# Tests for process exit status, wait status, bitfield encoding, and shell translations.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/status_probe.sh"
}

@test "normal exit status 0 and 42 are propagated" {
  run_lab "${PROBE}" direct_exit 0
  [ "${status}" -eq 0 ]

  run_lab "${PROBE}" direct_exit 42
  [ "${status}" -eq 42 ]

  run_lab "${PROBE}" subshell_exit 42
  [ "${status}" -eq 0 ]
  [ "${output}" = "42" ]

  run_lab "${PROBE}" cmdsub_exit 42
  [ "${status}" -eq 0 ]
  [ "${output}" = "42" ]
}

@test "exit status is truncated modulo 256" {
  run_lab "${PROBE}" direct_exit 256
  [ "${status}" -eq 0 ]

  run_lab "${PROBE}" direct_exit 300
  [ "${status}" -eq 44 ]

  run_lab "${PROBE}" subshell_exit 256
  [ "${status}" -eq 0 ]
  [ "${output}" = "0" ]

  run_lab "${PROBE}" subshell_exit 300
  [ "${status}" -eq 0 ]
  [ "${output}" = "44" ]
}

@test "exit 257 produces 1 except in ksh93u+ 2012 which dies by signal 1" {
  run_lab "${PROBE}" direct_exit 257
  family=$(lab_shell_family)
  case "${family}" in
    bash | dash | zsh | zsh-sh | busybox | posh | mksh | yash)
      [ "${status}" -eq 1 ]
      ;;
    ksh93)
      version=$(lab_shell_version)
      case "${version}" in
        *93u+m*)
          # ksh93u+m masks the exit operand with SH_EXITMASK (bltins/cflow.c), so 257 exits 1
          [ "${status}" -eq 1 ]
          ;;
        *)
          # AT&T ksh93u+ 2012 treats 257 as 256 + SIGHUP and kills itself with signal 1;
          # bats runs under bash, which reports that signal death as 128 + 1 = 129
          [ "${status}" -eq 129 ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "negative exit argument is rejected by strict POSIX shells and wrapped by others" {
  run_lab "${PROBE}" direct_exit -1
  family=$(lab_shell_family)
  case "${family}" in
    bash | zsh | zsh-sh | ksh93 | mksh)
      [ "${status}" -eq 255 ]
      ;;
    dash | busybox | yash)
      [ "${status}" -eq 2 ]
      ;;
    posh)
      [ "${status}" -eq 1 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command not found produces exit status 127" {
  run_lab "${PROBE}" notfound_cmd
  [ "${status}" -eq 0 ]
  [ "${output}" = "127" ]
}

@test "command found but not executable produces exit status 126" {
  noexec_script="${BATS_TEST_TMPDIR}/noexec.sh"
  printf '#!/bin/sh\nexit 0\n' >"${noexec_script}"
  chmod -x "${noexec_script}"
  run_lab "${PROBE}" noexec_cmd "${noexec_script}"
  [ "${status}" -eq 0 ]
  [ "${output}" = "126" ]
}

@test "child killed by SIGTERM produces 128+signo, 256+signo in ksh93, 384+signo in yash" {
  run_lab "${PROBE}" signal_child
  [ "${status}" -eq 0 ]
  family=$(lab_shell_family)
  case "${family}" in
    bash | dash | zsh | zsh-sh | busybox | posh | mksh)
      [ "${output}" = "143" ]
      ;;
    ksh93)
      [ "${output}" = "271" ]
      ;;
    yash)
      [ "${output}" = "399" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "wait on unknown PID produces exit status 127" {
  run_lab "${PROBE}" wait_unknown
  [ "${status}" -eq 0 ]
  [ "${output}" = "127" ]
}

@test "repeated wait on reaped child retains status or reports 127" {
  run_lab "${PROBE}" repeat_wait 42
  [ "${status}" -eq 0 ]
  family=$(lab_shell_family)
  case "${family}" in
    bash)
      # In POSIX mode (macOS /bin/sh, bash --posix), bash forgets a waited-for pid (42 127);
      # in default mode it keeps the status in its saved-pid table (42 42).
      # shellcheck disable=SC2016 # SHELLOPTS belongs to the shell under test
      run_lab_c 'case ":${SHELLOPTS}:" in *:posix:*) echo posix ;; *) echo native ;; esac'
      mode="${output}"
      run_lab "${PROBE}" repeat_wait 42
      case "${mode}" in
        posix) [ "${output}" = "42 127" ] ;;
        *) [ "${output}" = "42 42" ] ;;
      esac
      ;;
    dash | busybox)
      [ "${output}" = "42 42" ]
      ;;
    zsh | zsh-sh | ksh93 | yash | mksh | posh)
      [ "${output}" = "42 127" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline exit status defaults to rightmost command" {
  run_lab "${PROBE}" pipeline_default
  [ "${status}" -eq 0 ]
  [ "${output}" = "0" ]
}

@test "pipeline exit status under pipefail reports rightmost failure where supported" {
  run_lab "${PROBE}" pipeline_pipefail
  [ "${status}" -eq 0 ]
  family=$(lab_shell_family)
  case "${family}" in
    bash | zsh | zsh-sh | ksh93 | mksh | yash)
      [ "${output}" = "22" ]
      ;;
    busybox)
      # BusyBox ash builds pipefail in with CONFIG_ASH_BASH_COMPAT (default y; Alpine and Debian).
      [ "${output}" = "22" ]
      ;;
    posh)
      [ "${output}" = "unsupported" ]
      ;;
    dash)
      # Debian's dash 0.5.12-7 and later carries the upstream pipefail patch (2024-04-05),
      # first released in dash 0.5.13; older builds reject the option, so ask the shell itself.
      probe_output="${output}"
      run_lab_c 'set -o pipefail 2>/dev/null'
      if [ "${status}" -eq 0 ]; then
        [ "${probe_output}" = "22" ]
      else
        [ "${probe_output}" = "unsupported" ]
      fi
      ;;
    *)
      false
      ;;
  esac
}

@test "local declaration masks command substitution failure status" {
  run_lab "${PROBE}" declaration_mask_trap 42
  [ "${status}" -eq 0 ]
  # Output must show masked:0 and unmasked:42, and local (or typeset in ksh93) masked:0
  [[ "${output}" =~ "export_masked:0" ]]
  [[ "${output}" =~ "export_unmasked:42" ]]
  [[ "${output}" =~ (local|typeset)_masked:0 ]]
}

@test "C probe verifies kernel wait status bitfield layout and waitid siginfo" {
  probe_bin="${BATS_TEST_TMPDIR}/inspect_wstatus"
  "${CC:-cc}" -Wall -Wextra -Werror -o "${probe_bin}" "${BATS_TEST_DIRNAME}/inspect_wstatus.c"

  run "${probe_bin}" exit_val 42
  [ "${status}" -eq 0 ]
  [ "${output}" = "raw=0x2a00 WIFEXITED=1 WEXITSTATUS=42" ]

  run "${probe_bin}" exit_val 256
  [ "${status}" -eq 0 ]
  [ "${output}" = "raw=0x0000 WIFEXITED=1 WEXITSTATUS=0" ]

  run "${probe_bin}" kill_sig 15
  [ "${status}" -eq 0 ]
  [ "${output}" = "raw=0x000f WIFSIGNALED=1 WTERMSIG=15 WCOREDUMP=0" ]

  stop_sig=$(kill -l STOP)
  expected_hex=$(printf '0x%02x7f' "${stop_sig}")
  run "${probe_bin}" stop_sig
  [ "${status}" -eq 0 ]
  [ "${output}" = "raw=${expected_hex} WIFSTOPPED=1 WSTOPSIG=${stop_sig}" ]

  run "${probe_bin}" waitid_probe 42
  [ "${status}" -eq 0 ]
  [[ "${output}" =~ "si_code=1 si_status=42" ]]
}
