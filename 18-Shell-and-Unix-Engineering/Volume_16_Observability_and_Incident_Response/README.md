---
type: moc
track: [sde, distinguished, low-latency]
level:
status: draft
last_reviewed:
sources: []
---

# Volume 16 - Observability and Incident Response from the Shell

During an incident the shell is the debugger.
This volume turns the standard Linux and macOS diagnostic tools into fluent, safe, repeatable procedures, and ends with runbooks written as tested scripts.

## Prerequisites

- [Volume 00 - The Unix Process Model Under the Shell](../Volume_00_The_Unix_Process_Model/README.md)
- [Volume 07 - Coreutils, Findutils, and the Userland Zoo](../Volume_07_Coreutils_Findutils_and_the_Userland_Zoo/README.md)

## Chapters

1. [The 60-Second Linux Performance Analysis](Chapter_01_The_60_Second_Linux_Performance_Analysis.md) - Brendan Gregg's ten-command checklist, what each command shows, and how to read it.
2. [Processes and Resources: ps, top, and /proc](Chapter_02_Processes_and_Resources_ps_top_and_proc.md) - Process states, CPU and memory accounting, and reading `/proc` directly.
3. [Files and Sockets: lsof, ss, and netstat](Chapter_03_Files_and_Sockets_lsof_ss_and_netstat.md) - Open files, listening sockets, and TCP connection states.
4. [Tracing: strace, ltrace, perf, and bpftrace](Chapter_04_Tracing_strace_ltrace_perf_and_bpftrace.md) - System call tracing, sampling profilers, and eBPF one-liners.
5. [Logs at Scale](Chapter_05_Logs_at_Scale.md) - journalctl, log triage pipelines, and extracting time windows.
6. [macOS Diagnostics](Chapter_06_MacOS_Diagnostics.md) - dtruss, fs_usage, sample, spindump, and `log show`.
7. [Runbooks as Tested Scripts](Chapter_07_Runbooks_as_Tested_Scripts.md) - Encoding incident procedures as safe, tested, dry-run-first scripts.
8. [CPU Affinity and Real-Time Controls](Chapter_08_CPU_Affinity_and_Real_Time_Controls.md) - taskset, chrt, isolcpus, and IRQ affinity for latency-sensitive systems.

## Labs

Each chapter's worked example lives in [`labs/`](../labs/README.md) as a lab directory named `v16-cNN-<slug>`, with a bats test that runs under every shell in the matrix.
