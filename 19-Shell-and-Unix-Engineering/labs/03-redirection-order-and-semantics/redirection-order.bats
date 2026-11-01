#!/usr/bin/env bats
# Redirection order and semantics (POSIX.1-2024 XCU 2.7, 2.8.1, 2.9.1).
# Every test runs the shell under test through run_lab or run_lab_c; emit.sh is a real script, so
# the shell must fork and exec it with the descriptors the redirections left behind.

# shellcheck disable=SC2016 # every single-quoted string below is code for the shell under test

setup() {
  load ../lib/lab
  D=$BATS_TEST_TMPDIR
  EMIT=$BATS_TEST_DIRNAME/emit.sh
  ORDER=$BATS_TEST_DIRNAME/order.sh
}

# bash_posix_mode: succeed when the shell under test has bash's posix option on (for example bash
# started as sh); it does not touch $status or $output.
bash_posix_mode() {
  lab_shell_words
  "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null
}

# bash_ambiguous WORD: print the diagnostic bash writes for an ambiguous redirect of WORD in a
# one-line script run as `-c CODE lab` (so argv0 is "lab"). Bash 3.2 and 4.4 omit the line
# number, bash 5.1 and later include "line 1:".
bash_ambiguous() {
  case $(lab_shell_version) in
    3.* | 4.*) printf 'lab: %s: ambiguous redirect' "$1" ;;
    5.*) printf 'lab: line 1: %s: ambiguous redirect' "$1" ;;
    *) return 1 ;;
  esac
}

# ---------------------------------------------------------------------------
# Order of evaluation (2.7: "from beginning to end")
# ---------------------------------------------------------------------------

@test "order.sh prints exactly the report the note documents" {
  run_lab "$ORDER" "$D"
  [ "$status" -eq 0 ]
  # four.open: the second truncating open starts at offset 0 and "err\n" overwrites "out " in "out 0\n"
  [ "$output" = "one=[out 0|err|]
two.file=[out 0|]
two.outer=[err|]
three.stdout=[err|]
three.stderr=[out 0|]
four.dup=[out 0|err|]
four.open=[err|0|]
four.append=[out 0|err|]" ]
}

@test "command substitution idiom: 2>&1 >/dev/null captures stderr, the reverse captures nothing" {
  run_lab_c 'a=$("$1" 2>&1 >/dev/null); b=$("$1" >/dev/null 2>&1); echo "a=[$a] b=[$b]"' lab "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "a=[err] b=[]" ]
}

@test "a failing redirection stops the later ones, keeps the earlier ones, and the command never runs" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    "$2" >a </nonexistent/x >b
    if [ "$?" -ne 0 ]; then echo failed; fi
    if [ -f a ]; then echo a-created; fi
    if [ -f b ]; then echo b-created; fi
    if [ -s a ]; then echo command-ran; fi' lab "$D" "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "failed
a-created" ]
}

@test "the reverse order creates nothing: the failing input redirection comes first" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    "$2" </nonexistent/x >a
    if [ -f a ]; then echo a-created; else echo a-absent; fi' lab "$D" "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "a-absent" ]
}

@test "redirection words are expanded one at a time, so a later word sees an earlier side effect" {
  run_lab_c 'cd "$1" || exit 9; i=0; : >"f$((i=i+1))" 2>"g$i"; ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "f1
g1" ]
}

@test "two output redirections on one descriptor open and truncate both files, the last one wins except zsh MULTIOS" {
  run_lab_c 'cd "$1" || exit 9; echo old >a; echo old >b; echo hi >a >b; echo "a=[$(cat a)] b=[$(cat b)]"' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "$output" = "a=[hi] b=[hi]" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "a=[] b=[hi]" ]
      ;;
    *) false ;;
  esac
}

@test "dash and BusyBox ash expand every redirection word before opening any file, the others expand as they go" {
  run_lab_c 'cd "$1" || exit 9; : >f1 >"$(if [ -e f1 ]; then echo seen; else echo unseen; fi)"; ls' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | busybox)
      [ "$output" = "f1
unseen" ]
      ;;
    bash | zsh | zsh-sh | yash | posh | mksh | ksh93)
      [ "$output" = "f1
seen" ]
      ;;
    *) false ;;
  esac
}

@test "redirections and arguments are separated: the operator and its word are not arguments" {
  run_lab_c 'cd "$1" || exit 9; "$2" a >f b c 2>/dev/null; cat f' lab "$D" "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "out 3" ]
}

@test "the pipeline connects its pipe before the command's own redirections run" {
  run_lab_c 'cd "$1" || exit 9
    "$2" >f 2>/dev/null | cat >g
    "$2" 2>&1 | cat >h
    echo "f=[$(cat f)] g=[$(cat g)]"
    echo "h=[$(tr "\n" "|" <h)]"' lab "$D" "$EMIT"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # native zsh has MULTIOS: the pipe and the file both receive stdout
      [ "$output" = "f=[out 0] g=[out 0]
h=[out 0|err|]" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "f=[out 0] g=[]
h=[out 0|err|]" ]
      ;;
    *) false ;;
  esac
}

@test "2>&1 >/dev/null in a pipeline stage passes only stderr down the pipe, except native zsh also passes stdout" {
  run_lab_c '{ echo out; echo err >&2; } 2>&1 >/dev/null | tr a-z A-Z | sort'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # MULTIOS: stdout is the pipe and /dev/null, so "out" is copied into the pipe
      [ "$output" = "ERR
OUT" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "ERR" ]
      ;;
    *) false ;;
  esac
}

@test "an assignment prefix is not visible to the redirection word of an external command" {
  run_lab_c 'cd "$1" || exit 9; exec 2>/dev/null; v=old; v=new "$2" >"$v"; ls' lab "$D" "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "old" ]
}

@test "for a special built-in the order of assignment and redirection may reverse (ksh93 does)" {
  run_lab_c 'cd "$1" || exit 9; v=old; v=new : >"$v"; ls' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    ksh93) [ "$output" = "new" ] ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash) [ "$output" = "old" ] ;;
    *) false ;;
  esac
}

# ---------------------------------------------------------------------------
# Scope: simple command, compound command, function, exec, no command name
# ---------------------------------------------------------------------------

@test "a redirection on a compound command covers its commands, and an inner one overrides it" {
  run_lab_c 'cd "$1" || exit 9
    { echo a; echo b >other; echo c; } >f
    echo 1 >h; echo 2 >h
    { echo 1; echo 2; } >i
    echo "f=[$(tr "\n" "|" <f)] other=[$(cat other)] h=[$(cat h)] i=[$(tr "\n" "|" <i)]"' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "f=[a|c|] other=[b] h=[2] i=[1|2|]" ]
}

@test "a function's redirection is performed at each call, not at definition" {
  run_lab_c 'cd "$1" || exit 9; v=1; f() { echo x; } >"o$v"; v=2; f; ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "o2" ]
}

@test "exec with only redirections changes the current shell, a plain command does not" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    exec 3>f; echo a >&3; echo b >&3; exec 3>&-
    if echo c >&3; then echo still-open; else echo closed; fi
    echo "f=[$(tr "\n" "|" <f)]"' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "closed
f=[a|b|]" ]
}

@test "a command with no command name performs its redirections in a subshell, so they vanish" {
  run_lab_c 'exec 2>/dev/null; exec 6>&-; cd "$1" || exit 9
    6>f </dev/null
    if echo x >&6; then echo still-open; else echo gone; fi
    ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "gone
f" ]
}

@test "a redirection-only command fails when its redirection fails, and succeeds otherwise" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    x=1 </nonexistent/x
    if [ "$?" -ne 0 ]; then echo failed; fi
    >f </dev/null
    echo "ok=$?"' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "failed
ok=0" ]
}

@test "exec N> leaves descriptor N open in children except in mksh and ksh93" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    exec 3>f; sh -c "echo x >&3"; exec 3>&-
    echo "f=[$(cat f)]"' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    mksh | ksh93) [ "$output" = "f=[]" ] ;;
    bash | zsh | zsh-sh | yash | busybox | posh | dash) [ "$output" = "f=[x]" ] ;;
    *) false ;;
  esac
}

# ---------------------------------------------------------------------------
# Errors (2.8.1)
# ---------------------------------------------------------------------------

@test "a redirection error in a compound command, function, builtin, or utility does not exit the shell" {
  run_lab_c 'exec 2>/dev/null
    { echo in1; } </nonexistent/x
    f() { echo in2; }
    f </nonexistent/x
    echo in3 </nonexistent/x
    "$1" </nonexistent/x
    command : </nonexistent/x
    echo survived' lab "$EMIT"
  [ "$status" -eq 0 ]
  [ "$output" = "survived" ]
}

@test "a failing redirection reports to the stderr in force at that point, except zsh reports first" {
  # Each script is one line so the diagnostic's line number, when a shell prints one, is 1.
  # late must be the shell's own bare diagnostic: the text contains argv0, which differs
  # inside one family, so the expectation is that one string, not a list of wordings.
  run_lab_c 'echo x </nonexistent/x'
  bare_status=$status
  bare=$output
  run_lab_c 'echo x </nonexistent/x 2>/dev/null'
  late_status=$status
  late=$output
  run_lab_c 'echo x 2>/dev/null </nonexistent/x'
  early_status=$status
  early=$output
  [ "$late_status" -eq "$bare_status" ]
  [ "$early_status" -eq "$bare_status" ]
  [ "$late" = "$bare" ]
  case $bare in
    */nonexistent/x*) ;;
    *) false ;;
  esac
  # the command did not run, so no output line is exactly "x"
  if printf '%s\n' "$bare" | grep -Fxq x; then
    false
  fi
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$early" = "$bare" ]
      [ "$bare_status" -eq 1 ]
      ;;
    dash | yash)
      [ "$early" = "" ]
      [ "$bare_status" -eq 2 ]
      ;;
    bash | busybox | posh | mksh | ksh93)
      [ "$early" = "" ]
      [ "$bare_status" -eq 1 ]
      ;;
    *) false ;;
  esac
}

@test "a redirection error on a special built-in exits a non-interactive shell, except bash outside POSIX mode, native zsh, and yash" {
  run_lab_c 'exec 2>/dev/null; : </nonexistent/x; echo survived'
  case $(lab_shell_family) in
    bash)
      if bash_posix_mode; then
        [ "$status" -eq 1 ]
        [ "$output" = "" ]
      else
        [ "$status" -eq 0 ]
        [ "$output" = "survived" ]
      fi
      ;;
    zsh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "" ]
      ;;
    zsh-sh | busybox | posh | mksh | ksh93)
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      ;;
    *) false ;;
  esac
}

@test "exec with a failing redirection follows the special built-in rule" {
  run_lab_c 'exec 2>/dev/null; exec </nonexistent/x; echo survived'
  case $(lab_shell_family) in
    bash)
      if bash_posix_mode; then
        [ "$status" -eq 1 ]
        [ "$output" = "" ]
      else
        [ "$status" -eq 0 ]
        [ "$output" = "survived" ]
      fi
      ;;
    zsh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "survived" ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "" ]
      ;;
    zsh-sh | busybox | posh | mksh | ksh93)
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      ;;
    *) false ;;
  esac
}

@test "duplicating a descriptor that is not open is a redirection error and the command does not run" {
  run_lab_c 'exec 2>/dev/null; exec 9>&-
    if echo x >&9; then echo ran; else echo failed; fi' lab
  [ "$status" -eq 0 ]
  [ "$output" = "failed" ]
}

# ---------------------------------------------------------------------------
# noclobber (2.7.2)
# ---------------------------------------------------------------------------

@test "noclobber refuses an existing regular file; >| and >> still work" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    set -C
    echo 1 >f
    if echo 2 >f; then echo overwritten; else echo refused; fi
    cat f
    echo 3 >>f; cat f
    echo 4 >|f; cat f' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "refused
1
1
3
4" ]
}

@test "noclobber lets output go to a non-regular file such as /dev/null" {
  # The regular file is the control: without set -C the second write would succeed, so a
  # passing test has proved noclobber is on and that /dev/null is still accepted.
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    set -C
    echo 1 >reg
    if echo 2 >reg; then echo regular=overwritten; else echo regular=refused; fi
    echo x >/dev/null
    echo "null=$?"' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "regular=refused
null=0" ]
}

@test "noclobber refuses a dangling symlink instead of creating its target" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9
    set -C; ln -s nothere lnk
    if echo x >lnk; then echo created; else echo refused; fi
    ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "refused
lnk" ]
}

@test "noclobber makes > a lock-file primitive: the first creator wins, the second is refused" {
  run_lab_c 'cd "$1" || exit 9; set -C
    ( : >lock ) 2>/dev/null; first=$?
    ( : >lock ) 2>/dev/null; second=$?
    if [ "$first" -eq 0 ] && [ "$second" -ne 0 ]; then echo first-wins; else echo "first=$first second=$second"; fi' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "first-wins" ]
}

# ---------------------------------------------------------------------------
# The redirection word and operator syntax (2.7)
# ---------------------------------------------------------------------------

@test "a quoted digit is not a descriptor number and a quoted operator is not an operator" {
  run_lab_c 'cd "$1" || exit 9
    echo \2>a
    cat a
    echo 2\>a
    echo a2>b
    cat b
    ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "2
2>a
a2
a
b" ]
}

@test "the word after the operator is not field split, except bash reports it as ambiguous" {
  # One line, so bash 5's "line 1:" is stable. stderr stays visible: rc=1 alone also
  # matches a missing directory, which is not an ambiguous redirect.
  run_lab_c 'cd "$1" || exit 9; v="a b"; echo hi >$v; echo "rc=$?"; ls' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      if bash_posix_mode; then
        [ "$output" = "rc=0
a b" ]
      else
        diag=$(bash_ambiguous '$v') || false
        [ "$output" = "$diag
rc=1" ]
      fi
      ;;
    zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "rc=0
a b" ]
      ;;
    *) false ;;
  esac
}

@test "the word after the operator is not globbed in POSIX mode, but bash and zsh glob it" {
  run_lab_c 'exec 2>/dev/null; cd "$1" || exit 9; touch a1; echo hi > a*; ls; cat a1' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      if bash_posix_mode; then
        [ "$output" = "a*
a1" ]
      else
        [ "$output" = "a1
hi" ]
      fi
      ;;
    zsh)
      [ "$output" = "a1
hi" ]
      ;;
    zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "a*
a1" ]
      ;;
    *) false ;;
  esac
  # Several matches: bash refuses, native zsh tees onto every match, the others
  # create a file whose name is the literal pattern.
  run_lab_c 'cd "$1" || exit 9; mkdir -p m && cd m && touch a1 a2; echo hi > a*; echo "rc=$?"; echo "a1=[$(cat a1)]"; echo "a2=[$(cat a2)]"; if [ -f "a*" ]; then echo star=yes; else echo star=no; fi' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      if bash_posix_mode; then
        [ "$output" = "rc=0
a1=[]
a2=[]
star=yes" ]
      else
        diag=$(bash_ambiguous 'a*') || false
        [ "$output" = "$diag
rc=1
a1=[]
a2=[]
star=no" ]
      fi
      ;;
    zsh)
      [ "$output" = "rc=0
a1=[hi]
a2=[hi]
star=no" ]
      ;;
    zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "$output" = "rc=0
a1=[]
a2=[]
star=yes" ]
      ;;
    *) false ;;
  esac
}

@test "&>file is a bash-style extension: elsewhere it is a background job plus a truncating redirection" {
  run_lab_c 'cd "$1" || exit 9; { echo x &>f; wait; } >o; echo "o=[$(cat o)] f=[$(cat f)]"' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | busybox | mksh)
      [ "$output" = "o=[] f=[x]" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "$output" = "o=[] f=[x]" ] ;;
        *2012-08-01*) [ "$output" = "o=[x] f=[]" ] ;;
        *) false ;;
      esac
      ;;
    yash | posh | dash)
      [ "$output" = "o=[x] f=[]" ]
      ;;
    *) false ;;
  esac
}

@test "a multi-digit descriptor number is one redirection in bash, yash, and BusyBox but an argument elsewhere" {
  run_lab_c 'cd "$1" || exit 9; { echo hi 22>f; } >o; echo "o=[$(cat o)] f=[$(cat f)]"' lab "$D"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | yash | busybox)
      [ "$output" = "o=[hi] f=[]" ]
      ;;
    zsh | zsh-sh | posh | mksh | ksh93 | dash)
      [ "$output" = "o=[] f=[hi 22]" ]
      ;;
    *) false ;;
  esac
}

@test "<> opens for reading and writing without truncating, and creates a missing file" {
  run_lab_c 'cd "$1" || exit 9; printf abcdef >f; printf XY 1<>f; cat f; echo; : <>new; ls' lab "$D"
  [ "$status" -eq 0 ]
  [ "$output" = "XYcdef
f
new" ]
}

@test "{var}> allocates descriptor 10 in bash 4.4+ and ksh93, and 11 in native zsh" {
  # Measured at the top level of the script with only descriptors 0-5 open. ksh93 gives 12 inside
  # a subshell (cause not investigated), so the numbered probe never runs in one.
  expect=absent
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*) ;;
        4.* | 5.*) expect=10 ;;
        *) false ;;
      esac
      ;;
    ksh93) expect=10 ;;
    zsh) expect=11 ;;
    zsh-sh | yash | busybox | posh | mksh | dash) ;;
    *) false ;;
  esac
  if [ "$expect" = absent ]; then
    run_lab_c 'cd "$1" || exit 9
      ( exec {fd}>f; echo hi >&$fd; exec {fd}>&-; echo "fd=${fd:-unset}" ) 2>/dev/null
      echo "f=[$(cat f)]"' lab "$D"
    [ "$status" -eq 0 ]
    [ "$output" = "f=[]" ]
    [ -f "$D/f" ]
    [ ! -s "$D/f" ]
  else
    run_lab_c 'cd "$1" || exit 9
      exec {fd}>f
      echo hi >&$fd
      echo "fd=$fd"
      echo "body=[$(cat f)]"' lab "$D"
    [ "$status" -eq 0 ]
    [ "$output" = "fd=$expect
body=[hi]" ]
  fi
}
