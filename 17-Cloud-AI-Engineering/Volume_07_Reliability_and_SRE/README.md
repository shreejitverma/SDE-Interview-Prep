---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 07 - Reliability and Site Reliability Engineering

Volume 07 - Reliability and Site Reliability Engineering in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [SLIs, SLOs, and Error Budget Engineering](Chapter_01_SLIs_SLOs_and_Error_Budget_Engineering.md) - Quantitative SLI definition, multi-window multi-burn-rate alerting, error budget exhaustion policies, and freeze rules.
2. [Load Shedding, Backpressure, and Admission Control](Chapter_02_Load_Shedding_Backpressure_and_Admission_Control.md) - In-flight concurrency limits, token bucket rate limiters, CoDel queue management, and priority-based shedding.
3. [Resilient Communication: Retries, Jitter, and Circuit Breakers](Chapter_03_Resilient_Communication_Retries_Jitter_and_Circuit_Breakers.md) - Exponential backoff mathematics, full jitter vs decorrelated jitter, circuit breaker state transitions, and deadline propagation.
4. [Disaster Recovery Tiers and RTO/RPO Engineering](Chapter_04_Disaster_Recovery_Tiers_and_RTO_RPO_Engineering.md) - Backup/restore, pilot light, warm standby, and active-active architectures with mathematical RTO and RPO modeling.
5. [Chaos Engineering and Game Days](Chapter_05_Chaos_Engineering_and_Game_Days.md) - Fault Injection Simulator, network partition injection, zonal degradation drills, and production game day design.
6. [Incident Response, Blameless Postmortems, and RCA](Chapter_06_Incident_Response_Blameless_Postmortems_and_RCA.md) - Incident command hierarchy, chronological timeline assembly, cognitive bias elimination, and root cause mitigation tracking.
7. [Anatomy of Major Cloud Outages](Chapter_07_Anatomy_of_Major_Cloud_Outages.md) - Dissecting historical AWS and Google Cloud post-incident reports, cascaded failure loops, and lessons for platform designers.

## See also in Developer Toolchain

- [[Production-Incident-Simulation-Drills]]
