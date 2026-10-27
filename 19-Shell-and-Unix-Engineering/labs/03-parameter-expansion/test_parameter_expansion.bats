#!/usr/bin/env bats
# Tests for parameter expansion compliance, operator semantics, and shell divergences.

setup() {
  load ../lib/lab
  PARSE_URL="$BATS_TEST_DIRNAME/parse_url.sh"
}

@test "parse_url decomposes a complete URL using pure parameter expansion" {
  run_lab "${PARSE_URL}" "https://api.example.com:8443/v1/auth/tokens.tar.gz?env=prod"
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "SCHEME=https" ]
  [ "${lines[1]}" = "HOST=api.example.com" ]
  [ "${lines[2]}" = "PORT=8443" ]
  [ "${lines[3]}" = "DIR=/v1/auth" ]
  [ "${lines[4]}" = "FILE=tokens.tar.gz" ]
  [ "${lines[5]}" = "STEM=tokens.tar" ]
  [ "${lines[6]}" = "EXT=gz" ]
  [ "${lines[7]}" = "PATH_LEN=22" ]
  [ "${lines[8]}" = "HAS_QUERY=yes" ]
}

@test "parse_url aborts with error on missing parameter via colon-question operator" {
  run_lab "${PARSE_URL}"
  [ "${status}" -ne 0 ]
  # Diagnostic must mention usage message from :?
  case "${output}" in
    *usage:*parse_url.sh*) ;;
    *) false ;;
  esac
}

@test "unbraced dollar-ten expands to first parameter followed by zero" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c 'set -- a b c d e f g h i j k; printf "%s|%s\n" "$10" "${10}"'
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # Native zsh recognizes multi-digit positional parameters without braces
      [ "${output}" = "j|j" ]
      ;;
    bash | zsh-sh | dash | busybox | ksh93 | mksh | yash | posh)
      # POSIX rule: unbraced parameter is single-character digit $1 followed by literal character 0
      [ "${output}" = "a0|j" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "default value colon operator tests unset or null whereas bare hyphen tests unset only" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    unset u; empty=""
    printf "%s|%s|%s|%s\n" \
      "${u:-default}" "${u-default}" \
      "${empty:-default}" "${empty-default}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "default|default|default|" ]
}

@test "lazy evaluation skips word expansion and side effects when parameter is set" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    v="initial"
    count=0
    # Command substitution and arithmetic must not execute
    res="${v:-$(echo side_effect; count=$((count + 1)))}"
    printf "%s|%s\n" "${res}" "${count}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "initial|0" ]
}

@test "assigning default to positional parameter is rejected by POSIX shells but accepted by zsh" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '( : "${1:=assigned}" ) 2>/dev/null && echo "allowed" || echo "rejected"'
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      [ "${output}" = "allowed" ]
      ;;
    bash | dash | busybox | ksh93 | mksh | yash | posh)
      [ "${output}" = "rejected" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "string length operator counts characters in UTF-8 shells but raw bytes in byte-oriented shells" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    export LC_ALL=C.UTF-8
    # alpha beta gamma: 3 Greek characters, each 2 bytes in UTF-8 (6 bytes total)
    s="αβγ"
    raw_bytes=$(printf "%s" "$s" | wc -c | tr -d " \t")
    printf "%s|%s\n" "${#s}" "${raw_bytes}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh)
      [ "${output}" = "6|6" ]
      ;;
    bash | zsh | zsh-sh | ksh93 | yash | busybox)
      [ "${output}" = "3|6" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020*)
          [ "${output}" = "6|6" ]
          ;;
        *)
          [ "${output}" = "3|6" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "prefix removal distinguishes shortest from longest matching pattern" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    path="/one/two/three/four"
    printf "%s|%s\n" "${path#/*/}" "${path##/*/}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "two/three/four|four" ]
}

@test "suffix removal distinguishes shortest from longest matching pattern" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    path="archive.tar.gz"
    printf "%s|%s\n" "${path%.*}" "${path%%.*}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "archive.tar|archive" ]
}

@test "double quotes around expansion preserve pattern wildcards but inner quotes make wildcards literal" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    str="alpha*beta"
    # "${str#*beta}" keeps * as wildcard (matches whole string, removes it)
    # "${str#"*beta"}" quotes * as literal (fails to match "alpha*beta", keeps whole string)
    printf "%s|%s\n" "${str#*beta}" "${str#"*beta"}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "|alpha*beta" ]
}

@test "variable inside pattern acts as wildcard in POSIX shells but literal in native zsh" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    val="prefix_payload"
    pat="pre*_"
    printf "%s\n" "${val#$pat}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    zsh)
      # Native zsh treats $pat as literal unless GLOB_SUBST is enabled
      [ "${output}" = "prefix_payload" ]
      ;;
    bash | zsh-sh | dash | busybox | ksh93 | mksh | yash | posh)
      [ "${output}" = "payload" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "set -u permits default value fallback but aborts on string length or pattern trimming" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    unset u
    set -u
    # 1. Fallback provides value for unset
    res="${u:-safe_fallback}"
    # 2. String length fails on unset
    len_status="ok"
    ( : "${#u}" ) 2>/dev/null || len_status="err"
    # 3. Substring trim fails on unset
    pat_status="ok"
    ( : "${u#pat}" ) 2>/dev/null || pat_status="err"
    printf "%s|%s|%s\n" "${res}" "${len_status}" "${pat_status}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.*)
          [ "${output}" = "safe_fallback|err|ok" ]
          ;;
        *)
          [ "${output}" = "safe_fallback|err|err" ]
          ;;
      esac
      ;;
    zsh | zsh-sh | dash | busybox | ksh93 | mksh | yash | posh)
      [ "${output}" = "safe_fallback|err|err" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "alternative value operator substitutes word when set and null when unset" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    unset u; non_empty="active"; empty=""
    printf "%s|%s|%s|%s\n" \
      "${non_empty:+ALT}" "${non_empty+ALT}" \
      "${empty:+ALT}" "${empty+ALT}"
  '
  [ "${status}" -eq 0 ]
  [ "${output}" = "ALT|ALT||ALT" ]
}

@test "quoted segment inside a double-quoted trim pattern keeps later wildcards active except in posh" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    a="x?y=1"
    # POSIX: only the quoted ? is literal, so the * after it still matches "y=1"
    printf "%s|%s\n" "${a%%"?"*}" "${a%%\?*}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 bug: after a quoted segment the rest of the pattern is literal
      [ "${output}" = "x?y=1|x" ]
      ;;
    bash | zsh | zsh-sh | dash | busybox | ksh93 | mksh | yash)
      [ "${output}" = "x|x" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "empty quoted at-sign expands to zero fields under set -u except in posh" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    set --
    (
      set -u
      for f in "$@"; do printf "field<%s>\n" "${f}"; done
      echo ok
    ) 2>/dev/null || echo aborted
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    posh)
      # posh 0.14.1 reports "@: parameter not set", although POSIX exempts @ from set -u
      [ "${output}" = "aborted" ]
      ;;
    bash | zsh | zsh-sh | dash | busybox | ksh93 | mksh | yash)
      [ "${output}" = "ok" ]
      ;;
    *)
      false
      ;;
  esac
}
