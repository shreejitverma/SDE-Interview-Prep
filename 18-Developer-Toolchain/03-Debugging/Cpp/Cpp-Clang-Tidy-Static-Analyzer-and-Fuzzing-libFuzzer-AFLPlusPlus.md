---
type: concept
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Clang-Tidy, Static Analyzer, and Fuzzing (libFuzzer, AFL++)

> [!summary] Scope
> This note owns automated defect detection via AST linting, symbolic execution, and coverage-guided fuzzing in C++.
> It covers the Use, Explain, and Rebuild rungs by implementing clang-tidy checks, Clang Static Analyzer path engines, and fuzz targets using libFuzzer and AFL++ v5.03c.
> It links reactive sanitizer reports with proactive bug discovery in CI pipelines.

## Related

- Prerequisites: [[Cpp-AddressSanitizer-and-LeakSanitizer-Internals]]
- Next: [[Cpp-Playbook-Heap-Corruption]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_105_Sanitizers_Fuzzing_Testing_Concurrency/Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code|Chapter_105_Sanitizers_Fuzzing_and_Testing_Concurrent_Code]]
- Lab: `code/03-Debugging/fuzzing_libfuzzer/`
