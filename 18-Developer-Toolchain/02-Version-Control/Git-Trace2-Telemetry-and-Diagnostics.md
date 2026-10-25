---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Git Trace2 Telemetry and Diagnostics

> [!summary] Scope
> Owns structured performance instrumentation, diagnostic logging, and fleet-wide telemetry inside Git.
> Reaches Explain and Decide at Scale rungs by examining the GIT_TRACE2, GIT_TRACE2_PERF, and GIT_TRACE2_EVENT streams, Unix domain socket and named pipe telemetry sinks, process nesting hierarchies, child execution metrics, and git diagnose health bundles.
> Leaves developer productivity metrics synthesis and DORA dashboards to the engineering systems at scale pillar.

## Related

- Prerequisites: [[Git-Index-Stat-Cache-and-Fsmonitor]]
- Next: [[Git-Expert-Config]]
- See also: [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/Log-Triage-Pipelines|Log Triage Pipelines]]
- Lab: `code/02-Version-Control/trace2_collector/`
