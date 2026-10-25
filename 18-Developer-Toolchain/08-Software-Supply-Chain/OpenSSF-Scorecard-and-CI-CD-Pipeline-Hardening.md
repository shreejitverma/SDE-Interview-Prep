---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# CI/CD Pipeline Hardening and OpenSSF Scorecard

> [!summary] Scope
> Defense-in-depth engineering for continuous integration and delivery runners.
> Examines OpenSSF Scorecard v5.6.0 heuristics, GitHub Actions security anti-patterns including pull_request_target abuse, runner memory scraping, mutable action tag poisoning, and least-privilege token permissions.
> Establishes automated CI auditing policies using immutable commit SHA pinning and isolated ephemeral environments.
> Covers secret redacting and exfiltration prevention in automated build logs.

## Related

- Prerequisites: none
- Next: [[Reproducible-Builds-as-a-Security-Control]]
- See also: [[19-Shell-and-Unix-Engineering/22-Shell-and-AI-Agents/Agent-Shell-Sandboxing-Seatbelt-and-Landlock|Agent Shell Sandboxing Seatbelt and Landlock]]
