---
type: playbook
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Analyze Gigabyte Terminal Logs

> [!summary] Scope
> Owns the operational runbook for high-throughput log analysis on gigabyte-to-terabyte incident dumps directly in the terminal.
> Details pipeline architectures utilizing LC_ALL=C byte-locale bypass, fast decompression (pzstd, which parallelizes only multi-frame files, and pigz, which decompresses on one thread plus I/O and checksum threads), ripgrep pre-filtering into streaming mawk/gawk and jq, reservoir sampling, and numeric histogram aggregation.
> Reaches Use and Debug, putting the text-processing and buffering concept notes into emergency incident production practice.

## Related

- Prerequisites: [[Structured-Data-Wrangling-Jq-Yq-and-Coreutils]], [[Ripgrep-Internals-and-Fast-Text-Search]], [[Locale-Collation-and-Unicode-in-CLI-Tools]], [[Pipe-Buffering-and-Stdbuf]]
- Next: [[Tmux-Architecture-and-Session-Persistence]]
- See also: [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/Log-Triage-Pipelines|Log Triage Pipelines]], [[19-Shell-and-Unix-Engineering/20-Observability-and-Incident-Response/Sixty-Second-Linux-Analysis|Sixty Second Linux Analysis]]
- Lab: `code/01-Command-Line/large_log_pipeline/`
