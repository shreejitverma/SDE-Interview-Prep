---
type: playbook
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: [https://omscs.gatech.edu/cs-6210-advanced-operating-systems]
course: cs6210
tags: [cs6210, cs6210/plan]
---

# CS 6210 Study Plan

A week-by-week semester plan aligned with the Fall 2026 syllabus order, a two-week crash plan, an exam-prep routine, and a spaced-review schedule.
Tasks are plain checkboxes with Dataview inline fields (`[week:: n]`, `[due:: date]`), so the Tasks and Dataview plugins can query them without emoji signifiers.

> [!tip] Weekly rhythm (about 12-15 hours)
> 1. Watch the lesson videos and write the lesson note's concept headings in your own words (4-5 h).
> 2. Read the papers for the lesson, required ones in full, partial ones by the named sections, and fill the paper notes (3-4 h).
> 3. Run the lab and do one "experiments to try" variation (2-3 h).
> 4. Do the lesson's practice set closed-book, then open the folded answers (1-2 h).
> 5. Spaced review of earlier lessons from the table below (1 h).

> [!warning] Course projects
> Projects are individual work under the Georgia Tech honor code: never post them publicly, and disclose any AI tool use as the syllabus requires.
> This plan only schedules them; the labs here teach the underlying ideas without solving them.

## Semester plan

### Week 1 (lessons due Fri Aug 28)

- [ ] Study [Refresher: Virtual Memory and Caches](Part-0-Refresher/R01-Virtual-Memory-and-Caches.md) [week:: 1] [due:: 2026-08-28]
- [ ] Study [Refresher: Architecture, Storage, and Networking](Part-0-Refresher/R02-Architecture-Storage-and-Networking.md) [week:: 1] [due:: 2026-08-28]
- [ ] Study [Refresher: Concurrency, the Kernel, and C](Part-0-Refresher/R03-Concurrency-and-the-Kernel.md) [week:: 1] [due:: 2026-08-28]
- [ ] Study [L01 Introduction to Advanced Operating Systems](Part-1-OS-Structure-and-Virtualization/L01-Introduction-to-AOS.md) [week:: 1] [due:: 2026-08-28]
- [ ] Run [lab-00-refresher](labs/lab-00-refresher/README.md) [week:: 1] [due:: 2026-08-28]
- [ ] Run [lab-01-syscall-and-context-switch](labs/lab-01-syscall-and-context-switch/README.md) [week:: 1] [due:: 2026-08-28]
- [ ] Practice set [Practice L01](Practice/Practice-L01.md) [week:: 1] [due:: 2026-08-28]
- [ ] Practice set [Practice R](Practice/Practice-R.md) [week:: 1] [due:: 2026-08-28]
- [ ] Take the diagnostic preparedness test from the official course page and fix weak spots [week:: 1] [due:: 2026-08-28]

### Week 2 (lessons due Fri Sep 04)

- [ ] Study [L02a OS Structure Overview](Part-1-OS-Structure-and-Virtualization/L02a-OS-Structure-Overview.md) [week:: 2] [due:: 2026-09-04]
- [ ] Study [L02b The SPIN Approach](Part-1-OS-Structure-and-Virtualization/L02b-SPIN-Approach.md) [week:: 2] [due:: 2026-09-04]
- [ ] Study [L02c The Exokernel Approach](Part-1-OS-Structure-and-Virtualization/L02c-Exokernel-Approach.md) [week:: 2] [due:: 2026-09-04]
- [ ] Study [L02d The L3 Microkernel Approach](Part-1-OS-Structure-and-Virtualization/L02d-L3-Microkernel-Approach.md) [week:: 2] [due:: 2026-09-04]
- [ ] Read [Extensibility, Safety and Performance in the SPIN Operating System](Papers/L02-SPIN.md) (required) [week:: 2] [due:: 2026-09-04]
- [ ] Read [Exokernel: An Operating System Architecture for Application-Level Resource Management](Papers/L02-Exokernel.md) (required) [week:: 2] [due:: 2026-09-04]
- [ ] Read [On Micro-Kernel Construction](Papers/L02-On-Microkernel-Construction.md) (required) [week:: 2] [due:: 2026-09-04]
- [ ] Read [Improved Address-Space Switching on Pentium Processors by Transparently Multiplexing User Address Spaces](Papers/L02-Improved-Address-Space-Switching.md) (self-study) [week:: 2] [due:: 2026-09-04]
- [ ] Run [lab-01-syscall-and-context-switch](labs/lab-01-syscall-and-context-switch/README.md) [week:: 2] [due:: 2026-09-04]
- [ ] Run [lab-02-extensibility](labs/lab-02-extensibility/README.md) [week:: 2] [due:: 2026-09-04]
- [ ] Practice set [Practice L02](Practice/Practice-L02.md) [week:: 2] [due:: 2026-09-04]

### Week 3 (lessons due Fri Sep 11)

- [ ] Study [L03a Introduction to Virtualization](Part-1-OS-Structure-and-Virtualization/L03a-Introduction-to-Virtualization.md) [week:: 3] [due:: 2026-09-11]
- [ ] Study [L03b Memory Virtualization](Part-1-OS-Structure-and-Virtualization/L03b-Memory-Virtualization.md) [week:: 3] [due:: 2026-09-11]
- [ ] Study [L03c CPU and Device Virtualization](Part-1-OS-Structure-and-Virtualization/L03c-CPU-and-Device-Virtualization.md) [week:: 3] [due:: 2026-09-11]
- [ ] Read [Xen and the Art of Virtualization](Papers/L03-Xen.md) (required) [week:: 3] [due:: 2026-09-11]
- [ ] Read [Memory Resource Management in VMware ESX Server](Papers/L03-VMware-ESX-Memory.md) (required) [week:: 3] [due:: 2026-09-11]
- [ ] Run [lab-03-virtualization](labs/lab-03-virtualization/README.md) [week:: 3] [due:: 2026-09-11]
- [ ] Practice set [Practice L03](Practice/Practice-L03.md) [week:: 3] [due:: 2026-09-11]
- [ ] Project 1 opens (VM scheduling in KVM): read the spec, do lab-03 first for libvirt fluency [week:: 3] [due:: 2026-09-11]

### Week 4 (lessons due Fri Sep 18)

- [ ] Study [L04a Shared Memory Machines](Part-2-Parallel-Systems/L04a-Shared-Memory-Machines.md) [week:: 4] [due:: 2026-09-18]
- [ ] Study [L04b Synchronization](Part-2-Parallel-Systems/L04b-Synchronization.md) [week:: 4] [due:: 2026-09-18]
- [ ] Study [L04c Barrier Synchronization](Part-2-Parallel-Systems/L04c-Barrier-Synchronization.md) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors](Papers/L04-MCS-Scalable-Synchronization.md) (required) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Lightweight Remote Procedure Call](Papers/L04-LRPC.md) (required) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling](Papers/L04-Cache-Affinity-Scheduling.md) (partial) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design](Papers/L04-Multithreaded-Chip-Multiprocessors.md) (required) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System](Papers/L04-Tornado.md) (required) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Corey: An Operating System for Many Cores](Papers/L04-Corey.md) (partial) [week:: 4] [due:: 2026-09-18]
- [ ] Read [Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors](Papers/L04-Cellular-Disco.md) (partial) [week:: 4] [due:: 2026-09-18]
- [ ] Run [lab-04-cache-coherence](labs/lab-04-cache-coherence/README.md) [week:: 4] [due:: 2026-09-18]
- [ ] Run [lab-05-spinlocks](labs/lab-05-spinlocks/README.md) [week:: 4] [due:: 2026-09-18]
- [ ] Run [lab-06-barriers](labs/lab-06-barriers/README.md) [week:: 4] [due:: 2026-09-18]

### Week 5 (lessons due Fri Sep 25)

- [ ] Study [L04d Lightweight RPC](Part-2-Parallel-Systems/L04d-Lightweight-RPC.md) [week:: 5] [due:: 2026-09-25]
- [ ] Study [L04e Scheduling](Part-2-Parallel-Systems/L04e-Scheduling.md) [week:: 5] [due:: 2026-09-25]
- [ ] Study [L04f Shared Memory Multiprocessor OS](Part-2-Parallel-Systems/L04f-Shared-Memory-Multiprocessor-OS.md) [week:: 5] [due:: 2026-09-25]
- [ ] Run [lab-07-rpc-costs](labs/lab-07-rpc-costs/README.md) [week:: 5] [due:: 2026-09-25]
- [ ] Run [lab-08-scheduling](labs/lab-08-scheduling/README.md) [week:: 5] [due:: 2026-09-25]
- [ ] Run [lab-09-scalable-structures](labs/lab-09-scalable-structures/README.md) [week:: 5] [due:: 2026-09-25]
- [ ] Practice set [Practice L04](Practice/Practice-L04.md) [week:: 5] [due:: 2026-09-25]
- [ ] Project 1 due Mon Sep 28 [week:: 5] [due:: 2026-09-25]

### Week 6 (lessons due Fri Oct 02)

- [ ] Study [L05a Distributed Systems Definitions](Part-3-Distributed-Systems/L05a-Distributed-Systems-Definitions.md) [week:: 6] [due:: 2026-10-02]
- [ ] Study [L05b Lamport Clocks](Part-3-Distributed-Systems/L05b-Lamport-Clocks.md) [week:: 6] [due:: 2026-10-02]
- [ ] Study [L05c Latency Limits](Part-3-Distributed-Systems/L05c-Latency-Limits.md) [week:: 6] [due:: 2026-10-02]
- [ ] Read [Time, Clocks, and the Ordering of Events in a Distributed System](Papers/L05-Time-Clocks-Ordering.md) (required) [week:: 6] [due:: 2026-10-02]
- [ ] Read [Limits to Low-Latency Communication on High-Speed Networks](Papers/L05-Limits-Low-Latency.md) (required) [week:: 6] [due:: 2026-10-02]
- [ ] Read [The x-Kernel: An Architecture for Implementing Network Protocols](Papers/L05-x-Kernel.md) (required) [week:: 6] [due:: 2026-10-02]
- [ ] Read [Active Networks: Vision and Reality: Lessons from a Capsule-based System](Papers/L05-Active-Networks-ANTS.md) (required) [week:: 6] [due:: 2026-10-02]
- [ ] Read [Building Reliable, High-Performance Communication Systems from Components](Papers/L05-Ensemble-Systems-from-Components.md) (required) [week:: 6] [due:: 2026-10-02]
- [ ] Read [Performance of the Firefly RPC](Papers/L05-Firefly-RPC.md) (partial) [week:: 6] [due:: 2026-10-02]
- [ ] Run [lab-10-clocks-and-mutex](labs/lab-10-clocks-and-mutex/README.md) [week:: 6] [due:: 2026-10-02]
- [ ] Run [lab-07-rpc-costs](labs/lab-07-rpc-costs/README.md) [week:: 6] [due:: 2026-10-02]
- [ ] Test 1 (Lessons 1-4) window Fri Oct 2 to Mon Oct 5: run the Part 1 and Part 2 exam-prep block below [week:: 6] [due:: 2026-10-02]
- [ ] Project 2 open (barrier synchronization): read the spec; lab-06 covers the theory only [week:: 6] [due:: 2026-10-02]

### Week 7 (lessons due Fri Oct 09)

- [ ] Study [L05d Active Networks](Part-3-Distributed-Systems/L05d-Active-Networks.md) [week:: 7] [due:: 2026-10-09]
- [ ] Study [L05e Systems from Components](Part-3-Distributed-Systems/L05e-Systems-from-Components.md) [week:: 7] [due:: 2026-10-09]
- [ ] Run [lab-11-network-latency](labs/lab-11-network-latency/README.md) [week:: 7] [due:: 2026-10-09]
- [ ] Run [lab-12-components](labs/lab-12-components/README.md) [week:: 7] [due:: 2026-10-09]
- [ ] Practice set [Practice L05](Practice/Practice-L05.md) [week:: 7] [due:: 2026-10-09]

### Week 8 (lessons due Fri Oct 16)

- [ ] Study [L06a Spring Operating System](Part-3-Distributed-Systems/L06a-Spring-Operating-System.md) [week:: 8] [due:: 2026-10-16]
- [ ] Study [L06b Java RMI](Part-3-Distributed-Systems/L06b-Java-RMI.md) [week:: 8] [due:: 2026-10-16]
- [ ] Study [L06c Enterprise Java Beans](Part-3-Distributed-Systems/L06c-Enterprise-Java-Beans.md) [week:: 8] [due:: 2026-10-16]
- [ ] Read [An Overview of the Spring System](Papers/L06-Spring-Overview.md) (required) [week:: 8] [due:: 2026-10-16]
- [ ] Read [Subcontract: A Flexible Base for Distributed Programming](Papers/L06-Subcontract.md) (required) [week:: 8] [due:: 2026-10-16]
- [ ] Read [A Distributed Object Model for the Java System](Papers/L06-Java-Distributed-Object-Model.md) (required) [week:: 8] [due:: 2026-10-16]
- [ ] Read [Performance and Scalability of EJB Applications](Papers/L06-EJB-Performance.md) (required) [week:: 8] [due:: 2026-10-16]
- [ ] Run [lab-13-distributed-objects](labs/lab-13-distributed-objects/README.md) [week:: 8] [due:: 2026-10-16]
- [ ] Practice set [Practice L06](Practice/Practice-L06.md) [week:: 8] [due:: 2026-10-16]

### Week 9 (lessons due Fri Oct 23)

- [ ] Study [L07a Global Memory Systems](Part-4-Distributed-Subsystems-and-Recovery/L07a-Global-Memory-Systems.md) [week:: 9] [due:: 2026-10-23]
- [ ] Study [L07b Distributed Shared Memory](Part-4-Distributed-Subsystems-and-Recovery/L07b-Distributed-Shared-Memory.md) [week:: 9] [due:: 2026-10-23]
- [ ] Read [Implementing Global Memory Management in a Workstation Cluster](Papers/L07-GMS.md) (required) [week:: 9] [due:: 2026-10-23]
- [ ] Read [TreadMarks: Shared Memory Computing on Networks of Workstations](Papers/L07-TreadMarks.md) (required) [week:: 9] [due:: 2026-10-23]
- [ ] Read [Serverless Network File Systems](Papers/L07-xFS-Serverless-NFS.md) (required) [week:: 9] [due:: 2026-10-23]
- [ ] Read [Coda: A Highly Available File System for a Distributed Workstation Environment](Papers/L07-Coda.md) (partial) [week:: 9] [due:: 2026-10-23]
- [ ] Run [lab-14-global-memory](labs/lab-14-global-memory/README.md) [week:: 9] [due:: 2026-10-23]
- [ ] Run [lab-15-dsm](labs/lab-15-dsm/README.md) [week:: 9] [due:: 2026-10-23]
- [ ] Project 2 due Mon Oct 19 [week:: 9] [due:: 2026-10-23]
- [ ] Project 3 opens (distributed service with gRPC) [week:: 9] [due:: 2026-10-23]

### Week 10 (lessons due Fri Oct 30)

- [ ] Study [L07c Distributed File Systems](Part-4-Distributed-Subsystems-and-Recovery/L07c-Distributed-File-Systems.md) [week:: 10] [due:: 2026-10-30]
- [ ] Run [lab-16-dfs](labs/lab-16-dfs/README.md) [week:: 10] [due:: 2026-10-30]
- [ ] Practice set [Practice L07](Practice/Practice-L07.md) [week:: 10] [due:: 2026-10-30]

### Week 11 (lessons due Fri Nov 06)

- [ ] Study [L09a Giant Scale Services](Part-5-Internet-Scale-Real-Time-and-Security/L09a-Giant-Scale-Services.md) [week:: 11] [due:: 2026-11-06]
- [ ] Study [L09b MapReduce](Part-5-Internet-Scale-Real-Time-and-Security/L09b-MapReduce.md) [week:: 11] [due:: 2026-11-06]
- [ ] Study [L09c Content Delivery Networks](Part-5-Internet-Scale-Real-Time-and-Security/L09c-Content-Delivery-Networks.md) [week:: 11] [due:: 2026-11-06]
- [ ] Read [MapReduce: Simplified Data Processing on Large Clusters](Papers/L09-MapReduce.md) (required) [week:: 11] [due:: 2026-11-06]
- [ ] Read [Lessons from Giant-Scale Services](Papers/L09-Giant-Scale-Services.md) (partial) [week:: 11] [due:: 2026-11-06]
- [ ] Read [Web Search for a Planet: The Google Cluster Architecture](Papers/L09-Web-Search-for-a-Planet.md) (partial) [week:: 11] [due:: 2026-11-06]
- [ ] Read [Democratizing Content Publication with Coral](Papers/L09-Coral.md) (required) [week:: 11] [due:: 2026-11-06]
- [ ] Read [Dynamo: Amazon's Highly Available Key-value Store](Papers/L09-Dynamo.md) (required) [week:: 11] [due:: 2026-11-06]
- [ ] Read [Unraveling the Web Services Web: An Introduction to SOAP, WSDL, and UDDI](Papers/L09-Web-Services-SOAP-WSDL-UDDI.md) (self-study) [week:: 11] [due:: 2026-11-06]
- [ ] Read [The Next Step in Web Services](Papers/L09-Next-Step-in-Web-Services.md) (self-study) [week:: 11] [due:: 2026-11-06]
- [ ] Run [lab-19-giant-scale](labs/lab-19-giant-scale/README.md) [week:: 11] [due:: 2026-11-06]
- [ ] Run [lab-20-mapreduce](labs/lab-20-mapreduce/README.md) [week:: 11] [due:: 2026-11-06]
- [ ] Run [lab-21-dht](labs/lab-21-dht/README.md) [week:: 11] [due:: 2026-11-06]
- [ ] Practice set [Practice L09](Practice/Practice-L09.md) [week:: 11] [due:: 2026-11-06]
- [ ] Test 2 (Lessons 5-7) window Fri Nov 6 to Mon Nov 9: run the Part 3 and L07 exam-prep block [week:: 11] [due:: 2026-11-06]
- [ ] Project 3 due Mon Nov 9; Project 4 (MapReduce framework) opens Nov 3 [week:: 11] [due:: 2026-11-06]

### Week 12 (lessons due Fri Nov 13)

- [ ] Study [L10a TS-Linux](Part-5-Internet-Scale-Real-Time-and-Security/L10a-TS-Linux.md) [week:: 12] [due:: 2026-11-13]
- [ ] Study [L10b Persistent Temporal Streams](Part-5-Internet-Scale-Real-Time-and-Security/L10b-Persistent-Temporal-Streams.md) [week:: 12] [due:: 2026-11-13]
- [ ] Read [Supporting Time-Sensitive Applications on a Commodity OS](Papers/L10-Time-Sensitive-Commodity-OS.md) (required) [week:: 12] [due:: 2026-11-13]
- [ ] Read [Virtualize Everything but Time](Papers/L10-Virtualize-Everything-but-Time.md) (required) [week:: 12] [due:: 2026-11-13]
- [ ] Read [Persistent Temporal Streams](Papers/L10-Persistent-Temporal-Streams.md) (required) [week:: 12] [due:: 2026-11-13]
- [ ] Read [Yima: A Second-Generation Continuous Media Server](Papers/L10-Yima.md) (required) [week:: 12] [due:: 2026-11-13]
- [ ] Run [lab-22-realtime](labs/lab-22-realtime/README.md) [week:: 12] [due:: 2026-11-13]
- [ ] Run [lab-23-temporal-streams](labs/lab-23-temporal-streams/README.md) [week:: 12] [due:: 2026-11-13]
- [ ] Practice set [Practice L10](Practice/Practice-L10.md) [week:: 12] [due:: 2026-11-13]

### Week 13 (lessons due Fri Nov 20)

- [ ] Study [L08a Lightweight Recoverable Virtual Memory](Part-4-Distributed-Subsystems-and-Recovery/L08a-Lightweight-Recoverable-Virtual-Memory.md) [week:: 13] [due:: 2026-11-20]
- [ ] Study [L08b RioVista](Part-4-Distributed-Subsystems-and-Recovery/L08b-RioVista.md) [week:: 13] [due:: 2026-11-20]
- [ ] Study [L08c Quicksilver](Part-4-Distributed-Subsystems-and-Recovery/L08c-Quicksilver.md) [week:: 13] [due:: 2026-11-20]
- [ ] Read [Lightweight Recoverable Virtual Memory](Papers/L08-LRVM.md) (required) [week:: 13] [due:: 2026-11-20]
- [ ] Read [Free Transactions with Rio Vista](Papers/L08-Rio-Vista.md) (required) [week:: 13] [due:: 2026-11-20]
- [ ] Read [Recovery Management in QuickSilver](Papers/L08-Quicksilver.md) (required) [week:: 13] [due:: 2026-11-20]
- [ ] Read [The Recovery Manager of the System R Database Manager](Papers/L08-System-R-Recovery-Manager.md) (self-study) [week:: 13] [due:: 2026-11-20]
- [ ] Read [Operating System Transactions](Papers/L08-OS-Transactions.md) (partial) [week:: 13] [due:: 2026-11-20]
- [ ] Read [Large-scale Incremental Processing Using Distributed Transactions and Notifications](Papers/L08-Percolator.md) (partial) [week:: 13] [due:: 2026-11-20]
- [ ] Run [lab-17-recoverable-memory](labs/lab-17-recoverable-memory/README.md) [week:: 13] [due:: 2026-11-20]
- [ ] Run [lab-18-transactions](labs/lab-18-transactions/README.md) [week:: 13] [due:: 2026-11-20]
- [ ] Practice set [Practice L08](Practice/Practice-L08.md) [week:: 13] [due:: 2026-11-20]

### Week 14 (lessons due Fri Nov 27)

- [ ] Study [L11a Principles of Information Security](Part-5-Internet-Scale-Real-Time-and-Security/L11a-Principles-of-Information-Security.md) [week:: 14] [due:: 2026-11-27]
- [ ] Study [L11b Security in the Andrew System](Part-5-Internet-Scale-Real-Time-and-Security/L11b-Security-in-the-Andrew-System.md) [week:: 14] [due:: 2026-11-27]
- [ ] Read [The Protection of Information in Computer Systems](Papers/L11-Protection-Control-of-Information.md) (required) [week:: 14] [due:: 2026-11-27]
- [ ] Read [Integrating Security in a Large Distributed System](Papers/L11-Andrew-Security.md) (required) [week:: 14] [due:: 2026-11-27]
- [ ] Run [lab-24-security](labs/lab-24-security/README.md) [week:: 14] [due:: 2026-11-27]
- [ ] Practice set [Practice L11](Practice/Practice-L11.md) [week:: 14] [due:: 2026-11-27]

### Week 15 (lessons due Fri Dec 04)

- [ ] Project 4 work; consolidate Part 4 and Part 5 cheat sheets [week:: 15] [due:: 2026-12-04]

### Week 16 (lessons due Mon Dec 07)

- [ ] Project 4 due Mon Dec 7 [week:: 16] [due:: 2026-12-07]

### Week 17 (lessons due Fri Dec 11)

- [ ] Test 3 (Lessons 8-11) window Fri Dec 11 to Mon Dec 14: run the Part 4 (L08) and Part 5 exam-prep block [week:: 17] [due:: 2026-12-11]

## Exam-prep block (repeat before each test)

1. Rebuild each Part cheat sheet from memory on one page, then diff it against the notes.
2. Redo every practice question of the tested lessons closed-book, timed at about 2 minutes per point.
3. For every paper in scope, say the problem, key idea, and one number from the evaluation out loud in 60 seconds.
4. Draw the classic diagrams from memory: MCS lock queue, tournament and dissemination barriers, LRPC A-stack, GMS page-fault cases, TreadMarks twins and diffs, LRVM log, DQ curve, Coral routing.
5. Work released exam questions with your study group only as the syllabus allows; keep any notes from that private.

## Two-week crash plan

| Day | Focus | Output |
| --- | --- | --- |
| 1 | Refresher + L01, L02a-b | Concept headings filled, lab-01 |
| 2 | L02c-d, L03a | Border-crossing cost table, lab-02 |
| 3 | L03b-c | Shadow versus nested paging diagram, lab-03 |
| 4 | L04a-b | Lock comparison table, lab-04 and lab-05 |
| 5 | L04c-d | Barrier comparison table, lab-06 and lab-07 |
| 6 | L04e-f | Scheduling policy table, lab-08 and lab-09 |
| 7 | Review Parts 1-2 | Practice L01-L04 closed-book |
| 8 | L05a-c | Lamport mutual exclusion trace, lab-10 |
| 9 | L05d-e, L06a-c | Middleware comparison, lab-11 to lab-13 |
| 10 | L07a-c | GMS cases and TreadMarks diagram, lab-14 to lab-16 |
| 11 | L08a-c | LRVM and RioVista comparison, lab-17 and lab-18 |
| 12 | L09a-c | DQ worked example, lab-19 to lab-21 |
| 13 | L10a-b, L11a-b | Timer table and AFS handshake, lab-22 to lab-24 |
| 14 | Review Parts 3-5 | Practice L05-L11 closed-book |

## Spaced review

Review each lesson 1, 3, 7, and 21 days after you first study it, using its folded practice answers as flashcards.

| Studied | Review on day +1 | +3 | +7 | +21 |
| --- | --- | --- | --- | --- |
| Week 1 (R01, R02, R03, L01) | 2026-08-29 | 2026-08-31 | 2026-09-04 | 2026-09-18 |
| Week 2 (L02a, L02b, L02c, L02d) | 2026-09-05 | 2026-09-07 | 2026-09-11 | 2026-09-25 |
| Week 3 (L03a, L03b, L03c) | 2026-09-12 | 2026-09-14 | 2026-09-18 | 2026-10-02 |
| Week 4 (L04a, L04b, L04c) | 2026-09-19 | 2026-09-21 | 2026-09-25 | 2026-10-09 |
| Week 5 (L04d, L04e, L04f) | 2026-09-26 | 2026-09-28 | 2026-10-02 | 2026-10-16 |
| Week 6 (L05a, L05b, L05c) | 2026-10-03 | 2026-10-05 | 2026-10-09 | 2026-10-23 |
| Week 7 (L05d, L05e) | 2026-10-10 | 2026-10-12 | 2026-10-16 | 2026-10-30 |
| Week 8 (L06a, L06b, L06c) | 2026-10-17 | 2026-10-19 | 2026-10-23 | 2026-11-06 |
| Week 9 (L07a, L07b) | 2026-10-24 | 2026-10-26 | 2026-10-30 | 2026-11-13 |
| Week 10 (L07c) | 2026-10-31 | 2026-11-02 | 2026-11-06 | 2026-11-20 |
| Week 11 (L09a, L09b, L09c) | 2026-11-07 | 2026-11-09 | 2026-11-13 | 2026-11-27 |
| Week 12 (L10a, L10b) | 2026-11-14 | 2026-11-16 | 2026-11-20 | 2026-12-04 |
| Week 13 (L08a, L08b, L08c) | 2026-11-21 | 2026-11-23 | 2026-11-27 | 2026-12-11 |
| Week 14 (L11a, L11b) | 2026-11-28 | 2026-11-30 | 2026-12-04 | 2026-12-18 |

## Tracking

Open plan tasks by week (Dataview):

```dataview
TASK
FROM "01-CS-Foundations/Operating-Systems/AOS/00-Study-Plan"
WHERE !completed
GROUP BY week
```
