---
tags: [security/audit, audit/antivirus, clamav, provenance]
aliases: [ClamAV Scan Report, Whitepapers Security Audit, Antivirus Verification]
status: verified
created: 2026-09-17
type: paper
track: [distinguished, sde]
level:
last_reviewed:
sources: []
---

# Security Audit Report — Technical Whitepapers Archive

> [!summary]
> All files in this technical whitepapers archive were subjected to automated anti-malware and signature verification using the ClamAV scanning engine prior to indexing. The scan confirmed **0 infected files** across all scanned directories and payload archives.

---

## Scan Summary & Execution Metrics

```text
----------- SCAN SUMMARY -----------
Known viruses:      6784116
Engine version:     0.100.2
Scanned directories: 201
Scanned files:      508
Infected files:     0
Data scanned:       1240.22 MB
Data read:          605.43 MB (ratio 2.05:1)
Time:               176.584 sec (2 m 56 s)
------------------------------------
```

---

## Provenance & Verification Analysis

| Metric | Recorded Value | Significance |
| :--- | :--- | :--- |
| **Known Virus Signatures** | 6,784,116 | Full production signature database coverage. |
| **Engine Version** | ClamAV 0.100.2 | Standard open-source Unix malware analysis engine. |
| **Scanned Directories** | 201 | Complete recursive traversal of all subfolders. |
| **Scanned Files** | 508 | Includes PDFs, source code snippets, scripts, and archives. |
| **Infected Files** | **0** | Clean baseline for security research and system study. |
| **Total Scanned Payload**| 1,240.22 MB | Uncompressed inspection across all document streams. |
| **Data Read (Decompressed)**| 605.43 MB | 2.05:1 compression ratio representing packed PDFs. |
| **Scan Duration** | 176.584 seconds | Comprehensive high-depth heuristics inspection. |

---

## Archive Inventory Overview

The collection consists of two primary language distributions:
1. **`base` (English Edition)**:
   - Volume: ~90 core whitepapers and technical guides (~223 MB compressed)
   - Scope: Linux systems performance, eBPF tracing, Windows NT kernel internals, x86 memory hierarchy, high-performance TCP/IP, SQL injection, binary buffer overflows, and Linux hardening benchmarks.
2. **`pl` (Polish Edition)**:
   - Volume: ~31 foundational whitepapers and manuals (~112 MB compressed)
   - Scope: Inżynieria wsteczna (reverse engineering), ptrace i rootkity, robaki i wirusy, bezpieczeństwo aplikacji WWW, SELinux, oraz audyt baz danych PostgreSQL.

---

## Related Notes
- [[README|Technical Whitepapers Master Map of Content]]
- [[base/Index-Base-English-Whitepapers|Base English Whitepapers Index]]
- [[pl/Index-Polish-Whitepapers|Polish Whitepapers Index]]
