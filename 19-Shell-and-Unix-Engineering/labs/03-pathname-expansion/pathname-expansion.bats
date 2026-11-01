#!/usr/bin/env bats
# Pathname expansion: what the shell does with an unquoted pattern after field splitting.
# Every probe runs the shell under test (LAB_SHELL) through expand.sh inside a fixture tree.
# shellcheck disable=SC2016 # the code strings belong to the shell under test, not to this file

# The fixture's top level, in byte order (LC_ALL=C), without the hidden file.
ALL="12 args: <-n> <B1> <[x]> <^c> <a.c> <a1> <a[b> <b.c> <b.h> <dangling> <dir> <sp ace.c>"

setup() {
  load ../lib/lab
  LAB=$BATS_TEST_DIRNAME
  FX=$BATS_TEST_TMPDIR/fx
  # Several shells cite this source line in a diagnostic. Read it so a reformat cannot drift.
  EVAL_LINE=$(awk '{ if ($0 == "eval \"$2\"") { print NR; exit } }' "$LAB/expand.sh")
  [ -n "$EVAL_LINE" ]
  export LC_ALL=C
  mkdir "$FX"
  run_lab "$LAB/make-fixture.sh" "$FX"
  [ "$status" -eq 0 ]
}

# Restore a mode-000 directory even when an assertion above it fails.
teardown() {
  if [ -n "${PERM_LOCKED:-}" ] && [ -d "$PERM_LOCKED" ]; then
    chmod 755 "$PERM_LOCKED" 2>/dev/null || true
    unset PERM_LOCKED
  fi
}

# expand CODE: run CODE in the fixture tree; the result is in $status, $output, and $lines.
expand() {
  run_lab "$LAB/expand.sh" "$FX" "$1"
}

# expand_in DIR CODE: the same in another directory.
expand_in() {
  run_lab "$LAB/expand.sh" "$1" "$2"
}

# unchanged_in DIR PATTERN: assert the pattern matched nothing and survived as one literal word.
# Native zsh instead aborts the command with "no matches found" (its NOMATCH option).
unchanged_in() {
  expand_in "$1" "show $2"
  case $(lab_shell_family) in
    zsh)
      [ "$status" -eq 1 ]
      [ "$output" = "(eval):1: no matches found: $2" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      [ "$output" = "1 args: <$2>" ]
      ;;
    *) false ;;
  esac
}

unchanged() {
  unchanged_in "$FX" "$1"
}

# dots_listed: succeed when the shell under test includes . and .. among the matches of `.*`.
dots_listed() {
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.* | 4.* | 5.1*) return 0 ;;
        5.2* | 5.3*) return 1 ;;
        *) false ;;
      esac
      ;;
    dash | busybox | yash) return 0 ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) return 1 ;;
        *93u+*) return 0 ;;
        *) false ;;
      esac
      ;;
    mksh | posh | zsh | zsh-sh) return 1 ;;
    *) false ;;
  esac
}

@test "a pattern is replaced by the matching names in byte order, hidden names excluded" {
  expand 'show *'
  [ "$status" -eq 0 ]
  [ "$output" = "$ALL" ]
}

@test "a pattern that matches nothing is left unchanged, except in native zsh" {
  unchanged '*.zzz'
}

@test "a leading period must be matched explicitly, and .* lists dot and dot-dot only in some shells" {
  expand 'show .*'
  [ "$status" -eq 0 ]
  if dots_listed; then
    [ "$output" = "3 args: <.> <..> <.hidden>" ]
  else
    [ "$output" = "1 args: <.hidden>" ]
  fi
}

@test "the portable hidden-file pattern .[!.]* never lists dot or dot-dot" {
  expand 'show .[!.]*'
  [ "$status" -eq 0 ]
  [ "$output" = "1 args: <.hidden>" ]
}

@test "a non-matching bracket expression does not match a leading period either" {
  expand 'show [!a]*'
  [ "$status" -eq 0 ]
  [ "$output" = "9 args: <-n> <B1> <[x]> <^c> <b.c> <b.h> <dangling> <dir> <sp ace.c>" ]
}

@test "a caret negates a bracket expression in most shells but is a literal member in dash, mksh, and posh" {
  expand 'show [^a]*'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | mksh | posh)
      [ "$output" = "4 args: <^c> <a.c> <a1> <a[b>" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | ksh93)
      [ "$output" = "9 args: <-n> <B1> <[x]> <^c> <b.c> <b.h> <dangling> <dir> <sp ace.c>" ]
      ;;
    *) false ;;
  esac
}

@test "a character class works in a bracket expression except in posh" {
  expand 'show [[:lower:]]*'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "1 args: <[[:lower:]]*>" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "$output" = "8 args: <a.c> <a1> <a[b> <b.c> <b.h> <dangling> <dir> <sp ace.c>" ]
      ;;
    *) false ;;
  esac
}

@test "a slash is matched only by a slash: star and a bracket expression cannot cross a directory" {
  expand 'show d*/x.c'
  [ "$status" -eq 0 ]
  [ "$output" = "1 args: <dir/x.c>" ]
  unchanged 'd*x.c'
  unchanged 'dir?x.c'
  unchanged 'dir[/]x.c'
}

@test "a leading period after a slash must also be matched explicitly" {
  expand 'show dir/*'
  [ "$status" -eq 0 ]
  [ "$output" = "2 args: <dir/sub> <dir/x.c>" ]
  expand 'show */.dot'
  [ "$status" -eq 0 ]
  [ "$output" = "1 args: <dir/.dot>" ]
}

@test "a trailing slash keeps only directories" {
  expand 'show */'
  [ "$status" -eq 0 ]
  [ "$output" = "2 args: <a[b/> <dir/>" ]
}

@test "a dangling symlink is listed because expansion reads names and never opens them" {
  expand 'show d*; [ -e dangling ] || echo "-e says missing"; [ -L dangling ] && echo "-L says link"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "2 args: <dangling> <dir>" ]
  [ "${lines[1]}" = "-e says missing" ]
  [ "${lines[2]}" = "-L says link" ]
}

@test "quoted or escaped pattern characters are literal, and a quoted part does not stop the rest from globbing" {
  expand 'show "*"; show \*; show a"*"; show "a"*'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1 args: <*>" ]
  [ "${lines[1]}" = "1 args: <*>" ]
  [ "${lines[2]}" = "1 args: <a*>" ]
  [ "${lines[3]}" = "3 args: <a.c> <a1> <a[b>" ]
}

@test "a pattern produced by parameter expansion globs unless quoted, except in native zsh" {
  expand 'v="*.c"; show $v; show "$v"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "1 args: <*.c>" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "${lines[0]}" = "3 args: <a.c> <b.c> <sp ace.c>" ]
      ;;
    *) false ;;
  esac
  [ "${lines[1]}" = "1 args: <*.c>" ]
}

@test "a pattern produced by command substitution globs, except in native zsh" {
  expand 'show $(printf "%s" "*.c")'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "$output" = "1 args: <*.c>" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "3 args: <a.c> <b.c> <sp ace.c>" ]
      ;;
    *) false ;;
  esac
}

@test "pathname expansion runs after field splitting, and a matched name with a space is not split again" {
  expand 'IFS=:; v="*.c:b.h"; show $v'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "$output" = "1 args: <*.c:b.h>" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "4 args: <a.c> <b.c> <sp ace.c> <b.h>" ]
      ;;
    *) false ;;
  esac
}

@test "listing names through command substitution splits a name that contains a space" {
  expand 'show $(printf "%s\n" *.c)'
  [ "$status" -eq 0 ]
  [ "$output" = "4 args: <a.c> <b.c> <sp> <ace.c>" ]
}

@test "set -o noglob disables expansion everywhere, but set -f does so only outside native zsh" {
  expand 'set -o noglob; show *'
  [ "$status" -eq 0 ]
  [ "$output" = "1 args: <*>" ]
  expand 'set -f; show *'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "$output" = "$ALL" ]
      expand 'set -f; if [[ -o norcs ]]; then printf "%s\n" norcs-on; else printf "%s\n" norcs-off; fi; if [[ -o noglob ]]; then printf "%s\n" noglob-on; else printf "%s\n" noglob-off; fi'
      [ "$status" -eq 0 ]
      [ "$output" = "norcs-on"$'\n'"noglob-off" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "1 args: <*>" ]
      ;;
    *) false ;;
  esac
}

@test "an assignment and a case pattern follow no filename rules" {
  expand 'v=*; show "$v"; case .hidden in *) echo dot-ok;; esac; case a/b in a*b) echo slash-ok;; esac; case .hidden in [!a]*) echo neg-ok;; esac'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1 args: <*>" ]
  [ "${lines[1]}" = "dot-ok" ]
  [ "${lines[2]}" = "slash-ok" ]
  [ "${lines[3]}" = "neg-ok" ]
}

@test "a double star is just a star, except in zsh where it recurses" {
  expand 'show */*/y.c'
  [ "$status" -eq 0 ]
  [ "$output" = "1 args: <dir/sub/y.c>" ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      expand 'show **/y.c'
      [ "$status" -eq 0 ]
      [ "$output" = "1 args: <dir/sub/y.c>" ]
      ;;
    bash | yash | busybox | posh | mksh | ksh93 | dash)
      unchanged '**/y.c'
      ;;
    *) false ;;
  esac
}

@test "a matched name that starts with a dash is read as an option by the command" {
  mkdir "$BATS_TEST_TMPDIR/dash"
  expand_in "$BATS_TEST_TMPDIR/dash" 'printf "line\n" >./-n; printf "line\n" >f; cat *; echo ---; cat ./*'
  [ "$status" -eq 0 ]
  # %6d line number, then a tab. The same bytes from BSD cat, GNU cat, BusyBox cat, and mksh's builtin.
  numbered=$(printf '     1\tline\n---\nline\nline')
  [ "$output" = "$numbered" ]
}

@test "a loop over a pattern with no match runs once on the literal pattern, and an existence test guards it" {
  mkdir "$BATS_TEST_TMPDIR/empty"
  expand_in "$BATS_TEST_TMPDIR/empty" 'for f in *.c; do printf "<%s>\n" "$f"; done'
  case $(lab_shell_family) in
    zsh)
      [ "$status" -eq 1 ]
      [ "$output" = "(eval):1: no matches found: *.c" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      [ "$output" = "<*.c>" ]
      expand_in "$BATS_TEST_TMPDIR/empty" 'for f in *.c; do [ -e "$f" ] || continue; echo body; done; echo end'
      [ "$status" -eq 0 ]
      [ "$output" = "end" ]
      ;;
    *) false ;;
  esac
}

@test "a pattern with several matches makes test fail with an error status, except in ksh93" {
  expand '[ -f *.c ]; echo "rc=$?"'
  [ "$status" -eq 0 ]
  got=$output
  script=$LAB/expand.sh
  case $(lab_shell_family) in
    ksh93)
      [ "$got" = "rc=0" ]
      ;;
    bash)
      [ "$got" = "$script: line ${EVAL_LINE}: [: too many arguments"$'\n'"rc=2" ]
      ;;
    dash)
      [ "$got" = "$script: 1: [: a.c: unexpected operator"$'\n'"rc=2" ]
      ;;
    zsh)
      [ "$got" = "(eval):[:1: too many arguments"$'\n'"rc=2" ]
      ;;
    zsh-sh)
      [ "$got" = "$script:[:${EVAL_LINE}: too many arguments"$'\n'"rc=2" ]
      ;;
    yash)
      [ "$got" = "[: \`a.c' is not a valid operator"$'\n'"rc=2" ]
      ;;
    busybox)
      run_lab_c 'printf "%s\n" "$0"'
      [ "$status" -eq 0 ]
      [ "$got" = "$output: b.c: unknown operand"$'\n'"rc=2" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          [ "$got" = "$script: [: b.c: unexpected operator/operand"$'\n'"rc=2" ]
          ;;
        *2025/04/26*)
          [ "$got" = "E: $script: [: b.c: unexpected operator/operand"$'\n'"rc=2" ]
          ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$got" = "$script: [: b.c: unexpected operator/operand"$'\n'"rc=2" ]
      ;;
    *) false ;;
  esac
}

@test "an unreadable directory is not an error: the pattern is left unchanged, except native zsh" {
  PERM_LOCKED=$BATS_TEST_TMPDIR/perm/locked
  mkdir "$BATS_TEST_TMPDIR/perm" "$PERM_LOCKED"
  : >"$PERM_LOCKED/f"
  chmod 000 "$PERM_LOCKED"
  if ls "$BATS_TEST_TMPDIR/perm/locked" >/dev/null 2>&1; then
    # root reads any directory, so the permission rule cannot be shown
    expand_in "$BATS_TEST_TMPDIR/perm" 'show locked/*'
    chmod 755 "$PERM_LOCKED"
    [ "$status" -eq 0 ]
    [ "$output" = "1 args: <locked/f>" ]
  else
    unchanged_in "$BATS_TEST_TMPDIR/perm" 'locked/*'
    chmod 755 "$PERM_LOCKED"
  fi
}

@test "shells spell the empty-result option differently, and only some have one" {
  case $(lab_shell_family) in
    bash)
      expand 'shopt -s nullglob; show *.zzz'
      [ "$output" = "0 args:" ]
      expand 'shopt -s failglob; show *.zzz'
      [ "$status" -eq 1 ]
      [ "$output" = "$LAB/expand.sh: line ${EVAL_LINE}: no match: *.zzz" ]
      expand 'shopt -s dotglob; show *'
      [ "$output" = "13 args: <-n> <.hidden> <B1> <[x]> <^c> <a.c> <a1> <a[b> <b.c> <b.h> <dangling> <dir> <sp ace.c>" ]
      expand 'GLOBIGNORE="*.c"; show *'
      [ "$output" = "10 args: <-n> <.hidden> <B1> <[x]> <^c> <a1> <a[b> <b.h> <dangling> <dir>" ]
      expand 'shopt -s nocaseglob; show A*'
      [ "$output" = "3 args: <a.c> <a1> <a[b>" ]
      ;;
    zsh | zsh-sh)
      expand 'setopt nullglob; show *.zzz'
      [ "$output" = "0 args:" ]
      ;;
    ksh93)
      expand 'show ~(N)*.zzz'
      [ "$output" = "0 args:" ]
      ;;
    dash | busybox | yash | posh | mksh)
      run_lab_c 'command -v shopt >/dev/null 2>&1 || exit 1'
      [ "$status" -ne 0 ]
      run_lab_c 'command -v setopt >/dev/null 2>&1 || exit 1'
      [ "$status" -ne 0 ]
      unchanged '*.zzz'
      ;;
    *) false ;;
  esac
}

@test "a pattern expands before exec, so a large directory overflows the argument limit of an external command" {
  mkdir "$BATS_TEST_TMPDIR/many"
  run_lab "$LAB/many-files.sh" "$BATS_TEST_TMPDIR/many" 13000
  [ "$status" -eq 0 ]
  # the shell itself expands all 13000 names, and a builtin needs no exec (printf is not a builtin in every shell)
  expand_in "$BATS_TEST_TMPDIR/many" 'set -- *; echo "$#"'
  [ "$status" -eq 0 ]
  [ $((output)) -eq 13000 ]
  # an external command receives them all as one argument vector, and exec refuses
  expand_in "$BATS_TEST_TMPDIR/many" 'rc=0; /bin/echo * >/dev/null || rc=$?; echo "rc=$rc"'
  [ "$status" -eq 0 ]
  got=$output
  script=$LAB/expand.sh
  case $(lab_shell_family) in
    bash)
      [ "$got" = "$script: line ${EVAL_LINE}: /bin/echo: Argument list too long"$'\n'"rc=126" ]
      ;;
    dash)
      [ "$got" = "$script: 1: eval: /bin/echo: Argument list too long"$'\n'"rc=126" ]
      ;;
    zsh)
      [ "$got" = "(eval):1: argument list too long: /bin/echo"$'\n'"rc=127" ]
      ;;
    zsh-sh)
      [ "$got" = "$script:${EVAL_LINE}: argument list too long: /bin/echo"$'\n'"rc=127" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$got" = "${script}[${EVAL_LINE}]: eval[1]: /bin/echo: /bin/echo: cannot execute [Argument list too long]"$'\n'"rc=126" ]
          ;;
        *93u+*)
          [ "$got" = "${script}[${EVAL_LINE}]: eval: line 1: /bin/echo: cannot execute [Argument list too long]"$'\n'"rc=126" ]
          ;;
        *) false ;;
      esac
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          [ "$got" = "$script: /bin/echo: Argument list too long"$'\n'"rc=126" ]
          ;;
        *2025/04/26*)
          [ "$got" = "E: $script: /bin/echo: Argument list too long"$'\n'"rc=126" ]
          ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$got" = "$script: /bin/echo: Argument list too long"$'\n'"rc=1" ]
      ;;
    yash)
      [ "$got" = "eval: cannot execute command \`/bin/echo': Argument list too long"$'\n'"rc=126" ]
      ;;
    busybox)
      [ "$got" = "$script: eval: line ${EVAL_LINE}: /bin/echo: Argument list too long"$'\n'"rc=126" ]
      ;;
    *) false ;;
  esac
  # xargs splits the same names into batches that each fit, so none is lost
  expand_in "$BATS_TEST_TMPDIR/many" 'for f in *; do echo "$f"; done | xargs /bin/echo | wc -w'
  [ "$status" -eq 0 ]
  [ $((output)) -eq 13000 ]
}
