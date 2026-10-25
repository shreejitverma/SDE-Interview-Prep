---
type: playbook
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Design a Low-Noise Benchmark Harness Playbook

> [!summary] Scope
> This playbook provides a concrete operational runbook for configuring a host system and authoring a repeatable, low-noise benchmarking suite.
> It guides engineers through CPU core isolation with isolcpus, frequency governor locking, ASLR disabling, interleaved A/B sample execution, and Mann-Whitney U validation.
> It delivers a verifiable harness that reports its own run-to-run variance and aims to keep it under 1% on a well-tuned host.

## Related

- Prerequisites: [[Benchmark-Methodology-Noise-and-Environment-Control]], [[Statistics-for-Benchmarks-Coordinated-Omission-and-HDR-Histograms]]
- Next: [[Drill-Measurement-Methodology-Environment-Size-Bias]]
- See also: [[02-Programming-Languages/C++/CPP_Zero_to_Godhood/Volume_08_Advanced_Systems/Chapter_103_Microbenchmarking_and_Tail_Latency/Chapter_103_Microbenchmarking_and_Tail_Latency|Chapter_103_Microbenchmarking_and_Tail_Latency]]
- Lab: `code/04-Measurement-Methodology/low_noise_harness/` (requires: linux)
