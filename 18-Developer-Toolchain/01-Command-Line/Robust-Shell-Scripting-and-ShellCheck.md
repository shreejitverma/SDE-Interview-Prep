---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Robust Shell Scripting and ShellCheck

> [!summary] Scope
> Owns production shell scripting best practices, dissecting why strict mode (set -euo pipefail) silently fails inside conditionals and command assignments (SC2155).
> Covers reliable signal trapping on EXIT, ERR, INT, and TERM, safe temporary file creation (mktemp), idempotent file operations, and static analysis with ShellCheck.
> Reaches Use through Debug, handing language migration guidelines to Command-Line-Decision-Records.md.

## Related

- Prerequisites: [[Pipelines-Subshells-Redirection-and-Process-Substitution]], [[POSIX-sh-Bash-Zsh-and-Fish-Compared]]
- Next: [[Regex-Engines-Automata-Backtracking-and-ReDoS]]
- See also: [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/errexit-Semantics-and-Exceptions|errexit Semantics and Exceptions]], [[19-Shell-and-Unix-Engineering/14-Testing-and-Static-Analysis/ShellCheck-Warning-Families|ShellCheck Warning Families]]
- Lab: `code/01-Command-Line/robust_scripting_traps/`
