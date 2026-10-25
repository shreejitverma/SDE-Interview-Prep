---
type: moc
track: [sde, distinguished, low-latency, quant-dev]
level:
status: draft
last_reviewed:
sources:
  - "Linux Kernel Organization, Linux perf Man Pages and Wiki (Linux 6.x), Linux Kernel Organization, 2026, https://perfwiki.github.io/"
  - "Brendan Gregg, Systems Performance: Enterprise and the Cloud, 2nd Edition, Addison-Wesley, 2020, https://www.brendangregg.com/systems-performance-2nd-edition-book.html"
  - "bpftrace Developers, bpftrace Reference Guide (v0.27.1), bpftrace Project, 2026, https://github.com/bpftrace/bpftrace/blob/master/docs/reference_guide.md"
  - "Apple Inc., Instruments and xctrace Command-Line Tool, Apple Inc., 2026, https://developer.apple.com/documentation/xcode/improving-your-app-s-performance"
---

# Profiling Skill Ladder

This skill ladder establishes observable, testable profiling and performance diagnosis competencies across career levels from L4 through L8+.
Each capability requires direct measurement of runtime software bottlenecks and rigorous verification of performance optimizations.

## Level 4 (Software Engineer I)

- Can capture an on-CPU profile using Linux perf record or macOS samply and inspect the hottest functions in a flame graph: [[Linux-perf-in-Depth-and-perf-c2c]], [[macOS-Instruments-xctrace-and-DTrace]]
- Can execute deterministic call tracing in Python using cProfile and inspect cumulative execution time using pstats: [[Python-cProfile-Tachyon-and-Sampling-Profilers]]
- Can generate flame graphs for Rust applications using cargo-flamegraph and identify hot stack branches: [[Rust-CPU-Profiling-cargo-flamegraph-samply-and-Frame-Pointers]]
- Can run basic bpftrace one-liners to trace process execution counts, open files, and syscall frequencies: [[eBPF-bpftrace-ftrace-uprobes-and-USDT]]

## Level 5 (Software Engineer II)

- Can diagnose cache-line false sharing across CPU cores using perf c2c to identify bouncing modified cache lines (HITM): [[Linux-perf-in-Depth-and-perf-c2c]], [[Drill-Profiling-False-Sharing-Cache-Contention]]
- Can profile heap allocation hotspots, peak memory footprints, and temporary allocation churn in C++ using heaptrack: [[Cpp-Memory-Profiling-heaptrack-Massif-DHAT-and-Allocators]]
- Can profile Python applications out-of-process using py-spy or PEP 799 Tachyon sampling without introducing runtime observer overhead: [[Python-cProfile-Tachyon-and-Sampling-Profilers]], [[Python-CPython-perf-Trampoline-and-Tachyon-Internals]]
- Can pinpoint line-by-line memory allocations and native leaks in Python codebases using Bloomberg's memray: [[Python-Memory-Profiling-Memray-and-Line-Profiler]]
- Can profile Rust memory allocations and heap usage in unit tests using dhat-rs: [[Rust-Benchmarking-Criterion-Divan-Gungraun-and-dhat]]
- Can diagnose compile-time bottlenecks in C++ projects by capturing Clang -ftime-trace JSON outputs and parsing them in Speedscope: [[Cpp-Compile-Time-Profiling-ftime-trace-and-ClangBuildAnalyzer]]

## Level 6 (Senior Software Engineer)

- Can triage live production p99 latency spikes without restarting services using bpftrace off-CPU probes and differential flame graphs: [[Playbook-Diagnose-a-Production-Tail-Latency-Spike-with-eBPF-and-perf]], [[eBPF-bpftrace-ftrace-uprobes-and-USDT]]
- Can profile mixed-runtime applications across Python and native C extensions using -X perf_jit DWARF mappings and memray: [[Python-CPython-perf-Trampoline-and-Tachyon-Internals]], [[Drill-Profiling-Python-C-Extension-GIL-and-Allocation-Bottleneck]]
- Can diagnose monomorphization code bloat, instruction-cache misses, and compile-time inflation in Rust using cargo-llvm-lines: [[Rust-Compile-Time-and-Binary-Size-Profiling]], [[Drill-Profiling-Rust-Monomorphization-and-Inlining-Regression]]
- Can model basic-block instruction throughput, execution port saturation, and dependency latencies using llvm-mca: [[Cpp-CPU-Profiling-perf-samply-Callgrind-and-llvm-mca]]
- Can implement custom zone and frame markup instrumentation in high-frequency trading or gaming systems using Tracy: [[Tracy-and-Perfetto-Hybrid-Profilers]]
- Can run deterministic instruction-count benchmarks in CI using Gungraun to catch regressions independently of runner hardware noise: [[Rust-Benchmarking-Criterion-Divan-Gungraun-and-dhat]]

## Level 7 (Staff Software Engineer)

- Can architect continuous fleet-wide profiling pipelines using Parca or Grafana Pyroscope to continuously index CPU and memory profiles: [[Continuous-Profiling-GWP-Parca-Pyroscope-and-OTel-Profiles]]
- Can deploy automated feedback-directed binary optimization pipelines (PGO, AutoFDO with LBR, and LLVM BOLT) that achieve 7-15% fleet capacity wins: [[Cpp-PGO-AutoFDO-and-BOLT-Workflow]]
- Can implement custom sampling profilers in C++ or Rust that read kernel performance ring buffers and emit folded stack formats: [[Rebuild-Lab-Sampling-Profiler-to-Folded-Stacks]]
- Can correlate continuous profiling exemplars with distributed trace spans using OpenTelemetry Profiles standards: [[Continuous-Profiling-GWP-Parca-Pyroscope-and-OTel-Profiles]]
- Can automate headless macOS performance regression auditing in Xcode CI pipelines using xctrace recording and XML extraction: [[macOS-Instruments-xctrace-and-DTrace]]

## Level 8+ (Principal and Distinguished Engineer)

- Can formulate enterprise-wide profiling strategies, establishing standardized tool selections, continuous profiling retention, and compiler feedback loops: [[Profiling-Decision-Records]]
- Can lead corporate initiatives to mandate frame pointers (-fno-omit-frame-pointer) or evaluate next-generation unwind formats (GNU SFrame): [[Continuous-Profiling-GWP-Parca-Pyroscope-and-OTel-Profiles]], [[Profiling-Decision-Records]]
- Can identify and resolve fleet-wide microarchitectural inefficiencies across massive multi-tenant cloud clusters using Google-Wide Profiling methodologies: [[Continuous-Profiling-GWP-Parca-Pyroscope-and-OTel-Profiles]]
