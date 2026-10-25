---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Playbook: Diagnose Bazel and Buck2 Remote Cache Misses

> [!summary] Scope
> Owns the diagnostic runbook for isolating, root-causing, and resolving non-deterministic action cache misses in Bazel and Buck2 monorepos.
> Reaches Debug and Decide at Scale rungs by guiding engineers through compact execution logs, action graph queries, action inspection, and binary diffing with diffoscope.
> Provides remediation workflows for non-deterministic code generators, leaked absolute paths, and volatile environment variables.

## Related

- Prerequisites: [[Remote-Execution-API-and-Remote-Caching]], [[Bazel-Skyframe-Bzlmod-and-Execution-Model]]
- Next: [[Migrate-a-Polyglot-Codebase-to-Hermetic-Builds]]
- Lab: `code/06-Build-Systems/cache_miss_diagnosis/` (requires: network)
