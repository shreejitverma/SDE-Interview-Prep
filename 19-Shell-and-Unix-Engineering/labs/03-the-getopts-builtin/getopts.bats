#!/usr/bin/env bats
# Tests for POSIX getopts builtin and option parsing semantics.

setup() {
  load ../lib/lab
  PARSE_SCRIPT="$BATS_TEST_DIRNAME/parse_options.sh"
}

@test "parse_options.sh parses separate options and operands" {
  run_lab "$PARSE_SCRIPT" -v -f "data file.csv" -o output.bin first second
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "verbose=1" ]
  [ "${lines[1]}" = "file=data file.csv" ]
  [ "${lines[2]}" = "output=output.bin" ]
  [ "${lines[3]}" = "operands=2" ]
  [ "${lines[4]}" = "arg: first" ]
  [ "${lines[5]}" = "arg: second" ]
}

@test "parse_options.sh parses clustered and attached option arguments" {
  run_lab "$PARSE_SCRIPT" -vf"attached.txt" -oout.bin -- operand1
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "verbose=1" ]
  [ "${lines[1]}" = "file=attached.txt" ]
  [ "${lines[2]}" = "output=out.bin" ]
  [ "${lines[3]}" = "operands=1" ]
  [ "${lines[4]}" = "arg: operand1" ]
}

@test "parse_options.sh reports missing argument and exits with code 2" {
  run_lab "$PARSE_SCRIPT" -v -f
  [ "$status" -eq 2 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 sets name=? instead of name=: on missing arguments in silent mode
      case "$output" in
        *"unknown option -f"*) ;;
        *) false ;;
      esac
      ;;
    bash | dash | mksh | ksh93 | zsh | zsh-sh | busybox | yash)
      case "$output" in
        *"option -f requires an argument"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "parse_options.sh reports unknown option and exits with code 2" {
  run_lab "$PARSE_SCRIPT" -z
  [ "$status" -eq 2 ]
  case "$output" in
    *"unknown option -z"*) ;;
    *) false ;;
  esac
}

@test "getopts preserves whitespace and special characters in option arguments" {
  # shellcheck disable=SC2016
  run_lab_c '
    OPTIND=1
    getopts "f:" opt -f "$(printf "line 1\nline 2\twith tabs")"
    printf "opt=%s\n" "$opt"
    printf "OPTARG=%s\n" "$OPTARG"
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "opt=f" ]
  [ "${lines[1]}" = "OPTARG=line 1" ]
  [ "${lines[2]}" = "line 2	with tabs" ]
}

@test "getopts treats leading dash or double dash as option-argument value" {
  # shellcheck disable=SC2016
  run_lab_c '
    OPTIND=1
    getopts "f:o:" opt -f -v -o --
    printf "1: opt=%s arg=%s OPTIND=%s\n" "$opt" "$OPTARG" "$OPTIND"
    getopts "f:o:" opt -f -v -o --
    printf "2: opt=%s arg=%s OPTIND=%s\n" "$opt" "$OPTARG" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1: opt=f arg=-v OPTIND=3" ]
  [ "${lines[1]}" = "2: opt=o arg=-- OPTIND=5" ]
}

@test "single dash operand ends option processing and is preserved as operand" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- - -a
    OPTIND=1
    getopts "ab" opt
    rc=$?
    printf "status=%d opt=%s OPTIND=%s\n" "$rc" "$opt" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | dash | mksh | ksh93 | busybox | yash)
      [ "$output" = "status=1 opt=? OPTIND=1" ]
      ;;
    zsh | zsh-sh)
      [ "$output" = "status=1 opt= OPTIND=1" ]
      ;;
    posh)
      # posh 0.14.1 skips a single dash and continues parsing subsequent options
      [ "$output" = "status=0 opt=a OPTIND=3" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "double dash operand ends option processing and advances OPTIND" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -a -- -b
    OPTIND=1
    getopts "ab" opt
    printf "1: opt=%s OPTIND=%s\n" "$opt" "$OPTIND"
    getopts "ab" opt
    rc=$?
    printf "2: status=%d opt=%s OPTIND=%s\n" "$rc" "$opt" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | dash | mksh | posh | ksh93 | busybox)
      [ "${lines[0]}" = "1: opt=a OPTIND=2" ]
      [ "${lines[1]}" = "2: status=1 opt=? OPTIND=3" ]
      ;;
    yash)
      [ "${lines[0]}" = "1: opt=a OPTIND=1:2" ]
      [ "${lines[1]}" = "2: status=1 opt=? OPTIND=3" ]
      ;;
    zsh | zsh-sh)
      [ "${lines[0]}" = "1: opt=a OPTIND=1" ]
      [ "${lines[1]}" = "2: status=1 opt=a OPTIND=3" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "clustered options report implementation-specific OPTIND on first flag" {
  # shellcheck disable=SC2016
  run_lab_c '
    OPTIND=1
    getopts "ab" opt -ab
    printf "opt=%s OPTIND=%s\n" "$opt" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | zsh | zsh-sh | posh)
      [ "$output" = "opt=a OPTIND=1" ]
      ;;
    dash | busybox | mksh)
      [ "$output" = "opt=a OPTIND=2" ]
      ;;
    yash)
      [ "$output" = "opt=a OPTIND=1:2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "OPTARG is unset or cleared when option has no argument" {
  # shellcheck disable=SC2016
  run_lab_c '
    OPTIND=1
    OPTARG=initial_value
    getopts "a" opt -a
    printf "opt=%s OPTARG=%s\n" "$opt" "${OPTARG-UNSET}"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | mksh | posh | ksh93 | yash)
      [ "$output" = "opt=a OPTARG=UNSET" ]
      ;;
    dash | busybox | zsh | zsh-sh)
      [ "$output" = "opt=a OPTARG=" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "getopts parses explicit param operands instead of positional parameters" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -x -y -z
    OPTIND=1
    count=0
    while getopts "ab:" opt -a -b payload operand1 operand2 2>/dev/null; do
      count=$((count + 1))
      printf "%d: opt=%s arg=%s\n" "$count" "$opt" "${OPTARG-UNSET}"
    done
    printf "ended: count=%d OPTIND=%s positional_1=%s\n" "$count" "$OPTIND" "$1"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | mksh | posh | ksh93 | yash)
      [ "${lines[0]}" = "1: opt=a arg=UNSET" ]
      [ "${lines[1]}" = "2: opt=b arg=payload" ]
      [ "${lines[2]}" = "ended: count=2 OPTIND=4 positional_1=-x" ]
      ;;
    dash | busybox | zsh | zsh-sh)
      [ "${lines[0]}" = "1: opt=a arg=" ]
      [ "${lines[1]}" = "2: opt=b arg=payload" ]
      [ "${lines[2]}" = "ended: count=2 OPTIND=4 positional_1=-x" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "resetting OPTIND to 1 after complete loop allows parsing new arguments" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -a
    OPTIND=1
    while getopts "a" opt; do
      pass1_opt=$opt
    done
    set -- -b
    OPTIND=1
    while getopts "b" opt; do
      pass2_opt=$opt
    done
    printf "pass1=%s pass2=%s\n" "$pass1_opt" "$pass2_opt"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "pass1=a pass2=b" ]
}

@test "function scope: OPTIND is shared globally except in native zsh" {
  # shellcheck disable=SC2016
  run_lab_c '
    sub() {
      printf "in_sub=%s\n" "$OPTIND"
      OPTIND=99
    }
    OPTIND=42
    sub
    printf "after_sub=%s\n" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      [ "${lines[0]}" = "in_sub=1" ]
      [ "${lines[1]}" = "after_sub=42" ]
      ;;
    bash | dash | mksh | posh | ksh93 | yash | busybox | zsh-sh)
      [ "${lines[0]}" = "in_sub=42" ]
      [ "${lines[1]}" = "after_sub=99" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "readonly name variable returns status greater than 1 or aborts" {
  # shellcheck disable=SC2016
  run_lab_c '
    readonly ro_opt=orig
    getopts "a" ro_opt -a 2>/dev/null
    rc=$?
    printf "status=%d\n" "$rc"
  '
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2*) [ "$output" = "status=1" ] ;;
        *) [ "$output" = "status=2" ] ;;
      esac
      ;;
    dash | mksh | ksh93 | yash | busybox)
      [ "$output" = "status=2" ]
      ;;
    posh)
      [ "$output" = "status=1" ]
      ;;
    zsh | zsh-sh)
      # zsh catches write to readonly variable and aborts execution before printf
      [ "$output" = "" ]
      ;;
    *)
      false
      ;;
  esac
}

# is_old_bash: success for Bash 3.2, whose getopts differs from Bash 4.4 and later in several rows.
is_old_bash() {
  case $(lab_shell_version) in
    3.2*) return 0 ;;
    *) return 1 ;;
  esac
}

# is_ksh93_um: success for ksh93u+m (Debian), failure for the 2012 ksh93u+ that macOS ships.
is_ksh93_um() {
  case $(lab_shell_version) in
    *93u+m*) return 0 ;;
    *) return 1 ;;
  esac
}

@test "exact worked example: separate options and operands print the documented output" {
  cd "$BATS_TEST_DIRNAME"
  run_lab ./parse_options.sh -v -f config.json -o bundle.tar alpha beta
  [ "$status" -eq 0 ]
  [ "$output" = "verbose=1
file=config.json
output=bundle.tar
operands=2
arg: alpha
arg: beta" ]
}

@test "exact worked example: invalid flag prints the documented diagnostics and exits 2" {
  cd "$BATS_TEST_DIRNAME"
  run_lab ./parse_options.sh -z
  [ "$status" -eq 2 ]
  case $(lab_shell_family) in
    zsh)
      # native zsh sets $0 to the function name inside usage()
      [ "$output" = "./parse_options.sh: error: unknown option -z
Usage: usage [-v] [-f file] [-o output] [--] [operands...]" ]
      ;;
    bash | dash | mksh | ksh93 | posh | busybox | yash | zsh-sh)
      [ "$output" = "./parse_options.sh: error: unknown option -z
Usage: ./parse_options.sh [-v] [-f file] [-o output] [--] [operands...]" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "silent mode: unknown option and missing argument set name and OPTARG, status 0" {
  # shellcheck disable=SC2016
  run_lab_c 'getopts ":a:" o -z; printf "rc=%s o=%s OPTARG=%s\n" "$?" "$o" "${OPTARG-UNSET}"'
  [ "$status" -eq 0 ]
  [ "$output" = "rc=0 o=? OPTARG=z" ]
  # shellcheck disable=SC2016
  run_lab_c 'getopts ":a:" o -a; printf "rc=%s o=%s OPTARG=%s\n" "$?" "$o" "${OPTARG-UNSET}"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 reports a missing argument as name=? instead of name=:
      [ "$output" = "rc=0 o=? OPTARG=a" ]
      ;;
    bash | dash | mksh | ksh93 | zsh | zsh-sh | busybox | yash)
      [ "$output" = "rc=0 o=: OPTARG=a" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "loud mode: unknown option and missing argument set name to ? and clear OPTARG, status 0" {
  # stderr is discarded here; the diagnostics themselves are covered by the OPTERR test.
  # shellcheck disable=SC2016
  run_lab_c 'OPTARG=junk; getopts "a:" o -z 2>/dev/null; printf "rc=%s o=%s OPTARG=%s\n" "$?" "$o" "${OPTARG-UNSET}"'
  [ "$status" -eq 0 ]
  expected_unknown=$output
  # shellcheck disable=SC2016
  run_lab_c 'OPTARG=junk; getopts "a:" o -a 2>/dev/null; printf "rc=%s o=%s OPTARG=%s\n" "$?" "$o" "${OPTARG-UNSET}"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      # zsh leaves OPTARG empty instead of unsetting it
      [ "$expected_unknown" = "rc=0 o=? OPTARG=" ]
      [ "$output" = "rc=0 o=? OPTARG=" ]
      ;;
    bash | dash | mksh | ksh93 | posh | busybox | yash)
      [ "$expected_unknown" = "rc=0 o=? OPTARG=UNSET" ]
      [ "$output" = "rc=0 o=? OPTARG=UNSET" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "end of options: status 1, name is ? except in zsh, OPTARG unchanged in dash and zsh" {
  # shellcheck disable=SC2016
  run_lab_c 'o=keep; OPTARG=junk; getopts "a" o x; printf "rc=%s o=%s OPTARG=%s OPTIND=%s\n" "$?" "$o" "${OPTARG-UNSET}" "$OPTIND"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$output" = "rc=1 o=keep OPTARG=junk OPTIND=1" ]
      ;;
    dash)
      [ "$output" = "rc=1 o=? OPTARG=junk OPTIND=1" ]
      ;;
    bash | mksh | ksh93 | posh | busybox | yash)
      [ "$output" = "rc=1 o=? OPTARG=UNSET OPTIND=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "OPTARG classifier separates missing argument from unknown option in every shell" {
  # The portable form of the Posh workaround: name is ? or :, so look the option up in optstring.
  # shellcheck disable=SC2016
  code='
    classify() {
      case $o in
        :) echo missing ;;
        \?)
          case ":a:" in
            *"$OPTARG:"*) echo missing ;;
            *) echo unknown ;;
          esac
          ;;
        *) echo "name=$o" ;;
      esac
    }
    getopts ":a:" o "$1"
    classify
  '
  run_lab_c "$code" sh -a
  [ "$status" -eq 0 ]
  [ "$output" = "missing" ]
  run_lab_c "$code" sh -z
  [ "$status" -eq 0 ]
  [ "$output" = "unknown" ]
}

@test "OPTERR=0 silences diagnostics in Bash and BusyBox ash only" {
  # shellcheck disable=SC2016
  code='
    err=$(getopts "a" o -z 2>&1 >/dev/null)
    if [ -n "$err" ]; then echo reported; else echo silent; fi
  '
  run_lab_c "$code"
  [ "$status" -eq 0 ]
  [ "$output" = "reported" ]
  run_lab_c "OPTERR=0; $code"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | busybox)
      [ "$output" = "silent" ]
      ;;
    dash | mksh | ksh93 | posh | yash | zsh | zsh-sh)
      [ "$output" = "reported" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unset OPTIND: restart, continue, fail, or abort depending on the shell" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -a -b -c
    getopts abc o "$@"
    unset OPTIND
    getopts abc o "$@" 2>/dev/null
    rc=$?
    printf "rc=%s o=%s OPTIND=%s\n" "$rc" "$o" "${OPTIND-UNSET}"
  '
  case $(lab_shell_family) in
    dash)
      # dash 0.5.12 aborts inside unset itself ("Illegal number:"); fixed after 0.5.12
      [ "$status" -eq 2 ]
      case "$output" in
        *"Illegal number"*) ;;
        *) false ;;
      esac
      ;;
    bash | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "rc=0 o=a OPTIND=2" ]
      ;;
    mksh | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "rc=0 o=b OPTIND=3" ]
      ;;
    ksh93 | zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "rc=0 o=b OPTIND=UNSET" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = "rc=2 o=a OPTIND=UNSET" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "+v is an option only in mksh and zsh, and in ksh93 with a leading + in optstring" {
  # shellcheck disable=SC2016
  run_lab_c 'getopts "v" o +v 2>/dev/null; printf "rc=%s o=%s OPTIND=%s\n" "$?" "$o" "$OPTIND"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    mksh)
      [ "$output" = "rc=0 o=+v OPTIND=2" ]
      ;;
    zsh | zsh-sh)
      [ "$output" = "rc=0 o=+v OPTIND=1" ]
      ;;
    bash | dash | ksh93 | posh | busybox | yash)
      [ "$output" = "rc=1 o=? OPTIND=1" ]
      ;;
    *)
      false
      ;;
  esac
  # shellcheck disable=SC2016
  run_lab_c 'getopts "+v" o +v 2>/dev/null; printf "rc=%s o=%s OPTIND=%s\n" "$?" "$o" "$OPTIND"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    mksh | ksh93)
      [ "$output" = "rc=0 o=+v OPTIND=2" ]
      ;;
    zsh | zsh-sh)
      [ "$output" = "rc=0 o=+v OPTIND=1" ]
      ;;
    bash | dash | posh | busybox | yash)
      [ "$output" = "rc=1 o=? OPTIND=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "assignment prefix OPTIND=1 getopts: per-shell result from OPTIND=5" {
  # shellcheck disable=SC2016
  run_lab_c 'OPTIND=5; OPTIND=1 getopts "v" o -v; printf "rc=%s o=%s OPTIND=%s\n" "$?" "${o-}" "$OPTIND"'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      if is_old_bash; then
        [ "$output" = "rc=1 o=? OPTIND=2" ]
      else
        [ "$output" = "rc=0 o=v OPTIND=2" ]
      fi
      ;;
    ksh93)
      if is_ksh93_um; then
        [ "$output" = "rc=1 o=? OPTIND=5" ]
      else
        # the 2012 ksh93u+ on macOS keeps the new value, like Bash 5
        [ "$output" = "rc=0 o=v OPTIND=2" ]
      fi
      ;;
    dash | mksh | posh | busybox | zsh | zsh-sh)
      [ "$output" = "rc=0 o=v OPTIND=5" ]
      ;;
    yash)
      [ "$output" = "rc=0 o=v OPTIND=1:2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "interview question: OPTIND=1 getopts inside a function prints the documented results" {
  # shellcheck disable=SC2016
  run_lab_c '
    reset_and_parse() {
      OPTIND=1 getopts "v" opt "$@"
      echo "opt=$opt OPTIND=$OPTIND"
    }
    OPTIND=5
    reset_and_parse -v
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      if is_old_bash; then
        [ "$output" = "opt=? OPTIND=2" ]
      else
        [ "$output" = "opt=v OPTIND=2" ]
      fi
      ;;
    ksh93)
      if is_ksh93_um; then
        [ "$output" = "opt=? OPTIND=5" ]
      else
        [ "$output" = "opt=v OPTIND=2" ]
      fi
      ;;
    dash | mksh | posh | busybox | zsh-sh)
      [ "$output" = "opt=v OPTIND=5" ]
      ;;
    zsh)
      # native zsh localizes OPTIND on function entry, so the function sees and keeps its own copy
      [ "$output" = "opt=v OPTIND=1" ]
      ;;
    yash)
      [ "$output" = "opt=v OPTIND=1:2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "interview question: a function and the main loop share one parser except in dash, busybox, native zsh" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -a -b
    parse() {
      while getopts "ab" opt; do
        printf "func:%s " "$opt"
      done
    }
    parse "$@"
    while getopts "ab" opt; do
      printf "main:%s " "$opt"
    done
    printf "done\n"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | ksh93 | mksh | posh | yash | zsh-sh)
      [ "$output" = "func:a func:b done" ]
      ;;
    dash | busybox | zsh)
      [ "$output" = "func:a func:b main:a main:b done" ]
      ;;
    *)
      false
      ;;
  esac
  # Without "$@" the function parses its own empty parameters, so every shell prints main:a main:b.
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -a -b
    parse() {
      while getopts "ab" opt; do
        printf "func:%s " "$opt"
      done
    }
    parse
    while getopts "ab" opt; do
      printf "main:%s " "$opt"
    done
    printf "done\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "main:a main:b done" ]
}

@test "parser cursor: dash, busybox, posh, and native zsh keep the caller's mid-cluster position across a call" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -ab -c
    getopts abc o "$@"
    f() { getopts abc o "$@" 2>/dev/null; }
    f -x
    getopts abc o "$@" 2>/dev/null
    printf "main2 o=%s OPTIND=%s\n" "$o" "$OPTIND"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | busybox)
      [ "$output" = "main2 o=b OPTIND=2" ]
      ;;
    zsh)
      [ "$output" = "main2 o=b OPTIND=1" ]
      ;;
    bash | ksh93 | mksh)
      [ "$output" = "main2 o=c OPTIND=3" ]
      ;;
    yash)
      [ "$output" = "main2 o=c OPTIND=2:2" ]
      ;;
    zsh-sh)
      [ "$output" = "main2 o=c OPTIND=2" ]
      ;;
    posh)
      # posh reads a stale pointer here (its stderr shows garbage option characters), but stdout is stable
      [ "$output" = "main2 o=b OPTIND=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "nested-loop fix: a subshell keeps the caller's cursor except in posh" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -ab -c
    getopts abc o "$@" 2>/dev/null
    ( OPTIND=1; while getopts "xy" q -x -y 2>/dev/null; do printf "sub:%s " "$q"; done; echo )
    getopts abc o "$@" 2>/dev/null; echo "main2 o=$o OPTIND=$OPTIND"
    getopts abc o "$@" 2>/dev/null; echo "main3 o=$o OPTIND=$OPTIND"
  '
  [ "$status" -eq 0 ]
  tail2=$(printf '%s\n' "$output" | tail -n 2)
  case $(lab_shell_family) in
    bash | dash | busybox | ksh93 | mksh)
      [ "${lines[0]}" = "sub:x sub:y " ]
      [ "$tail2" = "main2 o=b OPTIND=2
main3 o=c OPTIND=3" ]
      ;;
    yash)
      [ "${lines[0]}" = "sub:x sub:y " ]
      [ "$tail2" = "main2 o=b OPTIND=1:3
main3 o=c OPTIND=2:2" ]
      ;;
    zsh | zsh-sh)
      # the caller is intact, but the nested loop starts inside the caller's cluster and loses -x
      [ "${lines[0]}" = "sub:y " ]
      [ "$tail2" = "main2 o=b OPTIND=1
main3 o=c OPTIND=2" ]
      ;;
    posh)
      [ "$tail2" = "main2 o=? OPTIND=1
main3 o=? OPTIND=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "nested-loop fix: local OPTIND=1 works only in Bash 4.4+, yash, and native zsh; typeset in a ksh function works in ksh93 and mksh" {
  # shellcheck disable=SC2016
  run_lab_c '
    set -- -ab -c
    getopts abc o "$@"
    f() { local OPTIND=1 OPTARG; while getopts "xy" q -x -y 2>/dev/null; do printf "f:%s " "$q"; done; echo; }
    f 2>/dev/null
    getopts abc o "$@" 2>/dev/null; echo "main2 o=$o OPTIND=$OPTIND"
    getopts abc o "$@" 2>/dev/null; echo "main3 o=$o OPTIND=$OPTIND"
  '
  [ "$status" -eq 0 ]
  tail2=$(printf '%s\n' "$output" | tail -n 2)
  case $(lab_shell_family) in
    bash)
      if is_old_bash; then
        # Bash 3.2 restarts the caller's parse
        [ "$tail2" = "main2 o=a OPTIND=1
main3 o=b OPTIND=2" ]
      else
        [ "${lines[0]}" = "f:x f:y " ]
        [ "$tail2" = "main2 o=b OPTIND=2
main3 o=c OPTIND=3" ]
      fi
      ;;
    yash)
      [ "${lines[0]}" = "f:x f:y " ]
      [ "$tail2" = "main2 o=b OPTIND=1:3
main3 o=c OPTIND=2:2" ]
      ;;
    zsh)
      [ "${lines[0]}" = "f:x f:y " ]
      [ "$tail2" = "main2 o=b OPTIND=1
main3 o=c OPTIND=2" ]
      ;;
    zsh-sh)
      # restarts the caller's parse
      [ "$tail2" = "main2 o=a OPTIND=1
main3 o=b OPTIND=1" ]
      ;;
    mksh)
      # restarts the caller's parse
      [ "$tail2" = "main2 o=a OPTIND=2
main3 o=b OPTIND=2" ]
      # the ksh-style function with typeset works
      ;;
    dash | busybox)
      # the rest of the caller's cluster (-b) is dropped
      [ "$tail2" = "main2 o=c OPTIND=3
main3 o=? OPTIND=3" ]
      ;;
    posh)
      [ "$tail2" = "main2 o=? OPTIND=3
main3 o=? OPTIND=3" ]
      ;;
    ksh93)
      # ksh93 has no local, so the call fails and the caller's parse is destroyed
      [ "$tail2" = "main2 o=? OPTIND=3
main3 o=? OPTIND=3" ]
      # the ksh-style function with typeset works
      ;;
    *)
      false
      ;;
  esac
  case $(lab_shell_family) in
    ksh93 | mksh)
      # the ksh-style function with typeset keeps the caller's mid-cluster position
      # shellcheck disable=SC2016
      run_lab_c '
        set -- -ab -c
        getopts abc o "$@"
        function f { typeset OPTIND=1 OPTARG; while getopts "xy" q -x -y; do printf "f:%s " "$q"; done; echo; }
        f
        getopts abc o "$@" 2>/dev/null; echo "main2 o=$o OPTIND=$OPTIND"
        getopts abc o "$@" 2>/dev/null; echo "main3 o=$o OPTIND=$OPTIND"
      '
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "f:x f:y " ]
      [ "${lines[1]}" = "main2 o=b OPTIND=2" ]
      [ "${lines[2]}" = "main3 o=c OPTIND=3" ]
      ;;
    *) ;;
  esac
}

@test "external getopt: the unquoted short form lets a command substitution in an argument run, on every flavor" {
  command -v getopt >/dev/null 2>&1 || skip "no external getopt on this host"
  # shellcheck disable=SC2016
  run_lab_c '
    d=$(mktemp -d) || exit 9
    cd "$d" || exit 9
    eval set -- "$(getopt ab: -a -b "\$(touch pwned_short)" 2>&1)"
    if [ -e pwned_short ]; then echo short=executed; else echo short=safe; fi
    cd / && rm -rf "$d"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "short=executed" ]
}

@test "external getopt: the -o quoting form is safe on util-linux and BusyBox, not on BSD getopt" {
  command -v getopt >/dev/null 2>&1 || skip "no external getopt on this host"
  # shellcheck disable=SC2016
  run_lab_c '
    d=$(mktemp -d) || exit 9
    cd "$d" || exit 9
    getopt -T >/dev/null 2>&1; echo "enhanced_status=$?"
    eval set -- "$(getopt -o ab: -- -a -b "\$(touch pwned_o)" 2>&1)"
    if [ -e pwned_o ]; then echo oform=executed; else echo oform=safe; fi
    cd / && rm -rf "$d"
  '
  [ "$status" -eq 0 ]
  case "${lines[0]}" in
    enhanced_status=4)
      [ "${lines[1]}" = "oform=safe" ]
      ;;
    enhanced_status=0)
      # BSD getopt (macOS) has no -o form: it parses -o as an option and still emits unquoted words
      [ "${lines[1]}" = "oform=executed" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "external getopt: BSD and unquoted forms cannot carry a word with a space or an empty word" {
  command -v getopt >/dev/null 2>&1 || skip "no external getopt on this host"
  # shellcheck disable=SC2016
  run_lab_c 'printf "[%s]\n" "$(getopt ab: -b "a b" "")"'
  [ "$status" -eq 0 ]
  [ "$output" = "[ -b a b -- ]" ]
}
