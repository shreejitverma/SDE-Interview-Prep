#!/usr/bin/env bats
# Tests for 03-simple-commands-and-assignment-prefixes.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/simple-cmd-probe.sh"
  SHOWARGS="$BATS_TEST_DIRNAME/../common/showargs.sh"
}

@test "order and dump-env modes: arguments expand before prefix assignments and suffix words stay arguments" {
  run_lab "$PROBE" dump-env alpha beta
  [ "$status" -eq 0 ]
  [ "$output" = "arg1=alpha arg2=beta child_env= " ]

  run_lab "$PROBE" order
  [ "$status" -eq 0 ]
  [ "$output" = "arg1=before arg2=trailing=word child_env=after parent_after=before" ]
}

@test "no-cmd-status mode: bare assignments take the last command substitution status while export masks it" {
  run_lab "$PROBE" no-cmd-status
  [ "$status" -eq 0 ]
  [ "$output" = "last_zero=0 last_seven=7 export_masked=0 a= b= c=" ]
}

@test "scope mode: assignment persistence across bare words, empty command words, built-ins, and functions" {
  run_lab "$PROBE" scope
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    dash | busybox | mksh | posh | yash)
      [ "$output" = "bare=bare empty_cmd=leaked regular=before special=during func=before" ]
      ;;
    ksh93 | zsh-sh)
      [ "$output" = "bare=bare empty_cmd=leaked regular=before special=during func=during" ]
      ;;
    zsh)
      [ "$output" = "bare=bare empty_cmd=leaked regular=before special=before func=before" ]
      ;;
    bash)
      ver=$(lab_shell_version)
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        case ${ver} in
          3.* | 4.* | 5.0*)
            [ "$output" = "bare=bare empty_cmd=leaked regular=before special=during func=during" ]
            ;;
          *)
            [ "$output" = "bare=bare empty_cmd=leaked regular=before special=during func=before" ]
            ;;
        esac
      else
        [ "$output" = "bare=bare empty_cmd=leaked regular=before special=before func=before" ]
      fi
      ;;
    *)
      false
      ;;
  esac
}

@test "decl-split mode: export suppresses field splitting only on unquoted valid assignment operands" {
  run_lab "$PROBE" decl-split
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    posh)
      [ "$output" = "decl_a=1 decl_b=2 quoted_q=1 quoted_b=2" ]
      ;;
    zsh)
      [ "$output" = "decl_a=1 b=2 decl_b=keep quoted_q=1 b=2 quoted_b=keep" ]
      ;;
    bash | dash | busybox | ksh93 | mksh | yash | zsh-sh)
      [ "$output" = "decl_a=1 b=2 decl_b=keep quoted_q=1 quoted_b=2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "invalid probe mode returns usage exit status 2" {
  run_lab "$PROBE" bad-mode
  [ "$status" -eq 2 ]
}

@test "lexical rule 7b: an expanded word containing equals is treated as a command name, not an assignment" {
  # shellcheck disable=SC2016 # expansion belongs to the shell under test
  run_lab_c '(w="x=1"; $w /usr/bin/true) 2>/dev/null; printf "rc=%d\n" "$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "rc=127" ]
}

@test "assignment prefix context: tilde expands after equals and colon without field splitting or globbing" {
  : >"$BATS_TEST_TMPDIR/one"
  : >"$BATS_TEST_TMPDIR/two"
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    cd "$1" || exit 1
    IFS=:
    v="a:b:*"
    x=~:~/bin:$v "$2" dump-env "ok" "done"
  ' sh "$BATS_TEST_TMPDIR" "$PROBE"
  [ "$status" -eq 0 ]
  [ "$output" = "arg1=ok arg2=done child_env=$HOME:$HOME/bin:a:b:* " ]
}

@test "prefix PATH assignment directs command search for the utility without mutating caller PATH" {
  bindir="$BATS_TEST_TMPDIR/custom_bin"
  mkdir -p "$bindir"
  cat >"$bindir/probe_util" <<'EOF'
#!/bin/sh
printf 'util_path=%s' "$PATH"
EOF
  chmod +x "$bindir/probe_util"
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    orig=$PATH
    PATH=$1 probe_util
    printf " caller_same=%d\n" "$([ "$PATH" = "$orig" ] && echo 1 || echo 0)"
  ' sh "$bindir"
  [ "$status" -eq 0 ]
  [ "$output" = "util_path=$bindir caller_same=1" ]
}

@test "multiple prefix assignments: second prefix sees first prefix except in mksh and bash 3.2" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'a=old; a=first x=$a "$1" dump-env "ok" "done"' sh "$PROBE"
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    mksh)
      [ "$output" = "arg1=ok arg2=done child_env=old " ]
      ;;
    bash)
      case $(lab_shell_version) in
        3.*)
          [ "$output" = "arg1=ok arg2=done child_env=old " ]
          ;;
        *)
          [ "$output" = "arg1=ok arg2=done child_env=first " ]
          ;;
      esac
      ;;
    dash | busybox | ksh93 | posh | yash | zsh | zsh-sh)
      [ "$output" = "arg1=ok arg2=done child_env=first " ]
      ;;
    *)
      false
      ;;
  esac
}

@test "side effects inside prefix assignments on external utilities persist unless the shell forks first" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    x=old
    a=$((x=1)) "$1" "ok" >/dev/null
    printf "%s\n" "$x"
  ' sh "$SHOWARGS"
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    zsh | zsh-sh)
      [ "$output" = "old" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$output" = "1" ]
          ;;
        *)
          [ "$output" = "old" ]
          ;;
      esac
      ;;
    bash | dash | busybox | mksh | posh | yash)
      [ "$output" = "1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "a 1.1 MB assignment stays in shell heap for a bare command but fails execve with E2BIG on external exec" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    big=$(head -c 1100000 /dev/zero | tr "\0" a)
    x=$big
    printf "len=%d " "${#x}"
    unset x
    x=$big "$1" >/dev/null 2>&1
    printf "ext_rc=%d\n" "$?"
  ' sh "$SHOWARGS"
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    zsh | zsh-sh)
      [ "$output" = "len=1100000 ext_rc=127" ]
      ;;
    posh)
      [ "$output" = "len=1100000 ext_rc=1" ]
      ;;
    bash | dash | busybox | ksh93 | mksh | yash)
      [ "$output" = "len=1100000 ext_rc=126" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command with no command name: bash, ksh93, and yash assign before redirection expansion while dash, ash, mksh, posh, and zsh expand redirections first" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c '
    x=old
    x=new >"$1/$x"
    if [ -e "$1/new" ]; then
      printf "target=new\n"
    elif [ -e "$1/old" ]; then
      printf "target=old\n"
    else
      printf "target=none\n"
    fi
  ' sh "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    bash | ksh93 | yash)
      [ "$output" = "target=new" ]
      ;;
    dash | busybox | mksh | posh | zsh | zsh-sh)
      [ "$output" = "target=old" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "prefix on a special built-in exports the name to later commands only in posix-mode bash and yash" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'unset u; u=1 :; printf "exported=%s\n" "$(env | grep -c "^u=1$")"'
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  case ${fam} in
    yash)
      [ "$output" = "exported=1" ]
      ;;
    bash)
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        [ "$output" = "exported=1" ]
      else
        [ "$output" = "exported=0" ]
      fi
      ;;
    dash | busybox | ksh93 | mksh | posh | zsh | zsh-sh)
      [ "$output" = "exported=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "readonly prefix on an external utility: the shell runs it, skips it and continues, or exits, by family" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'exec 2>/dev/null; readonly r=1; r=2 /bin/echo hi; printf "continued rc=%d\n" "$?"'
  fam=$(lab_shell_family)
  ran=$'hi\ncontinued rc=0'
  case ${fam} in
    bash)
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        case $(lab_shell_version) in
          3.* | 4.* | 5.0* | 5.1*)
            [ "$status" -eq 0 ]
            [ "$output" = "${ran}" ]
            ;;
          *)
            [ "$status" -eq 1 ]
            [ -z "$output" ]
            ;;
        esac
      else
        [ "$status" -eq 0 ]
        [ "$output" = "${ran}" ]
      fi
      ;;
    ksh93 | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "continued rc=1" ]
      ;;
    dash | busybox | mksh | yash)
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      ;;
    posh)
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "prefix on a function keyword definition: ksh93 reverts it, unlike its persisting POSIX-syntax functions" {
  # shellcheck disable=SC2016 # expansions belong to the shell under test
  run_lab_c 'function fn { :; }; f=before; f=during fn; printf "func=%s\n" "$f"'
  fam=$(lab_shell_family)
  case ${fam} in
    dash)
      [ "$status" -eq 2 ]
      [[ $output == *"Syntax error"* ]]
      ;;
    posh)
      [ "$status" -eq 1 ]
      [[ $output == *"syntax error"* ]]
      ;;
    zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "func=during" ]
      ;;
    busybox | ksh93 | mksh | yash | zsh)
      [ "$status" -eq 0 ]
      [ "$output" = "func=before" ]
      ;;
    bash)
      [ "$status" -eq 0 ]
      if "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null; then
        case $(lab_shell_version) in
          3.* | 4.* | 5.0*)
            [ "$output" = "func=during" ]
            ;;
          *)
            [ "$output" = "func=before" ]
            ;;
        esac
      else
        [ "$output" = "func=before" ]
      fi
      ;;
    *)
      false
      ;;
  esac
}
