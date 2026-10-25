---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 17 - The Interactive Environment and Dotfiles

An expert's interactive shell is a tuned instrument: predictable startup, fast prompt, good completion, searchable history, and reproducible configuration.
This volume covers each piece and how it is managed across machines.

## Prerequisites

- [Volume 01 - Terminals, TTYs, and Job Control](../Volume_01_Terminals_TTYs_and_Job_Control/README.md)
- [Volume 03 - Bash in Depth](../Volume_03_Bash_in_Depth/README.md)

## Chapters

1. [Startup Files for bash and zsh](Chapter_01_Startup_Files_for_Bash_and_Zsh.md) - Which files run for login, interactive, and non-interactive shells, as a decision table.
2. [PATH and Environment Management](Chapter_02_PATH_and_Environment_Management.md) - Ordering and deduplication, path_helper on macOS, and per-project environments.
3. [Prompts](Chapter_03_Prompts.md) - PS1 and PROMPT escapes, version control status, prompt performance, and asynchronous prompts.
4. [Writing Completions](Chapter_04_Writing_Completions.md) - bash complete and compgen, zsh compsys, and generating completions from a command line tool.
5. [History Design](Chapter_05_History_Design.md) - History files, sharing history across sessions, deduplication, and search.
6. [Productivity Tools: fzf, direnv, zoxide, and atuin](Chapter_06_Productivity_Tools_fzf_direnv_zoxide_and_atuin.md) - How each tool integrates with the shell, and what each one costs at startup.
7. [Dotfiles Management](Chapter_07_Dotfiles_Management.md) - Symlink managers, chezmoi, and Nix home-manager.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v17-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
