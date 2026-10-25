---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Respond to a Compromised Transitive Dependency

> [!summary] Scope
> Incident response runbook for rapidly triaging and neutralizing malicious open-source packages discovered in upstream registries.
> Details querying organizational SBOM repositories and lockfiles for blast-radius assessment, severing outbound network egress from build runners, revoking and rotating potentially scraped runner secrets, and publishing emergency transitive overrides.
> Includes post-incident transparency log audit steps and automated dependency rollback verification.
> Provides communication templates for reporting security advisories to downstream consumers.

## Related

- Prerequisites: [[SBOM-Standards-SPDX-CycloneDX-and-VEX]], [[Dependency-Confusion-Typosquatting-and-Registry-Attacks]]
- Next: [[Audit-and-Harden-a-GitHub-Actions-Release-Pipeline]]
- Lab: `code/08-Software-Supply-Chain/quarantine_response/`
