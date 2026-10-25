---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Diagnose CI Merge Queue Throughput Collapse

> [!summary] Scope
> Owns a complex organizational performance troubleshooting drill where a 500-engineer monorepo experiences complete merge queue throughput collapse.
> Reaches the Debug and Decide at Scale rungs by presenting a mathematical simulation model where a 3% test flake rate combined with naive batching reduces pass probability to near zero.
> Tasks the engineer with analyzing queue latency graphs, deriving optimal speculative batch sizes, and configuring quarantine rules to restore throughput.

## Related

- Prerequisites: [[Build-and-Test-Latency-Economics-and-Flaky-Tests]], [[Playbook-Quarantine-and-Eliminate-Flaky-Tests]]
- Next: [[Drill-Engineering-Systems-Audit-a-Gamed-Productivity-Dashboard]]
- Lab: `code/09-Engineering-Systems-at-Scale/drill_merge_queue_collapse/`
