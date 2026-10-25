---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Shell Parsing, Tokenization, and Expansion Order

> [!summary] Scope
> Owns POSIX shell parsing and grammar rules, lexing, alias substitution, and the Bash word expansion order: brace expansion; then tilde expansion, parameter expansion, arithmetic expansion, command substitution, and process substitution in one left-to-right pass; then word splitting, pathname expansion (globbing), and quote removal.
> Analyzes quoting semantics (single, double, and POSIX.1-2024 dollar-single quotes) and IFS boundary hazards.
> Reaches Use through Rebuild, handing pipeline execution mechanics to Pipelines-Subshells-Redirection-and-Process-Substitution.md.

## Related

- Prerequisites: none
- Next: [[Pipelines-Subshells-Redirection-and-Process-Substitution]]
- See also: [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Expansion-Order|Expansion Order]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Quoting-Backslash-Single-and-Double|Quoting Backslash Single and Double]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Dollar-Single-Quotes|Dollar Single Quotes]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Field-Splitting-and-IFS|Field Splitting and IFS]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Pathname-Expansion|Pathname Expansion]]
- Lab: `code/01-Command-Line/expansion_order/`
