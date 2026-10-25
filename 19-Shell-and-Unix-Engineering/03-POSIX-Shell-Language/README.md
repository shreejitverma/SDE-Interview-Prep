---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# The POSIX Shell Language

POSIX.1-2024 (XCU chapter 2 and the built-in utilities) is the only precise definition of what a shell must do.
These notes read it as a specification and check every rule against real shells in the lab matrix.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Reading the POSIX Standard](Reading-the-POSIX-Standard.md)
2. [Token Recognition](Token-Recognition.md)
3. [Shell Grammar](Shell-Grammar.md)
4. [Aliases](Aliases.md)
5. [Quoting: Backslash, Single, and Double Quotes](Quoting-Backslash-Single-and-Double.md)
6. [Dollar-Single-Quotes](Dollar-Single-Quotes.md)
7. [Expansion Order](Expansion-Order.md)
8. [Tilde Expansion](Tilde-Expansion.md)
9. [Parameter Expansion](Parameter-Expansion.md)
10. [Null versus Unset](Null-versus-Unset.md)
11. [Special Parameters](Special-Parameters.md)
12. [Command Substitution](Command-Substitution.md)
13. [Arithmetic Expansion](Arithmetic-Expansion.md)
14. [Field Splitting and IFS](Field-Splitting-and-IFS.md)
15. [Pathname Expansion](Pathname-Expansion.md)
16. [Quote Removal](Quote-Removal.md)
17. [Redirection Order and Semantics](Redirection-Order-and-Semantics.md)
18. [Here Documents](Here-Documents.md)
19. [Descriptor Duplication and Closing](Descriptor-Duplication-and-Closing.md)
20. [Simple Commands and Assignment Prefixes](Simple-Commands-and-Assignment-Prefixes.md)
21. [Special Built ins versus Utilities](Special-Built-ins-versus-Utilities.md)
22. [Command Search and PATH](Command-Search-and-PATH.md)
23. [Pipelines and And Or Lists](Pipelines-and-And-Or-Lists.md)
24. [Compound Commands](Compound-Commands.md)
25. [Functions and Scope](Functions-and-Scope.md)
26. [errexit Semantics and Its Exceptions](errexit-Semantics-and-Exceptions.md)
27. [Traps and EXIT](Traps-and-EXIT.md)
28. [Subshells and the Execution Environment](Subshells-and-the-Execution-Environment.md)
29. [The read Builtin](The-read-Builtin.md)
30. [The test Builtin](The-test-Builtin.md)
31. [printf versus echo](printf-versus-echo.md)
32. [The getopts Builtin](The-getopts-Builtin.md)
33. [The eval Builtin](The-eval-Builtin.md)
34. [The exec Builtin](The-exec-Builtin.md)
35. [The set Builtin and Shell Options](The-set-Builtin-and-Shell-Options.md)
36. [POSIX.1-2024 Changes](POSIX-2024-Changes.md)
