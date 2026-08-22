---
tags: [trading/networking, type/moc]
aliases: [Networking MOC, Kernel Bypass MOC]
status: seed
module: 06
created: 2026-08-22
---

# MOC — 06 Networking

Kernel bypass technologies, UDP multicast market data feeds, TCP low-latency stacks, network hardware, and physical topologies.

---

## Core Concepts
- [[Notes/Network Interface Card Architecture]] — RX/TX rings, DMA engines, packet descriptors, ring buffer overflow, PCIe bottlenecks.
- [[Notes/Kernel Bypass Technologies Overview]] — Solarflare OpenOnload, Solarflare `ef_vi`, DPDK, AF_XDP comparison matrix.
- [[Notes/Solarflare ef_vi Zero-Copy API]] — Direct descriptor ring access, hardware filters, zero-copy packet processing.
- [[Notes/DPDK Architecture for Trading]] — Poll Mode Drivers (PMD), HugePages, memory pools (`rte_mempool`), core affinity.
- [[Notes/UDP Multicast Market Data and A-B Feed Arbitration]] — Sequence gap detection, packet reordering, zero-loss feed arbitration.
- [[Notes/Low-Latency TCP for Order Entry]] — TCP_NODELAY, TCP_QUICKACK, custom user-space TCP implementations, socket buffer tuning.
- [[Notes/Switch Architectures in Trading]] — Store-and-Forward vs Cut-Through switches, Layer-1 matrix switches (Metamako/Arista 7130).
- [[Notes/Colocation and Physical Layer Infrastructure]] — Optical cross-connects, fiber propagation speed ($5\text{ ns/m}$), glass vs air (microwave/millimeter wave).

## Labs & Implementations
- [[Labs/Lab - 06 Zero-Loss Multicast Feed Arbitrator]] — Build an A/B multicast gap detection and arbitration engine using DPDK or `ef_vi` mock streams.

## Drills & War Stories
- [[Drills/Drill - 06 Multicast Packet Drop Diagnostics]] — Troubleshoot dropped packets during bursty market open sequences.
- [[Notes/War Story - The Microburst Buffer Overflow]] — How switch egress buffer exhaustion dropped 50% of orders during an NFP release.

## Canonical Sources
- [[Sources/Solarflare ef_vi User Guide]] — Low-level interface documentation for direct hardware packet injection.
- [[Sources/DPDK Programmer's Guide]] — Core architecture manual for DPDK polling drivers.
