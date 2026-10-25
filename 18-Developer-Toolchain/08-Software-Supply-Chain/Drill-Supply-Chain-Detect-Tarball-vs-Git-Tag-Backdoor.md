---
type: problem
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Detect a Malicious Release Tarball Payload Hidden from Git

> [!summary] Scope
> Hands-on forensic drill recreating an xz-style discrepancy where an upstream release archive contains modified build scripts that do not exist in the source Git repository tag.
> Guides the engineer through unpacking artifacts, diffing source trees with diffoscope, analyzing injected ELF symbol redirection hooks, and proving the discrepancy.
> Builds rapid forensic triage capability when auditing release tarballs.
> Includes automated scenario verification and detailed forensic analysis notes.

## Related

- Prerequisites: [[Reproducible-Builds-as-a-Security-Control]]
- Next: [[Drill-Supply-Chain-Exploit-and-Fix-Dependency-Confusion]]
- Lab: `code/08-Software-Supply-Chain/tarball_backdoor_detection/` (requires: linux)
