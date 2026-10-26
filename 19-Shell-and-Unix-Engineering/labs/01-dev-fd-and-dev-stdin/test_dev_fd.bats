#!/usr/bin/env bats
# Tests for /dev/fd, /dev/stdin, and descriptor plumbing across shells.

setup() {
  load ../lib/lab
  DEMO="$BATS_TEST_DIRNAME/dev_fd_demo.sh"
  PROBE_SRC="$BATS_TEST_DIRNAME/dev_fd_probe.c"
  PROBE_BIN="$BATS_TEST_TMPDIR/dev_fd_probe"
}

@test "reading from /dev/stdin consumes stream data" {
  payload="quantum-payload-3891"
  run_lab_c "printf '%s\n' \"${payload}\" | \"${DEMO}\" stream"
  case $(lab_shell_family) in
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          case "$(uname -s)" in
            Linux)
              # ksh93u+m on Linux connects pipelines with socketpair(2);
              # opening /dev/stdin (/proc/self/fd/0) calls sock_no_open returning ENXIO
              [ "${status}" -ne 0 ]
              case "${output}" in
                *"No such device or address"*) ;;
                *) false ;;
              esac
              ;;
            Darwin | FreeBSD)
              [ "${status}" -eq 0 ]
              [ "${output}" = "${payload}" ]
              ;;
            *)
              false
              ;;
          esac
          ;;
        *)
          [ "${status}" -eq 0 ]
          [ "${output}" = "${payload}" ]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      [ "${status}" -eq 0 ]
      [ "${output}" = "${payload}" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "sequential reads from /dev/stdin reflect kernel offset semantics" {
  sample_file="$BATS_TEST_TMPDIR/sample.txt"
  printf 'alpha\nbeta\n' >"${sample_file}"
  run_lab_c "\"${DEMO}\" reread < \"${sample_file}\""
  [ "${status}" -eq 0 ]
  case "$(uname -s)" in
    Darwin | FreeBSD)
      [ "${output}" = "1: alpha"$'\n'"2: beta" ]
      ;;
    Linux)
      [ "${output}" = "1: alpha"$'\n'"2: alpha" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "descriptor duplication with <& operates without filesystem paths" {
  payload="duplicated-channel"
  run_lab_c "printf '%s\n' \"${payload}\" | \"${DEMO}\" dup 3<&0"
  [ "${status}" -eq 0 ]
  [ "${output}" = "dup: ${payload}" ]
}

@test "opening a closed descriptor via /dev/fd returns non-zero error" {
  run_lab_c 'exec 3</dev/null; exec 3<&-; cat /dev/fd/3 2>&1'
  [ "${status}" -ne 0 ]
  case "$(uname -s)" in
    Darwin | FreeBSD)
      case "${output}" in
        *"Bad file descriptor"*) ;;
        *) false ;;
      esac
      ;;
    Linux)
      case "${output}" in
        *"No such file or directory"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "C probe confirms kernel offset behavior on regular file" {
  "${CC:-cc}" -Wall -Wextra -O2 -o "${PROBE_BIN}" "${PROBE_SRC}"
  sample_file="$BATS_TEST_TMPDIR/probe_sample.txt"
  run "${PROBE_BIN}" offset "${sample_file}"
  [ "${status}" -eq 0 ]
  case "$(uname -s)" in
    Darwin | FreeBSD)
      [ "${output}" = "offset: bsd-dup-ofd" ]
      ;;
    Linux)
      [ "${output}" = "offset: linux-new-ofd" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "C probe confirms socket open handling across kernels" {
  "${CC:-cc}" -Wall -Wextra -O2 -o "${PROBE_BIN}" "${PROBE_SRC}"
  run "${PROBE_BIN}" socket
  [ "${status}" -eq 0 ]
  case "$(uname -s)" in
    Darwin | FreeBSD)
      [ "${output}" = "socket: dup-ok" ]
      ;;
    Linux)
      [ "${output}" = "socket: enxio" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "C probe confirms closed descriptor error reporting" {
  "${CC:-cc}" -Wall -Wextra -O2 -o "${PROBE_BIN}" "${PROBE_SRC}"
  run "${PROBE_BIN}" closed
  [ "${status}" -eq 0 ]
  case "$(uname -s)" in
    Darwin | FreeBSD)
      [ "${output}" = "closed: ebadf" ]
      ;;
    Linux)
      [ "${output}" = "closed: enoent" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "C probe confirms anonymous pipe open via /dev/fd" {
  "${CC:-cc}" -Wall -Wextra -O2 -o "${PROBE_BIN}" "${PROBE_SRC}"
  run "${PROBE_BIN}" pipe
  [ "${status}" -eq 0 ]
  [ "${output}" = "pipe: ok" ]
}

@test "process substitution availability matches shell family matrix" {
  run_lab "${DEMO}" procsub
  [ "${status}" -eq 0 ]
  actual_output="${output}"
  case $(lab_shell_family) in
    bash)
      if run_lab_c '[ -o posix ]' 2>/dev/null && [ "${status}" -eq 0 ]; then
        case $(lab_shell_version) in
          [0-4].*)
            [ "${actual_output}" = "procsub: unsupported" ]
            ;;
          *)
            [ "${actual_output}" = "procsub: supported" ]
            ;;
        esac
      else
        [ "${actual_output}" = "procsub: supported" ]
      fi
      ;;
    zsh | zsh-sh | ksh93 | busybox)
      [ "${actual_output}" = "procsub: supported" ]
      ;;
    yash)
      if run_lab_c '[ -o posix ]' 2>/dev/null && [ "${status}" -eq 0 ]; then
        [ "${actual_output}" = "procsub: unsupported" ]
      else
        [ "${actual_output}" = "procsub: redirection only" ]
      fi
      ;;
    dash | posh | mksh)
      [ "${actual_output}" = "procsub: unsupported" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unknown action in demo script fails with status 2" {
  run_lab "${DEMO}" invalid-action
  [ "${status}" -eq 2 ]
}
