---
tags: [trading/time-measurement, type/moc]
aliases: [Time Measurement MOC, Microbenchmarking MOC]
status: evergreen
module: 07
created: 2026-08-22
---

# MOC — 07 Time & Measurement

High-precision time synchronization, hardware timestamping, cycle-accurate profiling, and latency distribution measurement.

```mermaid
flowchart LR
    GPS[GPS / Rubidium Reference] --> PTP[PTP IEEE 1588 / White Rabbit Grandmaster]
    PTP --> NIC[NIC Hardware PHY/MAC Timestamping]
    NIC --> RDTSC[CPU Invariant RDTSC Cycle Profiler]
    RDTSC --> HDR[HDR Histogram Logarithmic Percentiles]
```

---

## Core Concepts
- [[07 - Time & Measurement/Clock Sources and Hardware Timestamping]] — Ingress/Egress PHY, MAC, and DMA hardware timestamping; NIC oscillator stability.
- [[07 - Time & Measurement/Precision Time Protocol and White Rabbit]] — IEEE 1588v2, boundary/transparent clocks, sub-nanosecond White Rabbit synchronization.
- [[07 - Time & Measurement/CPU Timestamp Counter RDTSC Mechanics]] — `rdtsc`, `rdtscp`, memory barriers (`lfence`), invariant TSC, cycle-to-nanosecond calibration.
- [[07 - Time & Measurement/One-Way Latency vs Round-Trip Time Measurement]] — Asymmetric network paths, wire-to-wire vs wire-to-ack profiling.
- [[07 - Time & Measurement/Coordinated Omission in Low Latency Systems]] — Service time vs response time, burst distortions, synthetic load generator pitfalls.

## Labs & Implementations
- [[07 - Time & Measurement/Lab - 07 Cycle-Accurate RDTSC Profiler with HdrHistogram]] — Build a zero-overhead C++ instrumentation harness that exports HDR histograms with zero steady-state allocation.

## Drills & War Stories
- [[07 - Time & Measurement/War Story - The 2015 NYSE 3.5-Hour Gate Freeze]] — Deep-dive forensic breakdown of the July 8, 2015 NYSE market-wide freeze: gateway communication protocol mismatch, sequence verification rejections, and live reboot failure.

## Canonical Sources
- [[Sources/Intel 64 and IA-32 Architectures Software Developer's Manual]] — RDTSC/RDTSCP serialization mechanics.
- [[Sources/Systems Performance by Brendan Gregg]] — Microbenchmarking and performance profiling.
- [[Sources/Site Reliability Engineering at Scale for Financial Systems]] — High-availability operational standards.
