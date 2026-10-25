---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Regular Expressions and Text Processing

Most shell programs are text pipelines.
These notes cover regex flavors and engines, then grep, sed, awk, and their companions as the languages they are, and the locale settings that silently change results.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [BRE ERE and PCRE](BRE-ERE-and-PCRE.md)
2. [POSIX Leftmost Longest Matching](POSIX-Leftmost-Longest-Matching.md)
3. [Regex Engines NFA DFA and Backtracking](Regex-Engines-NFA-DFA-and-Backtracking.md)
4. [ReDoS](ReDoS.md)
5. [grep in Depth](grep-in-Depth.md)
6. [ripgrep Internals](ripgrep-Internals.md)
7. [sed: Pattern Space and Hold Space](sed-Pattern-and-Hold-Space.md)
8. [sed: Addresses and Branching](sed-Addresses-and-Branching.md)
9. [sed: In-Place Editing Portability](sed-In-Place-Editing-Portability.md)
10. [The awk Language](awk-Language.md)
11. [awk Associative Arrays and Idioms](awk-Associative-Arrays-and-Idioms.md)
12. [awk Implementations](awk-Implementations.md)
13. [Perl One Liners](Perl-One-Liners.md)
14. [Python as a Shell Companion](Python-as-a-Shell-Companion.md)
15. [tr, cut, and paste](tr-cut-and-paste.md)
16. [join and comm](join-and-comm.md)
17. [column, fold, and fmt](column-fold-and-fmt.md)
18. [Locale and LC_ALL](Locale-and-LC_ALL.md)
19. [Unicode and UTF 8 in Text Tools](Unicode-and-UTF-8-in-Text-Tools.md)
