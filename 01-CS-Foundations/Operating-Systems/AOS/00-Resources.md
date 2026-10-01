---
type: playbook
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: [https://omscs.gatech.edu/cs-6210-advanced-operating-systems]
course: cs6210
tags: [cs6210]
---

# CS 6210 Resources

Everything you need besides the lecture videos: the official course pages, the full reading list with stable links, background books, related courses, and the Linux documentation each lab builds on.

## Official

- [CS 6210 course page](https://omscs.gatech.edu/cs-6210-advanced-operating-systems): overview, prerequisites, sample syllabi, the diagnostic preparedness test, and the prerequisites and concepts list.
- The Fall 2026 syllabus orders the lessons 1-7, 9, 10, 8, 11 and has three tests (L1-4, L5-7, L8-11), four projects (VM scheduling in KVM, barrier synchronization, a distributed service with gRPC, a MapReduce framework), a pre-lab, a homework, and two paper summaries.
- Hardware note from the course page: the course VM environment needs VT-x or AMD-V; Apple M1 and M2 are not supported. On Apple M3 or later, the [lab VM](labs/setup/README.md) uses nested virtualization instead.

## Reading list

Reading status follows the Fall 2026 syllabus. Optional papers are not tested.

| Lesson | Paper | Venue | Reading | Link |
| --- | --- | --- | --- | --- |
| L02 | [Extensibility, Safety and Performance in the SPIN Operating System](Papers/L02-SPIN.md) | SOSP 1995 | required | [link](https://doi.org/10.1145/224056.224077) |
| L02 | [Exokernel: An Operating System Architecture for Application-Level Resource Management](Papers/L02-Exokernel.md) | SOSP 1995 | required | [link](https://doi.org/10.1145/224056.224076) |
| L02 | [On Micro-Kernel Construction](Papers/L02-On-Microkernel-Construction.md) | SOSP 1995 | required | [link](https://doi.org/10.1145/224056.224075) |
| L02 | [Improved Address-Space Switching on Pentium Processors by Transparently Multiplexing User Address Spaces](Papers/L02-Improved-Address-Space-Switching.md) | GMD TR 933, 1995 | self-study | in the private library |
| L03 | [Xen and the Art of Virtualization](Papers/L03-Xen.md) | SOSP 2003 | required | [link](https://doi.org/10.1145/945445.945462) |
| L03 | [Memory Resource Management in VMware ESX Server](Papers/L03-VMware-ESX-Memory.md) | OSDI 2002 | required | [link](https://www.usenix.org/legacy/event/osdi02/tech/waldspurger.html) |
| L04 | [Algorithms for Scalable Synchronization on Shared-Memory Multiprocessors](Papers/L04-MCS-Scalable-Synchronization.md) | TOCS 1991 | required | [link](https://doi.org/10.1145/103727.103729) |
| L04 | [Lightweight Remote Procedure Call](Papers/L04-LRPC.md) | TOCS 1990 | required | [link](https://doi.org/10.1145/77648.77650) |
| L04 | [Using Processor-Cache Affinity Information in Shared Memory Multiprocessor Scheduling](Papers/L04-Cache-Affinity-Scheduling.md) | IEEE TPDS 1993 | partial | [link](https://doi.org/10.1109/71.207589) |
| L04 | [Performance of Multithreaded Chip Multiprocessors and Implications for Operating System Design](Papers/L04-Multithreaded-Chip-Multiprocessors.md) | USENIX ATC 2005 | required | [link](https://www.usenix.org/conference/2005-usenix-annual-technical-conference/performance-multithreaded-chip-multiprocessors-and) |
| L04 | [Tornado: Maximizing Locality and Concurrency in a Shared Memory Multiprocessor Operating System](Papers/L04-Tornado.md) | OSDI 1999 | required | [link](https://www.usenix.org/conference/osdi-99/tornado-maximizing-locality-and-concurrency-shared-memory-multiprocessor-operating) |
| L04 | [Corey: An Operating System for Many Cores](Papers/L04-Corey.md) | OSDI 2008 | partial | [link](https://www.usenix.org/conference/osdi-08/corey-operating-system-many-cores) |
| L04 | [Cellular Disco: Resource Management Using Virtual Clusters on Shared-Memory Multiprocessors](Papers/L04-Cellular-Disco.md) | SOSP 1999 | partial | [link](https://doi.org/10.1145/319151.319162) |
| L05 | [Time, Clocks, and the Ordering of Events in a Distributed System](Papers/L05-Time-Clocks-Ordering.md) | CACM 1978 | required | [link](https://doi.org/10.1145/359545.359563) |
| L05 | [Limits to Low-Latency Communication on High-Speed Networks](Papers/L05-Limits-Low-Latency.md) | TOCS 1993 | required | [link](https://doi.org/10.1145/151244.151247) |
| L05 | [The x-Kernel: An Architecture for Implementing Network Protocols](Papers/L05-x-Kernel.md) | IEEE TSE 1991 | required | [link](https://doi.org/10.1109/32.67579) |
| L05 | [Active Networks: Vision and Reality: Lessons from a Capsule-based System](Papers/L05-Active-Networks-ANTS.md) | SOSP 1999 | required | [link](https://doi.org/10.1145/319151.319156) |
| L05 | [Building Reliable, High-Performance Communication Systems from Components](Papers/L05-Ensemble-Systems-from-Components.md) | SOSP 1999 | required | [link](https://doi.org/10.1145/319151.319157) |
| L05 | [Performance of the Firefly RPC](Papers/L05-Firefly-RPC.md) | SOSP 1989 | partial | [link](https://doi.org/10.1145/74850.74859) |
| L06 | [An Overview of the Spring System](Papers/L06-Spring-Overview.md) | Compcon 1994 | required | [link](https://doi.org/10.1109/CMPCON.1994.282935) |
| L06 | [Subcontract: A Flexible Base for Distributed Programming](Papers/L06-Subcontract.md) | SOSP 1993 | required | [link](https://doi.org/10.1145/168619.168625) |
| L06 | [A Distributed Object Model for the Java System](Papers/L06-Java-Distributed-Object-Model.md) | USENIX COOTS 1996 | required | [link](https://www.usenix.org/legacy/publications/library/proceedings/coots96/wollrath.html) |
| L06 | [Performance and Scalability of EJB Applications](Papers/L06-EJB-Performance.md) | OOPSLA 2002 | required | [link](https://doi.org/10.1145/582419.582443) |
| L07 | [Implementing Global Memory Management in a Workstation Cluster](Papers/L07-GMS.md) | SOSP 1995 | required | [link](https://doi.org/10.1145/224056.224072) |
| L07 | [TreadMarks: Shared Memory Computing on Networks of Workstations](Papers/L07-TreadMarks.md) | IEEE Computer 1996 | required | [link](https://doi.org/10.1109/2.485843) |
| L07 | [Serverless Network File Systems](Papers/L07-xFS-Serverless-NFS.md) | TOCS 1996 | required | [link](https://doi.org/10.1145/225535.225537) |
| L07 | [Coda: A Highly Available File System for a Distributed Workstation Environment](Papers/L07-Coda.md) | IEEE Trans. Computers 1990 | partial | [link](https://doi.org/10.1109/12.54838) |
| L08 | [Lightweight Recoverable Virtual Memory](Papers/L08-LRVM.md) | SOSP 1993 | required | [link](https://doi.org/10.1145/168619.168631) |
| L08 | [Free Transactions with Rio Vista](Papers/L08-Rio-Vista.md) | SOSP 1997 | required | [link](https://doi.org/10.1145/268998.266665) |
| L08 | [Recovery Management in QuickSilver](Papers/L08-Quicksilver.md) | TOCS 1988 | required | [link](https://doi.org/10.1145/35037.35060) |
| L08 | [The Recovery Manager of the System R Database Manager](Papers/L08-System-R-Recovery-Manager.md) | ACM Computing Surveys 1981 | self-study | [link](https://doi.org/10.1145/356842.356847) |
| L08 | [Operating System Transactions](Papers/L08-OS-Transactions.md) | SOSP 2009 | partial | [link](https://doi.org/10.1145/1629575.1629591) |
| L08 | [Large-scale Incremental Processing Using Distributed Transactions and Notifications](Papers/L08-Percolator.md) | OSDI 2010 | partial | [link](https://www.usenix.org/conference/osdi10/large-scale-incremental-processing-using-distributed-transactions-and) |
| L09 | [MapReduce: Simplified Data Processing on Large Clusters](Papers/L09-MapReduce.md) | OSDI 2004 | required | [link](https://www.usenix.org/conference/osdi-04/mapreduce-simplified-data-processing-large-clusters) |
| L09 | [Lessons from Giant-Scale Services](Papers/L09-Giant-Scale-Services.md) | IEEE Internet Computing 2001 | partial | [link](https://doi.org/10.1109/4236.939450) |
| L09 | [Web Search for a Planet: The Google Cluster Architecture](Papers/L09-Web-Search-for-a-Planet.md) | IEEE Micro 2003 | partial | [link](https://doi.org/10.1109/MM.2003.1196112) |
| L09 | [Democratizing Content Publication with Coral](Papers/L09-Coral.md) | NSDI 2004 | required | [link](https://www.usenix.org/conference/nsdi-04/democratizing-content-publication-coral) |
| L09 | [Dynamo: Amazon's Highly Available Key-value Store](Papers/L09-Dynamo.md) | SOSP 2007 | required | [link](https://doi.org/10.1145/1294261.1294281) |
| L09 | [Unraveling the Web Services Web: An Introduction to SOAP, WSDL, and UDDI](Papers/L09-Web-Services-SOAP-WSDL-UDDI.md) | IEEE Internet Computing 2002 | self-study | [link](https://doi.org/10.1109/4236.991449) |
| L09 | [The Next Step in Web Services](Papers/L09-Next-Step-in-Web-Services.md) | CACM 2003 | self-study | [link](https://doi.org/10.1145/944217.944234) |
| L10 | [Supporting Time-Sensitive Applications on a Commodity OS](Papers/L10-Time-Sensitive-Commodity-OS.md) | OSDI 2002 | required | [link](https://www.usenix.org/conference/osdi-02/supporting-time-sensitive-applications-commodity-os) |
| L10 | [Virtualize Everything but Time](Papers/L10-Virtualize-Everything-but-Time.md) | OSDI 2010 | required | [link](https://www.usenix.org/conference/osdi10/virtualize-everything-time) |
| L10 | [Persistent Temporal Streams](Papers/L10-Persistent-Temporal-Streams.md) | Middleware 2009 | required | [link](https://doi.org/10.1007/978-3-642-10445-9_17) |
| L10 | [Yima: A Second-Generation Continuous Media Server](Papers/L10-Yima.md) | IEEE Computer 2002 | required | [link](https://doi.org/10.1109/MC.2002.1009169) |
| L11 | [The Protection of Information in Computer Systems](Papers/L11-Protection-Control-of-Information.md) | Proceedings of the IEEE 1975 | required | [link](https://doi.org/10.1109/PROC.1975.9939) |
| L11 | [Integrating Security in a Large Distributed System](Papers/L11-Andrew-Security.md) | TOCS 1989 | required | [link](https://doi.org/10.1145/65000.65002) |
| optional | [Protection in the HYDRA Operating System](Papers/Optional-HYDRA-Protection.md) | SOSP 1975 | optional | [link](https://doi.org/10.1145/800213.806532) |
| optional | [Scalability Study of the KSR-1](Papers/Optional-KSR-1-Scalability.md) | Parallel Computing 1996 | optional | [link](https://doi.org/10.1016/0167-8191(96)00021-X) |
| optional | [The Multikernel: A New OS Architecture for Scalable Multicore Systems](Papers/Optional-Multikernel.md) | SOSP 2009 | optional | [link](https://doi.org/10.1145/1629575.1629579) |
| optional | [VirtualPower: Coordinated Power Management in Virtualized Enterprise Systems](Papers/Optional-Virtual-Power.md) | SOSP 2007 | optional | [link](https://doi.org/10.1145/1294261.1294287) |
| optional | [Protection and Communication Abstractions for Web Browsers in MashupOS](Papers/Optional-MashupOS.md) | SOSP 2007 | optional | [link](https://doi.org/10.1145/1294261.1294263) |
| optional | [Trust and Protection in the Illinois Browser Operating System](Papers/Optional-Illinois-Browser-OS.md) | OSDI 2010 | optional | [link](https://www.usenix.org/conference/osdi10/trust-and-protection-illinois-browser-operating-system) |
| optional | [Cluster-Based Scalable Network Services](Papers/Optional-Cluster-Based-Network-Services.md) | SOSP 1997 | optional | [link](https://doi.org/10.1145/268998.266662) |
| optional | [Manageability, Availability, and Performance in Porcupine: A Highly Scalable, Cluster-based Mail Service](Papers/Optional-Porcupine.md) | SOSP 1999 | optional | [link](https://doi.org/10.1145/319151.319152) |
| optional | [Improving MapReduce Performance in Heterogeneous Environments](Papers/Optional-LATE-MapReduce-Heterogeneous.md) | OSDI 2008 | optional | [link](https://www.usenix.org/conference/osdi-08/improving-mapreduce-performance-heterogeneous-environments) |
| optional | [Mach: A New Kernel Foundation for UNIX Development](Papers/Optional-Mach.md) | USENIX Summer 1986 | optional | in the private library |
| optional | [Finding a Needle in Haystack: Facebook's Photo Storage](Papers/Optional-Haystack.md) | OSDI 2010 | optional | [link](https://www.usenix.org/conference/osdi10/finding-needle-haystack-facebooks-photo-storage) |

## Background books

- [Operating Systems: Three Easy Pieces](https://pages.cs.wisc.edu/~remzi/OSTEP/) (Arpaci-Dusseau): the refresher topics, free online.
- Hennessy and Patterson, *Computer Architecture: A Quantitative Approach*: caches, coherence, and memory consistency for L04.
- Nagarajan, Sorin, Hill, Wood, *A Primer on Memory Consistency and Cache Coherence* (2nd ed.): the precise version of L04a.
- Michael L. Scott, *Shared-Memory Synchronization*: the lock and barrier algorithms of L04b-c, by an author of the MCS paper.
- Paul McKenney, [Is Parallel Programming Hard, And, If So, What Can You Do About It?](https://mirrors.edge.kernel.org/pub/linux/kernel/people/paulmck/perfbook/perfbook.html): per-CPU data, RCU, and scalable kernel structures for L04f.
- Martin Kleppmann, *Designing Data-Intensive Applications*: clocks, replication, quorums, and transactions for L05, L08, and L09.
- Tanenbaum and van Steen, *Distributed Systems*: logical clocks, distributed objects, and consistency models for L05-L07.

## Related courses

- [MIT 6.5840 Distributed Systems](https://pdos.csail.mit.edu/6.824/): MapReduce, Raft, and key-value stores; good extra practice for L05 and L09.
- [CMU 15-440 Distributed Systems](https://www.cs.cmu.edu/~15-440/): RPC, distributed file systems, and consistency.
- [GIOS (CS 6200) notes in this vault](../GIOS/_GIOS-Dashboard.md): the introductory course this one builds on.

## Linux documentation used by the labs

| Topic | Documentation |
| --- | --- |
| KVM | [KVM docs](https://docs.kernel.org/virt/kvm/index.html), [virsh manual](https://libvirt.org/manpages/virsh.html) |
| Page sharing | [Kernel samepage merging](https://docs.kernel.org/admin-guide/mm/ksm.html) |
| Application-level paging | [userfaultfd](https://docs.kernel.org/admin-guide/mm/userfaultfd.html) |
| Locking | [Kernel locking](https://docs.kernel.org/locking/index.html), [RCU](https://docs.kernel.org/RCU/index.html), [Rochester synchronization pseudocode](https://www.cs.rochester.edu/research/synchronization/pseudocode/ss.html) |
| Scheduling | [CFS design](https://docs.kernel.org/scheduler/sched-design-CFS.html), [SCHED_DEADLINE](https://docs.kernel.org/scheduler/sched-deadline.html) |
| Extensibility | [BPF](https://docs.kernel.org/bpf/index.html), [bpftrace](https://bpftrace.org/) |
| File systems | [FUSE](https://docs.kernel.org/filesystems/fuse/index.html), [libfuse](https://github.com/libfuse/libfuse) |
| Security | [seccomp filters](https://docs.kernel.org/userspace-api/seccomp_filter.html), [seL4](https://sel4.systems/) |
| Performance | [Brendan Gregg's perf examples](https://brendangregg.com/perf.html) |
| RPC and messaging | [gRPC docs](https://grpc.io/docs/), [Open MPI docs](https://www.open-mpi.org/doc/) |
| Lab VM | [Lima docs](https://lima-vm.io/docs/) |

## Private material

Lecture slides, paper PDFs, course review questions, and past tests are third-party or course material.
They live in the private library (`Library/01-CS-Foundations/Operating-Systems/CS6210-AOS/` in this vault, which is a local link to the private `career-ops` repository) and are never committed here.
Text extracts under `text/` there are for local search and study only.
