---
type: case-study
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: SolarWinds Orion SUNBURST Build System Infiltration

> [!summary] Scope
> Architectural review of the SolarWinds build pipeline compromise.
> Details how the Sunspot implant monitored background compiler processes to inject the SUNBURST backdoor into source files on disk immediately prior to compilation, resulting in legitimate cryptographic signing of backdoored binaries.
> Synthesizes requirements for isolated, ephemeral, and hermetic build environments.
> Analyzes why traditional code signing alone fails to prevent supply chain tampering.

## Related

- Prerequisites: [[SLSA-Framework-Build-and-Source-Tracks]], [[Reproducible-Builds-as-a-Security-Control]]
- Next: [[Case-Study-Event-Stream-and-Maintainer-Handoff]]
