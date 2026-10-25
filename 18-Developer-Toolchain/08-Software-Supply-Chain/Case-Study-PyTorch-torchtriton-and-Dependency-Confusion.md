---
type: case-study
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: PyTorch torchtriton Compromise and Enterprise Dependency Confusion

> [!summary] Scope
> Analysis of the December 2022 PyTorch-nightly supply chain compromise via torchtriton, contextualized with Alex Birsan original 2021 dependency confusion research.
> Explains how pip default resolution priority allowed an attacker to register torchtriton on public PyPI with a higher version number than the custom nightly index, resulting in arbitrary code execution and secret exfiltration during nightly installs.
> Details index pinning and scoped namespace solutions.
> Provides prevention guidelines for multi-index package installation pipelines.

## Related

- Prerequisites: [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]]
- Next: [[Case-Study-Shai-Hulud-and-Self-Replicating-Registry-Worms]]
