---
type: moc
track: [sde, distinguished]
level:
status: draft
last_reviewed:
sources: []
course: cs6210
tags: [cs6210]
---

# AOS Labs

Hands-on labs for every lesson: real commands, C and Python programs, and measurements.
Every lab runs in the Lima VM described in [setup](setup/README.md) and has `make`, `make run`, and `make test` targets.

> [!warning] Honor code
> These labs are original study material, not course project solutions.
> Labs that touch a project topic stop at observation, library primitives, or simulation; see each lab's guard note.

| Lab | Topic | Lessons | Guard |
| --- | --- | --- | --- |
| [lab-00-refresher](lab-00-refresher/README.md) | Refresher: page faults, TLB, caches, and a pthreads producer-consumer | R01, R02, R03 | - |
| [lab-01-syscall-and-context-switch](lab-01-syscall-and-context-switch/README.md) | Border crossings: syscall, context switch, and address space switch costs | L01, L02a, L02d | - |
| [lab-02-extensibility](lab-02-extensibility/README.md) | Safe extensibility today: bpftrace and eBPF, plus application-level paging with userfaultfd | L02b, L02c | - |
| [lab-03-virtualization](lab-03-virtualization/README.md) | KVM and libvirt by hand: lifecycle, vCPU pinning, ballooning, and KSM page sharing | L03a, L03b, L03c | observe and operate only; never write a vCPU scheduler or memory coordinator (Project 1) |
| [lab-04-cache-coherence](lab-04-cache-coherence/README.md) | Coherence traffic, false sharing, and memory-ordering litmus tests | L04a | - |
| [lab-05-spinlocks](lab-05-spinlocks/README.md) | Spinlock zoo: TAS, TTAS, backoff, ticket, Anderson, and MCS in C11 | L04b | - |
| [lab-06-barriers](lab-06-barriers/README.md) | Barrier algorithms: library barrier costs and a round and message simulator | L04c | no C, OpenMP, or MPI implementation of any barrier algorithm (Project 2) |
| [lab-07-rpc-costs](lab-07-rpc-costs/README.md) | Where RPC time goes: copies, crossings, and zero-copy on one machine | L04d, L05c | no gRPC store or vendor service (Project 3) |
| [lab-08-scheduling](lab-08-scheduling/README.md) | Affinity scheduling: taskset, perf sched, chrt, cgroups, and a policy simulator | L04e | - |
| [lab-09-scalable-structures](lab-09-scalable-structures/README.md) | Scalable kernel-style structures: per-CPU counters, read-mostly data, and false sharing | L04f | - |
| [lab-10-clocks-and-mutex](lab-10-clocks-and-mutex/README.md) | Lamport and vector clocks, total order, and Lamport mutual exclusion | L05a, L05b | - |
| [lab-11-network-latency](lab-11-network-latency/README.md) | Network latency budgets: ping, iperf3, tc netem, and capsule routing | L05d | - |
| [lab-12-components](lab-12-components/README.md) | Micro-protocol stacks from components and common-path optimization | L05e | - |
| [lab-13-distributed-objects](lab-13-distributed-objects/README.md) | Distributed objects: Java RMI, subcontract-style invocation, and a generic gRPC call | L06a, L06b, L06c | generic greeter only; no gRPC store or vendor service (Project 3) |
| [lab-14-global-memory](lab-14-global-memory/README.md) | Global memory: cluster LRU with epochs and remote paging costs | L07a | - |
| [lab-15-dsm](lab-15-dsm/README.md) | User-level DSM with mprotect and SIGSEGV, twins and diffs | L07b | - |
| [lab-16-dfs](lab-16-dfs/README.md) | Distributed file system ideas: RAID parity, log-structured writes, and a FUSE cache | L07c | - |
| [lab-17-recoverable-memory](lab-17-recoverable-memory/README.md) | Recoverable virtual memory: undo and redo logs, fsync, crash injection | L08a, L08b | - |
| [lab-18-transactions](lab-18-transactions/README.md) | Transactions and recovery: two-phase commit, write-ahead logging, and shadow paging | L08c | - |
| [lab-19-giant-scale](lab-19-giant-scale/README.md) | Giant-scale services: DQ, harvest and yield, replication versus partitioning | L09a | - |
| [lab-20-mapreduce](lab-20-mapreduce/README.md) | MapReduce as a model: Unix pipelines and a master scheduling simulator | L09b | no MapReduce framework with RPC workers, master, or file sharding (Project 4) |
| [lab-21-dht](lab-21-dht/README.md) | DHTs: consistent hashing, key-based routing, Coral sloppy DHT, Dynamo quorums | L09c | - |
| [lab-22-realtime](lab-22-realtime/README.md) | Timeliness on Linux: cyclictest, SCHED_FIFO and SCHED_DEADLINE, timers | L10a | - |
| [lab-23-temporal-streams](lab-23-temporal-streams/README.md) | A time-indexed stream store and clock synchronization | L10b | - |
| [lab-24-security](lab-24-security/README.md) | Protection in practice: capabilities, namespaces, seccomp, and an Andrew-style handshake | L11a, L11b | - |
