---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Bash Internals

The fastest way to settle an argument about shell behavior is to read the code that implements it.
These notes are a guided reading of the GNU bash source, ending with how to write a shell yourself.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Building Bash from Source](Building-Bash-from-Source.md)
2. [The Bash Parser](The-Bash-Parser.md)
3. [The Bash Lexer and Here Documents](The-Bash-Lexer-and-Here-Documents.md)
4. [Word Expansion in subst.c](Word-Expansion-in-subst-c.md)
5. [Command Execution in execute_cmd.c](Command-Execution-in-execute_cmd-c.md)
6. [The Fork Decision and Exec Optimization](The-Fork-Decision-and-Exec-Optimization.md)
7. [Job Table and SIGCHLD](Job-Table-and-SIGCHLD.md)
8. [Deferred Traps and Signal Safety](Deferred-Traps-and-Signal-Safety.md)
9. [Variable Contexts and Exported Functions](Variable-Contexts-and-Exported-Functions.md)
10. [Command Hashing](Command-Hashing.md)
11. [Debugging Bash with lldb](Debugging-Bash-with-lldb.md)
12. [How to Write a Shell](How-to-Write-a-Shell.md)
