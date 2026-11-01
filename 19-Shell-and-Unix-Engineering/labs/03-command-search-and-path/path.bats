#!/usr/bin/env bats
# Tests for POSIX XCU 2.9.1.4 and XBD 8.3 Command Search and PATH semantics.

setup() {
  load ../lib/lab
  PROBE="${BATS_TEST_DIRNAME}/path-probe.sh"
}

# lab_host: print darwin on macOS, otherwise the ID field of /etc/os-release.
lab_host() {
  # shellcheck disable=SC1091 # the file exists only on the host under test
  case $(uname -s) in
    Darwin) echo darwin ;;
    *) (. /etc/os-release && printf '%s\n' "${ID}") ;;
  esac
}

# measured_nul_status FAMILY: print the measured status of an all-NUL ENOEXEC file for a shell
# whose answer differs by host, or fail when no measurement exists for that shell on this host.
# Debian dash has patch 0007 file_is_binary() and Debian mksh R59 2025 has a magic check, so both give 126.
# macOS dash re-executes /bin/sh, which is bash there and rejects the file with 126.
# On Alpine dash, mksh R59 2020, and yash re-execute /bin/sh, BusyBox ash, which deletes NUL bytes and exits 0.
measured_nul_status() {
  case "$1:$(lab_host)" in
    dash:debian | mksh:debian | dash:darwin) echo 126 ;;
    dash:alpine | mksh:alpine | yash:alpine) echo 0 ;;
    *) return 1 ;;
  esac
}

@test "probe runs and verifies search order, slash bypass, empty prefix, hashing, and ENOEXEC" {
  case $(lab_shell_family) in
    ksh93)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_search_order:   func=func_ran command=ext_ran" ]
      [ "${lines[1]}" = "2_slash_bypass:   slash=ext_ran" ]
      [ "${lines[2]}" = "3_empty_prefix:   empty_prefix=ran_cwd" ]
      [ "${lines[3]}" = "4_hash_cache:     before=version_a after=version_a" ]
      [ "${lines[4]}" = "5_enoexec_fall:   script=enoexec_ok badbin=126" ]
      [ "${lines[5]}" = "6_exit_statuses:  notfound=127 noexec=126 dir=126" ]
      [ "${lines[6]}" = "7_command_p:      poisoned=127 recovered=0" ]
      ;;
    posh)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_search_order:   func=func_ran command=ext_ran" ]
      [ "${lines[1]}" = "2_slash_bypass:   slash=ext_ran" ]
      [ "${lines[2]}" = "3_empty_prefix:   empty_prefix=ran_cwd" ]
      [ "${lines[3]}" = "4_hash_cache:     before=version_a after=version_a" ]
      [ "${lines[4]}" = "5_enoexec_fall:   script=enoexec_ok badbin=0" ]
      [ "${lines[5]}" = "6_exit_statuses:  notfound=127 noexec=126 dir=126" ]
      [ "${lines[6]}" = "7_command_p:      poisoned=127 recovered=0" ]
      ;;
    busybox)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_search_order:   func=func_ran command=ext_ran" ]
      [ "${lines[1]}" = "2_slash_bypass:   slash=ext_ran" ]
      [ "${lines[2]}" = "3_empty_prefix:   empty_prefix=ran_cwd" ]
      [ "${lines[3]}" = "4_hash_cache:     before=version_b after=version_a" ]
      [ "${lines[4]}" = "5_enoexec_fall:   script=enoexec_ok badbin=0" ]
      [ "${lines[5]}" = "6_exit_statuses:  notfound=127 noexec=126 dir=126" ]
      [ "${lines[6]}" = "7_command_p:      poisoned=127 recovered=0" ]
      ;;
    bash | zsh | zsh-sh)
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_search_order:   func=func_ran command=ext_ran" ]
      [ "${lines[1]}" = "2_slash_bypass:   slash=ext_ran" ]
      [ "${lines[2]}" = "3_empty_prefix:   empty_prefix=ran_cwd" ]
      [ "${lines[3]}" = "4_hash_cache:     before=version_b after=version_a" ]
      [ "${lines[4]}" = "5_enoexec_fall:   script=enoexec_ok badbin=126" ]
      [ "${lines[5]}" = "6_exit_statuses:  notfound=127 noexec=126 dir=126" ]
      [ "${lines[6]}" = "7_command_p:      poisoned=127 recovered=0" ]
      ;;
    dash | mksh | yash)
      family=$(lab_shell_family)
      host=$(lab_host)
      st_nul=$(measured_nul_status "${family}") || skip "no measured all-NUL status for ${family} on ${host}"
      run_lab "${PROBE}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "1_search_order:   func=func_ran command=ext_ran" ]
      [ "${lines[1]}" = "2_slash_bypass:   slash=ext_ran" ]
      [ "${lines[2]}" = "3_empty_prefix:   empty_prefix=ran_cwd" ]
      [ "${lines[3]}" = "4_hash_cache:     before=version_b after=version_a" ]
      [ "${lines[4]}" = "5_enoexec_fall:   script=enoexec_ok badbin=${st_nul}" ]
      [ "${lines[5]}" = "6_exit_statuses:  notfound=127 noexec=126 dir=126" ]
      [ "${lines[6]}" = "7_command_p:      poisoned=127 recovered=0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command utility suppresses shell function lookup" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    zsh)
      # Native zsh command bypasses builtins and searches only PATH external utilities
      run_lab_c '
        cd() { printf "fn_cd"; }
        my_echo() { printf "fn_echo"; }
        sandbox=$(mktemp -d)
        printf "#!/bin/sh\nprintf external_my_echo\n" > "${sandbox}/my_echo"
        chmod +x "${sandbox}/my_echo"
        PATH="${sandbox}:${PATH}"
        out1=$(my_echo)
        out2=$(command my_echo)
        before=$(pwd)
        command cd "${sandbox}" 2>/dev/null
        after=$(pwd)
        [ "${before}" = "${after}" ] && same=unchanged || same=changed
        rm -rf "${sandbox}"
        printf "%s|%s|%s" "${out1}" "${out2}" "${same}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "fn_echo|external_my_echo|unchanged" ]
      ;;
    bash | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      # POSIX shells: command suppresses function lookup; command cd invokes cd builtin
      run_lab_c '
        cd() { printf "fn_cd"; }
        my_echo() { printf "fn_echo"; }
        sandbox=$(mktemp -d)
        printf "#!/bin/sh\nprintf external_my_echo\n" > "${sandbox}/my_echo"
        chmod +x "${sandbox}/my_echo"
        PATH="${sandbox}:${PATH}"
        out1=$(my_echo)
        out2=$(command my_echo)
        command cd /
        cur_pwd=$(pwd)
        rm -rf "${sandbox}"
        printf "%s|%s|%s" "${out1}" "${out2}" "${cur_pwd}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "fn_echo|external_my_echo|/" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command containing slash bypasses shell functions and PATH search" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      run_lab_c '
        sandbox=$(mktemp -d)
        printf "#!/bin/sh\nprintf external_binary\n" > "${sandbox}/tool"
        chmod +x "${sandbox}/tool"
        tool() { printf "function_override"; }
        out1=$(tool)
        out2=$("${sandbox}/tool")
        out3=$(cd "${sandbox}" && ./tool)
        rm -rf "${sandbox}"
        printf "%s|%s|%s" "${out1}" "${out2}" "${out3}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "function_override|external_binary|external_binary" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "empty PATH element resolves to current working directory" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/cwd" "${sandbox}/fallback"
        printf "#!/bin/sh\nprintf in_cwd\n" > "${sandbox}/cwd/target"
        chmod +x "${sandbox}/cwd/target"
        printf "#!/bin/sh\nprintf in_fallback\n" > "${sandbox}/fallback/target"
        chmod +x "${sandbox}/fallback/target"

        # Leading colon
        out_lead=$(cd "${sandbox}/cwd" && PATH=":${sandbox}/fallback" target)
        # Middle colon with empty entry: nothing before it holds target
        out_mid=$(cd "${sandbox}/cwd" && PATH="${sandbox}/none::${sandbox}/fallback" target)
        # Trailing colon with empty entry: nothing before it holds target
        out_trail=$(cd "${sandbox}/cwd" && PATH="${sandbox}/none:" target)

        rm -rf "${sandbox}"
        printf "%s|%s|%s" "${out_lead}" "${out_mid}" "${out_trail}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "in_cwd|in_cwd|in_cwd" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command location caching retains remembered path until hash -r or PATH reassignment" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    posh | ksh93)
      # ksh93 walks PATH for each external command despite its hash entry; posh has no command hash table
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/d1" "${sandbox}/d2"
        printf "#!/bin/sh\nprintf v2\n" > "${sandbox}/d2/runner"
        chmod +x "${sandbox}/d2/runner"
        PATH="${sandbox}/d1:${sandbox}/d2:${PATH}"
        export PATH
        runner >/dev/null
        printf "#!/bin/sh\nprintf v1\n" > "${sandbox}/d1/runner"
        chmod +x "${sandbox}/d1/runner"
        out_shadow=$(runner)
        hash -r 2>/dev/null || rehash 2>/dev/null || true
        out_flushed=$(runner)
        rm -rf "${sandbox}"
        printf "%s|%s" "${out_shadow}" "${out_flushed}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "v1|v1" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | mksh | dash)
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/d1" "${sandbox}/d2"
        printf "#!/bin/sh\nprintf v2\n" > "${sandbox}/d2/runner"
        chmod +x "${sandbox}/d2/runner"
        PATH="${sandbox}/d1:${sandbox}/d2:${PATH}"
        export PATH
        runner >/dev/null
        printf "#!/bin/sh\nprintf v1\n" > "${sandbox}/d1/runner"
        chmod +x "${sandbox}/d1/runner"
        out_shadow=$(runner)
        hash -r 2>/dev/null || rehash 2>/dev/null || true
        out_flushed=$(runner)
        rm -rf "${sandbox}"
        printf "%s|%s" "${out_shadow}" "${out_flushed}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "v2|v1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "cached command failure triggers repeated PATH search under POSIX semantics" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    bash)
      # Test both POSIX mode (repeats search) and non-POSIX mode (fails with ENOENT unless checkhash)
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/d1" "${sandbox}/d2"
        printf "#!/bin/sh\nprintf first\n" > "${sandbox}/d1/app"
        printf "#!/bin/sh\nprintf second\n" > "${sandbox}/d2/app"
        chmod +x "${sandbox}/d1/app" "${sandbox}/d2/app"
        PATH="${sandbox}/d1:${sandbox}/d2:${PATH}"
        export PATH
        app >/dev/null
        rm "${sandbox}/d1/app"
        set +o posix
        st_native=0
        app >/dev/null 2>&1 || st_native=$?
        set -o posix
        out_posix=$(app)
        rm -rf "${sandbox}"
        printf "%s|%s" "${st_native}" "${out_posix}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "127|second" ]
      ;;
    zsh | zsh-sh | yash | busybox | posh | mksh | ksh93 | dash)
      # Re-search on failure is standard behavior
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/d1" "${sandbox}/d2"
        printf "#!/bin/sh\nprintf first\n" > "${sandbox}/d1/app"
        printf "#!/bin/sh\nprintf second\n" > "${sandbox}/d2/app"
        chmod +x "${sandbox}/d1/app" "${sandbox}/d2/app"
        PATH="${sandbox}/d1:${sandbox}/d2:${PATH}"
        export PATH
        app >/dev/null
        rm "${sandbox}/d1/app"
        out=$(app)
        rm -rf "${sandbox}"
        printf "%s" "${out}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "second" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "exit status 127 for missing commands and unsearchable directories versus 126 for unexecutable files" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    mksh | posh)
      # pdksh derivatives treat unsearchable directory on direct pathname as command not found (127)
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/unsearchable"
        printf "#!/bin/sh\nprintf ok\n" > "${sandbox}/unsearchable/prog"
        chmod +x "${sandbox}/unsearchable/prog"
        chmod 600 "${sandbox}/unsearchable"

        st_direct=0
        "${sandbox}/unsearchable/prog" 2>/dev/null || st_direct=$?

        st_path=0
        (PATH="${sandbox}/unsearchable:${PATH}" prog 2>/dev/null) || st_path=$?

        chmod 700 "${sandbox}/unsearchable"

        touch "${sandbox}/noexec_file"
        chmod 000 "${sandbox}/noexec_file"
        st_noexec=0
        "${sandbox}/noexec_file" 2>/dev/null || st_noexec=$?

        mkdir "${sandbox}/dir_cmd"
        st_dir=0
        "${sandbox}/dir_cmd" 2>/dev/null || st_dir=$?

        rm -rf "${sandbox}"
        printf "%s|%s|%s|%s" "${st_direct}" "${st_path}" "${st_noexec}" "${st_dir}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "127|127|126|126" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | ksh93 | dash)
      run_lab_c '
        sandbox=$(mktemp -d)
        mkdir "${sandbox}/unsearchable"
        printf "#!/bin/sh\nprintf ok\n" > "${sandbox}/unsearchable/prog"
        chmod +x "${sandbox}/unsearchable/prog"
        chmod 600 "${sandbox}/unsearchable"

        st_direct=0
        "${sandbox}/unsearchable/prog" 2>/dev/null || st_direct=$?

        st_path=0
        (PATH="${sandbox}/unsearchable:${PATH}" prog 2>/dev/null) || st_path=$?

        chmod 700 "${sandbox}/unsearchable"

        touch "${sandbox}/noexec_file"
        chmod 000 "${sandbox}/noexec_file"
        st_noexec=0
        "${sandbox}/noexec_file" 2>/dev/null || st_noexec=$?

        mkdir "${sandbox}/dir_cmd"
        st_dir=0
        "${sandbox}/dir_cmd" 2>/dev/null || st_dir=$?

        rm -rf "${sandbox}"
        printf "%s|%s|%s|%s" "${st_direct}" "${st_path}" "${st_noexec}" "${st_dir}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "126|127|126|126" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "ENOEXEC fallback executes script without shebang and rejects binary with NUL byte" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    busybox | posh)
      # BusyBox ash silently discards NUL bytes (shell/ash.c); posh executes as empty script
      run_lab_c '
        sandbox=$(mktemp -d)
        printf "printf script_success\n" > "${sandbox}/plain_script"
        chmod +x "${sandbox}/plain_script"
        out_script=$("${sandbox}/plain_script")

        printf "\0\0\0\0\0\0\0\0" > "${sandbox}/corrupt_bin"
        chmod +x "${sandbox}/corrupt_bin"
        st_bin=0
        ("${sandbox}/corrupt_bin") 2>/dev/null || st_bin=$?

        rm -rf "${sandbox}"
        printf "%s|%s" "${out_script}" "${st_bin}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "script_success|0" ]
      ;;
    bash | zsh | zsh-sh | ksh93)
      # Native binary heuristic check: inspects buffer for NUL bytes and returns 126
      run_lab_c '
        sandbox=$(mktemp -d)
        printf "printf script_success\n" > "${sandbox}/plain_script"
        chmod +x "${sandbox}/plain_script"
        out_script=$("${sandbox}/plain_script")

        printf "\0\0\0\0\0\0\0\0" > "${sandbox}/corrupt_bin"
        chmod +x "${sandbox}/corrupt_bin"
        st_bin=0
        ("${sandbox}/corrupt_bin") 2>/dev/null || st_bin=$?

        rm -rf "${sandbox}"
        printf "%s|%s" "${out_script}" "${st_bin}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "script_success|126" ]
      ;;
    dash | mksh | yash)
      # The status differs by host; see measured_nul_status for each mechanism.
      family=$(lab_shell_family)
      host=$(lab_host)
      st_fallback=$(measured_nul_status "${family}") || skip "no measured all-NUL status for ${family} on ${host}"

      run_lab_c '
        sandbox=$(mktemp -d)
        printf "printf script_success\n" > "${sandbox}/plain_script"
        chmod +x "${sandbox}/plain_script"
        out_script=$("${sandbox}/plain_script")

        printf "\0\0\0\0\0\0\0\0" > "${sandbox}/corrupt_bin"
        chmod +x "${sandbox}/corrupt_bin"
        st_bin=0
        ("${sandbox}/corrupt_bin") 2>/dev/null || st_bin=$?

        rm -rf "${sandbox}"
        printf "%s|%s" "${out_script}" "${st_bin}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "script_success|${st_fallback}" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command -p uses default system PATH to find standard utilities" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      run_lab_c '
        st_poison=0
        (PATH="/nonexistent" ls /dev/null 2>/dev/null) || st_poison=$?
        st_recov=0
        (PATH="/nonexistent" command -p ls /dev/null >/dev/null 2>&1) || st_recov=$?
        printf "%s|%s" "${st_poison}" "${st_recov}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "127|0" ]
      ;;
    ksh93)
      run_lab_c '
        st_poison=0
        (PATH="/nonexistent" ls /dev/null 2>/dev/null) || st_poison=$?
        st_recov=0
        (PATH="/nonexistent" command -p ls /dev/null >/dev/null 2>&1) || st_recov=$?
        printf "%s|%s" "${st_poison}" "${st_recov}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "127|0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "command -v accurately reports builtins, functions, and external executables" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    dash | busybox)
      # dash and busybox ash exit with 127 on command -v failure
      run_lab_c '
        probe_func() { :; }
        v_bltn=$(command -v set)
        v_func=$(command -v probe_func)
        v_util=$(command -v ls)
        st_missing=0
        command -v nonexistent_tool_xyz >/dev/null 2>&1 || st_missing=$?
        printf "%s|%s|%s|%s" "${v_bltn}" "${v_func}" "${v_util##*/}" "${st_missing}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "set|probe_func|ls|127" ]
      ;;
    bash | zsh | zsh-sh | yash | posh | mksh | ksh93)
      run_lab_c '
        probe_func() { :; }
        v_bltn=$(command -v set)
        v_func=$(command -v probe_func)
        v_util=$(command -v ls)
        st_missing=0
        command -v nonexistent_tool_xyz >/dev/null 2>&1 || st_missing=$?
        printf "%s|%s|%s|%s" "${v_bltn}" "${v_func}" "${v_util##*/}" "${st_missing}"
      '
      [ "${status}" -eq 0 ]
      [ "${output}" = "set|probe_func|ls|1" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "plain foreground ENOEXEC file in a fresh shell runs, except Debian ksh93u+m 1.0.10 on arm64 crashes" {
  # Each run pins its environment, because this ksh93 build's crash status depends on the imported variables.
  local sandbox shell_path version
  sandbox=${BATS_TEST_TMPDIR}/fresh
  mkdir "${sandbox}"
  printf 'printf script_success\n' >"${sandbox}/plain_script"
  printf '\0\0\0\0\0\0\0\0' >"${sandbox}/corrupt_bin"
  chmod +x "${sandbox}/plain_script" "${sandbox}/corrupt_bin"
  cd "${sandbox}"
  lab_shell_words
  shell_path=$(command -v "${LAB_SHELL_WORDS[0]}")
  LAB_SHELL_WORDS[0]=${shell_path}
  run_pinned() {
    run env -i PATH=/usr/bin:/bin PWD="${sandbox}" "${LAB_SHELL_WORDS[@]}" -c "$1"
  }
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case $(lab_shell_family) in
    ksh93)
      version=$(lab_shell_version)
      case ${version}:$(uname -m) in
        *93u+m/1.0.10*:aarch64)
          run_pinned './plain_script; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${lines[0]}" = "realloc(): invalid pointer" ]
          [ "${lines[${#lines[@]} - 1]}" = "|262" ]
          run_pinned './corrupt_bin 2>/dev/null; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "|262" ]
          # A subshell and a command substitution run the file normally.
          run_pinned '(./plain_script); printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "script_success|0" ]
          run_pinned 'x=$(./plain_script); printf "%s|%s" "${x}" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "script_success|0" ]
          # With only PATH imported the script runs, then the shell takes SIGSEGV; with nothing imported it is clean.
          run env -i PATH=/usr/bin:/bin "${LAB_SHELL_WORDS[@]}" -c './plain_script; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [[ "${lines[0]}" == script_success* ]]
          [ "${lines[${#lines[@]} - 1]}" = "|267" ]
          run env -i "${LAB_SHELL_WORDS[@]}" -c './plain_script; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "script_success|0" ]
          ;;
        *93u+\ 2012*:*)
          run_pinned './plain_script; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "script_success|0" ]
          run_pinned './corrupt_bin 2>/dev/null; printf "|%s" "$?"'
          [ "${status}" -eq 0 ]
          [ "${output}" = "|126" ]
          ;;
        *)
          machine=$(uname -m)
          skip "no measured result for ksh ${version} on ${machine}"
          ;;
      esac
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | dash)
      run_pinned './plain_script; printf "|%s" "$?"'
      [ "${status}" -eq 0 ]
      [ "${output}" = "script_success|0" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "special built-in function definition: rejected, or accepted with the function or the built-in winning" {
  local family version
  family=$(lab_shell_family)
  # shellcheck disable=SC2016 # expressions run inside shell under test
  case ${family} in
    bash)
      # Set the mode explicitly: a bash invoked as sh starts in POSIX mode.
      run_lab_c 'set +o posix; eval "exit() { printf fn; }"; exit 7; printf after'
      [ "${status}" -eq 0 ]
      [ "${output}" = "fnafter" ]
      version=$(lab_shell_version)
      run_lab_c 'set -o posix; eval "exit() { printf fn; }"; exit 7; printf after'
      case ${version} in
        3.2.*)
          # Accepted, and the special built-in wins.
          [ "${status}" -eq 7 ]
          [ "${output}" = "" ]
          ;;
        *)
          # Rejected, and the failed special built-in eval ends the shell.
          [ "${status}" -ne 0 ]
          [ "${status}" -ne 7 ]
          [ "${output##*: }" = "is a special builtin" ]
          ;;
      esac
      ;;
    zsh | zsh-sh | busybox)
      run_lab_c 'exit() { printf fn; }; exit 7; printf after'
      [ "${status}" -eq 0 ]
      [ "${output}" = "fnafter" ]
      ;;
    mksh | posh | yash)
      run_lab_c 'exit() { printf fn; }; exit 7; printf after'
      [ "${status}" -eq 7 ]
      [ "${output}" = "" ]
      ;;
    dash)
      run_lab_c 'exit() { printf fn; }; exit 7; printf after'
      [ "${status}" -eq 2 ]
      [[ "${output}" == *"Bad function name"* ]]
      ;;
    ksh93)
      run_lab_c 'exit() { printf fn; }; exit 7; printf after'
      [ "${status}" -eq 1 ]
      [[ "${output}" == *"exit: invalid function name"* ]]
      ;;
    *)
      false
      ;;
  esac
}

@test "percent-builtin PATH element moves regular built-ins after earlier PATH entries only in dash" {
  # shellcheck disable=SC2016 # expressions run inside shell under test
  run_lab_c '
    sandbox=$(mktemp -d)
    printf "#!/bin/sh\nprintf external\n" > "${sandbox}/echo"
    chmod +x "${sandbox}/echo"
    saved=${PATH}
    PATH="${sandbox}:%builtin:${saved}"
    after=$(echo builtin)
    PATH="%builtin:${sandbox}:${saved}"
    before=$(echo builtin)
    rm -rf "${sandbox}"
    printf "%s|%s" "${after}" "${before}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    dash)
      [ "${output}" = "external|builtin" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox | posh | mksh | ksh93)
      [ "${output}" = "builtin|builtin" ]
      ;;
    *)
      false
      ;;
  esac
}
