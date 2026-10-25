---
type: case-study
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Case Study: Knight Capital Deployment Failure and Reused Feature Flags

> [!summary] Scope
> Analysis of the August 1, 2012 Knight Capital $460 million trading disaster documented in SEC Release No. 34-70694.
> Details how manual server-by-server deployment left one server running legacy code where a repurposed feature flag inadvertently triggered defunct Power Peg order code in production.
> Extracts rigorous release engineering rules for automated atomic fleet deployment, configuration immutability, and dead-flag decommissioning.
> Links the operational breakdown to foundational exchange architecture and automated risk controls.

## Related

- Prerequisites: [[Progressive-Delivery-Rollbacks-and-Feature-Flags]]
- Next: [[Case-Study-Left-Pad-and-Registry-Immutability]]
- See also: [[14-Low-Latency-Systems/02 - Exchange Architecture/War Story - The 2012 Knight Capital Disaster|War Story   The 2012 Knight Capital Disaster]]
