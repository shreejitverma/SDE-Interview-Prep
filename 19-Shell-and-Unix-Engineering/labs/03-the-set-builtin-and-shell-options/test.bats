#!/usr/bin/env bats
# test.bats - tests for the set builtin and shell options lab

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe.sh"
}

# bash_mode: print "posix" or "native" for the bash under test (bash enters POSIX mode when invoked
# as sh, with --posix, or after set -o posix), then its major version, e.g. "posix 3".
bash_mode() {
  lab_shell_words
  # shellcheck disable=SC2016 # these expansions belong to the shell under test
  "${LAB_SHELL_WORDS[@]}" -c 'if [ -o posix ]; then m=posix; else m=native; fi; printf "%s %s\n" "$m" "${BASH_VERSINFO[0]}"'
}

# dash_pipefail_expected: print yes, no, or unknown for the dash under test.
# Upstream dash gained pipefail in 0.5.13; Debian 0.5.12-12 backports it by patch (measured).
# Alpine dash 0.5.12, macOS /bin/dash, and Ubuntu 24.04 0.5.12-6ubuntu5 have no pipefail: Debian
# added the patch in 0.5.12-7 (its changelog), after the 0.5.12-6 that Ubuntu 24.04 is based on.
dash_pipefail_expected() {
  local v os dash_path
  os=$(uname -s) || os=unknown
  dash_path=$(command -v dash) || dash_path=""
  if command -v dpkg-query >/dev/null 2>&1 && v=$(dpkg-query -W -f '${Version}' dash 2>/dev/null) && [ -n "$v" ]; then
    if dpkg --compare-versions "$v" ge 0.5.12-12; then
      echo yes
    elif [ "$v" = 0.5.12-6ubuntu5 ]; then
      echo no
    else
      echo unknown
    fi
  elif command -v apk >/dev/null 2>&1 && v=$(apk list -I dash 2>/dev/null) && [ -n "$v" ]; then
    case $v in
      dash-0.5.12-*) echo no ;;
      *) echo unknown ;;
    esac
  elif [ "$os" = Darwin ] && [ "$dash_path" = /bin/dash ]; then
    echo no
  else
    echo unknown
  fi
}

# shell_lists_option NAME: succeed when `set -o` of the shell under test lists the option NAME.
shell_lists_option() {
  local listing
  lab_shell_words
  listing=$("${LAB_SHELL_WORDS[@]}" -c 'set -o' 2>/dev/null) || return 1
  printf '%s\n' "$listing" | grep -q "^$1"
}

@test "probe.sh params: sets, unsets with --, and handles arguments with leading hyphens" {
  run_lab "$PROBE" params apple banana cherry
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "count=3" ]
  [ "${lines[1]}" = "first=apple" ]
  [ "${lines[2]}" = "cleared_count=0" ]
  [ "${lines[3]}" = "hyphen_count=3" ]
  [ "${lines[4]}" = "hyphen_first=-a" ]
}

@test "probe.sh glob-toggle: set -f disables pathname expansion in POSIX shells but toggles norcs in native zsh" {
  mkdir -p "$BATS_TEST_TMPDIR/globtest"
  touch "$BATS_TEST_TMPDIR/globtest/alpha.txt" "$BATS_TEST_TMPDIR/globtest/beta.txt"
  run_lab "$PROBE" glob-toggle "$BATS_TEST_TMPDIR/globtest"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "glob_on_count=2" ]
  case $(lab_shell_family) in
    zsh)
      # In native zsh, -f is NO_RCS rather than noglob (-F or set -o noglob)
      [ "${lines[1]}" = "glob_off_count=2" ]
      [ "${lines[2]}" = "glob_off_literal=$BATS_TEST_TMPDIR/globtest/alpha.txt" ]
      ;;
    bash | dash | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      [ "${lines[1]}" = "glob_off_count=1" ]
      [ "${lines[2]}" = "glob_off_literal=$BATS_TEST_TMPDIR/globtest/*" ]
      ;;
    *)
      false
      ;;
  esac
  [ "${lines[3]}" = "glob_restored_count=2" ]
}

@test "probe.sh save-restore: set +o output can be captured and evaluated to restore option state" {
  run_lab "$PROBE" save-restore
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "during_mutation=f_present" ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 set +o prints only enabled options (set -o opt) and omits disabled options;
      # evaluating it cannot restore disabled options after mutation
      [ "${lines[1]}" = "after_restore=f_present" ]
      ;;
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "${lines[1]}" = "after_restore=f_absent" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe.sh assignment: preceding variable assignment persists on special built-in in POSIX mode" {
  run_lab "$PROBE" assignment
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | ksh93 | posh | mksh | busybox | yash | zsh-sh)
      [ "$output" = "persisted_var=persisted_val" ]
      ;;
    bash)
      # Native bash scopes the prefix to the call; POSIX mode (sh, --posix) persists it
      case $(bash_mode) in
        posix*) [ "$output" = "persisted_var=persisted_val" ] ;;
        native*) [ "$output" = "persisted_var=unset" ] ;;
        *) false ;;
      esac
      ;;
    zsh)
      # Native zsh scopes prefix to command
      [ "$output" = "persisted_var=unset" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "bare set dumps all shell variables whereas set -- unsets positional parameters" {
  # shellcheck disable=SC2016
  run_lab_c '
    LAB_SENTINEL_VAR="sentinel_val_987"
    out_set=$(set)
    set -- "arg1" "arg2"
    count_before=$#
    set --
    count_after=$#
    case $out_set in
      *LAB_SENTINEL_VAR=*) found=yes ;;
      *LAB_SENTINEL_VAR*) found=names_only ;;
      *) found=no ;;
    esac
    printf "found=%s before=%d after=%d\n" "$found" "$count_before" "$count_after"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 bare set outputs only variable names without values (name instead of name=value)
      [ "$output" = "found=names_only before=2 after=0" ]
      ;;
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "$output" = "found=yes before=2 after=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unquoted empty variable pitfall: bare set empty dumps variables while set -- empty clears parameters" {
  # shellcheck disable=SC2016
  run_lab_c '
    empty=""
    # shellcheck disable=SC2086,SC2248 # intentional unquoted expansion to demonstrate the trap
    dump_out=$(set $empty)
    # With set --, zero remaining arguments unsets positional parameters without dumping
    # shellcheck disable=SC2086,SC2248 # intentional unquoted expansion
    safe_out=$(set -- $empty)
    case $dump_out in
      *empty=*) dumped=yes ;;
      *empty*) dumped=names_only ;;
      *) dumped=no ;;
    esac
    [ -z "$safe_out" ] && safe_empty=yes || safe_empty=no
    printf "dumped=%s safe_empty=%s\n" "$dumped" "$safe_empty"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 bare set dumps variable names without values
      [ "$output" = "dumped=names_only safe_empty=yes" ]
      ;;
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "$output" = "dumped=yes safe_empty=yes" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -u fails on unset variable but permits empty positional parameters in conforming shells" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -u
    # Unset variable expansion must fail
    err_status=0
    (unset UNSET_PROBE; : "$UNSET_PROBE") 2>/dev/null || err_status=$?
    # $@ expansion with zero arguments: POSIX exempts $@ from nounset
    set --
    # posh 0.14.1 aborts on empty "$@" under set -u; use standard ${1+"$@"} fallback or raw "$@"
    empty_status=0
    out=$(echo "$@") 2>/dev/null || empty_status=$?
    printf "err_failed=%d empty_status=%d out=%s\n" "$((err_status > 0 ? 1 : 0))" "$empty_status" "$out"
  '
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 aborts the command substitution on empty "$@" under -u (status 1) but the shell goes on
      [ "$status" -eq 0 ]
      [ "$output" = "err_failed=1 empty_status=1 out=" ]
      ;;
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "err_failed=1 empty_status=0 out=" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -o pipefail propagates upstream pipeline failure" {
  # shellcheck disable=SC2016
  run_lab_c '
    if (set -o pipefail) 2>/dev/null; then
      set -o pipefail
      (exit 42) | (exit 0)
      printf "pipefail: %d\n" "$?"
    else
      printf "pipefail: unsupported\n"
    fi
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "pipefail: unsupported" ]
      ;;
    dash)
      # Supported only where dash has pipefail; the expectation is pinned per package, then
      # cross-checked against what `set -o` lists so the two cannot disagree
      case $(dash_pipefail_expected) in
        yes)
          [ "$output" = "pipefail: 42" ]
          shell_lists_option pipefail
          ;;
        no)
          [ "$output" = "pipefail: unsupported" ]
          if shell_lists_option pipefail; then false; fi
          ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "$output" = "pipefail: 42" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "special built-in fatal error in non-interactive shell aborts execution under POSIX conformance" {
  # shellcheck disable=SC2016
  run_lab_c 'set -o invalid_option_name_xyz 2>/dev/null; printf "survived\n"'
  case $(lab_shell_family) in
    dash | ksh93)
      # POSIX special built-in error aborts the non-interactive shell with status 2
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      ;;
    zsh | zsh-sh | posh | mksh)
      # These shells abort with status 1
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    busybox)
      # BusyBox ash setcmd returns non-zero retval without raising an exception; shell continues
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    yash)
      # Native yash continues; POSIX mode (--posix or posixly-correct) aborts with status 2
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      lab_shell_words
      run "${LAB_SHELL_WORDS[@]}" --posix -c 'set -o invalid_option_name_xyz 2>/dev/null; printf "survived\n"'
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      run "${LAB_SHELL_WORDS[@]}" -c 'set -o posixly-correct; set -o invalid_option_name_xyz 2>/dev/null; printf "survived\n"'
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      ;;
    bash)
      # Native bash continues; POSIX mode aborts with status 2 from bash 4.4 on, but bash 3.2
      # (macOS /bin/sh) still continues after an unknown -o name even in POSIX mode
      case $(bash_mode) in
        native* | "posix 3")
          [ "$status" -eq 0 ]
          [ "$output" = "survived" ]
          ;;
        posix*)
          [ "$status" -eq 2 ]
          [ -z "$output" ]
          ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "special built-in unknown option letter: POSIX shells and bash in POSIX mode abort, native bash and yash continue" {
  # shellcheck disable=SC2016
  run_lab_c 'set -Z 2>/dev/null; printf "survived\n"'
  case $(lab_shell_family) in
    dash | ksh93 | busybox)
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      ;;
    zsh | zsh-sh | posh | mksh)
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    bash)
      # Unlike an unknown -o name, an unknown letter aborts in POSIX mode in bash 3.2 as well
      case $(bash_mode) in
        native*)
          [ "$status" -eq 0 ]
          [ "$output" = "survived" ]
          ;;
        posix*)
          [ "$status" -eq 2 ]
          [ -z "$output" ]
          ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "obsolescent set - disables -x and -v across traditional shells" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -x -v
    { set -; } 2>/dev/null
    case "$-" in
      *x*|*v*) printf "active\n" ;;
      *) printf "cleared\n" ;;
    esac
  '
  [ "$status" -eq 0 ]
  last_line="${lines[$((${#lines[@]} - 1))]}"
  case $(lab_shell_family) in
    zsh | yash | posh)
      # Native zsh, yash, and posh do not implement obsolescent set - (removed in POSIX Issue 6)
      [ "$last_line" = "active" ]
      ;;
    dash | bash | zsh-sh | ksh93 | mksh | busybox)
      # Traditional Bourne and POSIX shells clear -x and -v on obsolescent set -
      [ "$last_line" = "cleared" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "multi-digit positional parameter expansion: \$10 vs \${10}" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- a b c d e f g h i j
    printf "raw=%s braced=%s\n" "$10" "${10}"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # Native zsh treats $10 as parameter 10
      [ "$output" = "raw=j braced=j" ]
      ;;
    dash | bash | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      # POSIX sh treats $10 as $1 followed by literal 0
      [ "$output" = "raw=a0 braced=j" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -a (allexport) automatically exports newly assigned variables to children" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -a
    ALLEXPORT_TEST_VAR="allexport_active_456"
    sh -c '\''printf "child=%s\n" "${ALLEXPORT_TEST_VAR:-unset}"'\''
  '
  [ "$status" -eq 0 ]
  [ "$output" = "child=allexport_active_456" ]
}

@test "set +o round-trip restores -f -u -C -a except in posh, which omits disabled options" {
  # shellcheck disable=SC2016
  run_lab_c '
    for o in f u C a; do
      saved=$(set +o)
      set -"$o"
      case $- in *"$o"*) during=on ;; *) during=off ;; esac
      # posh prints a bare "set" when no option is on, and eval of it dumps the variables
      eval "$saved" >/dev/null
      case $- in *"$o"*) after=on ;; *) after=off ;; esac
      printf "%s during=%s after=%s\n" "$o" "$during" "$after"
    done
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "${lines[0]}" = "f during=on after=on" ]
      [ "${lines[1]}" = "u during=on after=on" ]
      [ "${lines[2]}" = "C during=on after=on" ]
      [ "${lines[3]}" = "a during=on after=on" ]
      ;;
    dash | bash | zsh | zsh-sh | ksh93 | mksh | busybox | yash)
      [ "${lines[0]}" = "f during=on after=off" ]
      [ "${lines[1]}" = "u during=on after=off" ]
      [ "${lines[2]}" = "C during=on after=off" ]
      [ "${lines[3]}" = "a during=on after=off" ]
      ;;
    *)
      false
      ;;
  esac
  [ "${#lines[@]}" -eq 4 ]
}

@test "set -n in a script file silently skips every later line, including set +n, and exits 0" {
  printf 'set -n\necho should-not-print\nset +n\necho after\n' >"$BATS_TEST_TMPDIR/noexec.sh"
  run_lab "$BATS_TEST_TMPDIR/noexec.sh"
  [ "$status" -eq 0 ]
  [ -z "$output" ]
}

@test "set -- accepts 100000 positional parameters" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- $(awk "BEGIN { for (i = 1; i <= 100000; i++) print i }")
    printf "count=%d last=%s\n" "$#" "${100000}"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "count=100000 last=100000" ]
}

@test "pipefail status when an upstream writer dies of SIGPIPE (yes | head -n 1)" {
  # shellcheck disable=SC2016
  run_lab_c '
    if (set -o pipefail) 2>/dev/null; then
      set -o pipefail
      { yes | head -n 1 >/dev/null; } 2>/dev/null
      printf "sigpipe: %d\n" "$?"
    else
      printf "sigpipe: unsupported\n"
    fi
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "sigpipe: unsupported" ]
      ;;
    dash)
      case $(dash_pipefail_expected) in
        yes) [ "$output" = "sigpipe: 141" ] ;;
        no) [ "$output" = "sigpipe: unsupported" ] ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | mksh | busybox)
      [ "$output" = "sigpipe: 141" ]
      ;;
    yash)
      # yash reports 384 + the signal number
      [ "$output" = "sigpipe: 397" ]
      ;;
    ksh93)
      # ksh93u+m socket pipelines race between ECONNRESET in the writer (exit 1) and SIGPIPE
      # (256 + 13); macOS ksh93u+ 2012 reports 13
      lab_shell_words
      # shellcheck disable=SC2016 # KSH_VERSION belongs to the shell under test
      v=$("${LAB_SHELL_WORDS[@]}" -c 'printf "%s" "$KSH_VERSION"')
      case $v in
        *93u+m*)
          case $output in
            "sigpipe: 1" | "sigpipe: 269") true ;;
            *) false ;;
          esac
          ;;
        *) [ "$output" = "sigpipe: 13" ] ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}
