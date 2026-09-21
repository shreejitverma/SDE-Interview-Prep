---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Low Latency Study Plan - 12-Week Deep Dive

> This aligns with your existing [[14-Low-Latency-Systems/Roadmap - 12-Week Production Calibration|12-Week Production Calibration Roadmap]].

## Week 1-2: Hardware Sympathy & Memory Architecture
- [ ] CPU pipeline: fetch, decode, execute, retire. Branch prediction.
- [ ] Cache hierarchy: L1/L2/L3, cache lines (64B), associativity, eviction policies
- [ ] False sharing: why it happens, how to diagnose (perf stat), how to fix (__cacheline_aligned)
- [ ] NUMA: topology, local vs remote memory access, numactl
- [ ] Memory ordering: x86 TSO, ARM weak ordering, memory barriers
- [ ] **Resource:** [[14-Low-Latency-Systems/04 - Hardware Mechanical Sympathy]]

## Week 3-4: C++ for Low Latency
- [ ] std::atomic: all memory orders, compare_exchange_weak vs strong
- [ ] Lock-free SPSC queue (Lamport style)
- [ ] Lock-free MPSC queue
- [ ] Seqlock implementation
- [ ] Memory pools: arena allocator, pool allocator, slab allocator
- [ ] Hot-path optimization: no allocations, no syscalls, no branches, no vtables
- [ ] **Resource:** [[14-Low-Latency-Systems/08 - Low-Latency Programming]], [[14-Low-Latency-Systems/Interview/Coding-Problems-Low-Latency-CPP-Mastery]]

## Week 5-6: OS & Kernel Tuning
- [ ] CPU pinning (isolcpus, taskset, pthread_setaffinity)
- [ ] SCHED_FIFO, RT priorities, avoiding scheduler jitter
- [ ] Huge pages (2MB, 1GB), THP vs explicit huge pages
- [ ] IRQ affinity, softirq management
- [ ] /proc/sys/net tuning: buffer sizes, backlog, timestamping
- [ ] **Resource:** [[14-Low-Latency-Systems/05 - OS & Kernel Tuning]]

## Week 7-8: Networking
- [ ] TCP internals: 3-way handshake, Nagle's algorithm (disable!), TCP_NODELAY
- [ ] UDP: when and why, multicast group management
- [ ] Kernel bypass: DPDK, AF_XDP, Solarflare OpenOnload
- [ ] NIC offloading: RSS, RFS, GRO/GSO
- [ ] Timestamping: hardware vs software, PTP/PPS
- [ ] **Resource:** [[14-Low-Latency-Systems/06 - Networking]]

## Week 9-10: Trading Systems Architecture
- [ ] Market data feed handler architecture
- [ ] Order entry: lifecycle, FIX/SBE/ITCH
- [ ] Matching engine internals (review, not build)
- [ ] System-level design: end-to-end order flow architecture
- [ ] **Resource:** [[14-Low-Latency-Systems/Interview/Staff-Principal-System-Design-Blueprint]]

## Week 11-12: Mock Interviews & Synthesis
- [ ] 3 full mock interviews
- [ ] Review all retrospectives, identify patterns
- [ ] Company-specific: Optiver (mental math), Jump (deep systems), HRT (C++ coding)
- [ ] Revisit weak areas from mocks
- [ ] **Resource:** [[14-Low-Latency-Systems/Interview/question-bank-answers]]
