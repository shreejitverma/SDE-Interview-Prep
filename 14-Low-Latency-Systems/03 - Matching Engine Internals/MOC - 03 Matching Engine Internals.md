---
tags: [trading/matching-engine, type/moc]
aliases: [Matching Engine MOC, Order Book Internals MOC]
status: evergreen
module: 03
created: 2026-08-22
---

# MOC — 03 Matching Engine Internals

Data structures, allocation-free memory topologies, matching algorithms, and deterministic replay loops.

```mermaid
flowchart LR
    ORDER[Inbound Sequenced Order] --> LOB[Intrusive Double-Linked LOB]
    LOB --> ALGO[Matching Algorithm: FIFO / Pro-Rata]
    ALGO --> SMP[Self-Match Prevention Gate]
    SMP --> EXEC[Execution Report Generator]
    EXEC --> SNAP[Deterministic Snapshot Journal]
```

---

## Core Concepts
- [[03 - Matching Engine Internals/Order Book Data Structures]] — Price-indexed flat arrays, intrusive doubly-linked lists, bitmask level scans, eliminating pointer chasing.
- [[03 - Matching Engine Internals/Matching Algorithms]] — Price-Time Priority (FIFO), Pro-Rata allocation, Size-Time Priority, Split-Spread matching.
- [[03 - Matching Engine Internals/Self-Match Prevention Mechanisms]] — Cancel Oldest (CO), Cancel Newest (CN), Decrement and Cancel (DC), regulatory cross-wash prevention.
- [[03 - Matching Engine Internals/Deterministic Matching Engine State Recovery]] — Journal snapshotting, zero-allocation state replay, checksum verifications.

## Labs & Implementations
- [[03 - Matching Engine Internals/Lab - 03 High-Performance Intrusive LOB]] — Implement an allocation-free C++20 Limit Order Book achieving <20ns insertion/cancellation.

## Drills & War Stories
- [[03 - Matching Engine Internals/War Story - The 2013 NASDAQ SIP Outage]] — Deep-dive forensic breakdown of the August 22, 2013 3-Hour Tape C freeze: NYSE Arca reconnect surge, unbounded queue memory exhaustion, and cascading failover collapse.

## Canonical Sources
- [[Sources/How to Build an Exchange by Jane Street]] — Foundational architecture of modern deterministic financial venues.
- [[Sources/What Every Programmer Should Know About Memory by Ulrich Drepper]] — Cache hierarchies and memory alignment.
- [[Sources/Site Reliability Engineering at Scale for Financial Systems]] — High availability and zero-loss operations.
