#!/usr/bin/env bats
# Tests for command substitution: POSIX.1-2024 XCU 2.6.3, checked against the shell under test.
# Code under test goes through run_lab_c as $1 of 'sh -c': the lab prints, the test compares.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/cs-probe.sh"
  SHOWARGS="$BATS_TEST_DIRNAME/../common/showargs.sh"
}

# --- What the substitution produces ---

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "every trailing newline is removed, interior newlines and other whitespace are kept" {
  run_lab_c 'tab=$(printf "\t")
    x=$(printf "a \t\n\nb \t\n\n\n")
    printf "%d\n" "${#x}"
    [ "${x#"${x%?}"}" = "$tab" ] && echo ends-with-tab
    y=$(printf "\n\n\n"); printf "%d\n" "${#y}"'
  [ "$status" -eq 0 ]
  # a, space, tab, newline, newline, b, space, tab = 8 bytes; only the last three newlines go
  [ "${lines[0]}" = 8 ]
  [ "${lines[1]}" = ends-with-tab ]
  [ "${lines[2]}" = 0 ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "the sentinel idiom keeps the trailing newlines that the plain form loses" {
  run_lab_c 'plain=$(printf "a\n\n"); kept=$(printf "a\n\n"; echo .); kept=${kept%.}
    printf "%d %d\n" "${#plain}" "${#kept}"'
  [ "$status" -eq 0 ]
  [ "$output" = "1 3" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a quoted substitution is one field, an unquoted one is split, and empty results differ" {
  run_lab_c 'sa=$1
    "$sa" "$(printf "a  b\nc")"; "$sa" $(printf "a  b\nc")
    "$sa" $(printf ""); "$sa" "$(printf "")"' x "$SHOWARGS"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1 args: <a  b" ]
  [ "${lines[1]}" = "c>" ]
  [ "${lines[2]}" = "3 args: <a> <b> <c>" ]
  # an unquoted empty result contributes no field at all; a quoted one contributes one empty field
  [ "${lines[3]}" = "0 args:" ]
  [ "${lines[4]}" = "1 args: <>" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "an unquoted result splits on the current IFS, here newline only" {
  run_lab_c 'sa=$1; nl=$(printf "\n_"); IFS=${nl%_}; "$sa" $(printf "a  b\nc d")' x "$SHOWARGS"
  [ "$status" -eq 0 ]
  [ "$output" = "2 args: <a  b> <c d>" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "the result is not expanded again, and is globbed only when unquoted and not assigned" {
  cd "$BATS_TEST_TMPDIR"
  : >f1.txt
  : >f2.txt
  run_lab_c 'sa=$1
    v=$(printf "%s" "\$HOME ~ \$(echo hi) \$((1+1)) \${x}"); printf "%s\n" "$v"
    "$sa" $(echo "*.txt"); "$sa" "$(echo "*.txt")"; x=$(echo "*.txt"); "$sa" "$x"' x "$SHOWARGS"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = '$HOME ~ $(echo hi) $((1+1)) ${x}' ]
  case $(lab_shell_family) in
    zsh) [ "${lines[1]}" = "1 args: <*.txt>" ] ;; # native zsh needs GLOB_SUBST to glob a result
    bash | zsh-sh | busybox | dash | mksh | yash | ksh93 | posh) [ "${lines[1]}" = "2 args: <f1.txt> <f2.txt>" ] ;;
    *) false ;;
  esac
  [ "${lines[2]}" = "1 args: <*.txt>" ]
  [ "${lines[3]}" = "1 args: <*.txt>" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "for f in \$(ls) splits a file name that contains a space into two words" {
  cd "$BATS_TEST_TMPDIR"
  : >"a b"
  : >c
  run_lab_c 'n=0; for f in $(ls); do n=$((n + 1)); done; echo "words=$n"
    n=0; for f in *; do n=$((n + 1)); done; echo "glob=$n"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "words=3" ]
  [ "${lines[1]}" = "glob=2" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a substitution nests to depth 40 and sees the status of the command before it" {
  run_lab_c 's="echo deep"; i=0
    while [ "$i" -lt 40 ]; do s="echo \$($s)"; i=$((i + 1)); done
    eval "r=\$($s)"; echo "$r"
    false; echo "$(echo $?)"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = deep ]
  [ "${lines[1]}" = 1 ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "the substitution reads the shell's stdin and writes a pipe, while stderr still reaches the shell's stderr" {
  cd "$BATS_TEST_TMPDIR"
  run_lab_c 'x=$(cat); printf "in=%s\n" "$x"
    y=$(test -p /dev/stdout && echo pipe || echo other); echo "stdout=$y"
    { z=$(echo e >&2; echo o); } 2>err.txt; printf "z=%s err=%s\n" "$z" "$(cat err.txt)"
    v=$( (echo e >&2; echo o) 2>&1); printf "v=%s\n" "$(printf %s "$v" | tr "\n" "+")"' <<<"piped"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "in=piped" ]
  [ "${lines[1]}" = "stdout=pipe" ]
  [ "${lines[2]}" = "z=o err=e" ]
  [ "${lines[3]}" = "v=e+o" ]
}

# --- Status ---

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "an assignment or a bare substitution reports the substitution status, a command argument hides it" {
  run_lab_c 'x=$(exit 3); echo "assign=$?"
    $(exit 4); echo "bare=$?"
    x=$(exit 5) y=$(exit 6); echo "last=$?"
    echo $(exit 7) >/dev/null; echo "arg=$?"
    x=$(exit 8); y=ok; echo "after=$?"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "assign=3" ]
  [ "${lines[1]}" = "bare=4" ]
  [ "${lines[2]}" = "last=6" ]
  [ "${lines[3]}" = "arg=0" ]
  [ "${lines[4]}" = "after=0" ]
}

# POSIX.1-2024 XCU export APPLICATION USAGE: "export X=$(false)" leaves $? at 0.
# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "export and readonly with an assignment mask the substitution status, a separate assignment does not" {
  run_lab_c 'export a=$(exit 3); echo "export_masked=$?"
    b=$(exit 3); echo "split=$?"
    readonly c=$(exit 3); echo "readonly_masked=$?"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "export_masked=0" ]
  [ "${lines[1]}" = "split=3" ]
  [ "${lines[2]}" = "readonly_masked=0" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "local with an assignment masks the substitution status, and ksh93 spells it typeset" {
  case $(lab_shell_family) in
    ksh93) decl=typeset ;;
    bash | zsh | zsh-sh | busybox | dash | mksh | yash | posh) decl=local ;;
    *) false ;;
  esac
  run_lab_c 'f() { '"$decl"' v=$(exit 3); echo "masked=$?"; }; f
    g() { '"$decl"' w; w=$(exit 3); echo "split=$?"; }; g'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "masked=0" ]
  [ "${lines[1]}" = "split=3" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "set -e exits on a failed assignment from a substitution but not on one in a command argument" {
  run_lab_c 'set -e; x=$(false); echo notreached'
  [ "$status" -ne 0 ]
  [ -z "$output" ]
  run_lab_c 'set -e; echo "$(false)"; echo reached'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf "\\nreached")" ]
}

# POSIX.1-2024 XCU 2.13: the subshell is a duplicate of the environment, which includes options.
# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "whether set -e is in force inside the substitution depends on the shell" {
  # if -e is in force in the substitution, false ends it with status 1 and the assignment ends the shell
  code='set -e; x=$(false; echo after); printf "[%s]\\n" "$x"'
  run_lab_c "$code"
  case $(lab_shell_family) in
    bash)
      # bash resets -e in the substitution outside POSIX mode; inherit_errexit (4.4+) and POSIX mode keep it
      run_lab_c 'case :$SHELLOPTS: in *:posix:*) echo posix ;; esac'
      if [ "$output" = posix ]; then
        run_lab_c "$code"
        [ "$status" -eq 1 ]
        [ -z "$output" ]
        return 0
      fi
      run_lab_c "$code"
      [ "$status" -eq 0 ]
      [ "$output" = "[after]" ]
      run_lab_c "set -o posix; $code"
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      case $(lab_shell_version) in
        3.2*) ;;
        *)
          run_lab_c "shopt -s inherit_errexit; $code"
          [ "$status" -eq 1 ]
          [ -z "$output" ]
          ;;
      esac
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "[after]" ]
      ;;
    zsh | zsh-sh | yash | posh | mksh | ksh93 | dash)
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    *) false ;;
  esac
}

# --- The subshell ---

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "the substitution runs in a subshell: variables, directory, options and functions do not leak" {
  cd "$BATS_TEST_TMPDIR"
  mkdir sub
  run_lab_c 'x=1; f() { echo outer; }
    y=$(x=2; cd sub; set -f; f() { echo inner; }; echo "$x $(basename "$(pwd)") $(f)")
    echo "$y"
    echo "$x $(basename "$(pwd)") $(f)"
    case $- in *f*) echo noglob-leaked ;; *) echo glob-ok ;; esac'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "2 sub inner" ]
  [ "${lines[1]}" = "1 $(basename "$BATS_TEST_TMPDIR") outer" ]
  [ "${lines[2]}" = glob-ok ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "exit inside the substitution ends only the substitution" {
  run_lab_c 'x=$(echo before; exit 3); echo "after $? [$x]"'
  [ "$status" -eq 0 ]
  [ "$output" = "after 3 [before]" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "the substitution is a separate process, except in ksh93 where it can run in the same one" {
  run_lab_c 'p=$(/bin/sh -c "echo \$PPID"; true); if [ "$p" = "$$" ]; then echo same; else echo different; fi'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93) [ "$output" = same ] ;;
    bash | zsh | zsh-sh | busybox | dash | mksh | yash | posh) [ "$output" = different ] ;;
    *) false ;;
  esac
}

# --- Backquotes and parsing ---

# The backquote form halves backslashes before the inner command sees them; $(...) does not.
# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "inside backquotes a backslash pair is halved first, and nesting needs escaped backquotes" {
  run_lab_c 'eval '\''a=`printf "%s" "a\\\\b"`'\''; printf "bq:%s\n" "$a"
    eval '\''a=`printf "%s" "a\\\\\\\\b"`'\''; printf "bq2:%s\n" "$a"
    a=$(printf "%s" "a\\\\b"); printf "dp:%s\n" "$a"
    eval '\''a=`echo \`echo nested\``'\''; printf "nest:%s\n" "$a"'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = 'bq:a\b' ]
  [ "${lines[1]}" = 'bq2:a\\b' ]
  [ "${lines[2]}" = 'dp:a\\b' ]
  [ "${lines[3]}" = "nest:nested" ]
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a case pattern without a leading parenthesis parses inside the substitution, except bash 3.2 and posh" {
  run_lab_c 'x=$(case x in x) echo hi;; esac); echo "[$x]"
    y=$(case x in (x) echo hi;; esac); echo "[$y]"'
  fam=$(lab_shell_family)
  ver=$(lab_shell_version)
  case $fam:$ver in
    posh:* | bash:3.2*)
      [ "$status" -ne 0 ]
      [[ $output != *"[hi]"* ]]
      ;;
    bash:* | zsh:* | zsh-sh:* | busybox:* | dash:* | mksh:* | yash:* | ksh93:*)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "[hi]" ]
      [ "${lines[1]}" = "[hi]" ]
      ;;
    *) false ;;
  esac
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a parenthesis in a comment inside the substitution is a comment, except in posh" {
  run_lab_c 'x=$(echo a # )
echo b
); printf "[%s]\n" "$x" | tr "\n" "+"; echo'
  case $(lab_shell_family) in
    posh) [ "$status" -ne 0 ] ;;
    bash | zsh | zsh-sh | busybox | dash | mksh | yash | ksh93)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "[a+b]+" ]
      ;;
    *) false ;;
  esac
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a here-document body that contains a parenthesis parses inside the substitution, except bash 3.2 and posh" {
  run_lab_c 'x=$(cat <<EOT
a)b
EOT
); printf "[%s]\n" "$x"'
  fam=$(lab_shell_family)
  ver=$(lab_shell_version)
  case $fam:$ver in
    posh:* | bash:3.2*)
      [ "$status" -ne 0 ]
      [[ $output != *"[a)b]"* ]]
      ;;
    bash:* | zsh:* | zsh-sh:* | busybox:* | dash:* | mksh:* | yash:* | ksh93:*)
      [ "$status" -eq 0 ]
      [ "$output" = "[a)b]" ]
      ;;
    *) false ;;
  esac
}

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "\$((cmd);cmd) is an arithmetic expansion in dash and BusyBox ash, a subshell in the rest; a space fixes it" {
  run_lab_c 'x=$( (echo a); echo b); printf "[%s]\n" "$x" | tr "\n" "+"; echo'
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "[a+b]+" ]
  run_lab_c 'x=$((echo a); echo b); printf "[%s]\n" "$x" | tr "\n" "+"; echo'
  case $(lab_shell_family) in
    dash | busybox) [ "$status" -ne 0 ] ;;
    bash | zsh | zsh-sh | mksh | yash | ksh93 | posh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "[a+b]+" ]
      ;;
    *) false ;;
  esac
}

# --- Extensions and unspecified behavior ---

# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "\$(<file) reads the file in bash, zsh, mksh and ksh93 and is an empty command elsewhere" {
  printf 'hello\n\n' >"$BATS_TEST_TMPDIR/data"
  run_lab_c 'x=$(<"$1"); printf "[%s]\n" "$x"' x "$BATS_TEST_TMPDIR/data"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | mksh | ksh93) [ "$output" = "[hello]" ] ;;
    busybox | dash | yash | posh) [ "$output" = "[]" ] ;;
    *) false ;;
  esac
}

# POSIX.1-2024 XCU 2.6.3: with a null byte in the output the behavior is unspecified.
# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a null byte in the output is dropped, kept, or ends the value, depending on the shell" {
  run_lab_c '{ x=$(printf "a\0b"); } 2>/dev/null; printf "%d [%s]\n" "${#x}" "${x%b}"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | busybox | dash | mksh | posh) [ "$output" = "2 [a]" ] ;; # null removed: value is ab
    zsh | zsh-sh) [ "${output%% *}" = 3 ] ;;                        # null kept
    ksh93 | yash) [ "$output" = "1 [a]" ] ;;                        # value ends at the null
    *) false ;;
  esac
}

# POSIX.1-2024 XCU 2.6.3: a command string of only redirections gives unspecified results.
# shellcheck disable=SC2016 # the expansions belong to the shell under test
@test "a substitution that holds only a redirection is an empty result, except in mksh where it is an error" {
  run_lab_c 'x=$(>/dev/null); echo "[$x] $?"' </dev/null
  case $(lab_shell_family) in
    mksh) [ "$status" -ne 0 ] || [[ $output == *"funny"* ]] ;;
    bash | zsh | zsh-sh | busybox | dash | yash | ksh93 | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "[] 0" ]
      ;;
    *) false ;;
  esac
}

# --- Process behavior that needs the probe ---

@test "the substitution waits for end of file, so a background job that holds the pipe delays it" {
  d="$BATS_TEST_TMPDIR/hold"
  mkdir "$d"
  run_lab "$PROBE" holder "$d" hold
  [ "$status" -eq 0 ]
  # the one-second job had already written its marker when the assignment returned
  [ "$output" = "x=hi late=yes" ]
}

@test "redirecting the background job's output releases the substitution at once, except in yash and ksh93u+m" {
  d="$BATS_TEST_TMPDIR/release"
  mkdir "$d"
  run_lab "$PROBE" holder "$d" release
  [ "$status" -eq 0 ]
  fam=$(lab_shell_family)
  ver=$(lab_shell_version)
  case $fam:$ver in
    yash:* | ksh93:*93u+m*)
      # yash keeps a saved copy of the pipe in the job's own process (exec.c, exec_simple_command_with_words);
      # ksh93u+m waits for the last process the substitution forked (subshell.c, sh_subshell, job_wait)
      [ "$output" = "x=hi late=yes" ]
      ;;
    bash:* | zsh:* | zsh-sh:* | busybox:* | dash:* | mksh:* | posh:* | ksh93:*)
      # the marker was still absent when the assignment returned: the job writes it only after the probe
      # releases it, or after a timeout of at least 3 s
      [ "$output" = "x=hi late=no" ]
      ;;
    *) false ;;
  esac
}

@test "a 4 MiB result is held in full by the shell but cannot be passed to an external command" {
  run_lab "$PROBE" big 4194304
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "chars=4194304 heredoc_bytes=4194304" ]
  [ "${lines[1]}" = "external=fail" ]
}

@test "the probe rejects an unknown mode" {
  run_lab "$PROBE" nonsense
  [ "$status" -eq 2 ]
}
