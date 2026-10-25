---
type: case-study
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: CI/CD Secret Exfiltration: Codecov Bash Uploader and tj-actions

> [!summary] Scope
> Comparative forensic analysis of CI/CD secret harvesting incidents: the April 2021 Codecov Bash Uploader breach and the March 2025 tj-actions/changed-files compromise (CVE-2025-30066).
> Details how piping unverified remote scripts into CI runners or executing mutable third-party Git action tags exposes runner memory and environment tokens to exfiltration.
> Formulates strict immutability and credential-isolation controls for CI pipelines.
> Details defense strategies against memory scraping in continuous integration workers.

## Related

- Prerequisites: [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]]
- Next: [[Case-Study-PyTorch-torchtriton-and-Dependency-Confusion]]
