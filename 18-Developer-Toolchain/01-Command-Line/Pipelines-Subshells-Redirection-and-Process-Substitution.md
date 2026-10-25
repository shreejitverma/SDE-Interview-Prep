---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Pipelines, Subshells, Redirection, and Process Substitution

> [!summary] Scope
> Owns shell pipeline assembly, kernel pipe and dup2 mechanics, file descriptor duplication, subshell environment cloning, and exit status propagation (PIPESTATUS and set -o pipefail).
> Details here-documents, here-strings, and process substitution (<(...) and >(...)) via /dev/fd and named FIFOs.
> Reaches Use through Rebuild, handing stdio buffer deadlocks to Pipe-Buffering-and-Stdbuf.md and signal trap propagation to Robust-Shell-Scripting-and-ShellCheck.md.

## Related

- Prerequisites: [[Shell-Parsing-Tokenization-and-Expansion-Order]]
- Next: [[Pipe-Buffering-and-Stdbuf]]
- See also: [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Pipes-and-FIFOs|Pipes and FIFOs]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Pipelines-and-And-Or-Lists|Pipelines and And Or Lists]], [[19-Shell-and-Unix-Engineering/04-Bash/Process-Substitution|Process Substitution]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Here-Documents|Here Documents]], [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Subshells-and-the-Execution-Environment|Subshells and the Execution Environment]]
- Lab: `code/01-Command-Line/pipeline_redirection/`
