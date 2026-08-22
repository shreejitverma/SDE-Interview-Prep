---
tags: [trading/exchange-arch, type/moc]
aliases: [Exchange Architecture MOC]
status: seed
module: 02
created: 2026-08-22
---

# MOC — 02 Exchange Architecture

End-to-end design of tier-1 electronic matching venues: deterministic event sourcing, zero-loss sequencers, low-jitter gateways, and multi-cast distribution.

```mermaid
flowchart LR
    Participant([Client Gateway]) -->|TCP / TLS| GW[Order Gateway]
    GW -->|Pre-Trade Risk| PR[Risk Gate]
    PR -->|Unicast| SEQ[Sequencer / Journal]
    SEQ -->|Sequenced Ring / Multicast| ME[Matching Engine Core]
    ME -->|Execution Events| PUB[Market Data Publisher]
    ME -->|Drop Copy / Acks| GW
    PUB -->|UDP Multicast A/B| MDP[ITCH / MDP3 Feed]
```

---

## Core Concepts
- [[Notes/Exchange Gateway Architecture]] — Line handlers, session management, protocol transcoders, TCP terminator offload.
- [[Notes/Pre-Trade Risk Checks at Wire Speed]] — Credit limits, price collars, fat-finger checks, leaky-bucket throttles at line rate.
- [[Notes/The Sequenced-Stream Architecture]] — Total order broadcasting, hardware sequencers, deterministic log replication.
- [[Notes/Replicated State Machine Pattern in Exchanges]] — Raft vs. Paxos vs. single-sequencer architectures for nanosecond failover.
- [[Notes/Market Data Publisher Architecture]] — Multicast line handlers, snapshot/incremental generators, packet pacing.
- [[Notes/Drop Copy and Clearing Feeds]] — Asynchronous execution broadcast, guaranteed delivery mechanisms, risk clearing pipelines.
- [[Notes/Fairness and Determinism Metrics]] — Tail latency bounds, strict FIFO ingestion, matching engine jitter envelopes.

## Labs & Implementations
- [[Labs/Lab - 02 Sequenced Event Log Engine]] — Build a lock-free, memory-mapped deterministic sequencer with zero-copy persistence.

## Drills & War Stories
- [[Drills/Drill - 02 Exchange System Topologies]] — System design interview: design an exchange sustaining 2M orders/sec with <5µs p99.9 latency.
- [[Notes/War Story - LMAX Disruptor and the Death of Queues]] — How replacing actor-based queue locks with ring buffers redefined financial messaging.

## Canonical Sources
- [[Sources/How to Build an Exchange by Jane Street]] — Foundational architecture of modern deterministic financial venues.
- [[Sources/The LMAX Architecture by Martin Fowler]] — The single-writer, lock-free memory architecture.
