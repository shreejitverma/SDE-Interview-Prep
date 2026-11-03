#!/bin/sh
# posix-probe.sh: probe POSIX.1-2024 shell language and utility modifications.
# Exercises pipefail, $'...' quoting, declaration utilities, read -d,
# test binary operators (-ef, -nt, <), and cd -e across shell families.
# shellcheck disable=SC3003 # $'...' testing intentionally isolated behind eval
# shellcheck disable=SC2016 # Single-quoted probe strings are passed to eval
# shellcheck disable=SC3040 # Probing POSIX.1-2024 set -o pipefail option
# shellcheck disable=SC3045 # Probing POSIX.1-2024 read -d and cd -e options
# shellcheck disable=SC3012 # Probing POSIX.1-2024 test < lexicographical operator
# shellcheck disable=SC2162 # Testing delimiter reading without backslash escaping

set -eu

mode="${1:-help}"
shift || true

case "$mode" in
  pipefail-basic)
    # Tests pipeline status derivation without and with pipefail (POSIX.1-2024 XCU 2.9.2)
    s_default=0
    # Run pipeline in subshell to avoid exit under set -e
    (exit 2) | (exit 0) || s_default=$?

    if (set -o pipefail 2>/dev/null); then
      s_pipefail=0
      eval 'set -o pipefail; (exit 2) | (exit 0)' || s_pipefail=$?
      printf "default=%d pipefail=%d\n" "$s_default" "$s_pipefail"
    else
      printf "default=%d pipefail=unsupported\n" "$s_default"
    fi
    ;;

  pipefail-rightmost)
    # Tests rightmost non-zero status selection under pipefail
    if (set -o pipefail 2>/dev/null); then
      s_rightmost=0
      eval 'set -o pipefail; (exit 2) | (exit 7) | (exit 0)' || s_rightmost=$?
      printf "pipefail_rightmost=%d\n" "$s_rightmost"
    else
      printf "pipefail_rightmost=unsupported\n"
    fi
    ;;

  pipefail-sigpipe)
    # Tests pipeline exit status under pipefail when an upstream stage receives SIGPIPE
    if (set -o pipefail 2>/dev/null); then
      s_sigpipe=0
      eval 'set -o pipefail; yes | head -n 1 >/dev/null' 2>/dev/null || s_sigpipe=$?
      printf "sigpipe_status=%d\n" "$s_sigpipe"
    else
      printf "sigpipe_status=unsupported\n"
    fi
    ;;

  ansi-c-decode)
    # Tests ANSI-C quoting $'...' (POSIX.1-2024 XCU 2.2.4)
    # Uses eval so shells lacking $'...' do not trigger a parse error on script load
    cmd='x=$'"'"'hello\tworld\n'"'"'; printf "data=<%s>\n" "$x"'
    eval "$cmd"
    ;;

  declaration-utility)
    # Tests assignment context in declaration utilities (POSIX.1-2024 XCU 2.9.1)
    val="alpha beta"
    # shellcheck disable=SC2086 # Unquoted expansion verifies assignment context suppresses field splitting
    export x=$val
    printf "result=x=%s\n" "$x"
    ;;

  read-delim)
    # Tests read -d delimiter option (POSIX.1-2024 XCU read, Austin Group Defect 243)
    if (printf "alpha:beta\n" | read -d : item 2>/dev/null); then
      item=$(printf "alpha:beta\n" | {
        read -d : val
        printf "%s" "$val"
      })
      printf "read_delim=%s\n" "$item"
    else
      printf "read_delim=unsupported\n"
    fi
    ;;

  read-eof-status)
    # Tests read exit status on EOF (POSIX.1-2024 Defect 367: EOF must return 1)
    eof_status=0
    # shellcheck disable=SC2034 # item receives EOF
    read -r item </dev/null || eof_status=$?
    printf "eof_status=%d\n" "$eof_status"
    ;;

  test-binary-ops)
    # Tests -ef, -nt, and < in test/[ (POSIX.1-2024 XCU test, Austin Group Defect 375)
    dir="$1"
    f1="$dir/f1"
    f2="$dir/f2"
    f3="$dir/f3"
    printf "content1\n" >"$f1"
    ln -f "$f1" "$f2"
    /bin/sleep 1
    printf "content3\n" >"$f3"

    res_ef="fail"
    if test "$f1" -ef "$f2" 2>/dev/null; then
      res_ef="ok"
    fi

    res_nt="fail"
    if test "$f3" -nt "$f1" 2>/dev/null; then
      res_nt="ok"
    fi

    res_lt="fail"
    if test "apple" "<" "banana" 2>/dev/null; then
      res_lt="ok"
    fi

    printf "ef=%s nt=%s lt=%s\n" "$res_ef" "$res_nt" "$res_lt"
    ;;

  cd-physical-e)
    # Tests cd -P -e (POSIX.1-2024 XCU cd, Austin Group Defect 253).
    # Prints cd_e=ok when the option is accepted on a resolvable directory, else cd_e=unsupported.
    # Then removes the current directory and runs `cd -P .`, which succeeds in chdir(2) but cannot
    # determine the physical working directory: with -e the status must be non-zero (1 in the
    # shells that implement it), without -e it must stay 0.
    target_dir="$1"
    if (cd -P -e "$target_dir" 2>/dev/null); then
      res_cd="ok"
    else
      res_cd="unsupported"
    fi
    gone="$target_dir/gone"
    mkdir "$gone"
    res_with=0
    res_without=0
    (
      cd "$gone"
      rmdir "$gone"
      cd -P -e . 2>/dev/null
    ) || res_with=$?
    mkdir "$gone"
    (
      cd "$gone"
      rmdir "$gone"
      cd -P . 2>/dev/null
    ) || res_without=$?
    printf "cd_e=%s unresolvable_e=%d unresolvable_plain=%d\n" "$res_cd" "$res_with" "$res_without"
    ;;

  *)
    printf "Unknown mode: %s\n" "$mode" >&2
    exit 1
    ;;
esac
