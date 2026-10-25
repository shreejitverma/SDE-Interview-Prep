---
type: paper
track: [sde, distinguished, low-latency, quant-dev]
level:
status: seed
last_reviewed:
sources: []
---

# Paper: BOLT: A Practical Binary Optimizer for Data Centers (Panchenko et al. 2019)

> [!summary] Scope
> Owns the CGO 2019 systems paper by Maksim Panchenko et al. introducing post-link binary optimization for data centers.
> Details binary disassembly, Control Flow Graph reconstruction, LBR profile attribution, basic block reordering, and cold function splitting to eliminate instruction-cache and ITLB misses.
> Explains how BOLT extracts significant speedups on top of existing PGO and LTO compilations.

## Related

- Prerequisites: [[Paper-ThinLTO-Scalable-Incremental-LTO]], [[Paper-AutoFDO-Warehouse-Scale-Optimization]]
- Next: [[Paper-in-toto-Supply-Chain-Guarantees]]
