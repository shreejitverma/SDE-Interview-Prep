#!/usr/bin/env bats
# Bats lab matrix for Descriptor Duplication and Closing (POSIX.1-2024 XCU 2.7.5 and 2.7.6).
# Descriptors 3 and 4 are held by bats itself, so every probe here uses 6 to 9.
# shellcheck disable=SC2016 # the code strings below belong to the shell under test

setup_file() {
  "${CC:-cc}" -o "$BATS_FILE_TMPDIR/fdprobe" "$BATS_TEST_DIRNAME/fdprobe.c"
}

setup() {
  load ../lib/lab
  FDPROBE=$BATS_FILE_TMPDIR/fdprobe
  export FDPROBE
  IDIOMS=$BATS_TEST_DIRNAME/fd-idioms.sh
  cd "$BATS_TEST_TMPDIR" || return 1
}

# redir_fail_status: the status a command reports when a redirection fails, measured in the shell matrix: 2 in dash and yash, 1 elsewhere.
redir_fail_status() {
  case $(lab_shell_family) in
    dash | yash) echo 2 ;;
    bash | zsh | zsh-sh | busybox | posh | mksh | ksh93) echo 1 ;;
    *) return 1 ;;
  esac
}

# has_fd N: succeed when the fdprobe line in $output lists descriptor N.
has_fd() {
  case " $output " in
    *" $1 "*) return 0 ;;
    *) return 1 ;;
  esac
}

# lacks_fd N: succeed when the fdprobe line in $output does not list descriptor N (a bare `!` would not fail a bats test).
lacks_fd() {
  if has_fd "$1"; then
    return 1
  fi
  return 0
}

@test "a duplicated descriptor shares the file offset, two separate opens do not" {
  run_lab_c 'exec 6>dup; exec 7>&6; echo a >&6; echo b >&7; exec 6>&- 7>&-; cat dup'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'a\nb')" ]
  run_lab_c 'exec 6>two 7>two; echo aaaa >&6; echo b >&7; exec 6>&- 7>&-; cat two'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'b\naa')" ]
}

@test "reading through a duplicate consumes the shared offset" {
  printf 'l1\nl2\n' >in
  run_lab_c 'exec 6<in; exec 7<&6; read -r a <&6; read -r b <&7; echo "$a|$b"'
  [ "$status" -eq 0 ]
  [ "$output" = "l1|l2" ]
  run_lab_c 'exec 6<in; exec 7<in; read -r a <&6; read -r b <&7; echo "$a|$b"'
  [ "$status" -eq 0 ]
  [ "$output" = "l1|l1" ]
}

@test "n>&m copies what m refers to now, so the order of redirections decides" {
  d=$BATS_TEST_TMPDIR
  run_lab "$IDIOMS" direct "$d"
  [ "$status" -eq 0 ]
  [ "$output" = "stdout=[out,] stderr=[err,] status=3 merged=[] " ]
  run_lab "$IDIOMS" merge-first "$d"
  [ "$status" -eq 0 ]
  [ "$output" = "stdout=[] stderr=[] status=3 merged=[out,err,] " ]
  run_lab "$IDIOMS" merge-last "$d"
  [ "$status" -eq 0 ]
  [ "$output" = "stdout=[err,] stderr=[] status=3 merged=[out,] " ]
}

@test "3>&1 1>&2 2>&3 3>&- exchanges stdout and stderr" {
  run_lab "$IDIOMS" swap "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "stdout=[err,] stderr=[out,] status=3 merged=[] " ]
  run_lab_c '{ "$FDPROBE" 6>&1 1>&2 2>&6; } 2>&1'
  [ "$status" -eq 0 ]
  has_fd 6
  run_lab_c '{ "$FDPROBE" 6>&1 1>&2 2>&6 6>&-; } 2>&1'
  [ "$status" -eq 0 ]
  lacks_fd 6
}

@test "the stderr-filter idiom tags stderr, leaves stdout alone and keeps the exit status" {
  run_lab "$IDIOMS" prefix "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "stdout=[out,] stderr=[E: err,] status=3 merged=[] " ]
}

@test "an unknown mode is a usage error with status 64" {
  run_lab "$IDIOMS" bogus "$BATS_TEST_TMPDIR"
  [ "$status" -eq 64 ]
  [ "$output" = "fd-idioms: unknown mode: bogus" ]
}

@test "a duplicate keeps the open file description alive after the original is closed" {
  run_lab_c 'exec 6>keep; echo a >&6; exec 7>&6; exec 6>&-; { echo z >&6; } 2>/dev/null; echo x >&7; exec 7>&-; cat keep'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'a\nx')" ]
}

@test "after n>&- a write to n fails and nothing reaches the file" {
  run_lab_c 'exec 6>gone; exec 6>&-; { echo x >&6; } 2>/dev/null; echo "st=$?"; cat gone'
  [ "$status" -eq 0 ]
  want=$(redir_fail_status)
  [ "$output" = "st=$want" ]
  [ -e gone ]
  [ ! -s gone ]
}

@test "closing a descriptor that is not open is not an error, and n<&- closes like n>&-" {
  run_lab_c 'exec 9>&-; echo "a=$?"; : 9<&-; echo "b=$?"; exec 8>f; exec 8<&-; exec 8>&-; echo "c=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'a=0\nb=0\nc=0')" ]
  want=$(redir_fail_status)
  run_lab_c 'exec 8>f; exec 8<&-; { echo x >&8; } 2>/dev/null; echo "st=$?"'
  [ "$output" = "st=$want" ]
  run_lab_c 'exec 8>f; exec 8>&-; { echo x >&8; } 2>/dev/null; echo "st=$?"'
  [ "$output" = "st=$want" ]
  run_lab_c 'exec 8>f; exec 8<&-; "$FDPROBE"'
  [ "$status" -eq 0 ]
  lacks_fd 8
  run_lab_c 'exec 8>f; exec 8>&-; "$FDPROBE"'
  [ "$status" -eq 0 ]
  lacks_fd 8
}

@test "a close is not turned into a reopen on /dev/null for the child" {
  run_lab_c '"$FDPROBE" <&-'
  [ "$status" -eq 0 ]
  lacks_fd 0
  has_fd 1
  has_fd 2
  run_lab_c '"$FDPROBE" 2>&-'
  [ "$status" -eq 0 ]
  has_fd 0
  has_fd 1
  lacks_fd 2
  # With stdout closed the probe must report on stderr, which the lab helper captures.
  run_lab_c '"$FDPROBE" -e >&-'
  [ "$status" -eq 0 ]
  has_fd 0
  lacks_fd 1
  has_fd 2
}

@test "a closed stdout fails an external writer, and a built-in writer in every shell but zsh" {
  run_lab_c '/bin/echo x >&- 2>/dev/null; echo "st=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "st=1" ]
  run_lab_c 'echo x >&- 2>/dev/null; echo "st=$?"'
  [ "$status" -eq 0 ]
  last=${output##*st=}
  case $(lab_shell_family) in
    zsh | zsh-sh) [ "$last" = 0 ] ;;
    bash | yash | busybox | posh | mksh | ksh93 | dash) [ "$last" = 1 ] ;;
    *) false ;;
  esac
}

@test "n>&m with an unopened m fails the command; only a special built-in may end the shell" {
  want=$(redir_fail_status)
  run_lab_c '{ echo hi >&9; } 2>/dev/null; echo "after $?"'
  [ "$output" = "after $want" ]
  run_lab_c '{ { :; } >&9; } 2>/dev/null; echo "after $?"'
  [ "$output" = "after $want" ]
  run_lab_c 'f() { :; }; { f >&9; } 2>/dev/null; echo "after $?"'
  [ "$output" = "after $want" ]
}

@test "a compound command probes whether a descriptor is open without risking the shell" {
  run_lab_c 'exec 6>probe; { :; } >&6; echo "open=$?"; { { :; } >&7; } 2>/dev/null; echo "closed=$?"'
  [ "$status" -eq 0 ]
  want=$(redir_fail_status)
  [ "$output" = "$(printf 'open=0\nclosed=%s' "$want")" ]
}

@test "n>&n hands one close-on-exec descriptor to one command in ksh93 and mksh and changes nothing elsewhere" {
  run_lab_c 'exec 8>f; exec 9>&8; "$FDPROBE" 9>&9'
  [ "$status" -eq 0 ]
  has_fd 9
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | dash) has_fd 8 ;;
    mksh | ksh93) lacks_fd 8 ;;
    *) false ;;
  esac
}

@test "a redirection error on a special built-in ends the shell in the conforming shells" {
  case $(lab_shell_family) in
    dash | busybox | mksh | ksh93 | posh | zsh-sh) expect=stops ;;
    zsh | yash) expect=goes_on ;;
    bash)
      run_lab_c '[ -o posix ]'
      if [ "$status" -eq 0 ]; then expect=stops; else expect=goes_on; fi
      ;;
    *) false ;;
  esac
  want=$(redir_fail_status)
  run_lab_c ': >&9; echo "after $?"'
  case $expect in
    stops)
      [ "$status" -eq "$want" ]
      case $output in
        *after*) false ;;
        *) ;;
      esac
      ;;
    goes_on)
      [ "$status" -eq 0 ]
      [ "${output##*$'\n'}" = "after $want" ]
      ;;
    *) false ;;
  esac
}

@test "no shell checks the access mode of the source when it duplicates" {
  run_lab_c 'echo q >ro; exec 6<ro; : >&6; echo "a=$?"; exec 7>wo; : <&7; echo "b=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'a=0\nb=0')" ]
  run_lab_c 'echo q >ro; exec 6<ro; { echo x >&6; } 2>/dev/null; echo "st=$?"'
  [ "$status" -eq 0 ]
  # Apple's bash 3.2.57 also prints the text on stdout after a failed write, so judge the last line only.
  case $(lab_shell_family) in
    zsh | zsh-sh) [ "${output##*$'\n'}" = "st=0" ] ;;
    bash | yash | busybox | posh | mksh | ksh93 | dash) [ "${output##*$'\n'}" = "st=1" ] ;;
    *) false ;;
  esac
  content=$(cat ro)
  [ "$content" = q ]
}

@test "descriptors above 2 opened by exec reach children, except in ksh93 and mksh unless set -o posix" {
  run_lab_c 'exec 8>f; exec 9>&8; "$FDPROBE"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | dash)
      has_fd 8
      has_fd 9
      ;;
    mksh | ksh93)
      lacks_fd 8
      lacks_fd 9
      ;;
    *) false ;;
  esac
  run_lab_c 'set -o posix 2>/dev/null; echo "set=$?"; exec 8>f; "$FDPROBE"'
  case $(lab_shell_family) in
    mksh | bash | yash)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "set=0" ]
      has_fd 8
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "set=1" ]
      has_fd 8
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$status" -eq 0 ]
          [ "${lines[0]}" = "set=0" ]
          has_fd 8
          ;;
        *)
          [ "$status" -eq 2 ]
          [ -z "$output" ]
          ;;
      esac
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ -z "$output" ]
      ;;
    zsh | zsh-sh | posh)
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    *) false ;;
  esac
}

@test "a per-command redirection reaches the child in every shell and a trailing n>&- hides an inherited descriptor" {
  run_lab_c '"$FDPROBE" 8>f'
  [ "$status" -eq 0 ]
  has_fd 8
  run_lab_c 'exec 8>f; "$FDPROBE" 8>&-'
  [ "$status" -eq 0 ]
  lacks_fd 8
  run_lab_c 'exec 8>f; (exec 9>&8 8>&-; "$FDPROBE")'
  [ "$status" -eq 0 ]
  lacks_fd 8
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | dash) has_fd 9 ;;
    mksh | ksh93) lacks_fd 9 ;;
    *) false ;;
  esac
  # mksh and ksh93u+m mark exec descriptors close-on-exec, so only set -o posix lets a trailing n>&- show its effect there.
  case $(lab_shell_family) in
    mksh) posix=yes ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) posix=yes ;;
        *) posix=no ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | dash) posix=no ;;
    *) false ;;
  esac
  if [ "$posix" = yes ]; then
    run_lab_c 'set -o posix; exec 8>f; "$FDPROBE"'
    [ "$status" -eq 0 ]
    has_fd 8
    run_lab_c 'set -o posix; exec 8>f; "$FDPROBE" 8>&-'
    [ "$status" -eq 0 ]
    lacks_fd 8
    run_lab_c 'set -o posix; exec 8>f; (exec 9>&8 8>&-; "$FDPROBE")'
    [ "$status" -eq 0 ]
    lacks_fd 8
    has_fd 9
  fi
}

@test "a redirection on a compound command ends with it, exec keeps it, a subshell inherits it" {
  run_lab_c '{ echo a >&6; } 6>c1; { echo b >&6; } 2>/dev/null; echo "st=$?"; cat c1'
  [ "$status" -eq 0 ]
  [ "${#lines[@]}" -eq 2 ]
  want=$(redir_fail_status)
  [ "${lines[0]}" = "st=$want" ]
  [ "${lines[1]}" = a ]
  run_lab_c 'exec 6>c2; (echo in >&6); echo out >&6; cat c2'
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf 'in\nout')" ]
}

@test "a background child holding a duplicate of the pipe keeps a command substitution waiting" {
  # the background job belongs to an inner /bin/sh, so the outer shell has no job to wait for
  run_lab_c 's=$(date +%s); o=$(/bin/sh -c "/bin/sleep 3 & echo \$!"); e=$(date +%s); kill -9 "$o" 2>/dev/null; echo "dt=$((e - s))"'
  [ "$status" -eq 0 ]
  waited=${output#dt=}
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | dash | mksh) [ "$waited" -ge 3 ] ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "$waited" -lt 3 ] ;;
        *) [ "$waited" -ge 3 ] ;;
      esac
      ;;
    *) false ;;
  esac
  run_lab_c 's=$(date +%s); o=$(/bin/sh -c "/bin/sleep 3 >/dev/null & echo \$!"); e=$(date +%s); kill -9 "$o" 2>/dev/null; echo "dt=$((e - s))"'
  [ "$status" -eq 0 ]
  waited=${output#dt=}
  [ "$waited" -lt 3 ]
}

@test "with the job in the substitution's own shell, redirecting its output frees the substitution except in yash and ksh93u+m" {
  run_lab_c 's=$(date +%s); o=$(/bin/sleep 3 >/dev/null 2>&1 </dev/null & echo $!); e=$(date +%s); kill -9 "$o" 2>/dev/null; echo "dt=$((e - s))"'
  [ "$status" -eq 0 ]
  waited=${output#dt=}
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | busybox | posh | dash | mksh) [ "$waited" -lt 3 ] ;;
    yash) [ "$waited" -ge 3 ] ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "$waited" -ge 3 ] ;;
        *) [ "$waited" -lt 3 ] ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "the word after >& is expanded: a digit string duplicates and a lone - closes, except in BusyBox ash" {
  run_lab_c 'fd=2; { echo x >&$fd; } 2>e 1>o; echo "o=[$(cat o)] e=[$(cat e)]"'
  [ "$status" -eq 0 ]
  [ "$output" = "o=[] e=[x]" ]
  run_lab_c 'v=-; exec 6>f; exec 6>&$v; { echo y >&6; } 2>/dev/null; echo "st=$?"'
  case $(lab_shell_family) in
    busybox)
      [ "$status" -eq 2 ]
      case $output in
        *"st="*) false ;;
        *) ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | posh | mksh | ksh93 | dash)
      [ "$status" -eq 0 ]
      want=$(redir_fail_status)
      [ "$output" = "st=$want" ]
      ;;
    *) false ;;
  esac
  run_lab_c 'v=7; exec 6>f; exec 7>&6; exec 6>&$v; echo y >&6; echo "st=$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "st=0" ]
  content=$(cat f)
  [ "$content" = y ]
}

@test "a blank is allowed between >& and its word" {
  run_lab_c 'exec 6>b; echo x >& 6; exec 6>&-; cat b'
  [ "$status" -eq 0 ]
  [ "$output" = x ]
}

@test ">&word with a non-numeric word is unspecified: bash, zsh and BusyBox ash write both streams to a file" {
  run_lab_c 'echo x >&out 2>&1; echo "st=$?"'
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "st=0" ]
      [ -f out ]
      content=$(cat out)
      [ "$content" = x ]
      # The word opens one file for both streams, so stderr must land there too and after stdout.
      run_lab_c '{ echo o; echo e >&2; } >&both; echo "st=$?"'
      [ "$status" -eq 0 ]
      [ "$output" = "st=0" ]
      content=$(cat both)
      [ "$content" = $'o\ne' ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ ! -e out ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "${output##*$'\n'}" = "st=2" ]
      [ ! -e out ]
      ;;
    posh | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "${output##*$'\n'}" = "st=1" ]
      [ ! -e out ]
      ;;
    *) false ;;
  esac
}

@test "a descriptor number above 9 is a word in dash, mksh, ksh93, zsh and posh, a descriptor elsewhere" {
  run_lab_c '(exec 10>big; echo x >&10; exec 10>&-; cat big) 2>/dev/null; echo "rc=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | yash | busybox)
      [ "$output" = "$(printf 'x\nrc=0')" ]
      ;;
    dash | mksh | ksh93 | zsh | zsh-sh | posh)
      [ "$output" = "rc=127" ]
      [ -e big ]
      [ ! -s big ]
      ;;
    *) false ;;
  esac
}

@test "{var} allocates a descriptor of 10 or more in bash 4.1+, ksh93 and zsh" {
  run_lab_c '(exec {fd}>fdv && echo x >&$fd && { [ "$fd" -ge 10 ] && echo "ok"; }) 2>/dev/null; echo "rc=$?"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*) expect=absent ;;
        *) expect=works ;;
      esac
      ;;
    zsh | ksh93) expect=works ;;
    zsh-sh | yash | busybox | posh | mksh | dash) expect=absent ;;
    *) false ;;
  esac
  case $expect in
    works)
      [ "$output" = "$(printf 'ok\nrc=0')" ]
      content=$(cat fdv)
      [ "$content" = x ]
      ;;
    absent)
      [ "$output" = "rc=127" ]
      ;;
    *) false ;;
  esac
}

@test "native zsh copies a stdout redirect into a pipe (MULTIOS); no other shell does" {
  run_lab_c 'exec 6>&1; echo x >&6 | cat'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh) [ "$output" = "$(printf 'x\nx')" ] ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash) [ "$output" = x ] ;;
    *) false ;;
  esac
  run_lab_c 'exec 6>&1; { echo x >&6; } | cat'
  [ "$status" -eq 0 ]
  [ "$output" = x ]
}
