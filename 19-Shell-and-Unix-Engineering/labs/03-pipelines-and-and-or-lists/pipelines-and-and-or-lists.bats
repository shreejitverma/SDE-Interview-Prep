#!/usr/bin/env bats
# Tests for Pipelines and And-Or Lists: POSIX.1-2024 XCU 2.9.2, 2.9.3, 2.10, and 2.15.
# Code under test runs through run_lab SCRIPT or run_lab_c CODE via labs/lib/lab.bash.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/pipeline-probe.sh"
  CAP_SRC="$BATS_TEST_DIRNAME/pipe-capacity.c"
  CAP_BIN="$BATS_TEST_TMPDIR/pipe-capacity"
}

# expect_pipefail: set HAS_PIPEFAIL to yes or no from the shell's family, never from the shell's own answer.
# dash depends on the build, so its expectation comes from the package version:
# Debian patched pipefail in at 0.5.12-7, and upstream released it in 0.5.13.
expect_pipefail() {
  local real version cmd_path os
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox) HAS_PIPEFAIL=yes ;;
    posh) HAS_PIPEFAIL=no ;;
    dash)
      lab_shell_words
      cmd_path=$(command -v "${LAB_SHELL_WORDS[0]}")
      real=$(readlink -f "${cmd_path}")
      case $real in
        /bin/dash | /usr/bin/dash) ;;
        *) skip "no recorded pipefail expectation for unpackaged $real" ;;
      esac
      os=$(uname -s)
      if command -v dpkg-query >/dev/null 2>&1; then
        # shellcheck disable=SC2016 # ${Version} is a dpkg-query format field
        version=$(dpkg-query -W -f='${Version}' dash)
        if dpkg --compare-versions "$version" ge 0.5.12-7; then
          HAS_PIPEFAIL=yes
        else
          HAS_PIPEFAIL=no
        fi
      elif command -v apk >/dev/null 2>&1; then
        version=$(apk info -e -v dash 2>/dev/null)
        case $(apk version -t "${version#dash-}" 0.5.13) in
          "<") HAS_PIPEFAIL=no ;;
          *) HAS_PIPEFAIL=yes ;;
        esac
      elif [ "${os}" = Darwin ]; then
        HAS_PIPEFAIL=no
      else
        skip "no recorded pipefail expectation for dash on this platform"
      fi
      ;;
    *) false ;;
  esac
}

# --- Probe Script Modes ---

@test "probe assoc verifies left associativity and fake ternary trap" {
  run_lab "$PROBE" assoc
  [ "$status" -eq 0 ]
  [ "$output" = "assoc=run1 ternary=fallback" ]
}

@test "probe subshell checks whether last pipeline stage runs in a subshell" {
  run_lab "$PROBE" subshell
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | ksh93)
      [ "$output" = "val=modified" ]
      ;;
    bash | zsh-sh | dash | busybox | posh | mksh | yash)
      [ "$output" = "val=initial" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe pipefail verifies pipeline status derivation" {
  expect_pipefail
  run_lab "$PROBE" pipefail
  [ "$status" -eq 0 ]
  if [ "$HAS_PIPEFAIL" = yes ]; then
    [ "$output" = "default=0 pipefail=1" ]
  else
    [ "$output" = "default=0 pipefail=unsupported" ]
  fi
}

@test "probe errexit verifies set -e suppression on non-last operands" {
  run_lab "$PROBE" errexit
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 suppresses set -e across all operands in AND-OR lists (pdksh src/exec.c XERROK bug)
      [ "$output" = "first_operand=survived last_operand=survived" ]
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      [ "$output" = "first_operand=survived last_operand=exited" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "probe redir verifies redirection binding order against output pipe" {
  run_lab "$PROBE" redir "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # Native zsh enables MULTIOS by default, teeing output to both file and pipe
      [ "$output" = "downstream_bytes=8 file_bytes=8" ]
      ;;
    bash | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      # POSIX shells override the pipeline stdout with the redirection
      [ "$output" = "downstream_bytes=0 file_bytes=8" ]
      ;;
    *)
      false
      ;;
  esac
}

# --- AND-OR List Semantics ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "&& and || have equal precedence and group strictly left to right" {
  run_lab_c '
    r1=$(true || echo skip && echo r1)
    r2=$(false && echo skip || echo r2)
    r3=$(true && echo r3 || echo skip)
    r4=$(false || printf "r4a " && printf "r4b")
    printf "%s %s %s %s\n" "$r1" "$r2" "$r3" "$r4"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "r1 r2 r3 r4a r4b" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "short-circuit evaluation skips unreached commands and expansions" {
  run_lab_c '
    x=0; y=0
    false && x=$((x + 1))
    true || y=$((y + 1))
    printf "x=%d y=%d\n" "$x" "$y"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "x=0 y=0" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "exit status of an AND-OR list is the status of the last command executed" {
  run_lab_c '
    (exit 3) && echo skip; s1=$?
    (exit 0) && (exit 4); s2=$?
    (exit 5) || (exit 6); s3=$?
    (exit 0) || echo skip; s4=$?
    printf "%d %d %d %d\n" "$s1" "$s2" "$s3" "$s4"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "3 4 6 0" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "cmd1 && cmd2 || true idiom guarantees zero status and script survival under set -e" {
  run_lab_c '
    set -e
    false && false || true
    true && false || true
    printf "survived\n"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "survived" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "functions called before && or || have set -e suppressed across all commands" {
  run_lab_c '
    set -e
    fn_risky() {
      false
      printf "fn_continued\n"
    }
    fn_risky && printf "caller_continued\n"
  '
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "fn_continued" ]
  [ "${lines[1]}" = "caller_continued" ]
}

# --- Pipeline Semantics and Execution Environments ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "pipeline returns exit status of the last command without pipefail" {
  run_lab_c '
    (exit 2) | (exit 3) | (exit 0); s1=$?
    (exit 0) | (exit 4) | (exit 7); s2=$?
    printf "%d %d\n" "$s1" "$s2"
  '
  [ "$status" -eq 0 ]
  [ "$output" = "0 7" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "variable assignments in pipeline stages do not affect parent shell in POSIX mode" {
  run_lab_c '
    v="parent"
    printf "first\n" | { v="stage1"; } | { v="stage2"; }
    printf "v=%s\n" "$v"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | ksh93)
      # Native zsh and ksh93 execute the last stage in the current shell
      [ "$output" = "v=stage2" ]
      ;;
    bash | zsh-sh | dash | busybox | posh | mksh | yash)
      [ "$output" = "v=parent" ]
      ;;
    *)
      false
      ;;
  esac
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "bash lastpipe runs the last stage in the current shell only when job control is off" {
  run_lab_c '
    x=parent
    printf "hello\n" | read -r x
    default=$x
    if shopt -s lastpipe 2>/dev/null; then
      y=parent
      printf "hello\n" | read -r y
      lastpipe=$y
      set -m 2>/dev/null
      z=parent
      printf "hello\n" | read -r z
      monitor=$z
    else
      lastpipe=no_lastpipe
      monitor=no_lastpipe
    fi
    printf "default=%s lastpipe=%s monitor=%s\n" "$default" "$lastpipe" "$monitor"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2*)
          # bash 3.2 has shopt but not the lastpipe option
          [ "$output" = "default=parent lastpipe=no_lastpipe monitor=no_lastpipe" ]
          ;;
        *)
          [ "$output" = "default=parent lastpipe=hello monitor=parent" ]
          ;;
      esac
      ;;
    zsh | ksh93)
      # No shopt, and the last stage already runs in the current shell
      [ "$output" = "default=hello lastpipe=no_lastpipe monitor=no_lastpipe" ]
      ;;
    zsh-sh | dash | busybox | posh | mksh | yash)
      # No shopt, and every stage runs in a subshell
      [ "$output" = "default=parent lastpipe=no_lastpipe monitor=no_lastpipe" ]
      ;;
    *)
      false
      ;;
  esac
}

# --- Pipeline Negation with ! ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "pipeline negation with ! inverts status and requires separating whitespace" {
  run_lab_c '
    ! (exit 0); s1=$?
    ! (exit 3); s2=$?
    ! false | true; s3=$?
    !true 2>/dev/null; s4=$?
    printf "%d %d %d %d\n" "$s1" "$s2" "$s3" "$s4"
  '
  [ "$status" -eq 0 ]
  # s1: 0 inverted to 1; s2: 3 inverted to 0; s3: last command true (0) inverted to 1;
  # s4: !true is an ordinary command word that is not found (127), not a negation of true
  [ "$output" = "1 0 1 127" ]
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "double negation ! ! cmd is a syntax error or a double inversion, by shell" {
  # Control: a single ! parses and inverts in every shell, so the failures below are the second !.
  run_lab_c '! true; printf "control=%s\n" "$?"'
  [ "$status" -eq 0 ]
  [ "$output" = "control=1" ]
  run_lab_c 'printf "before\n"
! ! true; a=$?
! ! false; b=$?
printf "result a=%s b=%s\n" "$a" "$b"'
  family=$(lab_shell_family)
  if [ "$family" = bash ]; then
    case $(lab_shell_version) in
      3.2*) family=bash-3.2 ;;
      *) ;;
    esac
  fi
  case $family in
    dash | busybox | yash | bash-3.2)
      # The parser reports a syntax error on line 2 and the shell exits 2; line 1 already ran.
      [ "$status" -eq 2 ]
      [ "${lines[0]}" = "before" ]
      case $output in
        *[Ss]"yntax error"*) ;;
        *) false ;;
      esac
      case $output in
        *result*) false ;;
        *) ;;
      esac
      ;;
    zsh | zsh-sh)
      # zsh parses the whole -c string first, so not even line 1 runs.
      [ "$status" -eq 1 ]
      [ "${lines[0]}" != "before" ]
      case $output in
        *"parse error"*) ;;
        *) false ;;
      esac
      case $output in
        *result*) false ;;
        *) ;;
      esac
      ;;
    bash | ksh93 | mksh | posh)
      # Accepted as two inversions, so the status is the command's own.
      [ "$status" -eq 0 ]
      [ "${lines[0]}" = "before" ]
      [ "${lines[1]}" = "result a=0 b=1" ]
      ;;
    *)
      false
      ;;
  esac
}

# --- Pipefail and Pipeline Status Arrays ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "set -o pipefail propagates rightmost failure status" {
  expect_pipefail
  run_lab_c '
    if (set -o pipefail 2>/dev/null); then
      set -o pipefail
      (exit 2) | (exit 0) | (exit 0); s1=$?
      (exit 3) | (exit 7) | (exit 0); s2=$?
      (exit 0) | (exit 0) | (exit 0); s3=$?
      ! ((exit 2) | (exit 0)); s4=$?
      printf "%d %d %d %d\n" "$s1" "$s2" "$s3" "$s4"
    else
      printf "pipefail unsupported\n"
    fi
  '
  [ "$status" -eq 0 ]
  if [ "$HAS_PIPEFAIL" = yes ]; then
    [ "$output" = "2 7 0 0" ]
  else
    [ "$output" = "pipefail unsupported" ]
  fi
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "SIGPIPE in pipeline under pipefail yields signal termination status" {
  expect_pipefail
  family=$(lab_shell_family)
  # ksh93u+m default pipelines use socketpair(2), and the status is a race (see the fresh-shells test);
  # its posix option selects pipe(2), which makes the status deterministic.
  setup=:
  if [ "$family" = ksh93 ]; then
    case $(lab_shell_version) in
      *93u+m*) setup="set -o posix" ;;
      *) ;;
    esac
  fi
  run_lab_c "$setup"'
    if (set -o pipefail 2>/dev/null); then
      set -o pipefail
      yes | head -n 1 >/dev/null; plain=$?
      yes 2>/dev/null | head -n 1 >/dev/null; redirected=$?
      printf "plain=%s redirected=%s\n" "$plain" "$redirected"
    else
      printf "unsupported\n"
    fi
  '
  [ "$status" -eq 0 ]
  if [ "$HAS_PIPEFAIL" = no ]; then
    [ "$output" = "unsupported" ]
    return 0
  fi
  case $family in
    yash)
      # yash reports base 384 + 13; a redirected stage runs in an intermediate subshell
      # whose exit code keeps only 8 bits across the process boundary (397 & 255 = 141)
      [ "$output" = "plain=397 redirected=141" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*) [ "$output" = "plain=269 redirected=269" ] ;; # base 256 + 13
        *) [ "$output" = "plain=13 redirected=13" ] ;;         # macOS ksh93u+ 2012: raw signal
      esac
      ;;
    bash | zsh | zsh-sh | dash | mksh | busybox)
      # Standard Unix signal status base is 128 (128 + 13 = 141)
      [ "$output" = "plain=141 redirected=141" ]
      ;;
    *)
      false
      ;;
  esac
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "PIPESTATUS in bash and mksh and pipestatus in native zsh expose per-stage statuses, and no other shell sets them" {
  run_lab_c '
    (exit 2) | (exit 4) | (exit 0)
    printf "PIPESTATUS=%s pipestatus=%s\n" "${PIPESTATUS-unset}" "${pipestatus-unset}"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | mksh)
      # A bare array name expands to element 0
      [ "$output" = "PIPESTATUS=2 pipestatus=unset" ]
      run_lab_c '
        (exit 2) | (exit 4) | (exit 0)
        printf "%s %s %s\n" "${PIPESTATUS[0]}" "${PIPESTATUS[1]}" "${PIPESTATUS[2]}"
      '
      [ "$status" -eq 0 ]
      [ "$output" = "2 4 0" ]
      ;;
    zsh)
      [ "$output" = "PIPESTATUS=unset pipestatus=2 4 0" ]
      run_lab_c '
        (exit 2) | (exit 4) | (exit 0)
        printf "%s %s %s\n" "${pipestatus[1]}" "${pipestatus[2]}" "${pipestatus[3]}"
      '
      [ "$status" -eq 0 ]
      [ "$output" = "2 4 0" ]
      ;;
    zsh-sh | dash | ksh93 | yash | busybox | posh)
      [ "$output" = "PIPESTATUS=unset pipestatus=unset" ]
      ;;
    *)
      false
      ;;
  esac
}

# --- Asynchronous Pipelines ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "asynchronous pipeline sets \$! to the last stage and wait reports that stage's status" {
  # Each stage records its own process ID. The last stage reads a line from the first,
  # so by the time wait returns both records exist; their exit statuses (3 and 4) differ.
  run_lab_c '
    /bin/sh -c "echo \$\$ >\"\$1\"; echo go; exit 3" stage "$1/first" |
      /bin/sh -c "read -r _; echo \$\$ >\"\$1\"; exit 4" stage "$1/last" &
    pid=$!
    wait "$pid"
    st=$?
    first=$(cat "$1/first")
    last=$(cat "$1/last")
    is=other
    [ "$pid" = "$first" ] && is=first
    [ "$pid" = "$last" ] && is=last
    printf "pid_is=%s st=%d\n" "$is" "$st"
  ' sh "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  [ "$output" = "pid_is=last st=4" ]
}

# --- C Helper: Pipe Capacity and Write Atomicity ---

@test "pipe capacity measures 16 pages on Linux and 65536 bytes on macOS" {
  "${CC:-cc}" -O2 -Wall -Wextra -Werror "$CAP_SRC" -o "$CAP_BIN"
  run "$CAP_BIN"
  [ "$status" -eq 0 ]
  os=$(uname -s)
  pagesize=$(getconf PAGESIZE)
  case ${os} in
    Linux) [ "$output" = "pipe_capacity=$((16 * pagesize))" ] ;;
    Darwin) [ "$output" = "pipe_capacity=65536" ] ;;
    *) skip "no recorded pipe capacity for ${os}" ;;
  esac
}

# --- Portability Rows ---

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "a writer stage sees its own SIGPIPE status through \$? without pipefail" {
  family=$(lab_shell_family)
  setup=:
  if [ "$family" = ksh93 ]; then
    case $(lab_shell_version) in
      *93u+m*) setup="set -o posix" ;;
      *) ;;
    esac
  fi
  record="$BATS_TEST_TMPDIR/writer-status"
  run_lab_c "$setup"'
    { yes 2>/dev/null; echo "$?" >"$1"; } | head -n 1 >/dev/null
  ' sh "$record"
  [ "$status" -eq 0 ]
  # POSIX lets a shell skip waiting for anything but the last stage, so the record may land later.
  lab_poll 10 test -s "$record"
  run cat "$record"
  case $family in
    yash) [ "$output" = 397 ] ;;
    ksh93) [ "$output" = 269 ] ;;
    bash | zsh | zsh-sh | dash | mksh | busybox | posh) [ "$output" = 141 ] ;;
    *) false ;;
  esac
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "default SIGPIPE status under pipefail across 20 fresh shells" {
  expect_pipefail
  family=$(lab_shell_family)
  seen=" "
  i=0
  while [ "$i" -lt 20 ]; do
    run_lab_c '
      if (set -o pipefail 2>/dev/null); then
        set -o pipefail
        yes 2>/dev/null | head -n 1 >/dev/null
        printf "%s\n" "$?"
      else
        printf "unsupported\n"
      fi
    '
    [ "$status" -eq 0 ]
    case $seen in
      *" $output "*) ;;
      *) seen="$seen$output " ;;
    esac
    i=$((i + 1))
  done
  if [ "$HAS_PIPEFAIL" = no ]; then
    [ "$seen" = " unsupported " ]
    return 0
  fi
  case $family in
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          # A real race on socketpair(2): ECONNRESET makes GNU yes exit 1, SIGPIPE gives 269.
          # Either may appear in a run of 20, and nothing else may.
          case $seen in
            " 1 " | " 269 " | " 1 269 " | " 269 1 ") ;;
            *) false ;;
          esac
          ;;
        *) [ "$seen" = " 13 " ] ;;
      esac
      ;;
    bash | zsh | zsh-sh | dash | mksh | yash | busybox)
      [ "$seen" = " 141 " ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -o pipefail aborts a shell without it, and a subshell probe survives" {
  expect_pipefail
  run_lab_c 'set -o pipefail
printf "reached\n"'
  if [ "$HAS_PIPEFAIL" = yes ]; then
    [ "$status" -eq 0 ]
    [ "$output" = "reached" ]
  else
    case $(lab_shell_family) in
      dash) [ "$status" -eq 2 ] ;;
      posh) [ "$status" -eq 1 ] ;;
      *) false ;;
    esac
    case $output in
      *reached*) false ;;
      *) ;;
    esac
  fi
  run_lab_c 'if (set -o pipefail 2>/dev/null); then echo has; else echo lacks; fi; echo reached'
  [ "$status" -eq 0 ]
  if [ "$HAS_PIPEFAIL" = yes ]; then
    [ "$output" = "has
reached" ]
  else
    [ "$output" = "lacks
reached" ]
  fi
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "!( with no space is a negated subshell, a failed lookup, or a parse error, by shell" {
  empty="$BATS_TEST_TMPDIR/empty"
  mkdir "$empty"
  # An empty directory, because ksh93 and mksh read !(exit 0) as a negated pathname pattern.
  run_lab_c 'cd "$1" || exit 9
!(exit 0)
printf "st=%s\n" "$?"' sh "$empty"
  case $(lab_shell_family) in
    bash | zsh-sh | dash | busybox | posh | yash)
      [ "$status" -eq 0 ]
      [ "$output" = "st=1" ]
      ;;
    ksh93 | mksh)
      [ "$status" -eq 0 ]
      [ "${lines[1]}" = "st=127" ]
      ;;
    zsh)
      [ "$status" -eq 1 ]
      case $output in
        *"st="*) false ;;
        *) ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

# shellcheck disable=SC2016 # expressions belong to shell under test
@test "60 pipeline stages run under ulimit -n 32 except in zsh" {
  run_lab_c '
    p="printf \"ok\\n\""
    i=1
    while [ "$i" -lt 60 ]; do
      p="$p | cat"
      i=$((i + 1))
    done
    ulimit -n 32 2>/dev/null || { echo no_ulimit; exit 0; }
    out=$(eval "$p" 2>&1)
    st=$?
    case $out in
      ok) kind=ok ;;
      *"Bad file descriptor"*) kind=ebadf ;;
      *) kind="[$out]" ;;
    esac
    printf "st=%s out=%s\n" "$st" "$kind"
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      # The pipeline fails with EBADF in every zsh, but the status is not stable: usually 1, yet
      # under CPU load zsh 5.9.2 (nix, macOS) itself can crash in pipecleanfilelist (st=138, SIGBUS).
      case $output in
        "st=0 "* | "st= "*) false ;;
        "st="*" out=ebadf") ;;
        *) false ;;
      esac
      ;;
    posh)
      # posh has no ulimit builtin
      [ "$output" = "no_ulimit" ]
      ;;
    yash)
      # yash also warns that it cannot save a descriptor above the limit, and still succeeds
      [ "${lines[$((${#lines[@]} - 1))]}" = "st=0 out=ok" ]
      ;;
    bash | dash | ksh93 | mksh | busybox)
      [ "$output" = "st=0 out=ok" ]
      ;;
    *)
      false
      ;;
  esac
}
