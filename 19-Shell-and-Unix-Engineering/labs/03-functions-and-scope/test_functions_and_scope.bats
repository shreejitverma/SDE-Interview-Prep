#!/usr/bin/env bats
# Tests for POSIX shell function definition, call stack isolation, scoping, and shell divergences.

setup() {
  load ../lib/lab
  TASK_RUNNER="$BATS_TEST_DIRNAME/task_runner.sh"
}

# bash_is_posix: succeed when the bash under test already starts in POSIX mode (invoked as sh, or --posix).
bash_is_posix() {
  lab_shell_words
  "${LAB_SHELL_WORDS[@]}" -c '[ -o posix ]' 2>/dev/null
}

# run_bash_posix CODE [NAME [ARG...]]: run inline code in POSIX mode with the same bash binary,
# so the POSIX arms below execute under every bash in the matrix, not only under an "sh" entry.
run_bash_posix() {
  lab_shell_words
  run "${LAB_SHELL_WORDS[@]}" --posix -c "$@"
}

# run_yash_posixly CODE: run inline code with the same yash binary under -o posixly-correct.
run_yash_posixly() {
  lab_shell_words
  run "${LAB_SHELL_WORDS[@]}" -o posixly-correct -c "$@"
}

# output_has TEXT: succeed when the captured output contains TEXT.
output_has() {
  case "${output}" in
    *"$1"*) ;;
    *) false ;;
  esac
}

# output_lacks TEXT: succeed when the captured output does not contain TEXT.
output_lacks() {
  [[ "${output}" != *"$1"* ]]
}

@test "task_runner run preserves caller positional parameters across function shifts" {
  run_lab "${TASK_RUNNER}" run alpha beta gamma
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "BATCH_ITEM_1=alpha" ]
  [ "${lines[1]}" = "BATCH_ITEM_2=beta" ]
  [ "${lines[2]}" = "BATCH_ITEM_3=gamma" ]
  [ "${lines[3]}" = "CALLER_PARAMS_PRESERVED=alpha beta gamma" ]
  [ "${lines[4]}" = "CALLER_MATCH=true" ]
}

@test "task_runner sandbox isolates environment mutations and working directory via subshell" {
  run_lab "${TASK_RUNNER}" sandbox "${BATS_TEST_TMPDIR}"
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "SANDBOX_CWD=${BATS_TEST_TMPDIR}" ]
  [ "${lines[1]}" = "SANDBOX_STATE=mutated_inside_subshell" ]
  [ "${lines[3]}" = "PARENT_STATE=pristine" ]
  [ "${lines[4]}" = "CWD_PRESERVED=true" ]
}

@test "task_runner depth bounds recursion and returns status 2 on limit exceeded" {
  run_lab "${TASK_RUNNER}" depth 10
  [ "${status}" -eq 2 ]
  case "${output}" in
    *"Maximum recursion depth reached: 5"*) ;;
    *) false ;;
  esac
}

@test "positional parameters are isolated and restored across function call boundaries" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    set -- parent1 parent2 parent3
    f() {
      set -- child1 child2
      shift
      printf "%s|%s|%d\n" "$1" "$*" "$#"
    }
    f
    printf "%s|%s|%d\n" "$1" "$*" "$#"
  '
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "child2|child2|1" ]
  [ "${lines[1]}" = "parent1|parent1 parent2 parent3|3" ]
}

@test "subshell compound command function body isolates variable mutations and working directory" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    target="$1"
    f_sub() (
      var=mutated_child
      cd "$1"
      printf "%s|%s\n" "$var" "$PWD"
    )
    var=pristine_parent
    f_sub "$target"
    printf "%s|%s\n" "$var" "$PWD"
  ' "arg0" "${BATS_TEST_TMPDIR}"
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "mutated_child|${BATS_TEST_TMPDIR}" ]
  [ "${lines[1]}" = "pristine_parent|${PWD}" ]
}

@test "redirection attached to function definition is evaluated dynamically on each call" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    out_file="$1/first.log"
    emit_msg() {
      printf "%s\n" "$1"
    } >"$out_file"

    emit_msg "message_one"
    out_file="$1/second.log"
    emit_msg "message_two"

    cat "$1/first.log"
    cat "$1/second.log"
  ' "arg0" "${BATS_TEST_TMPDIR}"
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "message_one" ]
  [ "${lines[1]}" = "message_two" ]
}

@test "local is visible and writable down the call chain and restored on return, except in ksh93" {
  local code
  # shellcheck disable=SC2016 # expressions tested under target shell
  code='
    scoped_var="global_value"
    callee() {
      printf "callee_sees=%s\n" "${scoped_var}"
      scoped_var="callee_wrote"
    }
    caller() {
      local scoped_var="caller_value"
      callee
      printf "caller_sees=%s\n" "${scoped_var}"
    }
    caller
    printf "after_return=%s\n" "${scoped_var}"
  '
  run_lab_c "${code}"
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | dash | posh)
      [ "${lines[0]}" = "callee_sees=caller_value" ]
      [ "${lines[1]}" = "caller_sees=callee_wrote" ]
      [ "${lines[2]}" = "after_return=global_value" ]
      [ "${#lines[@]}" -eq 3 ]
      ;;
    ksh93)
      # ksh93 has no local built-in: the call fails and every reference is the one global variable
      output_has "local: not found"
      output_has "callee_sees=global_value"
      output_has "caller_sees=callee_wrote"
      output_has "after_return=callee_wrote"
      ;;
    *)
      false
      ;;
  esac
  lab_family=$(lab_shell_family)
  if [ "${lab_family}" = yash ]; then
    # yash refuses local under -o posixly-correct, so the variable stays global there too
    run_yash_posixly "${code}"
    [ "${status}" -eq 0 ]
    output_has "non-portable built-in is not supported"
    output_has "callee_sees=global_value"
    output_has "after_return=callee_wrote"
  fi
}

@test "ksh93 function syntax provides static lexical scoping with typeset" {
  local code
  # shellcheck disable=SC2016 # expressions tested under target shell
  code='
    function callee {
      printf "callee_sees=%s\n" "${x:-unset}"
    }
    function caller {
      typeset x="caller_scope"
      callee
      printf "caller_sees=%s\n" "${x}"
    }
    x="global_scope"
    caller
    printf "after_return=%s\n" "${x}"
  '
  run_lab_c "${code}"
  case $(lab_shell_family) in
    ksh93)
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "callee_sees=global_scope" ]
      [ "${lines[1]}" = "caller_sees=caller_scope" ]
      [ "${lines[2]}" = "after_return=global_scope" ]
      ;;
    bash | zsh | zsh-sh | mksh | yash)
      # Dynamic: the callee sees the caller's typeset, and the global comes back on return
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "callee_sees=caller_scope" ]
      [ "${lines[1]}" = "caller_sees=caller_scope" ]
      [ "${lines[2]}" = "after_return=global_scope" ]
      lab_family=$(lab_shell_family)
      if [ "${lab_family}" = yash ]; then
        run_yash_posixly "${code}"
        [ "${status}" -eq 2 ]
        output_has "syntax error: \`function' cannot be used as a command name"
        output_lacks "callee_sees="
      fi
      ;;
    busybox)
      # BusyBox ash accepts the function keyword extension, but lacks the typeset built-in
      [ "${status}" -eq 0 ]
      output_has "typeset: not found"
      output_has "callee_sees=global_scope"
      output_has "caller_sees=global_scope"
      output_has "after_return=global_scope"
      ;;
    dash)
      # function is an unknown command name, the printf line runs at top level, then the brace is rejected
      [ "${status}" -eq 2 ]
      output_has "function: not found"
      output_has "callee_sees=unset"
      output_has 'Syntax error: "}" unexpected'
      output_lacks "after_return="
      ;;
    posh)
      # posh rejects the construct with no output at all
      [ "${status}" -eq 1 ]
      output_has "syntax error: \`}' unexpected"
      output_lacks "callee_sees="
      ;;
    *)
      false
      ;;
  esac
}

@test "defining function with special built-in name exit is rejected by conforming shells" {
  local code='exit() { printf "override\n"; }; printf "defined\n"; exit 7'
  run_lab_c "${code}"
  case $(lab_shell_family) in
    dash)
      # POSIX XCU 2.9.5: function name shall not be a special built-in utility
      [ "${status}" -eq 2 ]
      output_has "Bad function name"
      output_lacks "defined"
      ;;
    ksh93)
      [ "${status}" -eq 1 ]
      output_has "invalid function name"
      output_lacks "defined"
      ;;
    bash)
      if ! bash_is_posix; then
        # Native bash allows functions to override special built-ins
        [ "${status}" -eq 0 ]
        [ "${lines[0]}" = "defined" ]
        [ "${lines[1]}" = "override" ]
      fi
      run_bash_posix "${code}"
      case $(lab_shell_version) in
        3.*)
          # Legacy bash 3.2 in POSIX mode allows definition but special built-in takes precedence
          [ "${status}" -eq 7 ]
          [ "${output}" = "defined" ]
          ;;
        *)
          # Modern bash in POSIX mode rejects defining a function with a special built-in name
          [ "${status}" -eq 2 ]
          output_has "is a special builtin"
          output_lacks "defined"
          ;;
      esac
      ;;
    busybox | zsh | zsh-sh)
      # BusyBox ash and zsh allow functions to override special built-ins
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "defined" ]
      [ "${lines[1]}" = "override" ]
      ;;
    mksh | posh | yash)
      # Function definition succeeds, but the special built-in runs: the shell exits 7, no override
      [ "${status}" -eq 7 ]
      [ "${output}" = "defined" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "function name containing hyphen is rejected by strict POSIX shells" {
  local code='task-exec() { printf "ok\n"; }; task-exec'
  run_lab_c "${code}"
  case $(lab_shell_family) in
    dash)
      # POSIX XBD 3.216: Name consists solely of alphanumeric characters and underscores
      [ "${status}" -eq 2 ]
      output_has "Bad function name"
      ;;
    ksh93 | posh)
      [ "${status}" -eq 1 ]
      output_has "invalid function name"
      ;;
    yash)
      [ "${status}" -eq 2 ]
      output_has "syntax error: invalid function name"
      ;;
    busybox | mksh | zsh | zsh-sh)
      # Extended identifier shells accept hyphens in function names
      [ "${status}" -eq 0 ]
      [ "${output}" = "ok" ]
      ;;
    bash)
      if ! bash_is_posix; then
        [ "${status}" -eq 0 ]
        [ "${output}" = "ok" ]
      fi
      run_bash_posix "${code}"
      case $(lab_shell_version) in
        3.* | 4.* | 5.0* | 5.1* | 5.2*)
          # Bash in POSIX mode prior to 5.3 enforces POSIX name rules
          [ "${status}" -eq 2 ]
          output_has "not a valid identifier"
          ;;
        *)
          # Bash 5.3 permits hyphens even in POSIX mode
          [ "${status}" -eq 0 ]
          [ "${output}" = "ok" ]
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "prefix variable assignment before function call leaks in ksh93 but is restored in other shells" {
  local code
  # shellcheck disable=SC2016 # expressions tested under target shell
  code='
    inspect_var() {
      printf "in_func=%s\n" "${PREFIX_TEST}"
    }
    PREFIX_TEST="initial_val"
    PREFIX_TEST="temp_val" inspect_var
    printf "after_func=%s\n" "${PREFIX_TEST}"
  '
  run_lab_c "${code}"
  [ "${status}" -eq 0 ]
  [ "${lines[0]}" = "in_func=temp_val" ]
  case $(lab_shell_family) in
    ksh93 | zsh-sh)
      # ksh93 and zsh in sh emulation mode leak assignment prefixes on function invocations
      [ "${lines[1]}" = "after_func=temp_val" ]
      ;;
    bash)
      if ! bash_is_posix; then
        # Native bash restores prefix assignments upon function return
        [ "${lines[1]}" = "after_func=initial_val" ]
      fi
      run_bash_posix "${code}"
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "in_func=temp_val" ]
      case $(lab_shell_version) in
        3.* | 4.*)
          # Legacy bash 3.2 and 4.4 in POSIX mode follow ksh semantics and leak prefix assignments
          [ "${lines[1]}" = "after_func=temp_val" ]
          ;;
        *)
          # Bash 5.1+ in POSIX mode restores prefix assignments per POSIX.1-2024
          [ "${lines[1]}" = "after_func=initial_val" ]
          ;;
      esac
      ;;
    zsh | yash | busybox | posh | mksh | dash)
      # POSIX standard allows restoring, which these shells implement
      [ "${lines[1]}" = "after_func=initial_val" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "return outside function terminates script in dash, zsh, and ksh but causes error in bash" {
  local code
  # shellcheck disable=SC2016 # expressions tested under target shell
  code='return 42; printf "unreachable=%d\n" "$?"'
  run_lab_c "${code}"
  case $(lab_shell_family) in
    dash | zsh | zsh-sh | ksh93 | mksh | posh | busybox | yash)
      # Exits script immediately with specified exit status
      [ "${status}" -eq 42 ]
      [ "${output}" = "" ]
      ;;
    bash)
      if ! bash_is_posix; then
        # In native bash, return outside function prints error diagnostic and continues execution
        [ "${status}" -eq 0 ]
        output_has "can only \`return' from a function or sourced script"
        case $(lab_shell_version) in
          3.* | 4.*) [ "${lines[1]}" = "unreachable=1" ] ;;
          *) [ "${lines[1]}" = "unreachable=2" ] ;;
        esac
      fi
      run_bash_posix "${code}"
      output_has "can only \`return' from a function or sourced script"
      case $(lab_shell_version) in
        3.* | 4.*)
          # Bash 3.2 and 4.4 in POSIX mode warn but continue execution
          [ "${status}" -eq 0 ]
          [ "${lines[1]}" = "unreachable=1" ]
          ;;
        *)
          # Bash 5.1+ in POSIX mode treats special built-in error as fatal (status 2)
          [ "${status}" -eq 2 ]
          output_lacks "unreachable="
          ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "return status greater than 255 is truncated to 8 bits in bash but preserved in dash and zsh memory" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    ret_wide() {
      return 257
    }
    ret_wide
    printf "STATUS=%d\n" "$?"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    bash | busybox | mksh)
      # GNU bash, BusyBox ash, and mksh mask return status to 8 bits: 257 & 255 = 1
      [ "${output}" = "STATUS=1" ]
      ;;
    dash | zsh | zsh-sh | ksh93 | yash | posh)
      # POSIX leaves >255 unspecified; these shells preserve full integer in $? memory
      [ "${output}" = "STATUS=257" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "inline local assignment with command substitution masks exit status" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    test_mask() {
      # shellcheck disable=SC2155 # intentional demonstration of SC2155
      local val=$(exit 42)
      printf "MASKED_CODE=%d\n" "$?"
    }
    test_mask
  '
  case $(lab_shell_family) in
    bash | zsh | zsh-sh | yash | busybox | mksh | dash | posh)
      [ "${status}" -eq 0 ]
      [ "${output}" = "MASKED_CODE=0" ]
      ;;
    ksh93)
      # local is not found (status 127), so $? captures the command-not-found error
      case "${output}" in
        *"local: not found"* | *"local:"*not*found*) ;;
        *) false ;;
      esac
      case "${output}" in
        *"MASKED_CODE=127"*) ;;
        *) false ;;
      esac
      ;;
    *)
      false
      ;;
  esac
}

@test "FUNCNEST bounds runaway recursion in bash and zsh and is ignored by every other shell" {
  local script="${BATS_TEST_TMPDIR}/funcnest.sh"
  # shellcheck disable=SC2016 # expressions tested under target shell
  printf '%s\n' \
    'FUNCNEST=5' \
    'depth=0' \
    'recurse() {' \
    '  depth=$((depth + 1))' \
    '  if [ "$depth" -ge 10 ]; then' \
    '    printf "unbounded_%d\n" "$depth"' \
    '    return 0' \
    '  fi' \
    '  recurse' \
    '}' \
    'recurse' \
    'printf "after status=%d depth=%d\n" "$?" "$depth"' >"${script}"
  run_lab "${script}"
  lab_family=$(lab_shell_family)
  lab_ver=$(lab_shell_version)
  case ${lab_family}:${lab_ver} in
    bash:3.*)
      # GNU bash 3.2 (macOS default) has no FUNCNEST (introduced in bash 4.2): the limit is ignored
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "unbounded_10" ]
      [ "${lines[1]}" = "after status=0 depth=10" ]
      ;;
    bash:*)
      # The call stack unwinds to the top level with status 1 and the script file carries on
      [ "${status}" -eq 0 ]
      output_has "maximum function nesting level exceeded (5)"
      output_lacks "unbounded_"
      [ "${lines[1]}" = "after status=1 depth=5" ]
      ;;
    zsh:* | zsh-sh:*)
      # zsh stops the script
      [ "${status}" -eq 1 ]
      output_has "maximum nested function level reached"
      output_lacks "unbounded_"
      output_lacks "after status="
      ;;
    dash:* | ksh93:* | mksh:* | yash:* | posh:* | busybox:*)
      # No FUNCNEST here: the assignment is an ordinary variable and the recursion runs past 5
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "unbounded_10" ]
      [ "${lines[1]}" = "after status=0 depth=10" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "built-in recursion caps stop zsh at 500, ksh93 at 1024, and Debian dash at 1000" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    printf "funcnest=%s\n" "${FUNCNEST-unset}"
    depth=0
    trap '\''printf "final_depth=%d\n" "$depth"'\'' EXIT
    recurse() {
      depth=$((depth + 1))
      if [ "$depth" -ge 1500 ]; then
        printf "reached_%d\n" "$depth"
        return 0
      fi
      recurse
    }
    recurse
  '
  local family
  family=$(lab_shell_family)
  if [ "${family}" = dash ] && [ ! -r /etc/debian_version ]; then
    # MAX_RECURSION is a Debian patch; upstream dash (Alpine, macOS) has no cap at this depth
    family=dash-upstream
  fi
  case "${family}" in
    zsh | zsh-sh)
      [ "${status}" -eq 1 ]
      [ "${lines[0]}" = "funcnest=500" ]
      output_has "maximum nested function level reached"
      output_lacks "reached_"
      # zsh skips the EXIT trap on this error, so log every depth to find where it stopped
      # shellcheck disable=SC2016 # expressions tested under target shell
      run_lab_c '
        depth=0
        recurse() {
          depth=$((depth + 1))
          printf "depth=%d\n" "$depth"
          recurse
        }
        recurse
      '
      [ "${status}" -eq 1 ]
      [ "${lines[$((${#lines[@]} - 2))]}" = "depth=500" ]
      ;;
    ksh93)
      [ "${status}" -eq 1 ]
      [ "${lines[0]}" = "funcnest=unset" ]
      output_has "recursion too deep"
      output_has "final_depth=1024"
      ;;
    dash)
      [ "${status}" -eq 2 ]
      [ "${lines[0]}" = "funcnest=unset" ]
      output_has "Maximum function recursion depth (1000) reached"
      output_has "final_depth=1000"
      ;;
    bash | mksh | yash | posh | busybox | dash-upstream)
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "funcnest=unset" ]
      [ "${lines[1]}" = "reached_1500" ]
      [ "${lines[2]}" = "final_depth=1500" ]
      ;;
    *)
      false
      ;;
  esac
}

@test "typeset in the fname() form is dynamic everywhere and global in ksh93" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    callee() {
      printf "callee_sees=%s\n" "${x}"
    }
    caller() {
      typeset x="caller_scope"
      callee
    }
    x="global_scope"
    caller
    printf "after_return=%s\n" "${x}"
  '
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    ksh93)
      # The assignment lands on the one global variable
      [ "${lines[0]}" = "callee_sees=caller_scope" ]
      [ "${lines[1]}" = "after_return=caller_scope" ]
      ;;
    bash | zsh | zsh-sh | mksh | yash)
      [ "${lines[0]}" = "callee_sees=caller_scope" ]
      [ "${lines[1]}" = "after_return=global_scope" ]
      ;;
    dash | posh | busybox)
      # typeset does not exist, so no assignment happens at all
      output_has "typeset: not found"
      output_has "callee_sees=global_scope"
      output_has "after_return=global_scope"
      ;;
    *)
      false
      ;;
  esac
}

@test "set inside a function leaks unless the shell localizes options" {
  local family probe_state
  family=$(lab_shell_family)
  # shellcheck disable=SC2016 # expressions tested under target shell
  probe_state='case $- in *f*) echo leaked ;; *) echo restored ;; esac'

  # Every shell: the fname() form with no localizing command leaks.
  run_lab_c "f() { set -f; }; f; ${probe_state}"
  [ "${status}" -eq 0 ]
  [ "${output}" = "leaked" ]

  # local - : alive is printed only when the function survives the declaration.
  run_lab_c "f() { local - >/dev/null 2>&1; echo alive; set -f; }; f; ${probe_state}"
  [ "${status}" -eq 0 ]
  case "${family}:$(lab_shell_version)" in
    bash:3.* | zsh:* | zsh-sh:* | ksh93:* | mksh:* | yash:*)
      [ "${lines[0]}" = "alive" ]
      [ "${lines[1]}" = "leaked" ]
      ;;
    bash:* | dash:* | busybox:*)
      [ "${lines[0]}" = "alive" ]
      [ "${lines[1]}" = "restored" ]
      ;;
    posh:*)
      # posh rejects local - and abandons the function, so set -f never runs
      [ "${output}" = "restored" ]
      ;;
    *)
      false
      ;;
  esac

  # The function keyword form: ksh93 and mksh localize options automatically.
  case "${family}" in
    ksh93 | mksh)
      run_lab_c "function g { set -f; }
g; ${probe_state}"
      [ "${status}" -eq 0 ]
      [ "${output}" = "restored" ]
      ;;
    bash | zsh | zsh-sh | yash | busybox)
      run_lab_c "function g { set -f; }
g; ${probe_state}"
      [ "${status}" -eq 0 ]
      [ "${output}" = "leaked" ]
      ;;
    dash)
      run_lab_c "function g { set -f; }
g; ${probe_state}"
      [ "${status}" -eq 2 ]
      output_has "Syntax error"
      ;;
    posh)
      run_lab_c "function g { set -f; }
g; ${probe_state}"
      [ "${status}" -eq 1 ]
      output_has "syntax error"
      ;;
    *)
      false
      ;;
  esac

  # zsh has its own option, native and emulating sh.
  if [ "${family}" = zsh ] || [ "${family}" = zsh-sh ]; then
    run_lab_c "f() { setopt localoptions; set -f; }; f; ${probe_state}"
    [ "${status}" -eq 0 ]
    [ "${output}" = "restored" ]
  fi
}

@test "\$0 keeps the script name in fname() except in native zsh, and names the function in ksh keyword functions" {
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c 'p() { printf "%s\n" "$0"; }; p' script_name
  [ "${status}" -eq 0 ]
  case $(lab_shell_family) in
    zsh) [ "${output}" = "p" ] ;;
    *) [ "${output}" = "script_name" ] ;;
  esac

  lab_family=$(lab_shell_family)
  if [ "${lab_family}" = bash ]; then
    # bash exposes the function name through FUNCNAME instead
    # shellcheck disable=SC2016 # expressions tested under target shell
    run_lab_c 'p() { printf "%s\n" "${FUNCNAME}"; }; p'
    [ "${status}" -eq 0 ]
    [ "${output}" = "p" ]
  fi

  # shellcheck disable=SC2016 # expressions tested under target shell
  local keyword_code='function k { printf "%s\n" "$0"; }
k'
  case $(lab_shell_family) in
    zsh | ksh93 | mksh)
      run_lab_c "${keyword_code}" script_name
      [ "${status}" -eq 0 ]
      [ "${output}" = "k" ]
      ;;
    bash | zsh-sh | yash | busybox)
      run_lab_c "${keyword_code}" script_name
      [ "${status}" -eq 0 ]
      [ "${output}" = "script_name" ]
      ;;
    dash)
      run_lab_c "${keyword_code}" script_name
      [ "${status}" -eq 2 ]
      output_has "Syntax error"
      ;;
    posh)
      run_lab_c "${keyword_code}" script_name
      [ "${status}" -eq 1 ]
      output_has "syntax error"
      ;;
    *)
      false
      ;;
  esac
}

@test "export -f reaches a child shell only in bash" {
  # shellcheck disable=SC2030 # consumed by the child processes this test starts
  export LAB_CHILD="${LAB_SHELL:-sh}"
  # shellcheck disable=SC2016 # expressions tested under target shell
  run_lab_c '
    f() { printf "inherited\n"; }
    export -f f
    printf "export_rc=%d\n" "$?"
    eval "${LAB_CHILD} -c f"
    printf "child_rc=%d\n" "$?"
  '
  case $(lab_shell_family) in
    bash)
      [ "${status}" -eq 0 ]
      [ "${lines[0]}" = "export_rc=0" ]
      [ "${lines[1]}" = "inherited" ]
      [ "${lines[2]}" = "child_rc=0" ]
      ;;
    zsh | zsh-sh | posh)
      # export rejects -f and the script carries on with no function in the child
      [ "${status}" -eq 0 ]
      output_has "export_rc=1"
      output_has "child_rc=127"
      output_lacks "inherited"
      ;;
    yash)
      [ "${status}" -eq 0 ]
      output_has "export_rc=2"
      output_has "child_rc=127"
      output_lacks "inherited"
      ;;
    dash | busybox | ksh93)
      # export is a special built-in here, so the usage error ends the shell
      [ "${status}" -eq 2 ]
      output_lacks "export_rc="
      ;;
    mksh)
      [ "${status}" -eq 1 ]
      output_lacks "export_rc="
      ;;
    *)
      false
      ;;
  esac

  lab_family=$(lab_shell_family)
  if [ "${lab_family}" = ksh93 ] || [ "${lab_family}" = mksh ]; then
    # typeset -xf is accepted, but the child shell still has no such function
    # shellcheck disable=SC2016 # expressions tested under target shell
    run_lab_c '
      f() { printf "inherited\n"; }
      typeset -xf f
      printf "typeset_rc=%d\n" "$?"
      eval "${LAB_CHILD} -c f"
      printf "child_rc=%d\n" "$?"
    '
    [ "${status}" -eq 0 ]
    [ "${lines[0]}" = "typeset_rc=0" ]
    output_has "child_rc=127"
    output_lacks "inherited"
  fi
}
