---
tags: [trading/matching-engine, type/moc]
aliases: [Matching Engine MOC, Order Book Internals MOC]
status: seed
module: 03
created: 2026-08-22
---

# MOC — 03 Matching Engine Internals

Data structures, allocation-free memory topologies, matching algorithms, and deterministic replay loops.

---

## Core Concepts
- [[Notes/Order Book Data Structures]] — Price-indexed arrays, flat-map B-trees, contiguous circular buffers, and intrusive double-linked lists.
- [[Notes/Matching Algorithms]] — Price-Time Priority (FIFO), Pro-Rata, Size-Time Priority, Split-Spread models.
- [[Notes/Self-Match Prevention Mechanisms]] — Cancel Oldest, Cancel Newest, Decrement and Cancel, price modification rules.
- [[Notes/Complex Order Types Execution]] — Hidden orders, Icebergs, Discretionary offsets, Stop-Loss triggers, Pegged orders.
- [[Notes/Deterministic Matching Engine State Recovery]] — Journal snapshotting, zero-allocation state replay, checksum verifications.
- [[Notes/Active-Active vs Active-Passive Failover]] — Lockstep dual-execution vs hot-standby ring replication.

## Labs & Implementations
- [[Labs/Lab - 03 High-Performance Intrusive LOB]] — Implement an allocation-free C++20 Limit Order Book achieving <20ns insertion/cancellation.

## Drills & War Stories
- [[Drills/Drill - 03 Order Book Memory Layout Optimization]] — Cache-line alignment, pointer chasing elimination, and benchmark validation.
- [[Notes/War Story - The Knight Capital Disaster]] — Analysis of dead code activation, deployment failure, and missing automated kill switches.

## Canonical Sources
- [[Sources/Building a Matching Engine in C++]] — Design patterns for ultra-low-latency deterministic execution cores.
