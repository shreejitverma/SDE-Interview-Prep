#!/usr/bin/env bats
# Tests for the Here Documents note: what the shell reads, expands, strips and hands to the command.
# Scripts that contain here-documents are written with printf at run time, so no formatter can
# change the layout (leading tabs, trailing spaces) that the tests depend on.
# shellcheck disable=SC2016 # script text in single quotes belongs to the shell under test
# shellcheck disable=SC2030,SC2031 # bats run sets output and status in the test shell, not a subshell

setup() {
  load ../lib/lab
  KIND="$BATS_TEST_DIRNAME/heredoc-kind.sh"
  LOOP="$BATS_TEST_DIRNAME/read-loop.sh"
  export TMPDIR="$BATS_TEST_TMPDIR"
  cd "$BATS_TEST_TMPDIR" || return 1
}

# mkscript NAME LINE...: write a script holding exactly these lines, one per argument.
mkscript() {
  local name=$1
  shift
  printf '%s\n' "$@" >"$BATS_TEST_TMPDIR/$name"
}

# --- what is expanded ---------------------------------------------------------------------------

@test "an unquoted delimiter expands parameters, command substitutions and arithmetic" {
  mkscript t.sh 'v=value' 'cat <<EOF' 'p=$v c=$(echo cmd) a=$((1 + 2)) b=`echo bq`' 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "p=value c=cmd a=3 b=bq" ]
}

@test "quoting any part of the delimiter word turns every expansion off" {
  local spelling
  for spelling in "'EOF'" '"EOF"' '\EOF' 'E"O"F'; do
    mkscript t.sh 'v=value' "cat <<$spelling" 'p=$v c=$(echo cmd) a=$((1 + 2)) \$x' 'EOF'
    run_lab "$BATS_TEST_TMPDIR/t.sh"
    [ "$status" -eq 0 ]
    [ "$output" = 'p=$v c=$(echo cmd) a=$((1 + 2)) \$x' ] || {
      echo "spelling $spelling gave: $output"
      false
    }
  done
}

@test "an unquoted body is not tilde-expanded, split, globbed or quote-processed" {
  mkscript t.sh 'HOME=/h' 'w="a  b"' 'touch gl1 gl2' 'cat <<EOF' \
    '~ *' "\"dq\" 'sq' \$'z'" '[$w]' 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' '~ *' "\"dq\" 'sq' \$'z'" '[a  b]')" ]
}

@test "in an unquoted body a backslash escapes only dollar, backquote, backslash and newline" {
  local bs=$'\\'
  mkscript t.sh 'cat <<EOF' '\$lit \\ \x \"' "joined $bs" 'lines' 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' '$lit \ \x \"' 'joined lines')" ]
}

@test "in a quoted body a backslash is literal and a backslash-newline is kept" {
  local bs=$'\\'
  mkscript t.sh "cat <<'EOF'" '\$lit \\ joined '"$bs" 'lines' 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' '\$lit \\ joined '"$bs" 'lines')" ]
}

# --- where the body ends --------------------------------------------------------------------------

@test "the delimiter must be alone on its line: a leading or trailing space does not end the body" {
  mkscript t.sh 'cat <<EOF' 'one' ' EOF' 'EOF ' 'two' 'EOF' 'echo after'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' one ' EOF' 'EOF ' two after)" ]
}

@test "a delimiter on the last line without a newline still ends the body" {
  printf 'cat <<EOF\nbody\nEOF' >"$BATS_TEST_TMPDIR/t.sh"
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = body ]
}

@test "in an unquoted body a backslash-newline before the delimiter line hides that delimiter" {
  printf 'cat <<EOF\nx\\\nEOF\nEOF\necho ok\n' >"$BATS_TEST_TMPDIR/t.sh"
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' xEOF ok)" ]
}

@test "an unterminated here-document is accepted by some shells and a syntax error in others" {
  local script family version
  script=$BATS_TEST_TMPDIR/t.sh
  printf 'echo before\ncat <<EOF\nbody\n' >"$script"
  run_lab "$script"
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case $family in
    zsh | zsh-sh | dash | busybox | ksh93)
      [ "$status" -eq 0 ]
      [ "$output" = "$(printf '%s\n' before body)" ]
      ;;
    bash)
      [ "$status" -eq 0 ]
      case $version in
        3.2.*)
          [ "$output" = "$(printf '%s\n' before body)" ]
          ;;
        4.4.* | 5.1.* | 5.2.* | 5.3.*)
          [ "$output" = "$(printf '%s\n' before "$script: line 3: warning: here-document at line 2 delimited by end-of-file (wanted \`EOF')" body)" ]
          ;;
        *) false ;;
      esac
      ;;
    mksh)
      [ "$status" -eq 1 ]
      case $version in
        *2020/10/31*)
          [ "$output" = "$(printf '%s\n' before "${script}[4]: here document 'EOF' unclosed")" ]
          ;;
        *2025/04/26*)
          [ "$output" = "$(printf '%s\n' before "E: ${script}[4]: here document 'EOF' unclosed")" ]
          ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$status" -eq 1 ]
      [ "$output" = "$(printf '%s\n' before "$script:4: here document \`EOF' unclosed")" ]
      ;;
    yash)
      [ "$status" -eq 2 ]
      [ "$output" = "$(printf '%s\n' before "$script:4: syntax error: the here-document content is not closed by \`EOF'")" ]
      ;;
    *) false ;;
  esac
}

# --- <<- ------------------------------------------------------------------------------------------

@test "<<- strips leading tabs only, from body lines and from the delimiter line" {
  local t=$'\t'
  mkscript t.sh 'cat <<-EOF' "${t}${t}tabs" '  spaces' "${t} ${t}mixed" "${t}EOF" 'echo after'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' tabs '  spaces' " ${t}mixed" after)" ]
}

@test "<<- does not strip a tab that comes from an expansion, and strips after line continuation" {
  local t=$'\t'
  mkscript t.sh 'v=$(printf "\t")' 'cat <<-EOF' "${t}\${v}x" "${t}a \\" "${t}${t}b" "${t}EOF"
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' "${t}x" "a ${t}${t}b")" ]
}

@test "<<- with a quoted delimiter strips tabs and leaves the body literal" {
  local t=$'\t'
  local bs=$'\\'
  mkscript t.sh "cat <<-'EOF'" "${t}\$x $bs" "${t}${t}b" "${t}EOF"
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' "\$x $bs" 'b')" ]
}

@test "without the dash the leading tabs of the body are kept" {
  local t=$'\t'
  mkscript t.sh 'cat <<EOF' "${t}kept" 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "${t}kept" ]
}

# --- when it is read and when it is expanded ------------------------------------------------------

@test "two here-documents on one line are read in order, and file descriptors other than 0 work" {
  mkscript t.sh 'cat <<A; cat <<B' 'first' 'A' 'second' 'B' \
    '{ cat <&4; cat <&3; } 3<<A 4<<B' 'three' 'A' 'four' 'B'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' first second four three)" ]
}

@test "the body starts after the end of the line, whatever follows the operator" {
  mkscript t.sh 'cat <<EOF | tr a-z A-Z && echo done' 'abc' 'EOF'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' ABC 'done')" ]
}

@test "a redirection that is never evaluated consumes its body without expanding it" {
  mkscript t.sh 'false && cat <<EOF' '$(touch skipped)' 'EOF' 'true && cat <<EOF' '$(touch ran)' 'EOF' 'echo after'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '\nafter')" ]
  [ ! -e skipped ]
  [ -e ran ]
}

@test "a here-document on a function definition is expanded again at every call" {
  mkscript t.sh 'x=1' 'f() { cat; } <<EOF' 'fn:$x' 'EOF' 'f' 'x=2' 'f'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' fn:1 fn:2)" ]
}

@test "a here-document on a loop is expanded once and shared by every iteration" {
  mkscript t.sh 'n=0' 'while [ "$n" -lt 2 ]; do cat; n=$((n + 1)); done <<EOF' '$(echo run >>runs)' 'EOF' 'cat runs'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '\nrun')" ]
}

@test "an expansion error in the body fails the command, and exits the shell only in yash" {
  local script family version message
  script=$BATS_TEST_TMPDIR/t.sh
  mkscript t.sh 'echo before' 'cat <<EOF' '${u?boom}' 'EOF' 'echo after'
  run_lab "$script"
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case $family in
    bash | busybox)
      message="$script: line 2: u: boom"
      ;;
    dash)
      message="$script: 2: u: boom"
      ;;
    zsh | zsh-sh)
      message="$script:2: u: boom"
      ;;
    ksh93)
      case $version in
        *93u+m*) message="${script}[2]: u: boom" ;;
        *2012*) message="$script: line 2: u: boom" ;;
        *) false ;;
      esac
      ;;
    mksh)
      case $version in
        *2020/10/31*) message="${script}[4]: u: boom" ;;
        *2025/04/26*) message="E: ${script}[4]: u: boom" ;;
        *) false ;;
      esac
      ;;
    posh)
      message="$script:4: u: boom"
      ;;
    yash)
      [ "$status" -eq 2 ]
      [ "$output" = "$(printf '%s\n' before 'yash: u: boom')" ]
      return
      ;;
    *) false ;;
  esac
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' before "$message" after)" ]
}

# --- here-documents inside other constructs -------------------------------------------------------

@test "a here-document inside a command substitution works" {
  mkscript t.sh 'x=$(cat <<EOF' 'ab' 'EOF' ')' 'echo "[$x]"'
  run_lab "$BATS_TEST_TMPDIR/t.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "[ab]" ]
}

@test "a close parenthesis in a body inside a command substitution breaks bash 3.2 and posh" {
  local script family version
  script=$BATS_TEST_TMPDIR/t.sh
  mkscript t.sh 'x=$(cat <<EOF' 'a ) b' 'EOF' ')' 'echo "[$x]"'
  run_lab "$script"
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case $family in
    posh)
      [ "$status" -eq 1 ]
      [ "$output" = "$script: here document \`EOF' unclosed" ]
      ;;
    bash)
      case $version in
        3.2.*)
          [ "$status" -eq 2 ]
          [ "$output" = "$(printf '%s\n' \
            "$script: line 2: b: command not found" \
            "$script: line 3: EOF: command not found" \
            "$script: line 4: syntax error near unexpected token \`)'" \
            "$script: line 4: \`)'")" ]
          ;;
        4.4.* | 5.1.* | 5.2.* | 5.3.*)
          [ "$status" -eq 0 ]
          [ "$output" = "[a ) b]" ]
          ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh | dash | busybox | ksh93 | mksh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "[a ) b]" ]
      ;;
    *) false ;;
  esac
}

@test "a loop fed by a here-document runs in the current shell" {
  run_lab "$LOOP" guarded
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' 'got a' 'got b' 'got c' 'n=3')" ]
}

@test "a command in the loop that reads standard input eats the rest of the here-document" {
  run_lab "$LOOP" eat
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' 'got a' 'n=1')" ]
}

@test "read-loop.sh rejects an unknown mode" {
  run_lab "$LOOP" bogus
  [ "$status" -eq 2 ]
  [ "$output" = "read-loop.sh: unknown mode: bogus" ]
}

# --- what the command receives --------------------------------------------------------------------

@test "every byte arrives intact at every size, for quoted and unquoted delimiters" {
  local count mode
  for count in 1 10 100 1640 7500; do
    for mode in quoted unquoted; do
      run_lab "$KIND" "$count" "$mode"
      [ "$status" -eq 0 ]
      local delivered=${output#*delivered=}
      delivered=${delivered%% expected=*}
      local expected=${output##*expected=}
      local nbytes=${delivered##* }
      [ "$delivered" = "$expected" ] || {
        echo "count=$count mode=$mode: $output"
        false
      }
      # cksum's second field is the byte count. 40 bytes per line is the claim.
      [ "$nbytes" -eq $((count * 40)) ] || {
        echo "count=$count mode=$mode bytes=$nbytes: $output"
        false
      }
    done
  done
}

# kind_of LINES: print the descriptor kind (pipe, file or other) of an N-line here-document.
kind_of() {
  run_lab "$KIND" "$1" quoted
  [ "$status" -eq 0 ] || return 1
  local k=${output#kind=}
  printf '%s\n' "${k%% *}"
}

@test "the descriptor is a pipe or a temporary file depending on the shell and the size" {
  local small large family version
  small=$(kind_of 10)
  large=$(kind_of 7500)
  family=$(lab_shell_family)
  version=$(lab_shell_version)
  case $family in
    bash)
      # 5.0 is not in the matrix. The changelog says pipes arrive in 5.1, so this
      # arm does not guess a result for 5.0 or for any other unmeasured bash.
      case $version in
        3.2.* | 4.4.*) [ "$small $large" = "file file" ] ;;
        5.1.* | 5.2.* | 5.3.*) [ "$small $large" = "pipe file" ] ;;
        *) false ;;
      esac
      ;;
    zsh | zsh-sh | ksh93 | mksh | posh) [ "$small $large" = "file file" ] ;;
    busybox) [ "$small $large" = "pipe pipe" ] ;;
    yash) [ "$small $large" = "pipe file" ] ;;
    dash)
      # No dash in the matrix sets a version variable. Host dash-16, Alpine 0.5.12
      # and Debian 0.5.12 all returned pipe for both sizes on every repeated run.
      case $version in
        n/a) [ "$small $large" = "pipe pipe" ] ;;
        *) false ;;
      esac
      ;;
    *) false ;;
  esac
}

@test "a large here-document does not block when the command never reads it" {
  {
    printf '/bin/echo ignored <<'"'"'EOF'"'"'\n'
    awk 'BEGIN { for (i = 1; i <= 40000; i++) printf "123456789\n" }'
    printf 'EOF\necho survived\n'
  } >"$BATS_TEST_TMPDIR/big.sh"
  run_lab "$BATS_TEST_TMPDIR/big.sh"
  [ "$status" -eq 0 ]
  [ "$output" = "$(printf '%s\n' ignored survived)" ]
}

@test "zsh needs a writable temporary directory for a small here-document and the others do not" {
  # shellcheck disable=SC2016 # the expansion belongs to the shell under test
  run_lab_c 'TMPDIR=/nonexistent/x TMPPREFIX=/nonexistent/x/zsh; export TMPDIR TMPPREFIX
cat <<EOF
small
EOF'
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "$status" -eq 1 ]
      [ "$output" = "zsh:2: can't create temp file for here document: no such file or directory" ]
      ;;
    bash | dash | busybox | ksh93 | mksh | posh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = small ]
      ;;
    *) false ;;
  esac
}

# --- the shell reading its own script from a here-document or a pipe ------------------------------

@test "a command in a script piped to sh can read the rest of the script, except in dash and BusyBox ash" {
  local family prefix
  family=$(lab_shell_family)
  # The not-found prefix is the shell's own argv0 (sh versus dash, sh versus ash).
  # Measure it, then require that exact text. It does not vary between runs.
  if [ "$family" = dash ] || [ "$family" = busybox ]; then
    lab_shell_words
    prefix=$("${LAB_SHELL_WORDS[@]}" -c "not_a_command_zz" 2>&1 || true)
    prefix=${prefix%%:*}
  fi
  run_lab_c "printf 'cat\nhello\nworld\n' | ${LAB_SHELL:-sh}; echo \"inner=\$?\""
  case $family in
    bash | zsh | zsh-sh | ksh93 | mksh | posh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "$(printf '%s\n' hello world inner=0)" ]
      ;;
    dash)
      [ "$status" -eq 0 ]
      [ "$output" = "$(printf '%s\n' "$prefix: 2: hello: not found" "$prefix: 3: world: not found" inner=127)" ]
      ;;
    busybox)
      [ "$status" -eq 0 ]
      [ "$output" = "$(printf '%s\n' "$prefix: hello: not found" "$prefix: world: not found" inner=127)" ]
      ;;
    *) false ;;
  esac
}

# --- the here-string extension --------------------------------------------------------------------

@test "the here-string <<< is a syntax error in dash, posh and BusyBox ash and works elsewhere" {
  local script family
  script=$BATS_TEST_TMPDIR/t.sh
  mkscript t.sh 'cat <<<hi'
  run_lab "$script"
  family=$(lab_shell_family)
  case $family in
    bash | zsh | zsh-sh | ksh93 | mksh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = hi ]
      ;;
    dash)
      [ "$status" -eq 2 ]
      [ "$output" = "$script: 1: Syntax error: redirection unexpected" ]
      ;;
    posh)
      [ "$status" -eq 1 ]
      [ "$output" = "$script:1: syntax error: \`< ' unexpected" ]
      ;;
    busybox)
      [ "$status" -eq 2 ]
      [ "$output" = "$script: line 1: syntax error: unexpected redirection" ]
      ;;
    *) false ;;
  esac
}

@test "heredoc-kind.sh rejects an unknown mode" {
  run_lab "$KIND" 1 bogus
  [ "$status" -eq 2 ]
  [ "$output" = "heredoc-kind.sh: unknown mode: bogus" ]
}
