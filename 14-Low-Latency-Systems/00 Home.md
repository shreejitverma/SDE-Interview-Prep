---
tags: [trading/master, type/moc]
aliases: [Home, Index, Master MOC]
status: evergreen
module: 00
created: 2026-08-22
---

# Master Index — Low-Latency Trading Systems & Exchange Architecture

Welcome to the vault. This repository is a production-grade, hardware-first knowledge base and operational curriculum for ultra-low-latency electronic trading infrastructure, exchange mechanics, and high-frequency execution pipelines.

Every note is grounded in the hardware-software boundary: nanosecond latency budgets, cache coherence protocols, kernel-bypass networking, lock-free concurrency, and FPGA acceleration.

---

## Curriculum Modules

| Module Directory | Primary Scope & Hardware Boundary |
| :--- | :--- |
| **[[01 - Market & Microstructure Fundamentals/MOC - 01 Market & Microstructure Fundamentals\|01 - Market & Microstructure Fundamentals]]** | Order lifecycles, SIP vs direct feeds, Reg NMS / MiFID II, CDA vs discrete cross auctions. |
| **[[02 - Exchange Architecture/MOC - 02 Exchange Architecture\|02 - Exchange Architecture]]** | Gateways, pre-trade risk, deterministic sequencers, matching engines, drop copy, multicast feeds. |
| **[[03 - Matching Engine Internals/MOC - 03 Matching Engine Internals\|03 - Matching Engine Internals]]** | LOB memory layout (intrusive lists vs arrays), matching algorithms, SMP, deterministic state replication. |
| **[[04 - Hardware Mechanical Sympathy/MOC - 04 Hardware Mechanical Sympathy\|04 - Hardware Mechanical Sympathy]]** | CPU pipelines, cache hierarchies (L1–L3), MESI/MOESI, NUMA, TLB/HugePages, PCIe Gen5, branch predictors. |
| **[[05 - OS & Kernel Tuning/MOC - 05 OS & Kernel Tuning\|05 - OS & Kernel Tuning]]** | `isolcpus`, `nohz_full`, `rcu_nocbs`, IRQ affinity, memory pinning (`mlockall`), C/P-state jitter elimination. |
| **[[06 - Networking/MOC - 06 Networking\|06 - Networking]]** | Kernel bypass (Solarflare Onload/ef_vi, DPDK), UDP multicast A/B arbitration, PTP, cut-through L1/L2 switches. |
| **[[07 - Time & Measurement/MOC - 07 Time & Measurement\|07 - Time & Measurement]]** | Hardware timestamping, PTP/White Rabbit, `rdtsc` profiling, HDR Histogram, Coordinated Omission. |
| **[[08 - Low-Latency Programming/MOC - 08 Low-Latency Programming\|08 - Low-Latency Programming]]** | C++20/23 memory model, atomics, SPSC/MPMC lock-free rings, allocation-free loops, branchless idioms. |
| **[[09 - Messaging & IPC/MOC - 09 Messaging & IPC\|09 - Messaging & IPC]]** | Lock-free SHM, Disruptor pattern, Aeron / Aeron Cluster, zero-copy single-writer ring buffers. |
| **[[10 - Protocols & Codecs/MOC - 10 Protocols & Codecs\|10 - Protocols & Codecs]]** | Fast binary framing: ITCH 5.0, OUCH 4.2, CME MDP3 (SBE), FIX/FAST, zero-copy SIMD parsing. |
| **[[11 - Participant-Side Systems/MOC - 11 Participant-Side Systems\|11 - Participant-Side Systems]]** | HFT tick-to-trade pipelines, feed handlers, book reconstructors, signal processors, hardware risk gates. |
| **[[12 - FPGAs & Hardware Acceleration/MOC - 12 FPGAs & Hardware Acceleration\|12 - FPGAs & Hardware Acceleration]]** | RTL vs HLS, SmartNICs, FPGA feed parsing, wire-speed risk filtering, hybrid PCIe DMA pipelines. |
| **[[13 - Reliability, Ops & Testing/MOC - 13 Reliability, Ops & Testing\|13 - Reliability, Ops & Testing]]** | Deterministic replay, PCAP packet injection testing, latency regression CI, failover models. |
| **[[14 - Industry Map & Canon/MOC - 14 Industry Map & Canon\|14 - Industry Map & Canon]]** | Canonical papers, talks, proprietary boundaries, interview technical bars, firm taxonomy. |

---

## Master References & Roadmap
- **[[Roadmap - 12-Week Production Calibration\|12-Week Production Calibration Roadmap]]** — Structured execution plan for senior/principal readiness.
- **[[04 - Hardware Mechanical Sympathy/Latency Numbers Every Trading Engineer Knows\|Latency Numbers Every Trading Engineer Knows]]** — Physical limits from CPU registers to trans-oceanic glass.

---

## Vault Design & Operational Doctrine
1. **Hardware-Up Verification**: We do not trust abstractions until we inspect the generated assembly and hardware counters (`perf`, VTune).
2. **Deterministic Reproducibility**: If a state machine cannot be replayed from a sequenced packet log byte-for-byte, it is broken.
3. **No Hidden Allocations**: Zero heap allocation in steady-state tick processing.
4. **Tail Latency Obsession**: $p99.99$ and max jitter matter more than mean latency.
