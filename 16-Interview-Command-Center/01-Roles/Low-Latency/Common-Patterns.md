# 🔄 Low Latency Common Patterns

## The Low Latency Commandments
1. **Zero allocations on hot path** — Pre-allocate everything. Use pools.
2. **Zero syscalls on hot path** — No `malloc`, no `write`, no `read`. Kernel bypass.
3. **Zero branches on hot path** — Branch-free code. Branchless min/max. Lookup tables.
4. **Zero virtual dispatch** — CRTP, templates, `final` keyword.
5. **Zero contention** — SPSC queues, per-thread data, no shared mutable state.
6. **Cache is king** — Data locality. SOA vs AOS. Prefetching.
7. **Measure everything** — rdtsc, hardware counters, flame graphs.

## Architecture: Tick-to-Trade Pipeline
```
NIC (kernel bypass) → Parser → Normalizer → Strategy → Risk → OMS → NIC
                                                                      ↑
All on a single pinned core, sequential, no locks, no allocations
```

## Resources
| Topic | Link |
|-------|------|
| Full LL Knowledge Base | [[14-Low-Latency-Systems/00 Home]] |
| Interview Deep Dive | [[14-Low-Latency-Systems/Interview/interview]] |
| C++ Cheatsheet | [[14-Low-Latency-Systems/Interview/Core-CPP-Low-Latency-Interview-Cheatsheet]] |
| System Design Blueprint | [[14-Low-Latency-Systems/Interview/Staff-Principal-System-Design-Blueprint]] |
| Thesis Deep Dive | [[14-Low-Latency-Systems/Interview/thesis-deep-dive]] |
| Technical Whitepapers | [[15-Technical-Whitepapers/10-Seminal-Low-Latency-Systems-Papers]] |
