---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Performance of Shell Programs

Shell performance is dominated by process creation and by how often data crosses a pipe.
These notes build a measured cost model, benchmark shell against Python and compiled tools, and end with when to stop writing shell.

Part of [Shell and Unix Engineering](../README.md).

## Learning order

Read in this order; each note links its prerequisite and the next note.

1. [Shell Cost Model](Shell-Cost-Model.md)
2. [Measuring with hyperfine](Measuring-with-hyperfine.md)
3. [Fork Avoidance](Fork-Avoidance.md)
4. [while-read Loops versus Single-Pass Tools](while-read-Loops-versus-Single-Pass-Tools.md)
5. [Big Data Pipelines](Big-Data-Pipelines.md)
6. [LC_ALL=C for Speed](LC_ALL-C-for-Speed.md)
7. [I/O-Bound versus CPU-Bound Stages](IO-Bound-versus-CPU-Bound-Stages.md)
8. [Shell versus Python versus Compiled Benchmarks](Shell-versus-Python-versus-Compiled-Benchmarks.md)
9. [When to Stop Writing Shell](When-to-Stop-Writing-Shell.md)
