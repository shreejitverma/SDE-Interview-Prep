---
type: moc
track: [sde, distinguished, low-latency, quant-dev]
level:
status: draft
last_reviewed:
sources:
  - "Free Software Foundation, Debugging with GDB: The GNU Source-Level Debugger (GDB 18.1), Free Software Foundation, 2026, https://sourceware.org/gdb/current/onlinedocs/gdb/"
  - "LLVM Project, LLDB Documentation (LLVM 23.1.4), LLVM Project, 2026, https://lldb.llvm.org/"
  - "Robert O'Callahan et al., rr: Lightweight Record and Replay for Linux (rr 5.10.0), Mozilla and rr-debugger, 2026, https://rr-project.org/"
  - "LLVM Project, AddressSanitizer (ASan) Documentation, LLVM Project, 2026, https://clang.llvm.org/docs/AddressSanitizer.html"
  - "Python Software Foundation, CPython Library: Debugging and Profiling (Python 3.15.0), Python Software Foundation, 2026, https://docs.python.org/3.15/library/debug.html"
  - "The Rust Project Developers, The rustc Book and Rust Unstable Book (rustc 1.99.1), The Rust Project, 2026, https://doc.rust-lang.org/rustc/"
---

# Debugging Skill Ladder

This skill ladder establishes observable, testable debugging competencies across career levels from L4 through L8+.
Each capability requires reproducible execution under realistic development and production incident conditions.

## Level 4 (Software Engineer I)

- Can launch an inferior under GDB or LLDB, set conditional line breakpoints, inspect local frame variables, and step through branching logic: [[Cpp-GDB-and-LLDB-Expert-Guide]]
- Can configure standard interactive post-mortem debugging in Python using pdb and capture crash tracebacks with faulthandler: [[Python-pdb-debugpy-and-Remote-Attach]], [[Python-faulthandler-and-Crash-Diagnostics]]
- Can enable AddressSanitizer and UndefinedBehaviorSanitizer flags in CMake or Cargo and interpret the resulting stack trace report: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]], [[Cpp-UndefinedBehaviorSanitizer-and-UB-Field-Guide]], [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]
- Can navigate Rust panic backtraces, configure RUST_BACKTRACE, and inspect variable states using rust-gdb or rust-lldb: [[Rust-GDB-LLDB-Panics-and-Backtraces]]
- Can establish an SSH port forward or debugpy listening socket to inspect a local containerized service: [[Python-pdb-debugpy-and-Remote-Attach]]

## Level 5 (Software Engineer II)

- Can triage a stripped release production crash from a core dump by downloading debug symbols and mapping registers to disassembly: [[Debug-a-Production-Crash-From-a-Core-Dump]], [[Drill-Debugging-Use-After-Free-Core-Dump]]
- Can write custom Python pretty-printers and synthetic child providers in GDB or LLDB for complex internal data structures: [[Cpp-Debugger-Scripting-and-Pretty-Printers]]
- Can run deterministic execution recording with rr, set reverse watchpoints on memory corruption addresses, and locate the root-cause instruction: [[Record-and-Replay-rr-UDB-and-Pernosco]]
- Can isolate memory leaks and cyclic reference graphs in Python services using tracemalloc and gc referrers: [[Python-tracemalloc-gc-and-Memory-Leaks]]
- Can detect data races in multi-threaded C++ and Rust code using ThreadSanitizer and evaluate unsafe blocks with Miri under Tree Borrows: [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]], [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]
- Can diagnose asynchronous task stalls, lost wakers, and unawaited futures in Tokio or asyncio applications using live diagnostic tools: [[Rust-Async-Debugging-tokio-console-and-tracing]], [[Python-Concurrency-Asyncio-Multiprocessing-and-Free-Threading-Bugs]], [[Drill-Debugging-Asyncio-and-Tokio-Deadlock]]

## Level 6 (Senior Software Engineer)

- Can diagnose complex heap metadata corruption, double-frees, and use-after-free bugs in production binaries without debuginfo: [[Cpp-Playbook-Heap-Corruption]], [[Core-Dumps-Minidumps-and-Crash-Reporting]]
- Can reconstruct caller variable state in heavily optimized -O3 and LTO binaries using DWARF location expressions and register tracking: [[Cpp-Debugging-Optimized-Code]]
- Can isolate deadlocks in multi-threaded services by dumping mutex ownership graphs and thread wait queues under GDB without restarting the process: [[Cpp-Playbook-Deadlocks-and-Livelocks]]
- Can safely attach to a live production CPython process using PEP 768 or py-spy dump to extract active stacks without destabilizing runtime throughput: [[Python-pdb-debugpy-and-Remote-Attach]], [[Python-Debugging-Hung-Processes-and-C-Extensions]]
- Can root-cause memory safety, alignment, and panic unwinding bugs across Foreign Function Interface (FFI) boundaries connecting Rust, C++, and Python: [[Rust-FFI-Boundary-Debugging-Across-Cpp-and-Python]], [[Drill-Debugging-Mixed-Python-Rust-FFI-Segfault]]
- Can construct automated property-based test suites using Hypothesis and Loom to systematically falsify concurrent synchronization invariants: [[Python-Hypothesis-and-Type-Checkers-as-Bug-Prevention]], [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]

## Level 7 (Staff Software Engineer)

- Can design and deploy automated out-of-process crash capture infrastructure across a fleet using Crashpad and federated debuginfod symbol servers: [[Core-Dumps-Minidumps-and-Crash-Reporting]], [[Symbol-Servers-and-debuginfod]]
- Can implement production sampling crash catchers using hardware memory tagging (ARM64 MTE) or GWP-ASan to catch zero-day memory bugs in production: [[Cpp-HWASan-and-Hardware-Memory-Tagging]]
- Can build custom continuous fuzzing pipelines in CI using libFuzzer, AFL++, and structure-aware mutators that detect edge-case parsing regressions: [[Cpp-Clang-Tidy-Static-Analyzer-and-Fuzzing-libFuzzer-AFLPlusPlus]], [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]
- Can architect distributed trace context propagation schemes that link upstream transaction failures to local core dumps and kernel panic reports: [[Debugging-Distributed-Systems-with-Traces-and-Correlation]]
- Can implement an in-house ptrace or Mach exception port debugger to support custom hardware, embedded targets, or proprietary execution runtimes: [[Rebuild-Lab-ptrace-Debugger-with-DWARF-Backtrace]], [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]]

## Level 8+ (Principal and Distinguished Engineer)

- Can formulate organizational debugging policy establishing sanitizer CI tiers, symbol server SLAs, and corporate memory safety migration roadmaps: [[Debugging-Decision-Records]]
- Can analyze complex OS kernel, dynamic loader, and hypervisor boundary bugs that evade standard debugging APIs: [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]], [[Remote-Debugging-gdbserver-lldb-server-and-RSP]]
- Can architect company-wide automated crash triage systems that cluster millions of daily client and server crash reports by normalized inline stack hashes: [[Core-Dumps-Minidumps-and-Crash-Reporting]], [[Symbol-Servers-and-debuginfod]]
- Can establish formal verification, property-based fuzzing, and concurrency testing gates that eliminate entire classes of synchronization bugs prior to production deployment: [[Debugging-Decision-Records]], [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]]
