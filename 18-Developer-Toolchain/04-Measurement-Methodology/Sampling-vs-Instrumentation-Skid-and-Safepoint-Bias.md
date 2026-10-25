---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Sampling vs Instrumentation: Skid and Safepoint Bias

> [!summary] Scope
> This note owns the fundamental trade-offs between sampling profilers and code instrumentation.
> It covers the Explain and Debug depth rungs by analyzing PMU counter overflow interrupts, observer perturbation overhead, instruction skid, PEBS/IBS hardware precision, and runtime safepoint bias.
> It equips engineers to identify when measurement tools lie about instruction-level bottlenecks before deploying system profilers.

## Related

- Prerequisites: none
- Next: [[Benchmark-Methodology-Noise-and-Environment-Control]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation|Linux Tracing and Instrumentation]]
