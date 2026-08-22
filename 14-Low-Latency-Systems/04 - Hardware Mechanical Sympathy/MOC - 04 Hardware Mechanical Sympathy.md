---
tags: [trading/hardware, type/moc]
aliases: [Mechanical Sympathy MOC, CPU Architecture MOC]
status: seed
module: 04
created: 2026-08-22
---

# MOC — 04 Hardware Mechanical Sympathy

CPU microarchitecture, cache-line physics, memory hierarchy, branch prediction, and nanosecond accounting.

---

## Core Concepts
- [[Notes/Latency Numbers Every Trading Engineer Knows]] — Physical latency budgets across modern x86/ARM memory and bus hierarchies.
- [[Notes/CPU Cache Hierarchy and Line Alignment]] — L1i, L1d, L2, L3 topologies, cache line bouncing, MESI/MOESI cache coherence.
- [[Notes/False Sharing and Cache Contention]] — Cache line invalidations, padding strategies (`hardware_destructive_interference_size`).
- [[Notes/NUMA Topologies and Inter-Socket Jitter]] — UPI/QPI interconnect overhead, socket affinity, local vs remote DRAM allocations.
- [[Notes/TLB Architecture and Huge Pages]] — 4KB vs 2MB vs 1GB pages, TLB misses, zeroing overhead, explicit HugeTLBFS.
- [[Notes/Branch Predictors and Pipeline Stalls]] — Pattern history tables, BTB misses, branchless programming idioms, conditional moves (`CMOV`).
- [[Notes/Hardware Prefetchers and Memory Streaming]] — Stream, spatial, and L2 prefetchers; cache pollution and prefetch suppression.
- [[Notes/Instruction-Level Parallelism and SIMD]] — Super-scalar pipelines, out-of-order execution windows, AVX-512 frequency scaling downclocking.
- [[Notes/PCIe Architecture and DMA]] — PCIe Gen4/5/6 lane bandwidth, TLP overhead, Root Complex, BAR memory, Direct Memory Access.

## Labs & Implementations
- [[Labs/Lab - 04 Cache Line Contention and Latency Benchmark]] — Profile false sharing and measure memory access latencies across L1/L2/L3/DRAM using `rdtsc`.

## Drills & War Stories
- [[Drills/Drill - 04 Dissecting Cache and Branch Assembly]] — Analyze compiler output, eliminating pipeline flushes and branch mispredictions.
- [[Notes/War Story - Skylake AVX-512 Frequency Throttling]] — How high-width SIMD instructions dropped base CPU clocks across production clusters.

## Canonical Sources
- [[Sources/What Every Programmer Should Know About Memory by Ulrich Drepper]] — The bible of memory subsystems and microarchitecture.
- [[Sources/Mechanical Sympathy by Martin Thompson]] — Pioneering principles of writing software that aligns with hardware.
