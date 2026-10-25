---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 06 - Regular Expressions and Text Processing

Most shell programs are text pipelines.
This volume covers regular expression flavors and the engines behind them, then grep, sed, and awk as the languages they are, and the locale settings that silently change their results.

## Prerequisites

- [Volume 02 - The POSIX Shell Command Language](../Volume_02_The_POSIX_Shell_Command_Language/README.md)

## Chapters

1. [Regular Expression Flavors: BRE, ERE, and PCRE](Chapter_01_Regex_Flavors_BRE_ERE_and_PCRE.md) - Syntax and semantic differences, POSIX leftmost-longest matching, and which tool uses which flavor.
2. [Regex Engines: Thompson NFA, DFA, and Backtracking](Chapter_02_Regex_Engines_NFA_DFA_and_Backtracking.md) - Engine theory and complexity guarantees, RE2 and the Rust regex crate, and catastrophic backtracking (ReDoS).
3. [grep and ripgrep Internals](Chapter_03_Grep_and_Ripgrep_Internals.md) - Literal search optimizations, memchr and SIMD prefilters, binary file detection, and directory traversal.
4. [sed as a Language](Chapter_04_Sed_as_a_Language.md) - Pattern space and hold space, addresses, branching, and the differences in in-place editing.
5. [awk I: The Language](Chapter_05_Awk_The_Language.md) - Records and fields, patterns and actions, associative arrays, and the execution model.
6. [awk II: Programs and Idioms](Chapter_06_Awk_Programs_and_Idioms.md) - Reports, joins, state machines, and one-pass aggregation.
7. [awk III: Implementations](Chapter_07_Awk_Implementations.md) - The one true awk, gawk, mawk, and BusyBox awk, with their extensions and performance differences.
8. [The Small Filters: tr, cut, paste, join, comm, column, fold, and fmt](Chapter_08_The_Small_Filters.md) - What each filter does, its edge cases, and how to compose them.
9. [Locale, Collation, and Unicode](Chapter_09_Locale_Collation_and_Unicode.md) - `LC_ALL`, `LC_COLLATE`, and `LC_CTYPE`, UTF-8 handling, and why `LC_ALL=C` changes both speed and results.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v06-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
