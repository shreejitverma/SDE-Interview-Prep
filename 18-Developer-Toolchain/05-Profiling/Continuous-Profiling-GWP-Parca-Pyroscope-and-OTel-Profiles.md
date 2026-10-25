---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Continuous Profiling: GWP, Parca, Pyroscope, and OTel Profiles

> [!summary] Scope
> This note owns fleet-wide, always-on continuous profiling infrastructure across distributed systems.
> It reaches the Explain and Decide rungs by exploring Google-Wide Profiling (GWP), Parca's in-kernel eBPF unwinding from .eh_frame (DWARF CFI) data, Grafana Pyroscope, and the OpenTelemetry Profiles protocol.
> It demonstrates how to correlate continuous CPU and off-CPU profiles directly with distributed trace IDs.

## Related

- Prerequisites: none
- Next: [[Profiling-Decision-Records]]
- See also: [[04-System-Design/00-Concepts/SLOs-and-Observability|SLOs and Observability]]
