#!/usr/bin/env bats
# Escape sequences: ECMA-48 and xterm.
# The shell under test (LAB_SHELL) emits and strips sequences; parser_probe is a model of the
# terminal that splits a byte stream; pty_probe plays the terminal end of a real pseudo-terminal.

setup() {
  load ../lib/lab
  PROBES="$BATS_TEST_DIRNAME/esc-probes.sh"
  CLIENT="$BATS_TEST_DIRNAME/term-client.sh"
  PARSER="$BATS_TEST_TMPDIR/parser_probe"
  PTY="$BATS_TEST_TMPDIR/pty_probe"
  ESC=$(printf '\033')
  BEL=$(printf '\007')
  CAN=$(printf '\030')
  cd "$BATS_TEST_TMPDIR" || return 1
}

# build_parser: compile the terminal model; a missing compiler fails the test.
build_parser() {
  "${CC:-cc}" -Wall -Wextra -Werror -pedantic -std=c99 -o "$PARSER" "$BATS_TEST_DIRNAME/parser_probe.c" || return 1
}

# build_pty: compile the pseudo-terminal harness.
build_pty() {
  "${CC:-cc}" -Wall -Wextra -Werror -pedantic -o "$PTY" "$BATS_TEST_DIRNAME/pty_probe.c" || return 1
}

# field KEY: print the value of the line "KEY=value" in $output.
field() {
  local line
  while IFS= read -r line; do
    case $line in
      "$1="*)
        printf '%s\n' "${line#*=}"
        return 0
        ;;
      *) ;;
    esac
  done <<<"$output"
  return 1
}

# parse: feed the bytes of $1 to the terminal model and leave its events in $output.
parse() {
  build_parser
  printf '%s' "$1" >"$BATS_TEST_TMPDIR/stream"
  run "$PARSER" <"$BATS_TEST_TMPDIR/stream"
  [ "$status" -eq 0 ]
}

# dash_lineage: print which dash the shell under test is, so each dash test can assert one exact value.
#   upstream-old   upstream dash up to 0.5.12 (Alpine 0.5.12-r*) and Apple's /bin/dash: no \e, no \x, no $'..'
#   debian-patch   Debian 0.5.12-12 and Ubuntu 24.04 0.5.12-6ubuntu5, with patch 0008: \e only
#   upstream-new   dash 0.5.13 and later: \e, \x and $'..'
# The version comes from LAB_DASH_VERSION when set (a hand-built dash, for example 0.5.13.5, or "apple"),
# otherwise from the package manager that owns the system dash. An unknown dash fails the test
# instead of being accepted with any outcome.
dash_lineage() {
  local word path version=${LAB_DASH_VERSION-}
  lab_shell_words
  if [ -z "$version" ]; then
    word=${LAB_SHELL_WORDS[0]}
    path=$(command -v "$word") || return 1
    path=$(readlink -f "$path" 2>/dev/null || printf '%s' "$path")
    case $(uname -s):$path in
      Darwin:/bin/dash) version=apple ;;
      Linux:/usr/bin/dash | Linux:/bin/dash)
        if command -v dpkg-query >/dev/null 2>&1; then
          version=$(dpkg-query -W -f='${Version}' dash)
        elif command -v apk >/dev/null 2>&1; then
          version=$(apk list -I dash 2>/dev/null)
          version=${version#dash-}
          version=${version%% *}
        fi
        ;;
      *) ;;
    esac
  fi
  case $version in
    apple | 0.5.12 | 0.5.12-r*) printf 'upstream-old\n' ;;
    0.5.12-6ubuntu5 | 0.5.12-12*) printf 'debian-patch\n' ;;
    0.5.13* | 0.5.1[4-9]*) printf 'upstream-new\n' ;;
    *)
      printf 'unknown dash build "%s": set LAB_DASH_VERSION\n' "$version" >&2
      return 1
      ;;
  esac
}

@test "the octal spelling of ESC works in every shell and produces byte 0x1b" {
  run_lab "$PROBES" forms
  [ "$status" -eq 0 ]
  v=$(field octal)
  [ "$v" = "1b5a" ]
}

@test "backslash-e in a printf format is shell specific" {
  run_lab "$PROBES" forms
  [ "$status" -eq 0 ]
  v=$(field backslash-e)
  case $(lab_shell_family) in
    yash) [ "$v" = "5c655a" ] ;;
    dash)
      # Three dash lineages: upstream up to 0.5.12 and Apple's dash print the two characters,
      # Debian's 0.5.12 patch 0008 adds the escape, and upstream 0.5.13 and later have it.
      case $(dash_lineage) in
        upstream-old) [ "$v" = "5c655a" ] ;;
        debian-patch | upstream-new) [ "$v" = "1b5a" ] ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox) [ "$v" = "1b5a" ] ;;
    *) false ;;
  esac
}

@test "hex escape in a printf format is shell specific" {
  run_lab "$PROBES" forms
  [ "$status" -eq 0 ]
  v=$(field hex-escape)
  case $(lab_shell_family) in
    yash) [ "$v" = "5c7831625a" ] ;;
    dash)
      # dash gained \x together with dollar-single-quote in 0.5.13; Debian's patch adds only \e.
      case $(dash_lineage) in
        upstream-old | debian-patch) [ "$v" = "5c7831625a" ] ;;
        upstream-new) [ "$v" = "1b5a" ] ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | posh | busybox) [ "$v" = "1b5a" ] ;;
    *) false ;;
  esac
}

@test "dollar-single-quote yields ESC where the shell supports it and a literal elsewhere" {
  run_lab "$PROBES" forms
  [ "$status" -eq 0 ]
  v=$(field dollar-quote)
  case $(lab_shell_family) in
    posh) [ "$v" = "245c3033335a" ] ;;
    dash)
      # 0.5.13 and later implement it; 0.5.12 (upstream, Debian, Apple) and earlier do not.
      case $(dash_lineage) in
        upstream-old | debian-patch) [ "$v" = "245c3033335a" ] ;;
        upstream-new) [ "$v" = "1b5a" ] ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox) [ "$v" = "1b5a" ] ;;
    *) false ;;
  esac
}

@test "echo -e is not portable: it is printed or interpreted depending on the shell" {
  run_lab "$PROBES" forms
  [ "$status" -eq 0 ]
  v=$(field echo-e)
  case $(lab_shell_family) in
    dash | posh | yash) [ "$v" = "2d65201b5a0a" ] ;;
    zsh | zsh-sh | ksh93 | mksh | busybox) [ "$v" = "1b5a0a" ] ;;
    bash)
      # Apple's bash 3.2 in POSIX mode (macOS /bin/sh) behaves like XSI echo: it interprets
      # backslash sequences without -e and so prints "-e " as an operand. Detect that by behavior.
      # shellcheck disable=SC2016 # the quoted text belongs to the shell under test
      run_lab_c 'echo "a\tb"'
      [ "$status" -eq 0 ]
      if [ "$output" = "$(printf 'a\tb')" ]; then
        [ "$v" = "2d65201b5a0a" ]
      else
        [ "$v" = "1b5a0a" ]
      fi
      ;;
    *) false ;;
  esac
}

@test "SGR sequences keep their parameter string for 16, 256, true and colon forms" {
  run_lab "$PROBES" sgr 16
  [ "$status" -eq 0 ]
  parse "$output"
  case $output in
    *'CSI params=1;31;44 inter= final=m'*'TEXT "x"'*'CSI params=0 inter= final=m'*) ;;
    *) false ;;
  esac
  run_lab "$PROBES" sgr 256
  parse "$output"
  case $output in
    *'CSI params=38;5;196;48;5;232 inter= final=m'*) ;;
    *) false ;;
  esac
  run_lab "$PROBES" sgr true
  parse "$output"
  case $output in
    *'CSI params=38;2;255;128;0 inter= final=m'*) ;;
    *) false ;;
  esac
  run_lab "$PROBES" sgr colon
  parse "$output"
  case $output in
    *'CSI params=38:2::255:128:0 inter= final=m'*) ;;
    *) false ;;
  esac
}

@test "CUP carries two parameters and an omitted parameter stays empty" {
  run_lab "$PROBES" cup 14 62
  [ "$status" -eq 0 ]
  parse "$output"
  [ "$output" = "CSI params=14;62 inter= final=H" ]
  parse "${ESC}[;5H"
  [ "$output" = "CSI params=;5 inter= final=H" ]
  parse "${ESC}[H"
  [ "$output" = "CSI params= inter= final=H" ]
}

@test "DEC private modes are CSI with a question mark parameter and h or l" {
  run_lab "$PROBES" dec set 1049
  parse "$output"
  [ "$output" = "CSI params=?1049 inter= final=h" ]
  run_lab "$PROBES" dec reset 25
  parse "$output"
  [ "$output" = "CSI params=?25 inter= final=l" ]
  run_lab "$PROBES" dec set 2004
  parse "$output"
  [ "$output" = "CSI params=?2004 inter= final=h" ]
}

@test "OSC strings end at BEL or at ST and the terminator is reported" {
  run_lab "$PROBES" title 'build 42' bel
  parse "$output"
  [ "$output" = 'OSC cmd=2 data="build 42" term=BEL' ]
  run_lab "$PROBES" title 'build 42' st
  parse "$output"
  [ "$output" = 'OSC cmd=2 data="build 42" term=ST' ]
}

@test "a C0 control is executed in the middle of a CSI and the sequence still completes" {
  parse "${ESC}[3"$'\n'"1m"
  [ "$output" = "$(printf 'C0 0x0A\nCSI params=31 inter= final=m')" ]
}

@test "CAN aborts a sequence in progress and the text after it is plain text" {
  parse "${ESC}[1;3${CAN}ok"
  [ "$output" = "$(printf 'CANCEL 0x18\nTEXT "ok"')" ]
}

@test "ESC inside a sequence abandons it and starts a new one" {
  parse "${ESC}[3${ESC}[0m1m"
  [ "$output" = "$(printf 'RESTART\nCSI params=0 inter= final=m\nTEXT "1m"')" ]
}

@test "input that ends inside a sequence is reported as incomplete" {
  parse "${ESC}[12"
  [ "$output" = "INCOMPLETE state=csi" ]
  parse "${ESC}]2;title"
  [ "$output" = "INCOMPLETE state=osc" ]
}

@test "sed strips complete CSI and OSC sequences and leaves the text" {
  payload="${ESC}[1;32mOK${ESC}[0m ${ESC}[10;20H${ESC}]2;t${BEL}done ${ESC}]8;;http://x${ESC}\\link${ESC}]8;;${ESC}\\"
  printf '%s\n' "$payload" >"$BATS_TEST_TMPDIR/in"
  run_lab "$PROBES" strip <"$BATS_TEST_TMPDIR/in"
  [ "$status" -eq 0 ]
  [ "$output" = "OK done link" ]
}

@test "a single sed pass lets a nested sequence reassemble while the terminal sees something else" {
  nested="${ESC}[3${ESC}[0m1m"
  printf '%s\n' "$nested" >"$BATS_TEST_TMPDIR/in"
  run_lab "$PROBES" strip <"$BATS_TEST_TMPDIR/in"
  [ "$status" -eq 0 ]
  # The filter removed the inner SGR 0 and left a valid SGR 31 behind.
  [ "$output" = "${ESC}[31m" ]
  # The terminal never sees SGR 31: it abandons "ESC [ 3" at the second ESC.
  parse "$nested"
  case $output in
    *'CSI params=31'*) false ;;
    *'CSI params=0 inter= final=m'*) ;;
    *) false ;;
  esac
}

@test "an unescaped colour prompt is 9 bytes wider than it looks" {
  run_lab "$PROBES" width
  [ "$status" -eq 0 ]
  [ "$output" = "bytes=20 visible=11 phantom=9" ]
}

@test "the line discipline echoes ESC as caret-bracket only while ECHOCTL is set" {
  build_pty
  run "$PTY" echo-ctl on
  [ "$status" -eq 0 ]
  [ "$output" = 'echo=a^[[A\r\n' ]
  run "$PTY" echo-ctl off
  [ "$status" -eq 0 ]
  [ "$output" = 'echo=a<ESC>[A\r\n' ]
}

@test "a cursor position report arrives on stdin like typed input and a raw-mode reader gets it" {
  build_pty
  lab_shell_words
  run "$PTY" term 7 42 "${LAB_SHELL_WORDS[@]}" "$CLIENT" cpr-raw
  [ "$status" -eq 0 ]
  case $output in
    *'transcript=<ESC>[6n'*'row=7 col=42\r\n'*'child_status=0'*) ;;
    *) false ;;
  esac
  run "$PTY" term 31 99 "${LAB_SHELL_WORDS[@]}" "$CLIENT" cpr-raw
  case $output in
    *'row=31 col=99\r\n'*) ;;
    *) false ;;
  esac
}

@test "the same report read by nobody is echoed onto the screen as caret-bracket text" {
  build_pty
  lab_shell_words
  run "$PTY" term 7 42 "${LAB_SHELL_WORDS[@]}" "$CLIENT" cpr-cooked
  [ "$status" -eq 0 ]
  case $output in
    *'transcript=<ESC>[6n^[[7;42R'*'child_status=0'*) ;;
    *) false ;;
  esac
}

@test "a title report returns attacker chosen bytes to the program's input" {
  build_pty
  lab_shell_words
  run "$PTY" term 1 1 "${LAB_SHELL_WORDS[@]}" "$CLIENT" title
  [ "$status" -eq 0 ]
  # ESC ] l  i d > p w n e d  ESC \
  case $output in
    *'reply-hex= 1b 5d 6c 69 64 3e 70 77 6e 65 64 1b 5c'*) ;;
    *) false ;;
  esac
}
