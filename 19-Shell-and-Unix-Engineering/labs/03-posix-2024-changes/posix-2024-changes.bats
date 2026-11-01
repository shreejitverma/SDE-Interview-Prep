#!/usr/bin/env bats
# posix-2024-changes.bats: test suite verifying POSIX.1-2024 additions and divergences.
# Exercises pipefail, ANSI-C quoting ($'...'), declaration utility assignment context,
# read -d and EOF exit status, test binary operators (-ef, -nt, <), and cd -e.
# shellcheck disable=SC2016 # Shell expressions asserted belong to the shell under test

# dash_pipefail: print "supported" or "unsupported" for the dash under test, from an oracle that
# does not ask dash itself. Upstream dash 0.5.12 rejects `set -o pipefail` (Alpine 3.22 0.5.12-r3,
# macOS /bin/dash); the Debian package carries a patch that adds it, measured on 0.5.12-12.
# A Debian dash older than the measured one fails the test instead of guessing.
dash_pipefail() {
  local version
  if version=$(dpkg-query -W -f='${Version}' dash 2>/dev/null) && [ -n "$version" ]; then
    if dpkg --compare-versions "$version" ge 0.5.12-12; then
      echo supported
    else
      echo "unmeasured Debian dash $version" >&2
      echo unmeasured
    fi
  else
    echo unsupported
  fi
}

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/posix-probe.sh"
  STATUS_EXIT="$BATS_TEST_TMPDIR/status-exit"
  "${CC:-cc}" -Wall -Wextra -Werror -o "$STATUS_EXIT" "$BATS_TEST_DIRNAME/status-exit.c"
}

@test "pipefail selects rightmost non-zero status in pipeline" {
  run_lab "$PROBE" pipefail-basic
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash)
      case $(dash_pipefail) in
        supported) [ "$output" = "default=0 pipefail=2" ] ;;
        unsupported) [ "$output" = "default=0 pipefail=unsupported" ] ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$output" = "default=0 pipefail=unsupported" ]
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$output" = "default=0 pipefail=2" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipefail picks rightmost failed stage among multiple non-zero exits" {
  run_lab "$PROBE" pipefail-rightmost
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash)
      case $(dash_pipefail) in
        supported) [ "$output" = "pipefail_rightmost=7" ] ;;
        unsupported) [ "$output" = "pipefail_rightmost=unsupported" ] ;;
        *) false ;;
      esac
      ;;
    posh)
      [ "$output" = "pipefail_rightmost=unsupported" ]
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$output" = "pipefail_rightmost=7" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "pipefail reports termination status for upstream SIGPIPE failure" {
  run_lab "$PROBE" pipefail-sigpipe
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "sigpipe_status=unsupported" ]
      ;;
    dash)
      case $(dash_pipefail) in
        supported) [ "$output" = "sigpipe_status=141" ] ;;
        unsupported) [ "$output" = "sigpipe_status=unsupported" ] ;;
        *) false ;;
      esac
      ;;
    yash)
      # yash encodes a signal death as 384 + N (measured on 2.58.1 and 2.61)
      [ "$output" = "sigpipe_status=397" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          # ksh93u+m connects pipeline stages via socketpair(2) instead of pipe(2);
          # upstream write after reader exit races between ECONNRESET (status 1)
          # and SIGPIPE (signal 13 with base 256, status 269).
          case $output in
            "sigpipe_status=1" | "sigpipe_status=269") true ;;
            *) false ;;
          esac
          ;;
        *)
          # macOS ksh93u+ 2012 reports raw signal 13
          [ "$output" = "sigpipe_status=13" ]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | mksh | busybox)
      [ "$output" = "sigpipe_status=141" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "ansi-c quoting expands escape sequences before word expansion" {
  run_lab "$PROBE" ansi-c-decode
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash)
      # dash 0.5.12 parses $ and '...' separately, so the dollar stays and no escape is decoded
      [ "$output" = 'data=<$hello\tworld\n>' ]
      ;;
    posh)
      [ "$output" = 'data=<$hello\tworld\n>' ]
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "${lines[0]}" = 'data=<hello	world' ]
      [ "${lines[1]}" = '>' ]
      ;;
    *)
      false
      ;;
  esac
}

@test "declaration utilities suppress field splitting on assignment words" {
  run_lab "$PROBE" declaration-utility
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 does not treat export as a declaration utility and field-splits
      [ "$output" = "result=x=alpha" ]
      ;;
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox)
      [ "$output" = "result=x=alpha beta" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read -d consumes tokens delimited by arbitrary character" {
  run_lab "$PROBE" read-delim
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh | dash)
      [ "$output" = "read_delim=unsupported" ]
      ;;
    yash)
      # read -d arrived in yash 2.59 (NEWS); 2.58.1 is the last release without it
      case $(lab_shell_version) in
        2.58*) [ "$output" = "read_delim=unsupported" ] ;;
        *) [ "$output" = "read_delim=alpha" ] ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | busybox)
      [ "$output" = "read_delim=alpha" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "read returns exit status 1 on end of file" {
  run_lab "$PROBE" read-eof-status
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | dash | ksh93 | mksh | yash | busybox | posh)
      [ "$output" = "eof_status=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "test builtin implements -ef, -nt, and string comparison operators" {
  run_lab "$PROBE" test-binary-ops "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash | dash | mksh | yash | busybox)
      [ "$output" = "ef=ok nt=ok lt=ok" ]
      ;;
    zsh | zsh-sh)
      # zsh requires condition evaluation syntax [[ for <
      [ "$output" = "ef=ok nt=ok lt=fail" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$output" = "ef=ok nt=ok lt=ok" ]
          ;;
        *)
          # Legacy ksh93u+ 2012 lacks < binary test operator
          [ "$output" = "ef=ok nt=ok lt=fail" ]
          ;;
      esac
      ;;
    posh)
      # posh 0.14.1 lacks -ef, -nt, and < in its test builtin
      [ "$output" = "ef=fail nt=fail lt=fail" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "cd -P -e fails only when the physical working directory cannot be determined" {
  run_lab "$PROBE" cd-physical-e "$BATS_TEST_TMPDIR"
  [ "$status" -eq 0 ]
  # Fields: option accepted on a resolvable directory, then the status of `cd -P -e .` and of
  # `cd -P .` inside a directory that has been removed (chdir succeeds, getcwd fails).
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2*)
          # no -e: the option is rejected, so the failure status is the usage error
          [ "$output" = "cd_e=unsupported unresolvable_e=1 unresolvable_plain=0" ]
          ;;
        *)
          [ "$output" = "cd_e=ok unresolvable_e=1 unresolvable_plain=0" ]
          ;;
      esac
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$output" = "cd_e=ok unresolvable_e=1 unresolvable_plain=0" ]
          ;;
        *)
          [ "$output" = "cd_e=unsupported unresolvable_e=2 unresolvable_plain=0" ]
          ;;
      esac
      ;;
    mksh | yash)
      [ "$output" = "cd_e=ok unresolvable_e=1 unresolvable_plain=0" ]
      ;;
    posh)
      # posh accepts and ignores -e, so the failure is never reported
      [ "$output" = "cd_e=ok unresolvable_e=0 unresolvable_plain=0" ]
      ;;
    dash | busybox)
      [ "$output" = "cd_e=unsupported unresolvable_e=2 unresolvable_plain=0" ]
      ;;
    zsh | zsh-sh)
      [ "$output" = "cd_e=unsupported unresolvable_e=1 unresolvable_plain=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "external process pipeline under pipefail propagates exact exit status" {
  run_lab_c '
    HELPER="'"$STATUS_EXIT"'"
    if (set -o pipefail 2>/dev/null); then
      set -o pipefail
      "$HELPER" exit 42 | "$HELPER" exit 0
      echo "pipefail_code=$?"
    else
      "$HELPER" exit 42 | "$HELPER" exit 0
      echo "default_code=$?"
    fi
  '
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      [ "$output" = "default_code=0" ]
      ;;
    dash)
      case $(dash_pipefail) in
        supported) [ "$output" = "pipefail_code=42" ] ;;
        unsupported) [ "$output" = "default_code=0" ] ;;
        *) false ;;
      esac
      ;;
    bash | zsh | zsh-sh | ksh93 | mksh | yash | busybox)
      [ "$output" = "pipefail_code=42" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "null-delimited stream reading keeps spaces and newlines inside records" {
  run_lab_c '
    HELPER="'"$STATUS_EXIT"'"
    if (printf "a\0" | read -d "" x 2>/dev/null); then
      count=0
      # shellcheck disable=SC2162 # testing raw delimiter processing
      "$HELPER" stream-nul | while read -d "" -r rec; do
        count=$((count + 1))
        printf "rec%d=<%s> " "$count" "$rec"
      done
      echo ""
    else
      echo "null_read=unsupported"
    fi
  '
  [ "$status" -eq 0 ]
  null_records=$(printf 'rec1=<alpha\nitem> rec2=<beta item> ')
  case $(lab_shell_family) in
    posh | dash | yash)
      # yash 2.61 accepts -d but treats the empty delimiter as no delimiter (read returns 1)
      [ "$output" = "null_read=unsupported" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$output" = "$null_records" ]
          ;;
        *)
          [ "$output" = "null_read=unsupported" ]
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | mksh | busybox)
      [ "$output" = "$null_records" ]
      ;;
    *)
      false
      ;;
  esac
}
