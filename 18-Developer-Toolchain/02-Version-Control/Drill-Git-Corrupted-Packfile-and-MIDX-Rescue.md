---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Corrupted Packfile Index and Multi-Pack-Index Rescue

> [!summary] Scope
> Owns a hands-on troubleshooting drill where a repository suffers truncated packfile indexes and broken multi-pack-index chains right before an urgent production deployment.
> Reaches the Debug rung by providing a realistic reproduction script, symptoms of failed object read operations, layered hints, and a verified step-by-step resolution script that regenerates index files from raw pack data and rebuilds the MIDX.
> Delineates its boundary from simple reflog recovery.

## Related

- Prerequisites: [[Git-GC-Maintenance-MIDX-and-Commit-Graph]], [[Repair-a-Corrupted-Git-Repository]]
- Next: [[Drill-Git-Automated-Bisect-on-Non-Linear-History]]
- Lab: `code/02-Version-Control/drill_packfile_rescue/`
