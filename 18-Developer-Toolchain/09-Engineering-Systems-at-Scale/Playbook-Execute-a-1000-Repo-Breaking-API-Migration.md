---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Execute a 1,000-Repository Breaking API Migration

> [!summary] Scope
> Owns the end-to-end execution runbook for deprecating an existing API and executing an automated breaking change campaign across hundreds to thousands of services.
> Reaches Decide at Scale rungs by providing templates for introducing backward-compatible shims, generating and validating AST codemods, sharding pull requests across service owners with automated presubmit gates, tracking adoption burndown, and safely deleting the legacy API.
> Sets operational boundaries for platform engineering teams.

## Related

- Prerequisites: [[Large-Scale-Changes-and-Codemods-Across-Repos]], [[Platform-Engineering-Paved-Roads-and-Deprecation]]
- Next: [[Playbook-Write-a-Toolchain-Migration-RFC]]
- See also: [[19-Shell-and-Unix-Engineering/23-Shell-at-Organizational-Scale/Migrating-Shell-to-Real-Languages|Migrating Shell to Real Languages]]
