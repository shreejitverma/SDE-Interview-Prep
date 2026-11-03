#!/usr/bin/env bats
# Tests for the exec special built-in utility across shell families.
# shellcheck disable=SC2016 # every quoted string passed to run_lab_c is code for the shell under test

setup_file() {
  "${CC:-cc}" -O2 -Wall -Wextra -Werror -D_FORTIFY_SOURCE=2 \
    -o "$BATS_FILE_TMPDIR/probe" "$BATS_TEST_DIRNAME/probe.c"
}

setup() {
  load ../lib/lab
  DEMO="$BATS_TEST_DIRNAME/exec_demo.sh"
  PROBE="$BATS_FILE_TMPDIR/probe"
}

# last_line: set LAST_LINE to the last line of the captured output.
last_line() {
  LAST_LINE=${lines[${#lines[@]} - 1]}
}

# sorted_output: set SORTED to the captured output lines in C-locale order.
sorted_output() {
  SORTED=$(LC_ALL=C sort <<<"$output")
}

# assert_exits N: the shell ended with status N before reaching the next command.
assert_exits() {
  [ "$status" -eq "$1" ]
  case "$output" in
    *after:*) return 1 ;;
    *) return 0 ;;
  esac
}

# assert_survives N: the shell reported a failure, kept running, and saw $? equal to N.
assert_survives() {
  [ "$status" -eq 0 ]
  last_line
  [ "$LAST_LINE" = "after:$1" ]
}

@test "exec replaces the shell image while preserving PID" {
  log_file="$BATS_TEST_TMPDIR/demo.log"
  run_lab "$DEMO" "$log_file" "$PROBE" pid
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 3 ]
  shell_pid="${lines[0]#shell_pid: }"
  [ "${lines[1]}" = "console: descriptors restored, replacing shell image" ]
  target_pid="${lines[2]}"
  [ "$shell_pid" = "$target_pid" ]
  run cat "$log_file"
  [ "$status" -eq 0 ]
  [ "$output" = "log: starting target command" ]
}

@test "the demo closes descriptors 3 and 4 before the target runs" {
  # ksh93 and mksh mark descriptors above 2 close-on-exec, so for them "closed" holds even without the close;
  # the check can fail in every shell that leaves them open (bash, dash, zsh, yash, posh, busybox).
  log_file="$BATS_TEST_TMPDIR/demo.log"
  run_lab "$DEMO" "$log_file" "$PROBE" fd 3
  [ "$status" -eq 0 ]
  last_line
  [ "$LAST_LINE" = "closed" ]
  run_lab "$DEMO" "$log_file" "$PROBE" fd 4
  [ "$status" -eq 0 ]
  last_line
  [ "$LAST_LINE" = "closed" ]
}

@test "exec with no command permanently redirects standard output in current shell" {
  out="$BATS_TEST_TMPDIR/out.txt"
  # The subshell keeps the redirection away from bats; both printf calls come after the exec returned.
  run_lab_c '( exec > "$1"; printf "line 1\n"; printf "line 2\n" )' sh "$out"
  [ "$status" -eq 0 ]
  # Nothing reached the original standard output: with no effect from exec, these lines would be here.
  [ -z "$output" ]
  run cat "$out"
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 2 ]
  [ "${lines[0]}" = "line 1" ]
  [ "${lines[1]}" = "line 2" ]
}

@test "exec restores standard descriptor via descriptor duplication" {
  out="$BATS_TEST_TMPDIR/saved.txt"
  run_lab_c '( exec 3>&1 1>"$1"; printf "to file\n"; exec 1>&3 3>&-; printf "to console\n" )' sh "$out"
  [ "$status" -eq 0 ]
  [ "$output" = "to console" ]
  run cat "$out"
  [ "$status" -eq 0 ]
  [ "$output" = "to file" ]
}

@test "exec bypasses functions in POSIX shells and runs them in zsh, mksh, and posh" {
  family=$(lab_shell_family)
  run_lab_c 'target_fn() { printf "in function\n"; }; exec target_fn; printf "after\n"'
  case "$family" in
    zsh | mksh | posh)
      # Native zsh, mksh (outside POSIX mode), and posh run the function, then the shell exits.
      [ "$status" -eq 0 ]
      [ "$output" = "in function" ]
      ;;
    bash | zsh-sh | yash | busybox | ksh93 | dash)
      # POSIX shells search PATH for a non-built-in utility and find none.
      [ "$status" -eq 127 ]
      case "$output" in
        *"not found"* | *"no such command"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
  if [ "$family" = mksh ]; then
    # set -o posix restricts the search to PATH, as the other shells do by default.
    run_lab_c 'set -o posix; target_fn() { printf "in function\n"; }; exec target_fn; printf "after\n"'
    [ "$status" -eq 127 ]
    case "$output" in
      *"inaccessible or not found"*) ;;
      *) false ;;
    esac
  fi
}

@test "exec bypasses built-in utilities in POSIX shells and runs them in zsh, mksh, and posh" {
  family=$(lab_shell_family)
  # ":" has no file in PATH, so only a shell that runs the built-in under exec returns 0.
  run_lab_c 'exec :; printf "after\n"'
  case "$family" in
    zsh | mksh | posh)
      [ "$status" -eq 0 ]
      [ -z "$output" ]
      ;;
    bash | zsh-sh | yash | busybox | ksh93 | dash)
      [ "$status" -eq 127 ]
      case "$output" in
        *"not found"* | *"no such command"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "EXIT trap is bypassed on successful exec process replacement" {
  run_lab_c 'trap "echo TRAP_RAN" EXIT; exec printf "EXEC_DONE\n"'
  [ "$status" -eq 0 ]
  [ "$output" = "EXEC_DONE" ]
}

@test "EXIT trap runs after exec resolved a function in mksh only" {
  family=$(lab_shell_family)
  run_lab_c 'trap "echo TRAP_RAN" EXIT; target_fn() { printf "in function\n"; }; exec target_fn'
  case "$family" in
    mksh)
      [ "$status" -eq 0 ]
      [ "${#lines[@]}" -eq 2 ]
      [ "${lines[0]}" = "in function" ]
      [ "${lines[1]}" = "TRAP_RAN" ]
      ;;
    zsh | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "in function" ]
      ;;
    bash | zsh-sh | yash | busybox | ksh93 | dash)
      # No function runs: exec finds no utility and the shell exits.
      # What the EXIT trap does on that failure path varies by shell and version, so it is not asserted.
      case "$output" in
        *"in function"*) false ;;
        *) true ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "assignment prefix on exec without operands persists in POSIX shells" {
  family=$(lab_shell_family)
  case "$family" in
    bash)
      # macOS /bin/sh is bash in POSIX mode, where the assignment persists; native bash drops it.
      run_lab_c '[ -o posix ]'
      posix=$status
      run_lab_c 'VAR=hello exec; printf "VAR=%s\n" "${VAR-unset}"'
      [ "$status" -eq 0 ]
      if [ "$posix" -eq 0 ]; then
        [ "$output" = "VAR=hello" ]
      else
        [ "$output" = "VAR=unset" ]
      fi
      # --posix forces POSIX mode in every bash version.
      lab_shell_words
      run "${LAB_SHELL_WORDS[@]}" --posix -c 'VAR=hello exec; printf "VAR=%s\n" "${VAR-unset}"'
      [ "$status" -eq 0 ]
      [ "$output" = "VAR=hello" ]
      ;;
    dash | ksh93 | mksh | posh | busybox | yash | zsh | zsh-sh)
      run_lab_c 'VAR=hello exec; printf "VAR=%s\n" "${VAR-unset}"'
      [ "$status" -eq 0 ]
      [ "$output" = "VAR=hello" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "assignment prefix on exec with a utility operand reaches the new image in every shell" {
  run_lab_c 'VAR=hello exec "$1" getenv VAR' sh "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "hello" ]
}

@test "file descriptors greater than 2 differ on cloexec across shell families" {
  family=$(lab_shell_family)
  out="$BATS_TEST_TMPDIR/fd_probe.txt"
  run_lab_c 'exec 3>"$1"; "$2" fd 3' sh "$out" "$PROBE"
  [ "$status" -eq 0 ]
  case "$family" in
    ksh93 | mksh)
      # ksh93 and mksh set FD_CLOEXEC on file descriptors > 2.
      [ "$output" = "closed" ]
      ;;
    bash | dash | zsh | zsh-sh | yash | busybox | posh)
      # Other shells leave descriptors > 2 open across execution of external utilities.
      [ "$output" = "open" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "descriptors greater than 2 stay open in the POSIX mode of every shell that has one" {
  family=$(lab_shell_family)
  # How each family enters POSIX mode inside the script; zsh-sh is already emulating sh, and dash, posh,
  # and busybox have no switch (set -o posix would end them).
  case "$family" in
    bash | mksh)
      run_lab_c 'set -o posix; exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
      [ "$status" -eq 0 ]
      [ "$output" = "open" ]
      ;;
    ksh93)
      # ksh93u+m has set -o posix; ksh93u+ (2012, macOS /bin/ksh) rejects the option and keeps FD_CLOEXEC.
      run_lab_c 'set -o posix' sh
      if [ "$status" -eq 0 ]; then
        run_lab_c 'set -o posix; exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
        [ "$status" -eq 0 ]
        [ "$output" = "open" ]
      else
        [ "$status" -eq 2 ]
        run_lab_c 'exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
        [ "$status" -eq 0 ]
        [ "$output" = "closed" ]
      fi
      ;;
    yash)
      run_lab_c 'set -o posixly-correct; exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
      [ "$status" -eq 0 ]
      [ "$output" = "open" ]
      ;;
    zsh-sh | zsh | dash | posh | busybox)
      run_lab_c 'exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
      [ "$status" -eq 0 ]
      [ "$output" = "open" ]
      ;;
    *)
      false
      ;;
  esac
  if [ "$family" = mksh ]; then
    # set -o sh is the other mode that skips FD_CLOEXEC (!Flag(FPOSIX) && !Flag(FSH) in c_exec).
    run_lab_c 'set -o sh; exec 3>/dev/null; "$1" fd 3' sh "$PROBE"
    [ "$status" -eq 0 ]
    [ "$output" = "open" ]
  fi
}

@test "exec -a overrides argv0 in supporting shells" {
  family=$(lab_shell_family)
  run_lab_c 'exec -a custom_prog "$1" argv0' sh "$PROBE"
  case "$family" in
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "custom_prog" ]
      ;;
    dash | posh)
      # dash and posh take "-a" as the command name and exit with status 127.
      [ "$status" -eq 127 ]
      case "$output" in
        *"-a: not found"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "exec -c executes with an empty environment in supporting shells" {
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  lab_shell_words
  # env -i gives the shell a known environment: one invalid-name entry, and an inherited HOME and SHLVL.
  # Without it the names depend on the caller (bats exports a function as BASH_FUNC_bats_readlinkf%%).
  run env -i PATH="$PATH" 'bad-ident=x' HOME=/inherited SHLVL=7 \
    "${LAB_SHELL_WORDS[@]}" -c 'exec -c "$1" env_names' sh "$PROBE"
  case "$family" in
    bash | zsh | zsh-sh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "env:" ]
      ;;
    mksh)
      # mksh keeps only _.
      [ "$status" -eq 0 ]
      sorted_output
      [ "$SORTED" = $'_\nenv:' ]
      ;;
    ksh93)
      [ "$status" -eq 0 ]
      case "$version" in
        *93u+m*)
          # ksh93u+m keeps _ and the inherited invalid-name entry, not HOME or SHLVL.
          sorted_output
          [ "$SORTED" = $'_\nbad-ident\nenv:' ]
          ;;
        *)
          sorted_output
          [ "$SORTED" = $'_\nenv:' ]
          ;;
      esac
      ;;
    dash | posh)
      # dash and posh take "-c" as the command name and exit with status 127.
      [ "$status" -eq 127 ]
      case "$output" in
        *"-c: not found"*) ;;
        *) false ;;
      esac
      ;;
    busybox)
      # busybox ash rejects the option with status 2.
      [ "$status" -eq 2 ]
      case "$output" in
        *"illegal option -c"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "exec -l prefixes argv0 with a dash where supported and fails differently elsewhere" {
  family=$(lab_shell_family)
  run_lab_c 'exec -l "$1" argv0; printf "after:%s\n" "$?"' sh "$PROBE"
  case "$family" in
    bash | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "-$PROBE" ]
      ;;
    mksh | yash)
      # The unknown option is a diagnostic and status 2; the shell goes on to the next command.
      assert_survives 2
      ;;
    ksh93 | busybox)
      # The unknown option is a usage error and ends the shell with status 2.
      assert_exits 2
      ;;
    dash | posh)
      # "-l" is taken as the command name.
      assert_exits 127
      case "$output" in
        *"-l: not found"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "failed exec in non-interactive shell exits immediately with 127" {
  family=$(lab_shell_family)
  run_lab_c 'exec ./nonexistent_file_xyz; printf "unreachable\n"'
  case "$family" in
    bash)
      version=$(lab_shell_version)
      case "$version" in
        3.2*)
          # GNU Bash 3.2 incorrectly exits with 126 (EX_NOEXEC) on slash pathnames.
          [ "$status" -eq 126 ]
          ;;
        *)
          # Bash 4.2+ conforms to POSIX and exits with 127 (EX_NOTFOUND).
          [ "$status" -eq 127 ]
          ;;
      esac
      ;;
    dash | zsh | zsh-sh | ksh93 | mksh | yash | busybox | posh)
      # POSIX conforming shells exit with 127 when utility is not found.
      [ "$status" -eq 127 ]
      ;;
    *)
      false
      ;;
  esac
  case "$output" in
    *unreachable*) false ;;
    *) true ;;
  esac
}

@test "failed exec of directory exits with status 126" {
  run_lab_c 'exec "$1"; printf "unreachable\n"' sh "$BATS_TEST_TMPDIR"
  [ "$status" -eq 126 ]
  case "$output" in
    *unreachable*) false ;;
    *) true ;;
  esac
}

@test "failed exec of a file without execute permission exits with status 126" {
  script="$BATS_TEST_TMPDIR/not_executable"
  : >"$script"
  chmod 644 "$script"
  run_lab_c 'exec "$1"; printf "unreachable\n"' sh "$script"
  [ "$status" -eq 126 ]
  case "$output" in
    *unreachable*) false ;;
    *) true ;;
  esac
}

@test "failed redirection on a bare exec ends the shell or leaves status 1 or 2" {
  family=$(lab_shell_family)
  lab_shell_words
  missing="$BATS_TEST_TMPDIR/no-such-dir/file"
  code='exec 3<"$1"; printf "after:%s\n" "$?"'
  case "$family" in
    bash)
      # Native bash continues with status 1; POSIX mode (macOS /bin/sh, or --posix) exits with status 1.
      run_lab_c '[ -o posix ]'
      posix=$status
      run_lab_c "$code" sh "$missing"
      if [ "$posix" -eq 0 ]; then
        assert_exits 1
      else
        assert_survives 1
      fi
      run "${LAB_SHELL_WORDS[@]}" --posix -c "$code" sh "$missing"
      assert_exits 1
      ;;
    zsh)
      run_lab_c "$code" sh "$missing"
      assert_survives 1
      ;;
    yash)
      run_lab_c "$code" sh "$missing"
      assert_survives 2
      run "${LAB_SHELL_WORDS[@]}" -o posixly-correct -c "$code" sh "$missing"
      assert_exits 2
      ;;
    dash)
      run_lab_c "$code" sh "$missing"
      assert_exits 2
      ;;
    zsh-sh | ksh93 | mksh | posh | busybox)
      run_lab_c "$code" sh "$missing"
      assert_exits 1
      ;;
    *)
      false
      ;;
  esac
}
