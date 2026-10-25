---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# C++ Compile-Time Profiling: ftime-trace and ClangBuildAnalyzer

> [!summary] Scope
> This note owns compile-time bottleneck profiling for large C++ codebases.
> It covers the Use, Explain, and Debug rungs by generating build traces with Clang -ftime-trace, parsing trace visualizer JSON in Speedscope/Perfetto, and analyzing template instantiation costs with ClangBuildAnalyzer.
> It equips engineers to identify expensive headers, template expansions, and optimizer passes that inflate build cycles.

## Related

- Prerequisites: none
- Next: [[Cpp-PGO-AutoFDO-and-BOLT-Workflow]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_09_Specialized_Mastery/Chapter_110_Build_Systems_and_Tooling/Chapter_110_Build_Systems_Dependency_Management_and_Tooling|Chapter_110_Build_Systems_Dependency_Management_and_Tooling]]
- Lab: `code/05-Profiling/ftime_trace_analysis/`
