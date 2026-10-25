---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Locale, Collation, and Unicode in CLI Tools

> [!summary] Scope
> Owns internationalization and encoding subsystems in command-line environments.
> Analyzes how LC_ALL, LC_COLLATE, and LC_CTYPE control byte versus multi-byte interpretation, regex range semantics ([a-z] matching uppercase in some en_US implementations), consistent sort order across sort/join/comm, and performance acceleration under LC_ALL=C.
> Details filename normalization pitfalls between macOS (HFS+ stores NFD, APFS preserves the form but matches normalization-insensitively) and Linux, which stores raw bytes that are usually NFC.
> Reaches Use through Debug, handing log processing runbooks to Analyze-Gigabyte-Terminal-Logs.md.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]], [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]]
- Next: [[Analyze-Gigabyte-Terminal-Logs]]
- See also: [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/Locale-and-LC_ALL|Locale and LC_ALL]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/Unicode-and-UTF-8-in-Text-Tools|Unicode and UTF 8 in Text Tools]], [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/Userland-Portability-Traps|Userland Portability Traps]], [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/sort-Keys-and-Stability|sort Keys and Stability]]
- Lab: `code/01-Command-Line/locale_collation_bench/`
