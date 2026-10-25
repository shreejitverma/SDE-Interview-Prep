---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 03 - Bash in Depth

Bash is the shell most production scripts are actually written for.
This volume covers every bash extension a senior engineer meets in real code, with its exact semantics, its version history, and the bugs it invites.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)

## Chapters

1. [Bash Startup, Modes, and Invocation](Chapter_01_Bash_Startup_Modes_and_Invocation.md) - Login, interactive, and non-interactive shells, POSIX mode and sh emulation, restricted mode, and `BASH_ENV`.
2. [Arrays and Associative Arrays](Chapter_02_Arrays_and_Associative_Arrays.md) - Indexed and associative arrays, their expansion forms, sparse indices, and passing arrays safely.
3. [Advanced Parameter Expansion and Transformations](Chapter_03_Advanced_Parameter_Expansion_and_Transformations.md) - Substrings, pattern substitution, case modification, indirection, and the `@` transformation operators.
4. [Namerefs, Scoping, and Dynamic Scope](Chapter_04_Namerefs_Scoping_and_Dynamic_Scope.md) - local and declare, namerefs, and the pitfalls of dynamic scoping.
5. [The Double-Bracket Conditional and Regular Expressions](Chapter_05_The_Double_Bracket_Conditional_and_Regex.md) - Pattern matching versus regex matching inside the double-bracket conditional, `BASH_REMATCH`, and its quoting rules.
6. [Extended Globbing and Shell Options](Chapter_06_Extended_Globbing_and_Shell_Options.md) - extglob, globstar, nullglob, failglob, dotglob, nocasematch, and every shopt option that changes semantics.
7. [Process Substitution and Coprocesses](Chapter_07_Process_Substitution_and_Coprocesses.md) - Input and output process substitution, its `/dev/fd` implementation, coproc, and their lifetime and error-handling gaps.
8. [Builtins for Data: read, mapfile, and printf](Chapter_08_Data_Builtins_read_mapfile_and_printf.md) - read options and its byte-at-a-time behavior on pipes, mapfile, `printf -v`, and `printf %q`.
9. [Error Handling in Bash](Chapter_09_Error_Handling_in_Bash.md) - pipefail, inherit_errexit, errtrace, functrace, the ERR trap, and `PIPESTATUS`.
10. [Job Control and Concurrency in Bash](Chapter_10_Job_Control_and_Concurrency_in_Bash.md) - `wait -n` and `wait -p`, jobs, disown, and SIGCHLD handling.
11. [Debugging and Introspection](Chapter_11_Debugging_and_Introspection_in_Bash.md) - xtrace and `PS4`, `BASH_XTRACEFD`, `BASH_SOURCE`, `FUNCNAME`, `LINENO`, caller, and bashdb.
12. [Bash Versions and Release History](Chapter_12_Bash_Versions_and_Release_History.md) - What each bash release from 3.2 to the current one changed, and the macOS bash 3.2 baseline.
13. [Loadable Builtins and Extending Bash](Chapter_13_Loadable_Builtins_and_Extending_Bash.md) - `enable -f`, the examples/loadables tree, and writing a builtin in C.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v03-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
