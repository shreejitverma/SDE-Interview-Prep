---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Recover Lost Git Work and Dangling Commits

> [!summary] Scope
> Owns emergency runbook procedures for rescuing lost source code, uncommitted changes, dropped stashes, and detached HEAD commits.
> Reaches Use and Debug rungs by providing a verified decision tree for scanning reflogs, walking dangling blobs with git fsck, inspecting rebase abort artifacts, and leveraging Jujutsu operation logs.
> Focuses strictly on data recovery, leaving repository structural corruption to the corruption playbook.

## Related

- Prerequisites: [[Git-Refs-Reflog-and-Reftable]], [[Git-Object-Model-and-Content-Addressing]]
- Next: [[Repair-a-Corrupted-Git-Repository]]
- Lab: `code/02-Version-Control/lost_work_recovery/`
