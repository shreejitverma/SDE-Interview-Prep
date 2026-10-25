---
type: problem
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Drill: Environment Size and Code Alignment Bias

> [!summary] Scope
> This problem note presents a hands-on drill reproducing a 15% execution swing caused solely by environment variable string sizes shifting stack alignment.
> It challenges the engineer to measure L1 instruction cache misses and branch target buffer collisions using PMU counters, diagnose the artifact, and stabilize the harness.
> It validates the foundational lessons of Mytkowicz et al.

## Related

- Prerequisites: [[Benchmark-Methodology-Noise-and-Environment-Control]], [[Hardware-Performance-Counters-Top-Down-TMA-and-Roofline]]
- Next: [[Drill-Measurement-Methodology-Coordinated-Omission-Trap]]
- Lab: `code/04-Measurement-Methodology/drill_env_size_bias/` (requires: linux, pmu)
