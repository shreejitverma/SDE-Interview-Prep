#!/usr/bin/env bats
# Tests for SIGPIPE and EPIPE semantics across shell families and the kernel interface.

setup() {
  load ../lib/lab
  PROBE_SCRIPT="$BATS_TEST_DIRNAME/pipe_probe.sh"
}

@test "pipeline default exit status reflects the last command despite writer SIGPIPE" {
  run_lab_c 'yes 2>/dev/null | head -n 2 >/dev/null; echo "$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "0" ]
}

@test "direct SIGPIPE termination exit status reflects shell signal convention" {
  run_lab_c 'sh -c "kill -13 \$\$" 2>/dev/null; echo "$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93)
      [ "$output" = "269" ]
      ;;
    yash)
      [ "$output" = "397" ]
      ;;
    bash | zsh | zsh-sh | dash | posh | mksh | busybox)
      [ "$output" = "141" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipefail option support and exit status propagation across shell families" {
  family=$(lab_shell_family)
  case "$family" in
    posh)
      # posh does not support set -o pipefail
      run_lab_c 'set -o pipefail 2>/dev/null'
      [ "$status" -ne 0 ]
      ;;
    dash)
      # Debian dash supports pipefail (since 0.5.12-7); upstream / Alpine dash does not
      if run_lab_c 'set -o pipefail 2>/dev/null' && [ "$status" -eq 0 ]; then
        run_lab_c 'set -o pipefail; yes 2>/dev/null | head -n 1 >/dev/null; echo "$?"'
        [ "$status" -eq 0 ]
        [ "$output" = "141" ]
      else
        [ "$status" -ne 0 ]
      fi
      ;;
    bash | zsh | zsh-sh | mksh | busybox)
      run_lab_c 'set -o pipefail; yes 2>/dev/null | head -n 1 >/dev/null; echo "$?"'
      [ "$status" -eq 0 ]
      [ "$output" = "141" ]
      ;;
    yash)
      # A stage killed by SIGPIPE reports 384 + 13
      run_lab_c 'set -o pipefail; yes | head -n 1 >/dev/null; echo "$?"'
      [ "$status" -eq 0 ]
      [ "$output" = "397" ]
      # With a redirection, yash runs yes in a grandchild; the intermediate yash exits 397 & 255
      run_lab_c 'set -o pipefail; yes 2>/dev/null | head -n 1 >/dev/null; echo "$?"'
      [ "$status" -eq 0 ]
      [ "$output" = "141" ]
      ;;
    ksh93)
      run_lab_c 'set -o pipefail; yes 2>/dev/null | head -n 1 >/dev/null; echo "$?"'
      [ "$status" -eq 0 ]
      version=$(lab_shell_version)
      case "$version" in
        *93u+m*)
          # socketpair transport: yes dies of SIGPIPE (269) or, when head exits with unread
          # data queued, its first write fails with ECONNRESET and it exits 1; timing decides
          case "$output" in
            1 | 269) ;;
            *) false ;;
          esac
          # posix mode connects stages with pipe(2), so the writer always dies of SIGPIPE
          run_lab_c 'set -o posix; set -o pipefail; yes 2>/dev/null | head -n 1 >/dev/null; echo "$?"'
          [ "$status" -eq 0 ]
          [ "$output" = "269" ]
          ;;
        *)
          # ksh93u+ 2012 returns raw signal 13
          [ "$output" = "13" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "pipestatus array availability across shell families" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    bash | mksh)
      run_lab_c 'true | false; echo "${PIPESTATUS[0]} ${PIPESTATUS[1]}"'
      [ "$status" -eq 0 ]
      [ "$output" = "0 1" ]
      ;;
    zsh)
      run_lab_c 'true | false; echo "${pipestatus[1]} ${pipestatus[2]}"'
      [ "$status" -eq 0 ]
      [ "$output" = "0 1" ]
      ;;
    zsh-sh | dash | posh | yash | busybox | ksh93)
      # Shells without standard PIPESTATUS array
      run_lab_c 'true | false; echo "${PIPESTATUS:-unset}"'
      [ "$status" -eq 0 ]
      [ "$output" = "unset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "ignoring SIGPIPE converts broken pipe to EPIPE on write" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  run_lab_c '(trap "" PIPE; yes) | head -n 2'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93)
      version=$(lab_shell_version)
      case "$version" in
        *93u+m*)
          # socketpair transport: ECONNRESET if head exits with unread data queued, else EPIPE
          [[ "$output" =~ "Connection reset by peer" || "$output" =~ "Broken pipe" ]]
          # posix mode connects stages with pipe(2), so the failed write is always EPIPE
          run_lab_c 'set -o posix; (trap "" PIPE; yes) | head -n 2'
          [ "$status" -eq 0 ]
          [[ "$output" =~ "Broken pipe" ]]
          ;;
        *)
          [[ "$output" =~ "Broken pipe" ]]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | posh | mksh | yash | busybox)
      [[ "$output" =~ "Broken pipe" ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipe_probe script executes cleanly and demonstrates mechanism" {
  run_lab "$PROBE_SCRIPT"
  [ "$status" -eq 0 ]
  [[ "$output" =~ "pipeline exit status: 0" ]]
  case $(lab_shell_family) in
    ksh93)
      version=$(lab_shell_version)
      case "$version" in
        *93u+m*)
          # socketpair transport: ECONNRESET or EPIPE depending on timing (see test 5)
          [[ "$output" =~ "Connection reset by peer" || "$output" =~ "Broken pipe" ]]
          ;;
        *)
          [[ "$output" =~ "Broken pipe" ]]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | posh | mksh | yash | busybox)
      [[ "$output" =~ "Broken pipe" ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "kernel C probe verifies atomic EPIPE and pending SIGPIPE delivery" {
  cat <<'EOF' >"$BATS_TEST_TMPDIR/probe_kernel.c"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int main(void) {
    int fds[2];
    if (pipe(fds) != 0) return 1;
    if (close(fds[0]) != 0) return 2;

    sigset_t set, pending;
    sigemptyset(&set);
    sigaddset(&set, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &set, NULL) != 0) return 3;

    ssize_t n = write(fds[1], "x", 1);
    int err = errno;

    if (n != -1 || err != EPIPE) return 4;

    sigemptyset(&pending);
    if (sigpending(&pending) != 0) return 5;
    if (!sigismember(&pending, SIGPIPE)) return 6;

    close(fds[1]);
    printf("KERNEL_PROBE_OK EPIPE=%d\n", err);
    return 0;
}
EOF
  cc -O2 -o "$BATS_TEST_TMPDIR/probe_kernel" "$BATS_TEST_TMPDIR/probe_kernel.c"
  run "$BATS_TEST_TMPDIR/probe_kernel"
  [ "$status" -eq 0 ]
  [ "$output" = "KERNEL_PROBE_OK EPIPE=32" ]
}
