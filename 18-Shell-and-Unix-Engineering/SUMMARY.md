# Shell and Unix Engineering: Zero to Godhood - Complete Table of Contents

A mechanism-first curriculum for the shell, the Unix process model beneath it, the userland around it, and every production context it runs in.
Organized into 22 volumes in six phases (179 chapters) and 10 appendices.
See [README.md](./README.md) for the roadmap and how to study, [STYLE.md](./STYLE.md) for the chapter contract, and [labs/README.md](./labs/README.md) for the lab harness and shell matrix.

## Phase 1 - The Substrate

The kernel and terminal machinery every shell command runs on.

### [Volume 00 - The Unix Process Model Under the Shell](./Volume_00_The_Unix_Process_Model/)
1. [Processes: fork, exec, wait, and posix_spawn](./Volume_00_The_Unix_Process_Model/Chapter_01_Fork_Exec_Wait_and_Posix_Spawn.md) - Process creation and copy-on-write, vfork and posix_spawn, the exec family and PATH search in execvp, reaping with wait, and the case against fork in "A fork() in the road" (HotOS 2019).
2. [Program Startup: argv, envp, auxv, and ARG_MAX](./Volume_00_The_Unix_Process_Model/Chapter_02_Program_Startup_Argv_Envp_and_ARG_MAX.md) - How the kernel lays out the initial process stack, argument and environment size limits, E2BIG, and exactly what a shell passes to every child.
3. [File Descriptors and Open File Descriptions](./Volume_00_The_Unix_Process_Model/Chapter_03_File_Descriptors_and_Open_File_Descriptions.md) - The per-process descriptor table and the system-wide open file table, offsets shared after fork and dup, `O_CLOEXEC`, and descriptor inheritance across exec.
4. [Pipes and FIFOs](./Volume_00_The_Unix_Process_Model/Chapter_04_Pipes_and_FIFOs.md) - Pipe buffers and capacity, `PIPE_BUF` write atomicity, blocking and end-of-file semantics, SIGPIPE and EPIPE, and named pipes.
5. [Exit Status and Wait Status](./Volume_00_The_Unix_Process_Model/Chapter_05_Exit_Status_and_Wait_Status.md) - The wait status encoding and its macros, the 8-bit exit code, the 126, 127, and 128 plus signal conventions, and `sysexits.h`.
6. [Signals](./Volume_00_The_Unix_Process_Model/Chapter_06_Signals_for_Shell_Programmers.md) - Dispositions, masks, and pending sets, delivery and default actions, inheritance across fork and exec, and async-signal-safety.
7. [Sessions, Process Groups, and Controlling Terminals](./Volume_00_The_Unix_Process_Model/Chapter_07_Sessions_Process_Groups_and_Controlling_Terminals.md) - setsid and setpgid, the foreground process group, orphaned process groups, SIGHUP on hangup, and daemonization.
8. [Filesystem Semantics a Scripter Relies On](./Volume_00_The_Unix_Process_Model/Chapter_08_Filesystem_Semantics_a_Scripter_Relies_On.md) - Inodes, hard and symbolic links, permissions and umask, atomic rename, unlink while open, fsync durability, and why Linux ignores setuid on interpreted scripts.
9. [The /proc and /dev/fd Interfaces](./Volume_00_The_Unix_Process_Model/Chapter_09_The_proc_and_dev_fd_Interfaces.md) - Introspecting processes from the shell through `/proc/PID`, the `/dev/fd` and `/dev/stdin` files, and their macOS equivalents.

### [Volume 01 - Terminals, TTYs, and Job Control](./Volume_01_Terminals_TTYs_and_Job_Control/)
1. [The TTY Subsystem and the Line Discipline](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_01_TTY_Subsystem_and_Line_Discipline.md) - Canonical and raw modes, echo, special characters, signal-generating keys, and the exact point where Ctrl-C becomes SIGINT.
2. [termios and stty](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_02_Termios_and_Stty.md) - Terminal attributes, saving and restoring modes, and reading single keys and passwords safely from a script.
3. [Pseudo-Terminals](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_03_Pseudo_Terminals.md) - The pty master and slave pair, how ssh, tmux, script, and expect allocate them, and why programs behave differently without a terminal.
4. [Escape Sequences: ECMA-48, VT100, and xterm](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_04_Escape_Sequences_ECMA_48_VT100_and_Xterm.md) - Control sequences for cursor movement, color, and modes, OSC sequences, terminfo and tput, and terminal escape injection.
5. [Job Control End to End](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_05_Job_Control_End_to_End.md) - SIGTSTP, SIGTTIN, SIGTTOU, and SIGCONT, the foreground process group, and how fg, bg, jobs, and wait drive them.
6. [Line Editing: Readline and ZLE](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_06_Line_Editing_Readline_and_ZLE.md) - Keymaps, inputrc, history expansion, completion hooks, and line editor widgets.
7. [Terminal Multiplexers and Emulators](./Volume_01_Terminals_TTYs_and_Job_Control/Chapter_07_Terminal_Multiplexers_and_Emulators.md) - The tmux and screen client-server architecture, terminal emulator rendering, and TERM negotiation.

## Phase 2 - The Language

The shell command language as a specification, as implemented by bash, and across the other shells.

### [Volume 02 - The POSIX Shell Command Language](./Volume_02_The_POSIX_Shell_Command_Language/)
1. [Reading the POSIX Standard](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_01_Reading_the_POSIX_Standard.md) - How POSIX.1-2024 is organized into XBD, XSH, XCU, and XRAT, what conformance means, and how to cite a section precisely.
2. [Token Recognition and the Shell Grammar](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_02_Token_Recognition_and_the_Shell_Grammar.md) - Tokenization rules, operators, reserved words, alias substitution, and the formal grammar.
3. [Quoting](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_03_Quoting_Rules.md) - Backslash, single quotes, double quotes, and dollar-single-quotes, and exactly what each one preserves.
4. [The Expansion Pipeline](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_04_The_Expansion_Pipeline.md) - The exact order of tilde, parameter, command substitution, and arithmetic expansion, followed by field splitting, pathname expansion, and quote removal.
5. [Parameter Expansion](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_05_Parameter_Expansion.md) - Default, alternate, and error forms, length, and prefix and suffix removal, with the distinction between null and unset.
6. [Field Splitting and IFS](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_06_Field_Splitting_and_IFS.md) - IFS whitespace and non-whitespace characters, empty fields, and the rules that every quoting bug comes from.
7. [Pathname Expansion](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_07_Pathname_Expansion.md) - Pattern matching notation, bracket expressions, leading dots, and what happens when a pattern matches nothing.
8. [Command Substitution and Arithmetic Expansion](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_08_Command_Substitution_and_Arithmetic_Expansion.md) - Subshell execution, trailing newline removal, backquote quoting rules, and arithmetic expansion semantics.
9. [Redirection and Here-Documents](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_09_Redirection_and_Here_Documents.md) - The order in which redirections apply, descriptor duplication and closing, noclobber, and here-document quoting.
10. [Simple Commands, Command Search, and Execution](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_10_Simple_Commands_Command_Search_and_Execution.md) - Assignment prefixes, special built-ins versus regular built-ins versus utilities, the command search order, and the execution environment.
11. [Compound Commands and Functions](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_11_Compound_Commands_and_Functions.md) - Lists, pipelines, and-or lists, subshells versus groups, loops, case, and function definition and scope.
12. [Exit Status, errexit, and the Contexts That Suppress It](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_12_Exit_Status_errexit_and_Its_Exceptions.md) - The exact semantics of `set -e`, every context that ignores it, and why errexit is not exception handling.
13. [Traps and Signals in the Shell](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_13_Traps_and_Signals_in_the_Shell.md) - trap semantics, the EXIT condition, signal inheritance in subshells and asynchronous lists, and cleanup patterns.
14. [The Shell Execution Environment and Subshells](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_14_Execution_Environment_and_Subshells.md) - What a subshell inherits, what it can never change in its parent, and which constructs create one.
15. [What Changed in POSIX.1-2024](./Volume_02_The_POSIX_Shell_Command_Language/Chapter_15_What_Changed_in_POSIX_2024.md) - The shell and utility changes in Issue 8 and their impact on portable scripts.

### [Volume 03 - Bash in Depth](./Volume_03_Bash_in_Depth/)
1. [Bash Startup, Modes, and Invocation](./Volume_03_Bash_in_Depth/Chapter_01_Bash_Startup_Modes_and_Invocation.md) - Login, interactive, and non-interactive shells, POSIX mode and sh emulation, restricted mode, and `BASH_ENV`.
2. [Arrays and Associative Arrays](./Volume_03_Bash_in_Depth/Chapter_02_Arrays_and_Associative_Arrays.md) - Indexed and associative arrays, their expansion forms, sparse indices, and passing arrays safely.
3. [Advanced Parameter Expansion and Transformations](./Volume_03_Bash_in_Depth/Chapter_03_Advanced_Parameter_Expansion_and_Transformations.md) - Substrings, pattern substitution, case modification, indirection, and the `@` transformation operators.
4. [Namerefs, Scoping, and Dynamic Scope](./Volume_03_Bash_in_Depth/Chapter_04_Namerefs_Scoping_and_Dynamic_Scope.md) - local and declare, namerefs, and the pitfalls of dynamic scoping.
5. [The Double-Bracket Conditional and Regular Expressions](./Volume_03_Bash_in_Depth/Chapter_05_The_Double_Bracket_Conditional_and_Regex.md) - Pattern matching versus regex matching inside the double-bracket conditional, `BASH_REMATCH`, and its quoting rules.
6. [Extended Globbing and Shell Options](./Volume_03_Bash_in_Depth/Chapter_06_Extended_Globbing_and_Shell_Options.md) - extglob, globstar, nullglob, failglob, dotglob, nocasematch, and every shopt option that changes semantics.
7. [Process Substitution and Coprocesses](./Volume_03_Bash_in_Depth/Chapter_07_Process_Substitution_and_Coprocesses.md) - Input and output process substitution, its `/dev/fd` implementation, coproc, and their lifetime and error-handling gaps.
8. [Builtins for Data: read, mapfile, and printf](./Volume_03_Bash_in_Depth/Chapter_08_Data_Builtins_read_mapfile_and_printf.md) - read options and its byte-at-a-time behavior on pipes, mapfile, `printf -v`, and `printf %q`.
9. [Error Handling in Bash](./Volume_03_Bash_in_Depth/Chapter_09_Error_Handling_in_Bash.md) - pipefail, inherit_errexit, errtrace, functrace, the ERR trap, and `PIPESTATUS`.
10. [Job Control and Concurrency in Bash](./Volume_03_Bash_in_Depth/Chapter_10_Job_Control_and_Concurrency_in_Bash.md) - `wait -n` and `wait -p`, jobs, disown, and SIGCHLD handling.
11. [Debugging and Introspection](./Volume_03_Bash_in_Depth/Chapter_11_Debugging_and_Introspection_in_Bash.md) - xtrace and `PS4`, `BASH_XTRACEFD`, `BASH_SOURCE`, `FUNCNAME`, `LINENO`, caller, and bashdb.
12. [Bash Versions and Release History](./Volume_03_Bash_in_Depth/Chapter_12_Bash_Versions_and_Release_History.md) - What each bash release from 3.2 to the current one changed, and the macOS bash 3.2 baseline.
13. [Loadable Builtins and Extending Bash](./Volume_03_Bash_in_Depth/Chapter_13_Loadable_Builtins_and_Extending_Bash.md) - `enable -f`, the examples/loadables tree, and writing a builtin in C.

### [Volume 04 - Inside the Bash Source](./Volume_04_Inside_the_Bash_Source/)
1. [Building Bash from Source and Navigating the Tree](./Volume_04_Inside_the_Bash_Source/Chapter_01_Building_Bash_from_Source.md) - configure options, the layout of the source tree, and building a debug binary.
2. [The Parser: parse.y and the Lexer](./Volume_04_Inside_the_Bash_Source/Chapter_02_The_Bash_Parser_and_Lexer.md) - The yacc grammar, the hand-written lexer, here-document handling, and alias expansion.
3. [The Word Expansion Engine: subst.c](./Volume_04_Inside_the_Bash_Source/Chapter_03_The_Bash_Word_Expansion_Engine.md) - How expansions are implemented, word lists and quoting flags, and the internals of field splitting.
4. [Command Execution: execute_cmd.c](./Volume_04_Inside_the_Bash_Source/Chapter_04_Bash_Command_Execution.md) - When bash forks, builtin dispatch, pipelines and redirections, and the exec optimization for the last command.
5. [Jobs, Signals, and Traps: jobs.c, trap.c, and sig.c](./Volume_04_Inside_the_Bash_Source/Chapter_05_Bash_Jobs_Signals_and_Traps_Internals.md) - The job table, SIGCHLD reaping, deferred trap execution, and signal safety inside the shell.
6. [Variables, Functions, and Hashing: variables.c and hashlib.c](./Volume_04_Inside_the_Bash_Source/Chapter_06_Bash_Variables_Functions_and_Hashing.md) - Variable contexts and scopes, the command hash table, and how exported functions are encoded.
7. [Debugging Bash Itself](./Volume_04_Inside_the_Bash_Source/Chapter_07_Debugging_Bash_Itself.md) - Stepping through the shell in lldb or gdb and tracing one command line from read to exit status.

### [Volume 05 - Zsh, Ksh, Dash, BusyBox, and the Shell Landscape](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/)
1. [The Shell Landscape: What Runs Where](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_01_The_Shell_Landscape_What_Runs_Where.md) - Which shell is `/bin/sh` and the login shell on each major operating system and container base image, and the compatibility level of each.
2. [Zsh as a Language](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_02_Zsh_as_a_Language.md) - How zsh differs from POSIX sh, no field splitting of parameters by default, arrays indexed from 1, emulate, and setopt.
3. [Zsh Expansion Flags and Glob Qualifiers](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_03_Zsh_Expansion_Flags_and_Glob_Qualifiers.md) - Parameter expansion flags, glob qualifiers, and extended globbing.
4. [The Zsh Line Editor and Completion System](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_04_Zsh_Line_Editor_and_Completion_System.md) - ZLE widgets, the compsys completion system and writing completion functions, and zsh startup files.
5. [Dash and Minimal POSIX Shells](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_05_Dash_and_Minimal_POSIX_Shells.md) - The design goals of dash, why Debian and Ubuntu use it as `/bin/sh`, posh and yash as strict references, and the bashisms that break.
6. [KornShell: ksh93 and mksh](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_06_KornShell_ksh93_and_mksh.md) - ksh93 features, discipline functions and compound variables, and mksh.
7. [BusyBox ash in Containers and Embedded Systems](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_07_BusyBox_ash_in_Containers_and_Embedded_Systems.md) - The BusyBox multi-call binary, the limits of its ash, and Alpine-based images.
8. [Fish and Interactive-First Shells](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_08_Fish_and_Interactive_First_Shells.md) - The design of fish, its deliberate incompatibility with POSIX, and when it is the right choice.
9. [Structured-Data Shells: PowerShell, Nushell, Oils, and Elvish](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_09_Structured_Data_Shells.md) - Object and table pipelines, what they fix about byte streams, and what they cost.
10. [Choosing a Shell](./Volume_05_Zsh_Ksh_Dash_BusyBox_and_the_Shell_Landscape/Chapter_10_Choosing_a_Shell.md) - Decision rules for scripts versus interactive use, portability targets, and the macOS bash 3.2 and zsh defaults.

## Phase 3 - The Userland

The utilities a shell program is made of: text processing, the core utilities, and structured data.

### [Volume 06 - Regular Expressions and Text Processing](./Volume_06_Regular_Expressions_and_Text_Processing/)
1. [Regular Expression Flavors: BRE, ERE, and PCRE](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_01_Regex_Flavors_BRE_ERE_and_PCRE.md) - Syntax and semantic differences, POSIX leftmost-longest matching, and which tool uses which flavor.
2. [Regex Engines: Thompson NFA, DFA, and Backtracking](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_02_Regex_Engines_NFA_DFA_and_Backtracking.md) - Engine theory and complexity guarantees, RE2 and the Rust regex crate, and catastrophic backtracking (ReDoS).
3. [grep and ripgrep Internals](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_03_Grep_and_Ripgrep_Internals.md) - Literal search optimizations, memchr and SIMD prefilters, binary file detection, and directory traversal.
4. [sed as a Language](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_04_Sed_as_a_Language.md) - Pattern space and hold space, addresses, branching, and the differences in in-place editing.
5. [awk I: The Language](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_05_Awk_The_Language.md) - Records and fields, patterns and actions, associative arrays, and the execution model.
6. [awk II: Programs and Idioms](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_06_Awk_Programs_and_Idioms.md) - Reports, joins, state machines, and one-pass aggregation.
7. [awk III: Implementations](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_07_Awk_Implementations.md) - The one true awk, gawk, mawk, and BusyBox awk, with their extensions and performance differences.
8. [The Small Filters: tr, cut, paste, join, comm, column, fold, and fmt](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_08_The_Small_Filters.md) - What each filter does, its edge cases, and how to compose them.
9. [Locale, Collation, and Unicode](./Volume_06_Regular_Expressions_and_Text_Processing/Chapter_09_Locale_Collation_and_Unicode.md) - `LC_ALL`, `LC_COLLATE`, and `LC_CTYPE`, UTF-8 handling, and why `LC_ALL=C` changes both speed and results.

### [Volume 07 - Coreutils, Findutils, and the Userland Zoo](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/)
1. [Four Userlands: GNU, BSD and macOS, BusyBox, and uutils](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_01_Four_Userlands_GNU_BSD_BusyBox_and_uutils.md) - History and design differences of the four userlands, and their current distribution adoption.
2. [Portability Traps Between Userlands](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_02_Portability_Traps_Between_Userlands.md) - `sed -i`, date, stat, readlink, xargs, `grep -P`, mktemp, and other flags that diverge.
3. [find in Depth](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_03_Find_in_Depth.md) - Expressions and operators, `-print0`, `-exec` with plus and semicolon, `-prune`, time and size tests, and traversal order.
4. [xargs and Argument Batching](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_04_Xargs_and_Argument_Batching.md) - `-0`, `-n`, `-P`, and `-I`, batching under `ARG_MAX`, and safe filename handling.
5. [sort, uniq, and External Merge Sort](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_05_Sort_Uniq_and_External_Merge_Sort.md) - Keys and field rules, stability, `-S` and temporary files, `--parallel`, and numeric, version, and human sorting.
6. [Files and Bytes: cp, mv, dd, tee, and install](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_06_Files_and_Bytes_cp_mv_dd_tee_and_install.md) - Reflinks and copy_file_range, sparse files, rename semantics across filesystems, and atomic installs.
7. [stdio Buffering and stdbuf](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_07_Stdio_Buffering_and_Stdbuf.md) - Full, line, and unbuffered modes, why pipelines appear to stall, and how stdbuf works.
8. [Process Utilities: timeout, flock, nohup, setsid, nice, and env](./Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/Chapter_08_Process_Wrapper_Utilities.md) - The mechanisms and portability of the utilities that wrap other commands.

### [Volume 08 - Structured Data at the Command Line](./Volume_08_Structured_Data_at_the_Command_Line/)
1. [jq I: The Language](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_01_Jq_The_Language.md) - Filters, paths, generators, and jq programs as functions from one JSON value to a stream of values.
2. [jq II: Advanced Programs and Streaming](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_02_Jq_Advanced_Programs_and_Streaming.md) - reduce, foreach, path expressions, `--stream` for huge inputs, and alternative jq implementations.
3. [YAML, TOML, and XML at the Command Line](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_03_YAML_TOML_and_XML_at_the_Command_Line.md) - The yq variants, dasel, and xmlstarlet, and the hazards of parsing YAML.
4. [CSV and Tabular Data](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_04_CSV_and_Tabular_Data.md) - Miller, xsv and qsv, csvkit, and CSV quoting edge cases.
5. [Databases as Shell Citizens: sqlite3 and DuckDB](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_05_Databases_as_Shell_Citizens_sqlite3_and_DuckDB.md) - Querying files with SQL from the shell, and when to switch tools.
6. [HTTP and APIs from the Shell](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_06_HTTP_and_APIs_from_the_Shell.md) - curl in depth: options, retries, timeouts, authentication, and response handling.
7. [When to Stop Using Text Tools](./Volume_08_Structured_Data_at_the_Command_Line/Chapter_07_When_to_Stop_Using_Text_Tools.md) - The signs that a pipeline needs a structured language, and how to migrate it.

## Phase 4 - Engineering Discipline

Writing shell that is correct under failure, tested, concurrent, fast enough, and secure.

### [Volume 09 - Robust and Defensive Scripting](./Volume_09_Robust_and_Defensive_Scripting/)
1. [Unofficial Strict Mode and Its Failure Modes](./Volume_09_Robust_and_Defensive_Scripting/Chapter_01_Unofficial_Strict_Mode_and_Its_Failure_Modes.md) - `set -euo pipefail`: what each option catches, what it misses, and better alternatives.
2. [Error Handling Patterns That Work](./Volume_09_Robust_and_Defensive_Scripting/Chapter_02_Error_Handling_Patterns_That_Work.md) - Explicit checks, die functions, ERR traps, and propagating errors out of functions and subshells.
3. [Cleanup and Traps](./Volume_09_Robust_and_Defensive_Scripting/Chapter_03_Cleanup_and_Traps.md) - Temporary resources, EXIT and signal traps, re-raising signals, and nested cleanup.
4. [Idempotency and Atomic Writes](./Volume_09_Robust_and_Defensive_Scripting/Chapter_04_Idempotency_and_Atomic_Writes.md) - Safe reruns, mktemp in the target filesystem, rename for atomic replacement, and fsync.
5. [Locking and Mutual Exclusion](./Volume_09_Robust_and_Defensive_Scripting/Chapter_05_Locking_and_Mutual_Exclusion.md) - flock, mkdir locks, lock files and PID reuse, and recovering stale locks.
6. [Retries, Backoff, and Timeouts](./Volume_09_Robust_and_Defensive_Scripting/Chapter_06_Retries_Backoff_and_Timeouts.md) - Exponential backoff with jitter, bounded retries, and timeout wrappers.
7. [Argument Parsing and CLI Design](./Volume_09_Robust_and_Defensive_Scripting/Chapter_07_Argument_Parsing_and_CLI_Design.md) - getopts, long options, the double-dash convention, help text, and exit code contracts.
8. [Configuration, Environment, and Logging](./Volume_09_Robust_and_Defensive_Scripting/Chapter_08_Configuration_Environment_and_Logging.md) - Precedence rules, input validation, logging to stderr, and machine-readable output.
9. [Dry Run and Apply Design](./Volume_09_Robust_and_Defensive_Scripting/Chapter_09_Dry_Run_and_Apply_Design.md) - Separating planning from execution, reviewable plans, and reversible operations.

### [Volume 10 - Testing, Static Analysis, and Tooling](./Volume_10_Testing_Static_Analysis_and_Tooling/)
1. [ShellCheck in Depth](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_01_ShellCheck_in_Depth.md) - How ShellCheck parses, its warning families and the bug each one prevents, directives, and configuration.
2. [shfmt and Style](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_02_Shfmt_and_Style.md) - Formatting, dialect detection, EditorConfig integration, and simplification.
3. [Unit Testing with bats-core](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_03_Unit_Testing_with_Bats_Core.md) - Test structure, run, helpers, setup and teardown, and the bats-support and bats-assert libraries.
4. [ShellSpec and shUnit2](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_04_ShellSpec_and_ShUnit2.md) - Behavior-driven shell testing, cross-shell support, and choosing a framework.
5. [Mocking, Stubbing, and Golden Tests](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_05_Mocking_Stubbing_and_Golden_Tests.md) - PATH-based command mocks, fake clocks and filesystems, and golden-output tests.
6. [Coverage for Shell](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_06_Coverage_for_Shell.md) - kcov and bashcov mechanics, and what line coverage means for shell.
7. [CI Matrices Across Shells and Operating Systems](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_07_CI_Matrices_Across_Shells_and_Operating_Systems.md) - A container per shell, macOS runners, and pinning tool versions.
8. [Reviewing Shell Code at Staff Level](./Volume_10_Testing_Static_Analysis_and_Tooling/Chapter_08_Reviewing_Shell_Code_at_Staff_Level.md) - A review checklist, ranking risk, and when to demand a rewrite in another language.

### [Volume 11 - Concurrency and Parallelism in Shell](./Volume_11_Concurrency_and_Parallelism_in_Shell/)
1. [Background Jobs and wait](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_01_Background_Jobs_and_Wait.md) - Asynchronous lists, the last background PID, wait, collecting exit statuses, and avoiding zombies.
2. [Bounded Parallelism](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_02_Bounded_Parallelism.md) - Job pools in pure shell, `xargs -P`, and GNU parallel.
3. [GNU parallel in Depth](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_03_GNU_Parallel_in_Depth.md) - Input sources, replacement strings, `--joblog`, resumable runs, and remote execution.
4. [Named Pipes, Coprocesses, and Descriptor Semaphores](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_04_Named_Pipes_Coprocesses_and_Descriptor_Semaphores.md) - FIFOs for IPC, coproc, and tokens in a pipe used as a counting semaphore.
5. [Races, Interleaving, and Atomicity](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_05_Races_Interleaving_and_Atomicity.md) - Output interleaving, `PIPE_BUF`, append semantics, and time-of-check to time-of-use races.
6. [Signals in Concurrent Scripts](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_06_Signals_in_Concurrent_Scripts.md) - Propagating signals to children, process groups, and graceful shutdown.
7. [Process Supervision: systemd and launchd](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_07_Process_Supervision_systemd_and_launchd.md) - Units and property lists, restart policies, and logging.
8. [Scheduling: cron, systemd Timers, and launchd](./Volume_11_Concurrency_and_Parallelism_in_Shell/Chapter_08_Scheduling_cron_systemd_Timers_and_launchd.md) - Environment differences, preventing overlapping runs, and handling missed runs.

### [Volume 12 - Performance of Shell Programs](./Volume_12_Performance_of_Shell_Programs/)
1. [A Cost Model for Shell](./Volume_12_Performance_of_Shell_Programs/Chapter_01_A_Cost_Model_for_Shell.md) - The measured cost of fork, exec, subshells, command substitution, and builtins versus external commands.
2. [Measuring Shell Performance](./Volume_12_Performance_of_Shell_Programs/Chapter_02_Measuring_Shell_Performance.md) - hyperfine, time, `strace -c`, dtruss, and perf, with noise control.
3. [Loops versus Single-Pass Tools](./Volume_12_Performance_of_Shell_Programs/Chapter_03_Loops_vs_Single_Pass_Tools.md) - Why a while-read loop is slow, and moving the work into awk, sort, and join.
4. [Big-Data Pipelines](./Volume_12_Performance_of_Shell_Programs/Chapter_04_Big_Data_Pipelines.md) - `sort -S`, `LC_ALL=C`, split and parallel, and telling I/O-bound stages from CPU-bound ones.
5. [Shell versus Python versus Compiled Tools, and When to Stop Writing Shell](./Volume_12_Performance_of_Shell_Programs/Chapter_05_Shell_vs_Python_vs_Compiled_Tools.md) - A benchmarked comparison on the same workloads, the crossover points, and a decision rule based on size, data structures, error handling, and team.

### [Volume 13 - Shell Security](./Volume_13_Shell_Security/)
1. [A Threat Model for Shell Scripts](./Volume_13_Shell_Security/Chapter_01_Threat_Model_for_Shell_Scripts.md) - Sources of untrusted input, trust boundaries, and privilege.
2. [Command and Code Injection](./Volume_13_Shell_Security/Chapter_02_Command_and_Code_Injection.md) - eval, `sh -c`, ssh remote commands, and safe quoting with `printf %q`.
3. [Argument Injection and Hostile Filenames](./Volume_13_Shell_Security/Chapter_03_Argument_Injection_and_Hostile_Filenames.md) - Leading dashes and the double-dash convention, newlines and glob characters in file names, and `find -print0`.
4. [Environment Attacks: PATH, IFS, and Startup Files](./Volume_13_Shell_Security/Chapter_04_Environment_Attacks_PATH_IFS_and_Startup_Files.md) - Hijacking command search, manipulating IFS, and the `BASH_ENV` and `ENV` variables.
5. [Shellshock](./Volume_13_Shell_Security/Chapter_05_Shellshock.md) - CVE-2014-6271 and its follow-up CVEs, the function import mechanism that caused them, and the fix.
6. [Temporary Files and TOCTOU](./Volume_13_Shell_Security/Chapter_06_Temporary_Files_and_TOCTOU.md) - Predictable names, symlink attacks, mktemp, and private directories.
7. [Privilege: setuid, sudo, and Capabilities](./Volume_13_Shell_Security/Chapter_07_Privilege_setuid_sudo_and_Capabilities.md) - Why setuid scripts are unsafe, sudoers rules and secure_path, and dropping privileges.
8. [Secrets in Shell](./Volume_13_Shell_Security/Chapter_08_Secrets_in_Shell.md) - Exposure through argv, the environment, `/proc`, and history, and safe handling patterns.
9. [Supply Chain: Piped Installers](./Volume_13_Shell_Security/Chapter_09_Supply_Chain_Piped_Installers.md) - The risks of piping a downloaded script into a shell, verification, and pinning.
10. [Case Study: The xz-utils Backdoor](./Volume_13_Shell_Security/Chapter_10_Case_Study_The_xz_Utils_Backdoor.md) - CVE-2024-3094, its build-time shell stages, and the lessons for reviewing build scripts.
11. [Restricted Shells and Sandboxing](./Volume_13_Shell_Security/Chapter_11_Restricted_Shells_and_Sandboxing.md) - rbash, chroot, namespaces, and their limits.

## Phase 5 - Shell in Production

Where shell runs in real systems: builds, containers, CI, cloud, incidents, the desktop, and AI agents.

### [Volume 14 - Build Systems and Packaging](./Volume_14_Build_Systems_and_Packaging/)
1. [Make as a Shell Driver](./Volume_14_Build_Systems_and_Packaging/Chapter_01_Make_as_a_Shell_Driver.md) - Recipe execution, one shell per line, `.ONESHELL`, `SHELL` and `.SHELLFLAGS`, and error propagation.
2. [Autoconf and Portable configure Scripts](./Volume_14_Build_Systems_and_Packaging/Chapter_02_Autoconf_and_Portable_Configure_Scripts.md) - M4 generation, the constraints of portable sh, and reading a configure script.
3. [Nix Builders and stdenv](./Volume_14_Build_Systems_and_Packaging/Chapter_03_Nix_Builders_and_Stdenv.md) - Derivation builders, setup.sh phases and hooks, and writeShellApplication.
4. [Homebrew, apt, and Package Hooks](./Volume_14_Build_Systems_and_Packaging/Chapter_04_Package_Manager_Hooks.md) - Formula install blocks, Debian maintainer scripts, and their failure modes.
5. [Designing Installer Scripts](./Volume_14_Build_Systems_and_Packaging/Chapter_05_Designing_Installer_Scripts.md) - Platform detection, idempotency, uninstall, and verification.

### [Volume 15 - Containers, CI, and Cloud](./Volume_15_Containers_CI_and_Cloud/)
1. [Dockerfile Shell Form versus Exec Form](./Volume_15_Containers_CI_and_Cloud/Chapter_01_Dockerfile_Shell_Form_vs_Exec_Form.md) - The RUN, CMD, and ENTRYPOINT forms, the SHELL instruction, and layer behavior.
2. [PID 1, Signals, and Zombie Reaping in Containers](./Volume_15_Containers_CI_and_Cloud/Chapter_02_PID_1_Signals_and_Zombie_Reaping_in_Containers.md) - Why PID 1 is special, tini and dumb-init, and exec in entrypoint scripts.
3. [Entrypoint Script Patterns](./Volume_15_Containers_CI_and_Cloud/Chapter_03_Entrypoint_Script_Patterns.md) - Configuration from the environment, waiting for dependencies, and graceful shutdown.
4. [Kubernetes and the Shell](./Volume_15_Containers_CI_and_Cloud/Chapter_04_Kubernetes_and_the_Shell.md) - Probes, init containers, kubectl output formats with jq, and kubectl debug.
5. [CI Shells: GitHub Actions and Others](./Volume_15_Containers_CI_and_Cloud/Chapter_05_CI_Shells_GitHub_Actions_and_Others.md) - Default shells and flags, run step semantics, and quoting untrusted workflow context.
6. [Cloud CLIs: AWS CLI and gcloud Scripting](./Volume_15_Containers_CI_and_Cloud/Chapter_06_Cloud_CLIs_AWS_CLI_and_Gcloud_Scripting.md) - Server-side filtering with `--query` and `--format`, pagination, retries, and credentials.
7. [SSH Automation and Remote Quoting](./Volume_15_Containers_CI_and_Cloud/Chapter_07_SSH_Automation_and_Remote_Quoting.md) - How ssh joins its arguments into one remote command, connection multiplexing, and fleet-wide fan-out.
8. [cloud-init and Bootstrap Scripts](./Volume_15_Containers_CI_and_Cloud/Chapter_08_Cloud_Init_and_Bootstrap_Scripts.md) - Boot stages, user-data scripts, and debugging first boot.

### [Volume 16 - Observability and Incident Response from the Shell](./Volume_16_Observability_and_Incident_Response/)
1. [The 60-Second Linux Performance Analysis](./Volume_16_Observability_and_Incident_Response/Chapter_01_The_60_Second_Linux_Performance_Analysis.md) - Brendan Gregg's ten-command checklist, what each command shows, and how to read it.
2. [Processes and Resources: ps, top, and /proc](./Volume_16_Observability_and_Incident_Response/Chapter_02_Processes_and_Resources_ps_top_and_proc.md) - Process states, CPU and memory accounting, and reading `/proc` directly.
3. [Files and Sockets: lsof, ss, and netstat](./Volume_16_Observability_and_Incident_Response/Chapter_03_Files_and_Sockets_lsof_ss_and_netstat.md) - Open files, listening sockets, and TCP connection states.
4. [Tracing: strace, ltrace, perf, and bpftrace](./Volume_16_Observability_and_Incident_Response/Chapter_04_Tracing_strace_ltrace_perf_and_bpftrace.md) - System call tracing, sampling profilers, and eBPF one-liners.
5. [Logs at Scale](./Volume_16_Observability_and_Incident_Response/Chapter_05_Logs_at_Scale.md) - journalctl, log triage pipelines, and extracting time windows.
6. [macOS Diagnostics](./Volume_16_Observability_and_Incident_Response/Chapter_06_MacOS_Diagnostics.md) - dtruss, fs_usage, sample, spindump, and `log show`.
7. [Runbooks as Tested Scripts](./Volume_16_Observability_and_Incident_Response/Chapter_07_Runbooks_as_Tested_Scripts.md) - Encoding incident procedures as safe, tested, dry-run-first scripts.
8. [CPU Affinity and Real-Time Controls](./Volume_16_Observability_and_Incident_Response/Chapter_08_CPU_Affinity_and_Real_Time_Controls.md) - taskset, chrt, isolcpus, and IRQ affinity for latency-sensitive systems.

### [Volume 17 - The Interactive Environment and Dotfiles](./Volume_17_The_Interactive_Environment_and_Dotfiles/)
1. [Startup Files for bash and zsh](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_01_Startup_Files_for_Bash_and_Zsh.md) - Which files run for login, interactive, and non-interactive shells, as a decision table.
2. [PATH and Environment Management](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_02_PATH_and_Environment_Management.md) - Ordering and deduplication, path_helper on macOS, and per-project environments.
3. [Prompts](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_03_Prompts.md) - PS1 and PROMPT escapes, version control status, prompt performance, and asynchronous prompts.
4. [Writing Completions](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_04_Writing_Completions.md) - bash complete and compgen, zsh compsys, and generating completions from a command line tool.
5. [History Design](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_05_History_Design.md) - History files, sharing history across sessions, deduplication, and search.
6. [Productivity Tools: fzf, direnv, zoxide, and atuin](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_06_Productivity_Tools_fzf_direnv_zoxide_and_atuin.md) - How each tool integrates with the shell, and what each one costs at startup.
7. [Dotfiles Management](./Volume_17_The_Interactive_Environment_and_Dotfiles/Chapter_07_Dotfiles_Management.md) - Symlink managers, chezmoi, and Nix home-manager.

### [Volume 18 - Shell and AI Agents](./Volume_18_Shell_and_AI_Agents/)
1. [How Coding Agents Execute Shell](./Volume_18_Shell_and_AI_Agents/Chapter_01_How_Coding_Agents_Execute_Shell.md) - Persistent versus fresh shells, working directory and environment state, timeouts, and output truncation.
2. [Quoting Across Tool Boundaries](./Volume_18_Shell_and_AI_Agents/Chapter_02_Quoting_Across_Tool_Boundaries.md) - JSON to shell, nested quoting, here-documents, and avoiding injection through generated commands.
3. [Command Safety Guards and Hooks](./Volume_18_Shell_and_AI_Agents/Chapter_03_Command_Safety_Guards_and_Hooks.md) - Pre-execution policy checks, deny and ask lists, and how they are bypassed.
4. [Sandboxing Agent Shells](./Volume_18_Shell_and_AI_Agents/Chapter_04_Sandboxing_Agent_Shells.md) - macOS Seatbelt, Linux Landlock, containers, and network restrictions.
5. [Designing Commands for Agents](./Volume_18_Shell_and_AI_Agents/Chapter_05_Designing_Commands_for_Agents.md) - Idempotency, dry runs, structured output, definitive empty states, and the AXI principles.

## Phase 6 - Mastery

The history and primary sources, the interview arena, and capstones that prove mastery.

### [Volume 19 - History, Papers, and Primary Sources](./Volume_19_History_Papers_and_Primary_Sources/)
1. [The Thompson and PWB Shells](./Volume_19_History_Papers_and_Primary_Sources/Chapter_01_The_Thompson_and_PWB_Shells.md) - The first Unix shell, the PWB (Mashey) shell, and the arrival of pipes.
2. [The Bourne Shell](./Volume_19_History_Papers_and_Primary_Sources/Chapter_02_The_Bourne_Shell.md) - Steve Bourne's design, its ALGOL 68 influenced source code, and its legacy.
3. [csh and the C Shell Critique](./Volume_19_History_Papers_and_Primary_Sources/Chapter_03_Csh_and_the_C_Shell_Critique.md) - Bill Joy's C shell, its interactive innovations, and Tom Christiansen's "Csh Programming Considered Harmful".
4. [The KornShell and POSIX Standardization](./Volume_19_History_Papers_and_Primary_Sources/Chapter_04_The_KornShell_and_POSIX_Standardization.md) - David Korn's shell, POSIX.2, and the choices the standard made.
5. [rc, es, and Plan 9](./Volume_19_History_Papers_and_Primary_Sources/Chapter_05_Rc_Es_and_Plan_9.md) - Tom Duff's rc shell, what its design fixed, and its descendants.
6. [The Unix Philosophy Papers](./Volume_19_History_Papers_and_Primary_Sources/Chapter_06_The_Unix_Philosophy_Papers.md) - "The UNIX Time-Sharing System" (1974), McIlroy on pipes, and "Program Design in the UNIX Environment".
7. [Modern Shell Language Design](./Volume_19_History_Papers_and_Primary_Sources/Chapter_07_Modern_Shell_Language_Design.md) - The Oils project, structured shells, and lessons for language designers.

### [Volume 20 - Interview Arena](./Volume_20_Interview_Arena/)
1. [What Happens When You Type a Command and Press Enter](./Volume_20_Interview_Arena/Chapter_01_What_Happens_When_You_Type_a_Command.md) - From the keyboard through the TTY, the parser, the expansions, fork, exec, and wait, to the exit status.
2. [Predict-the-Output Drills](./Volume_20_Interview_Arena/Chapter_02_Predict_the_Output_Drills.md) - Quoting, expansion, subshell, and errexit puzzles with explanations.
3. [Debug-This-Script Drills](./Volume_20_Interview_Arena/Chapter_03_Debug_This_Script_Drills.md) - Realistic broken scripts with root causes and fixes.
4. [One-Liner Fluency](./Volume_20_Interview_Arena/Chapter_04_One_Liner_Fluency.md) - Common interview text-processing tasks and their model solutions.
5. [LeetCode Shell Problems](./Volume_20_Interview_Arena/Chapter_05_LeetCode_Shell_Problems.md) - Model solutions for the four LeetCode shell problems, with complexity notes and portable alternatives.
6. [Design Drills](./Volume_20_Interview_Arena/Chapter_06_Design_Drills.md) - A safe deploy script, log rotation, backups with integrity checks, and fleet-wide command fan-out.
7. [Unix Systems Interview Questions](./Volume_20_Interview_Arena/Chapter_07_Unix_Systems_Interview_Questions.md) - Processes, signals, file descriptors, and terminals at senior to principal depth.
8. [The Staff Bar for Shell](./Volume_20_Interview_Arena/Chapter_08_The_Staff_Bar_for_Shell.md) - What separates L5, L6, and L7+ answers on shell and Unix questions.

### [Volume 21 - Capstones](./Volume_21_Capstones/)
1. [Capstone 1: A POSIX-Subset Shell in C](./Volume_21_Capstones/Chapter_01_Capstone_A_POSIX_Subset_Shell_in_C.md) - Pipelines, redirection, quoting, expansions, builtins, and job control, tested against dash as the oracle.
2. [Capstone 2: A Production CLI in Bash](./Volume_21_Capstones/Chapter_02_Capstone_A_Production_CLI_in_Bash.md) - Argument parsing, completions, a man page, a dry run, structured errors, a test suite, and a CI matrix.
3. [Capstone 3: Rescuing a Legacy Script](./Volume_21_Capstones/Chapter_03_Capstone_Rescuing_a_Legacy_Script.md) - Characterization tests first, then a safe refactor.
4. [Capstone 4: A Multi-Gigabyte Data Pipeline](./Volume_21_Capstones/Chapter_04_Capstone_A_Multi_Gigabyte_Data_Pipeline.md) - A benchmark of shell against Python and a compiled tool, and a written decision memo.
5. [Capstone 5: An Upstream Contribution](./Volume_21_Capstones/Chapter_05_Capstone_An_Upstream_Contribution.md) - A ShellCheck rule, a bats-core fix, or a well-formed bash bug report.

## Appendices

- [Appendix A: Glossary of Shell and Unix Terms](./Appendices/Appendix_A_Glossary_of_Shell_and_Unix_Terms.md) - Precise definitions of the shell, terminal, process, and userland terms used across the curriculum.
- [Appendix B: Shell Feature Portability Map](./Appendices/Appendix_B_Shell_Feature_Portability_Map.md) - Which features POSIX sh, bash, zsh, dash, ksh93, mksh, yash, posh, and BusyBox ash support, verified by the lab matrix.
- [Appendix C: Userland Utility Differences](./Appendices/Appendix_C_Userland_Utility_Differences.md) - Flag-by-flag differences between the GNU, BSD and macOS, BusyBox, and uutils utilities.
- [Appendix D: Signals, Exit Codes, and Kernel Limits](./Appendices/Appendix_D_Signals_Exit_Codes_and_Kernel_Limits.md) - Signal numbers and default actions, exit code conventions, and the kernel limits scripts hit, on Linux and macOS.
- [Appendix E: ShellCheck Warning Index](./Appendices/Appendix_E_ShellCheck_Warning_Index.md) - The ShellCheck warnings that matter most, with the bug each one prevents and the chapter that explains it.
- [Appendix F: Expansion and Redirection Cheat Sheet](./Appendices/Appendix_F_Expansion_and_Redirection_Cheat_Sheet.md) - Every parameter expansion form and redirection operator on one page.
- [Appendix G: sed, awk, and jq Quick Reference](./Appendices/Appendix_G_Sed_Awk_and_Jq_Quick_Reference.md) - Commands, functions, and idioms for the three text languages.
- [Appendix H: Incident Response One-Liner Cookbook](./Appendices/Appendix_H_Incident_Response_One_Liner_Cookbook.md) - Tested one-liners for production triage, grouped by symptom.
- [Appendix I: Primary Sources and Reading List](./Appendices/Appendix_I_Primary_Sources_and_Reading_List.md) - Specifications, source trees, papers, and books, with why each one matters.
- [Appendix J: 16-Week Mastery Study Plan](./Appendices/Appendix_J_16_Week_Mastery_Study_Plan.md) - A week-by-week plan with readings, labs, and interview checkpoints.
