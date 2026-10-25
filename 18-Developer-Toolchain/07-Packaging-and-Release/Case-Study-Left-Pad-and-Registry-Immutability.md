---
type: case-study
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: The left-pad Outage and Package Registry Immutability

> [!summary] Scope
> Root-cause review of the March 2016 left-pad unpublish incident that broke thousands of global software builds within minutes.
> Analyzes the architectural failure of public package registries to treat published software versions as immutable content-addressed artifacts.
> Details how modern package registries redesigned package revocation via non-destructive yank metadata and permanent package name reservation.
> Examines the trade-offs between maintainer autonomy and downstream dependency stability.

## Related

- Prerequisites: [[Release-Trains-Backports-and-Artifact-Registries]], [[Lockfiles-Vendoring-and-Offline-Mirrors]]
