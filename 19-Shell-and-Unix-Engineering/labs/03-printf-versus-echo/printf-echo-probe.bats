#!/usr/bin/env bats
# Tests for printf versus echo behavioral differences, portability,
# and specification conformance under POSIX.1-2024.

setup() {
  load ../lib/lab
  PROBE="$BATS_TEST_DIRNAME/printf-echo-probe.sh"
}

@test "leading-dash: printf formats arbitrary '-' strings safely while echo diverges on flags" {
  run_lab "$PROBE" leading-dash
  [ "$status" -eq 0 ]
  probe_out="$output"

  fam=$(lab_shell_family)
  case ${fam} in
    bash)
      posix_mode=no
      case $(lab_shell_version) in
        3.2*)
          run_lab_c '[ -o posix ]'
          if [ "$status" -eq 0 ]; then
            posix_mode=yes
          fi
          ;;
        *)
          ;;
      esac
      if [ "$posix_mode" = yes ]; then
        # bash 3.2 in POSIX mode (e.g. macOS /bin/sh) rejects options and prints literal -n / -e
        [ "$probe_out" = "echo_n=-n\n printf_n=-n\n echo_e=-e\n printf_e=-e\n echo_dd=-- --\n printf_dd=--\n" ]
      else
        [ "$probe_out" = "echo_n=EMPTY printf_n=-n\n echo_e=\n printf_e=-e\n echo_dd=-- --\n printf_dd=--\n" ]
      fi
      ;;
    ksh93 | zsh | zsh-sh | busybox | mksh)
      [ "$probe_out" = "echo_n=EMPTY printf_n=-n\n echo_e=\n printf_e=-e\n echo_dd=-- --\n printf_dd=--\n" ]
      ;;
    dash | posh)
      [ "$probe_out" = "echo_n=EMPTY printf_n=-n\n echo_e=-e\n printf_e=-e\n echo_dd=-- --\n printf_dd=--\n" ]
      ;;
    yash)
      # yash strictly conforms to POSIX XSI echo (ECHO_STYLE=SYSV): no option parsing
      [ "$probe_out" = "echo_n=-n\n printf_n=-n\n echo_e=-e\n printf_e=-e\n echo_dd=-- --\n printf_dd=--\n" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "escapes: printf %s preserves backslashes while %b expands them; echo diverges on default expansion" {
  run_lab "$PROBE" escapes
  [ "$status" -eq 0 ]
  probe_out="$output"

  fam=$(lab_shell_family)
  case ${fam} in
    bash)
      posix_mode=no
      case $(lab_shell_version) in
        3.2*)
          run_lab_c '[ -o posix ]'
          if [ "$status" -eq 0 ]; then
            posix_mode=yes
          fi
          ;;
        *)
          ;;
      esac
      if [ "$posix_mode" = yes ]; then
        # bash 3.2 in POSIX mode expands backslashes and halts at \c
        [ "$probe_out" = "echo_nl_multi=yes printf_s_literal=yes printf_b_multi=yes echo_c_halted=yes printf_b_c_halted=yes printf_s_c_literal=yes" ]
      else
        [ "$probe_out" = "echo_nl_multi=no printf_s_literal=yes printf_b_multi=yes echo_c_halted=no printf_b_c_halted=yes printf_s_c_literal=yes" ]
      fi
      ;;
    dash | zsh | yash)
      [ "$probe_out" = "echo_nl_multi=yes printf_s_literal=yes printf_b_multi=yes echo_c_halted=yes printf_b_c_halted=yes printf_s_c_literal=yes" ]
      ;;
    mksh | posh)
      # mksh and posh expand \n, but \c suppresses trailing newline without halting output of remaining characters
      [ "$probe_out" = "echo_nl_multi=yes printf_s_literal=yes printf_b_multi=yes echo_c_halted=no printf_b_c_halted=yes printf_s_c_literal=yes" ]
      ;;
    ksh93 | zsh-sh | busybox)
      [ "$probe_out" = "echo_nl_multi=no printf_s_literal=yes printf_b_multi=yes echo_c_halted=no printf_b_c_halted=yes printf_s_c_literal=yes" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "suppress-newline: printf '%s' suppresses newline universally while echo -n fails on '-n' payload" {
  run_lab "$PROBE" suppress-newline
  [ "$status" -eq 0 ]
  probe_out="$output"

  fam=$(lab_shell_family)
  case ${fam} in
    bash)
      posix_mode=no
      case $(lab_shell_version) in
        3.2*)
          run_lab_c '[ -o posix ]'
          if [ "$status" -eq 0 ]; then
            posix_mode=yes
          fi
          ;;
        *)
          ;;
      esac
      if [ "$posix_mode" = yes ]; then
        # bash 3.2 in POSIX mode does not treat -n as an option; prints both -n arguments with newline
        [ "$probe_out" = "echo_nonl=-n -n\n printf_nonl=-n" ]
      else
        [ "$probe_out" = "echo_nonl=EMPTY printf_nonl=-n" ]
      fi
      ;;
    yash)
      # yash does not recognize -n as an option; outputs both arguments as operands
      [ "$probe_out" = "echo_nonl=-n -n\n printf_nonl=-n" ]
      ;;
    dash)
      # dash checks only argv[1] for -n; the second -n is output as operand without trailing newline
      [ "$probe_out" = "echo_nonl=-n printf_nonl=-n" ]
      ;;
    ksh93 | zsh | zsh-sh | busybox | mksh | posh)
      # Option-looping shells consume all consecutive -n arguments, printing nothing
      [ "$probe_out" = "echo_nonl=EMPTY printf_nonl=-n" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "reuse: printf recycles format string and substitutes null string or zero for missing operands" {
  run_lab "$PROBE" reuse
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    busybox)
      # BusyBox ash runs its printf applet, which passes an empty string to numeric conversions for missing operands,
      # writing '<applet>: invalid number \x27\x27' to stderr; applet is 'sh' or 'ash'
      case $output in
        "sh: invalid number ''"$'\n'"multi=<alpha><beta><gamma> pair=<1,2><3,> empty=<><0>" | \
          "ash: invalid number ''"$'\n'"multi=<alpha><beta><gamma> pair=<1,2><3,> empty=<><0>")
          ;;
        *)
          false
          ;;
      esac
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          # Alpine mksh R59 2020/10/31 has no printf builtin; the BusyBox applet passes an empty string to numeric conversion, warning on stderr
          [ "$output" = "printf: invalid number ''"$'\n'"multi=<alpha><beta><gamma> pair=<1,2><3,> empty=<><0>" ]
          ;;
        *)
          # Debian mksh R59 2025/04/26 has no printf builtin; coreutils printf evaluates a missing numeric operand as 0 silently
          [ "$output" = "multi=<alpha><beta><gamma> pair=<1,2><3,> empty=<><0>" ]
          ;;
      esac
      ;;
    bash | dash | ksh93 | posh | yash | zsh | zsh-sh)
      [ "$output" = "multi=<alpha><beta><gamma> pair=<1,2><3,> empty=<><0>" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "closed-fd: printf and echo report write failure on closed stdout except in zsh" {
  run_lab "$PROBE" closed-fd
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    zsh | zsh-sh)
      # zsh CLOSE_CLEANLY macro treats errno == EBADF on stdout as clean completion
      [ "$output" = "echo_status=0 printf_status=0" ]
      ;;
    bash | dash | ksh93 | busybox | mksh | posh | yash)
      [ "$output" = "echo_status=1 printf_status=1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "format-injection: passing variable directly as format string triggers conversion errors" {
  run_lab "$PROBE" format-injection
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    busybox)
      # In '100% discount', % d is parsed as %d with space flag and missing operand;
      # BusyBox ash writes '<applet>: invalid number \x27\x27' to stderr (captured via 2>&1 into raw_vuln)
      case $output in
        "vuln=[100sh: invalid number ''"$'\n'" 0iscount] safe=[100% discount] missing=[user: ]" | \
          "vuln=[100ash: invalid number ''"$'\n'" 0iscount] safe=[100% discount] missing=[user: ]")
          ;;
        *)
          false
          ;;
      esac
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          # Alpine mksh R59 2020/10/31 has no printf builtin; the BusyBox applet's warning is captured into raw_vuln via 2>&1
          [ "$output" = "vuln=[100printf: invalid number ''"$'\n'" 0iscount] safe=[100% discount] missing=[user: ]" ]
          ;;
        *)
          # Debian mksh R59 2025/04/26 has no printf builtin; coreutils printf converts the missing operand to 0 without warning
          [ "$output" = "vuln=[100 0iscount] safe=[100% discount] missing=[user: ]" ]
          ;;
      esac
      ;;
    bash | dash | ksh93 | posh | yash | zsh | zsh-sh)
      [ "$output" = "vuln=[100 0iscount] safe=[100% discount] missing=[user: ]" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "option-terminator: printf -- strips option delimiter while echo prints -- literally" {
  # shellcheck disable=SC2016 # format string and operands belong to shell under test
  run_lab_c 'printf -- "foo\n"; echo -- "foo"'
  [ "$status" -eq 0 ]
  [ "$output" = "foo
-- foo" ]

  # printf -- without operands must fail because no format operand remains
  run_lab_c 'printf -- >/dev/null 2>&1'
  [ "$status" -ne 0 ]
}

@test "printf-v: a variable format operand starting with -v assigns a shell variable only where printf has -v" {
  run_lab "$PROBE" printf-v
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    bash)
      # -vNAME and -v NAME both assign; -v NAME with no format operand is a usage error
      [ "$output" = "joined=value split=value bare=fail bare_name=orig" ]
      ;;
    ksh93)
      case $(lab_shell_version) in
        *93u+m*)
          [ "$output" = "joined=value split=value bare=fail bare_name=orig" ]
          ;;
        *)
          # macOS ksh93u+ 2012 has no printf -v
          [ "$output" = "joined=orig split=orig bare=fail bare_name=orig" ]
          ;;
      esac
      ;;
    zsh | zsh-sh)
      # zsh takes -v only as a separate word
      [ "$output" = "joined=orig split=value bare=fail bare_name=orig" ]
      ;;
    dash | yash)
      # no -v: the option is rejected, so nothing is assigned
      [ "$output" = "joined=orig split=orig bare=fail bare_name=orig" ]
      ;;
    mksh | posh | busybox)
      # the external or applet printf takes -v as part of the format and succeeds
      [ "$output" = "joined=orig split=orig bare=ok bare_name=orig" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "percent-off: printf \"50% off\" reads '% o' as the octal conversion with a space flag" {
  run_lab "$PROBE" percent-off
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    ksh93)
      # ksh93 emits a space before the zero, giving '50 0ff'
      [ "$output" = "out=[50 0ff] status=0" ]
      ;;
    busybox)
      # the missing operand is rejected as an invalid number, so the status is 1
      [ "$output" = "out=[500ff] status=1" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          # Alpine: the BusyBox applet
          [ "$output" = "out=[500ff] status=1" ]
          ;;
        *)
          # Debian: coreutils printf
          [ "$output" = "out=[500ff] status=0" ]
          ;;
      esac
      ;;
    bash | dash | posh | yash | zsh | zsh-sh)
      [ "$output" = "out=[500ff] status=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "positional: numbered conversions (%n\$) are accepted by ksh93, zsh, yash and coreutils, rejected by bash, dash and BusyBox" {
  run_lab "$PROBE" positional
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    ksh93 | zsh | zsh-sh | yash | posh)
      # posh runs coreutils printf, which accepts %4096$s
      [ "$output" = "pair=accept big=accept" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          # Alpine: the BusyBox applet rejects %n$
          [ "$output" = "pair=reject big=reject" ]
          ;;
        *)
          # Debian: coreutils printf
          [ "$output" = "pair=accept big=accept" ]
          ;;
      esac
      ;;
    bash | dash | busybox)
      [ "$output" = "pair=reject big=reject" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "closed-fd-stderr: only some shells say why a write to a closed stdout failed" {
  run_lab "$PROBE" closed-fd-stderr
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    bash | zsh | zsh-sh | yash)
      # zsh reports the error although its status is 0 (see closed-fd)
      [ "$output" = "echo_err=bad-fd printf_err=bad-fd" ]
      ;;
    dash)
      [ "$output" = "echo_err=io-error printf_err=io-error" ]
      ;;
    ksh93)
      [ "$output" = "echo_err=none printf_err=none" ]
      ;;
    busybox)
      # the shell reports the failed echo; the printf applet is silent
      [ "$output" = "echo_err=bad-fd printf_err=none" ]
      ;;
    posh)
      # echo is silent; the external coreutils printf reports
      [ "$output" = "echo_err=none printf_err=bad-fd" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          # Alpine: silent echo, silent BusyBox applet
          [ "$output" = "echo_err=none printf_err=none" ]
          ;;
        *)
          # Debian: mksh reports echo, coreutils reports printf
          [ "$output" = "echo_err=bad-fd printf_err=bad-fd" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "extra-operands: a format with no conversion discards extra operands; only coreutils printf warns" {
  run_lab "$PROBE" extra-operands
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    posh)
      [ "$output" = "out=[hi<NL>] stderr=yes" ]
      ;;
    mksh)
      case $(lab_shell_version) in
        *2020/10/31*)
          [ "$output" = "out=[hi<NL>] stderr=no" ]
          ;;
        *)
          [ "$output" = "out=[hi<NL>] stderr=yes" ]
          ;;
      esac
      ;;
    bash | dash | ksh93 | yash | zsh | zsh-sh | busybox)
      [ "$output" = "out=[hi<NL>] stderr=no" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "external: /bin/echo and /usr/bin/printf behave per platform, whatever the shell under test" {
  run_lab "$PROBE" external
  [ "$status" -eq 0 ]
  [ "$output" != "unavailable" ]

  ver=$(/bin/echo --version 2>&1)
  case ${ver} in
    *"GNU coreutils"*)
      # coreutils echo: -n and -e swallowed, no escapes, exit 1 on a closed stdout;
      # coreutils printf warns about extra operands but exits 0
      [ "$output" = "n=[] e=[<NL>] dd=[-- x<NL>] nl=[a\\nb<NL>] c=[a\\cb<NL>] nn=[] closed_status=1
extra=[hi<NL>] extra_stderr=yes extra_status=0 s=[-n<NL>] b=[a<NL>b<NL>] dd=[foo<NL>]" ]
      ;;
    *)
      case $(uname -s) in
        Darwin)
          # macOS echo: -n is an option, -e is text, \c halts; printf fails on extra operands
          [ "$output" = "n=[] e=[-e<NL>] dd=[-- x<NL>] nl=[a\\nb<NL>] c=[a] nn=[-n] closed_status=1
extra=[hi<NL>] extra_stderr=yes extra_status=1 s=[-n<NL>] b=[a<NL>b<NL>] dd=[foo<NL>]" ]
          ;;
        *)
          # BusyBox applets: like coreutils, but printf is silent about extra operands
          [ "$output" = "n=[] e=[<NL>] dd=[-- x<NL>] nl=[a\\nb<NL>] c=[a\\cb<NL>] nn=[] closed_status=1
extra=[hi<NL>] extra_stderr=no extra_status=0 s=[-n<NL>] b=[a<NL>b<NL>] dd=[foo<NL>]" ]
          ;;
      esac
      ;;
  esac
}

@test "builtin-lookup: echo is found with an empty PATH everywhere but yash, printf only where it is a builtin" {
  # shellcheck disable=SC2016 # the code belongs to the shell under test
  run_lab_c '(PATH=/nonexistent; printf "%s\n" hi) >/dev/null 2>&1; p=$?; (PATH=/nonexistent; echo hi) >/dev/null 2>&1; e=$?; echo "printf=$p echo=$e"'
  [ "$status" -eq 0 ]

  fam=$(lab_shell_family)
  case ${fam} in
    mksh | posh)
      # no printf builtin: the system utility is looked up through PATH
      [ "$output" = "printf=127 echo=0" ]
      ;;
    yash)
      # echo and printf are substitutive built-ins: they run only if the utility is found in PATH
      [ "$output" = "printf=127 echo=127" ]
      ;;
    bash | dash | ksh93 | zsh | zsh-sh | busybox)
      [ "$output" = "printf=0 echo=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "octal: %b turns a zero plus octal digits into that byte in every shell" {
  run_lab "$PROBE" octal
  [ "$status" -eq 0 ]
  [ "$output" = "octal=4178ff" ]
}
