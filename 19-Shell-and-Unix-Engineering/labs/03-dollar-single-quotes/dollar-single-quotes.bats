#!/usr/bin/env bats
# Tests for POSIX.1-2024 dollar-single-quotes ($'...'), escape sequence decoding,
# quoting semantics, null-byte handling, and shell divergences.
# shellcheck disable=SC2016 # Literal dollar expressions being asserted

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/dollar-probe.sh"
  BYTE_DUMP="$BATS_TEST_TMPDIR/byte-dump"
  "${CC:-cc}" -Wall -Wextra -Werror -o "$BYTE_DUMP" "$BATS_TEST_DIRNAME/byte-dump.c"
}

@test "basic escape sequences decode tab and newline in conforming shells" {
  run_lab "$PROBE" basic-escapes
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = 'data=<hello	world' ]
      [ "${lines[1]}" = '>' ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = 'data=<$hello\tworld\n>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "escaped single quote within dollar-single-quote preserves quote" {
  run_lab "$PROBE" single-quote
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = "<it's>" ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      case $output in
        *"Syntax error: Unterminated quoted string"*) true ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$status" -ne 0 ]
      case $output in
        *"no closing quote"*) true ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "expansions and globs are disabled inside dollar-single-quotes" {
  run_lab "$PROBE" no-expansion
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = '<x=$x cmd=$(echo fail) arith=$((1+1)) glob=*>' ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = '<$x=$x cmd=$(echo fail) arith=$((1+1)) glob=*>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "evaluated dollar-single-quote produces a single quoted field" {
  run_lab "$PROBE" field-splitting
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = '2 args: <one two	three' ]
      [ "${lines[1]}" = 'four> <*>' ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = '2 args: <$one two\tthree\nfour> <$*>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "dollar-single-quote inside double quotes remains literal" {
  run_lab "$PROBE" double-quoted
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "<$'literal\\nnewline'>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "hex escape digit consumption diverges between bash-zsh and ksh93" {
  run_lab "$PROBE" hex-limit "$BYTE_DUMP"
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=3 hex= 41 34 32" ]
      ;;
    ksh93 | mksh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=3 hex= e4 85 82" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=7 hex= 24 5c 78 34 31 34 32" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "control character escape handling diverges between shell families" {
  run_lab "$PROBE" control-chars "$BYTE_DUMP"
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*)
          [ "$status" -eq 0 ]
          [ "$output" = "stdin: count=2 hex= 01 1f" ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "stdin: count=2 hex= 01 7f" ]
          ;;
      esac
      ;;
    ksh93 | yash | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=2 hex= 01 7f" ]
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=6 hex= 5c 63 61 5c 63 3f" ]
      ;;
    zsh | zsh-sh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=4 hex= 63 61 63 3f" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=7 hex= 24 5c 63 61 5c 63 3f" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "builtin handling of embedded null byte diverges between zsh and standard shells" {
  run_lab "$PROBE" null-byte-builtin "$BYTE_DUMP"
  case $(lab_shell_family) in
    bash | zsh-sh | ksh93 | yash | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=3 hex= 61 62 64" ]
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=4 hex= 61 62 63 64" ]
      ;;
    zsh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=5 hex= 61 62 00 63 64" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=7 hex= 61 24 62 5c 30 63 64" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "execve boundary truncates embedded null byte in zsh" {
  run_lab "$PROBE" null-byte-exec "$BYTE_DUMP"
  case $(lab_shell_family) in
    bash | zsh-sh | ksh93 | yash | mksh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=3 hex= 61 62 64" ]
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=4 hex= 61 62 63 64" ]
      ;;
    zsh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=2 hex= 61 62" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "argc=2" ]
      [ "${lines[1]}" = "argv[1]: len=7 hex= 61 24 62 5c 30 63 64" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "setting IFS to newline via dollar-single-quote splits correctly except in legacy sh" {
  run_lab "$PROBE" ifs-newline
  case $(lab_shell_family) in
    bash | zsh-sh | ksh93 | yash | mksh | busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "count=2: <line1 line2> <line3>" ]
      ;;
    zsh)
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "count=1: <line1 line2" ]
      [ "${lines[1]}" = "line3>" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "count=4: <li> <e1 li>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "parameter expansion default with dollar-single-quote inside double quotes diverges" {
  # shellcheck disable=SC2016 # expressions run under shell under test
  run_lab_c 'unset v; printf "<%s>\n" "${v:-$'"'default\\nval'"'}"'
  local test_status=$status
  local test_output=$output
  local line0="${lines[0]-}"
  local line1="${lines[1]-}"
  case $(lab_shell_family) in
    bash)
      [ "$test_status" -eq 0 ]
      case $(lab_shell_version) in
        3.*)
          # bash 3.2 expands in both native and POSIX mode
          [ "$line0" = "<default" ]
          [ "$line1" = "val>" ]
          ;;
        *)
          # bash >= 4.4 expands by default, but stays literal under POSIX mode
          run_lab_c '[ -o posix ]'
          if [ "$status" -eq 0 ]; then
            [ "$test_output" = "<$'default\\nval'>" ]
          else
            [ "$line0" = "<default" ]
            [ "$line1" = "val>" ]
          fi
          ;;
      esac
      ;;
    mksh)
      [ "$test_status" -eq 0 ]
      [ "$line0" = "<default" ]
      [ "$line1" = "val>" ]
      ;;
    posh)
      [ "$test_status" -eq 0 ]
      [ "$test_output" = "<$'defaultnval'>" ]
      ;;
    zsh | zsh-sh | ksh93 | yash | busybox | dash)
      [ "$test_status" -eq 0 ]
      [ "$test_output" = "<$'default\\nval'>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unicode escape support diverges between bash 3.2 and modern shells" {
  # \u decoding depends on the locale, and the Debian image sets none; C.UTF-8 exists
  # on the macOS host, Alpine (musl), and Debian, so name it directly.
  local loc=C.UTF-8
  # shellcheck disable=SC2016 # expressions run under shell under test
  LC_ALL="$loc" run_lab_c 'printf "%s" $'"'\\u03b1'"' | "$1" -s' _ "$BYTE_DUMP"
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*)
          [ "$status" -eq 0 ]
          [ "$output" = "stdin: count=6 hex= 5c 75 30 33 62 31" ]
          ;;
        *)
          [ "$status" -eq 0 ]
          [ "$output" = "stdin: count=2 hex= ce b1" ]
          ;;
      esac
      ;;
    zsh | zsh-sh | ksh93 | mksh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=2 hex= ce b1" ]
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=6 hex= 5c 75 30 33 62 31" ]
      ;;
    yash)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=6 hex= 3f 75 30 33 62 31" ]
      ;;
    dash | posh)
      [ "$status" -eq 0 ]
      [ "$output" = "stdin: count=7 hex= 24 5c 75 30 33 62 31" ]
      ;;
    *)
      false
      ;;
  esac
}
