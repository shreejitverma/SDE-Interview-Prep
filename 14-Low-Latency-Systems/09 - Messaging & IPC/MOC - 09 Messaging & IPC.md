---
tags: [trading/ipc-messaging, type/moc]
aliases: [Messaging IPC MOC, Disruptor Aeron MOC]
status: seed
module: 09
created: 2026-08-22
---

# MOC — 09 Messaging & IPC

High-throughput, nanosecond inter-process communication: shared memory rings, the Disruptor pattern, Aeron, and sequenced logs.

---

## Core Concepts
- [[Notes/Shared Memory IPC Topologies]] — POSIX SHM (`shm_open`, `mmap`), hugepage-backed SHM, page fault prevention, cache line layout.
- [[Notes/The LMAX Disruptor Architecture]] — Ring buffer, sequence barriers, multi-consumer dependency graphs, cache line padding.
- [[Notes/Aeron Messaging Transport]] — Driver architecture, lock-free IPC, media drivers, UDP unicast/multicast reliable transport.
- [[Notes/Aeron Cluster and Replicated State]] — Raft-like consensus, deterministic state machine sequencing, zero-copy log archiving.
- [[Notes/Backpressure Strategies in High-Throughput Pipelines]] — Dropping vs buffering vs back-propagating, ring buffer saturation handling.
- [[Notes/Zero-Copy Fan-Out Patterns]] — Single-writer multi-reader shared memory buses, core-to-core broadcast.

## Labs & Implementations
- [[Labs/Lab - 09 Ultra-Fast Shared Memory IPC Channel]] — Construct a sub-50ns bidirectional SHM transport between two pinned CPU cores.

## Drills & War Stories
- [[Drills/Drill - 09 Designing an IPC Pipeline for Tick Ingestion]] — Design the messaging fabric connecting feed handlers to the pricing engine.
- [[Notes/War Story - Unbounded Queues and the Out-of-Memory Cascade]] — How a downstream pricing engine backup caused memory exhaustion across the trading host.

## Canonical Sources
- [[Sources/Aeron Open-Source Repository and Wiki by Real Logic]] — Reference implementation of ultra-high-performance messaging.
- [[Sources/The LMAX Disruptor Technical Paper]] — High-performance alternative to bounded queues for concurrent programming.
