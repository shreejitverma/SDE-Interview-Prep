---
type: playbook
track: [sde, quant-dev, quant-research, low-latency, ai-eng, distinguished]
level:
status: draft
last_reviewed:
sources: []
---

# 📋 Quant Dev Study Plan — 10-Week Sprint

---

## Week 1-2: C++ Mastery Sprint
- [ ] Move semantics, perfect forwarding, universal references
- [ ] Smart pointers (unique_ptr, shared_ptr, weak_ptr) — ownership semantics
- [ ] Templates: SFINAE, CRTP, variadic templates, constexpr
- [ ] Memory layout: struct padding, cache lines, false sharing
- [ ] std::atomic, memory_order, happens-before relationships
- [ ] **Daily:** Implement 2 low-level C++ components from scratch
- [ ] **Resource:** [[02-Programming-Languages/C++]], [[14-Low-Latency-Systems/08 - Low-Latency Programming]]

## Week 3-4: Data Structures (C++ Implementation)
- [ ] Implement: lock-free queue, SPSC ring buffer
- [ ] Implement: order book (price-time priority) from scratch
- [ ] Implement: memory pool allocator, arena allocator
- [ ] Hash maps: open addressing, Robin Hood hashing
- [ ] Trees: B-tree, red-black tree internals (don't implement, understand)
- [ ] **Daily:** 2-3 LeetCode in C++ (focus on optimal solutions + clean code)

## Week 5-6: Trading Systems & Market Microstructure
- [ ] Order types: limit, market, IOC, FOK, stop, iceberg
- [ ] Market microstructure: bid-ask spread, order flow, market impact
- [ ] Matching engine architecture: price-time priority, pro-rata
- [ ] FIX protocol basics, SBE encoding
- [ ] Market data: L1 vs L2 vs L3, multicast feeds, sequencing
- [ ] **Resource:** [[14-Low-Latency-Systems/01 - Market & Microstructure Fundamentals]], [[14-Low-Latency-Systems/03 - Matching Engine Internals]]

## Week 7-8: Probability, Statistics & Brain Teasers
- [ ] Probability: Bayes' theorem, conditional probability, expectation, variance
- [ ] Distributions: Binomial, Poisson, Normal, Exponential
- [ ] Brain teasers: coin flips, dice games, card problems, Markov chains
- [ ] Mental math: quick multiplication, estimation, Fermi problems
- [ ] Statistics: hypothesis testing, confidence intervals, regression basics
- [ ] **Resource:** [[05-Quantitative-Finance/01-Mathematics]]

## Week 9: System Design for Trading
- [ ] Design: real-time market data aggregation system
- [ ] Design: order management system (OMS)
- [ ] Design: risk management engine
- [ ] Design: backtesting framework
- [ ] Network architecture: kernel bypass, DPDK, TCP vs UDP trade-offs

## Week 10: Mock Interviews & Company-Specific Prep
- [ ] 2 full mock interviews (C++ coding + system design + probability)
- [ ] Company-specific research for top 3 targets
- [ ] Review all retrospectives
- [ ] Optiver: practice mental math speed tests
- [ ] Jane Street: practice OCaml basics if applicable
- [ ] **Light maintenance:** 1-2 C++ problems/day

---

## 📊 Progress Tracking

| Week | C++ Practice | Problems | Math/Prob | System Design | Mocks |
|------|-------------|----------|-----------|---------------|-------|
| W1 | 10 components | 10 | 0 | 0 | 0 |
| W2 | 10 components | 10 | 0 | 0 | 0 |
| W3 | 5 impls | 15 | 0 | 0 | 0 |
| W4 | 5 impls | 15 | 0 | 0 | 0 |
| W5 | 2 systems | 10 | 5 | 0 | 0 |
| W6 | 2 systems | 10 | 5 | 0 | 0 |
| W7 | maintain | 5 | 15 | 0 | 0 |
| W8 | maintain | 5 | 15 | 0 | 0 |
| W9 | maintain | 5 | 5 | 4 | 0 |
| W10 | maintain | 5 | 5 | 1 | 2 |
