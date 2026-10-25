#!/usr/bin/env bats
# Tests for process spawning, subshell behavior, posix_spawn actions, and fork/vfork invariants.

setup() {
  load ../lib/lab
  SPAWN_PROBE="$BATS_TEST_DIRNAME/spawn_probe.sh"
  RUN_HARNESS="$BATS_TEST_DIRNAME/run_harness.sh"
}

@test "posix_spawn: \$\$ PID remains invariant in subshells" {
  run_lab "$SPAWN_PROBE" pid-subshell
  [ "$status" -eq 0 ]
  [[ "$output" =~ ^pid_match:\ [0-9]+$ ]]
}

@test "posix_spawn: missing executable exits 127 and unexecutable exits 126" {
  run_lab "$SPAWN_PROBE" exit-codes "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "missing:127 noexec:126" ]
}

@test "posix_spawn: child subshell cannot mutate parent environment" {
  run_lab "$SPAWN_PROBE" env-isolation
  [ "$status" -eq 0 ]
  [ "$output" = "parent_val:initial_parent_value" ]
}

@test "posix_spawn: exec'd child inherits descriptor 3 except where the shell sets cloexec (ksh93, mksh)" {
  run_lab "$SPAWN_PROBE" fd-inheritance "$BATS_TEST_TMPDIR/fd_out.txt"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93 | mksh)
      [ "$output" = "not_inherited" ]
      ;;
    bash | zsh | zsh-sh | dash | yash | busybox | posh)
      [ "$output" = "inherited_write" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "posix_spawn: subshell execution is virtual in ksh93 but forked in other families" {
  # shellcheck disable=SC2016 # expansions run under the shell under test
  run_lab_c '
    tmp="'"$BATS_TEST_TMPDIR"'/sub_ppid.txt"
    main_pid=$$
    (sh -c "echo \$PPID" > "$tmp"; echo finish >/dev/null)
    sub_ppid=$(cat "$tmp")
    if [ "$main_pid" = "$sub_ppid" ]; then
      echo virtual_subshell
    else
      echo forked_subshell
    fi
  '
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case "$family" in
    ksh93)
      [ "$output" = "virtual_subshell" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      [ "$output" = "forked_subshell" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "posix_spawn: C harness executes file actions and descriptor redirection" {
  run_lab "$RUN_HARNESS" "$BATS_TEST_TMPDIR" actions "$BATS_TEST_TMPDIR/action_out.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "spawn_actions_success" ]
  run cat "$BATS_TEST_TMPDIR/action_out.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "posix_spawn_file_actions_ok" ]
}

@test "posix_spawn: C harness returns ENOENT directly on missing executable" {
  run_lab "$RUN_HARNESS" "$BATS_TEST_TMPDIR" missing
  family=$(lab_shell_family)
  case "$family" in
    posh)
      [ "$status" -ne 0 ]
      [[ "$output" == *"@: parameter not set"* ]]
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      [ "$output" = "missing_err:2" ]
      ;;
    *)
      false
      ;;
  esac
}
