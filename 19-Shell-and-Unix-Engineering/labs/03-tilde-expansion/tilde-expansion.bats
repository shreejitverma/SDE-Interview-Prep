#!/usr/bin/env bats
# Tests for POSIX.1-2024 tilde expansion (~, ~username, assignments, colons, quoting, and shell divergences).
# shellcheck disable=SC2016,SC2030,SC2031 # Testing literal syntax strings and subshell environment overrides

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/tilde-probe.sh"
  TILDE_INSPECT="$BATS_TEST_TMPDIR/tilde-inspect"
  "${CC:-cc}" -Wall -Wextra -Werror -o "$TILDE_INSPECT" "$BATS_TEST_DIRNAME/tilde-inspect.c"

  # Obtain the true home directory for root on this host
  ROOT_DIR=$(awk -F: '$1 == "root" { print $6 }' /etc/passwd 2>/dev/null || echo "/root")
  [ -n "$ROOT_DIR" ] || ROOT_DIR="/root"
}

@test "unquoted tilde expands to HOME environment variable across all shells" {
  export HOME="$BATS_TEST_TMPDIR/testhome"
  run_lab "$PROBE" basic-home "$BATS_TEST_TMPDIR/testhome"
  [ "$status" -eq 0 ]
  [ "$output" = "home=<$BATS_TEST_TMPDIR/testhome>" ]
}

@test "expanded tilde is treated as if quoted against field splitting and globbing" {
  # When HOME contains spaces, ~ must expand to exactly 1 argument (treated as if quoted)
  export HOME="dir with spaces"
  run_lab "$PROBE" whitespace-home "$TILDE_INSPECT"
  [ "$status" -eq 0 ]
  [ "${lines[0]}" = "argc=1" ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      [ "${lines[1]}" = "argv[0]: len=15 <dir with spaces>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "tilde expansion behavior when HOME is unset reflects shell divergences" {
  # Start the shell with no HOME at all (as under env -i), so a shell that
  # reads the startup environment cannot pass by echoing an inherited HOME.
  unset HOME
  local user_name user_pw_line user_pw_dir
  user_name=$(id -un)
  user_pw_line=$("$TILDE_INSPECT" --getpwnam "$user_name")
  user_pw_dir=${user_pw_line#*dir=}
  [ -n "$user_pw_dir" ]
  run_lab "$PROBE" unset-home
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | busybox | mksh | yash)
      # POSIX leaves unset HOME unspecified; these shells keep literal ~
      [ "$output" = "<~>" ]
      ;;
    bash)
      # Fallback to getpwuid(getuid())->pw_dir (passwd home directory)
      [ "$output" = "<$user_pw_dir>" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          # ksh93u+m falls back to the passwd home directory
          [ "$output" = "<$user_pw_dir>" ]
          ;;
        *)
          # Legacy AT&T ksh93u+ (2012) returns the session login name from getlogin(3),
          # which logname(1) reports; it differs from id -un under su, sudo, cron, or a daemon.
          local login_name
          login_name=$(logname)
          [ "$output" = "<$login_name>" ]
          ;;
      esac
      ;;
    zsh | zsh-sh)
      # zsh expands unset HOME to empty string
      [ "$output" = "<>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "null HOME produces empty field in POSIX.1-2024 conforming shells but zero fields in legacy shells" {
  # POSIX.1-2024 XCU 2.6.1 (Austin Group Defect 1632): unquoted ~ with HOME="" produces 1 empty field
  export HOME=""
  run_lab_c 'set -- ~; printf "argc=%d" "$#"; for a in "$@"; do printf " <%s>" "$a"; done; echo'
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      case $(lab_shell_version) in
        3.2*)
          # Bash 3.2 produces 0 fields
          [ "$output" = "argc=0" ]
          ;;
        *)
          # Bash 4.4 and later produce 1 empty field, as POSIX.1-2024 requires
          [ "$output" = "argc=1 <>" ]
          ;;
      esac
      ;;
    zsh)
      # Native zsh produces 1 empty field, as POSIX.1-2024 requires
      [ "$output" = "argc=1 <>" ]
      ;;
    zsh-sh | dash | posh | ksh93 | mksh | busybox | yash)
      # zsh in sh emulation drops empty unquoted fields; historical shells produce 0 fields
      [ "$output" = "argc=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "user lookup ~root resolves to system password database home directory" {
  run_lab "$PROBE" user-lookup
  [ "$status" -eq 0 ]
  [ "$output" = "root=<$ROOT_DIR>" ]
}

@test "unknown user login name handling diverges between native zsh and standard shells" {
  run_lab "$PROBE" unknown-user
  case $(lab_shell_family) in
    zsh)
      # Native zsh treats unknown user as fatal named directory error
      [ "$status" -ne 0 ]
      case $output in
        *"no such user or named directory"*) true ;;
        *) false ;;
      esac
      ;;
    zsh-sh | bash | dash | posh | busybox | mksh | ksh93 | yash)
      # Standard shells leave the unexpandable word intact
      [ "$status" -eq 0 ]
      [ "$output" = "unknown=<~nosuchuser99999>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "directory stack expansions ~+ and ~- diverge between ksh/bash and minimal shells" {
  local dir_a="$BATS_TEST_TMPDIR/dir_a"
  local dir_b="$BATS_TEST_TMPDIR/dir_b"
  mkdir -p "$dir_a" "$dir_b"
  run_lab "$PROBE" dir-stack "$dir_a" "$dir_b"
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | busybox)
      # Minimal POSIX shells do not support ~+ or ~-; treat as literal
      [ "$output" = "pwd=<~+> oldpwd=<~->" ]
      ;;
    bash | ksh93 | mksh | yash | zsh | zsh-sh)
      # Supported extensions: ~+ is PWD, ~- is OLDPWD
      [ "$output" = "pwd=<$dir_b> oldpwd=<$dir_a>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "quoting inside tilde prefix suppresses expansion in standard shells but unquotes in ksh93 and zsh" {
  export HOME="$BATS_TEST_TMPDIR/testhome"
  run_lab "$PROBE" quoted-prefix
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | busybox | mksh | yash | bash | zsh-sh)
      # Quoting inside prefix suppresses tilde expansion in POSIX-conforming shells
      [ "$output" = "backslash=<~> double=<~> single=<~> empty_double=<~> quoted_user=<~root> escaped_char=<~root>" ]
      ;;
    ksh93 | zsh)
      # ksh93 and native zsh unquote characters inside tilde prefix before lookup
      [ "$output" = "backslash=<~> double=<~> single=<~> empty_double=<$BATS_TEST_TMPDIR/testhome> quoted_user=<$ROOT_DIR> escaped_char=<$ROOT_DIR>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "tilde expansion precedes parameter expansion so ~\$u does not expand login name in standard shells" {
  run_lab "$PROBE" expansion-order
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    dash | posh | busybox | mksh | yash | bash | zsh-sh)
      # Tilde expansion runs first; $u is not a valid login name, then $u expands to root
      [ "$output" = "var=<~root>" ]
      ;;
    ksh93 | zsh)
      # ksh93 and native zsh perform parameter expansion before or re-evaluate tildes
      [ "$output" = "var=<$ROOT_DIR>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "colon-separated tilde expansion operates in variable assignments across all shells" {
  export HOME="/mock/home"
  run_lab "$PROBE" assignment-colon
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    zsh | zsh-sh)
      # zsh expands after escaped colon foo\:~
      [ "$output" = "a=</mock/home/bin:$ROOT_DIR/bin> b=<:/mock/home> c=<foo:/mock/home> d=<foo::/mock/home>" ]
      ;;
    dash | posh | busybox | mksh | ksh93 | yash | bash)
      # Standard shells do not treat escaped colon as assignment delimiter
      [ "$output" = "a=</mock/home/bin:$ROOT_DIR/bin> b=<:/mock/home> c=<foo:~> d=<foo::/mock/home>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "assignment argument to ordinary utility diverges between native bash and POSIX conforming shells" {
  export HOME="/mock/home"
  run_lab "$PROBE" command-operand
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    bash)
      # Check if bash is running in POSIX mode ([ -o posix ])
      local is_posix
      is_posix=$(
        lab_shell_words
        "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ] && echo on || echo off' 2>/dev/null || echo off
      )
      if [ "$is_posix" = "on" ]; then
        case $(lab_shell_version) in
          3.* | 4.*)
            # Legacy bash in POSIX mode did not expand leading tilde but expanded after colon
            [ "${lines[0]}" = "operand=<PATH=~/bin:$ROOT_DIR/bin>" ]
            ;;
          *)
            # Bash 5.x in POSIX mode conforms strictly to POSIX: operand remains completely literal
            [ "${lines[0]}" = "operand=<PATH=~/bin:~root/bin>" ]
            ;;
        esac
      else
        # Native bash expands assignment-like arguments for normal utilities (magic_equal_subst)
        [ "${lines[0]}" = "operand=<PATH=/mock/home/bin:$ROOT_DIR/bin>" ]
      fi
      [ "${lines[1]}" = "export=</mock/home/bin:$ROOT_DIR/bin>" ]
      ;;
    mksh)
      # mksh (ksh88 behavior) expands tilde after = in command operands, but not after colon
      [ "${lines[0]}" = "operand=<PATH=/mock/home/bin:~root/bin>" ]
      [ "${lines[1]}" = "export=</mock/home/bin:$ROOT_DIR/bin>" ]
      ;;
    posh)
      # posh 0.14.1 does not treat export as a declaration utility; leaves argument unexpanded
      [ "${lines[0]}" = "operand=<PATH=~/bin:~root/bin>" ]
      [ "${lines[1]}" = "export=<~/bin:~root/bin>" ]
      ;;
    dash | busybox | ksh93 | yash | zsh | zsh-sh)
      # POSIX sh: operands of ordinary commands only expand leading tilde; export expands assignments
      [ "${lines[0]}" = "operand=<PATH=~/bin:~root/bin>" ]
      [ "${lines[1]}" = "export=</mock/home/bin:$ROOT_DIR/bin>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "parameter expansion default expands tilde when unquoted but preserves literal when quoted" {
  export HOME="/mock/home"
  run_lab "$PROBE" pe-default
  [ "$status" -eq 0 ]
  case $(lab_shell_family) in
    yash)
      # yash expands unquoted tilde inside double-quoted parameter expansion default
      [ "$output" = "unquoted=</mock/home> quoted=</mock/home>" ]
      ;;
    bash | dash | posh | busybox | mksh | ksh93 | zsh | zsh-sh)
      # POSIX shells expand unquoted default, but double quotes suppress tilde expansion
      [ "$output" = "unquoted=</mock/home> quoted=<~>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "unquoted redirection operand undergoes tilde expansion across all shells" {
  target="$BATS_TEST_TMPDIR/redir_target"
  mkdir -p "$target"
  export HOME="$target"
  run_lab "$PROBE" redirection "$target"
  [ "$status" -eq 0 ]
  [ "$output" = "redirect=ok" ]
  [ -f "$target/probe.out" ]
}
