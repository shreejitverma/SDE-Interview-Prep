---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Detached Rebase Failure and Dangling Tree Recovery

> [!summary] Scope
> Owns an interactive hands-on drill simulating a catastrophic rebase abort across stacked branches that leaves work detached and reflogs expired.
> Reaches the Debug and Use rungs under timed pressure, presenting the engineer with a broken repository state, folded diagnostic hints, and a model recovery command script reconstructing the target commits and branches.
> Leaves packfile and index-level corruptions to the corrupted packfile drill.

## Related

- Prerequisites: [[Git-Rebase-Internals-and-Stacked-Branches]], [[Recover-Lost-Git-Work]]
- Next: [[Drill-Git-Corrupted-Packfile-and-MIDX-Rescue]]
- Lab: `code/02-Version-Control/drill_rebase_recovery/`
