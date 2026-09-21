---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# Low Latency Question Bank

> Also see: [[14-Low-Latency-Systems/Interview/question-bank-answers|Full LL Question Bank (290KB)]]

## C++ Systems Coding
| # | Question | Difficulty | Status |
|---|---------|-----------|--------|
| 1 | Implement a lock-free SPSC ring buffer | Hard | ☐ |
| 2 | Implement a seqlock | Hard | ☐ |
| 3 | Implement an arena/pool allocator with O(1) alloc/free | Hard | ☐ |
| 4 | Explain all 6 memory orders. When would you use each? | Hard | ☐ |
| 5 | What is false sharing? How do you detect and fix it? | Medium | ☐ |
| 6 | Implement a cache-friendly matrix transpose | Medium | ☐ |
| 7 | compare_exchange_weak vs strong - when to use which? | Medium | ☐ |
| 8 | Why is `volatile` not sufficient for concurrency? | Medium | ☐ |
| 9 | Implement a wait-free bounded MPMC queue | Insane | ☐ |
| 10 | Hot-path audit: given this code, eliminate all latency sources | Hard | ☐ |

## Systems Architecture
| # | Question | Difficulty | Status |
|---|---------|-----------|--------|
| 1 | Design a market data feed handler (tick-to-trade < 1μs) | Hard | ☐ |
| 2 | How would you design a co-located trading system? | Hard | ☐ |
| 3 | Explain kernel bypass. Compare DPDK vs AF_XDP vs OpenOnload | Hard | ☐ |
| 4 | How do you achieve sub-microsecond latency end-to-end? | Hard | ☐ |
| 5 | Design a risk check engine that runs in the order hot path | Hard | ☐ |

## Hardware & OS
| # | Question | Difficulty | Status |
|---|---------|-----------|--------|
| 1 | Explain the CPU cache hierarchy. What happens on a cache miss? | Medium | ☐ |
| 2 | What is branch prediction? How do you help the predictor? | Medium | ☐ |
| 3 | Explain NUMA. Why does it matter for trading systems? | Medium | ☐ |
| 4 | How do you tune Linux for low-latency workloads? | Hard | ☐ |
| 5 | What is the cost of a syscall? Why avoid them on hot path? | Medium | ☐ |

## Networking
| # | Question | Difficulty | Status |
|---|---------|-----------|--------|
| 1 | Why disable Nagle's algorithm? What about TCP_QUICKACK? | Medium | ☐ |
| 2 | Explain multicast. How do exchanges distribute market data? | Medium | ☐ |
| 3 | What is DPDK? Walk through the packet processing pipeline | Hard | ☐ |
| 4 | How does hardware timestamping work? PTP vs NTP? | Medium | ☐ |
| 5 | Compare busy-polling vs epoll for low-latency networking | Medium | ☐ |
