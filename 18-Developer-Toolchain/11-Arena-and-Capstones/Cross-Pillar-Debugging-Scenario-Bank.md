---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Cross-Pillar Debugging Scenario Bank

> [!summary] Scope
> This scenario bank contains ten multi-system failure scenarios that cross process execution, dynamic linking, compiler optimization, concurrency, memory corruption, and container virtualization boundaries.
> Each scenario presents production symptoms, diagnostic command sequences, folded hint ladders, and complete root-cause analyses evaluated at L6, L7, and L8+ depth.
> It bridges debugging tools (GDB, LLDB, ptrace, sanitizers, core dumps) with runtime environments (Linux, macOS, container runtimes).

## Related

- Prerequisites: [[Debug-a-Production-Crash-From-a-Core-Dump]], [[Scientific-Debugging-Delta-Debugging-and-Heisenbugs]], [[Linux-perf-in-Depth-and-perf-c2c]]
- Next: [[Cross-Pillar-Design-Scenario-Bank]]
- See also: [[02-Programming-Languages/Python/Python-Backend-Interview-Prep/10-Testing-Debugging-Production|10 Testing Debugging Production]], [[14-Low-Latency-Systems/13 - Reliability, Ops & Testing/Observability Without Perturbation|Observability Without Perturbation]]
- Lab: `code/11-Arena-and-Capstones/cross_pillar_debug_scenarios/` (requires: linux, docker)
