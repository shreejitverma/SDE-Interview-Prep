---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 04 - Inside the Bash Source

The fastest way to settle an argument about shell behavior is to read the code that implements it.
This volume is a guided reading of the GNU bash source tree, from the parser to the job table, ending with stepping through bash in a debugger.

## Prerequisites

- [Volume 03 - Bash in Depth](../Volume_03_Bash_in_Depth/README.md)

## Chapters

1. [Building Bash from Source and Navigating the Tree](Chapter_01_Building_Bash_from_Source.md) - configure options, the layout of the source tree, and building a debug binary.
2. [The Parser: parse.y and the Lexer](Chapter_02_The_Bash_Parser_and_Lexer.md) - The yacc grammar, the hand-written lexer, here-document handling, and alias expansion.
3. [The Word Expansion Engine: subst.c](Chapter_03_The_Bash_Word_Expansion_Engine.md) - How expansions are implemented, word lists and quoting flags, and the internals of field splitting.
4. [Command Execution: execute_cmd.c](Chapter_04_Bash_Command_Execution.md) - When bash forks, builtin dispatch, pipelines and redirections, and the exec optimization for the last command.
5. [Jobs, Signals, and Traps: jobs.c, trap.c, and sig.c](Chapter_05_Bash_Jobs_Signals_and_Traps_Internals.md) - The job table, SIGCHLD reaping, deferred trap execution, and signal safety inside the shell.
6. [Variables, Functions, and Hashing: variables.c and hashlib.c](Chapter_06_Bash_Variables_Functions_and_Hashing.md) - Variable contexts and scopes, the command hash table, and how exported functions are encoded.
7. [Debugging Bash Itself](Chapter_07_Debugging_Bash_Itself.md) - Stepping through the shell in lldb or gdb and tracing one command line from read to exit status.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v04-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
