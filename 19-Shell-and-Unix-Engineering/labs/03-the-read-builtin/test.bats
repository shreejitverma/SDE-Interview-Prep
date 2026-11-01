#!/usr/bin/env bats
# Bats test suite for The read Builtin (POSIX.1-2024 XCU read).
# shellcheck disable=SC2016 # expressions belong to shell under test

setup() {
  load ../lib/lab
  PROBES=$BATS_TEST_DIRNAME/read-probes.sh
  cd "$BATS_TEST_TMPDIR" || return 1
}

# set_last_line: set last_line to the final line of $output (stderr diagnostics come first under `run`).
set_last_line() {
  last_line=${lines[${#lines[@]} - 1]}
}

# yash_generation: set yash_gen to old (yash 2.58.x, from the Alpine package) or new (yash 2.61,
# measured on a build of its release tarball); skip the test for any other yash, which is not measured here.
yash_generation() {
  local version
  version=$(lab_shell_version)
  case $version in
    2.58*) yash_gen=old ;;
    2.61*) yash_gen=new ;;
    *) skip "yash $version is not measured here (2.59 changed read per its NEWS)" ;;
  esac
}

@test "read splits into fields, trims outer IFS whitespace, and preserves remainder" {
  d=$BATS_TEST_TMPDIR
  printf '   alpha   beta   gamma   delta   \n' >"$d/input.txt"
  run_lab "$PROBES" split <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "v1=[alpha] v2=[beta] v3=[gamma   delta]" ]
}

@test "read assigns empty string to excess variable operands when fields are fewer" {
  d=$BATS_TEST_TMPDIR
  printf 'only_one\n' >"$d/input.txt"
  run_lab "$PROBES" split <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "v1=[only_one] v2=[] v3=[]" ]
}

@test "read without -r interprets backslash as escape and line continuation" {
  d=$BATS_TEST_TMPDIR
  printf 'escaped\\ space trailing\\\\\nescaped\\ space trailing\\\\\n' >"$d/input.txt"
  run_lab "$PROBES" raw-vs-escape <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = 'esc=[escaped space|trailing\] raw=[escaped\|space trailing\\]' ]
}

@test "read without -r joins lines on backslash-newline continuation" {
  d=$BATS_TEST_TMPDIR
  printf 'first\\\nsecond\nthird\n' >"$d/input.txt"
  run_lab_c '{ read e; read -r r; printf "e=[%s] r=[%s]" "$e" "$r"; }' <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = 'e=[firstsecond] r=[third]' ]
}

@test "read with IFS empty preserves all leading and trailing whitespace" {
  d=$BATS_TEST_TMPDIR
  printf '   padded string   \n' >"$d/input.txt"
  run_lab_c '{ IFS= read -r line; printf "[%s]" "$line"; }' <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "[   padded string   ]" ]
  run_lab_c '{ read -r line; printf "[%s]" "$line"; }' <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "[padded string]" ]
}

@test "read with duplicate variable operands retains the last assignment" {
  d=$BATS_TEST_TMPDIR
  printf 'first second\n' >"$d/input.txt"
  run_lab_c '{ read -r x x; printf "[%s]" "$x"; }' <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "[second]" ]
}

@test "read detects EOF on input lacking trailing newline, returning status 1 with partial line" {
  d=$BATS_TEST_TMPDIR
  printf 'unterminated' >"$d/input.txt"
  run_lab_c '{ val=unset; read -r val; st=$?; printf "st=%d val=[%s]" "$st" "$val"; }' <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "st=1 val=[unterminated]" ]
}

@test "read returns status 1 and empties variables on immediate EOF" {
  d=$BATS_TEST_TMPDIR
  : >"$d/empty.txt"
  run_lab_c '{ val=preset; read -r val; st=$?; printf "st=%d val=[%s]" "$st" "$val"; }' <"$d/empty.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "st=1 val=[]" ]
}

@test "robust while-read loop processes final record lacking newline, unlike naive loop" {
  d=$BATS_TEST_TMPDIR
  printf 'line1\nline2' >"$d/input.txt"
  run_lab "$PROBES" naive-loop <"$d/input.txt"
  [ "$status" -eq 0 ]
  [ "$output" = "LINE:[line1]" ]

  run_lab "$PROBES" eof-loop <"$d/input.txt"
  [ "$status" -eq 0 ]
  expected=$(printf 'LINE:[line1]\nLINE:[line2]')
  [ "$output" = "$expected" ]
}

@test "read -d delim splits on custom single-byte delimiter or reports unsupported" {
  d=$BATS_TEST_TMPDIR
  printf 'alpha:beta:gamma\n' >"$d/input.txt"
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | busybox | mksh | ksh93)
      run_lab "$PROBES" delim : <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "delim=[alpha] status=[0]" ]
      ;;
    dash)
      run_lab "$PROBES" delim : <"$d/input.txt"
      [ "$status" -eq 0 ]
      set_last_line
      [ "$last_line" = "delim=[] status=[2]" ]
      ;;
    yash)
      yash_generation
      run_lab "$PROBES" delim : <"$d/input.txt"
      [ "$status" -eq 0 ]
      set_last_line
      if [ "$yash_gen" = old ]; then
        [ "$last_line" = "delim=[] status=[2]" ]
      else
        [ "$last_line" = "delim=[alpha] status=[0]" ]
      fi
      ;;
    posh)
      run_lab "$PROBES" delim : <"$d/input.txt"
      [ "$status" -eq 0 ]
      set_last_line
      [ "$last_line" = "delim=[] status=[1]" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read -d '' processes null-delimited records" {
  d=$BATS_TEST_TMPDIR
  printf 'first/path\0second/path\0' >"$d/input.txt"
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | busybox | mksh)
      run_lab_c '{ IFS= read -r -d "" p1; IFS= read -r -d "" p2; printf "[%s][%s]" "$p1" "$p2"; }' <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "[first/path][second/path]" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          run_lab_c '{ IFS= read -r -d "" p1; IFS= read -r -d "" p2; printf "[%s][%s]" "$p1" "$p2"; }' <"$d/input.txt"
          [ "$status" -eq 0 ]
          [ "$output" = "[first/path][second/path]" ]
          ;;
        *)
          # Legacy ksh93u+ 2012 drops NUL and reads until newline or EOF
          run_lab_c '{ IFS= read -r -d "" p1; IFS= read -r -d "" p2; printf "[%s][%s]" "$p1" "$p2"; }' <"$d/input.txt"
          [ "$status" -eq 0 ]
          [ "$output" = "[first/pathsecond/path][]" ]
          ;;
      esac
      ;;
    dash)
      # Input is present, so status 2 cannot be an EOF: it is the invalid-option error.
      run_lab_c 'read -d "" p1' <"$d/input.txt"
      [ "$status" -eq 2 ]
      ;;
    yash)
      yash_generation
      if [ "$yash_gen" = old ]; then
        run_lab_c 'read -d "" p1' <"$d/input.txt"
        [ "$status" -eq 2 ]
      else
        # yash 2.61 accepts -d but treats the empty string as "no delimiter": it reads to EOF (status 1)
        # and the value stops at the first NUL.
        run_lab_c 'read -d "" p1; st=$?; printf "st=%d p1=[%s]" "$st" "$p1"' <"$d/input.txt"
        [ "$status" -eq 0 ]
        [ "$output" = "st=1 p1=[first/path]" ]
      fi
      ;;
    posh)
      # posh reports an invalid option with status 1; input is present, so it is not an EOF.
      run_lab_c 'read -d "" p1; st=$?; printf "st=%d p1=[%s]" "$st" "${p1-unset}"' <"$d/input.txt"
      [ "$status" -eq 0 ]
      set_last_line
      [ "$last_line" = "st=1 p1=[unset]" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read without operand sets REPLY in bash, zsh, ksh, mksh, busybox but errors in dash, posh, yash" {
  d=$BATS_TEST_TMPDIR
  printf 'standalone line\n' >"$d/input.txt"
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | mksh | busybox)
      run_lab_c '{ read; printf "[%s]" "$REPLY"; }' <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "[standalone line]" ]
      ;;
    dash)
      run_lab_c 'read' <"$d/input.txt"
      [ "$status" -eq 2 ]
      ;;
    yash)
      yash_generation
      run_lab_c 'read' <"$d/input.txt"
      if [ "$yash_gen" = old ]; then
        [ "$status" -eq 2 ]
      else
        [ "$status" -eq 4 ]
      fi
      ;;
    posh)
      # Input is present, so status 1 is the missing-operand error, not an EOF.
      run_lab_c 'read' <"$d/input.txt"
      [ "$status" -eq 1 ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read unbuffered stream handoff does not consume past the newline" {
  d=$BATS_TEST_TMPDIR
  printf 'line1\nline2\nline3\n' >"$d/input.txt"
  run_lab "$PROBES" stream-handoff <"$d/input.txt"
  [ "$status" -eq 0 ]
  expected=$(printf 'FIRST:[line1]\nline2\nline3')
  [ "$output" = "$expected" ]
}

@test "readonly variable error in read returns status > 1 or fails per implementation" {
  d=$BATS_TEST_TMPDIR
  printf 'one two three\n' >"$d/input.txt"
  case $(lab_shell_family) in
    bash)
      # bash 5.3+ returns status 2 (EX_MISCERROR) per POSIX.1-2024;
      # bash 4.2 through 5.2 halt and return status 1 (EXECUTION_FAILURE; the bash 4.2 CHANGES
      # entry "read ... readonly variable" added the failure); bash 3.2 through 4.1 return 0
      # and keep assigning later variables (4.0, 4.1 measured with the official bash images).
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      case $(lab_shell_version) in
        3.2* | 4.0* | 4.1*)
          [ "$status" -eq 0 ]
          [ "$output" = "st=0 a=one ro=fixed b=three" ]
          ;;
        4.* | 5.0* | 5.1* | 5.2*)
          [ "$status" -eq 0 ]
          [ "$output" = "st=1 a=one ro=fixed b=orig" ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "st=2 a=one ro=fixed b=orig" ]
          ;;
      esac
      ;;
    dash | mksh | busybox)
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "st=2 a=one ro=fixed b=orig" ]
      ;;
    ksh93)
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "st=1 a=one ro=fixed b=three" ]
      ;;
    yash)
      yash_generation
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      [ "$status" -eq 0 ]
      if [ "$yash_gen" = old ]; then
        [ "$output" = "st=1 a=one ro=fixed b=three" ]
      else
        [ "$output" = "st=2 a=one ro=fixed b=three" ]
      fi
      ;;
    zsh | zsh-sh)
      # In zsh, assigning to a readonly variable is a fatal shell error: the probe dies
      # with status 1 before it prints its result line.
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      [ "$status" -eq 1 ]
      [ -z "$output" ]
      ;;
    posh)
      run_lab "$PROBES" readonly-assign <"$d/input.txt"
      [ "$status" -eq 0 ]
      [ "$output" = "st=1 a=one ro=fixed b=orig" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipeline subshell trap isolates variable updates in subshell-based shells" {
  case $(lab_shell_family) in
    bash | dash | busybox | posh | mksh | yash | zsh-sh)
      run_lab_c 'count=0; printf "a\nb\nc\n" | while read -r line; do count=$((count + 1)); done; printf "%d" "$count"'
      [ "$status" -eq 0 ]
      [ "$output" = "0" ]
      ;;
    zsh | ksh93)
      run_lab_c 'count=0; printf "a\nb\nc\n" | while read -r line; do count=$((count + 1)); done; printf "%d" "$count"'
      [ "$status" -eq 0 ]
      [ "$output" = "3" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read-probes returns status 64 on unknown mode" {
  run_lab "$PROBES" bogus
  [ "$status" -eq 64 ]
  [ "$output" = "read-probes: unknown mode: bogus" ]
}

@test "read hands the rest of a pipe to the next reader without over-reading" {
  run_lab_c 'printf "line1\nline2\nline3\n" | { read -r first; printf "FIRST:[%s]\n" "$first"; cat; }'
  [ "$status" -eq 0 ]
  expected=$(printf 'FIRST:[line1]\nline2\nline3')
  [ "$output" = "$expected" ]
}

@test "read evaluates an array subscript inside a variable operand only in some shells" {
  # The subscript runs a command substitution that touches a flag file and then yields 1.
  name='a[$(: >injected.flag; echo 1)]'
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | mksh | posh)
      expect=ran
      ;;
    dash | busybox | ksh93 | yash)
      expect=clean
      ;;
    *)
      false
      ;;
  esac
  case $(lab_shell_family) in
    yash) yash_generation ;;
    *) ;;
  esac
  run_lab_c 'rm -f injected.flag; printf "x\n" | read -r "$1" 2>/dev/null; if [ -e injected.flag ]; then echo ran; else echo clean; fi' sh "$name"
  [ "$status" -eq 0 ]
  set_last_line
  [ "$last_line" = "$expect" ]
}

@test "read handles an embedded NUL byte in default newline mode per shell" {
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2* | 4.0* | 4.1* | 4.2*) expect='len=2 v=[ab]' ;; # truncates at the NUL (4.0 to 4.2 measured with the official bash images)
        *) expect='len=4 v=[abcd]' ;;                       # 4.3 and later drop the NUL
      esac
      ;;
    yash)
      yash_generation
      expect='len=2 v=[ab]' # yash 2.58.1 and 2.61 truncate at the NUL
      ;;
    zsh | zsh-sh)
      expect='len=5 v=[ab@cd]' # zsh keeps the NUL (shown as @ by tr)
      ;;
    dash | posh | mksh | ksh93 | busybox)
      expect='len=4 v=[abcd]'
      ;;
    *)
      false
      ;;
  esac
  run_lab_c 'printf "ab\0cd\nxyz\n" | { read -r v; printf "len=%d v=[%s]" "${#v}" "$(printf %s "$v" | tr "\0" @)"; }'
  [ "$status" -eq 0 ]
  [ "$output" = "$expect" ]
}
