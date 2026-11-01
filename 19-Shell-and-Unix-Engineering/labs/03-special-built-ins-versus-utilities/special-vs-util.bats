#!/usr/bin/env bats
# Verification lab for Special Built-ins versus Utilities (POSIX.1-2024 XCU 2.15, 2.9.1, 1.7).

# probe_field CODE [SHELL_OPTION...]: run CODE under the shell under test with stderr discarded
# and print STATUS:OUTPUT, with each newline in the output written as a comma.
probe_field() {
  local code=$1 out rc=0
  shift
  lab_shell_words
  out=$("${LAB_SHELL_WORDS[@]}" "$@" -c "exec 2>/dev/null; $code" 2>/dev/null </dev/null) || rc=$?
  printf '%s:%s' "$rc" "${out//$'\n'/,}"
}

# table_row [SHELL_OPTION...]: print one probe_field per probe, space-separated, in this order:
# redirection error, the same under command, option error, the same under command, command :,
# redirection error on an AND-OR left side, option error there, redirection error in a subshell there,
# assignment before a special built-in, the same under command, printf with PATH=/nonexistent.
table_row() {
  local code field sep=""
  while IFS= read -r code; do
    field=$(probe_field "$code" "$@")
    printf '%s%s' "$sep" "${field}"
    sep=" "
  done <<'EOF'
: </nonexistent/f; echo survived
command : </nonexistent/f; echo "rc=$?"
set -Z; echo survived
command set -Z; echo "rc=$?"
command :; echo "rc=$?"
: </nonexistent/f || :; echo survived
set -Z || :; echo survived
( : </nonexistent/f; echo after ) && echo ok
V=p :; echo "V=${V-}"
V=p command :; echo "V=${V-}"
o=$PATH; PATH=/nonexistent; printf ""; r=$?; PATH=$o; echo "rc=$r"
EOF
}

# expect_row GOT WANT: fail unless they match, printing both for the bats failure report.
expect_row() {
  printf 'got:  %s\nwant: %s\n' "$1" "$2"
  [ "$1" = "$2" ]
}

# shell_is_posix: succeed when the shell under test starts in POSIX mode (bash invoked as sh).
shell_is_posix() {
  run_lab_c '[ -o posix ]'
  [ "$status" -eq 0 ]
}

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/special-vs-util.sh"
}

@test "assign: variable assignments before a special built-in persist unless command is used" {
  run_lab "$PROBE" assign
  [ "$status" -eq 0 ]
  out=$output
  family=$(lab_shell_family)
  case $family in
    dash | busybox | mksh | posh | yash | zsh-sh)
      [ "$out" = "special=persisted regular=init command=init" ]
      ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$out" = "special=persisted regular=init command=init" ]
      else
        [ "$out" = "special=init regular=init command=init" ]
      fi
      ;;
    zsh)
      [ "$out" = "special=init regular=init command=init" ]
      ;;
    ksh93)
      ver=$(lab_shell_version)
      case $ver in
        *93u+m*)
          [ "$out" = "special=persisted regular=init command=init" ]
          ;;
        *)
          [ "$out" = "special=persisted regular=init command=suppressed" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "redir-err: redirection failure on a special built-in aborts non-interactive execution unless suppressed by command" {
  run_lab "$PROBE" redir-err
  [ "$status" -eq 0 ]
  out=$output
  family=$(lab_shell_family)
  case $family in
    dash | busybox | mksh | posh | ksh93 | zsh-sh)
      [ "$out" = "special=aborted regular=survived command=survived" ]
      ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$out" = "special=aborted regular=survived command=survived" ]
      else
        [ "$out" = "special=survived regular=survived command=survived" ]
      fi
      ;;
    zsh | yash)
      [ "$out" = "special=survived regular=survived command=survived" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "util-err: option error on a special built-in aborts non-interactive execution unless suppressed by command" {
  run_lab "$PROBE" util-err
  [ "$status" -eq 0 ]
  out=$output
  family=$(lab_shell_family)
  case $family in
    dash | busybox | mksh | ksh93 | zsh | zsh-sh)
      [ "$out" = "special=aborted command=survived" ]
      ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$out" = "special=aborted command=survived" ]
      else
        [ "$out" = "special=survived command=survived" ]
      fi
      ;;
    yash)
      [ "$out" = "special=survived command=survived" ]
      ;;
    posh)
      # posh 0.14.1 call_builtin resets builtin_flag from tp->flag without masking SPEC_BI under command
      [ "$out" = "special=aborted command=aborted" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "precedence: functions override regular utilities, command bypasses functions, and POSIX special built-ins outrank functions" {
  run_lab "$PROBE" precedence
  [ "$status" -eq 0 ]
  out=$output
  family=$(lab_shell_family)
  case $family in
    dash | mksh | posh | ksh93 | yash)
      [ "$out" = "regular=func command_true=bypassed special=builtin" ]
      ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then
        [ "$out" = "regular=func command_true=bypassed special=builtin" ]
      else
        [ "$out" = "regular=func command_true=bypassed special=func" ]
      fi
      ;;
    zsh | zsh-sh | busybox)
      [ "$out" = "regular=func command_true=bypassed special=func" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "decl: export acts as a declaration utility suppressing field splitting on name=value arguments" {
  run_lab "$PROBE" decl
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case $family in
    dash | busybox | mksh | ksh93 | yash | bash | zsh-sh)
      [ "$output" = "export=<two words> regular_argc=2 regular_arg1=<d_var=two>" ]
      ;;
    zsh)
      [ "$output" = "export=<two words> regular_argc=1 regular_arg1=<d_var=two words>" ]
      ;;
    posh)
      [ "$output" = "export=<two> regular_argc=2 regular_arg1=<d_var=two>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "exec-pid: an external utility runs in a forked child whereas exec replaces the shell process in-place" {
  src="$BATS_TEST_TMPDIR/printpid.c"
  bin="$BATS_TEST_TMPDIR/printpid"
  pidfile="$BATS_TEST_TMPDIR/ext.pid"
  cat >"$src" <<'EOF'
#include <stdio.h>
#include <unistd.h>

int main(void) {
  printf("%ld\n", (long)getpid());
  return 0;
}
EOF
  "${CC:-cc}" -O2 -Wall -Werror -o "$bin" "$src"

  run_lab "$PROBE" exec-pid "$bin" "$pidfile"
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 2 ]
  case ${lines[0]} in
    "external=forked shell_pid="*)
      shell_pid=${lines[0]#external=forked shell_pid=}
      ;;
    *)
      false
      ;;
  esac
  [ -n "$shell_pid" ]
  [ "${lines[1]}" = "$shell_pid" ]
}

@test "command search: special and intrinsic built-ins resolve without PATH while external utilities require PATH" {
  bindir="$BATS_TEST_TMPDIR/bin"
  mkdir -p "$bindir"
  ext="$bindir/my_ext_probe"
  cat >"$ext" <<'EOF'
#!/bin/sh
printf 'ext_ok\n'
EOF
  chmod +x "$ext"

  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    orig_path=$PATH
    bindir=$1
    target=$2
    PATH=$bindir
    ext_with_path=$(my_ext_probe)
    PATH=/nonexistent
    set -- special_ok
    cd "$target" && intrinsic=cd_ok
    if my_ext_probe >/dev/null 2>&1; then
      ext_no_path=ran
    else
      ext_no_path=$?
    fi
    if printf "" >/dev/null 2>&1; then
      reg_no_path=builtin
    else
      reg_no_path=$?
    fi
    PATH=$orig_path
    printf "with_path=%s special=%s intrinsic=%s ext_no_path=%s reg_no_path=%s\n" \
      "$ext_with_path" "$1" "$intrinsic" "$ext_no_path" "$reg_no_path"
  ' sh "$bindir" "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  family=$(lab_shell_family)
  case $family in
    dash | busybox | bash | ksh93 | zsh | zsh-sh)
      [ "$output" = "with_path=ext_ok special=special_ok intrinsic=cd_ok ext_no_path=127 reg_no_path=builtin" ]
      ;;
    mksh | posh | yash)
      [ "$output" = "with_path=ext_ok special=special_ok intrinsic=cd_ok ext_no_path=127 reg_no_path=127" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "table rows: exit status and outcome of each special built-in error, with and without command and POSIX mode" {
  family=$(lab_shell_family)
  case $family in
    dash)
      row=$(table_row)
      expect_row "${row}" "2: 0:rc=2 2: 0:rc=2 0:rc=0 2: 2: 2: 0:V=p 0:V= 0:rc=0"
      ;;
    busybox)
      row=$(table_row)
      expect_row "${row}" "1: 0:rc=1 2: 0:rc=2 0:rc=0 1: 2: 1: 0:V=p 0:V= 0:rc=0"
      ;;
    ksh93)
      ver=$(lab_shell_version)
      case $ver in
        *93u+m*)
          row=$(table_row)
          expect_row "${row}" "1: 0:rc=1 2: 0:rc=2 0:rc=0 1: 2: 1: 0:V=p 0:V= 0:rc=0"
          ;;
        *)
          row=$(table_row)
          expect_row "${row}" "1: 0:rc=1 2: 0:rc=2 0:rc=0 1: 2: 1: 0:V=p 0:V=p 0:rc=0"
          ;;
      esac
      ;;
    mksh)
      row=$(table_row)
      expect_row "${row}" "1: 0:rc=1 1: 0:rc=2 0:rc=0 1: 1: 1: 0:V=p 0:V= 0:rc=127"
      ;;
    posh)
      row=$(table_row)
      expect_row "${row}" "1: 0:rc=1 1: 1: 0:rc=0 1: 1: 1: 0:V=p 0:V= 0:rc=127"
      ;;
    yash)
      row=$(table_row)
      expect_row "${row}" "0:survived 0:rc=2 0:survived 0:rc=2 0:rc=0 0:survived 0:survived 0:after,ok 0:V=p 0:V= 0:rc=127"
      row=$(table_row -o posix)
      expect_row "${row}" "2: 0:rc=2 2: 0:rc=2 0:rc=0 2: 2: 2: 0:V=p 0:V= 0:rc=127"
      ;;
    zsh)
      row=$(table_row)
      expect_row "${row}" "0:survived 0:rc=1 1: 0:rc=127 0:rc=127 0:survived 1: 0:after,ok 0:V= 0:V= 0:rc=0"
      ;;
    zsh-sh)
      row=$(table_row)
      expect_row "${row}" "1: 0:rc=1 1: 0:rc=1 0:rc=0 1: 1: 1: 0:V=p 0:V= 0:rc=0"
      ;;
    bash)
      ver=$(lab_shell_version)
      case $ver in
        3.*)
          # bash 3.2 still honors an AND-OR left side for a redirection error
          posix_row="1: 0:rc=1 2: 0:rc=2 0:rc=0 0:survived 0:survived 0:after,ok 0:V=p 0:V= 0:rc=0"
          ;;
        *)
          posix_row="1: 0:rc=1 2: 0:rc=2 0:rc=0 1: 0:survived 1: 0:V=p 0:V= 0:rc=0"
          ;;
      esac
      native_row="0:survived 0:rc=1 0:survived 0:rc=2 0:rc=0 0:survived 0:survived 0:after,ok 0:V= 0:V= 0:rc=0"
      if shell_is_posix; then
        row=$(table_row)
        expect_row "${row}" "$posix_row"
      else
        row=$(table_row)
        expect_row "${row}" "$native_row"
      fi
      row=$(table_row -o posix)
      expect_row "${row}" "$posix_row"
      ;;
    *)
      false
      ;;
  esac
}

@test "eval definition: a function named eval is rejected, ignored, or wins, per shell" {
  # shellcheck disable=SC2016 # the probe belongs to the shell under test
  code='eval() { echo func; }; eval "echo builtin"; echo after'
  family=$(lab_shell_family)
  case $family in
    dash)
      field=$(probe_field "$code")
      expect_row "${field}" "2:"
      ;;
    ksh93)
      field=$(probe_field "$code")
      expect_row "${field}" "1:"
      ;;
    mksh | posh)
      field=$(probe_field "$code")
      expect_row "${field}" "0:builtin,after"
      ;;
    yash)
      field=$(probe_field "$code")
      expect_row "${field}" "0:builtin,after"
      field=$(probe_field "$code" -o posix)
      expect_row "${field}" "0:builtin,after"
      ;;
    busybox | zsh | zsh-sh)
      field=$(probe_field "$code")
      expect_row "${field}" "0:func,after"
      ;;
    bash)
      ver=$(lab_shell_version)
      case $ver in
        3.*) posix_field="0:builtin,after" ;;
        *) posix_field="2:" ;;
      esac
      if shell_is_posix; then
        field=$(probe_field "$code")
        expect_row "${field}" "$posix_field"
      else
        field=$(probe_field "$code")
        expect_row "${field}" "0:func,after"
      fi
      field=$(probe_field "$code" -o posix)
      expect_row "${field}" "$posix_field"
      ;;
    *)
      false
      ;;
  esac
}

@test "unknown mode exits with status 2" {
  run_lab "$PROBE" invalid-mode
  [ "$status" -eq 2 ]
}
