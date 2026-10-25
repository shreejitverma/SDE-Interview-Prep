---
type: moc
track: [sde, distinguished, low-latency, quant-dev]
level:
status: draft
last_reviewed:
sources:
  - "Brendan Gregg, Systems Performance: Enterprise and the Cloud, 2nd Edition, Addison-Wesley, 2020, https://www.brendangregg.com/systems-performance-2nd-edition-book.html"
  - "Intel Corporation, Intel VTune Profiler User Guide: Top-Down Microarchitecture Analysis Method, Intel Corporation, 2026, https://www.intel.com/content/www/us/en/docs/vtune-profiler/cookbook/2025-0/top-down-microarchitecture-analysis-method.html"
  - "Linux Kernel Organization, Linux perf Man Pages and Wiki (Linux 6.x), Linux Kernel Organization, 2026, https://perfwiki.github.io/"
  - "Google LLC, Google Benchmark User Guide (v1.10.0), Google LLC, 2026, https://github.com/google/benchmark/blob/main/docs/user_guide.md"
  - "Martin Ankerl, nanobench: Fast and Accurate Microbenchmarking for C++ (v4.4.0), Martin Ankerl, 2026, https://nanobench.ankerl.com/"
  - "Victor Stinner, pyperf: Run Python Benchmarks (v2.10.0), Python Software Foundation, 2026, https://pyperf.readthedocs.io/en/latest/"
---

# Measurement Methodology Topic Inventory

This document maps authoritative upstream source heading groups for benchmarking rigor, performance measurement, and statistical analysis to owning notes, target depth levels, and existing vault coverage.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Gregg, Systems Performance 2e: Ch 1-2 (Methodologies: USE Method, RED Method, Latency Analysis, Scientific Method) | [[USE-RED-and-Systems-Performance-Methodologies]] | Use, Explain | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon.md\|Gregg-Canon]] |
| Gregg, Systems Performance 2e: Ch 4-5 (Observability Tools, PMCs, Sampling vs Instrumentation, Observer Overhead) | [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]] | Explain, Debug | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation.md\|Linux-Tracing]] |
| Gregg, Systems Performance 2e: Ch 5 (Application Methodology: Off-CPU Analysis, Thread States, Lock Analysis) | [[Off-CPU-Analysis-Thread-States-and-Causal-Profiling]] | Use, Explain, Decide | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon.md\|Gregg-Canon]] |
| Gregg, Systems Performance 2e: Ch 6-7 (CPUs, Hardware Counters, Microarchitectural Bottlenecks, Flame Graphs) | [[Hardware-Performance-Counters-Top-Down-TMA-and-Roofline]] | Use, Explain, Decide | none |
| Gregg, Systems Performance 2e: Ch 6 (Flame Graphs, Visualization Formats, Differential Analysis) | [[Flame-Graphs-Differential-Flame-Graphs-and-Visualizations]] | Use, Explain, Debug | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon.md\|Gregg-Canon]] |
| Gregg, Systems Performance 2e: Ch 12 (Benchmarking Sins, Statistical Analysis, Variance, Confidence Intervals) | [[Benchmark-Methodology-Noise-and-Environment-Control]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency.md\|Chapter_103]] |
| Intel VTune TMA Docs: Pipeline Slots, Issue Width, Hierarchical TMA Levels 1-4 (Retiring, Bad Speculation, Front-End, Back-End) | [[Hardware-Performance-Counters-Top-Down-TMA-and-Roofline]] | Use, Explain, Decide | none |
| Linux perf Wiki & Man Pages: Hardware Counter Multiplexing, PEBS/IBS Precise Event-Based Sampling, Instruction Skid | [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]] | Explain, Debug | none |
| Google Benchmark / nanobench / pyperf: Environment Control, CPU Governors, Core Pinning, ASLR, Alignment Bias | [[Benchmark-Methodology-Noise-and-Environment-Control]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency.md\|Chapter_103]] |
| Benchmark Statistics Literature: Coordinated Omission, Open vs Closed Workloads, HdrHistogram Bucket Architecture | [[Statistics-for-Benchmarks-Coordinated-Omission-and-HDR-Histograms]] | Use, Explain, Rebuild | [[14-Low-Latency-Systems/07 - Time & Measurement/Coordinated Omission in Low Latency Systems.md\|Coordinated-Omission]] |
| CI Performance Gating: Wall-Clock vs Instruction Counts, Non-Parametric Hypothesis Testing, Change-Point Detection | [[Performance-Regression-Detection-in-CI]] | Explain, Decide | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Latency Regression Testing in Continuous Integration.md\|Latency-Testing]] |
| Measurement Architecture: Decision Records for Sampling vs Instrumentation, CI Gating Policies, Frame Pointer Standards | [[Measurement-Methodology-Decision-Records]] | Explain, Decide | none |

## Out of scope

The following source heading groups are explicitly excluded from Pillar 04:

- Gregg, Systems Performance 2e Ch 8-11 (File Systems, Disks, Network, Cloud Computing): Storage, network, and cloud infrastructure operations belong to 01-CS-Foundations and 17-Cloud-AI-Engineering; Developer Toolchain concentrates on CPU, memory, microbenchmarking, and runtime profiling.
