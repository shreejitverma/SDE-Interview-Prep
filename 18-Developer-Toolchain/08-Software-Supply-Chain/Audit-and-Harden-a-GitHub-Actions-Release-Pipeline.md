---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Audit and Harden a GitHub Actions Release Pipeline

> [!summary] Scope
> Step-by-step remediation guide for securing CI/CD release workflows against supply chain compromise.
> Walks through restricting top-level and job-level GITHUB_TOKEN permissions to read-only defaults, pinning all third-party actions to immutable full-length commit SHAs, isolating compilation environments from publishing credentials, and emitting signed SLSA L3 provenance attestations.
> Includes automated verification scripts and Scorecard CI evaluation setups.
> Delivers hardening checklists ready for enterprise repository rollouts.

## Related

- Prerequisites: [[OpenSSF-Scorecard-and-CI-CD-Pipeline-Hardening]], [[Sigstore-Keyless-Signing-Cosign-Fulcio-and-Rekor]]
- Next: [[Drill-Supply-Chain-Detect-Tarball-vs-Git-Tag-Backdoor]]
- See also: [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock|Agent Shell Sandboxing Seatbelt and Landlock]]
- Lab: `code/08-Software-Supply-Chain/harden_github_actions/`
