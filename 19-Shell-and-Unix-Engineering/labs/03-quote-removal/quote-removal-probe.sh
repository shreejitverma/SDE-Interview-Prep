#!/bin/sh
# quote-removal-probe: print what quote removal produced for one family of constructs.
# Every probe computes its output from real shell words; the bats file asserts the text.
# Usage: quote-removal-probe.sh MODE
# Modes: concat, norescan, dq-backslash, bare-backslash, quoted-quotes, null-words, split,
#   glob, case, param-pattern, default-word, brace, assign, redirect, heredoc, words, eval,
#   inject, backslash-newline, dollar-single, dollar-single-escape, levels, roundtrip.
# shellcheck disable=SC1001,SC1003,SC2016,SC2046,SC2086,SC2089,SC2090,SC2194,SC2254 # the probe exists to show what unquoted, oddly quoted and constant words do

LC_ALL=C
export LC_ALL
unset IFS

here=$(cd "$(dirname "$0")" && pwd -P) || exit 1

# show ARG...: print the argument count, then each argument between angle brackets.
show() {
  printf '%d:' "$#"
  for a in "$@"; do
    printf '<%s>' "$a"
  done
  printf '\n'
}

# in_tmpdir: enter a fresh scratch directory (honours TMPDIR); leave_tmpdir removes it.
in_tmpdir() {
  work=$(mktemp -d) || exit 1
  cd "$work" || exit 1
}
leave_tmpdir() {
  cd / || exit 1
  rm -rf "$work"
}

mode_concat() {
  # Four quoting forms in one word: one field, no quote characters left.
  show 'a'"b"\c'd e'
}

mode_norescan() {
  # The value holds every quote character; none of them is removed when it comes back from $x.
  x='"a" '\''b'\'' \c'
  show $x
  show "$x"
  y=$x
  show "$y"
  show $(printf '%s' "$x")
  printf 'len=%d\n' "${#x}"
}

mode_dq_backslash() {
  # Inside double quotes a backslash is removed only before $ ` " \ and newline.
  show "\$" "\a" "\\" "\"" "\`" "\'"
}

mode_bare_backslash() {
  # Outside quotes a backslash is removed before every character.
  show \a \\ \' \" \$ \`
}

mode_quoted_quotes() {
  # A quote character that is itself quoted is not removed.
  show "'" '"' \' \" "it's" 'say "hi"'
}

mode_null_words() {
  # A quoted null word survives quote removal as an empty field; an unquoted one vanishes.
  e=
  show ''
  show $e
  show $e''
  show "$e"
  show $e"" $e'' x
}

mode_split() {
  # Field splitting runs before quote removal, so only the unquoted part is split.
  x='a b'
  show $x'c d'
  show "$x"$x
  show $x""
}

mode_glob() {
  # Pathname expansion runs before quote removal, so a quoted metacharacter matches only itself.
  in_tmpdir
  : >ab
  : >ac
  : >'a*'
  show a?
  show a"?"
  show a\?
  show 'a'?
  x='?'
  show a$x
  show a"$x"
  show a*
  show a"*"
  leave_tmpdir
}

mode_case() {
  # The shell remembers which pattern characters were quoted after quote removal.
  m() { case $1 in $2) echo yes ;; *) echo no ;; esac }
  mq() { case $1 in "$2") echo yes ;; *) echo no ;; esac }
  bare=no dq=no bs=no sq=no mid=no
  case abc in a*) bare=yes ;; *) ;; esac
  case abc in "a*") dq=yes ;; *) ;; esac
  case abc in a\*) bs=yes ;; *) ;; esac
  case abc in 'a*') sq=yes ;; *) ;; esac
  case abc in a"*") mid=yes ;; *) ;; esac
  printf 'subject abc: bare=%s dq=%s bs=%s sq=%s mid=%s\n' "$bare" "$dq" "$bs" "$sq" "$mid"
  dq=no bs=no
  case 'a*' in "a*") dq=yes ;; *) ;; esac
  case 'a*' in a\*) bs=yes ;; *) ;; esac
  printf 'subject a*: dq=%s bs=%s\n' "$dq" "$bs"
  r1=$(m abc 'a*')
  r2=$(mq abc 'a*')
  printf 'pattern from variable: unquoted=%s quoted=%s\n' "$r1" "$r2"
}

mode_param_pattern() {
  # The pattern of ${y##pattern} keeps its quoted-ness the same way.
  y=abc
  r1=${y##*}
  r2=${y##"*"}
  r3=${y##\*}
  r4=${y##'*'}
  printf '<%s><%s><%s><%s>\n' "$r1" "$r2" "$r3" "$r4"
  # Inside double quotes the double quotes "have no effect" on the pattern, so it still strips all of "ab".
  r5="${y##'a'*}"
  r6="${y##"a"*}"
  printf 'inside double quotes: <%s><%s>\n' "$r5" "$r6"
}

mode_default_word() {
  # The word of ${u:-word} gets quote removal when it is used; ${u:=word} also when it is stored.
  unset u
  show ${u:-'a b'}
  show ${u:-a\ b}
  show ${u:-"a  b"}
  show "${u:-'a b'}"
  show ${u:=a\ b}
  show "$u"
}

mode_brace() {
  # POSIX rationale C.2.2.3: a backslash before } inside "${...}" is removed, but not in "\}".
  unset bar
  foo="${bar-\}}"
  goo="\}"
  printf '<%s><%s>\n' "$foo" "$goo"
}

mode_assign() {
  v='a'"b"\ c
  show "$v"
  n=''
  show "$n"
  w=$v'd'"$n"
  show "$w"
}

mode_redirect() {
  # The word after a redirection operator gets quote removal; the file name has no quotes.
  in_tmpdir
  : >'x'"y"'z w'
  : >\>
  : >"a\$b"
  for f in *; do
    printf '<%s>' "$f"
  done
  printf '\n'
  leave_tmpdir
}

mode_heredoc() {
  # A quoted delimiter word turns expansion off; the body never gets quote removal.
  v=X
  echo '-- unquoted delimiter'
  cat <<E
"q" 'r' \\ \$v $v \x
E
  echo '-- single-quoted delimiter'
  cat <<'E'
"q" 'r' \\ \$v $v \x
E
  echo '-- double-quoted delimiter'
  cat <<"E"
"q" 'r' \\ \$v $v \x
E
  echo '-- backslash delimiter'
  cat <<\E
"q" 'r' \\ \$v $v \x
E
  echo '-- partly quoted delimiter'
  in_tmpdir
  printf '%s\n' 'cat <<E""' '"q" '\''r'\'' \\ \$v $v \x' E >hd.sh
  # shellcheck source=/dev/null
  . ./hd.sh
  leave_tmpdir
  echo '-- delimiter with a space'
  cat <<'E F'
$v
E F
}

mode_words() {
  n=0
  for i in 'a b' "c" d\ e ''; do
    n=$((n + 1))
    printf '<%s>' "$i"
  done
  printf ' n=%d\n' "$n"
}

mode_eval() {
  # eval parses its argument as a new program, so its quotes are removed one more time.
  x='"a b"'
  set -- $x
  show "$@"
  eval "set -- $x"
  show "$@"
  y="it's"
  # A syntax error must stay inside the substitution: eval is a special builtin.
  # kept=yes would mean the value reached the program unchanged, so the quote was not consumed.
  if out=$(eval "set -- '$y'" 2>/dev/null && {
    printf '<%s>' "$1"
    if [ "$1" = "$y" ]; then printf ' kept=yes'; else printf ' kept=no'; fi
  }); then
    printf 'quote in value: parsed arg=%s\n' "$out"
  else
    echo 'quote in value: syntax error'
  fi
}

mode_inject() {
  # shellcheck source=/dev/null # sh-quote.sh is next to this script and is checked on its own
  . "$here/sh-quote.sh"
  z="'; echo INJECTED; echo '"
  echo '-- value pasted between single quotes'
  eval "show '$z'"
  echo '-- value passed through sh_quote'
  q=$(sh_quote "$z")
  eval "show $q"
}

mode_backslash_newline() {
  # Backslash-newline is deleted by the tokenizer, before quote removal, except inside single quotes.
  in_tmpdir
  printf '%s\n' 'show "a\' 'b" '\''a\' 'b'\'' a\' 'b' >s.sh
  # shellcheck source=/dev/null
  . ./s.sh
  leave_tmpdir
}

mode_dollar_single() {
  # The code is built at run time so that a shell without dollar-single-quotes still parses this file.
  sq=\'
  code="show \$${sq}abc${sq}"
  eval "$code"
}

mode_dollar_single_escape() {
  sq=\'
  code="show \$${sq}a\\${sq}b${sq}"
  eval "$code"
}

mode_levels() {
  # Quote a string that contains one single quote, again and again, and count the quotes.
  # shellcheck source=/dev/null # sh-quote.sh is next to this script and is checked on its own
  . "$here/sh-quote.sh"
  s="'"
  n=0
  while [ "$n" -le 4 ]; do
    qc=$(printf '%s' "$s" | tr -cd "'" | wc -c | tr -d ' ')
    printf 'level=%d len=%d quotes=%d\n' "$n" "${#s}" "$qc"
    s=$(sh_quote "$s")
    n=$((n + 1))
  done
  # Each eval removes exactly one level of quoting again.
  while [ "$n" -gt 0 ]; do
    eval "s=$s"
    n=$((n - 1))
    printf 'peel to level=%d len=%d\n' "$n" "${#s}"
  done
  printf 'last=<%s>\n' "$s"
}

mode_roundtrip() {
  # One eval removes exactly the quotes sh_quote added.
  # shellcheck source=/dev/null # sh-quote.sh is next to this script and is checked on its own
  . "$here/sh-quote.sh"
  nl='
'
  tail=$(printf 'tail\n.')
  tail=${tail%.}
  set -- '' "'" "''" "a'b" 'a b' '$HOME' '`id`' '\' '"' '*' '-n' 'a;b' '\x' '!' '~' '{a,b}' "a${nl}b" "$tail"
  for v in "$@"; do
    q=$(sh_quote "$v")
    r=
    eval "r=$q"
    if [ "$r" = "$v" ]; then
      printf 'ok len=%d\n' "${#v}"
    else
      printf 'FAIL <%s>\n' "$v"
    fi
  done
}

case ${1-} in
  concat) mode_concat ;;
  norescan) mode_norescan ;;
  dq-backslash) mode_dq_backslash ;;
  bare-backslash) mode_bare_backslash ;;
  quoted-quotes) mode_quoted_quotes ;;
  null-words) mode_null_words ;;
  split) mode_split ;;
  glob) mode_glob ;;
  case) mode_case ;;
  param-pattern) mode_param_pattern ;;
  default-word) mode_default_word ;;
  brace) mode_brace ;;
  assign) mode_assign ;;
  redirect) mode_redirect ;;
  heredoc) mode_heredoc ;;
  words) mode_words ;;
  eval) mode_eval ;;
  inject) mode_inject ;;
  backslash-newline) mode_backslash_newline ;;
  dollar-single) mode_dollar_single ;;
  dollar-single-escape) mode_dollar_single_escape ;;
  levels) mode_levels ;;
  roundtrip) mode_roundtrip ;;
  *)
    echo "usage: quote-removal-probe.sh MODE" >&2
    exit 2
    ;;
esac
