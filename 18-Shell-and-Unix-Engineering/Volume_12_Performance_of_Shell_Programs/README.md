---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 12 - Performance of Shell Programs

Shell performance is dominated by process creation and by how many times data crosses a pipe.
This volume builds a measured cost model, applies it to real pipelines, benchmarks shell against Python and compiled tools on the same workloads, and ends with a rule for when to stop writing shell.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)
- [Volume 06 - Regular Expressions and Text Processing](../Volume_06_Regular_Expressions_and_Text_Processing/README.md)
- [Volume 07 - Coreutils, Findutils, and the Userland Zoo](../Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/README.md)

## Chapters

1. [A Cost Model for Shell](Chapter_01_A_Cost_Model_for_Shell.md) - The measured cost of fork, exec, subshells, command substitution, and builtins versus external commands.
2. [Measuring Shell Performance](Chapter_02_Measuring_Shell_Performance.md) - hyperfine, time, `strace -c`, dtruss, and perf, with noise control.
3. [Loops versus Single-Pass Tools](Chapter_03_Loops_vs_Single_Pass_Tools.md) - Why a while-read loop is slow, and moving the work into awk, sort, and join.
4. [Big-Data Pipelines](Chapter_04_Big_Data_Pipelines.md) - `sort -S`, `LC_ALL=C`, split and parallel, and telling I/O-bound stages from CPU-bound ones.
5. [Shell versus Python versus Compiled Tools, and When to Stop Writing Shell](Chapter_05_Shell_vs_Python_vs_Compiled_Tools.md) - A benchmarked comparison on the same workloads, the crossover points, and a decision rule based on size, data structures, error handling, and team.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v12-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
