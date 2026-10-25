---
type: moc
track: [sde, distinguished, low-latency, quant-dev]
level:
status: draft
last_reviewed:
sources:
  - "Linux Kernel Organization, Linux perf Man Pages and Wiki (Linux 7.x), Linux Kernel Organization, 2026, https://perfwiki.github.io/"
  - "Brendan Gregg, Systems Performance: Enterprise and the Cloud, 2nd Edition, Addison-Wesley, 2020, https://www.brendangregg.com/systems-performance-2nd-edition-book.html"
  - "Brendan Gregg, BPF Performance Tools: Linux System and Application Observability, Addison-Wesley, 2019, https://www.brendangregg.com/bpf-performance-tools-book.html"
  - "bpftrace Developers, bpftrace Reference Guide (v0.27.1), bpftrace Project, 2026, https://github.com/bpftrace/bpftrace/blob/master/docs/reference_guide.md"
  - "Apple Inc., Instruments and xctrace Command-Line Tool, Apple Inc., 2026, https://developer.apple.com/documentation/xcode/improving-your-app-s-performance"
  - "Bartosz Taudul, Tracy Profiler User Manual (v0.14.1), Tracy Profiler Project, 2026, https://github.com/wolfpld/tracy"
  - "Google LLC, Perfetto: System Profiling, App Tracing and Analysis (v58.2), Google LLC, 2026, https://perfetto.dev/docs/"
  - "Milian Wolff, heaptrack: A Heap Memory Profiler for Linux (v1.5.0), KDE Project, 2026, https://github.com/KDE/heaptrack"
  - "Julian Seward et al., Valgrind User Manual: Cachegrind, Callgrind, Massif, DHAT (Valgrind 3.28.0), Valgrind Developers, 2026, https://valgrind.org/docs/manual/manual.html"
  - "Python Software Foundation, CPython 3.15 Documentation: profiling and profiling.sampling (Python 3.15.0), Python Software Foundation, 2026, https://docs.python.org/3.15/library/profiling.html"
  - "Python Software Foundation, Python Support for the Linux perf Profiler, Python Software Foundation, 2026, https://docs.python.org/3.15/howto/perf_profiling.html"
  - "Ben Frederickson, py-spy: A Sampling Profiler for Python Programs (v0.4.2), Ben Frederickson, 2026, https://github.com/benfred/py-spy"
  - "Emery Berger et al., Scalene: A High-Performance, High-Precision CPU, GPU, and Memory Profiler for Python (v2.4.0), Plasma Lab at UMass Amherst, 2026, https://github.com/plasma-umass/scalene"
  - "Bloomberg Finance L.P., Memray: Memory Profiler for Python (v1.20.0), Bloomberg Finance L.P., 2026, https://bloomberg.github.io/memray/"
  - "Joe Rickerby, pyinstrument: Call Stack Profiler for Python (v5.1.3), Joe Rickerby, 2026, https://pyinstrument.readthedocs.io/en/latest/"
  - "Gabriele Tornetta, Austin: A Frame Stack Sampler for CPython (v4.0.0 / austin-python 2.4.0), Gabriele Tornetta, 2026, https://github.com/P403n1x87/austin"
  - "Airspeed Velocity Developers, Airspeed Velocity: A Simple Python Benchmarking Tool (v0.6.6), asv Developers, 2026, https://asv.readthedocs.io/en/latest/"
  - "Brook Heisler, The Criterion.rs Book: Statistics-Driven Benchmarking in Rust (v0.8.2), Criterion.rs Developers, 2026, https://bheisler.github.io/criterion.rs/book/"
  - "Nikolai Vazquez, Divan: Fast and Simple Benchmarking for Rust (v0.1.21), Nikolai Vazquez, 2026, https://github.com/nvzqz/divan"
  - "Gungraun Developers, Gungraun: High-Precision Benchmark Framework for Rust (v0.20.0), Gungraun Project, 2026, https://github.com/gungraun/gungraun"
  - "Markus Stange, samply: Command-Line Sampling Profiler for macOS and Linux (v0.13.1), Markus Stange, 2026, https://github.com/mstange/samply"
  - "Parca Developers, Parca: Continuous Profiling for Linux (2026-10), Polar Signals, 2026, https://www.parca.dev/docs/overview"
  - "Grafana Labs, Grafana Pyroscope Documentation: Continuous Profiling Platform (2026-10), Grafana Labs, 2026, https://grafana.com/docs/pyroscope/latest/"
  - "OpenTelemetry Project, OpenTelemetry Profiles Specification (Public Alpha), Cloud Native Computing Foundation, 2026, https://opentelemetry.io/docs/specs/otel/profiles/"
---

# Profiling Topic Inventory

This document maps authoritative upstream source heading groups for profilers, tracers, and runtime telemetry tools to owning notes, target depth levels, and existing vault coverage.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| Linux perf Man Pages: perf-stat, perf-record, perf-report, perf-annotate, DWARF vs FP vs LBR Callchains | [[Linux-perf-in-Depth-and-perf-c2c]] | Use, Explain, Debug | [[12-Performance-Engineering/02-Profiling/profiling_guide.md\|profiling_guide]] |
| Linux perf Man Pages: perf-c2c, perf-mem, HITM Cache-Line Contention, NUMA Remote Hits, False Sharing | [[Linux-perf-in-Depth-and-perf-c2c]] | Use, Explain, Debug | [[12-Performance-Engineering/01-Cpu-Architecture/false_sharing.cpp\|false_sharing.cpp]] |
| Linux perf Man Pages: perf-sched, perf-lock, perf-trace eBPF Syscall Tracer | [[Linux-perf-in-Depth-and-perf-c2c]] | Use, Explain | none |
| Linux perf Man Pages: perf-intel-pt, perf-arm-spe, Hardware Instruction Control-Flow Tracing | [[Intel-VTune-AMD-uProf-and-Hardware-Tracing-PT-SPE]] | Use, Explain, Decide | none |
| Gregg BPF Performance Tools: ftrace (function_graph), eBPF Verifier, Maps, BTF, CO-RE, kprobes, uprobes, USDT | [[eBPF-bpftrace-ftrace-uprobes-and-USDT]] | Use, Explain, Debug | [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation.md\|Linux-Tracing]] |
| bpftrace Reference Guide: Probe Types (kprobe, uprobe, tracepoint, fentry, usdt), Builtins, Map Aggregations | [[eBPF-bpftrace-ftrace-uprobes-and-USDT]] | Use, Explain | [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/bpftrace-One-Liners.md\|bpftrace-One-Liners]] |
| Apple Instruments & xctrace Docs: Time Profiler, Counters/KPEP, Allocations, xctrace CLI, DTrace under SIP | [[macOS-Instruments-xctrace-and-DTrace]] | Use, Explain | none |
| Tracy Profiler Manual: Nanosecond Frame/Zone Client Markup, Context-Switch Tracking, Headless Capture | [[Tracy-and-Perfetto-Hybrid-Profilers]] | Use, Explain, Rebuild | none |
| Perfetto Docs: Service Model, ProtoZero Ring Buffers, Linux ftrace, heapprofd, PerfettoSQL Queries | [[Tracy-and-Perfetto-Hybrid-Profilers]] | Use, Explain, Rebuild | none |
| Continuous Profiling Docs: Google-Wide Profiling (GWP), Parca eBPF DWARF and Frame-Pointer Unwinding, Pyroscope, OTel Profiles | [[Continuous-Profiling-GWP-Parca-Pyroscope-and-OTel-Profiles]] | Explain, Decide | none |
| Profiling Architecture: Decision Records for Profiler Selection, Parca vs Pyroscope, Frame Pointer Policy | [[Profiling-Decision-Records]] | Explain, Decide | none |
| Profiler Implementation: Rebuild Lab for Sampling Profiler Emitting Folded Stack Formats | [[Rebuild-Lab-Sampling-Profiler-to-Folded-Stacks]] | Rebuild | none |
| C++ CPU Profiling: perf, samply, Callgrind Cache Simulation, llvm-mca Pipeline Modeling | [[Cpp-CPU-Profiling-perf-samply-Callgrind-and-llvm-mca]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency.md\|Chapter_103]] |
| C++ Memory Profiling: heaptrack, Valgrind Massif, DHAT, jemalloc/tcmalloc Arena Statistics | [[Cpp-Memory-Profiling-heaptrack-Massif-DHAT-and-Allocators]] | Use, Explain, Debug | [[12-Performance-Engineering/02-Profiling/profiling_guide.md\|profiling_guide]] |
| C++ Microbenchmarks: Google Benchmark, nanobench, DoNotOptimize, Hardware PMU Counters | [[Cpp-Google-Benchmark-and-nanobench]] | Use, Explain, Rebuild | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency.md\|Chapter_103]] |
| C++ Build Profiling: Clang -ftime-trace JSON Tracing, ClangBuildAnalyzer Template Cost | [[Cpp-Compile-Time-Profiling-ftime-trace-and-ClangBuildAnalyzer]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling.md\|Chapter_110]] |
| C++ Optimization Feedback: Instrumentation PGO, LBR AutoFDO, Post-Link Layout with LLVM BOLT | [[Cpp-PGO-AutoFDO-and-BOLT-Workflow]] | Use, Explain, Decide | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|Chapter_102]] |
| Python CPU Profilers: cProfile Tracing vs PEP 799 Tachyon Sampling, py-spy, pyinstrument, Scalene, Austin | [[Python-cProfile-Tachyon-and-Sampling-Profilers]] | Use, Explain, Debug | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| Python perf Integration: -X perf Trampoline Maps, -X perf_jit DWARF JIT Registration, Tachyon Frame Walking | [[Python-CPython-perf-Trampoline-and-Tachyon-Internals]] | Explain, Rebuild | none |
| Python Memory & Lines: Memray Native Allocator Hooking, Temporary Allocation Churn, line_profiler | [[Python-Memory-Profiling-Memray-and-Line-Profiler]] | Use, Explain | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| Python Benchmarks & Arrays: pyperf System Tuning, CPython pystats Specializers, asv, NumPy/Pandas GIL Stalls | [[Python-Benchmarking-pyperf-asv-and-NumPy-Pandas-Profiling]] | Use, Explain, Decide | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| Rust CPU Profiling: cargo flamegraph, samply, std Frame Pointers (-Cforce-frame-pointers=non-leaf) | [[Rust-CPU-Profiling-cargo-flamegraph-samply-and-Frame-Pointers]] | Use, Explain, Debug | none |
| Rust Benchmarking: Criterion.rs Regression, Divan Allocations, Gungraun Instruction Counters, dhat-rs | [[Rust-Benchmarking-Criterion-Divan-Gungraun-and-dhat]] | Use, Explain, Rebuild | none |
| Rust Build Diagnostics: cargo build --timings, -Z self-profile crox, cargo-llvm-lines, cargo-bloat | [[Rust-Compile-Time-and-Binary-Size-Profiling]] | Use, Explain, Debug | none |

## Out of scope

The following source heading groups are explicitly excluded from Pillar 05:

- Linux perf Man Pages: perf-kvm(1) and perf-iostat(1): Hypervisor guest exit profiling and PCIe I/O statistics are specialized cloud infrastructure operations outside developer toolchain scope.
- Gregg BPF Performance Tools Part II Ch 8-11 & 14-16 (File Systems, Disks, Networking, Security, Hypervisors): Storage, networking, kernel, and hypervisor administration BPF tools belong in operating systems and cloud engineering tracks rather than developer toolchains.
