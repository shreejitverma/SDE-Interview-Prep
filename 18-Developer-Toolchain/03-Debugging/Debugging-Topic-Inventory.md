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
  - "LLVM Project, ThreadSanitizer (TSan) Documentation, LLVM Project, 2026, https://clang.llvm.org/docs/ThreadSanitizer.html"
  - "LLVM Project, MemorySanitizer (MSan) Documentation, LLVM Project, 2026, https://clang.llvm.org/docs/MemorySanitizer.html"
  - "LLVM Project, UndefinedBehaviorSanitizer (UBSan) Documentation, LLVM Project, 2026, https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html"
  - "LLVM Project, LeakSanitizer (LSan) Documentation, LLVM Project, 2026, https://clang.llvm.org/docs/LeakSanitizer.html"
  - "LLVM Project, Hardware-assisted AddressSanitizer (HWASan) Design, LLVM Project, 2026, https://clang.llvm.org/docs/HardwareAssistedAddressSanitizerDesign.html"
  - "Apple Inc., Diagnosing Memory, Thread, and Crash Issues Early, Apple Inc., 2026, https://developer.apple.com/documentation/xcode/diagnosing-memory-thread-and-crash-issues-early"
  - "Julian Seward et al., Valgrind User Manual (Valgrind 3.28.0), Valgrind Developers, 2026, https://valgrind.org/docs/manual/manual.html"
  - "LLVM Project, libFuzzer: A Library for Coverage-Guided Fuzz Testing, LLVM Project, 2026, https://llvm.org/docs/LibFuzzer.html"
  - "Marc Heuse et al., AFL++ Documentation (v5.03c), AFLplusplus Project, 2026, https://aflplus.plus/docs/"
  - "Python Software Foundation, CPython Library: Debugging and Profiling (Python 3.15.0), Python Software Foundation, 2026, https://docs.python.org/3.15/library/debug.html"
  - "Python Software Foundation, CPython Debugging with GDB (Python 3.15.0), Python Software Foundation, 2026, https://docs.python.org/3.15/howto/gdb_helpers.html"
  - "Microsoft Corporation, debugpy: An Implementation of the Debug Adapter Protocol for Python (v1.8.22), Microsoft Corporation, 2026, https://github.com/microsoft/debugpy"
  - "David R. MacIver et al., Hypothesis: Property-Based Testing for Python (v6.170.9), Hypothesis Works, 2026, https://hypothesis.readthedocs.io/en/latest/"
  - "The Rust Project Developers, The rustc Book and Rust Unstable Book (rustc 1.99.1), The Rust Project, 2026, https://doc.rust-lang.org/rustc/"
  - "Ralf Jung et al., Miri: An Interpreter for Rust's Mid-level Intermediate Representation (rustc 1.101 nightly), The Rust Project, 2026, https://github.com/rust-lang/miri"
  - "Carl Lerche et al., Loom: Concurrency Permutation Testing for Rust (v0.7.2), Tokio Project, 2026, https://docs.rs/loom/latest/loom/"
  - "The Rust Fuzz Project, The Rust Fuzz Book (cargo-fuzz 0.13.2), The Rust Project, 2026, https://rust-fuzz.github.io/book/"
  - "Tokio Project, Tokio Console: Async Diagnostics for Rust (v0.1.14), Tokio Project, 2026, https://github.com/tokio-rs/console"
  - "Tokio Project, tracing: Application-Level Tracing for Rust (v0.1.41), Tokio Project, 2026, https://docs.rs/tracing/latest/tracing/"
  - "The Chromium Projects, Crashpad: Multi-Platform Crash Reporting System, Google LLC, 2026, https://chromium.googlesource.com/crashpad/crashpad/+/HEAD/doc/overview_design.md"
  - "elfutils Developers, debuginfod: Debuginfo Federation Server and Client (elfutils 0.196), elfutils Project, 2026, https://sourceware.org/elfutils/Debuginfod.html"
---

# Debugging Topic Inventory

This document maps authoritative upstream source heading groups for debugging tools across C++, Python, and Rust to owning notes, target depth levels, and existing vault coverage.

| Source heading group | Owning note | Depth target | Existing vault coverage |
| :--- | :--- | :--- | :--- |
| GDB Manual: Ch 1-4 (Sample Session, Command Line, Running Programs, Inferior I/O, Multi-Process fork, Multi-Thread Non-Stop) | [[Cpp-GDB-and-LLDB-Expert-Guide]] | Use, Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/09-part-design/36_advanced_debugging.md\|36_advanced_debugging]] |
| GDB Manual: Ch 5 (Stopping and Continuing, Breakpoints, Hardware Watchpoints, Catchpoints, Signals) | [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]] | Use, Explain, Rebuild | [[01-CS-Foundations/Operating-Systems/GIOS/Part-2-Process-Thread-Management/P2L1-Processes-and-Process-Management.md\|P2L1]] |
| GDB Manual: Ch 6-7 (Reverse Execution, Process Record and Replay) | [[Record-and-Replay-rr-UDB-and-Pernosco]] | Use, Explain | none |
| GDB Manual: Ch 8-10 (Stack Frames, Examining Memory and Registers, Value History) | [[Cpp-GDB-and-LLDB-Expert-Guide]] | Use, Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/09-part-design/36_advanced_debugging.md\|36_advanced_debugging]] |
| GDB Manual: Ch 11-12 (Debugging Optimized Code, Inlined Functions, Entry Values, Macro Expansion) | [[Cpp-Debugging-Optimized-Code]] | Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_104_Undefined_Behaviour_and_the_Optimiser/Chapter_104_Undefined_Behaviour_and_the_Optimiser.md\|Chapter_104]] |
| GDB Manual: Ch 13 & 31 (Tracepoints, Fast Tracepoints, In-Process Agent) | [[Cpp-GDB-and-LLDB-Expert-Guide]] | Use, Explain | none |
| GDB Manual: Ch 15 (Multi-Language Support: C, C++, Rust Demangling and Expressions) | [[Cpp-GDB-and-LLDB-Expert-Guide]] | Use, Explain | none |
| GDB Manual: Ch 16-18 (Symbol Table, Inferior Function Calls, Separate Debug Files, GDB Index, Core Files) | [[Core-Dumps-Minidumps-and-Crash-Reporting]] | Use, Explain, Decide | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation.md\|Observability]] |
| GDB Manual: Ch 19-21 & App E-G (Remote Debugging, Remote Serial Protocol RSP, Target XML Descriptions) | [[Remote-Debugging-gdbserver-lldb-server-and-RSP]] | Use, Explain | none |
| GDB Manual: Ch 22-23 (Python API: gdb.Value, Pretty-Printers, Frame Filters, Unwinders) | [[Cpp-Debugger-Scripting-and-Pretty-Printers]] | Use, Explain, Rebuild | none |
| GDB Manual: Ch 24-25 & 33 (Configuration, Command Line Options, Readline, History) | [[Debugging-Expert-Defaults-gdbinit-and-lldbinit]] | Use, Explain | none |
| GDB Manual: Ch 30 (JIT Debugging Interface, Custom JIT Readers) | [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]] | Explain, Rebuild | none |
| LLDB Documentation: Tutorial, GDB-to-LLDB Command Equivalence, Settings | [[Cpp-GDB-and-LLDB-Expert-Guide]] | Use, Explain | [[19-Shell-and-Unix-Engineering/05-Bash-Internals/Debugging-Bash-with-lldb.md\|Debugging-Bash]] |
| LLDB Documentation: Variable Formatting, Type Summaries, Synthetic Children, Formatter Bytecode | [[Cpp-Debugger-Scripting-and-Pretty-Printers]] | Use, Explain, Rebuild | none |
| LLDB Documentation: Symbolication, macOS dSYM Bundles, Spotlight Lookup, Crashlogs | [[Symbol-Servers-and-debuginfod]] | Use, Explain, Decide | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_102_Linking_Loading_ABI/Chapter_102_Linking_Loading_ABI_and_WPO.md\|Chapter_102]] |
| LLDB Documentation: Scripting Bridge, Python SB API, Custom Commands, Scripted Process Plugins | [[Cpp-Debugger-Scripting-and-Pretty-Printers]] | Use, Explain, Rebuild | none |
| LLDB Documentation: Architecture, Data Inspection Language (DIL) vs Clang JIT Expression Parser | [[How-Debuggers-Work-ptrace-Mach-and-Breakpoints]] | Explain, Debug | none |
| rr Documentation: Architecture, Single-Core Scheduling, syscallbuf, PMU Event Counters | [[Record-and-Replay-rr-UDB-and-Pernosco]] | Use, Explain, Decide | [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Deterministic Replay and Packet Injection Testing.md\|Deterministic-Replay]] |
| rr Documentation: CPU Requirements, Virtualization Constraints, Apple Silicon Limitations | [[Record-and-Replay-rr-UDB-and-Pernosco]] | Explain, Debug | none |
| rr Documentation: Deterministic Replay, Chaos Mode, Reverse Execution, Checkpoints | [[Record-and-Replay-rr-UDB-and-Pernosco]] | Use, Debug | none |
| Clang AddressSanitizer: 1:8 Shadow Memory, Redzones, Quarantine, Stack UAR, Container Overflow | [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]] | Use, Explain, Rebuild | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code.md\|Chapter_105]] |
| Clang LeakSanitizer: Standalone Mode, Reachability Heap Pointer Scanning, Suppressions | [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]] | Use, Explain | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code.md\|Chapter_105]] |
| Clang HWASan: ARM64 TBI and MTE Hardware Tagging, 1:16 Shadow, Tag Fault Modes | [[Cpp-HWASan-and-Hardware-Memory-Tagging]] | Explain, Decide | none |
| Clang ThreadSanitizer: Shadow State Machine, Vector Clocks, Happens-Before, TSan v3 | [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code.md\|Chapter_105]] |
| Clang MemorySanitizer: Bit-Exact Shadow Memory, Origin Tracking, Standard Library Recompilation | [[Cpp-ThreadSanitizer-and-MemorySanitizer-Internals]] | Use, Explain, Debug | none |
| Clang UndefinedBehaviorSanitizer: UB Check Catalog, Trap Mode, vptr Checks, Minimal Runtime | [[Cpp-UndefinedBehaviorSanitizer-and-UB-Field-Guide]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_104_Undefined_Behaviour_and_the_Optimiser/Chapter_104_Undefined_Behaviour_and_the_Optimiser.md\|Chapter_104]] |
| Apple Xcode Sanitizer Docs: Darwin ASan/TSan, Native leaks Tool, libgmalloc, MallocScribble | [[Cpp-Sanitizers-and-Memory-Debugging-on-macOS]] | Use, Explain, Debug | none |
| Valgrind User Manual: VEX Dynamic Binary Translation, Memcheck V/A Shadow Bits, Leak Kinds | [[Cpp-Valgrind-Memcheck-Helgrind-and-DRD]] | Use, Explain, Debug | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/manuscript/09-part-design/36_advanced_debugging.md\|36_advanced_debugging]] |
| Valgrind User Manual: Helgrind Locksets, DRD Segment Analysis, POSIX Thread Checking | [[Cpp-Valgrind-Memcheck-Helgrind-and-DRD]] | Use, Explain, Debug | none |
| libFuzzer Documentation: Coverage-Guided In-Process Fuzzing, SanitizerCoverage, Mutators | [[Cpp-Clang-Tidy-Static-Analyzer-and-Fuzzing-libFuzzer-AFLPlusPlus]] | Use, Explain, Rebuild | [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code.md\|Chapter_105]] |
| AFL++ Documentation: LLVM Instrumentation Modes, Forkserver, Persistent Mode, afl-fuzz | [[Cpp-Clang-Tidy-Static-Analyzer-and-Fuzzing-libFuzzer-AFLPlusPlus]] | Use, Explain | none |
| CPython Docs: pdb CLI, Post-Mortem, Breakpoint Commands, PEP 768 Live Remote PID Attach | [[Python-pdb-debugpy-and-Remote-Attach]] | Use, Explain, Debug | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| CPython Docs: PEP 669 Execution Event Monitoring, sys.monitoring Tool Slots, DISABLE Fast Path | [[Python-sys-monitoring-and-Tracing]] | Use, Explain, Rebuild | none |
| CPython Docs: faulthandler Crash Traceback Dumping, C Stack Dumping, Timeout Watchdogs | [[Python-faulthandler-and-Crash-Diagnostics]] | Use, Explain | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| CPython Docs: tracemalloc Allocator Tracing, Snapshot Diffs, GC Reference Cycles | [[Python-tracemalloc-gc-and-Memory-Leaks]] | Use, Explain, Debug | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| CPython GDB Support: python-gdb.py, py-bt, PyObject Introspection, Frame Unwinding | [[Python-Debugging-Hung-Processes-and-C-Extensions]] | Use, Explain | [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_29_PROFILING_BENCHMARKING_AND_DIAGNOSTICS.md\|Chapter_29]] |
| debugpy Documentation: Debug Adapter Protocol Architecture, Multi-Process Attach, Asyncio | [[Python-pdb-debugpy-and-Remote-Attach]] | Use, Explain | [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production.md\|10-Testing-Debugging]] |
| Hypothesis Documentation: Property-Based Testing, Conjecture Shrinking, RuleBasedStateMachine | [[Python-Hypothesis-and-Type-Checkers-as-Bug-Prevention]] | Use, Explain | [[02-Programming-Languages/Python/Python_Zero_to_Godhood/Chapter_41_Testing_Debugging_and_Quality_Assurance.md\|Chapter_41]] |
| rustc Book: Codegen Options, -C debuginfo, Split DWARF, Symbol Mangling, Frame Pointers | [[Rust-GDB-LLDB-Panics-and-Backtraces]] | Use, Explain | none |
| rustc Book: Tier 2 Sanitizer Targets, Unstable -Zsanitizer Flags, Sanitizer Target Runtime | [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]] | Use, Explain, Debug | none |
| Miri Documentation: MIR Undefined Behavior Detection, Stacked/Tree Borrows, Memory Leaks | [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]] | Use, Explain, Debug | none |
| Loom Documentation: Concurrency Permutation Testing, C11 Memory Model, Preemption Bounding | [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]] | Use, Explain | none |
| Rust Fuzz Book: cargo-fuzz LibFuzzer Integration, Fuzz Target Oracles, Arbitrary Trait | [[Rust-Miri-Sanitizers-Loom-and-Cargo-Fuzz]] | Use, Explain | none |
| Tokio Console & tracing Docs: Task Diagnostics, Lost Waker Detection, Span Context Chains | [[Rust-Async-Debugging-tokio-console-and-tracing]] | Use, Explain, Debug | none |
| Crashpad Documentation: Out-of-Process Crash Handler, Exception Ports, Minidump Generation | [[Core-Dumps-Minidumps-and-Crash-Reporting]] | Explain, Decide | none |
| debuginfod Documentation: HTTP Build ID Indexing, Client Federation, Debuginfo Stripping | [[Symbol-Servers-and-debuginfod]] | Use, Explain, Decide | none |

## Out of scope

The following source heading groups are explicitly excluded from Pillar 03:

- GDB Manual Ch 14 (Debugging Programs That Use Overlays): Legacy embedded memory-overlay mechanism not relevant to modern server or systems toolchains.
- GDB Manual Ch 26 & 28 (Using GDB under GNU Emacs; GDB Annotation Interface): Editor-specific legacy Emacs mode and deprecated level-2 annotation interface superseded by GDB/MI and DAP.
- Valgrind User Manual Ch 11-13 (Lackey, Nulgrind, BBV): Toy template tools and basic-block vector generators not used in production debugging workflows.
