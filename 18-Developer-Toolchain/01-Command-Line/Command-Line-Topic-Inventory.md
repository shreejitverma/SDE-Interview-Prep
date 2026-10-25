---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources:
  - "Chet Ramey, GNU Bash Reference Manual, Free Software Foundation, 2026, https://www.gnu.org/software/bash/manual/"
  - "IEEE Std 1003.1-2024 (POSIX.1-2024) Shell Command Language, IEEE and The Open Group, 2024, https://pubs.opengroup.org/onlinepubs/9799919799/utilities/V3_chap02.html"
  - "Peter Stephenson et al., The Z Shell Manual, Version 5.9, Zsh Development Group, 2022, https://zsh.sourceforge.io/Doc/"
  - "fish-shell developers, fish shell documentation, fish-shell, 2026, https://fishshell.com/docs/current/"
  - "Jim Meyering et al., GNU Coreutils Manual, Free Software Foundation, 2026, https://www.gnu.org/software/coreutils/manual/"
  - "Ken Pizzini and Paolo Bonzini, GNU sed Manual, Free Software Foundation, 2026, https://www.gnu.org/software/sed/manual/"
  - "Arnold Robbins, Effective AWK Programming: A User's Guide for GNU AWK, 5th Edition, Free Software Foundation, 2024, https://www.gnu.org/software/gawk/manual/"
  - "Stephen Dolan et al., jq Manual, jqlang, 2026, https://jqlang.org/manual/"
  - "Andrew Gallant, ripgrep User Guide and Architecture, GitHub, 2026, https://github.com/BurntSushi/ripgrep/blob/master/GUIDE.md"
  - "Google Inc., RE2 Regular Expression Engine Syntax and Documentation, Google, 2026, https://github.com/google/re2/wiki/Syntax"
  - "OpenBSD Project, OpenSSH Manual Pages: ssh(1), ssh_config(5), ssh-agent(1), ssh-keygen(1), OpenBSD, 2026, https://man.openbsd.org/ssh.1"
  - "Nicholas Marriott, tmux(1) Manual Page, OpenBSD, 2026, https://man.openbsd.org/tmux.1"
  - "Anish Athalye, Jon Gjengset, and Jose Javier Gonzalez, The Missing Semester of Your CS Education, MIT CSAIL, 2026, https://missing.csail.mit.edu/2026/"
  - "Microsoft Corporation, Language Server Protocol Specification 3.17, Microsoft, 2024, https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/"
  - "Microsoft Corporation, Debug Adapter Protocol Specification, Microsoft, 2026, https://microsoft.github.io/debug-adapter-protocol/specification"
  - "Max Brunsfeld et al., Tree-sitter Documentation, Tree-sitter, 2026, https://tree-sitter.github.io/tree-sitter/"
  - "Eelco Dolstra et al., Nix Reference Manual, NixOS Foundation, 2026, https://nix.dev/manual/nix/stable/"
  - "direnv authors, direnv documentation, direnv, 2026, https://direnv.net/"
  - "Jeff Dickey, mise-en-place documentation, mise, 2026, https://mise.jdx.dev/"
  - "Development Containers Community, Development Containers Specification, Linux Foundation, 2026, https://containers.dev/implementors/spec/"
  - "Vidar Holen, ShellCheck Wiki Check Categories, GitHub, 2026, https://github.com/koalaman/shellcheck/wiki"
  - "Command Line Interface Guidelines contributors, Command Line Interface Guidelines (clig.dev), 2026, https://clig.dev/"
---

# Command Line Topic Inventory

Curriculum coverage matrix mapping authoritative command-line and shell specifications to owning notes, depth-ladder targets, and existing vault foundations.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| GNU Bash Manual: Ch 1-3.1, 3.4, 3.5 Shell Syntax, Quoting, Parameters, Seven-Stage Word Expansion Order | [[Shell-Parsing-Tokenization-and-Expansion-Order]] | Explain | [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Expansion-Order.md\|19 Expansion Order]] |
| GNU Bash Manual: Ch 3.2, 3.3, 3.6-3.8 Pipelines, Redirections (Here-Docs, Process Substitution), Scripts | [[Pipelines-Subshells-Redirection-and-Process-Substitution]] | Explain | [[19-Shell-and-Unix-Engineering/01-Unix-Process-Model/Pipes-and-FIFOs.md\|19 Pipes and FIFOs]] |
| GNU Bash Manual: Ch 4 Builtins (set, shopt), Ch 5 Variables, Ch 6.4 Conditionals, Ch 6.7 Arrays | [[Robust-Shell-Scripting-and-ShellCheck]] | Use | [[19-Shell-and-Unix-Engineering/13-Robust-Scripting/Error-Handling-Patterns.md\|19 Error Handling Patterns]] |
| GNU Bash Manual: Ch 6.1-6.3 Invocation, Startup Files, Interactive Shells, Ch 6.10-6.11 POSIX Mode | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | [[19-Shell-and-Unix-Engineering/21-Interactive-Environment-and-Dotfiles/Startup-File-Decision-Table.md\|19 Startup Decision Table]] |
| GNU Bash Manual: Ch 7 Job Control (Basics, Builtins, Variables) | [[Rebuild-a-Minimal-Shell-With-Job-Control]] | Rebuild | [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Job-Control-Signals.md\|19 Job Control Signals]] |
| GNU Bash Manual: Ch 8-9 Readline, .inputrc, vi Mode, Programmable Completion, History | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Readline-and-inputrc.md\|19 Readline and inputrc]] |
| POSIX.1-2024 XCU Ch 2: 2.1-2.6 Quoting ($'...'), Token Recognition, Word Expansions, Pattern Matching | [[Shell-Parsing-Tokenization-and-Expansion-Order]] | Explain | [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/POSIX-2024-Changes.md\|19 POSIX 2024 Changes]] |
| POSIX.1-2024 XCU Ch 2: 2.7-2.10, 2.13 Redirection, Exit Status, Pipelines, Subshell Environments | [[Pipelines-Subshells-Redirection-and-Process-Substitution]] | Explain | [[19-Shell-and-Unix-Engineering/03-POSIX-Shell-Language/Subshells-and-the-Execution-Environment.md\|19 Subshells]] |
| POSIX.1-2024 XCU Ch 2: 2.11-2.12, 2.15 Job Control, Signals and Error Handling (trap), Special Builtins | [[Robust-Shell-Scripting-and-ShellCheck]] | Use | [[19-Shell-and-Unix-Engineering/13-Robust-Scripting/Cleanup-Traps-and-Re-Raising-Signals.md\|19 Traps]] |
| Zsh Manual: Ch 1-9 Invocation, Startup Files (.zshenv, .zshrc), Grammar, Multios, Prompt Hooks | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | [[19-Shell-and-Unix-Engineering/06-Zsh-Ksh-Dash-and-Other-Shells/Zsh-Startup-Files.md\|19 Zsh Startup]] |
| Zsh Manual: Ch 10-17 Jobs, Parameter Expansion Flags, Glob Qualifiers, Options, Builtins | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | none |
| Zsh Manual: Ch 18-22 ZLE Line Editor, Completion Widgets (compsys, compinit), Zsh C Modules | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | none |
| Fish Documentation: Autosuggestions, Abbreviations, Language Design, No Word Splitting, Rust Rewrite | [[POSIX-sh-Bash-Zsh-and-Fish-Compared]] | Use | none |
| GNU Coreutils: Ch 1-6 File Output (cat, base64), Parts of Files (head, tail), Summarizing (sha256sum) | [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]] | Use | [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/Checksums-and-Integrity.md\|19 Checksums]] |
| GNU Coreutils: Ch 7-9 Operating on Sorted Files (sort, uniq, comm), Fields (cut, paste, join), Version Sort | [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]] | Use | [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/sort-Keys-and-Stability.md\|19 Sort Keys]] |
| GNU Coreutils: Ch 10-18 Directory Listing (ls), File Ops (cp, mv, rm), Links (ln), Attributes (chmod, stat) | [[Robust-Shell-Scripting-and-ShellCheck]] | Use | [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/mktemp.md\|19 mktemp]] |
| GNU Coreutils: Ch 19-26 Working Context (stty), System Context, Modified Invocation (env, stdbuf, timeout) | [[Pipe-Buffering-and-Stdbuf]] | Use | [[19-Shell-and-Unix-Engineering/08-Coreutils-and-the-Userland/stdio-Buffering-and-stdbuf.md\|19 stdbuf]] |
| GNU sed Manual: Running sed (-n, -i, -E), Cycle Buffers (Pattern vs Hold Space), Multiline (N, D, P) | [[Sed-and-Awk-Stream-Processing]] | Explain | [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/sed-Pattern-and-Hold-Space.md\|19 sed Hold Space]] |
| GNU gawk Manual: awk Language (RS, FS, OFS, Associative Arrays, Functions), Two-Way I/O | [[Sed-and-Awk-Stream-Processing]] | Explain | [[15-Technical-Whitepapers/08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin.md\|DevOps Tooling AWK Section]] |
| jq Manual: Filters, Operators (map, select, group_by, INDEX), Streaming Parser (--stream), Slurpfile | [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]] | Use | [[19-Shell-and-Unix-Engineering/09-Structured-Data-and-HTTP/jq-Language.md\|19 jq Language]] |
| ripgrep Guide & Architecture: Automatic Filtering (.gitignore), SIMD Prefilters (Teddy), Lazy DFA, bstr | [[Ripgrep-Internals-and-Fast-Text-Search]] | Explain | [[19-Shell-and-Unix-Engineering/07-Regex-and-Text-Processing/ripgrep-Internals.md\|19 ripgrep Internals]] |
| Regex Engines: Automata (Thompson NFA, Lazy DFA) vs Backtracking (PCRE2, Python re), ReDoS Vulnerabilities | [[Regex-Engines-Automata-Backtracking-and-ReDoS]] | Explain | [[01-CS-Foundations/Computer-Science-Canon/Automata-and-Computability.md\|Automata and Computability]] |
| OpenSSH Manual Pages: ssh(1), ssh_config(5) (ProxyJump, ControlMaster, ControlPersist, IdentityAgent) | [[SSH-In-Depth-Keys-Forwarding-Certificates-and-FIDO2]] | Use | [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Config-and-ProxyJump.md\|19 SSH Config]] |
| OpenSSH Manual Pages: Certificate Authorities, FIDO2 Hardware Keys (ed25519-sk), Key Revocation (KRL) | [[Harden-SSH-Access-With-Certificates-and-Hardware-Keys]] | Use | [[19-Shell-and-Unix-Engineering/11-Networking-and-SSH/SSH-Certificates.md\|19 SSH Certificates]] |
| tmux(1) Manual Page: Socket Server, Hierarchy (Session, Window, Pane), PTY Multiplexing, Control Mode -C | [[Tmux-Architecture-and-Session-Persistence]] | Use | [[19-Shell-and-Unix-Engineering/02-Terminals-and-Job-Control/Terminal-Multiplexers-tmux-and-screen.md\|19 Terminal Multiplexers]] |
| MIT Missing Semester (2020 & 2026): Shell, Scripting, Editors, Data Wrangling, Environments, Packaging | [[Command-Line-Skill-Ladder]] | Use | none |
| LSP 3.17 & DAP Specs: JSON-RPC 2.0 Base Protocol, Synchronization, Semantic Tokens, DAP Lifecycle | [[Modal-Editing-LSP-Tree-Sitter-and-DAP]] | Explain | none |
| Tree-sitter Docs: Incremental GLR Parsing, S-expression Syntax Queries (.scm), Structural Highlighting | [[Modal-Editing-LSP-Tree-Sitter-and-DAP]] | Explain | none |
| Nix & Home Manager Manuals: Store Paths, Cryptographic Hashing, Derivations (.drv), Hermetic Sandbox | [[Nix-Store-Derivations-and-Flakes-Internals]] | Explain | [[13-Agentic-AI/Agentic-Harness/components/dotfiles-nix.md\|Agentic Harness dotfiles-nix]] |
| direnv & mise Docs: Prompt Hooks (.envrc), Tool Pinning (.rust-toolchain.toml, uv), Shims vs PATH | [[Dotfiles-Nix-Direnv-Mise-and-Devcontainers]] | Use | none |
| Devcontainers Spec: devcontainer.json Reference (Images, Features, Lifecycle Hooks Execution Order) | [[Dotfiles-Nix-Direnv-Mise-and-Devcontainers]] | Use | none |
| ShellCheck Wiki: Static Analysis Warnings (SC1xxx Parser, SC2xxx Semantic Bugs, SC3xxx Portability) | [[Robust-Shell-Scripting-and-ShellCheck]] | Use | [[19-Shell-and-Unix-Engineering/14-Testing-and-Static-Analysis/ShellCheck-Warning-Families.md\|19 ShellCheck Families]] |
| clig.dev & AXI: Dual-Mode CLI Architecture, Standard Streams, Exit Codes, 10 AXI Principles for AI Agents | [[CLI-Design-and-Agent-Friendly-Interfaces-AXI]] | Use | [[13-Agentic-AI/Agentic-Harness/components/axi.md\|AXI Principles]] |

## Out of scope

- GNU Bash Manual Ch 10 & Appendices: Autotools compilation and GNU Free Documentation License text.
- Zsh Manual Ch 23-26: Legacy TCP and FTP client systems and calendar utilities, omitted due to modern standard protocol tools.
- MIT Missing Semester Student Q&A Sessions: Unstructured open discussions omitted from technical curriculum reference.
- Codecov Bash Uploader security incident: Reassigned exclusively to 08-Software-Supply-Chain to ground CI/CD credential compromise and curl-to-sh attack vectors.
