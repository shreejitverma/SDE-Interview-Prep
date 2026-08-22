---
tags: [trading/ipc-messaging, type/moc]
aliases: [Messaging IPC MOC, Disruptor Aeron MOC]
status: evergreen
module: 09
created: 2026-08-22
---

# MOC — 09 Messaging & IPC

High-throughput, nanosecond inter-process communication: shared memory rings, the Disruptor pattern, Aeron, and sequenced logs.

```mermaid
flowchart LR
    SHM[POSIX Shared Memory /dev/shm] --> DISRUPTOR[LMAX Disruptor Pattern]
    DISRUPTOR --> AERON[Aeron Term Buffer Rotation]
    AERON --> CLUSTER[Aeron Cluster RSM]
```

---

## Core Concepts
- [[09 - Messaging & IPC/Shared Memory IPC Topologies]] — POSIX SHM (`shm_open`, `mmap`), hugepage-backed SHM, page fault prevention, cache line layout.
- [[09 - Messaging & IPC/The LMAX Disruptor Architecture]] — Ring buffer, sequence barriers, multi-consumer dependency graphs, cache line padding.
- [[09 - Messaging & IPC/Aeron Messaging Transport]] — Driver architecture, lock-free IPC, media drivers, UDP unicast/multicast reliable transport.
- [[09 - Messaging & IPC/Aeron Protocol Deep Dive and IPC Architecture]] — Term buffer rotation, `tryClaim()` zero-copy publishing, flow control, sub-100ns IPC mechanics.

## Labs & Implementations
- [[09 - Messaging & IPC/Lab - 09 Ultra-Fast Shared Memory IPC Channel]] — Construct a sub-50ns bidirectional SHM transport between two pinned CPU cores.

## Canonical Sources
- [[Sources/How to Build an Exchange by Jane Street]] — Replicated state machines and message distribution.
- [[Sources/Systems Performance by Brendan Gregg]] — Memory architectures and IPC performance.
- [[Sources/C++ Concurrency in Action by Anthony Williams]] — Lock-free concurrency and memory orders.
