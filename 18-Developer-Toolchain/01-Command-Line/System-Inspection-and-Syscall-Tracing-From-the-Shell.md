---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# System Inspection and Syscall Tracing From the Shell

> [!summary] Scope
> Owns live operating system triage and process inspection from the command line.
> Details system metrics tools (ps, top/htop, lsof, ss, vmstat, iostat) and system call tracing via strace (ptrace overhead, filtering, timing) and macOS counterparts (dtruss, ktrace).
> Reaches Use through Debug, handing in-depth profiling and PMU hardware counters to 04-Measurement-Methodology and 05-Profiling.

## Related

- Prerequisites: [[Proc-Filesystem-and-Process-Credentials]]
- Next: [[Containers-and-VMs-for-macOS-Development]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation|Linux Tracing and Instrumentation]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/strace-for-Incidents|strace for Incidents]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/lsof|lsof]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/ps-and-Process-States|ps and Process States]]
- Lab: `code/01-Command-Line/strace_sys_inspect/` (requires: linux)
