#!/bin/sh
# trap-probe: mechanism probes for traps and EXIT lifecycle.
# Usage: trap-probe.sh exit-clean DIR
#        trap-probe.sh exit-status
#        trap-probe.sh exit-override
#        trap-probe.sh subshell-trap
#        trap-probe.sh subshell-reset
#        trap-probe.sh sig-forward DIR

mode=${1:?mode}

case $mode in
  exit-clean)
    base_dir=${2:?dir}
    scratch_dir=$(mktemp -d "$base_dir/probe.XXXXXX") || exit 1

    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    cleanup() {
      rm -rf "$scratch_dir"
      printf 'cleaned\n'
    }
    trap cleanup EXIT

    marker="$scratch_dir/marker.txt"
    printf 'active\n' >"$marker"
    m_exists=1
    if [ -f "$marker" ]; then
      m_exists=0
    fi

    printf 'created=%s marker=%d\n' "$scratch_dir" "$m_exists"
    ;;

  exit-status)
    # Verify that the exit status of the script is preserved across an EXIT trap
    # even when commands inside the trap succeed or fail (POSIX.1-2024 XCU 2.15).
    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    cleanup() {
      rc=$?
      # Commands executed in trap alter $? locally
      true
      echo "trap-seen-rc=$rc"
    }
    trap cleanup EXIT

    # Execute a failing command as the terminal action
    # shellcheck disable=SC2015
    false || (exit 42)
    ;;

  exit-override)
    # Verify that an explicit exit command inside the EXIT trap overrides
    # the process exit status.
    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    cleanup() {
      exit 77
    }
    trap cleanup EXIT

    (exit 0)
    ;;

  subshell-trap)
    # Verify subshell trap clearance and independent subshell EXIT trap lifecycle.
    # POSIX.1-2024 XCU 2.13: traps that are not ignored are reset to default on subshell entry.
    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    parent_cleanup() {
      printf 'parent-exit\n'
    }
    trap parent_cleanup EXIT

    (
      # Subshell sets its own EXIT trap
      # shellcheck disable=SC2317,SC2329 # invoked by subshell trap upon exit
      sub_cleanup() {
        printf 'subshell-exit\n'
      }
      trap sub_cleanup EXIT
      printf 'subshell-running\n'
    )

    printf 'parent-continuing\n'
    ;;

  subshell-reset)
    # Verify that a subshell resets the parent's traps instead of inheriting them.
    # POSIX.1-2024 XCU 2.13: traps that are not ignored are reset to default on subshell entry.
    # The subshells set no trap of their own, so the only way one can survive SIGUSR1, print
    # parent-usr1, or print parent-exit on a normal exit is by inheriting the parent's traps.
    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    parent_cleanup() {
      printf 'parent-exit\n'
    }
    trap parent_cleanup EXIT
    trap 'printf "parent-usr1\n"' USR1

    # The subshell runs in the background so that ksh93 forks a real process for it (its foreground
    # subshells share the parent's PID); the child shell then signals its parent, the subshell.
    # shellcheck disable=SC2016 # expanded by the child shell
    (
      /bin/sh -c 'kill -USR1 "$PPID"'
      printf 'subshell-survived\n'
    ) &
    wait "$!"
    printf 'sub-status=%s\n' "$?"

    # A subshell that exits normally must not run the parent EXIT action either.
    (
      printf 'subshell-normal\n'
    )
    printf 'parent-continuing\n'
    ;;

  sig-forward)
    base_dir=${2:?dir}
    scratch_dir=$(mktemp -d "$base_dir/probe.XXXXXX") || exit 1

    # shellcheck disable=SC2317 # invoked by shell trap upon exit
    cleanup() {
      rm -rf "$scratch_dir"
      printf 'signal-cleaned\n'
    }
    trap cleanup EXIT
    trap 'exit 143' TERM

    # Background process sends SIGTERM to the script after a delay
    (
      sleep 0.5
      kill -TERM "$$"
    ) &

    # Wait for the signal to be delivered
    sleep 2
    ;;

  *)
    printf 'unknown mode: %s\n' "$mode" >&2
    exit 2
    ;;
esac
