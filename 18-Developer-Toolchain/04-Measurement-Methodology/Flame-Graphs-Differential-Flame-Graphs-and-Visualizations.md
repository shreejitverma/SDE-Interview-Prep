---
type: concept
track: [sde, distinguished]
level:
status: seed
last_reviewed:
sources: []
---

# Flame Graphs, Differential Flame Graphs, and Visualizations

> [!summary] Scope
> This note owns profile visualization formats, specifically Brendan Gregg's Flame Graphs, timeline flame charts, and red/blue differential flame graphs.
> It covers the Use, Explain, and Debug rungs, explaining folded stack generation, hierarchical call tree aggregation, and common visualization pitfalls such as recursion distortion and missing frame pointers.
> It establishes the visual inspection standard used across Pillar 05 profilers.

## Related

- Prerequisites: [[Sampling-vs-Instrumentation-Skid-and-Safepoint-Bias]]
- Next: [[Performance-Regression-Detection-in-CI]]
- See also: [[15-Technical-Whitepapers/01-Systems-Performance-and-Tracing/Brendan-Gregg-Performance-Canon|Brendan Gregg Performance Canon]]
