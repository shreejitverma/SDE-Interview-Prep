#!/usr/bin/env bats
# Tests for quote removal (POSIX.1-2024 XCU 2.6.7): which quote characters go, which stay, and when.
# shellcheck disable=SC2016 # expected values contain literal $ and backquote characters

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/quote-removal-probe.sh"
  export TMPDIR="$BATS_TEST_TMPDIR"
  NL=$'\n'
}

# Native zsh neither field splits nor globs the result of an unquoted expansion.
no_expansion_splitting() {
  local family
  family=$(lab_shell_family)
  [ "$family" = zsh ]
}

@test "the quote characters of the original word are removed and the pieces form one field" {
  run_lab "$PROBE" concat
  [ "$status" -eq 0 ]
  [ "$output" = "1:<abcd e>" ]
}

@test "an external command receives the argument with the quotes already gone" {
  run_lab_c "/usr/bin/env printf '<%s>' 'a'\"b\"\\c'd e' ''"
  [ "$status" -eq 0 ]
  [ "$output" = "<abcd e><>" ]
}

@test "quote characters that come out of an expansion are data and are never rescanned" {
  run_lab "$PROBE" norescan
  [ "$status" -eq 0 ]
  [ "${lines[1]}" = "1:<\"a\" 'b' \\c>" ]
  [ "${lines[2]}" = "1:<\"a\" 'b' \\c>" ]
  [ "${lines[3]}" = "3:<\"a\"><'b'><\\c>" ] # zsh splits an unquoted command substitution, unlike an unquoted parameter
  [ "${lines[4]}" = "len=10" ]
  if no_expansion_splitting; then
    [ "${lines[0]}" = "1:<\"a\" 'b' \\c>" ]
  else
    [ "${lines[0]}" = "3:<\"a\"><'b'><\\c>" ]
  fi
}

@test "in double quotes a backslash is removed only before dollar, backquote, double quote and backslash" {
  run_lab "$PROBE" dq-backslash
  [ "$status" -eq 0 ]
  [ "$output" = "6:<\$><\\a><\\><\"><\`><\\'>" ]
}

@test "outside quotes a backslash is removed before every character" {
  run_lab "$PROBE" bare-backslash
  [ "$status" -eq 0 ]
  [ "$output" = "6:<a><\\><'><\"><\$><\`>" ]
}

@test "a quote character that is itself quoted survives" {
  run_lab "$PROBE" quoted-quotes
  [ "$status" -eq 0 ]
  [ "$output" = "6:<'><\"><'><\"><it's><say \"hi\">" ]
}

@test "a quoted null word stays as an empty field and an unquoted null expansion vanishes" {
  run_lab "$PROBE" null-words
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1:<>" ]
  [ "${lines[1]}" = "0:" ]
  [ "${lines[2]}" = "1:<>" ]
  [ "${lines[3]}" = "1:<>" ]
  [ "${lines[4]}" = "3:<><><x>" ]
}

@test "field splitting runs before quote removal so only the unquoted part is split" {
  run_lab "$PROBE" split
  [ "$status" -eq 0 ]
  if no_expansion_splitting; then
    [ "${lines[0]}" = "1:<a bc d>" ]
    [ "${lines[1]}" = "1:<a ba b>" ]
    [ "${lines[2]}" = "1:<a b>" ]
  else
    [ "${lines[0]}" = "2:<a><bc d>" ]
    [ "${lines[1]}" = "2:<a ba><b>" ]
    [ "${lines[2]}" = "2:<a><b>" ]
  fi
}

@test "pathname expansion runs before quote removal so a quoted metacharacter matches only itself" {
  run_lab "$PROBE" glob
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "3:<a*><ab><ac>" ]
  [ "${lines[1]}" = "1:<a?>" ]
  [ "${lines[2]}" = "1:<a?>" ]
  [ "${lines[3]}" = "3:<a*><ab><ac>" ]
  [ "${lines[5]}" = "1:<a?>" ]
  [ "${lines[6]}" = "3:<a*><ab><ac>" ]
  [ "${lines[7]}" = "1:<a*>" ]
  if no_expansion_splitting; then
    [ "${lines[4]}" = "1:<a?>" ]
  else
    [ "${lines[4]}" = "3:<a*><ab><ac>" ]
  fi
}

@test "a case pattern remembers which of its characters were quoted" {
  run_lab "$PROBE" case
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "subject abc: bare=yes dq=no bs=no sq=no mid=no" ]
  [ "${lines[1]}" = "subject a*: dq=yes bs=yes" ]
  if no_expansion_splitting; then
    [ "${lines[2]}" = "pattern from variable: unquoted=no quoted=no" ]
  else
    [ "${lines[2]}" = "pattern from variable: unquoted=yes quoted=no" ]
  fi
}

@test "the pattern of a parameter expansion keeps its quoted characters literal" {
  run_lab "$PROBE" param-pattern
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "<><abc><abc><abc>" ]
  case $(lab_shell_family) in
    posh) [ "${lines[1]}" = "inside double quotes: <abc><abc>" ] ;; # posh stops treating the rest of the pattern as a pattern
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93 | dash)
      [ "${lines[1]}" = "inside double quotes: <><>" ]
      ;;
    *) false ;;
  esac
}

@test "the word of a default value gets quote removal and double quotes keep single quotes literal" {
  run_lab "$PROBE" default-word
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1:<a b>" ]
  [ "${lines[1]}" = "1:<a b>" ]
  [ "${lines[2]}" = "1:<a  b>" ]
  [ "${lines[3]}" = "1:<'a b'>" ]
  [ "${lines[5]}" = "1:<a b>" ]
}

@test "the word of := is stored after quote removal and is then split like any other value" {
  run_lab "$PROBE" default-word
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh) [ "${lines[4]}" = "1:<a b>" ] ;; # native zsh does not split
    bash)
      case $(lab_shell_version) in
        3.*) [ "${lines[4]}" = "1:<a b>" ] ;; # bash 3.2 substitutes the stored value as if quoted
        4.* | 5.*) [ "${lines[4]}" = "2:<a><b>" ] ;;
        *) false ;;
      esac
      ;;
    zsh-sh | yash | busybox | posh | mksh | ksh93 | dash) [ "${lines[4]}" = "2:<a><b>" ] ;;
    *) false ;;
  esac
}

@test "a backslash before a closing brace is removed inside a double-quoted expansion but not in a plain double-quoted string" {
  run_lab "$PROBE" brace
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*) [ "$output" = '<\}><\}>' ] ;; # bash 3.2 keeps the backslash
        4.* | 5.*) [ "$output" = '<}><\}>' ] ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash) [ "$output" = '<}><\}>' ] ;;
    *) false ;;
  esac
}

@test "an assignment value gets quote removal" {
  run_lab "$PROBE" assign
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "1:<ab c>" ]
  [ "${lines[1]}" = "1:<>" ]
  [ "${lines[2]}" = "1:<ab cd>" ]
}

@test "the word after a redirection operator gets quote removal and names the file" {
  run_lab "$PROBE" redirect
  [ "$status" -eq 0 ]
  [ "$output" = '<>><a$b><xyz w>' ]
}

@test "a quoted here-document delimiter disables expansion and the body never gets quote removal" {
  run_lab "$PROBE" heredoc
  [ "$status" -eq 0 ]
  verbatim='"q" '"'r'"' \\ \$v $v \x'
  [ "${lines[0]}" = "-- unquoted delimiter" ]
  [ "${lines[1]}" = '"q" '"'r'"' \ $v X \x' ]
  [ "${lines[2]}" = "-- single-quoted delimiter" ]
  [ "${lines[3]}" = "$verbatim" ]
  [ "${lines[4]}" = "-- double-quoted delimiter" ]
  [ "${lines[5]}" = "$verbatim" ]
  [ "${lines[6]}" = "-- backslash delimiter" ]
  [ "${lines[7]}" = "$verbatim" ]
  [ "${lines[8]}" = "-- partly quoted delimiter" ]
  [ "${lines[9]}" = "$verbatim" ]
  [ "${lines[10]}" = "-- delimiter with a space" ]
  [ "${lines[11]}" = '$v' ]
  [ "${#lines[@]}" -eq 12 ]
}

@test "the words of a for loop get quote removal and a quoted null word is an iteration" {
  run_lab "$PROBE" words
  [ "$status" -eq 0 ]
  [ "$output" = "<a b><c><d e><> n=4" ]
}

@test "eval removes quotes a second time and a quote inside a pasted value breaks the program" {
  run_lab "$PROBE" eval
  [ "$status" -eq 0 ]
  if no_expansion_splitting; then
    [ "${lines[0]}" = '1:<"a b">' ]
  else
    [ "${lines[0]}" = '2:<"a><b">' ]
  fi
  [ "${lines[1]}" = "1:<a b>" ]
  case $(lab_shell_family) in
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "${lines[2]}" = "quote in value: syntax error" ] ;;
        *"93u+ 2012"*) [ "${lines[2]}" = "quote in value: parsed arg=<its> kept=no" ] ;; # 2012 ksh93 closes an unterminated quote at the end of the string
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      [ "${lines[2]}" = "quote in value: syntax error" ]
      ;;
    *) false ;;
  esac
}

@test "a value pasted into eval between single quotes injects a command and sh_quote prevents it" {
  run_lab "$PROBE" inject
  [ "$status" -eq 0 ]
  expected="-- value pasted between single quotes${NL}1:<>${NL}INJECTED${NL}${NL}-- value passed through sh_quote${NL}1:<'; echo INJECTED; echo '>"
  [ "$output" = "$expected" ]
}

@test "backslash-newline is deleted before quote removal except inside single quotes" {
  run_lab "$PROBE" backslash-newline
  [ "$status" -eq 0 ]
  [ "$output" = "3:<ab><a\\${NL}b><ab>" ]
}

@test "dollar-single-quotes are a quoting form only where the shell supports them" {
  run_lab "$PROBE" dollar-single
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh) [ "$output" = '1:<$abc>' ] ;; # a literal dollar sign, then a single-quoted word
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93) [ "$output" = "1:<abc>" ] ;;
    *) false ;;
  esac
}

@test "an escaped single quote inside dollar-single-quotes is removed where supported and unterminated elsewhere" {
  run_lab "$PROBE" dollar-single-escape
  case $(lab_shell_family) in
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "$PROBE: 1: eval: Syntax error: Unterminated quoted string" ]
      ;;
    posh)
      [ "$status" -eq 1 ]
      [ "$output" = "$PROBE: no closing quote" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = "1:<a'b>" ]
      ;;
    *) false ;;
  esac
}

@test "each level of sh_quote multiplies the quote count by three and each eval peels exactly one level" {
  run_lab "$PROBE" levels
  [ "$status" -eq 0 ]
  # Seed is the one-quote input. Each later length and quote count is the recurrence,
  # checked against the probe, which measures sh_quote rather than printing these numbers.
  quotes=1
  len=1
  level=0
  lens=()
  while [ "$level" -le 4 ]; do
    [ "${lines[level]}" = "level=$level len=$len quotes=$quotes" ]
    lens[level]=$len
    [ "$quotes" -eq $((2 * (3 ** level) - 1)) ]
    len=$((len + 3 * quotes + 2))
    quotes=$((3 * quotes + 2))
    level=$((level + 1))
  done
  [ "${lines[5]}" = "peel to level=4 len=${lens[4]}" ]
  [ "${lines[6]}" = "peel to level=3 len=${lens[3]}" ]
  [ "${lines[7]}" = "peel to level=2 len=${lens[2]}" ]
  [ "${lines[8]}" = "peel to level=1 len=${lens[1]}" ]
  [ "${lines[9]}" = "peel to level=0 len=${lens[0]}" ]
  [ "${lines[10]}" = "last=<'>" ]
  [ "${#lines[@]}" -eq 11 ]
}

@test "sh_quote output survives one eval byte for byte for hostile values" {
  run_lab "$PROBE" roundtrip
  [ "$status" -eq 0 ]
  expected="ok len=0${NL}ok len=1${NL}ok len=2${NL}ok len=3${NL}ok len=3${NL}ok len=5${NL}ok len=4${NL}ok len=1${NL}ok len=1${NL}ok len=1${NL}ok len=2${NL}ok len=3${NL}ok len=2${NL}ok len=1${NL}ok len=1${NL}ok len=5${NL}ok len=3${NL}ok len=5"
  [ "$output" = "$expected" ]
}

@test "the probe rejects an unknown mode" {
  run_lab "$PROBE" no-such-mode
  [ "$status" -eq 2 ]
  [ "$output" = "usage: quote-removal-probe.sh MODE" ]
}

@test "the program receives the bytes left after quote removal, not the source word" {
  "${CC:-cc}" -Wall -Wextra -Werror -o "$BATS_TEST_TMPDIR/argv-inspect" "$BATS_TEST_DIRNAME/argv-inspect.c"
  run_lab_c "\"$BATS_TEST_TMPDIR/argv-inspect\" 'a'\"b\"\\c 'it'\\''s' \"x y\" ''"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "argc=4" ]
  [ "${lines[1]}" = "arg1 len=3 quotes=0" ]
  [ "${lines[2]}" = "arg2 len=4 quotes=1" ]
  [ "${lines[3]}" = "arg3 len=3 quotes=0" ]
  [ "${lines[4]}" = "arg4 len=0 quotes=0" ]
  [ "${#lines[@]}" -eq 5 ]
}

# write_quote_word_script REPS DEST: script whose one word is REPS copies of 'a'"a"\a.
write_quote_word_script() {
  awk -v reps="$1" 'BEGIN { printf "\"$1\" "; for (i = 0; i < reps; i++) printf "%s", "\047a\047\"a\"\\a"; printf "\n" }' >"$2"
}

@test "a source word larger than the Linux per-argument limit runs because only the surviving bytes reach execve" {
  "${CC:-cc}" -Wall -Wextra -Werror -o "$BATS_TEST_TMPDIR/argv-inspect" "$BATS_TEST_DIRNAME/argv-inspect.c"
  reps=20000
  write_quote_word_script 1 "$BATS_TEST_TMPDIR/one.sh"
  write_quote_word_script "$reps" "$BATS_TEST_TMPDIR/big.sh"
  # The generated script is the five-byte prefix "$1" , the word, and a newline.
  prefix='"$1" '
  overhead=$((${#prefix} + 1))
  one_bytes=$(wc -c <"$BATS_TEST_TMPDIR/one.sh")
  big_bytes=$(wc -c <"$BATS_TEST_TMPDIR/big.sh")
  one_word=$((one_bytes - overhead))
  big_word=$((big_bytes - overhead))
  [ "$one_word" -eq 8 ]
  [ "$big_word" -eq $((one_word * reps)) ]
  [ "$big_word" -eq 160000 ]
  [ "$big_word" -gt 131072 ]
  run_lab "$BATS_TEST_TMPDIR/one.sh" "$BATS_TEST_TMPDIR/argv-inspect"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "argc=1" ]
  [ "${lines[1]}" = "arg1 len=3 quotes=0" ]
  one_delivered=3
  run_lab "$BATS_TEST_TMPDIR/big.sh" "$BATS_TEST_TMPDIR/argv-inspect"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "argc=1" ]
  [ "${lines[1]}" = "arg1 len=$((one_delivered * reps)) quotes=0" ]
  [ $((one_delivered * reps)) -eq 60000 ]
  [ "${#lines[@]}" -eq 2 ]
}

# xargs_input TEXT: feed TEXT as one line to xargs printf from the shell under test.
xargs_input() {
  run_lab_c 'printf "%s\n" "$1" | xargs printf "<%s>"' sh "$1"
}

# xargs_flavor: print gnu, busybox, or bsd for the xargs that the shell under test finds first.
# GNU is findutils 4.10.0 and BusyBox is 1.37.0; another build is "unknown" so the test fails.
xargs_flavor() {
  local help kernel
  help=$(xargs --version 2>&1 || true)
  case "$help" in
    *"GNU findutils"*)
      if [ "${help%%$'\n'*}" = "xargs (GNU findutils) 4.10.0" ]; then
        echo gnu
      else
        echo unknown
      fi
      return 0
      ;;
    *"BusyBox v1.37.0"*)
      echo busybox
      return 0
      ;;
    *) ;;
  esac
  help=$(xargs --help 2>&1 || true)
  case "$help" in
    *"BusyBox v1.37.0"*) echo busybox ;;
    *)
      kernel=$(uname -s)
      if [ "$kernel" = Darwin ]; then
        echo bsd
      else
        echo unknown
      fi
      ;;
  esac
}

@test "xargs runs its own quote removal on its input and its rules differ from the shell's" {
  flavor=$(xargs_flavor)
  xargs_input "'a b'"
  [ "$status" -eq 0 ]
  [ "$output" = "<a b>" ]
  xargs_input '"c d"'
  [ "$status" -eq 0 ]
  [ "$output" = "<c d>" ]
  xargs_input 'e\ f'
  [ "$status" -eq 0 ]
  [ "$output" = "<e f>" ]
  xargs_input "it\\'s"
  [ "$status" -eq 0 ]
  [ "$output" = "<it's>" ]
  xargs_input "''"
  [ "$status" -eq 0 ]
  [ "$output" = "<>" ]
  xargs_input 'a"b c"d'
  [ "$status" -eq 0 ]
  [ "$output" = "<ab cd>" ]
  xargs_input "'say \"hi\"'"
  [ "$status" -eq 0 ]
  [ "$output" = '<say "hi">' ]
  xargs_input '"it'"'"'s"'
  [ "$status" -eq 0 ]
  [ "$output" = "<it's>" ]
  xargs_input 'a\\b'
  [ "$status" -eq 0 ]
  [ "$output" = '<a\b>' ]
  # Inside quotes a backslash is not an escape, so the shell habit of "say \"hi\"" breaks.
  xargs_input '"a\b"'
  [ "$status" -eq 0 ]
  [ "$output" = '<a\b>' ]
  xargs_input '"say \"hi\""'
  [ "$status" -eq 1 ]
  case "$flavor" in
    gnu) [ "$output" = "xargs: unmatched double quote; by default quotes are special to xargs unless you use the -0 option" ] ;;
    busybox) [ "$output" = "xargs: unmatched double quote" ] ;;
    bsd) [ "$output" = "xargs: unterminated quote" ] ;;
    *) false ;;
  esac
  xargs_input "'a\\'b'"
  [ "$status" -eq 1 ]
  case "$flavor" in
    gnu) [ "$output" = "xargs: unmatched single quote; by default quotes are special to xargs unless you use the -0 option" ] ;;
    busybox) [ "$output" = "xargs: unmatched single quote" ] ;;
    bsd) [ "$output" = "xargs: unterminated quote" ] ;;
    *) false ;;
  esac
}

@test "xargs implementations disagree about a newline inside a quoted string" {
  # stdout and stderr stay separate: GNU writes a finished field and the diagnostic
  # on different descriptors, and a merged capture reorders them under load.
  flavor=$(xargs_flavor)
  run_lab_c 'printf "%s\n" "$1" | xargs printf "<%s>" 2>"$2"' sh "x 'a${NL}b' y" "$BATS_TEST_TMPDIR/err"
  err=$(cat "$BATS_TEST_TMPDIR/err")
  case "$flavor" in
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "<x><a${NL}b><y>" ]
      [ "$err" = "" ]
      ;;
    gnu)
      [ "$status" -eq 1 ]
      [ "$output" = "<x>" ]
      [ "$err" = "xargs: unmatched single quote; by default quotes are special to xargs unless you use the -0 option" ]
      ;;
    bsd)
      [ "$status" -eq 1 ]
      [ "$output" = "" ]
      [ "$err" = "xargs: unterminated quote" ]
      ;;
    *) false ;;
  esac
}
