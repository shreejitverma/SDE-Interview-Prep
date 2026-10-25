---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 02 - The POSIX Shell Command Language

The POSIX standard (IEEE Std 1003.1-2024, XCU chapter 2) is the only precise definition of what a shell must do.
This volume reads it as a specification: tokenization, quoting, the ordered expansion pipeline, redirection, command search, and the exact semantics of errexit and traps.
Every chapter checks the specification against real shells with the lab matrix.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)

## Chapters

1. [Reading the POSIX Standard](Chapter_01_Reading_the_POSIX_Standard.md) - How POSIX.1-2024 is organized into XBD, XSH, XCU, and XRAT, what conformance means, and how to cite a section precisely.
2. [Token Recognition and the Shell Grammar](Chapter_02_Token_Recognition_and_the_Shell_Grammar.md) - Tokenization rules, operators, reserved words, alias substitution, and the formal grammar.
3. [Quoting](Chapter_03_Quoting_Rules.md) - Backslash, single quotes, double quotes, and dollar-single-quotes, and exactly what each one preserves.
4. [The Expansion Pipeline](Chapter_04_The_Expansion_Pipeline.md) - The exact order of tilde, parameter, command substitution, and arithmetic expansion, followed by field splitting, pathname expansion, and quote removal.
5. [Parameter Expansion](Chapter_05_Parameter_Expansion.md) - Default, alternate, and error forms, length, and prefix and suffix removal, with the distinction between null and unset.
6. [Field Splitting and IFS](Chapter_06_Field_Splitting_and_IFS.md) - IFS whitespace and non-whitespace characters, empty fields, and the rules that every quoting bug comes from.
7. [Pathname Expansion](Chapter_07_Pathname_Expansion.md) - Pattern matching notation, bracket expressions, leading dots, and what happens when a pattern matches nothing.
8. [Command Substitution and Arithmetic Expansion](Chapter_08_Command_Substitution_and_Arithmetic_Expansion.md) - Subshell execution, trailing newline removal, backquote quoting rules, and arithmetic expansion semantics.
9. [Redirection and Here-Documents](Chapter_09_Redirection_and_Here_Documents.md) - The order in which redirections apply, descriptor duplication and closing, noclobber, and here-document quoting.
10. [Simple Commands, Command Search, and Execution](Chapter_10_Simple_Commands_Command_Search_and_Execution.md) - Assignment prefixes, special built-ins versus regular built-ins versus utilities, the command search order, and the execution environment.
11. [Compound Commands and Functions](Chapter_11_Compound_Commands_and_Functions.md) - Lists, pipelines, and-or lists, subshells versus groups, loops, case, and function definition and scope.
12. [Exit Status, errexit, and the Contexts That Suppress It](Chapter_12_Exit_Status_errexit_and_Its_Exceptions.md) - The exact semantics of `set -e`, every context that ignores it, and why errexit is not exception handling.
13. [Traps and Signals in the Shell](Chapter_13_Traps_and_Signals_in_the_Shell.md) - trap semantics, the EXIT condition, signal inheritance in subshells and asynchronous lists, and cleanup patterns.
14. [The Shell Execution Environment and Subshells](Chapter_14_Execution_Environment_and_Subshells.md) - What a subshell inherits, what it can never change in its parent, and which constructs create one.
15. [What Changed in POSIX.1-2024](Chapter_15_What_Changed_in_POSIX_2024.md) - The shell and utility changes in Issue 8 and their impact on portable scripts.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v02-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
