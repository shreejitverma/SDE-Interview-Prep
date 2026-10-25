---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Dotfiles, Nix, Direnv, Mise, and Devcontainers

> [!summary] Scope
> Owns developer environment reproducibility and configuration management.
> Evaluates dotfile managers (GNU Stow, chezmoi, Home Manager), directory-scoped environment activation (direnv prompt hooks), runtime tool version pinning (mise shims vs PATH manipulation, .rust-toolchain.toml, uv), and devcontainer.json lifecycle hooks.
> Reaches Use through Decide at scale, handing Nix internal store architecture to Nix-Store-Derivations-and-Flakes-Internals.md.

## Related

- Prerequisites: none
- Next: [[Nix-Store-Derivations-and-Flakes-Internals]]
- See also: [[13-Agentic-AI/Agentic-Harness/components/dotfiles-nix|dotfiles nix]]
- Lab: `code/01-Command-Line/direnv_mise_shim/`
