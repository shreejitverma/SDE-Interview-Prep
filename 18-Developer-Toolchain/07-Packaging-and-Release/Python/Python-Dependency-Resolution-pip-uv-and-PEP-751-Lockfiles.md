---
type: concept
track: [sde, distinguished, ai-eng]
level:
status: seed
last_reviewed:
sources: []
---

# Python Dependency Resolution: pip, uv, and PEP 751 Lockfiles

> [!summary] Scope
> Algorithmic comparison between pip backtracking resolver via resolvelib and uv PubGrub-based solver with version maps and global hardlinked caches.
> Details universal lockfile invariants in uv.lock, the standardized PEP 751 pylock.toml file format, and cross-platform resolution constraints.
> Explores resolution failure troubleshooting and lockfile synchronization workflows.
> Analyzes performance benchmarks and cache deduplication mechanics in large monorepos.

## Related

- Prerequisites: [[Dependency-Resolution-SAT-PubGrub-and-MVS]], [[Lockfiles-Vendoring-and-Offline-Mirrors]]
- Next: [[Python-Conda-and-Pixi-for-Polyglot-Environments]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/02-Python-Core|02 Python Core]]
