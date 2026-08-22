---
tags: [trading/reliability-ops, type/moc]
aliases: [Reliability and Ops MOC, Exchange Testing MOC]
status: seed
module: 13
created: 2026-08-22
---

# MOC — 13 Reliability, Ops & Testing

Operational excellence, deterministic simulation, PCAP replay testing, latency regression CI, and zero-downtime exchange infrastructure.

---

## Core Concepts
- [[Notes/Deterministic Replay and Packet Injection Testing]] — Byte-for-byte state reproduction using hardware-timestamped PCAP traces.
- [[Notes/Exchange Simulators and Conformance Harnesses]] — Building deterministic exchange mock engines for gateway certification.
- [[Notes/Latency Regression Testing in CI-CD]] — Automated bare-metal performance testing, detecting 10ns regressions in continuous integration.
- [[Notes/Observability Without Perturbation]] — Zero-overhead monitoring, circular trace buffers, optical network taps, hardware counter sampling.
- [[Notes/Automated Kill Switches and Risk Circuit Breakers]] — Multi-tier kill switches: hardware link drop, software gateway cutoff, exchange cancel-on-disconnect.
- [[Notes/Disaster Recovery and Failover Protocols]] — Active-passive synchronization, state checkpointing, cold/warm/hot site transitions.

## Labs & Implementations
- [[Labs/Lab - 13 Deterministic Exchange Replay and Test Suite]] — Build an offline replay tool validating matching engine output against historical exchange pcaps.

## Drills & War Stories
- [[Drills/Drill - 13 Post-Mortem of a Production Outage]] — Conduct a rigorous root-cause analysis on a production failover split-brain event.
- [[Notes/War Story - The 2012 BATS IPO Glitch]] — Software bug in BATS' own matching engine crashing its own initial public offering.

## Canonical Sources
- [[Sources/Site Reliability Engineering at Scale for Financial Systems]] — High-availability principles for non-stop trading environments.
