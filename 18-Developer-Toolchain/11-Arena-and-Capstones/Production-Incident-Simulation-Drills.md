---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Production Incident Simulation Drills

> [!summary] Scope
> This drill bank provides six timed, highly realistic production incident drills that simulate catastrophic toolchain and infrastructure failures under emergency conditions.
> Scenarios cover corrupted build cache poisoning, silent shared-library ABI drift causing SIGSEGV in production, compromised CI dependency exfiltration, and hung asynchronous runtimes.
> Each drill features runnable reproduction setups in code/, strict time constraints, progressive diagnostic reveals, and verified recovery procedures.

## Related

- Prerequisites: [[Cross-Pillar-Debugging-Scenario-Bank]], [[Diagnose-Dynamic-Linker-and-ABI-Failures]], [[Respond-to-a-Compromised-Transitive-Dependency]]
- Next: [[Capstone-Mini-Git-in-Rust]]
- See also: [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing|Deterministic Replay and Packet Injection Testing]], [[17-Cloud-AI-Engineering/Volume_07_Reliability_and_SRE/README|README]]
- Lab: `code/11-Arena-and-Capstones/incident_sim_drills/` (requires: linux)
