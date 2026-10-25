---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# POSIX sh, Bash, Zsh, and Fish Compared

> [!summary] Scope
> Owns the comparative architectural and linguistic analysis of POSIX sh (dash), Bash, Zsh, and fish (covering its Rust rewrite in fish 4.x).
> Explains differences in array indices, word splitting defaults, glob qualifiers, startup file execution cascades (login vs non-login, interactive vs non-interactive), and programmable completion frameworks (complete, compsys, and fish completions).
> Reaches Use through Decide at scale, handing scripting reliability rules to Robust-Shell-Scripting-and-ShellCheck.md.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]]
- Next: [[Robust-Shell-Scripting-and-ShellCheck]]
- See also: [[19-Shell-and-Unix-Engineering/27-Reference/Shell-Feature-Portability-Map|Shell Feature Portability Map]], [[19-Shell-and-Unix-Engineering/06-Zsh-Ksh-Dash-and-Other-Shells/Zsh-Startup-Files|Zsh Startup Files]]
- Lab: `code/01-Command-Line/shell_startup_matrix/`
