---
type: case-study
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: Shai-Hulud and Self-Replicating Package Registry Worms

> [!summary] Scope
> Technical examination of the September 2025 Shai-Hulud self-propagating npm package worm.
> Details how the malicious package exploited automatic lifecycle install hooks to harvest developer environment variables and registry publishing tokens, automatically backdooring and publishing new versions of all packages owned by the compromised maintainer.
> Synthesizes defensive policies for disabling arbitrary execution during dependency installation and enforcing scoped, tokenless publishing.
> Analyzes the dynamics of rapid package infection across interconnected package ecosystems.

## Related

- Prerequisites: [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]]
