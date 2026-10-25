---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Chet Ramey, GNU Bash Reference Manual, Free Software Foundation, 2026, https://www.gnu.org/software/bash/manual/"
  - "IEEE Std 1003.1-2024 (POSIX.1-2024) Shell Command Language, IEEE and The Open Group, 2024, https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html"
  - "Jim Meyering et al., GNU Coreutils Manual, Free Software Foundation, 2026, https://www.gnu.org/software/coreutils/manual/"
  - "Andrew Gallant, ripgrep User Guide and Architecture, GitHub, 2026, https://github.com/BurntSushi/ripgrep/blob/master/GUIDE.md"
  - "OpenBSD Project, OpenSSH Manual Pages, OpenBSD, 2026, https://man.openbsd.org/ssh.1"
  - "Nicholas Marriott, tmux(1) Manual Page, OpenBSD, 2026, https://man.openbsd.org/tmux.1"
  - "Command Line Interface Guidelines contributors, Command Line Interface Guidelines (clig.dev), 2026, https://clig.dev/"
---

# Command Line Skill Ladder

Defines observable, testable engineering capabilities across shell engineering and developer command-line environments from junior engineer (L4) to distinguished engineer (L8+).

## Level 4 (L4)

- Constructs robust multi-stage shell pipelines using standard redirections, process substitutions, and here-documents without temporary file leaks as mastered in [[Pipelines-Subshells-Redirection-and-Process-Substitution]].
- Traverses complex directory hierarchies safely with `find` and `xargs` using NUL-delimited records to handle whitespace and unusual filenames as shown in [[File-Discovery-and-Batch-Execution-Find-Fd-Xargs-Parallel]].
- Manages persistent remote development sessions, detached windows, and split panes using `tmux` as detailed in [[Tmux-Architecture-and-Session-Persistence]].
- Configures client SSH connectivity, custom host aliases, and bastion jump hosts via `ProxyJump` without agent forwarding hazards as outlined in [[SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2]].
- Inspects and cleans shell scripts using `shellcheck` to eliminate common quoting mistakes and parameter expansion bugs as guided in [[Robust-Shell-Scripting-and-ShellCheck]].

## Level 5 (L5)

- Diagnoses and eliminates pipeline latency lag and deadlocks caused by libc `stdio` block buffering using `stdbuf` as practiced in [[Pipe-Buffering-and-Stdbuf]].
- Authors idempotent shell automation with strict error checking, cleanup traps on `EXIT` and `ERR`, and safe temporary directory allocation as executed in [[Drill-Command-Line-Quoting-and-Signal-Trap-Race]].
- Processes large tabular and JSON data streams using relational coreutils (`join`, `comm`, `sort`) and `jq` filter transformations as taught in [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]].
- Triages multi-gigabyte production server logs under incident conditions using parallel decompression, byte-locale acceleration (`LC_ALL=C`), and `ripgrep` pre-filtering as codified in [[Analyze-Gigabyte-Terminal-Logs]].
- Recovers corrupted terminal states, wedged raw-mode settings, and stale `SSH_AUTH_SOCK` variables inside reattached `tmux` sessions as resolved in [[Drill-Command-Line-SSH-Multiplexing-and-Broken-PTY]].
- Pins polyglot language toolchains and project environments locally using `mise` and `direnv` as detailed in [[Dotfiles-Nix-Direnv-Mise-and-Devcontainers]].

## Level 6 (L6)

- Rebuilds a minimal POSIX-compliant shell in C++ or Rust supporting lexical tokenization, pipelines, file redirections, and job control with `tcsetpgrp` as completed in [[Rebuild-a-Minimal-Shell-With-Job-Control]].
- Identifies and rewrites pathological regular expressions vulnerable to catastrophic backtracking (ReDoS) into linear-time automata patterns as demonstrated in [[Drill-Command-Line-Catastrophic-Backtracking-and-Stream-Bottleneck]].
- Provisions corporate OpenSSH Certificate Authorities, issues short-lived user and host certificates, and maintains Key Revocation Lists as engineered in [[Harden-SSH-Access-With-Certificates-and-Hardware-Keys]].
- Inspects system call boundaries and kernel state of wedged processes using `strace`, `lsof`, and `/proc` without causing thread stalls as detailed in [[System-Inspection-and-Syscall-Tracing-From-the-Shell]].
- Authors high-speed stream parsers using advanced `sed` hold-space cycles and `gawk` associative arrays as established in [[Sed-and-Awk-Stream-Processing]].
- Configures modal terminal editor integrations leveraging the Language Server Protocol, Tree-sitter AST queries, and the Debug Adapter Protocol as modeled in [[Modal-Editing-LSP-Tree-Sitter-and-DAP]].

## Level 7 (L7)

- Designs production command-line interfaces adhering to both human ergonomics (`clig.dev`) and machine-readable Agent eXperience Interface (AXI) standards as formalized in [[CLI-Design-and-Agent-Friendly-Interfaces-AXI]].
- Authors comprehensive Architectural Decision Records defining organizational boundaries for when to migrate brittle shell scripts to compiled languages as codified in [[Command-Line-Decision-Records]].
- Formulates organization-wide hermetic developer environment strategies balancing Nix flakes against Devcontainers and lightweight version shims as evaluated in [[Command-Line-Decision-Records]].
- Inspects and debugs `/nix/store` derivation graphs, realization closures, and NAR serialization to resolve cache-miss bottlenecks as explained in [[Nix-Store-Derivations-and-Flakes-Internals]].
- Tunes container and virtual machine bind-mount I/O performance on macOS hosts to eliminate developer build latency as investigated in [[Containers-and-VMs-for-macOS-Development]].

## Level 8+ (L8+)

- Establishes organizational CLI interface standards and automated linting rules ensuring that internal developer tooling operates deterministically with minimal token footprint for autonomous AI agents as directed in [[CLI-Design-and-Agent-Friendly-Interfaces-AXI]].
- Designs zero-trust workstation access architecture across thousands of engineers using hardware-backed FIDO2 resident keys and automated SSH certificate minting as proven in [[Harden-SSH-Access-With-Certificates-and-Hardware-Keys]].
- Architects organization-wide monorepo developer workstation environments that guarantee reproducible byte-for-byte realization across Linux and macOS platforms as guided in [[Nix-Store-Derivations-and-Flakes-Internals]].
- Standardizes internationalization, byte-collation performance, and cross-platform filesystem normalization policies across all internal build and data pipelines as mastered in [[Locale-Collation-and-Unicode-in-CLI-Tools]].
- Anticipates and directs the architectural transition from legacy POSIX shell tooling to type-safe, sandboxed, and agent-orchestrated developer interfaces as unified across [[Command-Line-Decision-Records]].
