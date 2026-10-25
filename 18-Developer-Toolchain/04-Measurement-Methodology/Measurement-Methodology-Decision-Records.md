---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Measurement Methodology Decision Records

> [!summary] Scope
> This playbook owns architectural decision records for measurement strategies across software engineering organizations.
> It evaluates sampling versus instrumentation, wall-clock timing versus deterministic instruction-count CI gates, and frame pointer unwinding versus DWARF/LBR/SFrame.
> It articulates corporate defaults, condition triggers that flip them, and operational trade-offs for fleet-wide measurement.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]], [[Performance-Regression-Detection-in-CI]]
- Next: [[Design-a-Low-Noise-Benchmark-Harness-Playbook]]
