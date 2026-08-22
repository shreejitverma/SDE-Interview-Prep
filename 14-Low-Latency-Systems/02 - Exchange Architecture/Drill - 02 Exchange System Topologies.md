---
tags: [trading/exchange-arch, type/drill]
aliases: [Drill 02, Exchange System Design Drill, Architecture Drill]
status: evergreen
module: 02
created: 2026-08-22
---

# Drill 02 — Exchange System Design & Infrastructure Topologies

> [!summary]
> Principal-level systems design drill calibrating your exchange architecture intuition. You are tasked with architecting a modern electronic exchange matching engine capable of sustaining **2,000,000 orders/second** with a **sub-5-microsecond $p99.99$ order-to-ack latency**. Attempt each design section before unfolding the solution.

---

### Challenge 1: Thread-to-Core Pinning & NUMA Memory Topology
**Prompt**:
Design the complete CPU core allocation and memory topology for a dual-socket 64-core server (AMD EPYC or Intel Sapphire Rapids) hosting the primary matching partition for a single liquid instrument (e.g. SPY equity or E-mini futures).

**Requirements**:
1. Assign exact roles to isolated CPU cores (Line handlers, Risk, Sequencer, Matching Engine, Market Data Publisher, Drop Copy).
2. Detail NUMA node memory allocation policy to guarantee zero cross-socket UPI interconnect traffic.

> [!question]- Unfold Solution
> 1. **Core Allocation Map (Socket 0 - PCIe Direct Connected)**:
>    - **Core 0 (OS Isolation Core)**: Linux OS kernel threads, SSH, monitoring daemons (`systemd`).
>    - **Cores 2, 4, 6, 8 (Gateway Line Handlers)**: 4 kernel-bypass worker threads polling Solarflare `ef_vi` RX rings.
>    - **Core 10 (Inline Pre-Trade Risk)**: Dedicated risk evaluator (or embedded directly within line handlers).
>    - **Core 12 (Central Sequencer)**: Single-writer monotonic sequence generator and journal committer.
>    - **Core 14 (Matching Engine Core)**: Isolated single-writer Price-Time Priority matching engine.
>    - **Core 16 (Market Data Publisher)**: ITCH / SBE binary formatter and UDP multicast TX line handler.
>    - **Core 18 (Drop Copy & Clearing)**: Out-of-band FIX ExecutionReport dispatcher.
> 2. **NUMA Memory Allocation Doctrine**:
>    - All memory-mapped ring buffers (`/dev/shm`), object pools, and HugePage buffers (`2MB pages`) must be allocated exclusively on **NUMA Node 0** (the node directly attached to the PCIe root complex of the 25G Solarflare NIC).
>    - Processes are launched with `numactl --cpunodebind=0 --membind=0` to physically prevent memory pages from allocating on Socket 1, completely eliminating cross-socket interconnect traversal (**saving 80–140 ns per memory access**).

---

### Challenge 2: Wire-to-Wire Order Ingress & Egress Pipeline
**Prompt**:
Trace the exact nanosecond timeline of an inbound aggressive BUY order from the moment photons hit the exchange's SFP28 optical transceiver to the moment the trade execution packet exits onto the public multicast wire.

**Requirements**:
Break down the sub-microsecond latency budget across all 6 physical and software stages.

> [!question]- Unfold Solution
> **End-to-End Latency Budget (Target: <2,500 ns Median / <5,000 ns $p99.99$)**:
>
> 1. **Stage 1 — Ingress PHY / MAC & Kernel Bypass RX (~550 ns)**:
>    - Optical signal enters SFP28 transceiver $\to$ NIC PCS/PMA SerDes $\to$ DMA push to host RX ring $\to$ Gateway thread detects new frame via `ef_vi` poll.
> 2. **Stage 2 — Gateway Framing & Session Check (~35 ns)**:
>    - Gateway validates binary OUCH header $\to$ checks monotonic client sequence $\to$ extracts order parameters using single-cycle `BSWAP`.
> 3. **Stage 3 — Pre-Trade Risk Gate (~20 ns)**:
>    - Validates fat-finger quantity $\to$ evaluates price collar against reference price $\to$ updates atomic gross credit counter.
> 4. **Stage 4 — Total-Order Sequencer & Journal Append (~25 ns)**:
>    - Dequeues from MPSC ingress ring $\to$ stamps 64-bit sequence number $\to$ copies zero-copy into `mmap` NVMe journal.
> 5. **Stage 5 — Matching Engine Execution (~25 ns)**:
>    - Sweeps top-of-book ask level $\to$ unlinks matched maker orders $\to$ updates level depth $\to$ emits internal trade record to egress SHM ring.
> 6. **Stage 6 — Market Data Formatting & Egress TX (~350 ns)**:
>    - Publisher thread formats ITCH 5.0 'E' (Order Executed) message $\to$ pushes frame to Solarflare TX DMA ring $\to$ NIC PHY serializes to optical wire.
>
> **Total Wire-to-Wire Latency: $\approx 1,005\text{ nanoseconds}$ (1.005 µs).**

---

### Challenge 3: Disaster Recovery & Active-Active Failover
**Prompt**:
Design a fault-tolerant failover mechanism between Primary Server A and Secondary Server B that guarantees **zero lost orders, zero duplicated execution reports, and sub-10-microsecond recovery time** during a sudden primary motherboard failure.

**Questions**:
1. How is the inbound event stream replicated between Server A and Server B?
2. How does the egress network switch prevent duplicate packets during normal operation?
3. How is the failover triggered and what is the exact hardware takeover sequence?

> [!question]- Unfold Solution
> 1. **Stream Replication via Optical Splitting**:
>    - Inbound optical fiber cables from all client gateways pass through passive optical splitters (50/50 split).
>    - Primary Server A and Secondary Server B receive identical physical Ethernet frames simultaneously.
>    - Both servers run deterministic single-writer Replicated State Machines (RSMs) that process the identical sequence of orders in lockstep.
> 2. **Duplicate Suppression via Layer-1 Matrix Switch**:
>    - Both Server A and Server B emit identical market data and execution packets to an egress Layer-1 switch (e.g. Arista 7130 FPGA switch).
>    - The FPGA switch is configured to pass Port A (Primary) to the live network while keeping Port B (Secondary) in **silent/muted state**.
> 3. **Hardware Failover Execution**:
>    - Primary Server A transmits a continuous 10-nanosecond heartbeat pulse to the FPGA switch.
>    - If Server A suffers a motherboard failure or kernel panic, the heartbeat signal ceases.
>    - Within **<1 microsecond**, the FPGA switch detects heartbeat loss and automatically un-mutes Port B, forwarding Secondary Server B's live packet stream to the network with **zero dropped orders and zero state desynchronization**.

---

## Related
- [[02 - Exchange Architecture/Exchange Gateway Architecture]]
- [[02 - Exchange Architecture/Pre-Trade Risk Checks at Wire Speed]]
- [[02 - Exchange Architecture/Replicated State Machine Pattern in Exchanges]]
- [[02 - Exchange Architecture/Fairness and Determinism Metrics]]
- [[02 - Exchange Architecture/MOC - 02 Exchange Architecture]]
