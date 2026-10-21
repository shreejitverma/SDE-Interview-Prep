---
type: moc
track: [cloud-ai, ai-eng, sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 01 - Cloud Architecture Principles

Volume 01 - Cloud Architecture Principles in the Cloud AI Engineering Zero to Godhood curriculum.

## Chapters

1. [Well-Architected Frameworks](Chapter_01_Well_Architected_Frameworks.md) - AWS Well-Architected versus Google Cloud Architecture Framework design pillars, non-negotiable architectural tenets, and review cadence.
2. [Regions, Zones, and Static Stability](Chapter_02_Regions_Zones_and_Static_Stability.md) - Availability zone independence, static stability mechanics, eliminating cross-zone runtime dependencies, and circular dependency prevention.
3. [Cell-Based Architecture and Shuffle Sharding](Chapter_03_Cell_Based_Architecture_and_Shuffle_Sharding.md) - Cell boundaries, partition keys, combinatoric shuffle sharding mathematics, and containing blast radius during poison-pill outages.
4. [Control Plane vs Data Plane Separation](Chapter_04_Control_Plane_vs_Data_Plane_Separation.md) - Strict isolation between control and data planes, asynchronous out-of-band updates, and graceful read-only data plane degradation.
5. [Constant Work and Workload Smoothing](Chapter_05_Constant_Work_and_Workload_Smoothing.md) - Constant-time execution patterns, eliminating load-proportional work, pre-allocating buffer pools, and preventing thundering herds.
6. [Multi-Account and Multi-Project Landing Zones](Chapter_06_Multi_Account_and_Multi_Project_Landing_Zones.md) - AWS Organizations, Control Tower, GCP Resource Hierarchy, centralized log routing, identity federation, and blast radius isolation.
