---
tags: [trading/time-measurement, type/moc]
aliases: [Time Measurement MOC, Microbenchmarking MOC]
status: seed
module: 07
created: 2026-08-22
---

# MOC — 07 Time & Measurement

High-precision time synchronization, hardware timestamping, cycle-accurate profiling, and latency distribution measurement.

---

## Core Concepts
- [[Notes/Clock Sources and Hardware Timestamping]] — Ingress/Egress PHY, MAC, and DMA hardware timestamping; NIC oscillator stability.
- [[Notes/Precision Time Protocol and White Rabbit]] — IEEE 1588v2, boundary/transparent clocks, sub-nanosecond White Rabbit synchronization.
- [[Notes/CPU Timestamp Counter RDTSC Mechanics]] — `rdtsc`, `rdtscp`, memory barriers (`lfence`), invariant TSC, cycle-to-nanosecond calibration.
- [[Notes/One-Way Latency vs Round-Trip Time Measurement]] — Asymmetric network paths, wire-to-wire vs wire-to-ack profiling.
- [[Notes/Latency Histograms and High Dynamic Range Profiling]] — HdrHistogram, logarithmic binning, tracking sub-microsecond percentiles ($p50$ to $p99.9999$).
- [[Notes/Coordinated Omission in Low Latency Systems]] — Service time vs response time, burst distortions, synthetic load generator pitfalls.
- [[Notes/Benchmarking Pitfalls and Measurement Bias]] — Warmup phases, compiler optimization dead-code elimination, cache state poisoning.

## Labs & Implementations
- [[Labs/Lab - 07 Cycle-Accurate RDTSC Profiler with HdrHistogram]] — Build a zero-overhead C++ instrumentation harness that exports HDR histograms with zero steady-state allocation.

## Drills & War Stories
- [[Drills/Drill - 07 Detecting Coordinated Omission in Benchmarks]] — Analyze skewed latency profiles and fix flawed benchmarking suites.
- [[Notes/War Story - The Out-of-Order RDTSC Trap]] — How missing serializing instructions (`lfence`) resulted in negative execution latency reports.

## Canonical Sources
- [[Sources/How NOT to Measure Latency by Gil Tene]] — Foundational talk on Coordinated Omission and percentile distortions.
- [[Sources/IEEE 1588-2019 Standard for Precision Clock Synchronization]] — The official PTP specification.
