---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Nix Store, Derivations, and Flakes Internals

> [!summary] Scope
> Owns the purely functional package management architecture of Nix.
> Explains /nix/store cryptographic hashing (input-addressed vs content-addressed store paths), derivation (.drv) evaluation and realization, Nix Archive (NAR) serialization, sandboxed builds without network access except for fixed-output derivations, binary cache substituters, and flake.lock input pinning.
> Reaches Explain through Decide at scale, bounding itself from general build systems covered in 06-Build-Systems.

## Related

- Prerequisites: [[Dotfiles-Nix-Direnv-Mise-and-Devcontainers]]
- Next: [[System-Inspection-and-Syscall-Tracing-From-the-Shell]]
- See also: [[13-Agentic-AI/Agentic-Harness/components/dotfiles-nix|dotfiles nix]]
- Lab: `code/01-Command-Line/nix_drv_inspector/`
