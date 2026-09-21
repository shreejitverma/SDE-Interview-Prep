---
tags: [systems/performance, security/offensive, security/defensive, kernel/internals, memory/architecture, networking, type/index]
aliases: [Base English Whitepapers, Technical Whitepapers English, Base Papers Catalog]
status: evergreen
created: 2026-09-17
type: paper
track: [distinguished, sde]
level:
last_reviewed:
sources: []
---

# Base Directory — English Technical Whitepapers Catalog

> [!summary]
> Complete catalog of the **90 English technical whitepapers (~223 MB)** covering Linux systems performance, eBPF superpowers, OS kernel internals (Linux, Windows NT, xv6), x86 memory hierarchy, high-speed TCP/IP networking, binary exploitation, and production system hardening. All items have been verified clean via [[../ClamAV-Audit-Report|ClamAV]].

---

## 1. Systems Performance, eBPF & Tracing (Brendan Gregg & Systems Engineers)

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **BPF: Tracing and More** | Brendan Gregg | 2017 | eBPF kernel tracing, BCC, kprobes | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#1-bpf-tracing-and-more-2017\|Gregg Canon]] |
| **Container Performance Analysis** | Brendan Gregg | — | cgroups, namespaces, Docker CPU/mem limits | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#2-container-performance-analysis\|Gregg Canon]] |
| **From DTrace To Linux** | Brendan Gregg | 2014 | Tracing evolution, kprobes, ftrace, perf | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#3-from-dtrace-to-linux-2014\|Gregg Canon]] |
| **Linux 4.x Performance Using BPF Superpowers** | Brendan Gregg | 2016 | eBPF bytecode in Linux 4.x, off-CPU analysis | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#4-linux-4x-performance-using-bpf-superpowers-2016\|Gregg Canon]] |
| **Linux Instrumentation** | Ian Munsie | 2010 | Kernel tracepoints, perf events, sysfs | [[../01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation#1-linux-instrumentation-ian-munsie-2010\|Tracing Guide]] |
| **Linux Performance Tools** | Brendan Gregg | 2014 | `vmstat`, `iostat`, `mpstat`, `sar`, `perf` | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#5-linux-performance-tools-2014--2015\|Gregg Canon]] |
| **Linux Performance Analysis New Tools and Old Secrets** | Brendan Gregg | — | CPU profiling, latency measurement, flamegraphs | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#6-linux-performance-analysis-new-tools-and-old-secrets\|Gregg Canon]] |
| **Linux Profiling at Netflix** | Brendan Gregg | 2015 | Cloud scale profiling, Java/Node flamegraphs | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#7-linux-profiling-at-netflix-2015\|Gregg Canon]] |
| **Linux Systems Performance** | Brendan Gregg | 2016 | Hardware PMU counters, IPC, cache misses | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#8-linux-systems-performance-2016\|Gregg Canon]] |
| **Open Source Systems Performance** | Brendan Gregg | 2013 | Methodologies for production Linux tuning | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#9-open-source-systems-performance-2013\|Gregg Canon]] |
| **Performance Analysis: The USE Method** | Brendan Gregg | 2012 | Utilization, Saturation, Errors triage | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#10-performance-analysis-the-use-method-2012\|Gregg Canon]] |
| **Performance Checklists for SREs** | Brendan Gregg | 2016 | 60-second incident triage, runqueue, iowait | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#11-performance-checklists-for-sres-2016\|Gregg Canon]] |
| **Performance Methodologies for Production Systems** | Brendan Gregg | 2013 | Anti-patterns (streetlamp, blame-someone-else) | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#12-performance-methodologies-for-production-systems-2013\|Gregg Canon]] |
| **System Performance** | Brendan Gregg | 2013 | Enterprise performance engineering foundations | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#13-system-performance-treatise-2013\|Gregg Canon]] |
| **Linux Performance Tools** | Brendan Gregg | 2015 | Updated toolchain, multi-core observability | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#5-linux-performance-tools-2014--2015\|Gregg Canon]] |
| **Performance Analysis Superpowers with Linux eBPF** | Brendan Gregg | 2015 | In-kernel filtering, map aggregations | [[../01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon#14-performance-analysis-superpowers-with-linux-ebpf-2015\|Gregg Canon]] |
| **Use "strace" to Understand Your Shell** | Harald König | 2015 | Syscall inspection, file descriptors, pipes | [[../01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation#2-use-strace-to-understand-your-shell-harald-könig-2015\|Tracing Guide]] |
| **System call tracing overhead** | Jörg Zinke | 2009 | ptrace cost vs kprobes/eBPF overhead | [[../01-Systems-Performance-and-Tracing/Linux-Tracing-and-Instrumentation#3-system-call-tracing-overhead-jörg-zinke-2009\|Tracing Guide]] |

---

## 2. Operating Systems Architecture & Kernel Internals

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **The UNIX Time-Sharing System** | Dennis M. Ritchie & Ken Thompson | 1974 | Original Unix architecture, files-as-streams | [[../02-Operating-Systems-and-Kernels/Unix-and-Linux-Kernel-Foundations#1-the-unix-time-sharing-system-ritchie--thompson-1974\|OS Foundations]] |
| **xv6: a simple, Unix-like teaching OS** | Russ Cox, Frans Kaashoek, Robert Morris | 2012 | MIT teaching kernel, process tables, paging | [[../02-Operating-Systems-and-Kernels/OS-From-Scratch-and-Teaching-Kernels#1-xv6-a-simple-unix-like-teaching-operating-system\|Teaching Kernels]] |
| **Writing a Simple OS — from Scratch** | Nick Blundell | 2010 | Bootloaders, real mode to protected mode, GDT | [[../02-Operating-Systems-and-Kernels/OS-From-Scratch-and-Teaching-Kernels#2-writing-a-simple-operating-system--from-scratch\|Teaching Kernels]] |
| **The Linux Kernel Hackers' Guide** | Michael K. Johnson | 1995 | Linux architecture, kernel modules, interrupts | [[../02-Operating-Systems-and-Kernels/Unix-and-Linux-Kernel-Foundations#2-the-linux-kernel-hackers-guide-michael-k-johnson-1995\|OS Foundations]] |
| **Unreliable Guide To Hacking The Linux Kernel** | Paul Rusty Russell | 2000 | Kernel locking rules, usercopy, spinlocks | [[../02-Operating-Systems-and-Kernels/Unix-and-Linux-Kernel-Foundations#3-unreliable-guide-to-hacking-the-linux-kernel-rusty-russell-2000\|OS Foundations]] |
| **The Linux Scheduler: a Decade of Wasted Cores** | Lozi, David, Thomas, et al. | 2016 | CFS bugs, overload-on-wakeup, group imbalance | [[../02-Operating-Systems-and-Kernels/Unix-and-Linux-Kernel-Foundations#4-the-linux-scheduler-a-decade-of-wasted-cores-2016\|OS Foundations]] |
| **The Linux Kernel Hidden Inside Windows 10** | Alex Ionescu | 2016 | WSL1 architecture, Pico processes, LXCore | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#11-the-linux-kernel-hidden-inside-windows-10-alex-ionescu-2016\|Windows NT Guide]] |
| **Windows NT Alerts Design Note** | David N. Cutler | 1989 | Asynchronous Procedure Calls (APCs), alerts | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#1-windows-nt-alerts-design-note-david-n-cutler-1989\|Windows NT Guide]] |
| **Windows: A Software Engineering Odyssey** | Mark Lucovsky | — | Evolution of NT OS design, portability | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#2-windows-a-software-engineering-odyssey-mark-lucovsky\|Windows NT Guide]] |
| **Disk Subsystem Performance Analysis for Windows** | Microsoft Engineering | 2014 | Storport driver, I/O queue depth, StorAHCI | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#3-disk-subsystem-performance-analysis-for-windows-2014\|Windows NT Guide]] |
| **Windows Error Codes** | Microsoft Reference | 2015 | `NTSTATUS`, Win32 `HRESULT`, error facility codes | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#4-windows-error-codes-reference-2015\|Windows NT Guide]] |
| **Kernel Debugging with WinDbg** | Microsoft DDK Team | 2005 | Target breakdown, KD protocol, symbols, KDNET | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#5-kernel-debugging-with-windbg-2005\|Windows NT Guide]] |
| **WinDbg. From A to Z!** | Robert Kuster | 2007 | User-mode and kernel-mode WinDbg cheat sheet | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#6-windbg-from-a-to-z-robert-kuster-2007\|Windows NT Guide]] |
| **Windows Kernel Internals: Overview** | David B. Probert, Ph.D. | — | Executive, Kernel, HAL, object model | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Traps, Interrupts, Exceptions** | David B. Probert, Ph.D. | — | IDT, ISRs, DPCs, IRQL levels (PASSIVE to HIGH) | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Advance Virtual Memory** | David B. Probert, Ph.D. | — | Page tables, PFN database, Working Set trimming | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Cache Manager** | David B. Probert, Ph.D. | — | Virtual Block Driver, CcCopyRead, lazy writer | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: I/O Architecture** | David B. Probert, Ph.D. | — | IRP dispatching, completion routines, fast I/O | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Lightweight Procedure Calls** | David B. Probert, Ph.D. | — | LPC/ALPC ports, shared memory message passing | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: NTFS** | David B. Probert, Ph.D. | — | Master File Table (MFT), journaling, logging | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: NT Registry Implementation** | David B. Probert, Ph.D. | — | Hive structure, bin allocation, cell records | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Object Manager** | David B. Probert, Ph.D. | — | Object headers, handles, security descriptors | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |
| **Windows Kernel Internals: Synchronization Mechanisms** | David B. Probert, Ph.D. | — | Spinlocks, Mutexes, Fast Mutexes, Pushlocks | [[../02-Operating-Systems-and-Kernels/Windows-NT-Internals-Architecture#7-windows-kernel-internals-series-david-b-probert-phd\|Windows NT Guide]] |

---

## 3. Memory Architecture, Microarchitecture & Concurrency

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **What Every Programmer Should Know About Memory** | Ulrich Drepper | 2007 | L1/L2/L3 caches, associativity, NUMA, TLBs, MESI | [[../03-Memory-Architecture-and-Concurrency/Ulrich-Drepper-Memory-Architecture#core-architecture--mechanisms\|Drepper Guide]] |
| **Why Threads Are A Bad Idea (for most purposes)** | John Ousterhout | 1995 | Event-driven vs multi-threaded architectures | [[../03-Memory-Architecture-and-Concurrency/Concurrency-and-Threading-Debates#1-why-threads-are-a-bad-idea-john-ousterhout-1995\|Concurrency Guide]] |
| **Virtual Threads** | Elaine Cheong & Fred Reiss | 2000 | User-level thread scheduling and M:N runtimes | [[../03-Memory-Architecture-and-Concurrency/Concurrency-and-Threading-Debates#2-virtual-threads-elaine-cheong-and-fred-reiss-2000\|Concurrency Guide]] |
| **When to use splay trees** | Eric K. Lee & Charles U. Martel | 2007 | Self-adjusting trees, amortized analysis | [[../03-Memory-Architecture-and-Concurrency/Data-Structures-and-Memory-Opt#1-when-to-use-splay-trees-lee--martel-2007\|Data Structures Guide]] |
| **What Happens During a Join: CPU & Memory Effects**| Stefan Manegold, Peter Boncz, Martin Kersten | — | Cache-miss behavior in database hash/merge joins | [[../03-Memory-Architecture-and-Concurrency/Data-Structures-and-Memory-Opt#2-what-happens-during-a-join-manegold-boncz-kersten\|Data Structures Guide]] |
| **Using CUDA in Practice** | Klaus Mueller | — | GPU warp execution, memory coalescing, shared mem | [[../03-Memory-Architecture-and-Concurrency/Data-Structures-and-Memory-Opt#3-using-cuda-in-practice-klaus-mueller\|Data Structures Guide]] |

---

## 4. Networking, TCP & Diagnostics

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **TCP Fast Open** | Radhakrishnan, Cheng, Chu, Jain, Raghavan | 2011 | Zero-RTT TCP handshake, TFO cookie exchange | [[../04-Networking-and-Protocols/High-Performance-TCP-and-Networking#1-tcp-fast-open-radhakrishnan-et-al\|High-Perf TCP]] |
| **Speeding up Networking** | Van Jacobson & Bob Felderman | 2006 | Network stack overhead, netchannels, polling | [[../04-Networking-and-Protocols/High-Performance-TCP-and-Networking#2-speeding-up-networking-van-jacobson--bob-felderman-2006\|High-Perf TCP]] |
| **Using TCPDump, TCPTrace, & XPlot to Debug Network Problems**| Jason Zurawski | 2013 | Packet capture analysis, time-sequence plots | [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS#1-using-tcpdump-tcptrace--xplot-to-debug-network-problems-jason-zurawski-2013\|Diagnostics Guide]] |
| **Open source firewall tools: Iptables and PF** | Elvir Kuric | — | Linux netfilter vs OpenBSD Packet Filter | [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS#2-open-source-firewall-tools-iptables-and-pf-elvir-kuric\|Diagnostics Guide]] |
| **Network Security Hardening Guide v1.2** | Security Working Group | 2017 | BGP, TCP SYN cookies, ICMP suppression | [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS#3-network-security-hardening-guide-v12-2017\|Diagnostics Guide]] |
| **DDoS Handbook** | Incident Response Team | 2015 | Volumetric, protocol, and application DDoS | [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS#4-ddos-handbook-and-ddos-tutorial-krassimir-tzvetanov\|Diagnostics Guide]] |
| **DDoS Tutorial** | Krassimir Tzvetanov | — | Anycast routing, scrubbing centers, rate limiting | [[../04-Networking-and-Protocols/Network-Diagnostics-and-DDoS#4-ddos-handbook-and-ddos-tutorial-krassimir-tzvetanov\|Diagnostics Guide]] |

---

## 5. Offensive Security, Exploitation & Vulnerability Research

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **Reverse Engineering for Beginners** | Dennis Yurichev | 2013 | Disassembly, IDA Pro, x86/x64 calling conventions | [[../05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng#1-reverse-engineering-for-beginners-dennis-yurichev-2013\|Binary Exploitation]] |
| **A Unique Examination of the Buffer Overflow Condition**| Terry Bruce Gillette | 2002 | Stack layout, EIP overwrite, NOP sleds | [[../05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng#2-a-unique-examination-of-the-buffer-overflow-condition-terry-bruce-gillette-2002\|Binary Exploitation]] |
| **Buffer overflow vulnerabilities** | Peter Buchlovsky & Adam Butcher | — | Shellcode crafting, off-by-one, format strings | [[../05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng#3-buffer-overflow-vulnerabilities-peter-buchlovsky--adam-butcher\|Binary Exploitation]] |
| **PE File Infection Techniques** | Konstantin Rozinov | — | Portable Executable header, code caves, packing | [[../05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng#4-pe-file-infection-techniques-konstantin-rozinov\|Binary Exploitation]] |
| **Tracing Privileged Memory Accesses to Discover Software Vulnerabilities**| Felix Wilhelm | 2015 | Hardware-assisted tracing, kernel vulnerability search | [[../05-Offensive-Security-and-Exploitation/Binary-Exploitation-and-Reverse-Eng#5-tracing-privileged-memory-accesses-felix-wilhelm-2015\|Binary Exploitation]] |
| **Advanced SQL Injection In SQL Server Applications**| Chris Anley | 2002 | `xp_cmdshell`, extended procs, out-of-band exfiltration | [[../05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection#1-advanced-sql-injection-in-sql-server-applications-chris-anley-2002\|SQL Injection Guide]] |
| **Blindfolded SQL Injection** | Ofer Maor & Amichai Shulman | — | Inference via content variations, error reflection | [[../05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection#2-blindfolded-sql-injection-ofer-maor--amichai-shulman\|SQL Injection Guide]] |
| **Blind SQL Injection** | Kevin Spett | — | Time-based SQLi, boolean-based SQLi | [[../05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection#3-blind-sql-injection-kevin-spett\|SQL Injection Guide]] |
| **Manipulating Microsoft SQL Server Using SQL Injection**| Cesar Cerrudo | — | Registry access via SQL, privilege escalation | [[../05-Offensive-Security-and-Exploitation/Database-Exploitation-and-SQL-Injection#4-manipulating-microsoft-sql-server-using-sql-injection-cesar-cerrudo\|SQL Injection Guide]] |
| **Decimalisation Table Attacks for PIN Cracking** | Mike Bond & Piotr Zieliński | — | Hardware Security Module (HSM) oracle attack | [[../05-Offensive-Security-and-Exploitation/Vulnerability-Research-and-Web-Exploits#1-decimalisation-table-attacks-for-pin-cracking-mike-bond--piotr-zieliński\|Vulnerability Research]] |
| **The Return of Robin Hood vs Cisco ASA** | Cedric Halbronn | 2018 | Cisco ASA firmware exploit, memory corruption | [[../05-Offensive-Security-and-Exploitation/Vulnerability-Research-and-Web-Exploits#2-the-return-of-robin-hood-vs-cisco-asa-cedric-halbronn-2018\|Vulnerability Research]] |
| **Bypassing Same Origin Policy v1.0** | Simon Egli | — | CORS misconfiguration, `document.domain`, Flash | [[../05-Offensive-Security-and-Exploitation/Vulnerability-Research-and-Web-Exploits#3-bypassing-same-origin-policy-simon-egli\|Vulnerability Research]] |
| **Checklist for Penetration Testing** | Mateus Felipe Tymburibá Ferreira | 2012 | PTES methodologies, reconnaissance, exploitation | [[../05-Offensive-Security-and-Exploitation/Vulnerability-Research-and-Web-Exploits#4-checklist-for-penetration-testing-mateus-ferreira-2012\|Vulnerability Research]] |

---

## 6. Defensive Security, Hardening & Compliance

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **Linux Hardening** | Michael Boelen | 2016 | Lynis author guide to enterprise Linux baseline | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#1-michael-boelen-linux-security-trilogy-2016\|Linux Hardening]] |
| **Linux Systems Compromised** | Michael Boelen | 2016 | Post-exploitation forensic triage, rootkits | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#1-michael-boelen-linux-security-trilogy-2016\|Linux Hardening]] |
| **Linux Security for Developers** | Michael Boelen | 2016 | Defensive programming, capabilities, seccomp | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#1-michael-boelen-linux-security-trilogy-2016\|Linux Hardening]] |
| **Securing & Hardening Linux v1.0** | Charalambous Glafkos | 2007 | Sysctl hardening, PAM, SSH configuration | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#2-securing--hardening-linux-charalambous-glafkos-2007\|Linux Hardening]] |
| **Hardened kernels for everyone** | Yves-Alexis Perez | 2015 | grsecurity/PaX, KSPP, ASLR, stack canaries | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#3-hardened-kernels-for-everyone-yves-alexis-perez-2015\|Linux Hardening]] |
| **Establishing, Implementing and Auditing Linux OS Hardening Standard**| Martin Jõgi | 2017 | CIS Benchmark automation, compliance audit | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#4-establishing-implementing-and-auditing-linux-hardening-martin-jõgi-2017\|Linux Hardening]] |
| **SELinux Policy Management Framework for HIS** | Luis Franco Marin | 2008 | Mandatory Access Control (MAC), Type Enforcement | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#5-selinux-policy-management-framework-luis-franco-marin-2008\|Linux Hardening]] |
| **A SysAdmin’s Essential Guide to Linux Workstation Security**| Security Research | — | Desktop/workstation encryption, USB guards | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#6-a-sysadmins-essential-guide-to-linux-workstation-security\|Linux Hardening]] |
| **Linux Security Review** | Independent Security Group | 2015 | Kernel vulnerabilities, CVE statistics, auditd | [[../06-Defensive-Security-and-Hardening/Linux-Operating-System-Hardening#7-linux-security-review-2015\|Linux Hardening]] |
| **Linux Security and the Chromium Sandbox** | Pati Gallardo | 2018 | Namespaces, chroot, seccomp-bpf in Chromium | [[../06-Defensive-Security-and-Hardening/Container-and-Microservice-Security#1-linux-security-and-the-chromium-sandbox-pati-gallardo-2018\|Container Security]] |
| **Docker and High Security Microservices** | Aaron Grattafiori | 2016 | Docker daemon attack surface, AppArmor, dropping caps | [[../06-Defensive-Security-and-Hardening/Container-and-Microservice-Security#2-docker-and-high-security-microservices-aaron-grattafiori-2016\|Container Security]] |
| **LXC, Docker, Security** | NCC Group Research | — | Container breakout vectors, shared kernel risks | [[../06-Defensive-Security-and-Hardening/Container-and-Microservice-Security#3-lxc-docker-security-ncc-group\|Container Security]] |
| **Application Security Verification Standard (ASVS) 3.0.1**| OWASP Foundation | 2016 | Verification levels (L1/L2/L3), security controls | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#1-owasp-application-security-verification-standard-asvs-301\|AppSec Guide]] |
| **OWASP Testing Guide v4** | Matteo Meucci & Andrew Muller | — | Web application penetration testing manual | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#2-owasp-testing-guide-v4-meucci--muller\|AppSec Guide]] |
| **Introduction to Application Security & OWASP Top 10**| Ralph Durkee | — | Threat modeling, secure SDLC, OWASP overview | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#3-introduction-to-application-security--owasp-top-10-ralph-durkee\|AppSec Guide]] |
| **Auditing Web Applications** | Robert Morella | 2015 | Source code review, dynamic scanning, auth logic | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#4-auditing-web-applications-robert-morella-2015\|AppSec Guide]] |
| **Security Configuration Benchmark For Apache** | Ryan Barnett | 2008 | CIS Apache benchmark, mod_security, SSL cipher suites | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#5-security-configuration-benchmark-for-apache-ryan-barnett-2008\|AppSec Guide]] |
| **Total security in a PostgreSQL database** | Robert Bernier | 2009 | Role-based access control, SSL connections, pg_hba.conf | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#6-total-security-in-a-postgresql-database-robert-bernier-2009\|AppSec Guide]] |
| **PostgreSQL Portland Performance Practice Project**| Mark Wong | 2009 | Shared buffers, checkpoint tuning, work_mem benchmarks | [[../06-Defensive-Security-and-Hardening/Application-and-Infrastructure-Sec#7-postgresql-portland-performance-practice-project-mark-wong-2009\|AppSec Guide]] |

---

## 7. Developer Tooling & SysAdmin Foundations

| Paper Title | Author(s) | Year | Domain / Focus | Reference Guide |
| :--- | :--- | :--- | :--- | :--- |
| **The AWK Programming Language** | Alfred V. Aho, Brian W. Kernighan, Peter J. Weinberg | 1988 | Stream processing, pattern-action, text filtering | [[../08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin#1-the-awk-programming-language-aho-kernighan-weinberg-1988\|Tooling Guide]] |
| **Vim for humans** | Vincent Jousse | 2015 | Modal editing, motions, buffers, productivity | [[../08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin#2-vim-for-humans-vincent-jousse-2015\|Tooling Guide]] |
| **DevOps Toolchain** | UpGuard Engineering | — | Automated configuration management, CI/CD pipelines | [[../08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin#3-devops-toolchain-upguard\|Tooling Guide]] |
| **SysAdmin Magazine: Tools & Tips for Security Admins**| SysAdmin Editorial | 2016 | Practical administration scripts, forensics, log auditing | [[../08-Developer-Tooling-and-Foundations/Developer-Tooling-and-SysAdmin#4-sysadmin-magazine-tools--tips-for-security-admins-2016\|Tooling Guide]] |

---

## Related Master Navigation
- [[../README|Master Map of Content (MOC)]]
- [[../ClamAV-Audit-Report|ClamAV Security Audit Report]]
- [[../pl/Index-Polish-Whitepapers|Polish Technical Whitepapers Catalog]]
