---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Regex Engines: Automata, Backtracking, and ReDoS

> [!summary] Scope
> Owns regular expression engine theory and implementation architectures, contrasting automata-based engines (Thompson NFA, PikeVM, lazy DFA) with recursive backtracking matchers.
> Details polynomial time guarantees versus catastrophic backtracking vulnerability (ReDoS), literal prefiltering, and feature divergence across PCRE2, Google RE2, Rust regex, and Python re.
> Reaches Explain through Debug, handing tool search integration to Ripgrep-Internals-and-Fast-Text-Search.md.

## Related

- Prerequisites: none
- Next: [[Ripgrep-Internals-and-Fast-Text-Search]]
- See also: [[01-CS-Foundations/Computer-Science-Canon/Automata-and-Computability|Automata and Computability]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/Regex-Engines-NFA-DFA-and-Backtracking|Regex Engines NFA DFA and Backtracking]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/ReDoS|ReDoS]], [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/BRE-ERE-and-PCRE|BRE ERE and PCRE]]
- Lab: `code/01-Command-Line/redos_benchmark/`
