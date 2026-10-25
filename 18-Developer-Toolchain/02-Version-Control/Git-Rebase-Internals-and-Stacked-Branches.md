---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Rebase Internals and Stacked Branches

> [!summary] Scope
> Owns linear history rewriting state machines, patch replay, and multi-branch dependency management.
> Reaches Use, Explain, and Debug rungs by analyzing the .git/rebase-merge sequencer engine, interactive command scripting, stacked branch management with --update-refs, experimental git history subcommands, cherry-pick, and revert.
> Sets the boundary with server-side worktree-free rewriting and disaster recovery playbooks.

## Related

- Prerequisites: [[Git-Three-Way-Merge-ORT-and-Diff-Algorithms]]
- Next: [[Git-Replay-and-Server-Side-History-Rewriting]]
