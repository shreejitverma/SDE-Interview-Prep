#!/usr/bin/env bats
# terminfo.bats - terminfo database, tput exit statuses, parameter strings, search order.
# Every terminal here is compiled from lab.ti into $BATS_TEST_TMPDIR, so no system entry is needed.
# Some userlands ship no ncurses utilities at all (Alpine without the ncurses package: BusyBox has no
# tput, tic or infocmp applet). There each test asserts the real failure mode instead: exit status 127.

# shellcheck disable=SC2016 # single-quoted code belongs to the shell under test
bats_require_minimum_version 1.5.0

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/probe_terminfo.sh"
  TI="$BATS_TEST_TMPDIR/ti"
  mkdir -p "$TI" "$BATS_TEST_TMPDIR/home"
  if have_ncurses; then
    tic -x -o "$TI" "$BATS_TEST_DIRNAME/lab.ti"
  fi
  export TERMINFO="$TI" TERM=labterm HOME="$BATS_TEST_TMPDIR/home"
  unset COLUMNS LINES TERMINFO_DIRS
}

# have_ncurses: true when an ncurses tput is on PATH (feature detection, never a platform file).
have_ncurses() {
  command -v tput >/dev/null 2>&1
}

# sys_vt100: true when the system terminfo database has an entry for vt100.
sys_vt100() {
  local d
  for d in /etc/terminfo /usr/share/terminfo /lib/terminfo /usr/lib/terminfo; do
    if [ -f "$d/v/vt100" ] || [ -f "$d/76/vt100" ]; then
      return 0
    fi
  done
  return 1
}

# absent_status ARG...: with no tput, the probe reports exit status 127 and no bytes.
absent_status() {
  run_lab "$PROBE" status "$@"
  [ "$output" = "rc=127 hex=" ]
}

esc_hex() {
  local out
  printf "%b" "$1" >"$BATS_TEST_TMPDIR/esc.bin"
  out=$(od -An -v -tx1 "$BATS_TEST_TMPDIR/esc.bin")
  printf "%s" "${out//[[:space:]]/}"
}

@test "tput exit status: boolean true 0, boolean false 1, absent number -1 with 0, absent string 1" {
  if ! have_ncurses; then
    # A missing tput is status 127 for every kind of capability, so it is distinct from a false
    # boolean (1) but looks like "capability absent" to any script that only tests for nonzero.
    absent_status am
    absent_status hs
    absent_status cols
    absent_status lm
    absent_status bold
    absent_status blink
    return 0
  fi
  run_lab "$PROBE" status am
  [ "$output" = "rc=0 hex=" ]
  run_lab "$PROBE" status hs
  [ "$output" = "rc=1 hex=" ]
  run_lab "$PROBE" status cols
  [ "$output" = "rc=0 hex=3130300a" ]
  run_lab "$PROBE" status lm
  [ "$output" = "rc=0 hex=2d310a" ]
  run_lab "$PROBE" status bold
  [ "$output" = "rc=0 hex=1b5b316d" ]
  run_lab "$PROBE" status blink
  [ "$output" = "rc=1 hex=" ]
}

@test "tput usage 2, unknown terminal 3, unknown capability 4" {
  if ! have_ncurses; then
    lab_shell_words
    run -127 "${LAB_SHELL_WORDS[@]}" -c 'tput clear 2>/dev/null'
    run -127 "${LAB_SHELL_WORDS[@]}" -c 'tput -T nonesuch_terminal clear 2>/dev/null'
    run -127 "${LAB_SHELL_WORDS[@]}" -c 'tput nonesuch_cap 2>/dev/null'
    return 0
  fi
  run_lab_c 'env -u TERM tput clear 2>/dev/null'
  [ "$status" -eq 2 ]
  run_lab_c 'tput -T nonesuch_terminal clear 2>/dev/null'
  [ "$status" -eq 3 ]
  run_lab_c 'tput nonesuch_cap 2>/dev/null'
  [ "$status" -eq 4 ]
}

@test "cup runs %i and %p through tparm: row 10 column 20 becomes ESC [ 11 ; 21 H" {
  if ! have_ncurses; then
    absent_status cup 10 20
    absent_status cup 0 0
    return 0
  fi
  run_lab "$PROBE" status cup 10 20
  [ "$output" = "rc=0 hex=1b5b31313b323148" ]
  run_lab "$PROBE" status cup 0 0
  [ "$output" = "rc=0 hex=1b5b313b3148" ]
}

@test "setaf takes the three branches of its %? conditional" {
  if ! have_ncurses; then
    absent_status setaf 1
    absent_status setaf 9
    absent_status setaf 196
    return 0
  fi
  run_lab "$PROBE" status setaf 1
  want=$(esc_hex '\033[31m')
  [ "$output" = "rc=0 hex=$want" ]
  run_lab "$PROBE" status setaf 9
  want=$(esc_hex '\033[91m')
  [ "$output" = "rc=0 hex=$want" ]
  run_lab "$PROBE" status setaf 196
  want=$(esc_hex '\033[38;5;196m')
  [ "$output" = "rc=0 hex=$want" ]
}

@test "COLUMNS and LINES override the database, and -T makes tput ignore them" {
  if ! have_ncurses; then
    # COLUMNS and LINES are read by tput itself; without tput they produce nothing.
    run_lab_c 'COLUMNS=132 LINES=50 tput cols 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    run_lab_c 'COLUMNS=132 LINES=50 tput -T labterm cols 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    return 0
  fi
  run_lab_c 'COLUMNS=132 LINES=50 tput cols; COLUMNS=132 LINES=50 tput lines'
  [ "$output" = "$(printf '132\n50')" ]
  run_lab_c 'COLUMNS=132 LINES=50 tput -T labterm cols; COLUMNS=132 LINES=50 tput -T labterm lines'
  [ "$output" = "$(printf '100\n30')" ]
}

@test "under set -e an unguarded capture of an absent capability aborts, a guarded one survives" {
  if ! have_ncurses; then
    # The guard also covers a missing tput; the unguarded form dies with 127 before "reached".
    run_lab "$PROBE" cache-safe blink
    [ "$status" -eq 0 ]
    [ "$output" = "reached cached=" ]
    lab_shell_words
    run -127 "${LAB_SHELL_WORDS[@]}" "$PROBE" cache-unsafe bold
    [[ "$output" != *reached* ]]
    return 0
  fi
  run_lab "$PROBE" cache-safe blink
  [ "$status" -eq 0 ]
  [ "$output" = "reached cached=" ]
  run_lab "$PROBE" cache-unsafe blink
  [ "$status" -eq 1 ]
  [ "$output" = "" ]
  run_lab "$PROBE" cache-unsafe bold
  [ "$status" -eq 0 ]
  [ "$output" = "reached cached=1b5b316d" ]
}

@test "tput -S yields the same bytes as separate calls, and an unknown name aborts the batch" {
  if ! have_ncurses; then
    run_lab_c 'printf "bold\nsgr0\n" | tput -S 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    return 0
  fi
  run_lab_c 'printf "bold\nsgr0\n" | tput -S | od -An -v -tx1 | tr -d " \n"'
  [ "$output" = "1b5b316d1b5b6d" ]
  run_lab_c 'printf "bold\nnosuch\nsgr0\n" | tput -S 2>/dev/null | od -An -v -tx1 | tr -d " \n"'
  [ "$output" = "1b5b316d" ]
  run_lab_c 'printf "bold\nhs\nsgr0\n" | tput -S >/dev/null 2>&1; echo $?'
  [ "$output" = "5" ]
  run_lab_c 'printf "bold\nnosuch\nsgr0\n" | tput -S >/dev/null 2>&1; echo $?'
  [ "$output" = "4" ]
  run_lab_c 'printf "bold\nhs\nsgr0\n" | tput -S 2>/dev/null | od -An -v -tx1 | tr -d " \n"'
  [ "$output" = "1b5b316d1b5b6d" ]
}

@test "TERMINFO beats TERMINFO_DIRS and the first TERMINFO_DIRS entry wins" {
  if ! have_ncurses; then
    # No tput means no search order to observe; the variables cannot make the utility appear.
    run_lab_c 'TERMINFO="$1" TERMINFO_DIRS="$2" tput cols 2>/dev/null; echo "rc=$?"' sh "$TI" "$BATS_TEST_TMPDIR/ti2"
    [ "$output" = "rc=127" ]
    return 0
  fi
  sed 's/cols#100/cols#90/' "$BATS_TEST_DIRNAME/lab.ti" >"$BATS_TEST_TMPDIR/other.ti"
  mkdir -p "$BATS_TEST_TMPDIR/ti2"
  tic -x -o "$BATS_TEST_TMPDIR/ti2" "$BATS_TEST_TMPDIR/other.ti"
  TI2="$BATS_TEST_TMPDIR/ti2"
  run_lab_c 'TERMINFO="$1" TERMINFO_DIRS="$2" tput cols' sh "$TI" "$TI2"
  [ "$output" = "100" ]
  run_lab_c 'env -u TERMINFO TERMINFO_DIRS="$1" tput cols' sh "$TI2"
  [ "$output" = "90" ]
  run_lab_c 'env -u TERMINFO TERMINFO_DIRS="$1:$2" tput cols' sh "$TI2" "$TI"
  [ "$output" = "90" ]
  run_lab_c 'env -u TERMINFO TERMINFO_DIRS="$1:$2" tput cols' sh "$TI" "$TI2"
  [ "$output" = "100" ]
}

@test "an alias in the name field resolves, and longname is the description" {
  if ! have_ncurses; then
    run_lab_c 'tput -T labalias cols 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    run_lab_c 'tput longname 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    return 0
  fi
  run_lab_c 'tput -T labalias cols'
  [ "$output" = "100" ]
  run_lab_c 'tput longname'
  [ "$output" = "lab terminal" ]
}

@test "tic -x keeps a user-defined capability, plain tic drops it" {
  if ! have_ncurses; then
    run_lab_c 'tic -x -o "$1" "$2" 2>/dev/null; echo "rc=$?"' sh "$TI" "$BATS_TEST_DIRNAME/lab.ti"
    [ "$output" = "rc=127" ]
    run_lab_c 'tput labseq 2>/dev/null; echo "rc=$?"'
    [ "$output" = "rc=127" ]
    return 0
  fi
  run_lab "$PROBE" status labseq
  [ "$output" = "rc=0 hex=1b5b39396d" ]
  mkdir -p "$BATS_TEST_TMPDIR/plain"
  tic -o "$BATS_TEST_TMPDIR/plain" "$BATS_TEST_DIRNAME/lab.ti" 2>/dev/null
  run_lab_c 'TERMINFO="$1" tput labseq 2>/dev/null' sh "$BATS_TEST_TMPDIR/plain"
  [ "$status" -eq 4 ]
}

@test "compiled entry lives in a one-letter or two-digit-hex directory and its header sizes add up" {
  if ! have_ncurses; then
    # tic is missing too, so nothing is written and the scratch database stays empty.
    run_lab_c 'tic -o "$1" "$2" 2>/dev/null; echo "rc=$?"' sh "$TI" "$BATS_TEST_DIRNAME/lab.ti"
    [ "$output" = "rc=127" ]
    left=$(ls -A "$TI")
    [ -z "$left" ]
    return 0
  fi
  mkdir -p "$BATS_TEST_TMPDIR/plain"
  tic -o "$BATS_TEST_TMPDIR/plain" "$BATS_TEST_DIRNAME/lab.ti" 2>/dev/null
  if [ -f "$BATS_TEST_TMPDIR/plain/l/labterm" ]; then
    [ ! -e "$BATS_TEST_TMPDIR/plain/6c/labterm" ]
    file=$BATS_TEST_TMPDIR/plain/l/labterm
  else
    [ -f "$BATS_TEST_TMPDIR/plain/6c/labterm" ]
    file=$BATS_TEST_TMPDIR/plain/6c/labterm
  fi
  run_lab "$PROBE" header "$file"
  [[ "$output" == magic=0x011a\ * || "$output" == magic=0x021e\ * ]]
  if [[ "$output" == magic=0x011a\ * ]]; then
    [ "$output" = "magic=0x011a names=30 bools=2 nums=14 offsets=360 table=89 size=881 expected=881" ]
  fi
  size=${output##*size=}
  size=${size%% *}
  want=${output##*expected=}
  [ "$size" = "$want" ]
  head -c 100 "$file" >"$BATS_TEST_TMPDIR/trunc"
  run_lab "$PROBE" header "$BATS_TEST_TMPDIR/trunc"
  size=${output##*size=}
  size=${size%% *}
  want=${output##*expected=}
  [ "$size" != "$want" ]
}

@test "tput is an external utility in every shell; only zsh has a terminfo module" {
  if have_ncurses; then
    run_lab_c 'command -v tput'
    [[ "$output" == /* ]]
  else
    # No ncurses package: neither tput nor tic resolves, and BusyBox has no applet for them.
    run_lab_c 'command -v tput; echo "rc=$?"'
    [[ "$output" == rc=[1-9]* ]]
    run_lab_c 'command -v tic; echo "rc=$?"'
    [[ "$output" == rc=[1-9]* ]]
    if command -v busybox >/dev/null 2>&1; then
      run busybox --list
      [ "$status" -eq 0 ]
      [[ "$output" != *$'\ntput\n'* ]]
      [[ "$output" != *$'\ntic\n'* ]]
      [[ "$output" != *$'\ninfocmp\n'* ]]
    fi
  fi
  case $(lab_shell_family) in
    zsh | zsh-sh)
      if have_ncurses; then
        run_lab_c 'zmodload zsh/terminfo && echo "${terminfo[cols]}" && echoti cup 10 20 | od -An -v -tx1 | tr -d " \n"'
        [ "$status" -eq 0 ]
        [ "$output" = "$(printf '100\n1b5b31313b323148')" ]
        fam=$(lab_shell_family)
        if [ "$fam" = zsh-sh ]; then
          # In sh emulation the unbraced subscript is not an array reference: $terminfo is empty.
          run_lab_c 'zmodload zsh/terminfo && echo $terminfo[cols]'
          [ "$output" = "no[cols]" ]
        fi
      else
        # The module reads the system database through libncurses, with no tput or tic involved.
        sys_vt100
        run_lab_c 'unset TERMINFO; TERM=vt100; zmodload zsh/terminfo && echo "${terminfo[cols]}" && echoti cup 10 20 | od -An -v -tx1 | tr -d " \n"'
        [ "$status" -eq 0 ]
        [ "$output" = "$(printf '80\n1b5b31313b323148')" ]
      fi
      ;;
    bash | dash | posh | mksh | ksh93 | yash | busybox)
      run_lab_c 'command -v echoti >/dev/null 2>&1 || echo absent'
      [ "$output" = "absent" ]
      ;;
    *) false ;;
  esac
}
