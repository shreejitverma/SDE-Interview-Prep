---
tags: [trading/kernel-tuning, type/moc]
aliases: [Kernel Tuning MOC, Linux Low Latency MOC]
status: seed
module: 05
created: 2026-08-22
---

# MOC — 05 OS & Kernel Tuning

Eliminating OS-induced jitter: CPU core isolation, tickless kernels, interrupt steering, memory locking, and power management.

---

## Core Concepts
- [[Notes/Kernel Boot Parameters for Core Isolation]] — `isolcpus`, `nohz_full`, `rcu_nocbs`, `rcu_nocb_poll`, `irqaffinity`.
- [[Notes/Linux Thread Pinning and Core Affinity]] — `pthread_setaffinity_np`, NUMA-aware core binding, cache domain pinning.
- [[Notes/Interrupt Routing and MSI-X Tuning]] — Hardware IRQ steering, SMP affinity masks, RPS/RFS overhead, pinning NIC queues.
- [[Notes/Memory Locking and Zero Page Faults]] — `mlockall(MCL_CURRENT | MCL_FUTURE)`, pre-faulting stacks, disabling swap (`vm.swappiness`).
- [[Notes/CPU Power States and Jitter Sources]] — C-states (sleep), P-states (frequency scaling), disabling deep C-states (`intel_idle.max_cstate=0`).
- [[Notes/Real-Time Scheduling Policies]] — `SCHED_FIFO`, `SCHED_RR`, priority inversion, context switch overhead vs CPU pinning.
- [[Notes/Transparent Huge Pages vs Explicit HugeTLBFS]] — Defrag daemon jitter (`khugepaged`), explicit pool pre-allocation.

## Labs & Implementations
- [[Labs/Lab - 05 Production Core Isolation and Jitter Measurement]] — Configure an isolated Linux core and measure latency jitter distribution using `cyclictest` and custom rdtsc loops.

## Drills & War Stories
- [[Drills/Drill - 05 Diagnosing Production OS Jitter]] — Socratic debugging drill: identifying root causes for 15µs periodic latency spikes.
- [[Notes/War Story - The 10ms Jitter Spike from khugepaged]] — How background memory compaction halted critical market-making threads.

## Canonical Sources
- [[Sources/Systems Performance by Brendan Gregg]] — Comprehensive reference for Linux performance and kernel tracing.
- [[Sources/Red Hat Enterprise Linux for Real Time Tuning Guide]] — Production tuning guidelines for low-latency kernel deployments.
